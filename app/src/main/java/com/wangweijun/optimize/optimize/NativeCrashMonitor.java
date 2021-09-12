package com.wangweijun.optimize.optimize;

public class NativeCrashMonitor {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public void init(){

        nativeInit();

        nativeSetup();
    }

    public void nofityCondition(){

        nativeNofityCondition();
    }

    private native void nativeInit();

    private native void nativeSetup();

    public static native final void nativeCrash();

    private native void nativeNofityCondition();
}
