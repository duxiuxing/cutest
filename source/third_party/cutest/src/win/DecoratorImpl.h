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

  HANDLE thread_handle;
  Event *run_completed; // 用于标志工作线程是否结束的事件

public:
  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception );
  virtual const CPPUNIT_NS::TestResultCollector *testResultCollector();

protected:
  Result test_result;
  CPPUNIT_NS::TestResultCollector result_collector;
  testing::internal::TestResultXmlPrinter *result_printer;

public: // 重载TestListener的成员方法
  virtual void startTest( CPPUNIT_NS::Test *test ) override;
  virtual void endTest( CPPUNIT_NS::Test *test ) override;

protected:
  CPPUNIT_NS::Test *runing_test;
};

CUTEST_NS_END
