#include "listener_manager.h"

#include "cutest/helper.h"
#include "cutest/runner.h"

#include <algorithm>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

CUTEST_NS_BEGIN

ListenerManager::ListenerManager()
    : m_failureIndex(0) {}

void ListenerManager::Add(Listener* listener) {
    Listeners::iterator it = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it == m_listeners.end()) {
        m_listeners.push_back(listener);
    }
}

void ListenerManager::Remove(Listener* listener) {
    Listeners::iterator it = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it != m_listeners.end()) {
        m_listeners.erase(it);
    }
}

class StartTestRunTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_test;

public:
    StartTestRunTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* test)
        : ListenerManager::TaskBase(manager, event)
        , m_test(test) {}

    virtual void Run() {
        m_manager->StartTestRunImmediately(m_test);
    }
};

void ListenerManager::startTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) {
    if (CUTEST_NS::IsOnMainThread()) {
        StartTestRunImmediately(test);
    } else {
        Event* event = Event::CreateInstance();
        StartTestRunTask* task = new StartTestRunTask(this, event, test);
        Runner::Instance()->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::StartTestRunImmediately(CPPUNIT_NS::Test* test) {
    while (!m_testRecords.empty()) {
        m_testRecords.pop();
    }
    m_failureIndex = 0;

    TestRecord record;
    record.MsStart = CUTEST_NS::TickCount64();
    m_testRecords.push(record);

    Listeners::iterator it = m_listeners.begin();
    while (it != m_listeners.end()) {
        (*it)->OnRunnerStart(test);
        ++it;
    }
}

class EndTestRunTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_test;

public:
    EndTestRunTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* test)
        : ListenerManager::TaskBase(manager, event)
        , m_test(test) {}

    virtual void Run() {
        m_manager->EndTestRunImmediately(m_test);
    }
};

void ListenerManager::endTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) {
    if (CUTEST_NS::IsOnMainThread()) {
        EndTestRunImmediately(test);
    } else {
        Event* event = Event::CreateInstance();
        Runner* runner = Runner::Instance();
        EndTestRunTask* task = new EndTestRunTask(this, event, test);
        runner->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::EndTestRunImmediately(CPPUNIT_NS::Test* test) {
    TestRecord& record = m_testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - record.MsStart);
    m_testRecords.pop();

    Listeners::reverse_iterator it = m_listeners.rbegin();
    while (it != m_listeners.rend()) {
        (*it)->OnRunnerEnd(test, msElapsed);
        ++it;
    }
}

class StartSuiteTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_suite;

public:
    StartSuiteTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* suite)
        : ListenerManager::TaskBase(manager, event)
        , m_suite(suite) {}

    virtual void Run() {
        m_manager->StartSuiteImmediately(m_suite);
    }
};

void ListenerManager::startSuite(CPPUNIT_NS::Test* suite) {
    if (CUTEST_NS::IsOnMainThread()) {
        StartSuiteImmediately(suite);
    } else {
        Event* event = Event::CreateInstance();
        StartSuiteTask* task = new StartSuiteTask(this, event, suite);
        Runner::Instance()->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::StartSuiteImmediately(CPPUNIT_NS::Test* suite) {
    TestRecord record;
    record.MsStart = CUTEST_NS::TickCount64();
    m_testRecords.push(record);

    Listeners::iterator it = m_listeners.begin();
    while (it != m_listeners.end()) {
        (*it)->OnSuiteStart(suite);
        ++it;
    }
}

class EndSuiteTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_suite;

public:
    EndSuiteTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* suite)
        : ListenerManager::TaskBase(manager, event)
        , m_suite(suite) {}

    virtual void Run() {
        m_manager->EndSuiteImmediately(m_suite);
    }
};

void ListenerManager::endSuite(CPPUNIT_NS::Test* suite) {
    if (CUTEST_NS::IsOnMainThread()) {
        EndSuiteImmediately(suite);
    } else {
        Runner* runner = Runner::Instance();
        Event* event = Event::CreateInstance();
        EndSuiteTask* task = new EndSuiteTask(this, event, suite);
        runner->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::EndSuiteImmediately(CPPUNIT_NS::Test* suite) {
    TestRecord& record = m_testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - record.MsStart);
    m_testRecords.pop();

    Listeners::reverse_iterator it = m_listeners.rbegin();
    while (it != m_listeners.rend()) {
        (*it)->OnSuiteEnd(suite, msElapsed);
        ++it;
    }
}

class StartTestTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_test;

public:
    StartTestTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* test)
        : ListenerManager::TaskBase(manager, event)
        , m_test(test) {}

    virtual void Run() {
        m_manager->StartTestImmediately(m_test);
    }
};

void ListenerManager::startTest(CPPUNIT_NS::Test* test) {
    if (CUTEST_NS::IsOnMainThread()) {
        StartTestImmediately(test);
    } else {
        Event* event = Event::CreateInstance();
        StartTestTask* task = new StartTestTask(this, event, test);
        Runner::Instance()->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
    // 在这记录开始时间，避免把线程切换的时间也计算在内
    TestRecord record;
    record.MsStart = CUTEST_NS::TickCount64();
    m_testRecords.push(record);
}

void ListenerManager::StartTestImmediately(CPPUNIT_NS::Test* test) {
    Listeners::iterator it = m_listeners.begin();
    while (it != m_listeners.end()) {
        (*it)->OnTestStart(test);
        ++it;
    }
}

class AddFailureTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::TestFailure* m_failure;

public:
    AddFailureTask(ListenerManager* manager, Event* event, const CPPUNIT_NS::TestFailure& failure)
        : ListenerManager::TaskBase(manager, event)
        , m_failure(NULL) {
        m_failure = failure.clone();
    }

    virtual ~AddFailureTask() {
        delete m_failure;
    }

    virtual void Run() {
        m_manager->AddFailureImmediately(*m_failure);
    }
};

void ListenerManager::addFailure(const CPPUNIT_NS::TestFailure& failure) {
    if (CUTEST_NS::IsOnMainThread()) {
        AddFailureImmediately(failure);
    } else {
        Event* event = Event::CreateInstance();
        AddFailureTask* task = new AddFailureTask(this, event, failure);
        Runner::Instance()->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::AddFailureImmediately(const CPPUNIT_NS::TestFailure& failure) {
    if (!m_testRecords.empty()) {
        TestRecord& record = m_testRecords.top();

        if (failure.isError()) {
            ++record.ErrorCount;
        } else {
            ++record.FailureCount;
        }
    }

    Listeners::iterator it = m_listeners.begin();
    while (it != m_listeners.end()) {
        (*it)->OnFailureAdd(m_failureIndex, failure);
        ++it;
    }

    ++m_failureIndex;
}

class EndTestTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* m_test;
    unsigned int m_msElapsed;

public:
    EndTestTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* test, unsigned int msElapsed)
        : ListenerManager::TaskBase(manager, event)
        , m_test(test)
        , m_msElapsed(msElapsed) {}

    virtual void Run() {
        m_manager->EndTestImmediately(m_test, m_msElapsed);
    }
};

void ListenerManager::endTest(CPPUNIT_NS::Test* test) {
    Runner* runner = Runner::Instance();
    // 在这记录用例耗时，避免把线程切换的时间也计算在内
    TestRecord& record = m_testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - record.MsStart);

    if (CUTEST_NS::IsOnMainThread()) {
        EndTestImmediately(test, msElapsed);
    } else {
        Event* event = Event::CreateInstance();
        EndTestTask* task = new EndTestTask(this, event, test, msElapsed);
        runner->AsyncRunOnMainThread(task, true);
        event->Wait();
        event->Destroy();
    }
}

void ListenerManager::EndTestImmediately(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    TestRecord& record = m_testRecords.top();
    Listeners::reverse_iterator it = m_listeners.rbegin();
    while (it != m_listeners.rend()) {
        (*it)->OnTestEnd(test, record.ErrorCount, record.FailureCount, msElapsed);
        ++it;
    }

    m_testRecords.pop();
}

CUTEST_NS_END
