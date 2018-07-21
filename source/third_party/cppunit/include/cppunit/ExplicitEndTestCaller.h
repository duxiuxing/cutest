#pragma once

#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

CPPUNIT_NS_BEGIN

/*
  timeout_ms 单位：毫秒，默认为0，表示永不超时；
  用例启动之后，如果timeout_ms之后还没有结束，则视为超时，
  此时CUTEST_NS::Runner会调用Fixture::endTest()来结束用例。
*/
template <class Fixture, unsigned int timeout_ms = 0>
class ExplicitEndTestCaller
  : public CUTEST_NS::Runnable
  , public CPPUNIT_NS::TestCase
{
  typedef void ( Fixture::*TestMethod )();

public:
  ExplicitEndTestCaller( std::string name, TestMethod test )
    : TestCase( name )
    , m_fixture( NULL )
    , m_test( test )
    , m_fixtureMethodId( FIXTURE_METHOD_ID_NONE )
    , m_event( NULL )
  {}

  ~ExplicitEndTestCaller()
  {
    if ( m_fixture )
    {
      delete m_fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest() override
  {
    m_fixtureMethodId = FIXTURE_METHOD_ID_TEST;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override
  {
    m_fixtureMethodId = FIXTURE_METHOD_ID_SET_UP;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override
  {
    m_fixtureMethodId = FIXTURE_METHOD_ID_TEAR_DOWN;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 实现Runnable::run()
  virtual void run()
  {
    switch ( m_fixtureMethodId )
    {
    case FIXTURE_METHOD_ID_SET_UP:
      {
        if ( !m_fixture )
        {
          m_fixture = new Fixture;
        }
        m_fixture->setUp();
        m_event->post();
      }
      break;
    case FIXTURE_METHOD_ID_TEAR_DOWN:
      {
        m_fixture->tearDown();
        m_event->post();
      }
      break;
    case FIXTURE_METHOD_ID_TEST:
      {
        m_fixture->setEvent( m_event );
		CUTEST_NS::Runner::instance()->registerExplicitEndTest( m_fixture, timeout_ms );
        ( m_fixture->*m_test )();
      }
      break;
    default:
      break;
    }
  }

private:
  ExplicitEndTestCaller( const ExplicitEndTestCaller &other );
  ExplicitEndTestCaller &operator =( const ExplicitEndTestCaller &other );

  Fixture *m_fixture;
  TestMethod m_test;

  // 用于标识在run()方法中要调用的方法
  enum FixtureMethodId
  {
    FIXTURE_METHOD_ID_NONE = 0,     // 初始值，无意义
    FIXTURE_METHOD_ID_SET_UP,       // m_fixture->setUp();
    FIXTURE_METHOD_ID_TEAR_DOWN,    // m_fixture->tearDown();
    FIXTURE_METHOD_ID_TEST,         // (m_fixture->*m_test)();
  };
  FixtureMethodId m_fixtureMethodId;

  CUTEST_NS::Event *m_event;
};

CPPUNIT_NS_END
