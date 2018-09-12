
package com.tencent.cutest;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Xml;

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
        InputStream inputStream = null;

        try {
            inputStream = context.getAssets().open("test_config.xml");
        } catch (IOException e) {
            // 对应this.getAssets().open()
            e.printStackTrace();
            return;
        }

        // 创建XmlPullParser对象来解析xml
        XmlPullParser parser = Xml.newPullParser();
        try {
            parser.setInput(inputStream, "UTF-8");
        } catch (XmlPullParserException e) {
            // 对应parser.setInput()
            e.printStackTrace();
            return;
        }

        try {
            int eventType = parser.getEventType();

            while (eventType != XmlPullParser.END_DOCUMENT) {
                if (eventType == XmlPullParser.START_TAG) {
                    // 处理名为test的元素
                    if (parser.getName().equals("test")) {
                        int count = parser.getAttributeCount();
                        for (int i = 0; i < count; ++i) {
                            String attrName = parser.getAttributeName(i);
                            if (attrName.equals("libName")) {
                                // 根据libName的配置来加载so
                                String libName = parser.getAttributeValue(i);
                                System.loadLibrary(libName);
                            }
                        }
                    }
                }

                eventType = parser.next();
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
            return;
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        try {
            inputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
    }
}
