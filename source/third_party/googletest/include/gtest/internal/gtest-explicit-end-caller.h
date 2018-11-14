#pragma once

#include <cppunit/Protector.h>
#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Helper.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

namespace testing { // _CUTEST_IMPL

/*
  msTimeout 单位：毫秒，默认为0，表示永不超时；
  用例启动之后，如果msTimeout之后还没有结束，则视为超时，
  此时CUTEST_NS::Runner会调用Fixture::EndTest()来结束用例。
*/
template <class Fixture, unsigned int msTimeout = 0>
class ExplicitEndTestCaller
  : public CUTEST_NS::Runnable
  , public CPPUNIT_NS::TestCase {
  typedef void (Fixture::*TestMethod)();
  class MethodFunctor : public CPPUNIT_NS::Functor {
   public:
    typedef void (ExplicitEndTestCaller::*Method)();

    MethodFunctor(ExplicitEndTestCaller* target, Method method)
      : m_target(target)
      , m_method(method)
    {}

    bool operator()() const {
      (m_target->*m_method)();
      return true;
    }

   private:
    ExplicitEndTestCaller* m_target;
    Method m_method;
  };

 public:
  ExplicitEndTestCaller(std::string name, TestMethod test)
    : TestCase(name)
    , m_fixture(NULL)
    , m_test(test)
    , m_fixtureMethodId(FIXTURE_METHOD_ID_NONE)
    , m_event(NULL)
  {}

  virtual ~ExplicitEndTestCaller() {
    if (m_fixture) {
      delete m_fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest() override {
    RunTestOnMainThread();
  }

  void RunTestOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result)
        m_result->protect(MethodFunctor(this, &ExplicitEndTestCaller::RunTestImmediately),
                          this);
      else {
        RunTestImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEST;
      m_event = CUTEST_NS::Event::CreateInstance();
      CUTEST_NS::Runner::Instance()->AsyncRunOnMainThread(this, false);
      m_event->Wait();
      m_event->Destroy();
      m_event = NULL;
    }
  }

  void RunTestImmediately() {
    m_fixture->SetEvent(m_event);
    CUTEST_NS::Runner::Instance()->RegisterExplicitEndTest(m_fixture, msTimeout);
    (m_fixture->*m_test)();
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override {
    SetUpOnMainThread();
  }

  void SetUpOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result) {
        m_result->protect(MethodFunctor(this, &ExplicitEndTestCaller::SetUpImmediately),
                          this, "setUp() failed");
      } else {
        SetUpImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_SET_UP;
      m_event = CUTEST_NS::Event::CreateInstance();
      CUTEST_NS::Runner::Instance()->AsyncRunOnMainThread(this, false);
      m_event->Wait();
      m_event->Destroy();
      m_event = NULL;
    }
  }

  void SetUpImmediately() {
    if (!m_fixture) {
      m_fixture = new Fixture;
    }
    m_fixture->SetUp();
    m_event->Post();
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override {
    TearDownOnMainThread();
  }

  void TearDownOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result) {
        m_result->protect(MethodFunctor(this, &ExplicitEndTestCaller::TearDownImmediately),
                          this, "tearDown() failed");
      } else {
        TearDownImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEAR_DOWN;
      m_event = CUTEST_NS::Event::CreateInstance();
      CUTEST_NS::Runner::Instance()->AsyncRunOnMainThread(this, false);
      m_event->Wait();
      m_event->Destroy();
      m_event = NULL;
    }
  }

  void TearDownImmediately() {
    m_fixture->TearDown();
    delete m_fixture;
    m_fixture = NULL;
    m_event->Post();
  }

  // 实现Runnable::Run()
  virtual void Run() {
    switch (m_fixtureMethodId) {
    case FIXTURE_METHOD_ID_SET_UP:
      SetUpOnMainThread();
      break;
    case FIXTURE_METHOD_ID_TEAR_DOWN:
      TearDownOnMainThread();
      break;
    case FIXTURE_METHOD_ID_TEST:
      RunTestOnMainThread();
      break;
    default:
      break;
    }
  }

 private:
  ExplicitEndTestCaller(const ExplicitEndTestCaller&);
  ExplicitEndTestCaller& operator =(const ExplicitEndTestCaller&);

  Fixture* m_fixture;
  TestMethod m_test;

  // 用于标识在Runnable::Run()方法中要调用的方法
  enum FixtureMethodId {
    FIXTURE_METHOD_ID_NONE = 0,  // 初始值，无意义
    FIXTURE_METHOD_ID_SET_UP,    // m_fixture->SetUp();
    FIXTURE_METHOD_ID_TEAR_DOWN, // m_fixture->TearDown();
    FIXTURE_METHOD_ID_TEST,      // (m_fixture->*m_test)();
  };
  FixtureMethodId m_fixtureMethodId;

  CUTEST_NS::Event* m_event;
};

} // namespace testing { // _CUTEST_IMPL
