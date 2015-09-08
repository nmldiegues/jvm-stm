package t6;

public class IntHolder{

    private int number;

    public IntHolder(int number){
	this.number = number;
    }

    public int getNumber(){
	return number;
    }

    public void setNumber(int number){
    	setNumber2(number);
    }
    
    public void setNumber2(int number){
    	setNumber3(number);
    }

    public void setNumber3(int number){
    	setNumber4(number);
    }
    
    public void setNumber4(int number){
    	this.number = number;
    }
}