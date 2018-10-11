#include "gtest/gtest.h"
#include "SimpleTimer.h"

class GTestExplicitEndTest
  : public testing::ExplicitEndTest
  , public SimpleTimer::Callback {
 public:
  GTestExplicitEndTest();

  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  unsigned long long msStart;
};

GTestExplicitEndTest::GTestExplicitEndTest()
  : msStart(0) {}

void GTestExplicitEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
}

void GTestExplicitEndTest::onTimeUp() {
  unsigned long long ms = CUTEST_NS::tickCount64() - this->msStart;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
  endTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, EndTestAfterOneSecond) {
  this->msStart = CUTEST_NS::tickCount64();
  SimpleTimer::instance()->setCallback(1000, this);
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(GTestExplicitEndTest, AutoEndTestAfterOneSecond, 1000) {}
