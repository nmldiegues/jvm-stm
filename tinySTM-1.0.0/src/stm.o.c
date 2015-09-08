/*
 * File:
 *   stm.c
 * Author(s):
 *   Pascal Felber <pascal.felber@unine.ch>
 * Description:
 *   STM functions.
 *
 * Copyright (c) 2007-2009.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <sched.h>

#include "stm.h"

#include "atomic.h"
#include "gc.h"

/* ################################################################### *
 * DEFINES
 * ################################################################### */

#define COMPILE_TIME_ASSERT(pred)       switch (0) { case 0: case pred: ; }
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
# define likely(x)                      __builtin_expect(!!(x), 1)
# define unlikely(x)                    __builtin_expect(!!(x), 0)
#endif /* defined(__GNUC__) || defined(__INTEL_COMPILER) */

/* Designs */
#define WRITE_BACK_ETL                  0
#define WRITE_BACK_CTL                  1
#define WRITE_THROUGH                   2

static const char *design_names[] = {
  /* 0 */ "WRITE-BACK (ETL)",
  /* 1 */ "WRITE-BACK (CTL)",
  /* 2 */ "WRITE-THROUGH"
};


/* Contention managers */
#define CM_SUICIDE                      0
#define CM_DELAY                        1
#define CM_BACKOFF                      2
#define CM_MODULAR                      3

static const char *cm_names[] = {
  /* 0 */ "SUICIDE",
  /* 1 */ "DELAY",
  /* 2 */ "BACKOFF",
  /* 3 */ "MODULAR"
};






#ifdef EXPLICIT_TX_PARAMETER
# define TX_RETURN                      return tx
# define TX_GET                         /* Nothing */
#else /* ! EXPLICIT_TX_PARAMETER */
# define TX_RETURN                      /* Nothing */
# define TX_GET                         stm_tx_t *tx = stm_get_tx()
#endif /* ! EXPLICIT_TX_PARAMETER */


/* Note: stdio is thread-safe */
# define IO_FLUSH                       fflush(NULL)
# define PRINT_DEBUG(...)               printf(__VA_ARGS__); fflush(NULL)

# define PRINT_DEBUG2(...)              PRINT_DEBUG(__VA_ARGS__)

#ifndef RW_SET_SIZE
# define RW_SET_SIZE                    4096                /* Initial size of read/write sets */
#endif /* ! RW_SET_SIZE */

#ifndef LOCK_ARRAY_LOG_SIZE
# define LOCK_ARRAY_LOG_SIZE            20                  /* Size of lock array: 2^20 = 1M */
#endif /* LOCK_ARRAY_LOG_SIZE */

#ifndef LOCK_SHIFT_EXTRA
# define LOCK_SHIFT_EXTRA               2                   /* 2 extra shift */
#endif /* LOCK_SHIFT_EXTRA */



#define NO_SIGNAL_HANDLER               "NO_SIGNAL_HANDLER"

#define XSTR(s)                         STR(s)
#define STR(s)                          #s

/* ################################################################### *
 * TYPES
 * ################################################################### */

enum {                                  /* Transaction status */
  TX_IDLE = 0,
  TX_ACTIVE = 1,                        /* Lowest bit indicates activity */
  TX_COMMITTED = (1 << 1),
  TX_ABORTED = (2 << 1),
  TX_COMMITTING = (1 << 1) | TX_ACTIVE,
  TX_ABORTING = (2 << 1) | TX_ACTIVE,
  TX_KILLED = (3 << 1) | TX_ACTIVE,
  TX_IRREVOCABLE = 0x08 | TX_ACTIVE     /* Fourth bit indicates irrevocability */
};
#define STATUS_BITS                     4
#define STATUS_MASK                     ((1 << STATUS_BITS) - 1)

# define SET_STATUS(s, v)               ((s) = (v))
# define UPDATE_STATUS(s, v)            ((s) = (v))
# define GET_STATUS(s)                  ((s))
#define IS_ACTIVE(s)                    ((GET_STATUS(s) & 0x01) == TX_ACTIVE) 

typedef struct r_entry {                /* Read set entry */
  stm_word_t version;                   /* Version read */
  volatile stm_word_t *lock;            /* Pointer to lock (for fast access) */
} r_entry_t;

typedef struct r_set {                  /* Read set */
  r_entry_t *entries;                   /* Array of entries */
  int nb_entries;                       /* Number of entries */
  int size;                             /* Size of array */
} r_set_t;

typedef struct w_entry {                /* Write set entry */
  union {                               /* For padding... */
    struct {
      volatile stm_word_t *addr;        /* Address written */
      stm_word_t value;                 /* New (write-back) or old (write-through) value */
      stm_word_t mask;                  /* Write mask */
      stm_word_t version;               /* Version overwritten */
      volatile stm_word_t *lock;        /* Pointer to lock (for fast access) */
      struct w_entry *next;             /* Next address covered by same lock (if any) */
    };
    stm_word_t padding[8];              /* Padding (multiple of a cache line) */
  };
} w_entry_t;

typedef struct w_set {                  /* Write set */
  w_entry_t *entries;                   /* Array of entries */
  int nb_entries;                       /* Number of entries */
  int size;                             /* Size of array */
  int has_writes;                       /* Has the write set any real write (vs. visible reads) */
} w_set_t;

typedef struct cb_entry {               /* Callback entry */
  void (*f)(TXPARAMS void *);           /* Function */
  void *arg;                            /* Argument to be passed to function */
} cb_entry_t;

#ifndef MAX_SPECIFIC
# define MAX_SPECIFIC                   16
#endif /* MAX_SPECIFIC */

typedef struct stm_tx {                 /* Transaction descriptor */
  sigjmp_buf env;                       /* Environment for setjmp/longjmp (must be first field!) */
  stm_tx_attr_t *attr;                  /* Transaction attributes (user-specified) */
  volatile stm_word_t status;           /* Transaction status */
  stm_word_t start;                     /* Start timestamp */
  stm_word_t end;                       /* End timestamp (validity range) */
  r_set_t r_set;                        /* Read set */
  w_set_t w_set;                        /* Write set */
  unsigned int ro:1;                    /* Is this execution read-only? */
  unsigned int can_extend:1;            /* Can this transaction be extended? */
  int nesting;                          /* Nesting level */
  void *data[MAX_SPECIFIC];             /* Transaction-specific data (fixed-size array for better speed) */
  struct stm_tx *next;                  /* For keeping track of all transactional threads */
  volatile stm_word_t *c_lock;          /* Pointer to contented lock (cause of abort) */
  unsigned long retries;                /* Number of consecutive aborts (retries) */
  unsigned long aborts;                 /* Total number of aborts (cumulative) */
  unsigned long aborts_1;               /* Total number of transactions that abort once or more (cumulative) */
  unsigned long aborts_2;               /* Total number of transactions that abort twice or more (cumulative) */
  unsigned long aborts_ro;              /* Aborts due to wrong read-only specification (cumulative) */
  unsigned long aborts_locked_read;     /* Aborts due to trying to read when locked (cumulative) */
  unsigned long aborts_locked_write;    /* Aborts due to trying to write when locked (cumulative) */
  unsigned long aborts_validate_read;   /* Aborts due to failed validation upon read (cumulative) */
  unsigned long aborts_validate_write;  /* Aborts due to failed validation upon write (cumulative) */
  unsigned long aborts_validate_commit; /* Aborts due to failed validation upon commit (cumulative) */
  unsigned long aborts_invalid_memory;  /* Aborts due to invalid memory access (cumulative) */
  unsigned long max_retries;            /* Maximum number of consecutive aborts (retries) */
} stm_tx_t;

static int nb_specific = 0;             /* Number of specific slots used (<= MAX_SPECIFIC) */

static int initialized = 0;             /* Has the library been initialized? */

pthread_mutex_t quiesce_mutex;          /* Mutex to support quiescence */
pthread_cond_t quiesce_cond;            /* Condition variable to support quiescence */
volatile stm_word_t quiesce;            /* Prevent threads from entering transactions upon quiescence */
volatile stm_word_t threads_count;      /* Number of active threads */
stm_tx_t *threads;                      /* Head of linked list of threads */


/*
 * Transaction nesting is supported in a minimalist way (flat nesting):
 * - When a transaction is started in the context of another
 *   transaction, we simply increment a nesting counter but do not
 *   actually start a new transaction.
 * - The environment to be used for setjmp/longjmp is only returned when
 *   no transaction is active so that it is not overwritten by nested
 *   transactions. This allows for composability as the caller does not
 *   need to know whether it executes inside another transaction.
 * - The commit of a nested transaction simply decrements the nesting
 *   counter. Only the commit of the top-level transaction will actually
 *   carry through updates to shared memory.
 * - An abort of a nested transaction will rollback the top-level
 *   transaction and reset the nesting counter. The call to longjmp will
 *   restart execution before the top-level transaction.
 * Using nested transactions without setjmp/longjmp is not recommended
 * as one would need to explicitly jump back outside of the top-level
 * transaction upon abort of a nested transaction. This breaks
 * composability.
 */

/*
 * Reading from the previous version of locked addresses is implemented
 * by peeking into the write set of the transaction that owns the
 * lock. Each transaction has a unique identifier, updated even upon
 * retry. A special "commit" bit of this identifier is set upon commit,
 * right before writing the values from the redo log to shared memory. A
 * transaction can read a locked address if the identifier of the owner
 * does not change between before and after reading the value and
 * version, and it does not have the commit bit set.
 */

/* ################################################################### *
 * CALLBACKS
 * ################################################################### */

#define MAX_CB                          16

/* Declare as static arrays (vs. lists) to improve cache locality */
static cb_entry_t init_cb[MAX_CB];      /* Init thread callbacks */
static cb_entry_t exit_cb[MAX_CB];      /* Exit thread callbacks */
static cb_entry_t start_cb[MAX_CB];     /* Start callbacks */
static cb_entry_t commit_cb[MAX_CB];    /* Commit callbacks */
static cb_entry_t abort_cb[MAX_CB];     /* Abort callbacks */

static int nb_init_cb = 0;
static int nb_exit_cb = 0;
static int nb_start_cb = 0;
static int nb_commit_cb = 0;
static int nb_abort_cb = 0;


/* ################################################################### *
 * THREAD-LOCAL
 * ################################################################### */

static __thread stm_tx_t* thread_tx = NULL;

/* ################################################################### *
 * LOCKS
 * ################################################################### */

/*
 * A lock is a unsigned int of the size of a pointer.
 * The LSB is the lock bit. If it is set, this means:
 * - At least some covered memory addresses is being written.
 * - Write-back (ETL): all bits of the lock apart from the lock bit form
 *   a pointer that points to the write log entry holding the new
 *   value. Multiple values covered by the same log entry and orginized
 *   in a linked list in the write log.
 * - Write-through and write-back (CTL): all bits of the lock apart from
 *   the lock bit form a pointer that points to the transaction
 *   descriptor containing the write-set.
 * If the lock bit is not set, then:
 * - All covered memory addresses contain consistent values.
 * - Write-back (ETL and CTL): all bits of the lock besides the lock bit
 *   contain a version number (timestamp).
 * - Write-through: all bits of the lock besides the lock bit contain a
 *   version number.
 *   - The high order bits contain the commit time.
 *   - The low order bits contain an incarnation number (incremented
 *     upon abort while writing the covered memory addresses).
 * When visible reads are enabled, two bits are used as read and write
 * locks. A read-locked address can be read by an invisible reader.
 */

# define OWNED_BITS                     1                   /* 1 bit */
# define WRITE_MASK                     0x01                /* 1 bit */
# define OWNED_MASK                     (WRITE_MASK)
# define LOCK_BITS                      (OWNED_BITS)
#define MAX_THREADS                     8192                /* Upper bound (large enough) */
#define VERSION_MAX                     ((~(stm_word_t)0 >> LOCK_BITS) - MAX_THREADS)

#define LOCK_GET_OWNED(l)               (l & OWNED_MASK)
#define LOCK_GET_WRITE(l)               (l & WRITE_MASK)
#define LOCK_SET_ADDR_WRITE(a)          (a | WRITE_MASK)    /* WRITE bit set */
#define LOCK_GET_ADDR(l)                (l & ~(stm_word_t)OWNED_MASK)
# define LOCK_GET_TIMESTAMP(l)          (l >> OWNED_BITS)   /* Logical shift (unsigned) */
# define LOCK_SET_TIMESTAMP(t)          (t << OWNED_BITS)   /* OWNED bits not set */
#define LOCK_UNIT                       (~(stm_word_t)0)

/*
 * We use the very same hash functions as TL2 for degenerate Bloom
 * filters on 32 bits.
 */
# define FILTER_HASH(a)                 (((stm_word_t)a >> 2) ^ ((stm_word_t)a >> 5))
# define FILTER_BITS(a)                 (1 << (FILTER_HASH(a) & 0x1F))

/*
 * We use an array of locks and hash the address to find the location of the lock.
 * We try to avoid collisions as much as possible (two addresses covered by the same lock).
 */
#define LOCK_ARRAY_SIZE                 (1 << LOCK_ARRAY_LOG_SIZE)
#define LOCK_MASK                       (LOCK_ARRAY_SIZE - 1)
#define LOCK_SHIFT                      (((sizeof(stm_word_t) == 4) ? 2 : 3) + LOCK_SHIFT_EXTRA)
#define LOCK_IDX(a)                     (((stm_word_t)(a) >> LOCK_SHIFT) & LOCK_MASK)
# define GET_LOCK(a)                    (locks + LOCK_IDX(a))

static volatile stm_word_t locks[LOCK_ARRAY_SIZE];

/* ################################################################### *
 * CLOCK
 * ################################################################### */

/* At least twice a cache line (512 bytes to be on the safe side) */
static volatile stm_word_t gclock[1024 / sizeof(stm_word_t)];
# define CLOCK                          (gclock[512 / sizeof(stm_word_t)])

#define GET_CLOCK                       (ATOMIC_LOAD_ACQ(&CLOCK))
#define FETCH_INC_CLOCK                 (ATOMIC_FETCH_INC_FULL(&CLOCK))

/* ################################################################### *
 * STATIC
 * ################################################################### */

/*
 * Returns the transaction descriptor for the CURRENT thread.
 */
static inline stm_tx_t *stm_get_tx()
{
  return thread_tx;
}



/*
 * Initialize quiescence support.
 */
static inline void stm_quiesce_init()
{
  PRINT_DEBUG("==> stm_quiesce_init()\n");

  if (pthread_mutex_init(&quiesce_mutex, NULL) != 0) {
    fprintf(stderr, "Error creating mutex\n");
    exit(1);
  }
  if (pthread_cond_init(&quiesce_cond, NULL) != 0) {
    fprintf(stderr, "Error creating condition variable\n");
    exit(1);
  }
  quiesce = 0;
  threads_count = 0;
  threads = NULL;
}

/*
 * Clean up quiescence support.
 */
static inline void stm_quiesce_exit()
{
  PRINT_DEBUG("==> stm_quiesce_exit()\n");

  pthread_cond_destroy(&quiesce_cond);
  pthread_mutex_destroy(&quiesce_mutex);
}

/*
 * Called by each thread upon initialization for quiescence support.
 */
static inline void stm_quiesce_enter_thread(stm_tx_t *tx)
{
  PRINT_DEBUG("==> stm_quiesce_enter_thread(%p)\n", tx);

  pthread_mutex_lock(&quiesce_mutex);
  /* Add new descriptor at head of list */
  tx->next = threads;
  threads = tx;
  threads_count++;
  pthread_mutex_unlock(&quiesce_mutex);
}

/*
 * Called by each thread upon exit for quiescence support.
 */
static inline void stm_quiesce_exit_thread(stm_tx_t *tx)
{
  stm_tx_t *t, *p;

  PRINT_DEBUG("==> stm_quiesce_exit_thread(%p)\n", tx);

  /* Can only be called if non-active */
  assert(!IS_ACTIVE(tx->status));

  pthread_mutex_lock(&quiesce_mutex);
  /* Remove descriptor from list */
  p = NULL;
  t = threads;
  while (t != tx) {
    assert(t != NULL);
    p = t;
    t = t->next;
  }
  if (p == NULL)
    threads = t->next;
  else
    p->next = t->next;
  threads_count--;
  if (quiesce) {
    /* Wake up someone in case other threads are waiting for us */
    pthread_cond_signal(&quiesce_cond);
  }
  pthread_mutex_unlock(&quiesce_mutex);
}

/*
 * Wait for all transactions to be block on a barrier.
 */
static inline void stm_quiesce_barrier(stm_tx_t *tx, void (*f)(void *), void *arg)
{
  PRINT_DEBUG("==> stm_quiesce_barrier()\n");

  /* Can only be called if non-active */
  assert(tx == NULL || !IS_ACTIVE(tx->status));

  pthread_mutex_lock(&quiesce_mutex);
  /* Wait for all other transactions to block on barrier */
  threads_count--;
  if (quiesce == 0) {
    /* We are first on the barrier */
    quiesce = 1;
  }
  while (quiesce) {
    if (threads_count == 0) {
      /* Everybody is blocked */
      if (f != NULL)
        f(arg);
      /* Release transactional threads */
      quiesce = 0;
      pthread_cond_broadcast(&quiesce_cond);
    } else {
      /* Wait for other transactions to stop */
      pthread_cond_wait(&quiesce_cond, &quiesce_mutex);
    }
  }
  threads_count++;
  pthread_mutex_unlock(&quiesce_mutex);
}

/*
 * Wait for all transactions to be be out of their current transaction.
 */
static inline int stm_quiesce(stm_tx_t *tx, int block)
{
  stm_tx_t *t;

  PRINT_DEBUG("==> stm_quiesce(%p)\n", tx);

  if (IS_ACTIVE(tx->status)) {
    /* Only one active transaction can quiesce at a time, others must abort */
    if (pthread_mutex_trylock(&quiesce_mutex) != 0)
      return 1;
  } else {
    /* We can safely block because we are inactive */
    pthread_mutex_lock(&quiesce_mutex);
  }
  /* We own the lock at this point */
  if (block)
    ATOMIC_STORE_REL(&quiesce, 2);
  /* Make sure we read latest status data */
  ATOMIC_MB_FULL;
  /* Not optimal as we check transaction sequentially and might miss some inactivity states */
  for (t = threads; t != NULL; t = t->next) {
    if (t == tx)
      continue;
    /* Wait for all other transactions to become inactive */
    while (IS_ACTIVE(t->status))
      ;
  }
  if (!block)
    pthread_mutex_unlock(&quiesce_mutex);
  return 0;
}

/*
 * Check if transaction must block.
 */
static inline int stm_check_quiesce(stm_tx_t *tx)
{
  stm_word_t s;

  /* Must be called upon start (while already active but before acquiring any lock) */
  assert(IS_ACTIVE(tx->status));

  ATOMIC_MB_FULL;
  if (ATOMIC_LOAD_ACQ(&quiesce) == 2) {
    s = ATOMIC_LOAD(&tx->status);
    SET_STATUS(tx->status, TX_IDLE);
    while (ATOMIC_LOAD_ACQ(&quiesce) == 2) {
      sched_yield();
    }
    SET_STATUS(tx->status, GET_STATUS(s));
    return 1;
  }
  return 0;
}

/*
 * Release threads blocked after quiescence.
 */
static inline void stm_quiesce_release(stm_tx_t *tx)
{
  ATOMIC_STORE_REL(&quiesce, 0);
  pthread_mutex_unlock(&quiesce_mutex);
}

/*
 * Reset clock and timestamps
 */
static inline void rollover_clock(void *arg)
{
  PRINT_DEBUG("==> rollover_clock()\n");

  /* Reset clock */
  CLOCK = 0;
  /* Reset timestamps */
  memset((void *)locks, 0, LOCK_ARRAY_SIZE * sizeof(stm_word_t));
}

/*
 * Check if stripe has been read previously.
 */
static inline r_entry_t *stm_has_read(stm_tx_t *tx, volatile stm_word_t *lock)
{
  r_entry_t *r;
  int i;

  PRINT_DEBUG("==> stm_has_read(%p[%lu-%lu],%p)\n", tx, (unsigned long)tx->start, (unsigned long)tx->end, lock);

  /* Look for read */
  r = tx->r_set.entries;
  for (i = tx->r_set.nb_entries; i > 0; i--, r++) {
    if (r->lock == lock) {
      /* Return first match*/
      return r;
    }
  }
  return NULL;
}


/*
 * (Re)allocate read set entries.
 */
static inline void stm_allocate_rs_entries(stm_tx_t *tx, int extend)
{
  PRINT_DEBUG("==> stm_allocate_rs_entries(%p[%lu-%lu],%d)\n", tx, (unsigned long)tx->start, (unsigned long)tx->end, extend);

  if (extend) {
    /* Extend read set */
    tx->r_set.size *= 2;
    if ((tx->r_set.entries = (r_entry_t *)realloc(tx->r_set.entries, tx->r_set.size * sizeof(r_entry_t))) == NULL) {
      perror("realloc read set");
      exit(1);
    }
  } else {
    /* Allocate read set */
    if ((tx->r_set.entries = (r_entry_t *)malloc(tx->r_set.size * sizeof(r_entry_t))) == NULL) {
      perror("malloc read set");
      exit(1);
    }
  }
}

/*
 * (Re)allocate write set entries.
 */
static inline void stm_allocate_ws_entries(stm_tx_t *tx, int extend)
{

  PRINT_DEBUG("==> stm_allocate_ws_entries(%p[%lu-%lu],%d)\n", tx, (unsigned long)tx->start, (unsigned long)tx->end, extend);

  if (extend) {
    /* Extend write set */
    int j;
    w_entry_t *ows, *nws;
    /* Allocate new write set */
    ows = tx->w_set.entries;
    if ((nws = (w_entry_t *)malloc(tx->w_set.size * 2 * sizeof(w_entry_t))) == NULL) {
      perror("malloc write set");
      exit(1);
    }
    /* Copy write set */
    memcpy(nws, ows, tx->w_set.size * sizeof(w_entry_t));
    /* Update pointers and locks */
    for (j = 0; j < tx->w_set.nb_entries; j++) {
      if (ows[j].next != NULL)
        nws[j].next = nws + (ows[j].next - ows);
    }
    for (j = 0; j < tx->w_set.nb_entries; j++) {
      if (ows[j].lock == GET_LOCK(ows[j].addr)) 
        ATOMIC_STORE_REL(ows[j].lock, LOCK_SET_ADDR_WRITE((stm_word_t)&nws[j]));
    }
    tx->w_set.entries = nws;
    tx->w_set.size *= 2;
    free(ows);
  } else {
    /* Allocate write set */
    if ((tx->w_set.entries = (w_entry_t *)malloc(tx->w_set.size * sizeof(w_entry_t))) == NULL) {
      perror("malloc write set");
      exit(1);
    }
  }

}

/*
 * Validate read set (check if all read addresses are still valid now).
 */
static inline int stm_validate(stm_tx_t *tx)
{
  r_entry_t *r;
  int i;
  stm_word_t l;

  PRINT_DEBUG("==> stm_validate(%p[%lu-%lu])\n", tx, (unsigned long)tx->start, (unsigned long)tx->end);

  /* Validate reads */
  r = tx->r_set.entries;
  for (i = tx->r_set.nb_entries; i > 0; i--, r++) {
    /* Read lock */
    l = ATOMIC_LOAD(r->lock);
    /* Unlocked and still the same version? */
    if (LOCK_GET_OWNED(l)) {
      /* Do we own the lock? */
      w_entry_t *w = (w_entry_t *)LOCK_GET_ADDR(l);
      /* Simply check if address falls inside our write set (avoids non-faulting load) */
      if (!(tx->w_set.entries <= w && w < tx->w_set.entries + tx->w_set.nb_entries))
      {
        /* Locked by another transaction: cannot validate */
        return 0;
      }
      /* We own the lock: OK */
    } else {
      if (LOCK_GET_TIMESTAMP(l) != r->version) {
        /* Other version: cannot validate */
        return 0;
      }
      /* Same version: OK */
    }
  }
  return 1;
}

/*
 * Extend snapshot range.
 */
static inline int stm_extend(stm_tx_t *tx)
{
  stm_word_t now;

  PRINT_DEBUG("==> stm_extend(%p[%lu-%lu])\n", tx, (unsigned long)tx->start, (unsigned long)tx->end);

  /* Get current time */
  now = GET_CLOCK;
  if (now >= VERSION_MAX) {
    /* Clock overflow */
    return 0;
  }
  /* Try to validate read set */
  if (stm_validate(tx)) {
    /* It works: we can extend until now */
    tx->end = now;
    return 1;
  }
  return 0;
}


/*
 * Initialize the transaction descriptor before start or restart.
 */
static inline void stm_prepare(stm_tx_t *tx)
{
 start:
  /* Start timestamp */
  tx->start = tx->end = GET_CLOCK; /* OPT: Could be delayed until first read/write */

  /* Allow extensions */
  tx->can_extend = 1;
  if (tx->start >= VERSION_MAX) {
    /* Block all transactions and reset clock */
    stm_quiesce_barrier(tx, rollover_clock, NULL);
    goto start;
  }
  /* Read/write set */
  tx->w_set.has_writes = 0;
  tx->w_set.nb_entries = 0;
  tx->r_set.nb_entries = 0;


  /* Set status */
  UPDATE_STATUS(tx->status, TX_ACTIVE);

  stm_check_quiesce(tx);
}

/*
 * Rollback transaction.
 */
static inline void stm_rollback(stm_tx_t *tx, int reason)
{
  w_entry_t *w;
  int i;

  PRINT_DEBUG("==> stm_rollback(%p[%lu-%lu])\n", tx, (unsigned long)tx->start, (unsigned long)tx->end);

  assert(IS_ACTIVE(tx->status));

  /* Drop locks */
  i = tx->w_set.nb_entries;
  if (i > 0) {
    w = tx->w_set.entries;
    for (; i > 0; i--, w++) {
      if (w->next == NULL) {
        /* Only drop lock for last covered address in write set */
        ATOMIC_STORE(w->lock, LOCK_SET_TIMESTAMP(w->version));
      }
    }
    /* Make sure that all lock releases become visible */
    ATOMIC_MB_WRITE;
  }


  tx->retries++;
  tx->aborts++;
  if (tx->retries == 1)
    tx->aborts_1++;
  else if (tx->retries == 2)
    tx->aborts_2++;
  if (tx->max_retries < tx->retries)
    tx->max_retries = tx->retries;

  /* Callbacks */
  if (nb_abort_cb != 0) {
    int cb;
    for (cb = 0; cb < nb_abort_cb; cb++)
      abort_cb[cb].f(TXARGS abort_cb[cb].arg);
  }

  /* Set status to ABORTED */
  SET_STATUS(tx->status, TX_ABORTED);

  /* Reset nesting level */
  tx->nesting = 1;


  /* Wait until contented lock is free */
  if (tx->c_lock != NULL) {
    /* Busy waiting (yielding is expensive) */
    while (LOCK_GET_OWNED(ATOMIC_LOAD(tx->c_lock))) {
      sched_yield();
    }
    tx->c_lock = NULL;
  }

  /* Reset field to restart transaction */
  stm_prepare(tx);

  /* Jump back to transaction start */
  if (tx->attr == NULL || !tx->attr->no_retry)
    siglongjmp(tx->env, reason);
}

/*
 * Load a word-sized value (invisible read).
 */
static inline stm_word_t stm_read_invisible(stm_tx_t *tx, volatile stm_word_t *addr)
{
  volatile stm_word_t *lock;
  stm_word_t l, l2, value, version;
  r_entry_t *r;
  w_entry_t *w;

  PRINT_DEBUG2("==> stm_read_invisible(t=%p[%lu-%lu],a=%p)\n", tx, (unsigned long)tx->start, (unsigned long)tx->end, addr);

  assert(IS_ACTIVE(tx->status));


  /* Get reference to lock */
  lock = GET_LOCK(addr);

  /* Note: we could check for duplicate reads and get value from read set */

  /* Read lock, value, lock */
 restart:
  l = ATOMIC_LOAD_ACQ(lock);
 restart_no_load:
  if (LOCK_GET_WRITE(l)) {
    /* Locked */
    if (l == LOCK_UNIT) {
      /* Data modified by a unit store: should not last long => retry */
      goto restart;
    }
    /* Do we own the lock? */
    w = (w_entry_t *)LOCK_GET_ADDR(l);
    /* Simply check if address falls inside our write set (avoids non-faulting load) */
    if (tx->w_set.entries <= w && w < tx->w_set.entries + tx->w_set.nb_entries) {
      /* Yes: did we previously write the same address? */
      while (1) {
        if (addr == w->addr) {
          /* Yes: get value from write set (or from memory if mask was empty) */
          value = (w->mask == 0 ? ATOMIC_LOAD(addr) : w->value);
          break;
        }
        if (w->next == NULL) {
          /* No: get value from memory */
          value = ATOMIC_LOAD(addr);
          break;
        }
        w = w->next;
      }
      /* No need to add to read set (will remain valid) */
      return value;
    }
    /* Conflict: CM kicks in (we could also check for duplicate reads and get value from read set) */
    tx->c_lock = lock;
    /* Abort */
    tx->aborts_locked_read++;
    stm_rollback(tx, STM_ABORT_RW_CONFLICT);
    return 0;
  } else {
    /* Not locked */
    value = ATOMIC_LOAD_ACQ(addr);
    l2 = ATOMIC_LOAD_ACQ(lock);
    if (l != l2) {
      l = l2;
      goto restart_no_load;
    }
    /* Check timestamp */
    version = LOCK_GET_TIMESTAMP(l);
    /* Valid version? */
    if (version > tx->end) {
      /* No: try to extend first (except for read-only transactions: no read set) */
      if (tx->ro || !tx->can_extend || !stm_extend(tx)) {
        /* Not much we can do: abort */
        tx->aborts_validate_read++;
        stm_rollback(tx, STM_ABORT_VAL_READ);
        return 0;
      }
      /* Verify that version has not been overwritten (read value has not
       * yet been added to read set and may have not been checked during
       * extend) */
      l = ATOMIC_LOAD_ACQ(lock);
      if (l != l2) {
        l = l2;
        goto restart_no_load;
      }
      /* Worked: we now have a good version (version <= tx->end) */
    }
  }
  /* We have a good version: add to read set (update transactions) and return value */

  if (!tx->ro) {
    /* Add address and version to read set */
    if (tx->r_set.nb_entries == tx->r_set.size)
      stm_allocate_rs_entries(tx, 1);
    r = &tx->r_set.entries[tx->r_set.nb_entries++];
    r->version = version;
    r->lock = lock;
  }
  return value;
}


/*
 * Store a word-sized value (return write set entry or NULL).
 */
static inline w_entry_t *stm_write(stm_tx_t *tx, volatile stm_word_t *addr, stm_word_t value, stm_word_t mask)
{
  volatile stm_word_t *lock;
  stm_word_t l, version;
  w_entry_t *w;
  w_entry_t *prev = NULL;

  PRINT_DEBUG2("==> stm_write(t=%p[%lu-%lu],a=%p,d=%p-%lu,m=0x%lx)\n",
               tx, (unsigned long)tx->start, (unsigned long)tx->end, addr, (void *)value, (unsigned long)value, (unsigned long)mask);

  assert(IS_ACTIVE(tx->status));

  if (tx->ro) {
    /* Disable read-only and abort */
    assert(tx->attr != NULL);
    /* Update attributes to inform the caller */
    tx->attr->read_only = 0;
    tx->aborts_ro++;
    stm_rollback(tx, STM_ABORT_RO_WRITE);
    return NULL;
  }

  /* Get reference to lock */
  lock = GET_LOCK(addr);

  /* Try to acquire lock */
 restart:
  l = ATOMIC_LOAD_ACQ(lock);
 restart_no_load:
  if (LOCK_GET_OWNED(l)) {
    /* Locked */
    if (l == LOCK_UNIT) {
      /* Data modified by a unit store: should not last long => retry */
      goto restart;
    }
    /* Do we own the lock? */
    w = (w_entry_t *)LOCK_GET_ADDR(l);
    /* Simply check if address falls inside our write set (avoids non-faulting load) */
    if (tx->w_set.entries <= w && w < tx->w_set.entries + tx->w_set.nb_entries) {
      /* Yes */
      if (mask == 0) {
        /* No need to insert new entry or modify existing one */
        return w;
      }
      prev = w;
      /* Did we previously write the same address? */
      while (1) {
        if (addr == prev->addr) {
          /* No need to add to write set */
          if (mask != ~(stm_word_t)0) {
            if (prev->mask == 0)
              prev->value = ATOMIC_LOAD(addr);
            value = (prev->value & ~mask) | (value & mask);
          }
          prev->value = value;
          prev->mask |= mask;
          return prev;
        }
        if (prev->next == NULL) {
          /* Remember last entry in linked list (for adding new entry) */
          break;
        }
        prev = prev->next;
      }
      /* Get version from previous write set entry (all entries in linked list have same version) */
      version = prev->version;
      /* Must add to write set */
      if (tx->w_set.nb_entries == tx->w_set.size)
        stm_allocate_ws_entries(tx, 1);
      w = &tx->w_set.entries[tx->w_set.nb_entries];
      goto do_write;
    }
    /* Conflict: CM kicks in */
    tx->c_lock = lock;
    /* Abort */
    tx->aborts_locked_write++;
    stm_rollback(tx, STM_ABORT_WW_CONFLICT);
    return NULL;
  }
  /* Not locked */
  /* Handle write after reads (before CAS) */
  version = LOCK_GET_TIMESTAMP(l);
 acquire:
  if (version > tx->end) {
    /* We might have read an older version previously */
    if (!tx->can_extend || stm_has_read(tx, lock) != NULL) {
      /* Read version must be older (otherwise, tx->end >= version) */
      /* Not much we can do: abort */
      tx->aborts_validate_write++;
      stm_rollback(tx, STM_ABORT_VAL_WRITE);
      return NULL;
    }
  }
  /* Acquire lock (ETL) */
  if (tx->w_set.nb_entries == tx->w_set.size)
    stm_allocate_ws_entries(tx, 1);
  w = &tx->w_set.entries[tx->w_set.nb_entries];
  if (ATOMIC_CAS_FULL(lock, l, LOCK_SET_ADDR_WRITE((stm_word_t)w)) == 0)
    goto restart;
  /* We own the lock here (ETL) */
do_write:
  /* Add address to write set */
  w->addr = addr;
  w->mask = mask;
  w->lock = lock;
  if (mask == 0) {
    /* Do not write anything */
#ifndef NDEBUG
    w->value = 0;
#endif /* ! NDEBUG */
  } else
  {
    /* Remember new value */
    if (mask != ~(stm_word_t)0)
      value = (ATOMIC_LOAD(addr) & ~mask) | (value & mask);
    w->value = value;
  }
  w->version = version;
  w->next = NULL;
  if (prev != NULL) {
    /* Link new entry in list */
    prev->next = w;
  }
  tx->w_set.nb_entries++;
  tx->w_set.has_writes++;


  return w;
}

/*
 * Store a word-sized value in a unit transaction.
 */
static inline int stm_unit_write(volatile stm_word_t *addr, stm_word_t value, stm_word_t mask, stm_word_t *timestamp)
{
  volatile stm_word_t *lock;
  stm_word_t l;

  PRINT_DEBUG2("==> stm_unit_write(a=%p,d=%p-%lu,m=0x%lx)\n",
               addr, (void *)value, (unsigned long)value, (unsigned long)mask);

  /* Get reference to lock */
  lock = GET_LOCK(addr);

  /* Try to acquire lock */
 restart:
  l = ATOMIC_LOAD_ACQ(lock);
  if (LOCK_GET_OWNED(l)) {
    /* Locked: wait until lock is free */
    sched_yield();
    goto restart;
  }
  /* Not locked */
  if (timestamp != NULL && LOCK_GET_TIMESTAMP(l) > *timestamp) {
    /* Return current timestamp */
    *timestamp = LOCK_GET_TIMESTAMP(l);
    return 0;
  }
  /* TODO: would need to store thread ID to be able to kill it (for wait freedom) */
  if (ATOMIC_CAS_FULL(lock, l, LOCK_UNIT) == 0)
    goto restart;
  ATOMIC_STORE(addr, value);
  /* Update timestamp with newer value (may exceed VERSION_MAX by up to MAX_THREADS) */
  l = FETCH_INC_CLOCK + 1;
  if (timestamp != NULL)
    *timestamp = l;
  /* Make sure that lock release becomes visible */
  ATOMIC_STORE_REL(lock, LOCK_SET_TIMESTAMP(l));
  if (l >= VERSION_MAX) {
    /* Block all transactions and reset clock (current thread is not in active transaction) */
    stm_quiesce_barrier(NULL, rollover_clock, NULL);
  }
  return 1;
}

/*
 * Catch signal (to emulate non-faulting load).
 */
static void signal_catcher(int sig)
{
  stm_tx_t *tx = stm_get_tx();

  /* A fault might only occur upon a load concurrent with a free (read-after-free) */
  PRINT_DEBUG("Caught signal: %d\n", sig);

  if (tx == NULL || (tx->attr != NULL && tx->attr->no_retry)) {
    /* There is not much we can do: execution will restart at faulty load */
    fprintf(stderr, "Error: invalid memory accessed and no longjmp destination\n");
    exit(1);
  }

  tx->aborts_invalid_memory++;
  /* Will cause a longjmp */
  stm_rollback(tx, STM_ABORT_SIGNAL);
}

/* ################################################################### *
 * STM FUNCTIONS
 * ################################################################### */

/*
 * Called once (from main) to initialize STM infrastructure.
 */
void stm_init()
{
  struct sigaction act;

  PRINT_DEBUG("==> stm_init()\n");

  if (initialized)
    return;

  PRINT_DEBUG("\tsizeof(word)=%d\n", (int)sizeof(stm_word_t));

  PRINT_DEBUG("\tVERSION_MAX=0x%lx\n", (unsigned long)VERSION_MAX);

  COMPILE_TIME_ASSERT(sizeof(stm_word_t) == sizeof(void *));
  COMPILE_TIME_ASSERT(sizeof(stm_word_t) == sizeof(atomic_t));


  memset((void *)locks, 0, LOCK_ARRAY_SIZE * sizeof(stm_word_t));


  CLOCK = 0;
  stm_quiesce_init();


  if (getenv(NO_SIGNAL_HANDLER) == NULL) {
    /* Catch signals for non-faulting load */
    act.sa_handler = signal_catcher;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGBUS, &act, NULL) < 0 || sigaction(SIGSEGV, &act, NULL) < 0) {
      perror("sigaction");
      exit(1);
    }
  }
  initialized = 1;
}

/*
 * Called once (from main) to clean up STM infrastructure.
 */
void stm_exit()
{
  PRINT_DEBUG("==> stm_exit()\n");

  stm_quiesce_exit();

}

/*
 * Called by the CURRENT thread to initialize thread-local STM data.
 */
TXTYPE stm_init_thread()
{
  stm_tx_t *tx;

  PRINT_DEBUG("==> stm_init_thread()\n");

  if ((tx = stm_get_tx()) != NULL)
    TX_RETURN;


  /* Allocate descriptor */
  if ((tx = (stm_tx_t *)malloc(sizeof(stm_tx_t))) == NULL) {
    perror("malloc tx");
    exit(1);
  }
  /* Set status (no need for CAS or atomic op) */
  tx->status = TX_IDLE;
  /* Read set */
  tx->r_set.nb_entries = 0;
  tx->r_set.size = RW_SET_SIZE;
  stm_allocate_rs_entries(tx, 0);
  /* Write set */
  tx->w_set.nb_entries = 0;
  tx->w_set.size = RW_SET_SIZE;
  stm_allocate_ws_entries(tx, 0);
  /* Nesting level */
  tx->nesting = 0;
  /* Transaction-specific data */
  memset(tx->data, 0, MAX_SPECIFIC * sizeof(void *));
  /* Contented lock */
  tx->c_lock = NULL;
  tx->retries = 0;
  /* Statistics */
  tx->aborts = 0;
  tx->aborts_1 = 0;
  tx->aborts_2 = 0;
  tx->aborts_ro = 0;
  tx->aborts_locked_read = 0;
  tx->aborts_locked_write = 0;
  tx->aborts_validate_read = 0;
  tx->aborts_validate_write = 0;
  tx->aborts_validate_commit = 0;
  tx->aborts_invalid_memory = 0;
  tx->max_retries = 0;
  /* Store as thread-local data */
  thread_tx = tx;
  stm_quiesce_enter_thread(tx);

  /* Callbacks */
  if (nb_init_cb != 0) {
    int cb;
    for (cb = 0; cb < nb_init_cb; cb++)
      init_cb[cb].f(TXARGS init_cb[cb].arg);
  }

  TX_RETURN;
}

/*
 * Called by the CURRENT thread to cleanup thread-local STM data.
 */
void stm_exit_thread(TXPARAM)
{
  TX_GET;

  PRINT_DEBUG("==> stm_exit_thread(%p[%lu-%lu])\n", tx, (unsigned long)tx->start, (unsigned long)tx->end);

  /* Callbacks */
  if (nb_exit_cb != 0) {
    int cb;
    for (cb = 0; cb < nb_exit_cb; cb++)
      exit_cb[cb].f(TXARGS exit_cb[cb].arg);
  }

  stm_quiesce_exit_thread(tx);

  free(tx->r_set.entries);
  free(tx->w_set.entries);
  free(tx);

  thread_tx = NULL;
}

/*
 * Called by the CURRENT thread to start a transaction.
 */
sigjmp_buf *stm_start(TXPARAMS stm_tx_attr_t *attr)
{
  TX_GET;

  PRINT_DEBUG("==> stm_start(%p)\n", tx);

  /* Increment nesting level */
  if (tx->nesting++ > 0)
    return NULL;

  /* Attributes */
  tx->attr = attr;
  tx->ro = (attr == NULL ? 0 : attr->read_only);

  /* Initialize transaction descriptor */
  stm_prepare(tx);

  /* Callbacks */
  if (nb_start_cb != 0) {
    int cb;
    for (cb = 0; cb < nb_start_cb; cb++)
      start_cb[cb].f(TXARGS start_cb[cb].arg);
  }

  return &tx->env;
}

/*
 * Called by the CURRENT thread to commit a transaction.
 */
int stm_commit(TXPARAM)
{
  w_entry_t *w;
  stm_word_t t;
  int i;
  TX_GET;

  PRINT_DEBUG("==> stm_commit(%p[%lu-%lu])\n", tx, (unsigned long)tx->start, (unsigned long)tx->end);

  /* Decrement nesting level */
  if (--tx->nesting > 0)
    return 1;

  assert(IS_ACTIVE(tx->status));

  /* A read-only transaction can commit immediately */
  if (tx->w_set.nb_entries == 0)
    goto end;


  /* Update transaction */

  /* Get commit timestamp (may exceed VERSION_MAX by up to MAX_THREADS) */
  t = FETCH_INC_CLOCK + 1;

  /* Try to validate (only if a concurrent transaction has committed since tx->start) */
  if (tx->start != t - 1 && !stm_validate(tx)) {
    /* Cannot commit */
    tx->aborts_validate_commit++;
    stm_rollback(tx, STM_ABORT_VALIDATE);
    return 0;
  }

  /* Install new versions, drop locks and set new timestamp */
  w = tx->w_set.entries;
  for (i = tx->w_set.nb_entries; i > 0; i--, w++) {
    if (w->mask != 0)
      ATOMIC_STORE(w->addr, w->value);
    /* Only drop lock for last covered address in write set */
    if (w->next == NULL)
      ATOMIC_STORE_REL(w->lock, LOCK_SET_TIMESTAMP(t));
  }

 end:
  tx->retries = 0;



  /* Callbacks */
  if (nb_commit_cb != 0) {
    int cb;
    for (cb = 0; cb < nb_commit_cb; cb++)
      commit_cb[cb].f(TXARGS commit_cb[cb].arg);
  }


  /* Set status to COMMITTED */
  SET_STATUS(tx->status, TX_COMMITTED);

  return 1;
}

/*
 * Called by the CURRENT thread to abort a transaction.
 */
void stm_abort(TXPARAMS int reason)
{
  TX_GET;
  stm_rollback(tx, reason | STM_ABORT_EXPLICIT);
}

/*
 * Called by the CURRENT thread to load a word-sized value.
 */
stm_word_t stm_load(TXPARAMS volatile stm_word_t *addr)
{
  TX_GET;

  return stm_read_invisible(tx, addr);
}

/*
 * Called by the CURRENT thread to store a word-sized value.
 */
void stm_store(TXPARAMS volatile stm_word_t *addr, stm_word_t value)
{
  TX_GET;

  stm_write(tx, addr, value, ~(stm_word_t)0);
}

/*
 * Called by the CURRENT thread to store part of a word-sized value.
 */
void stm_store2(TXPARAMS volatile stm_word_t *addr, stm_word_t value, stm_word_t mask)
{
  TX_GET;

  stm_write(tx, addr, value, mask);
}

/*
 * Called by the CURRENT thread to inquire about the status of a transaction.
 */
int stm_active(TXPARAM)
{
  TX_GET;

  return IS_ACTIVE(tx->status);
}

/*
 * Called by the CURRENT thread to inquire about the status of a transaction.
 */
int stm_aborted(TXPARAM)
{
  TX_GET;

  return (GET_STATUS(tx->status) == TX_ABORTED);
}


/*
 * Called by the CURRENT thread to obtain an environment for setjmp/longjmp.
 */
sigjmp_buf *stm_get_env(TXPARAM)
{
  TX_GET;

  /* Only return environment for top-level transaction */
  return tx->nesting == 0 ? &tx->env : NULL;
}

/*
 * Get transaction attributes.
 */
stm_tx_attr_t *stm_get_attributes(TXPARAM)
{
  TX_GET;

  return tx->attr;
}

/*
 * Return statistics about a thread/transaction.
 */
int stm_get_stats(TXPARAMS const char *name, void *val)
{
  TX_GET;

  if (strcmp("read_set_size", name) == 0) {
    *(unsigned int *)val = tx->r_set.size;
    return 1;
  }
  if (strcmp("write_set_size", name) == 0) {
    *(unsigned int *)val = tx->w_set.size;
    return 1;
  }
  if (strcmp("read_set_nb_entries", name) == 0) {
    *(unsigned int *)val = tx->r_set.nb_entries;
    return 1;
  }
  if (strcmp("write_set_nb_entries", name) == 0) {
    *(unsigned int *)val = tx->w_set.nb_entries;
    return 1;
  }
  if (strcmp("read_only", name) == 0) {
    *(unsigned int *)val = tx->ro;
    return 1;
  }
  if (strcmp("nb_aborts", name) == 0) {
    *(unsigned long *)val = tx->aborts;
    return 1;
  }
  if (strcmp("nb_aborts_1", name) == 0) {
    *(unsigned long *)val = tx->aborts_1;
    return 1;
  }
  if (strcmp("nb_aborts_2", name) == 0) {
    *(unsigned long *)val = tx->aborts_2;
    return 1;
  }
  if (strcmp("nb_aborts_ro", name) == 0) {
    *(unsigned long *)val = tx->aborts_ro;
    return 1;
  }
  if (strcmp("nb_aborts_locked_read", name) == 0) {
    *(unsigned long *)val = tx->aborts_locked_read;
    return 1;
  }
  if (strcmp("nb_aborts_locked_write", name) == 0) {
    *(unsigned long *)val = tx->aborts_locked_write;
    return 1;
  }
  if (strcmp("nb_aborts_validate_read", name) == 0) {
    *(unsigned long *)val = tx->aborts_validate_read;
    return 1;
  }
  if (strcmp("nb_aborts_validate_write", name) == 0) {
    *(unsigned long *)val = tx->aborts_validate_write;
    return 1;
  }
  if (strcmp("nb_aborts_validate_commit", name) == 0) {
    *(unsigned long *)val = tx->aborts_validate_commit;
    return 1;
  }
  if (strcmp("nb_aborts_invalid_memory", name) == 0) {
    *(unsigned long *)val = tx->aborts_invalid_memory;
    return 1;
  }
  if (strcmp("max_retries", name) == 0) {
    *(unsigned long *)val = tx->max_retries;
    return 1;
  }
  return 0;
}

/*
 * Return STM parameters.
 */
int stm_get_parameter(const char *name, void *val)
{
  if (strcmp("contention_manager", name) == 0) {
    *(const char **)val = cm_names[CM];
    return 1;
  }
  if (strcmp("design", name) == 0) {
    *(const char **)val = design_names[DESIGN];
    return 1;
  }
  if (strcmp("initial_rw_set_size", name) == 0) {
    *(int *)val = RW_SET_SIZE;
    return 1;
  }
#ifdef COMPILE_FLAGS
  if (strcmp("compile_flags", name) == 0) {
    *(const char **)val = XSTR(COMPILE_FLAGS);
    return 1;
  }
#endif /* COMPILE_FLAGS */
  return 0;
}

/*
 * Set STM parameters.
 */
int stm_set_parameter(const char *name, void *val)
{
  return 0;
}

/*
 * Create transaction-specific data (return -1 on error).
 */
int stm_create_specific()
{
  if (nb_specific >= MAX_SPECIFIC) {
    fprintf(stderr, "Error: maximum number of specific slots reached\n");
    return -1;
  }
  return nb_specific++;
}

/*
 * Store transaction-specific data.
 */
void stm_set_specific(TXPARAMS int key, void *data)
{
  TX_GET;

  assert (key >= 0 && key < nb_specific);
  tx->data[key] = data;
}

/*
 * Fetch transaction-specific data.
 */
void *stm_get_specific(TXPARAMS int key)
{
  TX_GET;

  assert (key >= 0 && key < nb_specific);
  return tx->data[key];
}

/*
 * Register callbacks for an external module (must be called before creating transactions).
 */
int stm_register(void (*on_thread_init)(TXPARAMS void *arg),
                 void (*on_thread_exit)(TXPARAMS void *arg),
                 void (*on_start)(TXPARAMS void *arg),
                 void (*on_commit)(TXPARAMS void *arg),
                 void (*on_abort)(TXPARAMS void *arg),
                 void *arg)
{
  if ((on_thread_init != NULL && nb_init_cb >= MAX_CB) ||
      (on_thread_exit != NULL && nb_exit_cb >= MAX_CB) ||
      (on_start != NULL && nb_start_cb >= MAX_CB) ||
      (on_commit != NULL && nb_commit_cb >= MAX_CB) ||
      (on_abort != NULL && nb_abort_cb >= MAX_CB)) {
    fprintf(stderr, "Error: maximum number of modules reached\n");
    return 0;
  }
  /* New callback */
  if (on_thread_init != NULL) {
    init_cb[nb_init_cb].f = on_thread_init;
    init_cb[nb_init_cb++].arg = arg;
  }
  /* Delete callback */
  if (on_thread_exit != NULL) {
    exit_cb[nb_exit_cb].f = on_thread_exit;
    exit_cb[nb_exit_cb++].arg = arg;
  }
  /* Start callback */
  if (on_start != NULL) {
    start_cb[nb_start_cb].f = on_start;
    start_cb[nb_start_cb++].arg = arg;
  }
  /* Commit callback */
  if (on_commit != NULL) {
    commit_cb[nb_commit_cb].f = on_commit;
    commit_cb[nb_commit_cb++].arg = arg;
  }
  /* Abort callback */
  if (on_abort != NULL) {
    abort_cb[nb_abort_cb].f = on_abort;
    abort_cb[nb_abort_cb++].arg = arg;
  }

  return 1;
}

/*
 * Called by the CURRENT thread to load a word-sized value in a unit transaction.
 */
stm_word_t stm_unit_load(volatile stm_word_t *addr, stm_word_t *timestamp)
{
  volatile stm_word_t *lock;
  stm_word_t l, l2, value;

  PRINT_DEBUG2("==> stm_unit_load(a=%p)\n", addr);

  /* Get reference to lock */
  lock = GET_LOCK(addr);

  /* Read lock, value, lock */
 restart:
  l = ATOMIC_LOAD_ACQ(lock);
 restart_no_load:
  if (LOCK_GET_OWNED(l)) {
    /* Locked: wait until lock is free */
    sched_yield();
    goto restart;
  }
  /* Not locked */
  value = ATOMIC_LOAD_ACQ(addr);
  l2 = ATOMIC_LOAD_ACQ(lock);
  if (l != l2) {
    l = l2;
    goto restart_no_load;
  }

  if (timestamp != NULL)
    *timestamp = LOCK_GET_TIMESTAMP(l);

  return value;
}

/*
 * Called by the CURRENT thread to store a word-sized value in a unit transaction.
 */
int stm_unit_store(volatile stm_word_t *addr, stm_word_t value, stm_word_t *timestamp)
{
  return stm_unit_write(addr, value, ~(stm_word_t)0, timestamp);
}

/*
 * Called by the CURRENT thread to store part of a word-sized value in a unit transaction.
 */
int stm_unit_store2(volatile stm_word_t *addr, stm_word_t value, stm_word_t mask, stm_word_t *timestamp)
{
  return stm_unit_write(addr, value, mask, timestamp);
}

/*
 * Enable or disable extensions and set upper bound on snapshot.
 */
void stm_set_extension(TXPARAMS int enable, stm_word_t *timestamp)
{
  TX_GET;

  tx->can_extend = enable;
  if (timestamp != NULL && *timestamp < tx->end)
    tx->end = *timestamp;
}

/*
 * Get curent value of global clock.
 */
stm_word_t stm_get_clock()
{
  return GET_CLOCK;
}

/*
 * Get current transaction descriptor.
 */
stm_tx_t *stm_current_tx()
{
  return stm_get_tx();
}

/* ################################################################### *
 * UNDOCUMENTED STM FUNCTIONS (USE WITH CARE!)
 * ################################################################### */


int stm_set_irrevocable(TXPARAMS int serial)
{
  fprintf(stderr, "Irrevocability is not supported in this configuration\n");
  exit(-1);
  return 1;
}
