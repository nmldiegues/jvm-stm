package t9;

import jvstm.VBox;

class VAccountJVSTM implements Account {
    private final VBox<Long> balance = new VBox<Long>();

    VAccountJVSTM(long balance) {
        setBalance(balance);
    }

    @Override
	public long getBalance() {
        return balance.get();
    }

    void setBalance(long newBalance) {
        this.balance.put(newBalance);
    }

    @Override
    public void withdraw(final long amount) {
		setBalance(getBalance() - amount);
    }

    @Override
    public void deposit(final long amount) {
    	setBalance(getBalance() + amount);
    }

    @Override
	public boolean canWithdraw(long amount) {
        return amount < getBalance();
    }
}
