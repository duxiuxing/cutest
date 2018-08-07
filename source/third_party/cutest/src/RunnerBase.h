#pragma once

#include "AutoEndTest.h"
#include "cutest/Runner.h"
#include "Decorator.h"
#include "ProgressListenerManager.h"

CUTEST_NS_BEGIN

class ExplicitEndTest;

class RunnerBase : public Runner
{
public:
  RunnerBase();
  virtual ~RunnerBase();

public:
  virtual void setAlwaysCallTestOnMainThread( bool value );
  virtual bool alwaysCallTestOnMainThread();

  virtual void setTreatTimeoutAsError( bool value );
  virtual bool treatTimeoutAsError();

public: // Runner接口族的实现
  virtual void addListener( ProgressListener *listener );
  virtual void removeListener( ProgressListener *listener );

protected:
  ProgressListenerManager listener_manager;


public:
  virtual void start( CPPUNIT_NS::Test *test );
  virtual void stop();

  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception );

  virtual unsigned int errorCount() const;
  virtual unsigned int failureCount() const;
  virtual unsigned int totalFailureCount() const;
  virtual const CPPUNIT_NS::TestFailure *failureAt( unsigned int index ) const;

protected:
  Decorator *test_decorator;


public: // ExplicitEndTest相关的方法
  virtual void registerExplicitEndTest( ExplicitEndTest *test, unsigned int timeout_ms );
  virtual void unregisterExplicitEndTest( ExplicitEndTest *test );

protected:
  ExplicitEndTest *runing_test;
  AutoEndTest auto_end_test;
  bool always_call_test_on_main_thread;
  bool treat_timeout_as_error;
};

CUTEST_NS_END
