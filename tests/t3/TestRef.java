package t3;

public class TestRef{

    public static void main(String[] args){
	RefHolder.number = new Integer(0);
	Thread t1 = new ThreadRef();
	Thread t2 = new ThreadRef();
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	}catch(InterruptedException e) {}
	
	String res = "LOLSIZE: ";
	res += RefHolder.number;
	System.out.println(res);
    }



}
