package t4;

public class TestInt{

    public static void main(String[] args){
	int[] number = new int[1];
	number[0] = 0;
	Thread t1 = new ThreadInt(number);
	Thread t2 = new ThreadInt(number);
	
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
