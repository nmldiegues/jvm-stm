package t7;

import avestm.Atomic;

public class ThreadInt extends Thread{

	private IntHolder holder;
	private boolean catchExc;

	public ThreadInt(IntHolder holder, boolean catchExc){
		this.holder = holder;
		this.catchExc = catchExc;
	}

	@Atomic
	public void run(){
		int i;
		int tmp;
		for(i = 0; i < 500000; i++){
			tmp = holder.getNumber();
			tmp = tmp + 1;
			holder.setNumber(tmp);
		}
		if(catchExc) {
			try {
				holder.throwException();
			}
			catch(RuntimeException e) {

			}
		}
		else {
			holder.throwException();
		}
	}

}
