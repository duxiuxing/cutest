#include "GTestExplicitEndTest.h"

void
GTestExplicitEndTest::run() {
  // 这里才结束测试用例
  endTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, explicit_end_test_after_1s) {
  // 框架会在主线程调用此方法
  // 1s之后再调用EndTest()来结束用例
  CUTEST_NS::Runner::instance()->delayRunOnMainThread(1000, this, false);
}
