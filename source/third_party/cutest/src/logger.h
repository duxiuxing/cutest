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
    virtual void onRunnerStart(CPPUNIT_NS::Test* test);
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed);

    virtual void onSuiteStart(CPPUNIT_NS::Test* suite);
    virtual void onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed);

    virtual void onTestStart(CPPUNIT_NS::Test* test);
    virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
    virtual void onTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed);
    //////////////////////////////////////////////////////////////////////////

protected:
    unsigned int passedTestCaseCount; // 通过的用例记个数就行
    bool firstFailureOfCurrentTest; // 是否为当前Test的首个失败信息
    std::list<std::string> failedTestCaseNames; // 不通过的要把名字记录下来
};

CUTEST_NS_END
