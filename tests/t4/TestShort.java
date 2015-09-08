package t4;

public class TestShort{

    public static void main(String[] args){
    short k = 0;
    short[] number = new short[1];
	number[0] = k;
	Thread t1 = new ThreadShort(number);
	Thread t2 = new ThreadShort(number);
	
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
