package t3;

import avestm.Atomic;

public class ThreadInt extends Thread{

    @Atomic public void run(){
	int i;
	int tmp;
	for(i = 0; i < 500000; i++){
	    tmp = IntHolder.number;
	    tmp = tmp + 1;
	    IntHolder.number = tmp;
	}
    }

}
