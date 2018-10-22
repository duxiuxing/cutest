#pragma once

#include <cppunit/extensions/TestDecorator.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/Test.h>
#include <WTypes.h>

#include "../decorator.h"
#include "../result.h"
#include "cutest/event.h"
#include "gtest/internal/gtest-result-xml-printer.h"

CUTEST_NS_BEGIN

class DecoratorImpl
    : public Decorator
    , public CPPUNIT_NS::TestDecorator
    , public CPPUNIT_NS::TestListener {
public:
    DecoratorImpl(CPPUNIT_NS::Test* test);

    // 跳过基类TestDecorator的析构函数
    ~DecoratorImpl() {}

    virtual void destroy();

    virtual void addListener(CPPUNIT_NS::TestListener* listener);

    virtual void start();
    virtual void stop();

protected:
    static UINT __stdcall threadFunction(LPVOID param);
    void runOnWorkerThread();

    HANDLE hThread;
    Event* runCompleted; // 用于标志工作线程是否结束的事件

public:
    virtual void addFailure(bool isError, CPPUNIT_NS::Exception* exception);
    virtual const CPPUNIT_NS::TestResultCollector* testResultCollector();

protected:
    Result result;
    CPPUNIT_NS::TestResultCollector resultCollector;
    testing::internal::TestResultXmlPrinter* resultPrinter;

public: // 重载TestListener的成员方法
    virtual void startTest(CPPUNIT_NS::Test* test) override;
    virtual void endTest(CPPUNIT_NS::Test* test) override;

protected:
    CPPUNIT_NS::Test* currentTest;
};

CUTEST_NS_END
