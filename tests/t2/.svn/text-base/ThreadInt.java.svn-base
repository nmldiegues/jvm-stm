package t2;

import avestm.Atomic;

public class ThreadInt extends Thread{

    private IntHolder holder;

    public ThreadInt(IntHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	int tmp;
	for(i = 0; i < 500000; i++){
	    tmp = holder.number;
	    tmp = tmp + 1;
	    holder.number = tmp;
	}
    }

}
