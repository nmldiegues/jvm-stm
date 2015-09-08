package t9;

import java.util.Random;

import jvstm.Transaction;
import jvstm.TransactionalCommand;

public class TestTransfersJVSTM {

	private static Random RND = new Random();

	Account ac[];
	Counter counter;

	TestTransfersJVSTM(int numAccounts) {
		counter = new CounterJVSTM();
		ac = new Account[numAccounts];

		for (int i = 0; i < numAccounts; i++) {
			ac[i] = new VAccountJVSTM(10);
		}
	}

	public Account[] getAccounts() {
		return ac;
	}
	
	void walkAccounts() {
		Transaction.transactionallyDo(new TransactionalCommand() {
			public void doIt() {
				long total = 0L;
				for(Account acc : ac) {
					total += acc.getBalance();
				}				
			}
		});
	}
	
	void transTransferAmount(final int idx1, final int idx2) {
		Transaction.transactionallyDo(new TransactionalCommand() {
			@Override
			public void doIt() {
				transferAmount(idx1, idx2);
			}
		});
	}

	void transferAmount(final int idx1, final int idx2) {
		final Account acc1 = ac[idx1];
		final Account acc2 = ac[idx2];

		final long value = acc1.getBalance() / 2;

		acc1.withdraw(value);
		acc2.deposit(value);
		counter.inc();
		
	}

	public static void main(String[] args) {
		
		if(args.length != 3) {
			System.out.println("Usage: ./jamvm -cp t8/build/jvstm.jar:. t9/TestTransfersJVSTM <num_accounts> <num_threads> <num_transfers>");
			return;
		}
		
		final int numAccounts = Integer.parseInt(args[0]);
		final int numThreads = Integer.parseInt(args[1]);
		final int numTotal = Integer.parseInt(args[2]);

		Transaction.begin();
		final TestTransfersJVSTM ti = new TestTransfersJVSTM(numAccounts);
		Transaction.commit();

		Thread threads[] = new Thread[numThreads];

		for (int i = 0; i < numThreads; i++) {
			threads[i] = new Thread() {
				@Override
				public void run() {
					long loops = numTotal / numThreads;
					for (int i = 0; i < loops; i++) {
						ti.transTransferAmount(RND.nextInt(numAccounts), RND.nextInt(numAccounts));
					}
					System.out.println("\t\tThread " + Thread.currentThread().getId() + " finished!");
				}
			};
		}

		Thread walker = new Thread() {
			@Override
			public void run() {
				while(true) {
					ti.walkAccounts();
				}
			}
		};
		walker.setDaemon(true);
		
		final long start = System.currentTimeMillis();

		walker.start();
		
		for (int i = 0; i < numThreads; i++) {
			threads[i].start();
		}        
		for (int i = 0; i < numThreads; i++) {
			try {
				threads[i].join();
			} catch (Throwable t) {
				throw new Error("erro");
			}
		}        
		System.out.println("TestTransfers\t" + numThreads 
				+ "\t" + numAccounts 
				+ "\t" + (System.currentTimeMillis() - start));
		System.out.println("Counter = " + ti.counter.getCount());
		
		long total = 0L;
		for(Account acc : ti.getAccounts()) {
			total += acc.getBalance();
		}
		
		System.out.println("Total = " + total + " should be: " + numAccounts*10);
	}
}
