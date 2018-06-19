#pragma once

#include <cppunit/Portability.h>
#include "cutest/Runnable.h"

CUTEST_NS_BEGIN

class ManualEndTest;

// 根据每个ManualEndTest的超时时长启动Timer，到点之后调用Callback::onTimeout()
class TestTimeoutCounter : public Runnable
{
public:
  class Callback
  {
  public:
    virtual void onTimeout( ManualEndTest *test, TestTimeoutCounter *counter ) = 0;
  };

public:
  TestTimeoutCounter( ManualEndTest *test );
  void start( unsigned int timeout_ms, Callback *callback );
  void addFailure();

protected:
  ManualEndTest *test;
  Callback *callback;
  unsigned int timeout_ms;     // 记录start()的入参，即ManualEndTest的超时时长
  unsigned long long start_ms; // 记录调用start()的时刻，用于计算ManualEndTest的实际执行时长

public:
  // Runnable::run()的实现
  virtual void run();
};

/*
CUTEST_NS::RunnerBase内部使用的一个帮助类：
    - 使用CPPUNIT_MANUAL_END_TEST_WITH_TIMEOUT，可以为ManualEndTest指定一个超时时长(单位：ms)；
    - AutoEndTest的设计是，当ManualEndTest执行时长超过了指定的超时时长，
      就通过TestTimeoutCounter::addFailure()打印错误信息，
      并且调用ManualEndTest::endTest()结束当前用例（见AutoEndTest::onTimeout()的实现）。
*/
class AutoEndTest : public TestTimeoutCounter::Callback
{
public:
  AutoEndTest();
  void check( ManualEndTest *test, unsigned int timeout_ms );
  void cancel();

protected:
  ManualEndTest *test;
  TestTimeoutCounter *counter;

public:
  // TestTimeoutCounter::Callback::OnTimeout()的实现
  virtual void onTimeout( ManualEndTest *test, TestTimeoutCounter *counter );
};

CUTEST_NS_END
