#include "RunnerBase.h"

#include "cutest/ExplicitEndTest.h"
#include "cutest/Runnable.h"

#include "gtest/gtest-message.h"

CUTEST_NS_BEGIN

RunnerBase::RunnerBase()
    : testDecorator(NULL)
    , currentTest(NULL)
    , alwaysCallTestOnMainThread(false)
    , treatTimeoutAsError(false)
    , state(STATE_NONE) {
    addListener(this);
}

RunnerBase::~RunnerBase() {
    stop();

    if (this->testDecorator) {
        this->testDecorator->destroy();
        this->testDecorator = NULL;
    }
}

void
RunnerBase::setAlwaysCallTestOnMainThread(bool value) {
    this->alwaysCallTestOnMainThread = value;
}

bool
RunnerBase::isAlwaysCallTestOnMainThread() {
    return this->alwaysCallTestOnMainThread;
}

void
RunnerBase::setTreatTimeoutAsError(bool value) {
    this->treatTimeoutAsError = value;
}

bool
RunnerBase::isTreatTimeoutAsError() {
    return this->treatTimeoutAsError;
}

void
RunnerBase::addListener(ProgressListener* listener) {
    this->listener_manager.add(listener);
}

void
RunnerBase::removeListener(ProgressListener* listener) {
    this->listener_manager.remove(listener);
}

void
RunnerBase::start(CPPUNIT_NS::Test* test) {
    switch (this->state) {
    case STATE_NONE:
        this->state = STATE_RUNING;
        break;
    case STATE_RUNING:
        return;
    case STATE_STOPPING:
        return;
    default:
        return;
    }

    if (this->testDecorator) {
        this->testDecorator->destroy();
        this->testDecorator = NULL;
    }

    this->testDecorator = Decorator::createInstance(test);
    this->testDecorator->addListener(&this->listener_manager);
    this->testDecorator->start();
}

void
RunnerBase::stop() {
    switch (this->state) {
    case STATE_NONE:
        return;
    case STATE_RUNING:
        this->state = STATE_STOPPING;
        break;
    case STATE_STOPPING:
        return;
    default:
        return;
    }

    if (this->testDecorator) {
        this->testDecorator->stop();

        if (this->currentTest) {
            this->currentTest->endTest();
            this->currentTest = NULL;
        }
    }
}

void
RunnerBase::addFailure(bool isError, CPPUNIT_NS::Exception* exception) {
    if (this->testDecorator) {
        this->testDecorator->addFailure(isError, exception);
    }
}

unsigned int
RunnerBase::errorCount() const {
    if (this->testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = this->testDecorator->testResultCollector();
        return (unsigned int)collector->testErrors();
    }
    return 0;
}

unsigned int
RunnerBase::failureCount() const {
    if (this->testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = this->testDecorator->testResultCollector();
        return (unsigned int)collector->testFailures();
    }
    return 0;
}

unsigned int
RunnerBase::totalFailureCount() const {
    if (this->testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = this->testDecorator->testResultCollector();
        return (unsigned int)collector->testFailuresTotal();
    }
    return 0;
}

const CPPUNIT_NS::TestFailure*
RunnerBase::failureAt(unsigned int index) const {
    if (this->testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = this->testDecorator->testResultCollector();
        if (index < collector->failures().size()) {
            return collector->failures()[index];
        }
    }
    return NULL;
}

void
RunnerBase::registerExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) {
    this->currentTest = test;
    this->autoEndTest.check(test, msTimeout);
}

void
RunnerBase::unregisterExplicitEndTest(ExplicitEndTest* test) {
    if (this->currentTest == test) {
        this->currentTest = NULL;
        this->autoEndTest.cancel();
    }
}

void
RunnerBase::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    this->state = STATE_NONE;
}

thread_id RunnerBase::mainThreadId = 0;

thread_id
mainThreadId() {
    return RunnerBase::mainThreadId;
}

void
RunnerBase::run() {
    RunnerBase::mainThreadId = currentThreadId();
}

CUTEST_NS_END
