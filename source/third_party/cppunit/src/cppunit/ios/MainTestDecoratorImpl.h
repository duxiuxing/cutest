#pragma once

#include "../MainTestDecorator.h" // 接口定义

#include <cppunit/extensions/TestDecorator.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>

#include "cutest/Event.h"

CPPUNIT_NS_BEGIN

class MainTestDecoratorImpl
  : public MainTestDecorator
  , public TestDecorator
  , public TestListener
{
public:
  MainTestDecoratorImpl( Test *test );

  // 故意跳过TestDecorator的析构函数
  ~MainTestDecoratorImpl();

  virtual void destroy();

  virtual void addListener( TestListener *listener );

  virtual void start();
  virtual void stop();

protected:
  static void *threadFunction( void *pThis );
  void runOnWorkerThread();

  CUTEST_NS::Event *_runCompleted; // 用于标志工作线程是否结束的事件

public:
  virtual void addFailure( bool is_error, Exception *exception );
  virtual const TestResultCollector *testResultCollector();

protected:
  TestResult _testResult;
  TestResultCollector _resultCollector;

  enum State
  {
    STATE_NONE = 0,
    STATE_RUNING,
  };
  State _state;

public: // 重载TestListener的成员方法
  virtual void startTest( Test *test );
  virtual void endTest( Test *test );

protected:
  Test *_testRuning;
};

CPPUNIT_NS_END
