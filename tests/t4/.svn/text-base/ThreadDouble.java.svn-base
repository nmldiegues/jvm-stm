package t4;

import avestm.Atomic;

public class ThreadDouble extends Thread{

    private double[] number;

    public ThreadDouble(double[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	double tmp;
    	for(i = 0; i < 500; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = tmp + 1.0;
    			number[j] = tmp;
    		}
    	}
    }

}
