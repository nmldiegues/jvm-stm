package t4;

import avestm.Atomic;

public class ThreadByte extends Thread{

    private byte[] number;

    public ThreadByte(byte[] number){
	this.number = number;
    }

    @Atomic
    public void run(){
    	int i;
    	byte tmp;
    	for(i = 0; i < 25; i++){
    		for(int j = 0; j < number.length; j++) {
    			tmp = number[j];
    			tmp = (byte)(tmp + (byte)1);
    			number[j] = tmp;
    		}
    	}
    }

}
