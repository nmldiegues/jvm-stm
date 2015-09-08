package t6;

import avestm.Atomic;

public class ThreadDouble extends Thread{

    private DoubleHolder holder;

    public ThreadDouble(DoubleHolder holder){
	this.holder = holder;
    }

    public void run(){
    	doIt();
    }
    
    public void doIt() {
    	moreIndirection();
    }
    
    @Atomic
    public void moreIndirection() {
    	int i;
    	double tmp;
    	for(i = 0; i < 500000; i++){
    	    tmp = holder.getNumber();
    	    tmp = tmp + 1.0;
    	    holder.setNumber(tmp);
    	}
    }

}
