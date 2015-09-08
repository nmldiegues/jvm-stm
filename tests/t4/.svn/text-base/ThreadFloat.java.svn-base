package t4;

import avestm.Atomic;

public class ThreadFloat extends Thread{

    private float[] number;

    public ThreadFloat(float[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	float tmp;
    	for(i = 0; i < 500; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = tmp + 1.0f;
    			number[j] = tmp;
    		}
    	}
    }

}
