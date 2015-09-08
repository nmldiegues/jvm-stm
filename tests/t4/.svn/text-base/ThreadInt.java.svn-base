package t4;

import avestm.Atomic;

public class ThreadInt extends Thread{

    private int[] number;

    public ThreadInt(int[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	int tmp;
    	for(i = 0; i < 500; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = tmp + 1;
    			number[j] = tmp;
    		}
    	}
    }

}
