#include "DecoratorImpl.h"
#include "SynchronizationObjectImpl.h"

#include "cutest/Runner.h"
#include <process.h>

#define GTEST_IMPLEMENTATION_ 1
#include "src/gtest-internal-inl.h"
#undef GTEST_IMPLEMENTATION_

CUTEST_NS_BEGIN

Decorator *
Decorator::createInstance( CPPUNIT_NS::Test *test )
{
  return new DecoratorImpl( test );
}

DecoratorImpl::DecoratorImpl( CPPUNIT_NS::Test *test )
  : TestDecorator( test )
  , m_testResult( new CPPUNIT_NS::SynchronizationObjectImpl(), new CPPUNIT_NS::SynchronizationObjectImpl() )
  , m_resultCollector( new CPPUNIT_NS::SynchronizationObjectImpl() )
  , m_resultXmlPrinter( NULL )
  , m_state( STATE_NONE )
  , m_testRuning( NULL )
  , m_hThread( NULL )
{
  m_testResult.addListener( this );
  m_testResult.addListener( &m_resultCollector );

  m_runCompleted = Event::createInstance();
}

DecoratorImpl::~DecoratorImpl()
{
  if ( m_resultXmlPrinter )
  {
    Runner::instance()->removeListener( m_resultXmlPrinter );
    delete m_resultXmlPrinter;
  }

  m_runCompleted->wait();
  m_runCompleted->destroy();
  m_test = NULL;
}

void
DecoratorImpl::destroy()
{
  delete this;
}

void
DecoratorImpl::addListener( CPPUNIT_NS::TestListener *listener )
{
  m_testResult.addListener( listener );
}

void
DecoratorImpl::start()
{
  if ( STATE_RUNING == m_state )
  {
    return;
  }

  // 根据参数构造TestResultXmlPrinter
  if ( testing::internal::UnitTestOptions::GetOutputFormat() == "xml" )
  {
    m_resultXmlPrinter = new testing::internal::TestResultXmlPrinter(
      testing::internal::UnitTestOptions::GetAbsolutePathToOutputFile().c_str() );

    Runner::instance()->addListener( m_resultXmlPrinter );
  }

  m_state = STATE_RUNING;

  m_runCompleted->reset();

  HANDLE hThread = ( HANDLE )_beginthreadex( NULL, 0,
                   threadFunction, this, CREATE_SUSPENDED, NULL );

  ::SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );

  ::DuplicateHandle( ::GetCurrentProcess(),
                     hThread,
                     ::GetCurrentProcess(),
                     &m_hThread,
                     0,
                     FALSE,
                     DUPLICATE_SAME_ACCESS );

  ::ResumeThread( hThread );
}

UINT __stdcall
DecoratorImpl::threadFunction( LPVOID pThis )
{
  DecoratorImpl *decorator = ( DecoratorImpl * )pThis;

  decorator->runOnWorkerThread();

  return 0;
}

void
DecoratorImpl::runOnWorkerThread()
{
  m_testResult.runTest( this );

  ::CloseHandle( m_hThread );
  m_hThread = NULL;

  m_runCompleted->post();
}

void
DecoratorImpl::stop()
{
  if ( STATE_NONE == m_state )
  {
    return;
  }

  m_state = STATE_NONE;
  m_testResult.stop();
}

const CPPUNIT_NS::TestResultCollector *
DecoratorImpl::testResultCollector()
{
  return &m_resultCollector;
}

void
DecoratorImpl::startTest( CPPUNIT_NS::Test *test )
{
  m_testRuning = test;
}

void
DecoratorImpl::endTest( CPPUNIT_NS::Test *test )
{
  m_testRuning = NULL;
}

void
DecoratorImpl::addFailure( bool is_error, CPPUNIT_NS::Exception *exception )
{
  if ( is_error )
  {
    m_testResult.addError( m_testRuning, exception );
  }
  else
  {
    m_testResult.addFailure( m_testRuning, exception );
  }
}

CUTEST_NS_END
