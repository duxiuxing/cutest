#pragma once

#include "AutoEndTest.h"
#include "cutest/Runner.h"
#include "Decorator.h"
#include "ProgressListenerManager.h"

CUTEST_NS_BEGIN

class ManualEndTest;

class RunnerBase : public Runner
{
public:
  RunnerBase();
  virtual ~RunnerBase();

public: // Runner接口族的实现
  virtual void addListener( ProgressListener *listener );
  virtual void removeListener( ProgressListener *listener );

protected:
  ProgressListenerManager m_listenerManager;


public:
  virtual void start( CPPUNIT_NS::Test *test );
  virtual void stop();

  virtual void addFailure( bool is_error, CPPUNIT_NS::Exception *exception );

  virtual unsigned int errorCount() const;
  virtual unsigned int failureCount() const;
  virtual unsigned int totalFailureCount() const;
  virtual const CPPUNIT_NS::TestFailure *failureAt( unsigned int index ) const;

protected:
  Decorator *m_testDecorator;


public: // ManualEndTest相关的方法
  virtual void registerManualEndTest( ManualEndTest *test, unsigned int timeout_ms );
  virtual void unregisterManualEndTest( ManualEndTest *test );

protected:
  ManualEndTest *m_testRuning;
  AutoEndTest m_autoEndTest;
};

CUTEST_NS_END
