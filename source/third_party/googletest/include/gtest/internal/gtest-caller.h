#pragma once

#include <cppunit/Protector.h>
#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Helper.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

namespace testing { // _CUTEST_IMPL

template <class Fixture>
class TestCaller
  : public CUTEST_NS::Runnable
  , public CPPUNIT_NS::TestCase {
  typedef void (Fixture::*TestMethod)();
  class MethodFunctor : public CPPUNIT_NS::Functor {
   public:
    typedef void (TestCaller::*Method)();

    MethodFunctor(TestCaller* target, Method method)
      : m_target(target)
      , m_method(method)
    {}

    bool operator()() const {
      (m_target->*m_method)();
      return true;
    }

   private:
    TestCaller* m_target;
    Method m_method;
  };

 public:
  TestCaller(std::string name, TestMethod test)
    : TestCase(name)
    , m_fixture(NULL)
    , m_test(test)
    , m_fixtureMethodId(FIXTURE_METHOD_ID_NONE)
    , m_event(NULL) {
  }

  virtual ~TestCaller() {
    if (m_fixture) {
      delete m_fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest() override {
    if (CUTEST_NS::Runner::Instance()->IsAlwaysCallTestOnMainThread()) {
      RunTestOnMainThread();
    } else {
      RunTestImmediately();
    }
  }

  void RunTestOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::RunTestImmediately),
          this);
      } else {
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
    (m_fixture->*m_test)();
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override {
    if (CUTEST_NS::Runner::Instance()->IsAlwaysCallTestOnMainThread()) {
      SetUpOnMainThread();
    } else {
      SetUpImmediately();
    }
  }

  void SetUpOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::SetUpImmediately),
          this,
          "setUp() failed");
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
    m_fixture = new Fixture;
    m_fixture->SetUp();
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override {
    if (CUTEST_NS::Runner::Instance()->IsAlwaysCallTestOnMainThread()) {
      TearDownOnMainThread();
    } else {
      TearDownImmediately();
    }
  }

  void TearDownOnMainThread() {
    if (CUTEST_NS::IsOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::TearDownImmediately),
          this,
          "tearDown() failed");
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
    if (m_event) {
      m_event->Post();
    }
  }

 private:
  TestCaller(const TestCaller&);
  TestCaller& operator =(const TestCaller&);

  Fixture* m_fixture;
  TestMethod m_test;

  // 用于标识在run()方法中要调用的方法
  enum FixtureMethodId {
    FIXTURE_METHOD_ID_NONE = 0,     // 初始值，无意义
    FIXTURE_METHOD_ID_SET_UP,       // m_fixture->SetUp();
    FIXTURE_METHOD_ID_TEAR_DOWN,    // m_fixture->TearDown();
    FIXTURE_METHOD_ID_TEST,         // (m_fixture->*m_test)();
  };
  FixtureMethodId m_fixtureMethodId;

  CUTEST_NS::Event* m_event;
};

} // namespace testing { // _CUTEST_IMPL
