package t5;

/**
 * Tests that the layout of a complex object is not broken with 
 * cross-slot-boundaries within writes or reads using the STM.
 */
public class TestAll{

	public static void main(String[] args) throws Exception{
		FieldsHolder holder = new FieldsHolder();

		Thread t1 = new ThreadFields(holder);
		Thread t2 = new ThreadFields(holder);
		Thread t3 = new ThreadFields(holder);
		Thread t4 = new ThreadFields(holder);

		t1.start();
		t2.start();
		t3.start();
		t4.start();
		
		t1.join();
		t2.join();
		t3.join();
		t4.join();
	
		System.out.println("Expected vs Obtained");
		System.out.println("i1: 100 vs " + holder.i1);
		System.out.println("i2: 100 vs " + holder.i2);
		System.out.println("l1: 100 vs " + holder.l1);
		System.out.println("i3: 100 vs " + holder.i3);
		System.out.println("i4: 100 vs " + holder.i4);
		System.out.println("f1: 0.0 vs " + holder.f1);
		System.out.println("i5: 0 vs " + holder.i5);
		System.out.println("d1: 100.0 vs " + holder.d1);
		System.out.println("l2: 0 vs " + holder.l2);
		System.out.println("i6: 0 vs " + holder.i6);
		System.out.println("f2: 0.0 vs " + holder.f2);
		System.out.println("c1: a vs " + holder.c1);
		System.out.println("f3: 100.0 vs " + holder.f3);
		System.out.println("s1: 102 vs " + holder.s1);
	}

}