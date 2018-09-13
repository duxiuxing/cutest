
package com.tencent.cutest;

import android.os.Handler;
import android.os.Looper;

public class Runner {

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("cutest");
    }

    public native static String version();

    public native static void start(ProgressListener listener);

    public native static void stop();

    protected native static void internalRun(long native_runnable,
                                             boolean is_auto_delete);

    public native static String failureDetails(int index);

    public static void asyncRunOnMainThread(long native_runnable, boolean is_auto_delete) {
        final long final_native_runnable = native_runnable;
        final boolean final_is_auto_delete = is_auto_delete;

        Looper looper = Looper.getMainLooper();
        Handler handler = new Handler(looper);
        handler.post(new Runnable() {
            @Override
            public void run() {
                internalRun(final_native_runnable, final_is_auto_delete);
            }
        });
    }

    public static void delayRunOnMainThread(
            long delay_ms,
            long native_runnable,
            boolean is_auto_delete) {
        final long final_native_runnable = native_runnable;
        final boolean final_is_auto_delete = is_auto_delete;

        Looper looper = Looper.getMainLooper();
        Handler handler = new Handler(looper);
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                internalRun(final_native_runnable, final_is_auto_delete);
            }
        }, delay_ms);
    }
}
