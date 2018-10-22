#pragma once

#include <list>
#include <string>

#include "cutest/listener.h"

CUTEST_NS_BEGIN

class Logger : public Listener {
public:
    Logger();

    //////////////////////////////////////////////////////////////////////////
    // 重载Listener的成员方法
    virtual void OnRunnerStart(CPPUNIT_NS::Test* test);
    virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed);

    virtual void OnSuiteStart(CPPUNIT_NS::Test* suite);
    virtual void OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed);

    virtual void OnTestStart(CPPUNIT_NS::Test* test);
    virtual void OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
    virtual void OnTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed);
    //////////////////////////////////////////////////////////////////////////

protected:
    unsigned int m_passedTestCaseCount; // 通过的用例记个数就行
    bool m_isFirstFailureOfCurrentTest; // 是否为当前Test的首个失败信息
    std::list<std::string> m_failedTestCaseNames; // 不通过的要把名字记录下来
};

CUTEST_NS_END
