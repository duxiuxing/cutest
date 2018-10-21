#pragma once

#include <cppunit/Exception.h>
#include <cppunit/Test.h>

#include "Listener.h"

CUTEST_NS_BEGIN

class Event;
class ExplicitEndTest;
class Runnable;

class GTEST_API_ Runner {
public:
    static Runner* instance();

public: // Help接口族
    /*
        在主线程异步调用runnable->run()
        @param runnable 指向要调用的Runnable对象
        @param isAutoDelete
        - 若为true，Runner调用runnable->run()之后会接着调用delete runnable;
        - 若为false，Runner则只负责调用runnable->run()。
    */
    virtual void asyncRunOnMainThread(Runnable* runnable, bool isAutoDelete) = 0;

    /*
        在主线程延迟调用runnable->run()
        @param msDelay 延迟时间，单位是ms
        @param runnable 指向要调用的Runnable对象
        @param isAutoDelete
        - 若为true，Runner调用runnable->run()之后会接着调用delete runnable;
        - 若为false，Runner则只负责调用runnable->run()。
    */
    virtual void delayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete) = 0;

    virtual void setAlwaysCallTestOnMainThread(bool value) = 0;
    virtual bool isAlwaysCallTestOnMainThread() = 0;

    virtual void setTreatTimeoutAsError(bool value) = 0;
    virtual bool isTreatTimeoutAsError() = 0;

public: // Runner接口族
    virtual void addListener(Listener* listener) = 0;
    virtual void removeListener(Listener* listener) = 0;

    virtual void start(CPPUNIT_NS::Test* test) = 0;
    virtual void stop() = 0;

    virtual void waitUntilAllTestEnd() = 0;

    virtual void addFailure(bool isError, CPPUNIT_NS::Exception* exception) = 0;

    virtual unsigned int errorCount() const = 0;
    virtual unsigned int failureCount() const = 0;
    virtual unsigned int totalFailureCount() const = 0; // 等于ErrorCount + FailureCount
    virtual const CPPUNIT_NS::TestFailure* failureAt(unsigned int index) const = 0;

public: // ExplicitEndTest相关接口族
    virtual void registerExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) = 0;
    virtual void unregisterExplicitEndTest(ExplicitEndTest* test) = 0;
};

CUTEST_NS_END
