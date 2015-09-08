package t5;

import avestm.Atomic;

public class ThreadFields extends Thread{

	private FieldsHolder holder;

	public ThreadFields(FieldsHolder holder){
		this.holder = holder;
	}

	@Atomic
	public void run(){
		for(int i = 0; i < 25; i++) {
			holder.setI1(holder.getI1() + 1);
			holder.setI2(holder.getI2() + 1);
			holder.setL1(holder.getL1() + 1L);
			holder.setI3(holder.getI3() + 1);
			holder.setI4(holder.getI4() + 1);
			holder.setD1(holder.getD1() + 1.0);
			holder.setF3(holder.getF3() + 1.0f);
			holder.setS1((short)(holder.getS1() + 1));
		}
	}

}