package t12;

public class DistributedCounter implements ICounter{
        private static int count;
        
        @avestm.Atomic
        public void inc() {
                count++;
        }

        @avestm.Atomic
        public static int getCount() {
                return count;
        }
}