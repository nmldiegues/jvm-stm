package t11;

import java.util.Random;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/**
 * Tests writes/reads of references in transactions.
 */
public class Main {

	public static int NUMBER_ITEMS;
	public static int NUMBER_THREADS;
	public static int NUMBER_LOOPS;
	public static LinkedItem[] debugArr;
	
	public static void main(String[] args) {
		if(args.length != 3) {
			System.out.println("Usage: ./jamvm t11/Main <number_items> <number_threads> <number_loops>");
			return;
		}
		NUMBER_ITEMS = Integer.parseInt(args[0]);
		NUMBER_THREADS = Integer.parseInt(args[1]);
		NUMBER_LOOPS = Integer.parseInt(args[2]);
		debugArr = new LinkedItem[NUMBER_ITEMS];
		
		ExecutorService threadPool = Executors.newCachedThreadPool();
		LinkedItem iterItem = new LinkedItem(0);
		LinkedItem startItem = iterItem;
		debugArr[0] = startItem;
		
		for(int i = 1; i < NUMBER_ITEMS; i++){
			LinkedItem tmp = iterItem;
			iterItem = new LinkedItem(i);
			debugArr[i] = iterItem;
			iterItem.setPrevious(tmp);
			tmp.setNext(iterItem);
		}
		
		iterItem.setNext(startItem);
		startItem.setPrevious(iterItem);
		
		Runnable[] runnables = new Runnable[NUMBER_THREADS];
		Future<?>[] futures = new Future<?>[NUMBER_THREADS];
		for(int i = 0; i < runnables.length; i++){
			runnables[i] = new Worker(startItem, NUMBER_LOOPS);
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
		
		int totalSumForwards = 0;
		iterItem = startItem;
		for(int i = 0; i < NUMBER_ITEMS; i++){
			totalSumForwards += iterItem.getId();
			iterItem = iterItem.getNext();
		}
		
		int totalSumBackwards = 0;
		iterItem = startItem;
		for(int i = 0; i < NUMBER_ITEMS; i++){
			totalSumBackwards += iterItem.getId();
			iterItem = iterItem.getPrevious();
		}
		
		int expectedSum = ((NUMBER_ITEMS - 1) * (NUMBER_ITEMS)) / 2;
		
		System.out.println("Consistency check. Expected: " + expectedSum + " ;forwards: " + totalSumForwards + " ;backwards: " + totalSumBackwards);
	}

	final private static ThreadLocal<Random> RANDOM = new ThreadLocal<Random>(){
		@Override
		protected Random initialValue() {
			return new Random();
		}
	};
	
	public static class Worker implements Runnable {

		final private LinkedItem startItem;
		final private int loops;
		
		public Worker(LinkedItem startItem, int loops){
			this.startItem = startItem;
			this.loops = loops;
		}
		
		@Override
		public void run() {
			for(int i = 0; i < loops; i++){
				swapRandomItems((i % 3) != 0);
			}
		}
		
		private boolean acceptableSwap(LinkedItem itemA, LinkedItem itemB){
			LinkedItem itemA_previous = itemA.getPrevious();
			LinkedItem itemA_next = itemA.getNext();
			
			if(itemA_previous == itemB || itemA_next == itemB) return false;
			
			return true;
		}
		
		@avestm.Atomic
		private void swapRandomItems(boolean forward){
			Random rnd = RANDOM.get();
			int itemA_ID = rnd.nextInt(NUMBER_ITEMS);
			int itemB_ID = rnd.nextInt(NUMBER_ITEMS);
			
			while(itemA_ID == itemB_ID){
				itemB_ID = rnd.nextInt(NUMBER_ITEMS);
			}
			
			LinkedItem itemA = null;
			LinkedItem itemB = null;
			LinkedItem iterItem = startItem;
			
			for(int i = 0; i < NUMBER_ITEMS; i++){
				int curID = iterItem.getId();
				
				if(curID == itemA_ID) itemA = iterItem;
				else if(curID == itemB_ID) itemB = iterItem;
				
				if(forward) iterItem = iterItem.getNext();
				else iterItem = iterItem.getPrevious();
			}
			
			if(itemA == null || itemB == null){
//				for(int i = 0; i < NUMBER_ITEMS; i++){
//					int curID = debugArr[i].getId();
//					int prevID = debugArr[i].getPrevious().getId();
//					int nextID = debugArr[i].getNext().getId();
//					System.out.println("Item " + curID + " has prev: " + prevID + " has next " + nextID);
//				}
				throw new RuntimeException("Failed consistency! Could not find one of the items: " + itemA_ID + " " + itemB_ID);
			}
			
			if(!acceptableSwap(itemA, itemB)) return;
			
			LinkedItem itemA_previous = itemA.getPrevious();
			LinkedItem itemA_next = itemA.getNext();
			LinkedItem itemB_previous = itemB.getPrevious();
			LinkedItem itemB_next= itemB.getNext();
			
			itemA.setPrevious(itemB_previous);
			itemB_previous.setNext(itemA);
			itemA.setNext(itemB_next);
			itemB_next.setPrevious(itemA);
			
			itemB.setPrevious(itemA_previous);
			itemA_previous.setNext(itemB);
			itemB.setNext(itemA_next);
			itemA_next.setPrevious(itemB);
			
			//System.out.println("Finished a swap between: " + itemA_ID + " " + itemB_ID);
		}
	}
	
}