package t3;

import avestm.Atomic;

public class ThreadLong extends Thread{

    @Atomic 
    public void run(){
	int i;
	long tmp;
	for(i = 0; i < 500000; i++){
	    tmp = LongHolder.number;
	    tmp = tmp + 1L;
	    LongHolder.number = tmp;
	}
    }

}
