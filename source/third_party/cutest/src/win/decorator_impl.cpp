#include "decorator_impl.h"
#include "synchronization_object_impl.h"

#include "cutest/runner.h"
#include <process.h>

#define GTEST_IMPLEMENTATION_ 1
#include "src/gtest-internal-inl.h"
#undef GTEST_IMPLEMENTATION_

CUTEST_NS_BEGIN

Decorator* Decorator::CreateInstance(CPPUNIT_NS::Test* test) {
    return new DecoratorImpl(test);
}

DecoratorImpl::DecoratorImpl(CPPUNIT_NS::Test* test)
    : TestDecorator(test)
    , m_result(new CPPUNIT_NS::SynchronizationObjectImpl(), new CPPUNIT_NS::SynchronizationObjectImpl())
    , m_resultCollector(new CPPUNIT_NS::SynchronizationObjectImpl())
    , m_resultPrinter(NULL)
    , m_currentTest(NULL)
    , m_hThread(NULL) {
    m_result.addListener(this);
    m_result.addListener(&m_resultCollector);

    m_runCompleted = Event::CreateInstance();
}

void DecoratorImpl::Destroy() {
    if (m_resultPrinter) {
        Runner::Instance()->RemoveListener(m_resultPrinter);
        delete m_resultPrinter;
        m_resultPrinter = NULL;
    }

    if (m_runCompleted) {
        m_runCompleted->Wait();
        m_runCompleted->Destroy();
        m_runCompleted = NULL;
    }
    CPPUNIT_NS::TestDecorator::m_test = NULL;

    delete this;
}

void DecoratorImpl::AddListener(CPPUNIT_NS::TestListener* listener) {
    m_result.addListener(listener);
}

void DecoratorImpl::Start() {
    // 根据参数构造TestResultXmlPrinter
    if (testing::internal::UnitTestOptions::GetOutputFormat() == "xml") {
        m_resultPrinter = new testing::internal::TestResultXmlPrinter(
            testing::internal::UnitTestOptions::GetAbsolutePathToOutputFile().c_str());

        Runner::Instance()->AddListener(m_resultPrinter);
    }

    m_runCompleted->Reset();

    HANDLE sourceHandle = (HANDLE)_beginthreadex(NULL, 0,
                          ThreadFunction, this, CREATE_SUSPENDED, NULL);

    ::SetThreadPriority(sourceHandle, THREAD_PRIORITY_NORMAL);

    ::DuplicateHandle(::GetCurrentProcess(),
                      sourceHandle,
                      ::GetCurrentProcess(),
                      &m_hThread,
                      0,
                      FALSE,
                      DUPLICATE_SAME_ACCESS);

    ::ResumeThread(sourceHandle);
}

UINT __stdcall DecoratorImpl::ThreadFunction(LPVOID param) {
    DecoratorImpl* decorator = (DecoratorImpl*)param;

    decorator->RunOnWorkerThread();

    return 0;
}

void DecoratorImpl::RunOnWorkerThread() {
    m_result.runTest(this);

    ::CloseHandle(m_hThread);
    m_hThread = NULL;

    m_runCompleted->Post();
}

void DecoratorImpl::Stop() {
    m_result.stop();
}

const CPPUNIT_NS::TestResultCollector* DecoratorImpl::TestResultCollector() {
    return &m_resultCollector;
}

void DecoratorImpl::startTest(CPPUNIT_NS::Test* test) {
    m_currentTest = test;
}

void DecoratorImpl::endTest(CPPUNIT_NS::Test* test) {
    m_currentTest = NULL;
}

void DecoratorImpl::AddFailure(bool isError, CPPUNIT_NS::Exception* exception) {
    if (isError) {
        m_result.addError(m_currentTest, exception);
    } else {
        m_result.addFailure(m_currentTest, exception);
    }
}

CUTEST_NS_END
