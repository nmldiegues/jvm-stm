package t13;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/**
 * Also static fields in a more complex situation with super calls.
 */
public class Main {

	public static int NUMBER_THREADS;
	public static int NUMBER_INCS;

	public static void main(String[] args) {

		if(args.length != 2) {
			System.out.println("Usage: ./jamvm t12/Main <number_threads> <number_loops>");
			return;
		}

		NUMBER_THREADS = Integer.parseInt(args[0]);
		NUMBER_INCS = Integer.parseInt(args[1]);
		
		ExecutorService threadPool = Executors.newCachedThreadPool();

		DummySuperClass[] itemsArray = new DummyClass[NUMBER_THREADS];
		int expectedSum = 0;

		for(int i = 0; i < NUMBER_THREADS; i++) {
			itemsArray[i] = new DummyClass(i + 1);
			expectedSum += (i + 1);
		}

		Runnable[] runnables = new Runnable[NUMBER_THREADS];
		Future<?>[] futures = new Future<?>[NUMBER_THREADS];
		for(int i = 0; i < runnables.length; i++){
			runnables[i] = new Worker(itemsArray[i], NUMBER_INCS);
		}

		long startTime = System.nanoTime();

		for(int i = 0; i < runnables.length; i++){
			futures[i] = threadPool.submit(runnables[i]);
		}

		for(int i = 0; i < runnables.length; i++){
			try {
				futures[i].get();
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (ExecutionException e) {
				e.printStackTrace();
				System.exit(0);
			}
		}

		threadPool.shutdownNow();

		expectedSum *= NUMBER_INCS;

		System.out.println("Expected: " +  expectedSum);
		System.out.println("Obtained: " + itemsArray[0].getCounter());

	}

	public static class Worker implements Runnable {

		final private DummySuperClass item;
		final private int numIncs;

		public Worker(DummySuperClass item, int numIncs){
			this.item = item;
			this.numIncs = numIncs;
		}

		@Override
		public void run() {

			for(int i = 0; i < numIncs; i++) {
				item.inc();
			}

		}
	}
}