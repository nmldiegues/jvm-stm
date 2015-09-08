package t7;

public class TestInt{

    public static void main(String[] args){
	IntHolder holder = new IntHolder(0);
	Thread t1 = new ThreadInt(holder, true);
	Thread t2 = new ThreadInt(holder, false);
	
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
