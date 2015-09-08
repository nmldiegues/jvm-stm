package t4;

public class TestDouble{

    public static void main(String[] args){
	double[] number = new double[1];
	number[0] = 0.0;
	Thread t1 = new ThreadDouble(number);
	Thread t2 = new ThreadDouble(number);
	
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
