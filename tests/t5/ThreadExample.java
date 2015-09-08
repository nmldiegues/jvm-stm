package t5;

import avestm.Atomic;

public class ThreadExample extends Thread{

    private Holder holder;

    public ThreadExample(Holder holder){
	this.holder = holder;
    }

    @Atomic
    public void run(){
	holder.setO(holder.getO() + 1);
	holder.setD(holder.getD() + 1);
	holder.setF(holder.getF() + 1);
	holder.setL(holder.getL() + 1);
	holder.setC((char)(holder.getC() + 1));
	holder.setI(holder.getI() + 1);
	holder.setS((short)(holder.getS() + (short)1));
	holder.setB(!holder.getB());
	holder.setBy((byte)(holder.getBy() + (byte)1));
    }

}