#include "ProgressListenerManager.h"
#include "cutest/Runner.h"

#include <algorithm>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

CUTEST_NS_BEGIN

ProgressListenerManager::ProgressListenerManager()
  : failure_index( 0 )
{}

void
ProgressListenerManager::add( ProgressListener *listener )
{
  TestProgressListeners::iterator it = std::find( this->listeners.begin(), this->listeners.end(), listener );
  if ( it == this->listeners.end() )
  {
    this->listeners.push_back( listener );
  }
}

void
ProgressListenerManager::remove( ProgressListener *listener )
{
  TestProgressListeners::iterator it = std::find( this->listeners.begin(), this->listeners.end(), listener );
  if ( it != this->listeners.end() )
  {
    this->listeners.erase( it );
  }
}

class StartTestRunTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *test;

public:
  StartTestRunTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , test( test_in )
  {}

  virtual void run()
  {
    this->manager->startTestRunImmediately( this->test );
  }
};

void
ProgressListenerManager::startTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    startTestRunImmediately( test );
  }
  else
  {
    Event *event = Event::createInstance();
    StartTestRunTask *task = new StartTestRunTask( this, event, test );
    Runner::instance()->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::startTestRunImmediately( CPPUNIT_NS::Test *test )
{
  while ( !this->test_record.empty() )
  {
    this->test_record.pop();
  }
  this->failure_index = 0;

  TestRecord record;
  record.start_ms = Runner::tickCount64();
  this->test_record.push( record );

  TestProgressListeners::iterator it = this->listeners.begin();
  while ( it != this->listeners.end() )
  {
    ( *it )->onRunnerStart( test );
    ++it;
  }
}

class EndTestRunTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *test;

public:
  EndTestRunTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , test( test_in )
  {}

  virtual void run()
  {
    this->manager->endTestRunImmediately( this->test );
  }
};

void
ProgressListenerManager::endTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    endTestRunImmediately( test );
  }
  else
  {
    Event *event = Event::createInstance();
    Runner *runner = Runner::instance();
    EndTestRunTask *task = new EndTestRunTask( this, event, test );
    runner->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::endTestRunImmediately( CPPUNIT_NS::Test *test )
{
  TestRecord &record = this->test_record.top();
  unsigned int elapsed_ms = ( unsigned int )( Runner::tickCount64() - record.start_ms );
  this->test_record.pop();

  TestProgressListeners::reverse_iterator it = this->listeners.rbegin();
  while ( it != this->listeners.rend() )
  {
    ( *it )->onRunnerEnd( test, elapsed_ms );
    ++it;
  }
}

class StartSuiteTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *suite;

public:
  StartSuiteTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *suite_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , suite( suite_in )
  {}

  virtual void run()
  {
    this->manager->startSuiteImmediately( this->suite );
  }
};

void
ProgressListenerManager::startSuite( CPPUNIT_NS::Test *suite )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    startSuiteImmediately( suite );
  }
  else
  {
    Event *event = Event::createInstance();
    StartSuiteTask *task = new StartSuiteTask( this, event, suite );
    Runner::instance()->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::startSuiteImmediately( CPPUNIT_NS::Test *suite )
{
  TestRecord record;
  record.start_ms = Runner::tickCount64();
  this->test_record.push( record );

  TestProgressListeners::iterator it = this->listeners.begin();
  while ( it != this->listeners.end() )
  {
    ( *it )->onSuiteStart( suite );
    ++it;
  }
}

class EndSuiteTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *suite;

public:
  EndSuiteTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *suite_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , suite( suite_in )
  {}

  virtual void run()
  {
    this->manager->endSuiteImmediately( this->suite );
  }
};

void
ProgressListenerManager::endSuite( CPPUNIT_NS::Test *suite )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    endSuiteImmediately( suite );
  }
  else
  {
    Runner *runner = Runner::instance();
    Event *event = Event::createInstance();
    EndSuiteTask *task = new EndSuiteTask( this, event, suite );
    runner->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::endSuiteImmediately( CPPUNIT_NS::Test *suite )
{
  TestRecord &record = this->test_record.top();
  unsigned int elapsed_ms = ( unsigned int )( Runner::tickCount64() - record.start_ms );
  this->test_record.pop();

  TestProgressListeners::reverse_iterator it = this->listeners.rbegin();
  while ( it != this->listeners.rend() )
  {
    ( *it )->onSuiteEnd( suite, elapsed_ms );
    ++it;
  }
}

class StartTestTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *test;

public:
  StartTestTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , test( test_in )
  {}

  virtual void run()
  {
    this->manager->StartTestImmediately( this->test );
  }
};

void
ProgressListenerManager::startTest( CPPUNIT_NS::Test *test )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    StartTestImmediately( test );
  }
  else
  {
    Event *event = Event::createInstance();
    StartTestTask *task = new StartTestTask( this, event, test );
    Runner::instance()->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
  // 在这记录开始时间，避免把线程切换的时间也计算在内
  TestRecord record;
  record.start_ms = Runner::tickCount64();
  this->test_record.push( record );
}

void
ProgressListenerManager::StartTestImmediately( CPPUNIT_NS::Test *test )
{
  TestProgressListeners::iterator it = this->listeners.begin();
  while ( it != this->listeners.end() )
  {
    ( *it )->onTestStart( test );
    ++it;
  }
}

class AddFailureTask : public ProgressListenerManager::TaskBase
{
protected:
  ProgressListenerManager *manager;
  CPPUNIT_NS::TestFailure *failure;

public:
  AddFailureTask( ProgressListenerManager *manager_in, Event *event, const CPPUNIT_NS::TestFailure &failure_in )
    : ProgressListenerManager::TaskBase( manager_in, event )
    , failure( NULL )
  {
    this->failure = failure_in.clone();
  }

  virtual ~AddFailureTask()
  {
    delete this->failure;
  }

  virtual void run()
  {
    this->manager->addFailureImmediately( *this->failure );
  }
};

void
ProgressListenerManager::addFailure( const CPPUNIT_NS::TestFailure &failure )
{
  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    addFailureImmediately( failure );
  }
  else
  {
    Event *event = Event::createInstance();
    AddFailureTask *task = new AddFailureTask( this, event, failure );
    Runner::instance()->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::addFailureImmediately( const CPPUNIT_NS::TestFailure &failure )
{
  if ( !this->test_record.empty() )
  {
    TestRecord &record = this->test_record.top();

    if ( failure.isError() )
    {
      record.errors++;
    }
    else
    {
      record.failures++;
    }
  }

  TestProgressListeners::iterator it = this->listeners.begin();
  while ( it != this->listeners.end() )
  {
    ( *it )->onFailureAdd( this->failure_index, failure );
    ++it;
  }

  ++this->failure_index;
}

class EndTestTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *test;
  unsigned int elapsed_ms;

public:
  EndTestTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test_in, unsigned int elapsed_ms_in )
    : ProgressListenerManager::TaskBase( manager, event )
    , test( test_in )
    , elapsed_ms( elapsed_ms_in )
  {}

  virtual void run()
  {
    this->manager->endTestImmediately( this->test, this->elapsed_ms );
  }
};

void
ProgressListenerManager::endTest( CPPUNIT_NS::Test *test )
{
  Runner *runner = Runner::instance();
  // 在这记录用例耗时，避免把线程切换的时间也计算在内
  TestRecord &record = this->test_record.top();
  unsigned int elapsed_ms = ( unsigned int )( Runner::tickCount64() - record.start_ms );

  if ( CUTEST_NS::Runner::currentThreadId() == CUTEST_NS::Runner::mainThreadId() )
  {
    endTestImmediately( test, elapsed_ms );
  }
  else
  {
    Event *event = Event::createInstance();
    EndTestTask *task = new EndTestTask( this, event, test, elapsed_ms );
    runner->asyncRunOnMainThread( task, true );
    event->wait();
    event->destroy();
  }
}

void
ProgressListenerManager::endTestImmediately( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  TestRecord &record = this->test_record.top();
  TestProgressListeners::reverse_iterator it = this->listeners.rbegin();
  while ( it != this->listeners.rend() )
  {
    ( *it )->onTestEnd( test, record.errors, record.failures, elapsed_ms );
    ++it;
  }

  this->test_record.pop();
}

CUTEST_NS_END
