package t4;

public class TestLong{

    public static void main(String[] args){
	long[] number = new long[1];
	number[0] = 0L;
	Thread t1 = new ThreadLong(number);
	Thread t2 = new ThreadLong(number);
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	    }catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += number[0];
	System.out.println(res);
    }



}
