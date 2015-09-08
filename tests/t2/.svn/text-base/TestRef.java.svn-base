package t2;

public class TestRef{

    public static void main(String[] args){
	RefHolder holder = new RefHolder();
	holder.number = new Integer(0);
	Thread t1 = new ThreadRef(holder);
	Thread t2 = new ThreadRef(holder);
	
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
