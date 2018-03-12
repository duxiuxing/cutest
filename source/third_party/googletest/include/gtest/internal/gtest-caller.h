#pragma once

#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

CPPUNIT_NS_BEGIN

template <class T>
class GTestCaller
  : public CUTEST_NS::Runnable
  , public TestCase
{
  typedef void ( T::*TestMethod )();

public:
  GTestCaller( std::string name, TestMethod test_method )
    : TestCase( name )
    , m_testMethod( test_method )
    , m_test( NULL )
    , m_asyncRunMethod( ASYNC_RUN_NONE )
    , m_event( NULL )
  {}

  // 重载TestCase::runTest()
  virtual void runTest() override
  {
    m_asyncRunMethod = ASYNC_RUN_TEST;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override
  {
    m_asyncRunMethod = ASYNC_RUN_SET_UP;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override
  {
    m_asyncRunMethod = ASYNC_RUN_TEAR_DOWN;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 实现Runnable::run()
  virtual void run()
  {
    switch ( m_asyncRunMethod )
    {
    case ASYNC_RUN_SET_UP:
      {
        m_test = new T;
        m_test->SetUp();
      }
      break;
    case ASYNC_RUN_TEAR_DOWN:
      {
        m_test->TearDown();
        delete m_test;
        m_test = NULL;
      }
      break;
    case ASYNC_RUN_TEST:
      {
        ( m_test->*m_testMethod )();
      }
      break;
    default:
      break;
    }
    m_event->post();
  }

private:
  GTestCaller( const GTestCaller & );
  GTestCaller &operator =( const GTestCaller & );

  TestMethod m_testMethod;
  T *m_test;

  // 用于标识在run()方法中要调用的方法
  enum AsyncRunMethod
  {
    ASYNC_RUN_NONE = 0,     // 初始值，无意义
    ASYNC_RUN_SET_UP,       // m_test->SetUp();
    ASYNC_RUN_TEAR_DOWN,    // m_test->TearDown();
    ASYNC_RUN_TEST,         // (m_test->*m_testMethod)();
  };
  AsyncRunMethod m_asyncRunMethod;

  CUTEST_NS::Event *m_event;
};

CPPUNIT_NS_END
