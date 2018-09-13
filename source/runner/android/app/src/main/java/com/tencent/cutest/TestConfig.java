package com.tencent.cutest;

import android.content.Context;
import android.util.Xml;
import android.widget.Toast;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.io.InputStream;

class TestConfig {
    private static final TestConfig test_config = new TestConfig();

    static TestConfig getInstance() {
        return test_config;
    }

    protected String title;

    private TestConfig() {
    }

    public boolean load(Context context) {
        this.title = context.getString(R.string.app_name);

        // 获得assets下test_config.xml的输入流
        InputStream input_stream = null;

        try {
            input_stream = context.getAssets().open("test_config.xml");
        } catch (IOException e) {
            // 对应this.getAssets().open()
            e.printStackTrace();
            return false;
        }

        // 创建XmlPullParser对象来解析xml
        XmlPullParser parser = Xml.newPullParser();
        try {
            parser.setInput(input_stream, "UTF-8");
        } catch (XmlPullParserException e) {
            // 对应parser.setInput()
            e.printStackTrace();
            return false;
        }

        try {
            int event_type = parser.getEventType();

            while (event_type != XmlPullParser.END_DOCUMENT) {
                if (event_type == XmlPullParser.START_TAG) {
                    // 处理名为root的元素
                    if (parser.getName().equals("root")) {
                        int count = parser.getAttributeCount();
                        for (int i = 0; i < count; ++i) {
                            String attr_name = parser.getAttributeName(i);
                            if (attr_name.equals("title")) {
                                this.title = parser.getAttributeValue(i);
                            }
                        }
                    }

                    // 处理名为test的元素
                    if (parser.getName().equals("test")) {
                        int count = parser.getAttributeCount();
                        for (int i = 0; i < count; ++i) {
                            String attr_name = parser.getAttributeName(i);
                            if (attr_name.equals("libName")) {
                                // 根据libName的配置来加载so
                                String lib_name = parser.getAttributeValue(i);
                                try {
                                    System.loadLibrary(lib_name);
                                } catch (Throwable e) {
                                    String text = String.format(
                                            context.getString(R.string.load_so_error_format),
                                            lib_name);
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
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        try {
            input_stream.close();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public String getTitle() {
        return this.title;
    }
}
