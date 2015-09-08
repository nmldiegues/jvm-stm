package t3;

public class TestFloat{

    public static void main(String[] args){
	FloatHolder.number = 0.0f;
	Thread t1 = new ThreadFloat();
	Thread t2 = new ThreadFloat();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += FloatHolder.number;
	System.out.println(res);
    }



}
