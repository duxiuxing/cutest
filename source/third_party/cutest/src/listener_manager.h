#pragma once

#include <cppunit/TestListener.h>

#include "cutest/event.h"
#include "cutest/runnable.h"
#include "cutest/listener.h"

// std
#include <cppunit/portability/CppUnitVector.h>
#include <stack>
#include <string>

CUTEST_NS_BEGIN

class ListenerManager : public CPPUNIT_NS::TestListener {
public:
    ListenerManager();

    void Add(Listener* listener);
    void Remove(Listener* listener);

protected:
    typedef CppUnitVector<Listener*> Listeners;
    Listeners m_listeners;

public:
    //////////////////////////////////////////////////////////////////////////
    // 重载TestListener的成员方法
    virtual void startTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) override;
    void StartTestRunImmediately(CPPUNIT_NS::Test* test);

    virtual void endTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) override;
    void EndTestRunImmediately(CPPUNIT_NS::Test* test);

    virtual void startSuite(CPPUNIT_NS::Test* suite) override;
    void StartSuiteImmediately(CPPUNIT_NS::Test* suite);

    virtual void endSuite(CPPUNIT_NS::Test* suite) override;
    void EndSuiteImmediately(CPPUNIT_NS::Test* suite);

    virtual void startTest(CPPUNIT_NS::Test* test) override;
    void StartTestImmediately(CPPUNIT_NS::Test* test);

    virtual void addFailure(const CPPUNIT_NS::TestFailure& failure) override;
    void AddFailureImmediately(const CPPUNIT_NS::TestFailure& failure);

    virtual void endTest(CPPUNIT_NS::Test* test) override;
    void EndTestImmediately(CPPUNIT_NS::Test* test, unsigned int msElapsed);

    class TaskBase : public Runnable {
    protected:
        ListenerManager* m_manager;
        Event* m_event;

    public:
        TaskBase(ListenerManager* manager, Event* event)
            : m_manager(manager)
            , m_event(event) {}

        virtual ~TaskBase() {
            if (m_event) {
                m_event->Post();
            }
        }
    };

protected:
    struct TestRecord {
        TestRecord()
            : MsStart(0)
            , ErrorCount(0)
            , FailureCount(0) {}

        unsigned long long MsStart;
        int ErrorCount;
        int FailureCount;
    };

    std::stack<TestRecord> m_testRecords;

    unsigned int m_failureIndex;
};

CUTEST_NS_END
