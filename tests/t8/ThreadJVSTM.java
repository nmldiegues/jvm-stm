package t8;

import jvstm.Transaction;
import jvstm.TransactionalCommand;
import jvstm.VBox;

public class ThreadJVSTM extends Thread{

	private Counter counter;
	private int loops;

	public ThreadJVSTM(Counter counter, int loops) {
		this.counter = counter;
		this.loops = loops;
	}

	public void run() {
		Transaction.transactionallyDo(new TransactionalCommand() {
			public void doIt() {
				for(int i = 0; i < loops; i++) {
					counter.inc();
				}
			}
		});
	}
}