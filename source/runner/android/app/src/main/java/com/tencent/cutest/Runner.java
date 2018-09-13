
package com.tencent.cutest;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Xml;
import android.widget.Toast;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.io.InputStream;

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

    public static void loadTestConfig(Context context) {
        // 获得assets下test_config.xml的输入流
        InputStream input_stream = null;

        try {
            input_stream = context.getAssets().open("test_config.xml");
        } catch (IOException e) {
            // 对应this.getAssets().open()
            e.printStackTrace();
            return;
        }

        // 创建XmlPullParser对象来解析xml
        XmlPullParser parser = Xml.newPullParser();
        try {
            parser.setInput(input_stream, "UTF-8");
        } catch (XmlPullParserException e) {
            // 对应parser.setInput()
            e.printStackTrace();
            return;
        }

        try {
            int event_type = parser.getEventType();

            while (event_type != XmlPullParser.END_DOCUMENT) {
                if (event_type == XmlPullParser.START_TAG) {
                    // 处理名为test的元素
                    if (parser.getName().equals("test")) {
                        int count = parser.getAttributeCount();
                        for (int i = 0; i < count; ++i) {
                            String attrName = parser.getAttributeName(i);
                            if (attrName.equals("libName")) {
                                // 根据libName的配置来加载so
                                String libName = parser.getAttributeValue(i);
                                try {
                                    System.loadLibrary(libName);
                                } catch (Throwable e) {
                                    String text = String.format(
                                            context.getString(R.string.load_so_error_format),
                                            libName);
                                    Toast.makeText(context, text, Toast.LENGTH_LONG).show();
                                }
                            }
                        }
                    }
                }

                event_type = parser.next();
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
            return;
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        try {
            input_stream.close();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
    }
}
