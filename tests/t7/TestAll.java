package t7;

/**
 * Tests exception handling or the lack of it (which causes aborts).
 * Three threads will catch the exception in the end of the increments, 
 * while one does not. Therefore only the work of the last 3 should 
 * be visible. 
 */
public class TestAll{
	
	public static void main(String[] args) throws Exception{
		
		IntHolder holder = new IntHolder(0);
		Thread t1 = new ThreadInt(holder, false);
		Thread t2 = new ThreadInt(holder, true);
		Thread t3 = new ThreadInt(holder, true);
		Thread t4 = new ThreadInt(holder, true);
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		System.out.println("Expected: " + 3*500000);
		System.out.println("Obtained: " + holder.getNumber() );
	}
	
}