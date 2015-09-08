package t9;

import jvstm.VBox;

public class CounterJVSTM implements Counter {
	private final VBox<Long> count = new VBox<Long>(0L);

	@Override
	public long getCount() {
		return count.get();
	}

	@Override
	public void inc() {
		count.put(getCount() + 1);
	}
}