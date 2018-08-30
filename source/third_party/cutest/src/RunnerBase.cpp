#include "RunnerBase.h"

#include "cutest/ExplicitEndTest.h"
#include "cutest/Runnable.h"

#include "gmock/gmock.h"
#include "gtest/gtest-message.h"

CUTEST_NS_BEGIN

void
RunnerBase::initGoogleMock()
{
  static bool init_once = true;
  if ( init_once )
  {
    init_once = false;
    testing::InitGoogleMock( &__argc, __wargv );
  }
}

RunnerBase::RunnerBase()
  : test_decorator( NULL )
  , runing_test( NULL )
  , always_call_test_on_main_thread( false )
  , treat_timeout_as_error( false )
  , state( STATE_NONE )
{
  addListener( this );
}

RunnerBase::~RunnerBase()
{
  stop();

  if ( this->test_decorator )
  {
    this->test_decorator->destroy();
    this->test_decorator = NULL;
  }
}

void
RunnerBase::setAlwaysCallTestOnMainThread( bool value )
{
  this->always_call_test_on_main_thread = value;
}

bool
RunnerBase::alwaysCallTestOnMainThread()
{
  return this->always_call_test_on_main_thread;
}

void
RunnerBase::setTreatTimeoutAsError( bool value )
{
  this->treat_timeout_as_error = value;
}

bool
RunnerBase::treatTimeoutAsError()
{
  return this->treat_timeout_as_error;
}

void
RunnerBase::addListener( ProgressListener *listener )
{
  this->listener_manager.add( listener );
}

void
RunnerBase::removeListener( ProgressListener *listener )
{
  this->listener_manager.remove( listener );
}

void
RunnerBase::start( CPPUNIT_NS::Test *test )
{
  switch ( this->state )
  {
  case STATE_NONE:
    this->state = STATE_RUNING;
    break;
  case STATE_RUNING:
    return;
  case STATE_STOPPING:
    return;
  default:
    return;
  }

  if ( this->test_decorator )
  {
    this->test_decorator->destroy();
    this->test_decorator = NULL;
  }

  this->test_decorator = Decorator::createInstance( test );
  this->test_decorator->addListener( &this->listener_manager );
  this->test_decorator->start();
}

void
RunnerBase::stop()
{
  switch ( this->state )
  {
  case STATE_NONE:
    return;
  case STATE_RUNING:
    this->state = STATE_STOPPING;
    break;
  case STATE_STOPPING:
    return;
  default:
    return;
  }

  if ( this->test_decorator )
  {
    this->test_decorator->stop();

    if ( this->runing_test )
    {
      this->runing_test->endTest();
      this->runing_test = NULL;
    }
  }
}

void
RunnerBase::addFailure( bool is_error, CPPUNIT_NS::Exception *exception )
{
  if ( this->test_decorator )
  {
    this->test_decorator->addFailure( is_error, exception );
  }
}

unsigned int
RunnerBase::errorCount() const
{
  if ( this->test_decorator )
  {
    const CPPUNIT_NS::TestResultCollector *collector = this->test_decorator->testResultCollector();
    return ( unsigned int )collector->testErrors();
  }
  return 0;
}

unsigned int
RunnerBase::failureCount() const
{
  if ( this->test_decorator )
  {
    const CPPUNIT_NS::TestResultCollector *collector = this->test_decorator->testResultCollector();
    return ( unsigned int )collector->testFailures();
  }
  return 0;
}

unsigned int
RunnerBase::totalFailureCount() const
{
  if ( this->test_decorator )
  {
    const CPPUNIT_NS::TestResultCollector *collector = this->test_decorator->testResultCollector();
    return ( unsigned int )collector->testFailuresTotal();
  }
  return 0;
}

const CPPUNIT_NS::TestFailure *
RunnerBase::failureAt( unsigned int index ) const
{
  if ( this->test_decorator )
  {
    const CPPUNIT_NS::TestResultCollector *collector = this->test_decorator->testResultCollector();
    if ( index < collector->failures().size() )
    {
      return collector->failures()[index];
    }
  }
  return NULL;
}

void
RunnerBase::registerExplicitEndTest( ExplicitEndTest *test, unsigned int timeout_ms )
{
  this->runing_test = test;
  this->auto_end_test.check( test, timeout_ms );
}

void
RunnerBase::unregisterExplicitEndTest( ExplicitEndTest *test )
{
  if ( this->runing_test == test )
  {
    this->runing_test = NULL;
    this->auto_end_test.cancel();
  }
}

void
RunnerBase::onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  this->state = STATE_NONE;
}

CUTEST_NS_END
