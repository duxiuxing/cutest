#include "listener_manager.h"

#include "cutest/helper.h"
#include "cutest/runner.h"

#include <algorithm>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

CUTEST_NS_BEGIN

ListenerManager::ListenerManager()
    : failureIndex(0) {}

void
ListenerManager::add(Listener* listener) {
    Listeners::iterator it = std::find(this->listeners.begin(), this->listeners.end(), listener);
    if (it == this->listeners.end()) {
        this->listeners.push_back(listener);
    }
}

void
ListenerManager::remove(Listener* listener) {
    Listeners::iterator it = std::find(this->listeners.begin(), this->listeners.end(), listener);
    if (it != this->listeners.end()) {
        this->listeners.erase(it);
    }
}

class StartTestRunTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* test;

public:
    StartTestRunTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* testParam)
        : ListenerManager::TaskBase(manager, event)
        , test(testParam) {}

    virtual void run() {
        this->manager->startTestRunImmediately(this->test);
    }
};

void
ListenerManager::startTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) {
    if (CUTEST_NS::isOnMainThread()) {
        startTestRunImmediately(test);
    } else {
        Event* event = Event::createInstance();
        StartTestRunTask* task = new StartTestRunTask(this, event, test);
        Runner::instance()->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::startTestRunImmediately(CPPUNIT_NS::Test* test) {
    while (!this->testRecords.empty()) {
        this->testRecords.pop();
    }
    this->failureIndex = 0;

    TestRecord record;
    record.msStart = CUTEST_NS::tickCount64();
    this->testRecords.push(record);

    Listeners::iterator it = this->listeners.begin();
    while (it != this->listeners.end()) {
        (*it)->onRunnerStart(test);
        ++it;
    }
}

class EndTestRunTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* test;

public:
    EndTestRunTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* testParam)
        : ListenerManager::TaskBase(manager, event)
        , test(testParam) {}

    virtual void run() {
        this->manager->endTestRunImmediately(this->test);
    }
};

void
ListenerManager::endTestRun(CPPUNIT_NS::Test* test, CPPUNIT_NS::TestResult*) {
    if (CUTEST_NS::isOnMainThread()) {
        endTestRunImmediately(test);
    } else {
        Event* event = Event::createInstance();
        Runner* runner = Runner::instance();
        EndTestRunTask* task = new EndTestRunTask(this, event, test);
        runner->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::endTestRunImmediately(CPPUNIT_NS::Test* test) {
    TestRecord& record = this->testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::tickCount64() - record.msStart);
    this->testRecords.pop();

    Listeners::reverse_iterator it = this->listeners.rbegin();
    while (it != this->listeners.rend()) {
        (*it)->onRunnerEnd(test, msElapsed);
        ++it;
    }
}

class StartSuiteTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* suite;

public:
    StartSuiteTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* suiteParam)
        : ListenerManager::TaskBase(manager, event)
        , suite(suiteParam) {}

    virtual void run() {
        this->manager->startSuiteImmediately(this->suite);
    }
};

void
ListenerManager::startSuite(CPPUNIT_NS::Test* suite) {
    if (CUTEST_NS::isOnMainThread()) {
        startSuiteImmediately(suite);
    } else {
        Event* event = Event::createInstance();
        StartSuiteTask* task = new StartSuiteTask(this, event, suite);
        Runner::instance()->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::startSuiteImmediately(CPPUNIT_NS::Test* suite) {
    TestRecord record;
    record.msStart = CUTEST_NS::tickCount64();
    this->testRecords.push(record);

    Listeners::iterator it = this->listeners.begin();
    while (it != this->listeners.end()) {
        (*it)->onSuiteStart(suite);
        ++it;
    }
}

class EndSuiteTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* suite;

public:
    EndSuiteTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* suiteParam)
        : ListenerManager::TaskBase(manager, event)
        , suite(suiteParam) {}

    virtual void run() {
        this->manager->endSuiteImmediately(this->suite);
    }
};

void
ListenerManager::endSuite(CPPUNIT_NS::Test* suite) {
    if (CUTEST_NS::isOnMainThread()) {
        endSuiteImmediately(suite);
    } else {
        Runner* runner = Runner::instance();
        Event* event = Event::createInstance();
        EndSuiteTask* task = new EndSuiteTask(this, event, suite);
        runner->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::endSuiteImmediately(CPPUNIT_NS::Test* suite) {
    TestRecord& record = this->testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::tickCount64() - record.msStart);
    this->testRecords.pop();

    Listeners::reverse_iterator it = this->listeners.rbegin();
    while (it != this->listeners.rend()) {
        (*it)->onSuiteEnd(suite, msElapsed);
        ++it;
    }
}

class StartTestTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* test;

public:
    StartTestTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* testParam)
        : ListenerManager::TaskBase(manager, event)
        , test(testParam) {}

    virtual void run() {
        this->manager->StartTestImmediately(this->test);
    }
};

void
ListenerManager::startTest(CPPUNIT_NS::Test* test) {
    if (CUTEST_NS::isOnMainThread()) {
        StartTestImmediately(test);
    } else {
        Event* event = Event::createInstance();
        StartTestTask* task = new StartTestTask(this, event, test);
        Runner::instance()->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
    // 在这记录开始时间，避免把线程切换的时间也计算在内
    TestRecord record;
    record.msStart = CUTEST_NS::tickCount64();
    this->testRecords.push(record);
}

void
ListenerManager::StartTestImmediately(CPPUNIT_NS::Test* test) {
    Listeners::iterator it = this->listeners.begin();
    while (it != this->listeners.end()) {
        (*it)->onTestStart(test);
        ++it;
    }
}

class AddFailureTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::TestFailure* failure;

public:
    AddFailureTask(ListenerManager* manager, Event* event, const CPPUNIT_NS::TestFailure& failureParam)
        : ListenerManager::TaskBase(manager, event)
        , failure(NULL) {
        this->failure = failureParam.clone();
    }

    virtual ~AddFailureTask() {
        delete this->failure;
    }

    virtual void run() {
        this->manager->addFailureImmediately(*this->failure);
    }
};

void
ListenerManager::addFailure(const CPPUNIT_NS::TestFailure& failure) {
    if (CUTEST_NS::isOnMainThread()) {
        addFailureImmediately(failure);
    } else {
        Event* event = Event::createInstance();
        AddFailureTask* task = new AddFailureTask(this, event, failure);
        Runner::instance()->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::addFailureImmediately(const CPPUNIT_NS::TestFailure& failure) {
    if (!this->testRecords.empty()) {
        TestRecord& record = this->testRecords.top();

        if (failure.isError()) {
            record.errorCount++;
        } else {
            record.failureCount++;
        }
    }

    Listeners::iterator it = this->listeners.begin();
    while (it != this->listeners.end()) {
        (*it)->onFailureAdd(this->failureIndex, failure);
        ++it;
    }

    ++this->failureIndex;
}

class EndTestTask : public ListenerManager::TaskBase {
protected:
    CPPUNIT_NS::Test* test;
    unsigned int msElapsed;

public:
    EndTestTask(ListenerManager* manager, Event* event, CPPUNIT_NS::Test* testParam, unsigned int msElapsedParam)
        : ListenerManager::TaskBase(manager, event)
        , test(testParam)
        , msElapsed(msElapsedParam) {}

    virtual void run() {
        this->manager->endTestImmediately(this->test, this->msElapsed);
    }
};

void
ListenerManager::endTest(CPPUNIT_NS::Test* test) {
    Runner* runner = Runner::instance();
    // 在这记录用例耗时，避免把线程切换的时间也计算在内
    TestRecord& record = this->testRecords.top();
    unsigned int msElapsed = (unsigned int)(CUTEST_NS::tickCount64() - record.msStart);

    if (CUTEST_NS::isOnMainThread()) {
        endTestImmediately(test, msElapsed);
    } else {
        Event* event = Event::createInstance();
        EndTestTask* task = new EndTestTask(this, event, test, msElapsed);
        runner->asyncRunOnMainThread(task, true);
        event->wait();
        event->destroy();
    }
}

void
ListenerManager::endTestImmediately(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    TestRecord& record = this->testRecords.top();
    Listeners::reverse_iterator it = this->listeners.rbegin();
    while (it != this->listeners.rend()) {
        (*it)->onTestEnd(test, record.errorCount, record.failureCount, msElapsed);
        ++it;
    }

    this->testRecords.pop();
}

CUTEST_NS_END
