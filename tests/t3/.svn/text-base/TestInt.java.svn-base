package t3;

public class TestInt{

    public static void main(String[] args){
	IntHolder.number = 0;
	Thread t1 = new ThreadInt();
	Thread t2 = new ThreadInt();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += IntHolder.number;
	System.out.println(res);
    }



}
