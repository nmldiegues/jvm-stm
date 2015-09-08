package t6;

import avestm.Atomic;

public class ThreadInt extends Thread{

    private IntHolder holder;

    public ThreadInt(IntHolder holder){
	this.holder = holder;
    }

    public void run(){
    	doIt();
    }

    @Atomic
    public void doIt() {
    	moreIndirection();
    }
    
    public void moreIndirection() {
    	int i;
    	int tmp;
    	for(i = 0; i < 500000; i++){
    	    tmp = holder.getNumber();
    	    tmp = tmp + 1;
    	    holder.setNumber(tmp);
    	}
    }
}
