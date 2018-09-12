#pragma once

#include "cutest/Event.h"
#include "cutest/MfcTestDialog.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

#ifdef _MSC_VER
CPPUNIT_NS_BEGIN

/*
  timeout_ms 单位：毫秒，默认为0，表示永不超时；
  用例启动之后，如果timeout_ms之后还没有结束，则视为超时，
  此时CUTEST_NS::Runner会调用Fixture::endTest()来结束用例。
*/
template<class Fixture, class DialogClass, unsigned int timeout_ms = 0>
class MfcTestDialogCaller
  : public CUTEST_NS::Runnable
  , public CPPUNIT_NS::TestCase
{
  class MethodFunctor : public CPPUNIT_NS::Functor
  {
  public:
    typedef void ( MfcTestDialogCaller::*Method )();

    MethodFunctor( MfcTestDialogCaller *target, Method method )
      : m_target( target )
      , m_method( method )
    {}

    bool operator()() const
    {
      ( m_target->*m_method )();
      return true;
    }

  private:
    MfcTestDialogCaller *m_target;
    Method m_method;
  };

public:
  MfcTestDialogCaller( std::string name )
    : TestCase( name )
    , m_fixture( NULL )
    , m_fixtureMethodId( FIXTURE_METHOD_ID_NONE )
    , m_event( NULL )
  {}

  ~MfcTestDialogCaller()
  {
    if ( m_fixture )
    {
      delete m_fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest() override
  {
    runTestOnMainThread();
  }

  void runTestOnMainThread()
  {
    if ( CUTEST_NS::isOnMainThread() )
    {
      if ( m_result )
        m_result->protect( MethodFunctor( this, &MfcTestDialogCaller::runTestImmediately ),
                           this );
      else
      {
        runTestImmediately();
      }
    }
    else
    {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEST;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void runTestImmediately()
  {
    m_fixture->setEvent( m_event );
    CUTEST_NS::Runner::instance()->registerExplicitEndTest( m_fixture, timeout_ms );
    AFX_MANAGE_STATE( AfxGetStaticModuleState() );
    CUTEST_NS::CMfcTestDialog<DialogClass> *testDlg = new CUTEST_NS::CMfcTestDialog<DialogClass>( m_fixture );
    testDlg->Create( DialogClass::IDD );
    testDlg->ShowWindow( SW_NORMAL );
    m_fixture->attach( testDlg->GetSafeHwnd() );
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override
  {
    setUpOnMainThread();
  }

  void setUpOnMainThread()
  {
    if ( CUTEST_NS::isOnMainThread() )
    {
      if ( m_result )
      {
        m_result->protect( MethodFunctor( this, &MfcTestDialogCaller::setUpImmediately ),
                           this, "setUp() failed" );
      }
      else
      {
        setUpImmediately();
      }
    }
    else
    {
      m_fixtureMethodId = FIXTURE_METHOD_ID_SET_UP;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void setUpImmediately()
  {
    if ( !m_fixture )
    {
      m_fixture = new Fixture;
    }
    m_fixture->setUp();
    m_event->post();
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override
  {
    tearDownOnMainThread();
  }

  void tearDownOnMainThread()
  {
    if ( CUTEST_NS::isOnMainThread() )
    {
      if ( m_result )
      {
        m_result->protect( MethodFunctor( this, &MfcTestDialogCaller::tearDownImmediately ),
                           this, "tearDown() failed" );
      }
      else
      {
        tearDownImmediately();
      }
    }
    else
    {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEAR_DOWN;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void tearDownImmediately()
  {
    m_fixture->tearDown();
    delete m_fixture;
    m_fixture = NULL;
    m_event->post();
  }

  // 实现Runnable::run()
  virtual void run()
  {
    switch ( m_fixtureMethodId )
    {
    case FIXTURE_METHOD_ID_SET_UP:
      setUpOnMainThread();
      break;
    case FIXTURE_METHOD_ID_TEAR_DOWN:
      tearDownOnMainThread();
      break;
    case FIXTURE_METHOD_ID_TEST:
      runTestOnMainThread();
      break;
    default:
      break;
    }
  }

private:
  MfcTestDialogCaller( const MfcTestDialogCaller &other );
  MfcTestDialogCaller &operator =( const MfcTestDialogCaller &other );

  Fixture *m_fixture;

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
#endif // #ifdef _MSC_VER
