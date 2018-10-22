#pragma once

#include "cutest/helper.h"
#include "cutest/runner.h"

#include "auto_end_test.h"
#include "decorator.h"
#include "listener_manager.h"

CUTEST_NS_BEGIN

class ExplicitEndTest;

class RunnerBase
    : public Runner
    , public Listener
    , public Runnable {
    friend thread_id CUTEST_NS::MainThreadId();

public:
    RunnerBase();

public:
    virtual void SetAlwaysCallTestOnMainThread(bool value) override;
    virtual bool IsAlwaysCallTestOnMainThread() override;

    virtual void SetTreatTimeoutAsError(bool value) override;
    virtual bool IsTreatTimeoutAsError() override;

public: // Runner接口族的实现
    virtual void AddListener(Listener* listener) override;
    virtual void RemoveListener(Listener* listener) override;

protected:
    ListenerManager m_listenerManager;

public:
    virtual void Start(CPPUNIT_NS::Test* test) override;
    virtual void Stop() override;

    virtual void WaitUntilAllTestEnd() override;

    virtual void AddFailure(bool isError, CPPUNIT_NS::Exception* exception) override;

    virtual unsigned int ErrorCount() const override;
    virtual unsigned int FailureCount() const override;
    virtual unsigned int TotalFailureCount() const override;
    virtual const CPPUNIT_NS::TestFailure* FailureAt(unsigned int index) const override;

protected:
    Decorator* m_testDecorator;

public: // ExplicitEndTest相关的方法
    virtual void RegisterExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) override;
    virtual void UnregisterExplicitEndTest(ExplicitEndTest* test) override;

protected:
    ExplicitEndTest* m_currentTest;
    AutoEndTest m_autoEndTest;
    bool m_isAlwaysCallTestOnMainThread;
    bool m_isTreatTimeoutAsError;

    enum State {
        STATE_NONE = 0,  // 空闲状态，上一状态为STATE_RUNING or STATE_STOPPING
        STATE_RUNING,    // 调用了Runner::Start()之后，上一状态为STATE_NONE
        STATE_STOPPING,  // 调用了Runner::Stop()之后，上一状态为STATE_RUNING
    };
    State m_state;
    virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) override;

    static thread_id s_mainThreadId;

    // 实现Runnable::Run()
    virtual void Run() override;
};

CUTEST_NS_END
