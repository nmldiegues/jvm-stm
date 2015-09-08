package t2;

public class TestInt{

    public static void main(String[] args){
	IntHolder holder = new IntHolder();
	holder.number = 0;
	Thread t1 = new ThreadInt(holder);
	Thread t2 = new ThreadInt(holder);
	
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
