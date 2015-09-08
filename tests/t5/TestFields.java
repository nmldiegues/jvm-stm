package t5;

public class TestFields{

    public static void main(String[] args){
	FieldsHolder holder = new FieldsHolder();
	Thread t1 = new ThreadFields(holder);
	Thread t2 = new ThreadFields(holder);
	
	t1.start();
	t2.start();
	try{
	    t1.join();
	} catch(InterruptedException e){}
	
	try{
	    t2.join();
	    }catch(InterruptedException e) {}
	
	System.out.println(holder.toString());
    }



}