package t13;

public class DummyClass extends DummySuperClass {

        private int units;
        
        public DummyClass(int units) {
                this.units = units;
        }
        
        @Override
        public void inc() {
                for(int i = 0; i < units; i++) {
                        super.inc();
                } 
        }
}