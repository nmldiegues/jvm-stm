package t3;

import avestm.Atomic;

public class ThreadRef extends Thread{

    @Atomic public void run(){
	int i;
	Integer tmp;
	for(i = 0; i < 500000; i++){
	    tmp = RefHolder.number;
	    tmp = new Integer(tmp + 1);
	    RefHolder.number = tmp;
	}
    }

}
