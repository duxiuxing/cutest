#pragma once

#include <cppunit/extensions/TestDecorator.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/Test.h>
#include <WTypes.h>

#include "../Decorator.h" // 接口定义
#include "../Result.h"
#include "gtest/internal/gtest-result-xml-printer.h"
#include "cutest/Event.h"

CUTEST_NS_BEGIN

class DecoratorImpl
  : public Decorator
  , public CPPUNIT_NS::TestDecorator
  , public CPPUNIT_NS::TestListener
{
public:
  DecoratorImpl( CPPUNIT_NS::Test *test );

  // 故意跳过TestDecorator的析构函数
  ~DecoratorImpl();

  virtual void destroy();

  virtual void addListener( CPPUNIT_NS::TestListener *listener );

  virtual void start();
  virtual void stop();

protected:
  static UINT __stdcall threadFunction( LPVOID pThis );
  void runOnWorkerThread();

  HANDLE m_hThread;
  Event *m_runCompleted; // 用于标志工作线程是否结束的事件

public:
  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception );
  virtual const CPPUNIT_NS::TestResultCollector *testResultCollector();

protected:
  Result m_testResult;
  CPPUNIT_NS::TestResultCollector m_resultCollector;
  testing::internal::TestResultXmlPrinter *m_resultXmlPrinter;

  enum State
  {
    STATE_NONE = 0,
    STATE_RUNING,
  };
  State m_state;

public: // 重载TestListener的成员方法
  virtual void startTest( CPPUNIT_NS::Test *test ) override;
  virtual void endTest( CPPUNIT_NS::Test *test ) override;

protected:
  CPPUNIT_NS::Test *m_testRuning;
};

CUTEST_NS_END
