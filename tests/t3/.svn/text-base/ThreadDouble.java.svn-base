package t3;

import avestm.Atomic;

public class ThreadDouble extends Thread{

    @Atomic public void run(){
	int i;
	double tmp;
	for(i = 0; i < 500000; i++){
	    tmp = DoubleHolder.number;
	    tmp = tmp + 1.0;
	    DoubleHolder.number = tmp;
	}
    }

}
