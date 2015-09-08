package t4;

import avestm.Atomic;

public class ThreadChar extends Thread{

    private char[] number;

    public ThreadChar(char[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	char tmp;
    	for(i = 0; i < 50; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = (char)(tmp + (char)1);
    			number[j] = tmp;
    		}
    	}
    }

}
