package t3;

/**
 * Tests concurrent writes and reads to shared public static fields.
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
		IntHolder.number = 0;
		
		Thread t1 = new ThreadInt();
		Thread t2 = new ThreadInt();
		Thread t3 = new ThreadInt();
		Thread t4 = new ThreadInt();
		
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
		System.out.println("\tObtained: " + IntHolder.number + "\n");
	}
	
	public static void testShort() throws Exception {
		ShortHolder.number = (short)0;
		
		Thread t1 = new ThreadShort();
		Thread t2 = new ThreadShort();
		Thread t3 = new ThreadShort();
		Thread t4 = new ThreadShort();
		
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
		System.out.println("\tObtained: " + ShortHolder.number + "\n");
	}
	
	public static void testFloat() throws Exception {
		FloatHolder.number = 0.0f;
		
		Thread t1 = new ThreadFloat();
		Thread t2 = new ThreadFloat();
		Thread t3 = new ThreadFloat();
		Thread t4 = new ThreadFloat();
		
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
		System.out.println("\tObtained: " + FloatHolder.number + "\n");
	}
	
	public static void testLong() throws Exception {
		LongHolder.number = 0L;
		
		Thread t1 = new ThreadLong();
		Thread t2 = new ThreadLong();
		Thread t3 = new ThreadLong();
		Thread t4 = new ThreadLong();
		
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
		System.out.println("\tObtained: " + LongHolder.number + "\n");
	}
	
	public static void testDouble() throws Exception {
		DoubleHolder.number = 0.0;
		
		Thread t1 = new ThreadDouble();
		Thread t2 = new ThreadDouble();
		Thread t3 = new ThreadDouble();
		Thread t4 = new ThreadDouble();
		
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
		System.out.println("\tObtained: " + DoubleHolder.number + "\n");
	}
	
	public static void testRef() throws Exception {
		RefHolder.number = new Integer(0);
		
		Thread t1 = new ThreadRef();
		Thread t2 = new ThreadRef();
		Thread t3 = new ThreadRef();
		Thread t4 = new ThreadRef();
		
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
		System.out.println("\tObtained: " + RefHolder.number + "\n");
	}
}