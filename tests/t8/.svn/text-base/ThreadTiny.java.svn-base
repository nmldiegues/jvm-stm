package t8;

import avestm.Atomic;

public class ThreadTiny extends Thread{

	private Counter counter;
	private int loops;

	public ThreadTiny(Counter counter, int loops) {
		this.counter = counter;
		this.loops = loops;
	}

	@Atomic
	public void run() {
		for(int i = 0; i < loops; i++) {
			counter.inc();
		}
	}
}