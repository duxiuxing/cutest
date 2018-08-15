#include "GTestWaitAsynEndTest.h"
//#include <Windows.h>

GTestWaitAsynEndTest::GTestWaitAsynEndTest() {}

void GTestWaitAsynEndTest::SetUp() {
  time_up = new CountDownLatch(1);
}

void GTestWaitAsynEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
  delete time_up;
}

void GTestWaitAsynEndTest::onTimeUp() {
  time_up->countDown();
}

TEST_F(GTestWaitAsynEndTest, end_test_after_1s) { 
  unsigned long long start = CUTEST_NS::Runner::instance()->tickCount64();

  SimpleTimer::instance()->setCallback(1000, this);

  time_up->await();

  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount64() - start;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
