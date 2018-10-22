#include "auto_end_test.h"

#include "cutest/explicit_end_test.h"
#include "cutest/helper.h"
#include "cutest/runner.h"

#include "gtest/gtest.h"

CUTEST_NS_BEGIN

TestTimeoutCounter::TestTimeoutCounter(ExplicitEndTest* testParam)
    : m_test(testParam)
    , m_callback(NULL)
    , m_msTimeout(0)
    , m_msStart(0) {}

void TestTimeoutCounter::Start(unsigned int msTimeout, Callback* callback) {
    if (msTimeout) {
        m_callback = callback;
        m_msTimeout = msTimeout;
        m_msStart = CUTEST_NS::TickCount64();
        Runner::Instance()->DelayRunOnMainThread(msTimeout, this, false);
    } else {
        // 超时时间为0表示无限等待，这种Test不需要启动Timer
        delete this;
    }
}

void TestTimeoutCounter::Run() {
    // 计算ExplicitEndTest当前的执行时长
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - m_msStart);
    if (msElapsed < m_msTimeout) {
        /*
            超时补偿逻辑：
            Timer存在误差，比如Runner::instance()->delayRunOnMainThread(1000, ...)指定的是1000ms，
            但有可能在980ms左右就调用TestTimeoutCounter::run()了，为了避免因误差导致ExplicitEndTest被提前结束，
            此处增加一个100ms的补偿逻辑。
        */
        Runner::Instance()->DelayRunOnMainThread(100, this, false);
    } else {
        m_callback->OnTimeout(m_test, this);
        delete this;
    }
}

void TestTimeoutCounter::AddFailure() {
    // 计算ExplicitEndTest当前的执行时长
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - m_msStart);

    if (msElapsed < m_msTimeout) {
        // ExplicitEndTest还没超时就被自动结束了？需要Review AutoEndTest的设计。
        EXPECT_GE(msElapsed, m_msTimeout) << "AutoEndTest early end the TestCase!";
    } else if (Runner::Instance()->IsTreatTimeoutAsError()) {
        EXPECT_LE(msElapsed, m_msTimeout) << "ExplicitEndTest Timeout!";
    }
}

AutoEndTest::AutoEndTest()
    : m_test(NULL)
    , m_counter(NULL) {}

void AutoEndTest::Check(ExplicitEndTest* test, unsigned int msTimeout) {
    if (test && msTimeout) {
        m_test = test;
        m_counter = new TestTimeoutCounter(test);
        m_counter->Start(msTimeout, this);
    } else {
        m_test = NULL;
        m_counter = NULL;
    }
}

void AutoEndTest::Cancel() {
    m_test = NULL;
    m_counter = NULL;
}

void AutoEndTest::OnTimeout(ExplicitEndTest* test, TestTimeoutCounter* counter) {
    if (m_test == test && m_counter == counter) {
        m_counter->AddFailure();
        m_test->EndTest();
    }
}

CUTEST_NS_END
