#pragma once

#include "cutest/define.h"

// cppunit
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

// std
#include <list>
#include <stack>
#include <string>

CUTEST_NS_BEGIN

class Listener {
public:
    virtual ~Listener() {}

    virtual void onRunnerStart(CPPUNIT_NS::Test* test) {}
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {}

    virtual void onSuiteStart(CPPUNIT_NS::Test* suite) {}
    virtual void onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) {}

    virtual void onTestStart(CPPUNIT_NS::Test* test) {}
    virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {}
    virtual void onTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed) {}
};

CUTEST_NS_END
