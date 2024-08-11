public class GlobalKeyListener2 {
    static {
        System.loadLibrary("GlobalKeyListener2");
    }

    public static void main(String[] args) {
        GlobalKeyListener listener2 = new GlobalKeyListener();
        listener2.startListening();
    }
}