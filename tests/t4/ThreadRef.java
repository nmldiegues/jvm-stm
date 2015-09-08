package t4;

import avestm.Atomic;

public class ThreadRef extends Thread{

    private Integer[] number;

    public ThreadRef(Integer[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	Integer tmp;
    	for(i = 0; i < 500; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = new Integer(tmp + 1);
    			number[j] = tmp;
    		}
    	}
    }

}
