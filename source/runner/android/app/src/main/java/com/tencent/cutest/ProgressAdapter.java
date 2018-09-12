
package com.tencent.cutest;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class ProgressAdapter extends BaseAdapter {
    public class ItemInfo {
        public String text;

        public static final int END_TEST_OK = -1; // 测试通过
        public static final int END_TEST_FAIL = -2; // 测试失败
        public int index; // 负数用来表示特殊的Item，其他值表示测试结果索引

        public ItemInfo(String text, int index) {
            this.text = text;
            this.index = index;
        }
    }

    Context context;
    ArrayList<ItemInfo> item_info_list;

    public ProgressAdapter(Context context) {
        this.context = context;
        this.item_info_list = new ArrayList<ItemInfo>();
    }

    @Override
    public View getView(int position, View convert_view, ViewGroup parent) {
        ItemInfo item = this.item_info_list.get(position);
        if (item == null) {
            return null;
        }

        if (convert_view == null) {
            LayoutInflater service = (LayoutInflater) this.context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convert_view = service.inflate(R.layout.list_item, parent, false);
        }
        TextView text_view = (TextView) convert_view.findViewById(R.id.text_view);
        text_view.setText(item.text);
        return convert_view;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public int getCount() {
        return this.item_info_list.size();
    }

    @Override
    public Object getItem(int position) {
        return this.item_info_list.get(position);
    }

    public void endTestPassed(String test_name, int elapsed_ms) {
        String text = String.format(this.context.getString(R.string.end_test_passed_format),
                test_name, elapsed_ms);
        ItemInfo item = new ItemInfo(text, ItemInfo.END_TEST_OK);
        this.item_info_list.add(item);
        notifyDataSetChanged();
    }

    public void endTestFailed(String test_name, int error_count, int failure_count) {
        String text = String.format(this.context.getString(R.string.end_test_failed_format),
                test_name, error_count, failure_count);
        ItemInfo item = new ItemInfo(text, ItemInfo.END_TEST_FAIL);
        this.item_info_list.add(item);
        notifyDataSetChanged();
    }

    public void addFailure(int index, boolean is_error, String file_name, int line_number) {
        String text;
        if (is_error) {
            text = String.format(this.context.getString(R.string.error_source_line_format),
                    file_name, line_number);
        } else {
            text = String.format(this.context.getString(R.string.failure_source_line_format),
                    file_name, line_number);
        }
        ItemInfo item = new ItemInfo(text, index);
        this.item_info_list.add(item);
        notifyDataSetChanged();
    }

    public void reset(int minimum_capacity) {
        this.item_info_list.clear();
        if (minimum_capacity != 0) {
            this.item_info_list.ensureCapacity(minimum_capacity);
        }
        notifyDataSetChanged();
    }
}
