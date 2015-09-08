package t1;

public class TestLong{

    public static void main(String[] args){
	LongHolder holder = new LongHolder(0L);
	Thread t1 = new ThreadLong(holder);
	Thread t2 = new ThreadLong(holder);
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += holder.getNumber();
	System.out.println(res);
    }



}
