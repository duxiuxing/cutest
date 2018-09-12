#pragma once

#include "../Logger.h"
#include "../RunnerBase.h"

#include "cutest/Helper.h"

#include <cppunit/JniTestCaller.h>
#include <jni.h>

CUTEST_NS_BEGIN

class RunnerImpl
    : public RunnerBase
    , public Runnable {
    friend class Runner;
    friend thread_id CUTEST_NS::mainThreadId();

    DEFINE_JCLASS_NAME("com/tencent/cutest/Runner");

public:
    RunnerImpl();

protected:
    Logger test_progress_logger;

public:
    // Runner的接口实现
    virtual void asyncRunOnMainThread(Runnable* runnable, bool is_auto_delete);
    virtual void delayRunOnMainThread(unsigned int delay_ms, Runnable* runnable, bool is_auto_delete);

    virtual void waitUntilAllTestEnd();

protected:
    static thread_id main_thread_id;

    // 实现Runnable::run()
    virtual void run();
};

CUTEST_NS_END
