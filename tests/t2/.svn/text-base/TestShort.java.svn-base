package t2;

public class TestShort{

    public static void main(String[] args){
	short k = 0;
	ShortHolder holder = new ShortHolder();
	holder.number = k;
	Thread t1 = new ThreadShort(holder);
	Thread t2 = new ThreadShort(holder);
	
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
