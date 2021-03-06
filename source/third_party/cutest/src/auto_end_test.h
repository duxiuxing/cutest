﻿#pragma once

#include <cppunit/Portability.h>
#include "cutest/runnable.h"

CUTEST_NS_BEGIN

class ExplicitEndTest;

// 根据每个ExplicitEndTest的超时时长启动Timer，到点之后调用Callback::onTimeout()
class TestTimeoutCounter : public Runnable {
public:
    class Callback {
    public:
        virtual void OnTimeout(ExplicitEndTest* test, TestTimeoutCounter* counter) = 0;
    };

public:
    TestTimeoutCounter(ExplicitEndTest* test);
    void Start(unsigned int msTimeout, Callback* callback);
    void AddFailure();

protected:
    ExplicitEndTest* m_test;
    Callback* m_callback;
    unsigned int m_msTimeout;     // 记录Start()的入参，即ExplicitEndTest的超时时长
    unsigned long long m_msStart; // 记录调用Start()的时刻，用于计算ExplicitEndTest的实际执行时长

public:
    // Runnable::Run()的实现
    virtual void Run();
};

/*
    CUTEST_NS::RunnerBase内部使用的一个帮助类：
    - 使用CPPUNIT_MANUAL_END_TEST_WITH_TIMEOUT，可以为ExplicitEndTest指定一个超时时长(单位：ms)；
    - AutoEndTest的设计是，当ExplicitEndTest执行时长超过了指定的超时时长，
      就通过TestTimeoutCounter::addFailure()打印错误信息，
      并且调用ExplicitEndTest::endTest()结束当前用例（见AutoEndTest::onTimeout()的实现）。
*/
class AutoEndTest : public TestTimeoutCounter::Callback {
public:
    AutoEndTest();
    void Check(ExplicitEndTest* test, unsigned int msTimeout);
    void Cancel();

protected:
    ExplicitEndTest* m_test;
    TestTimeoutCounter* m_counter;

public:
    // TestTimeoutCounter::Callback::OnTimeout()的实现
    virtual void OnTimeout(ExplicitEndTest* test, TestTimeoutCounter* counter);
};

CUTEST_NS_END
