package t5;

public class FieldsHolder{

	public String toString(){
		return new String("" + 
				"LOLSIZE i1: " + i1 + "\n" +
				"LOLSIZE i2: " + i2 + "\n" +
				"LOLSIZE l1: " + l1 + "\n" +
				"LOLSIZE i3: " + i3 + "\n" +
				"LOLSIZE i4: " + i4 + "\n" + 
				"LOLSIZE f1: " + f1 + "\n" + 
				"LOLSIZE i5: " + i5 + "\n" + 
				"LOLSIZE d1: " + d1 + "\n" + 
				"LOLSIZE l2: " + l2 + "\n" + 
				"LOLSIZE i6: " + i6 + "\n" + 
				"LOLSIZE f2: " + f2 + "\n" +
				"LOLSIZE f3: " + f3 + "\n" +
				"LOLSIZE s1: " + s1 + "\n" +
				"LOLSIZE c1: " + c1 + "\n");
	}

	public int i1;
	public int i2;
	public long l1;
	public int i3;
	public int i4;
	public float f1;
	public int i5;
	public double d1;
	public long l2;
	public int i6;
	public float f2;
	public float f3;
	public char c1;
	public short s1;

	public FieldsHolder(){
		this.i1 = 0;
		this.i2 = 0;
		this.l1 = 0L;
		this.i3 = 0;
		this.i4 = 0;
		this.f1 = 0.0f;
		this.i5 = 0;
		this.d1 = 0.0;
		this.l2 = 0L;
		this.i6 = 0;
		this.f2 = 0.0f;
		this.c1 = 'a';
		this.f3 = 0.0f;
		this.s1 = (short)2;
	}


	public int getI1(){
		return this.i1;
	}

	public void setI1(int i){
		this.i1 = i;
	}
	
	public int getI2(){
		return this.i2;
	}

	public void setI2(int i){
		this.i2 = i;
	}

	public int getI3(){
		return this.i3;
	}

	public void setI3(int i){
		this.i3 = i;
	}
	public int getI4(){
		return this.i4;
	}

	public void setI4(int i){
		this.i4 = i;
	}
	
	public long getL1(){
		return this.l1;
	}

	public void setL1(long l){
		this.l1 = l;
	}
	
	public long getL2(){
		return this.l2;
	}

	public void setL2(long l){
		this.l2 = l;
	}
	
	public void setI5(int i){
		this.i5 = i;
	}
	
	public int getI5(){
		return this.i5;
	}
	
	public void setI6(int i){
		this.i6 = i;
	}
	
	public int getI6(){
		return this.i6;
	}
	
	public void setD1(double d){
		this.d1 = d;
	}
	
	public double getD1(){
		return this.d1;
	}
	
	public void setF1(float f){
		this.f1 = f;
	}
	
	public float getF1(){
		return this.f1;
	}
	
	public void setF2(float f){
		this.f2 = f;
	}
	
	public float getF2(){
		return this.f2;
	}
	public void setF3(float f){
		this.f3 = f;
	}
	
	public float getF3(){
		return this.f3;
	}
	
	public short getS1() {
		return this.s1;
	}
	
	public void setS1(short s) {
		s1 = s;
	}
	
	public char getC1() {
		return this.c1;
	}
	
	public void setC1(char c) {
		c1 = c;
	}
}