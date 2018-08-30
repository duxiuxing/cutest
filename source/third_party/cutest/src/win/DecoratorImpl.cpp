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
  , test_result( new CPPUNIT_NS::SynchronizationObjectImpl(), new CPPUNIT_NS::SynchronizationObjectImpl() )
  , result_collector( new CPPUNIT_NS::SynchronizationObjectImpl() )
  , result_printer( NULL )
  , runing_test( NULL )
  , thread_handle( NULL )
{
  test_result.addListener( this );
  test_result.addListener( &this->result_collector );

  this->run_completed = Event::createInstance();
}

DecoratorImpl::~DecoratorImpl()
{
  if ( this->result_printer )
  {
    Runner::instance()->removeListener( this->result_printer );
    delete this->result_printer;
  }

  this->run_completed->wait();
  this->run_completed->destroy();
  CPPUNIT_NS::TestDecorator::m_test = NULL;
}

void
DecoratorImpl::destroy()
{
  delete this;
}

void
DecoratorImpl::addListener( CPPUNIT_NS::TestListener *listener )
{
  test_result.addListener( listener );
}

void
DecoratorImpl::start()
{
  // 根据参数构造TestResultXmlPrinter
  if ( testing::internal::UnitTestOptions::GetOutputFormat() == "xml" )
  {
    this->result_printer = new testing::internal::TestResultXmlPrinter(
      testing::internal::UnitTestOptions::GetAbsolutePathToOutputFile().c_str() );

    Runner::instance()->addListener( this->result_printer );
  }

  this->run_completed->reset();

  HANDLE source_handle = ( HANDLE )_beginthreadex( NULL, 0,
                         threadFunction, this, CREATE_SUSPENDED, NULL );

  ::SetThreadPriority( source_handle, THREAD_PRIORITY_NORMAL );

  ::DuplicateHandle( ::GetCurrentProcess(),
                     source_handle,
                     ::GetCurrentProcess(),
                     &this->thread_handle,
                     0,
                     FALSE,
                     DUPLICATE_SAME_ACCESS );

  ::ResumeThread( source_handle );
}

UINT
__stdcall
DecoratorImpl::threadFunction( LPVOID pThis )
{
  DecoratorImpl *decorator = ( DecoratorImpl * )pThis;

  decorator->runOnWorkerThread();

  return 0;
}

void
DecoratorImpl::runOnWorkerThread()
{
  test_result.runTest( this );

  ::CloseHandle( this->thread_handle );
  this->thread_handle = NULL;

  this->run_completed->post();
}

void
DecoratorImpl::stop()
{
  test_result.stop();
}

const CPPUNIT_NS::TestResultCollector *
DecoratorImpl::testResultCollector()
{
  return &this->result_collector;
}

void
DecoratorImpl::startTest( CPPUNIT_NS::Test *test )
{
  this->runing_test = test;
}

void
DecoratorImpl::endTest( CPPUNIT_NS::Test *test )
{
  this->runing_test = NULL;
}

void
DecoratorImpl::addFailure( bool is_error, CPPUNIT_NS::Exception *exception )
{
  if ( is_error )
  {
    test_result.addError( this->runing_test, exception );
  }
  else
  {
    test_result.addFailure( this->runing_test, exception );
  }
}

CUTEST_NS_END
