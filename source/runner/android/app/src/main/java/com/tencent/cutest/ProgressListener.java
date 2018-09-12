
package com.tencent.cutest;

public interface ProgressListener {
    // 对应于ProgressListener::onRunnerStart()
    public void onRunnerStart(int all_test_count);

    // 对应于ProgressListener::onRunnerEnd()
    public void onRunnerEnd(int elapsed_ms);

    // 对应于ProgressListener::onTestStart()
    public void onTestStart(String test_name);

    // 对应于ProgressListener::onFailureAdd()
    public void onFailureAdd(
            int index,
            boolean is_error,
            String file_name,
            int line_number);

    // 对应于ProgressListener::onTestEnd()
    public void onTestEnd(
            String test_name,
            int error_count,
            int failure_count,
            int elapsed_ms);
}
