#include "GTestExplicitEndTest.h"

GTestExplicitEndTest::GTestExplicitEndTest()
  : tick_count_setup(0) {
}

void GTestExplicitEndTest::SetUp() {
  this->tick_count_setup = CUTEST_NS::Runner::instance()->tickCount64();
}

void GTestExplicitEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount64() - this->tick_count_setup;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}

void GTestExplicitEndTest::onTimeUp() {
  // 这里才结束测试用例
  endTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, explicit_end_test_after_1s) {
  SimpleTimer::instance()->setCallback(1000, this);
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(GTestExplicitEndTest, auto_end_test_after_1s, 1000) {}
