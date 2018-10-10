#include "AutoEndTest.h"

#include "cutest/ExplicitEndTest.h"
#include "cutest/Helper.h"
#include "cutest/Runner.h"

#include "gtest/gtest.h"

CUTEST_NS_BEGIN

TestTimeoutCounter::TestTimeoutCounter(ExplicitEndTest* testParam)
    : test(testParam)
    , callback(NULL)
    , msTimeout(0)
    , msStart(0) {}

void
TestTimeoutCounter::start(unsigned int msTimeoutParam, Callback* callbackParam) {
    if (msTimeoutParam) {
        this->callback = callbackParam;
        this->msTimeout = msTimeoutParam;
        this->msStart = CUTEST_NS::tickCount64();
        Runner::instance()->delayRunOnMainThread(msTimeoutParam, this, false);
    } else {
        // 超时时间为0表示无限等待，这种Test不需要启动Timer
        delete this;
    }
}

void
TestTimeoutCounter::run() {
    // 计算ExplicitEndTest当前的执行时长
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::tickCount64() - this->msStart);
    if (msElapsed < this->msTimeout) {
        /*
            超时补偿逻辑：
            Timer存在误差，比如Runner::instance()->delayRunOnMainThread(1000, ...)指定的是1000ms，
            但有可能在980ms左右就调用TestTimeoutCounter::run()了，为了避免因误差导致ExplicitEndTest被提前结束，
            此处增加一个100ms的补偿逻辑。
        */
        Runner::instance()->delayRunOnMainThread(100, this, false);
    } else {
        this->callback->onTimeout(this->test, this);
        delete this;
    }
}

void
TestTimeoutCounter::addFailure() {
    // 计算ExplicitEndTest当前的执行时长
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::tickCount64() - this->msStart);

    if (msElapsed < this->msTimeout) {
        // ExplicitEndTest还没超时就被自动结束了？需要Review AutoEndTest的设计。
        EXPECT_GE(msElapsed, this->msTimeout) << "AutoEndTest early end the TestCase!";
    } else if (Runner::instance()->isTreatTimeoutAsError()) {
        EXPECT_LE(msElapsed, this->msTimeout) << "ExplicitEndTest Timeout!";
    }
}

AutoEndTest::AutoEndTest()
    : test(NULL)
    , counter(NULL) {}

void
AutoEndTest::check(ExplicitEndTest* testParam, unsigned int msTimeout) {
    if (testParam && msTimeout) {
        this->test = testParam;
        this->counter = new TestTimeoutCounter(testParam);
        this->counter->start(msTimeout, this);
    } else {
        this->test = NULL;
        this->counter = NULL;
    }
}

void
AutoEndTest::cancel() {
    this->test = NULL;
    this->counter = NULL;
}

void
AutoEndTest::onTimeout(ExplicitEndTest* testParam, TestTimeoutCounter* counterParam) {
    if (this->test == testParam && this->counter == counterParam) {
        this->counter->addFailure();
        this->test->endTest();
    }
}

CUTEST_NS_END
