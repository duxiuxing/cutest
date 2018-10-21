#pragma once

#include <cppunit/extensions/TestDecorator.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/Test.h>

#include "../decorator.h"
#include "../result.h"
#include "cutest/event.h"

CUTEST_NS_BEGIN

class DecoratorImpl
    : public Decorator
    , public CPPUNIT_NS::TestDecorator
    , public CPPUNIT_NS::TestListener {
public:
    DecoratorImpl(CPPUNIT_NS::Test* test);

    // 故意跳过TestDecorator的析构函数
    ~DecoratorImpl();

    virtual void destroy() override;

    virtual void addListener(CPPUNIT_NS::TestListener* listener) override;

    virtual void start() override;
    virtual void stop() override;

protected:
    static void* threadFunction(void* param);
    void runOnWorkerThread();

    Event* run_completed; // 用于标志工作线程是否结束的事件

public:
    virtual void addFailure(bool isError, CPPUNIT_NS::Exception* exception) override;
    virtual const CPPUNIT_NS::TestResultCollector* testResultCollector() override;

protected:
    Result test_result;
    CPPUNIT_NS::TestResultCollector result_collector;

public: // 重载TestListener的成员方法
    virtual void startTest(CPPUNIT_NS::Test* test) override;
    virtual void endTest(CPPUNIT_NS::Test* test) override;

protected:
    CPPUNIT_NS::Test* runing_test;
};

CUTEST_NS_END
