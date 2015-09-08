package t2;

public class TestFloat{

    public static void main(String[] args){
	FloatHolder holder = new FloatHolder();
	holder.number = 0.0f;
	Thread t1 = new ThreadFloat(holder);
	Thread t2 = new ThreadFloat(holder);
	
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
