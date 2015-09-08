package t5;

public class ThreadedTest{

    public static void main(String[] args){
	short s = 1;
	byte by = 1;
	Holder holder = new Holder(new Integer(1),
				   1.0d,
				   1.0f,
				   1L,
				   1,
				   s,
				   'a',
				   true,
				   by);
	Thread t1 = new ThreadExample(holder);
	Thread t2 = new ThreadExample(holder);
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	    }catch(InterruptedException e) {}
	
	System.out.println(holder.toString());
    }



}