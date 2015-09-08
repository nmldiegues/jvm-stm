package t1;

public class TestRef{

    public static void main(String[] args){
	RefHolder holder = new RefHolder(new SpecialInt(0));
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
	res += holder.getSpecial().toString();
	System.out.println(res);
    }



}
