#pragma once

#include <cppunit/Exception.h>
#include <cppunit/Test.h>

#include "ProgressListener.h"

#if (defined(_WIN32) || defined(_WIN64))

  CUTEST_NS_BEGIN
  typedef unsigned long thread_id;
  CUTEST_NS_END

#else

  #include <pthread.h>
  #include <unistd.h>
  CUTEST_NS_BEGIN
  typedef pid_t thread_id;
  CUTEST_NS_END

#endif

CUTEST_NS_BEGIN

class Event;
class ExplicitEndTest;
class Runnable;

class CPPUNIT_API Runner
{
public:
  static const char *version();
  static thread_id currentThreadId();
  static thread_id mainThreadId();

  /*
    返回当前系统的时间，单位为ms

    主要使用场景：根据用例执行前后的系统时间差计算耗时
  */
  static unsigned long long tickCount();

  static Runner *instance();

public: // Help接口族
  /*
    在主线程异步调用runnable->run()
    @param runnable 指向要调用的Runnable对象
    @param is_auto_delete
      - 若为true，Runner调用runnable->run()之后会接着调用delete runnable;
      - 若为false，Runner则只负责调用runnable->run()。
  */
  virtual void asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete ) = 0;

  /*
    在主线程延迟调用runnable->run()
    @param delay_ms 延迟时间，单位是ms
    @param runnable 指向要调用的Runnable对象
    @param is_auto_delete
      - 若为true，Runner调用runnable->run()之后会接着调用delete runnable;
      - 若为false，Runner则只负责调用runnable->run()。
  */
  virtual void delayRunOnMainThread( unsigned int delay_ms, Runnable *runnable, bool is_auto_delete ) = 0;

  virtual void setAlwaysCallTestOnMainThread( bool value ) = 0;
  virtual bool alwaysCallTestOnMainThread() = 0;

public: // Runner接口族
  virtual void addListener( ProgressListener *listener ) = 0;
  virtual void removeListener( ProgressListener *listener ) = 0;

  virtual void start( CPPUNIT_NS::Test *test ) = 0;
  virtual void stop() = 0;

  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception ) = 0;

  virtual unsigned int errorCount() const = 0;
  virtual unsigned int failureCount() const = 0;
  virtual unsigned int totalFailureCount() const = 0; // 等于ErrorCount + FailureCount
  virtual const CPPUNIT_NS::TestFailure *failureAt( unsigned int index ) const = 0;

public: // ExplicitEndTest相关接口族
  virtual void registerExplicitEndTest( ExplicitEndTest *test, unsigned int timeout_ms ) = 0;
  virtual void unregisterExplicitEndTest( ExplicitEndTest *test ) = 0;
};

CUTEST_NS_END
