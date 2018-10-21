#include "DecoratorImpl.h"
#include "SynchronizationObjectImpl.h"

#include "cutest/runner.h"
#include <process.h>

#define GTEST_IMPLEMENTATION_ 1
#include "src/gtest-internal-inl.h"
#undef GTEST_IMPLEMENTATION_

CUTEST_NS_BEGIN

Decorator*
Decorator::createInstance(CPPUNIT_NS::Test* test) {
    return new DecoratorImpl(test);
}

DecoratorImpl::DecoratorImpl(CPPUNIT_NS::Test* test)
    : TestDecorator(test)
    , result(new CPPUNIT_NS::SynchronizationObjectImpl(), new CPPUNIT_NS::SynchronizationObjectImpl())
    , resultCollector(new CPPUNIT_NS::SynchronizationObjectImpl())
    , resultPrinter(NULL)
    , currentTest(NULL)
    , hThread(NULL) {
    result.addListener(this);
    result.addListener(&this->resultCollector);

    this->runCompleted = Event::createInstance();
}

void
DecoratorImpl::destroy() {
    if (this->resultPrinter) {
        Runner::instance()->removeListener(this->resultPrinter);
        delete this->resultPrinter;
        this->resultPrinter = NULL;
    }

    if (this->runCompleted) {
        this->runCompleted->wait();
        this->runCompleted->destroy();
        this->runCompleted = NULL;
    }
    CPPUNIT_NS::TestDecorator::m_test = NULL;

    delete this;
}

void
DecoratorImpl::addListener(CPPUNIT_NS::TestListener* listener) {
    this->result.addListener(listener);
}

void
DecoratorImpl::start() {
    // 根据参数构造TestResultXmlPrinter
    if (testing::internal::UnitTestOptions::GetOutputFormat() == "xml") {
        this->resultPrinter = new testing::internal::TestResultXmlPrinter(
            testing::internal::UnitTestOptions::GetAbsolutePathToOutputFile().c_str());

        Runner::instance()->addListener(this->resultPrinter);
    }

    this->runCompleted->reset();

    HANDLE source_handle = (HANDLE)_beginthreadex(NULL, 0,
                           threadFunction, this, CREATE_SUSPENDED, NULL);

    ::SetThreadPriority(source_handle, THREAD_PRIORITY_NORMAL);

    ::DuplicateHandle(::GetCurrentProcess(),
                      source_handle,
                      ::GetCurrentProcess(),
                      &this->hThread,
                      0,
                      FALSE,
                      DUPLICATE_SAME_ACCESS);

    ::ResumeThread(source_handle);
}

UINT
__stdcall
DecoratorImpl::threadFunction(LPVOID param) {
    DecoratorImpl* decorator = (DecoratorImpl*)param;

    decorator->runOnWorkerThread();

    return 0;
}

void
DecoratorImpl::runOnWorkerThread() {
    this->result.runTest(this);

    ::CloseHandle(this->hThread);
    this->hThread = NULL;

    this->runCompleted->post();
}

void
DecoratorImpl::stop() {
    this->result.stop();
}

const CPPUNIT_NS::TestResultCollector*
DecoratorImpl::testResultCollector() {
    return &this->resultCollector;
}

void
DecoratorImpl::startTest(CPPUNIT_NS::Test* test) {
    this->currentTest = test;
}

void
DecoratorImpl::endTest(CPPUNIT_NS::Test* test) {
    this->currentTest = NULL;
}

void
DecoratorImpl::addFailure(bool isError, CPPUNIT_NS::Exception* exception) {
    if (isError) {
        this->result.addError(this->currentTest, exception);
    } else {
        this->result.addFailure(this->currentTest, exception);
    }
}

CUTEST_NS_END
