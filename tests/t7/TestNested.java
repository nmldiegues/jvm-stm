package t7;

import avestm.Atomic;

public class TestNested{

    public static void main(String[] args){
    	IntHolder holder = new IntHolder(0);
    	doIt(holder);
    	String res = "LOLSIZE: ";
    	res += holder.getNumber();
    	System.out.println(res);
    }
    
    @Atomic
    public static void doIt(IntHolder holder) {
    	Thread t1 = new ThreadNested(holder, true);
    	Thread t2 = new ThreadNested(holder, false);
    	
    	//t1.start();
    	//t2.start();
//    	try{
//    	    t1.join();
//    	} catch(InterruptedException e){}
    	
    	/*try{
    	    t2.join();
    	}catch(InterruptedException e) {}*/
    	
    }

}
