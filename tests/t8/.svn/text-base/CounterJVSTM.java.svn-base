package t8;

import jvstm.Transaction;
import jvstm.TransactionalCommand;
import jvstm.VBox;

public class CounterJVSTM implements Counter {
	private final VBox<Long> count = new VBox<Long>(0L);

	public long getCount() {
		return count.get();
	}

	public void inc() {
		count.put(getCount() + 1);
	}
}