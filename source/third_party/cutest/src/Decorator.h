#pragma once

// cppunit
#include <cppunit/Exception.h>
#include <cppunit/TestListener.h>
#include <cppunit/TestResultCollector.h>

#include "cutest/Define.h"

CUTEST_NS_BEGIN

class Decorator
{
public:
  // 工厂方法，外部要通过它来创建Decorator对象
  static Decorator *createInstance( CPPUNIT_NS::Test *test );

protected:
  // 外部要通过MainTestDecorator::Destroy()来销毁MainTestDecorator对象
  virtual ~Decorator() {}

public:
  // 销毁MainTestDecorator对象
  virtual void destroy() = 0;

  virtual void addListener( CPPUNIT_NS::TestListener *listener ) = 0;

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception ) = 0;
  virtual const CPPUNIT_NS::TestResultCollector *testResultCollector() = 0;
};

CUTEST_NS_END
