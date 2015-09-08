package t3;

public class TestShort{

    public static void main(String[] args){
	short k = 0;
	ShortHolder.number = k;
	Thread t1 = new ThreadShort();
	Thread t2 = new ThreadShort();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += ShortHolder.number;
	System.out.println(res);
    }



}
