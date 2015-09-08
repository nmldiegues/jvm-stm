package t2;

public class TestDouble{

    public static void main(String[] args){
	DoubleHolder holder = new DoubleHolder();
	holder.number = 0.0;
	Thread t1 = new ThreadDouble(holder);
	Thread t2 = new ThreadDouble(holder);
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += holder.number;
	System.out.println(res);
    }



}
