#include "GTestWaitAsynEndTest.h"
#include <Windows.h>

GTestWaitAsynEndTest::GTestWaitAsynEndTest()
  : tick_count_setup(0)
  , time_up(false) {
}

void GTestWaitAsynEndTest::SetUp() {
  this->tick_count_setup = CUTEST_NS::Runner::instance()->tickCount64();
}

void GTestWaitAsynEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
}

void GTestWaitAsynEndTest::onTimeUp() {
  this->time_up = true;
}

TEST_F(GTestWaitAsynEndTest, end_test_after_1s) {
  this->time_up = false;
  SimpleTimer::instance()->setCallback(1000, this);

  MSG msg = {0};
  while (!this->time_up && ::GetMessage(&msg, NULL, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount64() - this->tick_count_setup;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
