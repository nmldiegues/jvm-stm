package t11;

public class LinkedItem {
	private LinkedItem previous;
	private LinkedItem next;
	
	private final int id;

	public LinkedItem(int id){
		this.id = id;
	}
	
	public LinkedItem getPrevious() {
		return previous;
	}

	public void setPrevious(LinkedItem previous) {
		this.previous = previous;
	}

	public LinkedItem getNext() {
		return next;
	}

	public void setNext(LinkedItem next) {
		this.next = next;
	}

	public int getId() {
		return id;
	}
	
}

