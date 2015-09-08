package t3;

public class TestDouble{

    public static void main(String[] args){
	DoubleHolder.number = 0.0;
	Thread t1 = new ThreadDouble();
	Thread t2 = new ThreadDouble();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += DoubleHolder.number;
	System.out.println(res);
    }



}
