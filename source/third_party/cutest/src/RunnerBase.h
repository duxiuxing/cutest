#pragma once

#include "cutest/Helper.h"
#include "cutest/Runner.h"

#include "AutoEndTest.h"
#include "Decorator.h"
#include "ProgressListenerManager.h"

CUTEST_NS_BEGIN

class ExplicitEndTest;

class RunnerBase
    : public Runner
    , public ProgressListener
    , public Runnable {
    friend thread_id CUTEST_NS::mainThreadId();

public:
    RunnerBase();
    virtual ~RunnerBase();

public:
    virtual void setAlwaysCallTestOnMainThread(bool value) override;
    virtual bool isAlwaysCallTestOnMainThread() override;

    virtual void setTreatTimeoutAsError(bool value) override;
    virtual bool isTreatTimeoutAsError() override;

public: // Runner接口族的实现
    virtual void addListener(ProgressListener* listener) override;
    virtual void removeListener(ProgressListener* listener) override;

protected:
    ProgressListenerManager listener_manager;

public:
    virtual void start(CPPUNIT_NS::Test* test) override;
    virtual void stop() override;

    virtual void addFailure(bool isError, CPPUNIT_NS::Exception* exception) override;

    virtual unsigned int errorCount() const override;
    virtual unsigned int failureCount() const override;
    virtual unsigned int totalFailureCount() const override;
    virtual const CPPUNIT_NS::TestFailure* failureAt(unsigned int index) const override;

protected:
    Decorator* testDecorator;

public: // ExplicitEndTest相关的方法
    virtual void registerExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) override;
    virtual void unregisterExplicitEndTest(ExplicitEndTest* test) override;

protected:
    ExplicitEndTest* currentTest;
    AutoEndTest autoEndTest;
    bool alwaysCallTestOnMainThread;
    bool treatTimeoutAsError;

    enum State {
        STATE_NONE = 0,  // 空闲状态，上一状态为STATE_RUNING or STATE_STOPPING
        STATE_RUNING,    // 调用了Runner::start()之后，上一状态为STATE_NONE
        STATE_STOPPING,  // 调用了Runner::stop()之后，上一状态为STATE_RUNING
    };
    State state;
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) override;

    static thread_id mainThreadId;

    // 实现Runnable::run()
    virtual void run() override;
};

CUTEST_NS_END
