#include "ProgressListenerManager.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/Runner.h"

CUTEST_NS_BEGIN

ProgressListenerManager::ProgressListenerManager()
  : m_failureIndex( 0 )
{}

void
ProgressListenerManager::add( ProgressListener *listener )
{
  m_listeners.insert( listener );
}

void
ProgressListenerManager::remove( ProgressListener *listener )
{
  m_listeners.erase( listener );
}

class StartTestRunTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_test;

public:
  StartTestRunTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test )
    : ProgressListenerManager::TaskBase( manager, event )
    , _test( test )
  {}

  virtual void run()
  {
    m_manager->startTestRunOnMainThread( _test );
  }
};

void
ProgressListenerManager::startTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * )
{
  Event *event = Event::createInstance();
  StartTestRunTask *task = new StartTestRunTask( this, event, test );
  Runner::instance()->asyncRunOnMainThread( task, true );
  event->wait();
  event->destroy();
}

void
ProgressListenerManager::startTestRunOnMainThread( CPPUNIT_NS::Test *test )
{
  while ( !m_testRecord.empty() )
  {
    m_testRecord.pop();
  }
  m_failureIndex = 0;

  TestRecord record;
  record.startMs = Runner::tickCount();
  m_testRecord.push( record );

  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onRunnerStart( test );
    ++it;
  }
}

class EndTestRunTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_test;

public:
  EndTestRunTask( ProgressListenerManager *manager, CPPUNIT_NS::Test *test )
    : ProgressListenerManager::TaskBase( manager, NULL )
    , _test( test )
  {
  }

  virtual void run()
  {
    m_manager->endTestRunOnMainThread( _test );
  }
};

void
ProgressListenerManager::endTestRun( CPPUNIT_NS::Test *test, CPPUNIT_NS::TestResult * )
{
  Runner *runner = Runner::instance();
  EndTestRunTask *task = new EndTestRunTask( this, test );
  runner->asyncRunOnMainThread( task, true );
}

void
ProgressListenerManager::endTestRunOnMainThread( CPPUNIT_NS::Test *test )
{
  TestRecord &record = m_testRecord.top();
  unsigned int elapsedMs = ( unsigned int )( Runner::tickCount() - record.startMs );
  m_testRecord.pop();

  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onRunnerEnd( test, elapsedMs );
    ++it;
  }
}

class StartSuiteTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_suite;

public:
  StartSuiteTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *suite )
    : ProgressListenerManager::TaskBase( manager, event )
    , _suite( suite )
  {}

  virtual void run()
  {
    m_manager->startSuiteOnMainThread( _suite );
  }
};

void
ProgressListenerManager::startSuite( CPPUNIT_NS::Test *suite )
{
  Event *event = Event::createInstance();
  StartSuiteTask *task = new StartSuiteTask( this, event, suite );
  Runner::instance()->asyncRunOnMainThread( task, true );
  event->wait();
  event->destroy();
}

void
ProgressListenerManager::startSuiteOnMainThread( CPPUNIT_NS::Test *suite )
{
  TestRecord record;
  record.startMs = Runner::tickCount();
  m_testRecord.push( record );

  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onSuiteStart( suite );
    ++it;
  }
}

class EndSuiteTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_suite;

public:
  EndSuiteTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *suite )
    : ProgressListenerManager::TaskBase( manager, event )
    , _suite( suite )
  {}

  virtual void run()
  {
    m_manager->endSuiteOnMainThread( _suite );
  }
};

void
ProgressListenerManager::endSuite( CPPUNIT_NS::Test *suite )
{
  Runner *runner = Runner::instance();
  Event *event = Event::createInstance();
  EndSuiteTask *task = new EndSuiteTask( this, event, suite );
  runner->asyncRunOnMainThread( task, true );
  event->wait();
  event->destroy();
}

void
ProgressListenerManager::endSuiteOnMainThread( CPPUNIT_NS::Test *suite )
{
  TestRecord &record = m_testRecord.top();
  unsigned int elapsedMs = ( unsigned int )( Runner::tickCount() - record.startMs );
  m_testRecord.pop();

  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onSuiteEnd( suite, elapsedMs );
    ++it;
  }
}

class StartTestTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_test;

public:
  StartTestTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test )
    : ProgressListenerManager::TaskBase( manager, event )
    , _test( test )
  {}

  virtual void run()
  {
    m_manager->StartTestOnMainThread( _test );
  }
};

void
ProgressListenerManager::startTest( CPPUNIT_NS::Test *test )
{
  Event *event = Event::createInstance();
  StartTestTask *task = new StartTestTask( this, event, test );
  Runner::instance()->asyncRunOnMainThread( task, true );
  event->wait();
  event->destroy();

  // 在这记录开始时间，避免把线程切换的时间也计算在内
  TestRecord record;
  record.startMs = Runner::tickCount();
  m_testRecord.push( record );
}

void
ProgressListenerManager::StartTestOnMainThread( CPPUNIT_NS::Test *test )
{
  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onTestStart( test );
    ++it;
  }
}

class AddFailureTask : public Runnable
{
protected:
  ProgressListenerManager *_manager;
  CPPUNIT_NS::TestFailure *_failure;

public:
  AddFailureTask( ProgressListenerManager *manager, const CPPUNIT_NS::TestFailure &failure )
    : _manager( manager )
    , _failure( NULL )
  {
    _failure = failure.clone();
  }

  virtual ~AddFailureTask()
  {
    delete _failure;
  }

  virtual void run()
  {
    _manager->addFailureOnMainThread( *_failure );
  }
};

void
ProgressListenerManager::addFailure( const CPPUNIT_NS::TestFailure &failure )
{
  AddFailureTask *task = new AddFailureTask( this, failure );
  Runner::instance()->asyncRunOnMainThread( task, true );
}

void
ProgressListenerManager::addFailureOnMainThread( const CPPUNIT_NS::TestFailure &failure )
{
  TestRecord &record = m_testRecord.top();

  if ( failure.isError() )
  {
    record.errors++;
  }
  else
  {
    record.failures++;
  }

  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onFailureAdd( m_failureIndex, failure );
    ++it;
  }

  ++m_failureIndex;
}

class EndTestTask : public ProgressListenerManager::TaskBase
{
protected:
  CPPUNIT_NS::Test *_test;
  unsigned int _elapsedMs;

public:
  EndTestTask( ProgressListenerManager *manager, Event *event, CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
    : ProgressListenerManager::TaskBase( manager, event )
    , _test( test )
    , _elapsedMs( elapsed_ms )
  {}

  virtual void run()
  {
    m_manager->endTestOnMainThread( _test, _elapsedMs );
  }
};

void
ProgressListenerManager::endTest( CPPUNIT_NS::Test *test )
{
  Runner *runner = Runner::instance();
  // 在这记录用例耗时，避免把线程切换的时间也计算在内
  TestRecord &record = m_testRecord.top();
  unsigned int elapsedMs = ( unsigned int )( Runner::tickCount() - record.startMs );

  Event *event = Event::createInstance();
  EndTestTask *task = new EndTestTask( this, event, test, elapsedMs );
  runner->asyncRunOnMainThread( task, true );
  event->wait();
  event->destroy();
}

void
ProgressListenerManager::endTestOnMainThread( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  TestRecord &record = m_testRecord.top();
  TestProgressListeners::iterator it = m_listeners.begin();
  while ( it != m_listeners.end() )
  {
    ( *it )->onTestEnd( test, record.errors, record.failures, elapsed_ms );
    ++it;
  }

  m_testRecord.pop();
}

CUTEST_NS_END
