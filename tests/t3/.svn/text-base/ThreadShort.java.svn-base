package t3;

import avestm.Atomic;

public class ThreadShort extends Thread{

    @Atomic public void run(){
	int i;
	short tmp;
	for(i = 0; i < 5000; i++){
	    tmp = ShortHolder.number;
	    tmp = (short)(tmp + 1);
	    ShortHolder.number = tmp;
	}
    }

}
