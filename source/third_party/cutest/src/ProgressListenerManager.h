#pragma once

#include <cppunit/TestListener.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/ProgressListener.h"

// std
#include <list>
#include <set>
#include <stack>
#include <string>

CUTEST_NS_BEGIN

class ProgressListenerManager : public CPPUNIT_NS::TestListener
{
public:
  ProgressListenerManager();

  void add( ProgressListener *listener );
  void remove( ProgressListener *listener );

protected:
  typedef std::set<ProgressListener *> TestProgressListeners;
  TestProgressListeners listeners;

public:
  //////////////////////////////////////////////////////////////////////////
  // 重载TestListener的成员方法
  virtual void startTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * );
  void startTestRunOnMainThread( CPPUNIT_NS::Test *test );

  virtual void endTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * );
  void endTestRunOnMainThread( CPPUNIT_NS::Test *test );

  virtual void startSuite( CPPUNIT_NS::Test *suite );
  void startSuiteOnMainThread( CPPUNIT_NS::Test *suite );

  virtual void endSuite( CPPUNIT_NS::Test *suite );
  void endSuiteOnMainThread( CPPUNIT_NS::Test *suite );

  virtual void startTest( CPPUNIT_NS::Test *test );
  void StartTestOnMainThread( CPPUNIT_NS::Test *test );

  virtual void addFailure( const CPPUNIT_NS::TestFailure &failure );
  void addFailureOnMainThread( const CPPUNIT_NS::TestFailure &failure );

  virtual void endTest( CPPUNIT_NS::Test *test );
  void endTestOnMainThread( CPPUNIT_NS::Test *test, unsigned int elapsed_ms );

  class TaskBase : public Runnable
  {
  protected:
    ProgressListenerManager *manager;
    Event *event;

  public:
    TaskBase( ProgressListenerManager *manager_in, Event *event_in )
      : manager( manager_in )
      , event( event_in )
    {
    }

    virtual ~TaskBase()
    {
      if ( this->event )
      {
        this->event->post();
      }
    }
  };

protected:
  struct TestRecord
  {
    TestRecord() : start_ms( 0 ), errors( 0 ), failures( 0 )
    {}

    unsigned long long start_ms;
    int errors;
    int failures;
  };

  std::stack<TestRecord> test_record;

  unsigned int failure_index;
};

CUTEST_NS_END
