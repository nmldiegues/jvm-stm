package t3;

import avestm.Atomic;

public class ThreadFloat extends Thread{

    @Atomic public void run(){
	int i;
	float tmp;
	for(i = 0; i < 500000; i++){
	    tmp = FloatHolder.number;
	    tmp = tmp + 1.0f;
	    FloatHolder.number = tmp;
	}
    }

}
