package t6;

/**
 * Tests different levels of @Atomic annotation in method call chain.
 * This is relevant since these scenarios exercise different interception 
 * points in the VM where we start/commit the transaction.
 */
public class TestAll{
	
	public static void main(String[] args) throws Exception{
		
		testInt();
		testLong();
		testDouble();
		
	}
	
	public static void testInt() throws Exception {
		IntHolder holder = new IntHolder(0);
		Thread t1 = new ThreadInt(holder);
		Thread t2 = new ThreadInt(holder);
		Thread t3 = new ThreadInt(holder);
		Thread t4 = new ThreadInt(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Integers");
		System.out.println("\tExpected: " + 500000*4);
		System.out.println("\tObtained: " + holder.getNumber() + "\n");
	}
	
	public static void testLong() throws Exception {
		LongHolder holder = new LongHolder(0L);
		Thread t1 = new ThreadLong(holder);
		Thread t2 = new ThreadLong(holder);
		Thread t3 = new ThreadLong(holder);
		Thread t4 = new ThreadLong(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Long");
		System.out.println("\tExpected: " + 500000L*4L);
		System.out.println("\tObtained: " + holder.getNumber() + "\n");
	}
	
	public static void testDouble() throws Exception {
		DoubleHolder holder = new DoubleHolder(0);
		Thread t1 = new ThreadDouble(holder);
		Thread t2 = new ThreadDouble(holder);
		Thread t3 = new ThreadDouble(holder);
		Thread t4 = new ThreadDouble(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Double");
		System.out.println("\tExpected: " + 500000.0*4.0);
		System.out.println("\tObtained: " + holder.getNumber() + "\n");
	}
	
}