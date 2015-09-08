package t8;

public class CounterTiny implements Counter{
    private long count = 0L;

    public long getCount() {
        return count;
    }

    public void inc() {
        count += 1L;
    }
}