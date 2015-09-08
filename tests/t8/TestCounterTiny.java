package t8;

/**
 * Long shared counter increments with outerLoops transactions and innerLoops increments.
 */
public class TestCounterTiny {

	public static void main(String[] args) {
		final Counter counter = new CounterTiny();

		if(args.length != 2) {
			System.out.println("Usage: ./jamvm t8/TestCounterTiny <outer_loops> <inner_loops>");
			return;
		}
		
		int outerLoops = Integer.parseInt(args[0]);
		int innerLoops = Integer.parseInt(args[1]);
		for(int i = 0; i < outerLoops; i++) {
			Thread t1 = new ThreadTiny(counter, innerLoops);
			Thread t2 = new ThreadTiny(counter, innerLoops);
			Thread t3 = new ThreadTiny(counter, innerLoops);
			Thread t4 = new ThreadTiny(counter, innerLoops);

			t1.start();
			t2.start();
			t3.start();
			t4.start();

			try {
				t1.join();
				t2.join();
				t3.join();
				t4.join();
			} catch(InterruptedException e) {}
		}
		
		System.out.println("Expected: " + outerLoops * innerLoops * 4);
		System.out.println("Obtained: " + counter.getCount());
	}
}
