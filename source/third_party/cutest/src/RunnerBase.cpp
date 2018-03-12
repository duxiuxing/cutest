#include "RunnerBase.h"

#include "cutest/ManualEndTest.h"
#include "cutest/Runnable.h"

#include "gtest/gtest-message.h"

CUTEST_NS_BEGIN

const char *
Runner::version()
{
  static std::string s_version;

  if ( s_version.empty() )
  {
    if ( 0 == BUILD_NUMBER )
    {
      // 本地编译会进入这个分支，这个时候取当前时刻作为版本号。
      s_version = __TIME__;
      s_version.append( " " );
      s_version.append( __DATE__ );
    }
    else
    {
      // 持续集成会进入这个分支，这个时候按照实际版本信息来拼接版本号。
      int major   = MAJOR_VERSION;
      int minor   = MINOR_VERSION;
      int fix     = FIX_VERSION;
      int build   = BUILD_NUMBER;
      s_version = testing::internal::StreamableToString( major ) + "."
                  + testing::internal::StreamableToString( minor ) + "."
                  + testing::internal::StreamableToString( fix ) + "."
                  + testing::internal::StreamableToString( build );
    }
  }

  return s_version.c_str();
}

RunnerBase::RunnerBase()
  : m_testDecorator( NULL )
  , m_testRuning( NULL )
{}

RunnerBase::~RunnerBase()
{
  stop();

  if ( m_testDecorator )
  {
    m_testDecorator->destroy();
    m_testDecorator = NULL;
  }
}

void
RunnerBase::addListener( ProgressListener *listener )
{
  m_listenerManager.add( listener );
}

void
RunnerBase::removeListener( ProgressListener *listener )
{
  m_listenerManager.remove( listener );
}

void
RunnerBase::start( CPPUNIT_NS::Test *test )
{
  stop();

  if ( m_testDecorator )
  {
    m_testDecorator->destroy();
    m_testDecorator = NULL;
  }

  m_testDecorator = Decorator::createInstance( test );
  m_testDecorator->addListener( &m_listenerManager );
  m_testDecorator->start();
}

void
RunnerBase::stop()
{
  if ( m_testDecorator )
  {
    m_testDecorator->stop();

    if ( m_testRuning )
    {
      m_testRuning->endTest();
      m_testRuning = NULL;
    }
  }
}

void
RunnerBase::addFailure( bool is_error, CPPUNIT_NS::Exception *exception )
{
  if ( m_testDecorator )
  {
    m_testDecorator->addFailure( is_error, exception );
  }
}

unsigned int
RunnerBase::errorCount() const
{
  if ( m_testDecorator )
  {
    const CPPUNIT_NS::TestResultCollector *resultCollector = m_testDecorator->testResultCollector();
    return ( unsigned int )resultCollector->testErrors();
  }
  return 0;
}

unsigned int
RunnerBase::failureCount() const
{
  if ( m_testDecorator )
  {
    const CPPUNIT_NS::TestResultCollector *resultCollector = m_testDecorator->testResultCollector();
    return ( unsigned int )resultCollector->testFailures();
  }
  return 0;
}

unsigned int
RunnerBase::totalFailureCount() const
{
  if ( m_testDecorator )
  {
    const CPPUNIT_NS::TestResultCollector *resultCollector = m_testDecorator->testResultCollector();
    return ( unsigned int )resultCollector->testFailuresTotal();
  }
  return 0;
}

const CPPUNIT_NS::TestFailure *
RunnerBase::failureAt( unsigned int index ) const
{
  if ( m_testDecorator )
  {
    const CPPUNIT_NS::TestResultCollector *resultCollector = m_testDecorator->testResultCollector();
    if ( index < resultCollector->failures().size() )
    {
      return resultCollector->failures()[index];
    }
  }
  return NULL;
}

void
RunnerBase::registerManualEndTest( ManualEndTest *test, unsigned int timeout_ms )
{
  m_testRuning = test;
  m_autoEndTest.check( test, timeout_ms );
}

void
RunnerBase::unregisterManualEndTest( ManualEndTest *test )
{
  if ( m_testRuning == test )
  {
    m_testRuning = NULL;
    m_autoEndTest.cancel();
  }
}

CUTEST_NS_END
