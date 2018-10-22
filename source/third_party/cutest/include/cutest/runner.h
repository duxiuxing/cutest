#pragma once

#include <cppunit/Exception.h>
#include <cppunit/Test.h>

#include "listener.h"

CUTEST_NS_BEGIN

class Event;
class ExplicitEndTest;
class Runnable;

class GTEST_API_ Runner {
public:
    static Runner* Instance();

public: // Help接口族
    /*
        在主线程异步调用runnable->Run()
        @param runnable 指向要调用的Runnable对象
        @param isAutoDelete
        - 若为true，Runner调用runnable->Run()之后会接着调用delete runnable;
        - 若为false，Runner则只负责调用runnable->Run()。
    */
    virtual void AsyncRunOnMainThread(Runnable* runnable, bool isAutoDelete) = 0;

    /*
        在主线程延迟调用runnable->run()
        @param msDelay 延迟时间，单位是ms
        @param runnable 指向要调用的Runnable对象
        @param isAutoDelete
        - 若为true，Runner调用runnable->run()之后会接着调用delete runnable;
        - 若为false，Runner则只负责调用runnable->run()。
    */
    virtual void DelayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete) = 0;

    virtual void SetAlwaysCallTestOnMainThread(bool value) = 0;
    virtual bool IsAlwaysCallTestOnMainThread() = 0;

    virtual void SetTreatTimeoutAsError(bool value) = 0;
    virtual bool IsTreatTimeoutAsError() = 0;

public: // Runner接口族
    virtual void AddListener(Listener* listener) = 0;
    virtual void RemoveListener(Listener* listener) = 0;

    virtual void Start(CPPUNIT_NS::Test* test) = 0;
    virtual void Stop() = 0;

    virtual void WaitUntilAllTestEnd() = 0;

    virtual void AddFailure(bool isError, CPPUNIT_NS::Exception* exception) = 0;

    virtual unsigned int ErrorCount() const = 0;
    virtual unsigned int FailureCount() const = 0;
    virtual unsigned int TotalFailureCount() const = 0; // 等于ErrorCount + FailureCount
    virtual const CPPUNIT_NS::TestFailure* FailureAt(unsigned int index) const = 0;

public: // ExplicitEndTest相关接口族
    virtual void RegisterExplicitEndTest(ExplicitEndTest* test, unsigned int msTimeout) = 0;
    virtual void UnregisterExplicitEndTest(ExplicitEndTest* test) = 0;
};

CUTEST_NS_END
