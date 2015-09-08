#include <pthread.h>

#include "stm.h"

#define RO                              1
#define RW                              0
#define START(id, ro)                   { stm_tx_attr_t _a = {id, ro}; sigjmp_buf *_e = stm_start(&_a); if (_e != NULL) sigsetjmp(*_e, 0)
#define LOAD(addr)                      stm_load((stm_word_t *)addr)
#define STORE(addr, value)              stm_store((stm_word_t *)addr, (stm_word_t)value)
#define COMMIT                          stm_commit(); }

typedef struct data_struct{
	int* ptr1;
	int* ptr2;
} Data;

void* test1(void* arg){
	stm_init_thread();
	Data* data = (Data*)arg;
	int i;
	int tmp;
	START(0, RW);
	for(i = 0; i < 1000; i++){
		if((i % 2) == 0){
			tmp = (int)LOAD(data->ptr1);
			STORE(data->ptr1, tmp + 1);
		}
		else{
			tmp = (int)LOAD(data->ptr2);
			STORE(data->ptr2, tmp + 1);
		}
	}
	COMMIT
	stm_exit_thread();

	return NULL;
}

void* test2(void* arg){
	stm_init_thread();
	Data* data = (Data*)arg;
	int i;
	int tmp;
	START(1, RW);
	for(i = 0; i < 1000; i++){
		if((i % 2) == 0){
			tmp = (int)LOAD(data->ptr2);
			STORE(data->ptr2, tmp + 1);
		}
		else{
			tmp = (int)LOAD(data->ptr1);
			STORE(data->ptr1, tmp + 1);
		}
	}
	COMMIT
	stm_exit_thread();

	return NULL;
}

int main(int argc, char* argv[]){
	stm_init();

	int* counter1;
	int* counter2;

	counter1 = (int*)malloc(sizeof(int));
	counter2 = (int*)malloc(sizeof(int));
	*counter1 = 0;
	*counter2 = 0;

	Data data;
	data.ptr1 = counter1;
	data.ptr2 = counter2;

	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1, NULL, test1, (void *)&data);
	pthread_create(&thread2, NULL, test2, (void *)&data);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Final. counter1: %d\tcounter2: %d\n", *counter1, *counter2);
	return 0;
}
