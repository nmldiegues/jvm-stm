package t4;

/**
 * Tests concurrent writes and reads to shared arrays.
 * This one makes use of byte and char (contrary to the previous
 * tests) since it is able to have much more small counters 
 * of those types to actually have parallelism in the increments.
 */
public class TestAll{
	
	public static void main(String[] args) throws Exception{
		
		testInt();
		testByte();
		testChar();
		testShort();
		testFloat();
		testLong();
		testDouble();
		testRef();
		
	}
	
	public static void testInt() throws Exception {
		int[] number = new int[1000];
		for(int i = 0; i < number.length; i++) {
			number[i] = 0;
		}
		
		Thread t1 = new ThreadInt(number);
		Thread t2 = new ThreadInt(number);
		Thread t3 = new ThreadInt(number);
		Thread t4 = new ThreadInt(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		int sum = 0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}

		System.out.println("Test Integers");
		System.out.println("\tExpected: " + 500000*4);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testByte() throws Exception {
		byte[] number = new byte[100000];
		for(int i = 0; i < number.length; i++) {
			number[i] = (byte)0;
		}
		
		Thread t1 = new ThreadByte(number);
		Thread t2 = new ThreadByte(number);
		Thread t3 = new ThreadByte(number);
		Thread t4 = new ThreadByte(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		int sum = 0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}

		System.out.println("Test Bytes");
		System.out.println("\tExpected: " + 100000*4*25);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testChar() throws Exception {
		char[] number = new char[100000];
		for(int i = 0; i < number.length; i++) {
			number[i] = (char)0;
		}
		
		Thread t1 = new ThreadChar(number);
		Thread t2 = new ThreadChar(number);
		Thread t3 = new ThreadChar(number);
		Thread t4 = new ThreadChar(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		int sum = 0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}

		System.out.println("Test Char");
		System.out.println("\tExpected: " + 100000*4*50);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testShort() throws Exception {
		short[] number = new short[100];
		for(int i = 0; i < number.length; i++) {
			number[i] = (short)0;
		}
		
		Thread t1 = new ThreadShort(number);
		Thread t2 = new ThreadShort(number);
		Thread t3 = new ThreadShort(number);
		Thread t4 = new ThreadShort(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		short sum = 0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}
		
		System.out.println("Test Shorts");
		System.out.println("\tExpected: " + 5000*4);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testFloat() throws Exception {
		float[] number = new float[1000];
		for(int i = 0; i < number.length; i++) {
			number[i] = 0.0f;
		}
		
		Thread t1 = new ThreadFloat(number);
		Thread t2 = new ThreadFloat(number);
		Thread t3 = new ThreadFloat(number);
		Thread t4 = new ThreadFloat(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		float sum = 0.0f;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}
		
		System.out.println("Test Float");
		System.out.println("\tExpected: " + 500000.0f*4.0f);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testLong() throws Exception {
		long[] number = new long[1000];
		for(int i = 0; i < number.length; i++) {
			number[i] = 0L;
		}
		Thread t1 = new ThreadLong(number);
		Thread t2 = new ThreadLong(number);
		Thread t3 = new ThreadLong(number);
		Thread t4 = new ThreadLong(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		long sum = 0L;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}
		
		System.out.println("Test Long");
		System.out.println("\tExpected: " + 500000L*4L);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testDouble() throws Exception {
		double[] number = new double[1000];
		for(int i = 0; i < number.length; i++) {
			number[i] = 0.0;
		}
		
		Thread t1 = new ThreadDouble(number);
		Thread t2 = new ThreadDouble(number);
		Thread t3 = new ThreadDouble(number);
		Thread t4 = new ThreadDouble(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		double sum = 0.0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}
		
		System.out.println("Test Double");
		System.out.println("\tExpected: " + 500000.0*4.0);
		System.out.println("\tObtained: " + sum + "\n");
	}
	
	public static void testRef() throws Exception {
		Integer[] number = new Integer[1000];
		for(int i = 0; i < number.length; i++) {
			number[i] = new Integer(0);
		}
		
		Thread t1 = new ThreadRef(number);
		Thread t2 = new ThreadRef(number);
		Thread t3 = new ThreadRef(number);
		Thread t4 = new ThreadRef(number);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		int sum = 0;
		for(int i = 0; i < number.length; i++) {
			sum += number[i];
		}
		
		System.out.println("Test Ref");
		System.out.println("\tExpected: " + 500000*4);
		System.out.println("\tObtained: " + sum + "\n");
	}
}