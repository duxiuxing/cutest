#pragma once

#include <cppunit/Protector.h>
#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Helper.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

namespace testing {

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
    if (CUTEST_NS::Runner::instance()->isAlwaysCallTestOnMainThread()) {
      runTestOnMainThread();
    } else {
      runTestImmediately();
    }
  }

  void runTestOnMainThread() {
    if (CUTEST_NS::isOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::runTestImmediately),
          this);
      } else {
        runTestImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEST;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void runTestImmediately() {
    (m_fixture->*m_test)();
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override {
    if (CUTEST_NS::Runner::instance()->isAlwaysCallTestOnMainThread()) {
      setUpOnMainThread();
    } else {
      setUpImmediately();
    }
  }

  void setUpOnMainThread() {
    if (CUTEST_NS::isOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::setUpImmediately),
          this,
          "setUp() failed");
      } else {
        setUpImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_SET_UP;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void setUpImmediately() {
    m_fixture = new Fixture;
    m_fixture->SetUp();
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override {
    if (CUTEST_NS::Runner::instance()->isAlwaysCallTestOnMainThread()) {
      tearDownOnMainThread();
    } else {
      tearDownImmediately();
    }
  }

  void tearDownOnMainThread() {
    if (CUTEST_NS::isOnMainThread()) {
      if (m_result) {
        m_result->protect(
          MethodFunctor(this, &TestCaller::tearDownImmediately),
          this,
          "tearDown() failed");
      } else {
        tearDownImmediately();
      }
    } else {
      m_fixtureMethodId = FIXTURE_METHOD_ID_TEAR_DOWN;
      m_event = CUTEST_NS::Event::createInstance();
      CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
      m_event->wait();
      m_event->destroy();
      m_event = NULL;
    }
  }

  void tearDownImmediately() {
    m_fixture->TearDown();
    delete m_fixture;
    m_fixture = NULL;
  }

  // 实现Runnable::run()
  virtual void run() {
    switch (m_fixtureMethodId) {
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
    if (m_event) {
      m_event->post();
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

} // namespace testing {
