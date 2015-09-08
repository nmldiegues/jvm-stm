package t2;

import avestm.Atomic;

public class ThreadFloat extends Thread{

    private FloatHolder holder;

    public ThreadFloat(FloatHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	float tmp;
	for(i = 0; i < 500000; i++){
	    tmp = holder.number;
	    tmp = tmp + 1.0f;
	    holder.number = tmp;
	}
    }

}
