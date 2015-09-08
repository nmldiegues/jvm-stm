package t5;

import avestm.Atomic;

public class ThreadSC extends Thread{

    private SCHolder holder;

    public ThreadSC(SCHolder holder){
	this.holder = holder;
    }

	@Atomic
    public void run(){
    	//holder.s1 = (short)(holder.s1 + (short)1);
    	holder.c1 = (char)(holder.c1 + 1);
    	//char x = holder.c1;
    	//holder.c2 = (char)(holder.c2 + 1);
    	//holder.s2 = (short)(holder.s2 + (short)1);
    	//holder.c3 = (char)(holder.c3 + 1);
    	//holder.setS((short)(holder.getS() + (short)1));
    	//holder.setC((char)(holder.getC() + 1));
    }

}