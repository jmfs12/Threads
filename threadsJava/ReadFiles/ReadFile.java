import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Lock;

public class ReadFile extends Thread{
    private final Lock lock = new ReentrantLock();
    public void run(String word, int n){
        lock.lock();
        GlobalData.fileID++;
        lock.unlock();

        String nameFile = (char) GlobalData.fileID + ".txt";


    }
}
