#pragma once

#include <cppunit/TestListener.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/ProgressListener.h"

// std
#include <cppunit/portability/CppUnitVector.h>
#include <stack>
#include <string>

CUTEST_NS_BEGIN

class ProgressListenerManager : public CPPUNIT_NS::TestListener {
public:
    ProgressListenerManager();

    void add(ProgressListener* listener);
    void remove(ProgressListener* listener);

protected:
    typedef CppUnitVector<ProgressListener*> TestProgressListeners;
    TestProgressListeners listeners;

public:
    //////////////////////////////////////////////////////////////////////////
    // 重载TestListener的成员方法
    virtual void startTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*);
    void startTestRunImmediately(CPPUNIT_NS::Test* test);

    virtual void endTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*);
    void endTestRunImmediately(CPPUNIT_NS::Test* test);

    virtual void startSuite(CPPUNIT_NS::Test* suite);
    void startSuiteImmediately(CPPUNIT_NS::Test* suite);

    virtual void endSuite(CPPUNIT_NS::Test* suite);
    void endSuiteImmediately(CPPUNIT_NS::Test* suite);

    virtual void startTest(CPPUNIT_NS::Test* test);
    void StartTestImmediately(CPPUNIT_NS::Test* test);

    virtual void addFailure(const CPPUNIT_NS::TestFailure& failure);
    void addFailureImmediately(const CPPUNIT_NS::TestFailure& failure);

    virtual void endTest(CPPUNIT_NS::Test* test);
    void endTestImmediately(CPPUNIT_NS::Test* test, unsigned int msElapsed);

    class TaskBase : public Runnable {
    protected:
        ProgressListenerManager* manager;
        Event* event;

    public:
        TaskBase(ProgressListenerManager* managerParam, Event* eventParam)
            : manager(managerParam)
            , event(eventParam) {}

        virtual ~TaskBase() {
            if (this->event) {
                this->event->post();
            }
        }
    };

protected:
    struct TestRecord {
        TestRecord()
            : msStart(0)
            , errorCount(0)
            , failureCount(0) {}

        unsigned long long msStart;
        int errorCount;
        int failureCount;
    };

    std::stack<TestRecord> testRecords;

    unsigned int failureIndex;
};

CUTEST_NS_END
