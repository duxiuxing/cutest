package com.tencent.cutest;

import android.app.AlertDialog;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TabHost;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
        implements OnClickListener, ProgressListener, OnItemClickListener {
    FloatingActionButton button_run;
    long test_start_ms;
    long test_end_ms;

    ProgressBar progress_bar;
    int count_tests_ran; // 记录已经执行的用例个数
    int count_tests_total; // 记录用例的总数
    int count_errors;
    int count_failures;

    ListView list_view_passed;
    ProgressAdapter list_adapter_passed;

    ListView list_view_failed;
    ProgressAdapter list_adapter_failed;

    public MainActivity() {
        this.count_tests_ran = 0;
        this.count_tests_total = 0;
        this.count_errors = 0;
        this.count_failures = 0;

        this.test_start_ms = System.currentTimeMillis();
        this.test_end_ms = test_start_ms;
    }

    @Override
    public void onRunnerStart(int all_test_count) {
        this.test_start_ms = System.currentTimeMillis();
        updateTime();

        this.count_tests_ran = 0;
        this.count_tests_total = all_test_count;
        this.progress_bar.setMax(this.count_tests_total);
        updateTestsRun();

        this.count_errors = 0;
        updateErrors();

        this.count_failures = 0;
        updateFailures();

        this.list_adapter_passed.reset(all_test_count);
        this.list_adapter_failed.reset(all_test_count);
    }

    @Override
    public void onRunnerEnd(int elapsed_ms) {
        updateTime();
        updateTextProgress(this.getString(R.string.text_progress_none));
    }

    @Override
    public void onTestStart(String test_name) {
        updateTextProgress(test_name);
    }

    @Override
    public void onFailureAdd(int index, boolean is_error, String file_name, int line_number) {
        if (is_error) {
            ++this.count_errors;
            updateErrors();
        } else {
            ++this.count_failures;
            updateFailures();
        }
        this.list_adapter_failed.addFailure(index, is_error, file_name, line_number);
    }

    @Override
    public void onTestEnd(String test_name, int error_count, int failure_count, int elapsed_ms) {
        updateTime();

        ++this.count_tests_ran;
        updateTestsRun();

        if ((0 == error_count) && (0 == failure_count)) {
            this.list_adapter_passed.endTestPassed(test_name, elapsed_ms);
        } else {
            this.list_adapter_failed.endTestFailed(test_name, error_count, failure_count);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT); // 竖屏设置
        setContentView(R.layout.activity_main);

        // ActionBar
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        CharSequence sub_title = String.format(this.getString(R.string.sub_title_format),
                Runner.version());
        toolbar.setSubtitle(sub_title);
        setSupportActionBar(toolbar);

        this.button_run = (FloatingActionButton) findViewById(R.id.button_run);
        this.button_run.setOnClickListener(this);

        this.progress_bar = (ProgressBar) findViewById(R.id.progress_bar);

        TabHost tab_host = (TabHost) findViewById(R.id.tabhost);
        tab_host.setup();
        tab_host.addTab(tab_host.newTabSpec("tab_passed")
                .setIndicator(this.getString(R.string.tab_passed), null)
                .setContent(R.id.list_view_passed));
        tab_host.addTab(tab_host.newTabSpec("tab_failed")
                .setIndicator(this.getString(R.string.tab_failed), null)
                .setContent(R.id.list_view_failed));
        this.list_adapter_passed = new ProgressAdapter(this);
        this.list_view_passed = (ListView) findViewById(R.id.list_view_passed);
        this.list_view_passed.setAdapter(this.list_adapter_passed);

        this.list_adapter_failed = new ProgressAdapter(this);
        this.list_view_failed = (ListView) findViewById(R.id.list_view_failed);
        this.list_view_failed.setAdapter(this.list_adapter_failed);
        this.list_view_failed.setOnItemClickListener(this);

        Runner.loadTestConfig(this);
        Runner.start(this);
    }

    void updateTime() {
        this.test_end_ms = System.currentTimeMillis();
        long elapsed_ms = this.test_end_ms - this.test_start_ms;
        String text = String.format(
                this.getString(R.string.text_time_format),
                Long.toString(elapsed_ms));
        TextView tv = (TextView) findViewById(R.id.text_time);
        tv.setText(text);
    }

    void updateTextProgress(String test_name) {
        TextView tv = (TextView) findViewById(R.id.text_progress);
        tv.setText(test_name);
    }

    void updateTestsRun() {
        this.progress_bar.setProgress(this.count_tests_ran);
        String text = String.format(
                this.getString(R.string.text_runs_format),
                this.count_tests_ran, this.count_tests_total);
        TextView tv = (TextView) findViewById(R.id.text_runs);
        tv.setText(text);

        if (this.count_tests_ran == this.count_tests_total) {
            this.button_run.setEnabled(true);
        }
    }

    void updateErrors() {
        TextView text = (TextView) findViewById(R.id.text_errors);
        TextView label = (TextView) findViewById(R.id.label_errors);
        text.setText(Integer.toString(this.count_errors));

        int color = (0 == this.count_errors) ? android.graphics.Color.WHITE : android.graphics.Color.RED;
        text.setTextColor(color);
        label.setTextColor(color);
    }

    void updateFailures() {
        TextView text = (TextView) findViewById(R.id.text_failures);
        TextView label = (TextView) findViewById(R.id.label_failures);
        text.setText(Integer.toString(this.count_failures));

        int color = (0 == this.count_failures)
                ? android.graphics.Color.WHITE
                : android.graphics.Color.RED;
        text.setTextColor(color);
        label.setTextColor(color);
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        if (parent == this.list_view_failed) {
            ProgressAdapter.ItemInfo info =
                    (ProgressAdapter.ItemInfo) this.list_adapter_failed.getItem(position);
            if (info.index >= 0) {
                new AlertDialog.Builder(this)
                        .setTitle(this.getString(R.string.title_details))
                        .setMessage(Runner.failureDetails(info.index))
                        .setPositiveButton(this.getString(R.string.button_close), null)
                        .show();
            }

        }
    }

    @Override
    public void onClick(View view) {
        if (R.id.button_run == view.getId()) {
            Runner.start(this);
            this.button_run.setEnabled(false);
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
