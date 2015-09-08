package t5;

public class TestSC{

	public static void main(String[] args){
		SCHolder holder = new SCHolder();
		holder.s1 = (short)1;
		holder.c1 = 'a';
		System.out.println(holder.toString());
		Thread t1 = new ThreadSC(holder);
		Thread t2 = new ThreadSC(holder);

		t1.start();
		t2.start();
		try{
			t1.join();
		} catch(InterruptedException e){}

		try{
	    t2.join();
	    }catch(InterruptedException e) {}

		System.out.println(holder.toString());
		//System.out.println("LOLSIZE: c1 " + holder.c1);
//		System.out.println("LOLSIZE: c2 " + holder.c2);
//		System.out.println("LOLSIZE: s2 " + holder.s2);
//		System.out.println("LOLSIZE: c3 " + holder.c3);
	}



}