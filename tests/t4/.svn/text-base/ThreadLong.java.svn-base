package t4;

import avestm.Atomic;

public class ThreadLong extends Thread{

    private long[] number;

    public ThreadLong(long[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	long tmp;
    	for(i = 0; i < 500; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = tmp + 1L;
    			number[j] = tmp;
    		}
    	}
    }

}
