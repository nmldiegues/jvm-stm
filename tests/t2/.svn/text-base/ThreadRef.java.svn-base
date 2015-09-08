package t2;

import avestm.Atomic;

public class ThreadRef extends Thread{

    private RefHolder holder;

    public ThreadRef(RefHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	Integer tmp;
	for(i = 0; i < 500000; i++){
	    tmp = holder.number;
	    tmp = new Integer(tmp + 1);
	    holder.number = tmp;
	}
    }

}
