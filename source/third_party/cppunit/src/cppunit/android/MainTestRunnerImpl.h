#pragma once

#include "../RunnerBase.h"
#include "../Logger.h"

#include "cutest/Runnable.h"

#include <cppunit/JniTestCaller.h>

#include <jni.h>

CPPUNIT_NS_BEGIN

class MainTestRunnerImpl
  : public MainTestRunnerBase
  , public Runnable
{
  friend class MainTestRunner;
  DEFINE_JCLASS_NAME( "com/tencent/cppunit/MainTestRunner" );

public:
  MainTestRunnerImpl();

protected:
  TestProgressLogger _testProgressLogger;

public:
  // MainTestRunner的接口实现
  virtual void asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete );
  virtual void delayRunOnMainThread( unsigned int delay_ms, Runnable *runnable, bool is_auto_delete );

protected:
  static thread_id s_mainThreadId;

  // 实现Runnable::run()
  virtual void run();
};

CPPUNIT_NS_END
