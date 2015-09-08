package t7;

public class IntHolder{

    private int number;

    public IntHolder(int number){
	this.number = number;
    }

    public int getNumber(){
	return number;
    }

    public void setNumber(int number){
	this.number = number;
    }
    
    public void throwException() {
    	throw new RuntimeException();
    }

}