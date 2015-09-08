package t2;

/**
 * Tests concurrent writes and reads to shared public normal fields.
 * This should not be any different than what t1 already exercises.
 */
public class TestAll{
	
	public static void main(String[] args) throws Exception{
		
		testInt();
		testShort();
		testFloat();
		testLong();
		testDouble();
		testRef();
		
	}
	
	public static void testInt() throws Exception {
		IntHolder holder = new IntHolder();
		holder.number = 0;
		
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
		System.out.println("\tObtained: " + holder.number + "\n");
	}
	
	public static void testShort() throws Exception {
		ShortHolder holder = new ShortHolder();
		holder.number = (short)0;
		
		Thread t1 = new ThreadShort(holder);
		Thread t2 = new ThreadShort(holder);
		Thread t3 = new ThreadShort(holder);
		Thread t4 = new ThreadShort(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Shorts");
		System.out.println("\tExpected: " + 5000*4);
		System.out.println("\tObtained: " + holder.number + "\n");
	}
	
	public static void testFloat() throws Exception {
		FloatHolder holder = new FloatHolder();
		holder.number = 0.0f;
		
		Thread t1 = new ThreadFloat(holder);
		Thread t2 = new ThreadFloat(holder);
		Thread t3 = new ThreadFloat(holder);
		Thread t4 = new ThreadFloat(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Float");
		System.out.println("\tExpected: " + 500000.0f*4.0f);
		System.out.println("\tObtained: " + holder.number + "\n");
	}
	
	public static void testLong() throws Exception {
		LongHolder holder = new LongHolder();
		holder.number = 0L;
		
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
		System.out.println("\tObtained: " + holder.number + "\n");
	}
	
	public static void testDouble() throws Exception {
		DoubleHolder holder = new DoubleHolder();
		holder.number = 0.0;
		
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
		System.out.println("\tObtained: " + holder.number + "\n");
	}
	
	public static void testRef() throws Exception {
		RefHolder holder = new RefHolder();
		holder.number = new Integer(0);
		
		Thread t1 = new ThreadRef(holder);
		Thread t2 = new ThreadRef(holder);
		Thread t3 = new ThreadRef(holder);
		Thread t4 = new ThreadRef(holder);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Test Ref");
		System.out.println("\tExpected: " + 500000*4);
		System.out.println("\tObtained: " + holder.number + "\n");
	}
}