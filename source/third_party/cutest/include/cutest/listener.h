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

    virtual void OnRunnerStart(CPPUNIT_NS::Test* test) {}
    virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {}

    virtual void OnSuiteStart(CPPUNIT_NS::Test* suite) {}
    virtual void OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) {}

    virtual void OnTestStart(CPPUNIT_NS::Test* test) {}
    virtual void OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {}
    virtual void OnTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed) {}
};

CUTEST_NS_END
