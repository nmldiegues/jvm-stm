package t9;

class VAccountTiny implements Account {
    private long balance;

    VAccountTiny(long balance) {
        setBalance(balance);
    }

    @Override
	public long getBalance() {
        return balance;
    }

    void setBalance(long newBalance) {
        this.balance = newBalance;
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
