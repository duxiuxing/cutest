#pragma once

#include <list>
#include <string>

#include "cutest/ProgressListener.h"

CUTEST_NS_BEGIN

class Logger : public ProgressListener {
public:
    Logger();

    //////////////////////////////////////////////////////////////////////////
    // 重载ProgressListener的成员方法
    virtual void onRunnerStart(CPPUNIT_NS::Test* test);
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms);

    virtual void onSuiteStart(CPPUNIT_NS::Test* suite);
    virtual void onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int elapsed_ms);

    virtual void onTestStart(CPPUNIT_NS::Test* test);
    virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
    virtual void onTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int error_count,
        unsigned int failure_count,
        unsigned int elapsed_ms);
    //////////////////////////////////////////////////////////////////////////

protected:
    unsigned int passed_test_cases; // 通过的用例记个数就行
    bool first_failure_of_a_test; // 是否为当前Test的首个失败信息
    std::list<std::string> failed_test_cases; // 不通过的要把名字记录下来
};

CUTEST_NS_END
