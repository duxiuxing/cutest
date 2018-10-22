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

    virtual void Destroy();

    virtual void AddListener(CPPUNIT_NS::TestListener* listener);

    virtual void Start();
    virtual void Stop();

protected:
    static UINT __stdcall ThreadFunction(LPVOID param);
    void RunOnWorkerThread();

    HANDLE m_hThread;
    Event* m_runCompleted; // 用于标志工作线程是否结束的事件

public:
    virtual void AddFailure(bool isError, CPPUNIT_NS::Exception* exception);
    virtual const CPPUNIT_NS::TestResultCollector* TestResultCollector();

protected:
    Result m_result;
    CPPUNIT_NS::TestResultCollector m_resultCollector;
    testing::internal::TestResultXmlPrinter* m_resultPrinter;

public: // 重载TestListener的成员方法
    virtual void startTest(CPPUNIT_NS::Test* test) override;
    virtual void endTest(CPPUNIT_NS::Test* test) override;

protected:
    CPPUNIT_NS::Test* m_currentTest;
};

CUTEST_NS_END
