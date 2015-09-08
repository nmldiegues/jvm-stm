package t1;

import avestm.Atomic;

public class ThreadShort extends Thread{

    private ShortHolder holder;

    public ThreadShort(ShortHolder holder){
	this.holder = holder;
    }

    @Atomic public void run(){
	int i;
	short tmp;
	for(i = 0; i < 5000; i++){
	    tmp = holder.getNumber();
	    tmp = (short)(tmp + 1);
	    holder.setNumber(tmp);
	}
    }

}
