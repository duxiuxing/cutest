#include "runner_base.h"

#include "cutest/explicit_end_test.h"
#include "cutest/runnable.h"

#include "gtest/gtest-message.h"

CUTEST_NS_BEGIN

RunnerBase::RunnerBase()
    : m_testDecorator(NULL)
    , m_currentTest(NULL)
    , m_isAlwaysCallTestOnMainThread(false)
    , m_isTreatTimeoutAsError(false)
    , m_state(STATE_NONE) {
    AddListener(this);
}

void RunnerBase::SetAlwaysCallTestOnMainThread(bool value) {
    m_isAlwaysCallTestOnMainThread = value;
}

bool RunnerBase::IsAlwaysCallTestOnMainThread() {
    return m_isAlwaysCallTestOnMainThread;
}

void RunnerBase::SetTreatTimeoutAsError(bool value) {
    m_isTreatTimeoutAsError = value;
}

bool RunnerBase::IsTreatTimeoutAsError() {
    return m_isTreatTimeoutAsError;
}

void RunnerBase::AddListener(Listener* listener) {
    m_listenerManager.Add(listener);
}

void RunnerBase::RemoveListener(Listener* listener) {
    m_listenerManager.Remove(listener);
}

void RunnerBase::Start(CPPUNIT_NS::Test* test) {
    switch (m_state) {
    case STATE_NONE:
        m_state = STATE_RUNING;
        break;
    case STATE_RUNING:
        return;
    case STATE_STOPPING:
        return;
    default:
        return;
    }

    if (m_testDecorator) {
        m_testDecorator->Destroy();
        m_testDecorator = NULL;
    }

    m_testDecorator = Decorator::CreateInstance(test);
    m_testDecorator->AddListener(&m_listenerManager);
    m_testDecorator->Start();
}

void RunnerBase::Stop() {
    switch (m_state) {
    case STATE_NONE:
        return;
    case STATE_RUNING:
        m_state = STATE_STOPPING;
        break;
    case STATE_STOPPING:
        return;
    default:
        return;
    }

    if (m_testDecorator) {
        m_testDecorator->Stop();

        if (m_currentTest) {
            m_currentTest->EndTest();
            m_currentTest = NULL;
        }
    }
}

void RunnerBase::WaitUntilAllTestEnd() {
    if (m_testDecorator) {
        m_testDecorator->Destroy();
        m_testDecorator = NULL;
    }
}

void RunnerBase::AddFailure(bool isError, CPPUNIT_NS::Exception* exception) {
    if (m_testDecorator) {
        m_testDecorator->AddFailure(isError, exception);
    }
}

unsigned int RunnerBase::ErrorCount() const {
    if (m_testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = m_testDecorator->TestResultCollector();
        return (unsigned int)collector->testErrors();
    }
    return 0;
}

unsigned int RunnerBase::FailureCount() const {
    if (m_testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = m_testDecorator->TestResultCollector();
        return (unsigned int)collector->testFailures();
    }
    return 0;
}

unsigned int RunnerBase::TotalFailureCount() const {
    if (m_testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = m_testDecorator->TestResultCollector();
        return (unsigned int)collector->testFailuresTotal();
    }
    return 0;
}

const CPPUNIT_NS::TestFailure* RunnerBase::FailureAt(unsigned int index) const {
    if (m_testDecorator) {
        const CPPUNIT_NS::TestResultCollector* collector = m_testDecorator->TestResultCollector();
        if (index < collector->failures().size()) {
            return collector->failures()[index];
        }
    }
    return NULL;
}

void RunnerBase::RegisterExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) {
    m_currentTest = test;
    m_autoEndTest.Check(test, msTimeout);
}

void RunnerBase::UnregisterExplicitEndTest(ExplicitEndTest* test) {
    if (m_currentTest == test) {
        m_currentTest = NULL;
        m_autoEndTest.Cancel();
    }
}

void RunnerBase::OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    m_state = STATE_NONE;
}

thread_id RunnerBase::s_mainThreadId = 0;

thread_id MainThreadId() {
    return RunnerBase::s_mainThreadId;
}

void RunnerBase::Run() {
    RunnerBase::s_mainThreadId = CurrentThreadId();
}

CUTEST_NS_END
