package t8;

import jvstm.Transaction;
import jvstm.TransactionalCommand;

/**
 * Long shared counter increments with outerLoops transactions and innerLoops increments.
 * javac -cp t8/build/jvstm.jar:. t8/TestCounterJVSTM.java 
 * ./jamvm_pristine -cp t8/build/jvstm.jar:. t8/TestCounterJVSTM 1000 1000
 */
public class TestCounterJVSTM {

	public static void main(String[] args) {
		final Counter counter = new CounterJVSTM();

		if(args.length != 2) {
			System.out.println("Usage: ./jamvm -cp t8/build/jvstm.jar:. t8/TestCounterJVSTM <outer_loops> <inner_loops>");
			return;
		}
		
		int outerLoops = Integer.parseInt(args[0]);
		int innerLoops = Integer.parseInt(args[1]);
		for(int i = 0; i < outerLoops; i++) {
			Thread t1 = new ThreadJVSTM(counter, innerLoops);
			Thread t2 = new ThreadJVSTM(counter, innerLoops);
			Thread t3 = new ThreadJVSTM(counter, innerLoops);
			Thread t4 = new ThreadJVSTM(counter, innerLoops);

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
