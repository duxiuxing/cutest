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

  unsigned long long tick_count_start;
};

GTestExplicitEndTest::GTestExplicitEndTest()
  : tick_count_start(0) {}

void GTestExplicitEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
}

void GTestExplicitEndTest::onTimeUp() {
  unsigned long long ms = CUTEST_NS::tickCount64() - this->tick_count_start;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
  endTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, explicit_end_test_after_1s) {
  this->tick_count_start = CUTEST_NS::tickCount64();
  SimpleTimer::instance()->setCallback(1000, this);
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(GTestExplicitEndTest, auto_end_test_after_1s, 1000) {}
