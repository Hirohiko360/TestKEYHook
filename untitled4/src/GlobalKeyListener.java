public class GlobalKeyListener {
    static {
        System.loadLibrary("GlobalKeyListener");
    }

    public native void startListening();

    public static void main(String[] args) {
        GlobalKeyListener listener = new GlobalKeyListener();
        listener.startListening();
    }
}