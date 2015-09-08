package t4;

import avestm.Atomic;

public class ThreadShort extends Thread{

    private short[] number;

    public ThreadShort(short[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	short tmp;
    	for(i = 0; i < 50; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = (short)(tmp + 1);
    			number[j] = tmp;
    		}
    	}
    }

}
