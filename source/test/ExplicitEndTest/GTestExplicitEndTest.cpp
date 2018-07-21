#include "GTestExplicitEndTest.h"

GTestExplicitEndTest::GTestExplicitEndTest()
  : tick_count_setup(0) {
}

void GTestExplicitEndTest::SetUp() {
  this->tick_count_setup = CUTEST_NS::Runner::instance()->tickCount();
}

void GTestExplicitEndTest::TearDown() {
  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount() - this->tick_count_setup;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1050);
}

void GTestExplicitEndTest::run() {
  // 这里才结束测试用例
  endTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, explicit_end_test_after_1s) {
  // 框架会在主线程调用此方法
  // 1s之后再调用EndTest()来结束用例
  CUTEST_NS::Runner::instance()->delayRunOnMainThread(1000, this, false);
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(GTestExplicitEndTest, auto_end_test_after_1s, 1000) {
}
