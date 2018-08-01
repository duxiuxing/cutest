#include "RunnerBase.h"

#include "cutest/ExplicitEndTest.h"
#include "cutest/Runnable.h"

#include "gtest/gtest-message.h"

CUTEST_NS_BEGIN

const char *
Runner::version()
{
  static std::string version;

  if ( version.empty() )
  {
    if ( 0 == BUILD_NUMBER )
    {
      // 本地编译会进入这个分支，这个时候取当前时刻作为版本号。
      version = __TIME__;
      version.append( " " );
      version.append( __DATE__ );
    }
    else
    {
      // 持续集成会进入这个分支，这个时候按照实际版本信息来拼接版本号。
      int major   = MAJOR_VERSION;
      int minor   = MINOR_VERSION;
      int fix     = FIX_VERSION;
      int build   = BUILD_NUMBER;
      version = testing::internal::StreamableToString( major ) + "."
                + testing::internal::StreamableToString( minor ) + "."
                + testing::internal::StreamableToString( fix ) + "."
                + testing::internal::StreamableToString( build );
    }
  }

  return version.c_str();
}

RunnerBase::RunnerBase()
  : test_decorator( NULL )
  , runing_test( NULL )
  , always_call_test_on_main_thread( false )
{}

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
  stop();

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

CUTEST_NS_END
