#pragma once

#include "AutoEndTest.h"
#include "cutest/Runner.h"
#include "Decorator.h"
#include "ProgressListenerManager.h"

CUTEST_NS_BEGIN

class ExplicitEndTest;

class RunnerBase : public Runner, public ProgressListener {
public:
    static void initGoogleMock();

    RunnerBase();
    virtual ~RunnerBase();

public:
    virtual void setAlwaysCallTestOnMainThread(bool value) override;
    virtual bool alwaysCallTestOnMainThread() override;

    virtual void setTreatTimeoutAsError(bool value) override;
    virtual bool treatTimeoutAsError() override;

public: // Runner接口族的实现
    virtual void addListener(ProgressListener* listener) override;
    virtual void removeListener(ProgressListener* listener) override;

protected:
    ProgressListenerManager listener_manager;

public:
    virtual void start(CPPUNIT_NS::Test* test) override;
    virtual void stop() override;

    virtual void addFailure(bool is_error, CPPUNIT_NS::Exception* exception) override;

    virtual unsigned int errorCount() const override;
    virtual unsigned int failureCount() const override;
    virtual unsigned int totalFailureCount() const override;
    virtual const CPPUNIT_NS::TestFailure* failureAt(unsigned int index) const override;

protected:
    Decorator* test_decorator;


public: // ExplicitEndTest相关的方法
    virtual void registerExplicitEndTest(ExplicitEndTest* test, unsigned int timeout_ms) override;
    virtual void unregisterExplicitEndTest(ExplicitEndTest* test) override;

protected:
    ExplicitEndTest* runing_test;
    AutoEndTest auto_end_test;
    bool always_call_test_on_main_thread;
    bool treat_timeout_as_error;

    enum State {
        STATE_NONE = 0,  // 空闲状态，上一状态为STATE_RUNING or STATE_STOPPING
        STATE_RUNING,    // 调用了Runner::start()之后，上一状态为STATE_NONE
        STATE_STOPPING,  // 调用了Runner::stop()之后，上一状态为STATE_RUNING
    };
    State state;
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms) override;
};

CUTEST_NS_END
