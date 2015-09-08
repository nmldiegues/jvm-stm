package t4;

public class TestRef{

    public static void main(String[] args){
	Integer[] number = new Integer[1];
	number[0] = new Integer(0);
	Thread t1 = new ThreadRef(number);
	Thread t2 = new ThreadRef(number);
	
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
