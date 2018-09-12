
package com.tencent.cppunit;

public interface TestProgressListener {
    // 对应于TestProgressListener::StartTestRun()
    public void onStartTestRun(int all_test_count);

    // 对应于TestProgressListener::EndTestRun()
    public void onEndTestRun(int elapsed_ms);

    // 对应于TestProgressListener::StartTest()
    public void onStartTest(String test_name);

    // 对应于TestProgressListener::AddFailure()
    public void onAddFailure(
            int index,
            boolean is_error,
            String file_name,
            int line_number);

    // 对应于TestProgressListener::EndTest()
    public void onEndTest(
            String test_name,
            int error_count,
            int failure_count,
            int elapsed_ms);
}
