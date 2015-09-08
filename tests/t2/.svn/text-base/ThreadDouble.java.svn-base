package t2;

import avestm.Atomic;

public class ThreadDouble extends Thread{

    private DoubleHolder holder;

    public ThreadDouble(DoubleHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	double tmp;
	for(i = 0; i < 500000; i++){
	    tmp = holder.number;
	    tmp = tmp + 1.0;
	    holder.number = tmp;
	}
    }

}
