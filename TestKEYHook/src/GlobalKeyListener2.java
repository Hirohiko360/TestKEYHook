public class GlobalKeyListener2 {
    static {
        System.loadLibrary("GlobalKeyListener2");
    }

    public native void startListening();

    public static void main(String[] args) {
        GlobalKeyListener2 listener = new GlobalKeyListener2();
        listener.startListening();
    }
}

