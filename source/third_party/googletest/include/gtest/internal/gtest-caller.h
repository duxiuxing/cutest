#pragma once

#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

namespace testing {

template <class Fixture>
class TestCaller
  : public CUTEST_NS::Runnable
  , public CPPUNIT_NS::TestCase {
  typedef void (Fixture::*TestMethod)();

 public:
  TestCaller(std::string name, TestMethod test)
    : TestCase(name)
	, m_fixture(NULL)
    , m_test(test)    
    , m_fixtureMethodId(FIXTURE_METHOD_ID_NONE)
    , m_event(NULL) {
  }

  // 重载TestCase::runTest()
  virtual void runTest() override {
    m_fixtureMethodId = FIXTURE_METHOD_ID_TEST;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override {
    m_fixtureMethodId = FIXTURE_METHOD_ID_SET_UP;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override {
    m_fixtureMethodId = FIXTURE_METHOD_ID_TEAR_DOWN;
    m_event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    m_event->wait();
    m_event->destroy();
    m_event = NULL;
  }

  // 实现Runnable::run()
  virtual void run() {
    switch (m_fixtureMethodId) {
    case FIXTURE_METHOD_ID_SET_UP: {
        m_fixture = new Fixture;
        m_fixture->SetUp();
      }
      break;
    case FIXTURE_METHOD_ID_TEAR_DOWN: {
        m_fixture->TearDown();
        delete m_fixture;
        m_fixture = NULL;
      }
      break;
    case FIXTURE_METHOD_ID_TEST: {
        (m_fixture->*m_test)();
      }
      break;
    default:
      break;
    }
    m_event->post();
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
