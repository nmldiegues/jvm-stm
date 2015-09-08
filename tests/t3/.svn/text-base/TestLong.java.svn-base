package t3;

public class TestLong{

    public static void main(String[] args){
	LongHolder.number = 0L;
	Thread t1 = new ThreadLong();
	Thread t2 = new ThreadLong();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += LongHolder.number;
	System.out.println(res);
    }



}
