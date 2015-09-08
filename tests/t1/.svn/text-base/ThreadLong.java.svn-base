package t1;

import avestm.Atomic;

public class ThreadLong extends Thread{

    private LongHolder holder;

    public ThreadLong(LongHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	long tmp;
	for(i = 0; i < 500000; i++){
	    tmp = holder.getNumber();
	    tmp = tmp + 1L;
	    holder.setNumber(tmp);
	}
    }

}
