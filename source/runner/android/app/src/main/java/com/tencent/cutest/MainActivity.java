
package com.tencent.cppunit;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TabHost;
import android.widget.TabWidget;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
        implements OnClickListener, TestProgressListener, OnItemClickListener {
    FloatingActionButton mButtonRun;
    long mTestStartTimeMs;
    long mTestEndTimeMs;

    ProgressBar mTestsProgress;
    int mTestsRun; // 记录已经执行的用例个数
    int mTestsCount; // 记录用例的总数
    int mErrors;
    int mFailures;

    ListView mListViewPassed;
    TestProgressAdapter mListAdapterPassed;

    ListView mListViewFailed;
    TestProgressAdapter mListAdapterFailed;

    public MainActivity() {
        mTestsRun = 0;
        mTestsCount = 0;
        mErrors = 0;
        mFailures = 0;

        mTestStartTimeMs = System.currentTimeMillis();
        mTestEndTimeMs = mTestStartTimeMs;
    }

    @Override
    public void onStartTestRun(int all_test_count) {
        mTestStartTimeMs = System.currentTimeMillis();
        updateTime();

        mTestsRun = 0;
        mTestsCount = all_test_count;
        mTestsProgress.setMax(mTestsCount);
        updateTestsRun();

        mErrors = 0;
        updateErrors();

        mFailures = 0;
        updateFailures();

        mListAdapterPassed.reset(all_test_count);
        mListAdapterFailed.reset(all_test_count);
    }

    @Override
    public void onEndTestRun(int elapsed_ms) {
        updateTime();
        updateTextProgress(this.getString(R.string.text_progress_none));
    }

    @Override
    public void onStartTest(String test_name) {
        updateTextProgress(test_name);
    }

    @Override
    public void onAddFailure(int index, boolean is_error, String file_name, int line_number) {
        if (is_error) {
            ++mErrors;
            updateErrors();
        } else {
            ++mFailures;
            updateFailures();
        }
        mListAdapterFailed.addFailure(index, is_error, file_name, line_number);
    }

    @Override
    public void onEndTest(String test_name, int error_count, int failure_count, int elapsed_ms) {
        updateTime();

        ++mTestsRun;
        updateTestsRun();

        if ((0 == error_count) && (0 == failure_count))
            mListAdapterPassed.endTestPassed(test_name, elapsed_ms);
        else
            mListAdapterFailed.endTestFailed(test_name, error_count, failure_count);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT); // 竖屏设置
        setContentView(R.layout.activity_main);

        // ActionBar
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        CharSequence subTitle = String.format(this.getString(R.string.sub_title_format),
                MainTestRunner.version());
        toolbar.setSubtitle(subTitle);
        setSupportActionBar(toolbar);


        mButtonRun = (FloatingActionButton) findViewById(R.id.button_run);
        mButtonRun.setOnClickListener(this);

        mTestsProgress = (ProgressBar) findViewById(R.id.progress_bar);

        TabHost tabHost = (TabHost) findViewById(R.id.tabhost);
        tabHost.setup();
        tabHost.addTab(tabHost.newTabSpec("tab_passed")
                .setIndicator(this.getString(R.string.tab_passed), null)
                .setContent(R.id.list_view_passed));
        tabHost.addTab(tabHost.newTabSpec("tab_failed")
                .setIndicator(this.getString(R.string.tab_failed), null)
                .setContent(R.id.list_view_failed));

        // 让两个Tab上的文字居中显式，Android Studio版本使用compileSdkVersion 14，不需要这么设置
        // TabWidget tabWidget = tabHost.getTabWidget();
        // for (int i = 0; i < tabWidget.getChildCount(); ++i) {
        //    View child = tabWidget.getChildAt(i);

        //    TextView textView = (TextView) child.findViewById(android.R.id.title);
        //    RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) textView
        //            .getLayoutParams();
        //    params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, 0);
        //    params.addRule(RelativeLayout.CENTER_IN_PARENT, RelativeLayout.TRUE);
        // }

        mListAdapterPassed = new TestProgressAdapter(this);
        mListViewPassed = (ListView) findViewById(R.id.list_view_passed);
        mListViewPassed.setAdapter(mListAdapterPassed);

        mListAdapterFailed = new TestProgressAdapter(this);
        mListViewFailed = (ListView) findViewById(R.id.list_view_failed);
        mListViewFailed.setAdapter(mListAdapterFailed);
        mListViewFailed.setOnItemClickListener(this);

        MainTestRunner.loadTestConfig(this);
        MainTestRunner.start(this);
    }

    void updateTime() {
        mTestEndTimeMs = System.currentTimeMillis();
        long elapsedMs = mTestEndTimeMs - mTestStartTimeMs;
        String text = String.format(
                this.getString(R.string.text_time_format),
                Long.toString(elapsedMs));
        TextView tv = (TextView) findViewById(R.id.text_time);
        tv.setText(text);
    }

    void updateTextProgress(String test_name) {
        TextView tv = (TextView) findViewById(R.id.text_progress);
        tv.setText(test_name);
    }

    void updateTestsRun() {
        mTestsProgress.setProgress(mTestsRun);
        String text = String.format(
                this.getString(R.string.text_runs_format),
                mTestsRun, mTestsCount);
        TextView tv = (TextView) findViewById(R.id.text_runs);
        tv.setText(text);

        if (mTestsRun == mTestsCount)
            mButtonRun.setEnabled(true);
    }

    void updateErrors() {
        TextView text = (TextView) findViewById(R.id.text_errors);
        TextView label = (TextView) findViewById(R.id.label_errors);
        text.setText(Integer.toString(mErrors));

        int color = (0 == mErrors) ? android.graphics.Color.WHITE : android.graphics.Color.RED;
        text.setTextColor(color);
        label.setTextColor(color);
    }

    void updateFailures() {
        TextView text = (TextView) findViewById(R.id.text_failures);
        TextView label = (TextView) findViewById(R.id.label_failures);
        text.setText(Integer.toString(mFailures));

        int color = (0 == mFailures) ? android.graphics.Color.WHITE : android.graphics.Color.RED;
        text.setTextColor(color);
        label.setTextColor(color);
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        if (parent == mListViewFailed) {
            TestProgressAdapter.ItemInfo info = (TestProgressAdapter.ItemInfo) mListAdapterFailed
                    .getItem(position);
            if (info.mIndex >= 0) {
                new AlertDialog.Builder(this)
                        .setTitle(this.getString(R.string.title_details))
                        .setMessage(MainTestRunner.failureDetails(info.mIndex))
                        .setPositiveButton(this.getString(R.string.button_close), null)
                        .show();
            }

        }
    }

    @Override
    public void onClick(View view) {
        if (R.id.button_run == view.getId()) {
            MainTestRunner.start(this);
            mButtonRun.setEnabled(false);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
