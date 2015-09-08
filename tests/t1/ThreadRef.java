package t1;

import avestm.Atomic;

public class ThreadRef extends Thread{

	private RefHolder holder;

	public ThreadRef(RefHolder holder){
		this.holder = holder;
	}

	@Atomic public void run(){
		Integer tmp;
		for(int i = 0; i < 500000; i++){
			holder.setNumber(new Integer(holder.getNumber() + 1));
		}
		
		//holder.setNumber(new Integer(holder.getNumber() + 1));
//		somemethod();
//		double a = 0.0;
//		System.out.println("uma constante" + "outra constante" + "outra");
//		a = 1.0 + 5.0 * 2.0 / 1.0;
//		a = 1.0 + 5.0 * 2.0 / 1.0;
//		a = 1.0 + 5.0 * 2.0 / 1.0;
//		a = 1.0 + 5.0 * 2.0 / 1.0;
		
//		holder.setNumber(new SpecialInt(holder.getNumber() + 1));
//		
//		char[][] somearray = new char[500000][1];
//		for(i = 0; i < somearray.length; i++) {
//			somearray[i] = new char[1];
//			somearray[i][0] = 'z';
//		}
	}
	
//	private void somemethod() {
//		holder.setNumber(new SpecialInt(holder.getNumber() + 1));
//	}

}
