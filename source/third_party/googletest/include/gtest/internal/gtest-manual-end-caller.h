#pragma once

#ifdef _CUTEST

#include <cppunit/TestCase.h>

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

CPPUNIT_NS_BEGIN

/*
  timeout_ms 单位：毫秒，默认为0，表示永不超时；
  用例启动之后，如果timeout_ms之后还没有结束，则视为超时，
  此时CUTEST::Runner会调用Fixture::endTest()来结束用例。
*/
template <class Fixture, unsigned int timeout_ms = 0>
class GTestManualEndCaller
  : public CUTEST_NS::Runnable
  , public TestCase {
  typedef void (Fixture::*TestBody)();

 public:
  GTestManualEndCaller(std::string name, TestBody test_body_in)
    : TestCase(name)
    , test_body(test_body_in)
    , fixture(NULL)
    , fixture_method_id(FIXTURE_METHOD_ID_NONE)
    , event(NULL)
  {}

  ~GTestManualEndCaller() {
    if (this->fixture) {
      delete this->fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest() override {
    this->fixture_method_id = FIXTURE_METHOD_ID_TEST_BODY;
    this->event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    this->event->wait();
    this->event->destroy();
    this->event = NULL;
  }

  // 重载TestFixture::setUp()
  virtual void setUp() override {
    this->fixture_method_id = FIXTURE_METHOD_ID_SET_UP;
    this->event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    this->event->wait();
    this->event->destroy();
    this->event = NULL;
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown() override {
    this->fixture_method_id = FIXTURE_METHOD_ID_TEAR_DOWN;
    this->event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread(this, false);
    this->event->wait();
    this->event->destroy();
    this->event = NULL;
  }

  // 实现Runnable::run()
  virtual void run() {
    switch (this->fixture_method_id) {
    case FIXTURE_METHOD_ID_SET_UP: {
        if (!this->fixture) {
          this->fixture = new Fixture;
        }
        this->fixture->SetUp();
        this->event->post();
      }
      break;
    case FIXTURE_METHOD_ID_TEAR_DOWN: {
        this->fixture->TearDown();
        this->event->post();
      }
      break;
    case FIXTURE_METHOD_ID_TEST_BODY: {
        this->fixture->setEvent(this->event);
        CUTEST_NS::Runner::instance()->registerManualEndTest(this->fixture, timeout_ms);
        (this->fixture->*test_body)();
      }
      break;
    default:
      break;
    }
  }

 private:
  GTestManualEndCaller(const GTestManualEndCaller&);
  GTestManualEndCaller& operator =(const GTestManualEndCaller&);

  TestBody test_body;
  Fixture* fixture;

  // 用于标识在run()方法中要调用的方法
  enum FixtureMethodId {
    FIXTURE_METHOD_ID_NONE = 0,     // 初始值，无意义
    FIXTURE_METHOD_ID_SET_UP,       // this->fixture->SetUp();
    FIXTURE_METHOD_ID_TEAR_DOWN,    // this->fixture->TearDown();
    FIXTURE_METHOD_ID_TEST_BODY,    // (this->fixture->*test_body)();
  };
  FixtureMethodId fixture_method_id;

  CUTEST_NS::Event* event;
};

CPPUNIT_NS_END

#endif // #ifdef _CUTEST
