package t12;

import java.util.Random;

/**
 * Tests static writes/reads in a more complex scenario.
 */
public class Main {
	private static ICounter[] counters;
	
	private static class CounterThread extends Thread {
		private final Random rand;
		private final ICounter[] counters;
		private final int loops;
		
		public CounterThread(ICounter[] counters, int loops) {
			this.rand = new Random();
			this.counters = counters;
			this.loops = loops;
		}
		
		public void run() {
			for (int i = 0; i < loops; i++) {
				counters[rand.nextInt(counters.length)].inc();
			}
		}
	}
	
	public static void main(String[] args) throws Exception {
		
		if(args.length != 3) {
			System.out.println("Usage: ./jamvm t12/Main <number_threads> <num_counters> <number_loops>");
			return;
		}
		
		final int NUM_THREADS = Integer.parseInt(args[0]);
		final int NUM_COUNTERS = Integer.parseInt(args[1]);
		final int NUM_LOOPS = Integer.parseInt(args[2]);
		
		counters = new ICounter[NUM_COUNTERS];
		
		for (int i = 0; i < counters.length; i++) {
			counters[i] = new DistributedCounter();
		}
		
		CounterThread[] threads = new CounterThread[NUM_THREADS];
		for (int i = 0; i < threads.length; i++) {
			threads[i] = new CounterThread(counters, NUM_LOOPS);
		}
		
		for (CounterThread t : threads) {
			t.start();
		}
		
		for (CounterThread t : threads) {
			t.join();
		}
		
		System.out.println("Expected: " + NUM_THREADS * NUM_LOOPS);
		System.out.println("Obtained: " + DistributedCounter.getCount());
	}
}
