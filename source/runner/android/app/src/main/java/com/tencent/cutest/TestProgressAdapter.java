
package com.tencent.cppunit;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class TestProgressAdapter extends BaseAdapter {
    public class ItemInfo {
        public String mText;

        public static final int END_TEST_OK = -1; // 测试通过
        public static final int END_TEST_FAIL = -2; // 测试失败
        public int mIndex; // 负数用来表示特殊的Item，其他值表示测试结果索引

        public ItemInfo(String text, int index) {
            mText = text;
            mIndex = index;
        }
    }

    Context mContext;
    ArrayList<ItemInfo> mItemInfoList;

    public TestProgressAdapter(Context context) {
        mContext = context;
        mItemInfoList = new ArrayList<ItemInfo>();
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ItemInfo item = mItemInfoList.get(position);
        if (item == null)
            return null;

        if (convertView == null) {
            LayoutInflater service = (LayoutInflater) mContext
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = service.inflate(R.layout.list_item, parent, false);
        }
        TextView textView = (TextView) convertView.findViewById(R.id.text_view);
        textView.setText(item.mText);
        return convertView;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public int getCount() {
        return mItemInfoList.size();
    }

    @Override
    public Object getItem(int position) {
        return mItemInfoList.get(position);
    }

    public void endTestPassed(String test_name, int elapsed_ms) {
        String text = String.format(mContext.getString(R.string.end_test_passed_format),
                test_name, elapsed_ms);
        ItemInfo item = new ItemInfo(text, ItemInfo.END_TEST_OK);
        mItemInfoList.add(item);
        notifyDataSetChanged();
    }

    public void endTestFailed(String test_name, int error_count, int failure_count) {
        String text = String.format(mContext.getString(R.string.end_test_failed_format),
                test_name, error_count, failure_count);
        ItemInfo item = new ItemInfo(text, ItemInfo.END_TEST_FAIL);
        mItemInfoList.add(item);
        notifyDataSetChanged();
    }

    public void addFailure(int index, boolean is_error, String file_name, int line_number) {
        String text;
        if (is_error) {
            text = String.format(mContext.getString(R.string.error_source_line_format),
                    file_name, line_number);
        } else {
            text = String.format(
                    mContext.getString(R.string.failure_source_line_format),
                    file_name, line_number);
        }
        ItemInfo item = new ItemInfo(text, index);
        mItemInfoList.add(item);
        notifyDataSetChanged();
    }

    public void reset(int minimum_capacity) {
        mItemInfoList.clear();
        if (minimum_capacity != 0)
            mItemInfoList.ensureCapacity(minimum_capacity);
        notifyDataSetChanged();
    }
}
