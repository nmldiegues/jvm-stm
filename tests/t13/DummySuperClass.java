package t13;

public class DummySuperClass {

        static private int counter;
        
        @avestm.Atomic
        public int getCounter() {
                return counter;
        }
        
        @avestm.Atomic
        public void inc() {
                counter++;
        }
}