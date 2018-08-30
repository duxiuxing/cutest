#include "gtest/gtest.h"
#include "cutest/CountDownLatch.h"
#include "SimpleTimer.h"

class GTestWaitAsynEndTest
  : public testing::Test
  , public SimpleTimer::Callback {
 public:
  GTestWaitAsynEndTest();

  virtual void SetUp() override;
  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  CUTEST_NS::CountDownLatch* time_up;
};

GTestWaitAsynEndTest::GTestWaitAsynEndTest()
  : time_up(NULL) {}

void GTestWaitAsynEndTest::SetUp() {
  time_up = new CUTEST_NS::CountDownLatch(1);
}

void GTestWaitAsynEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
  delete time_up;
}

void GTestWaitAsynEndTest::onTimeUp() {
  time_up->countDown();
}

TEST_F(GTestWaitAsynEndTest, end_test_after_1s) {
  unsigned long long start = CUTEST_NS::tickCount64();

  SimpleTimer::instance()->setCallback(1000, this);

  time_up->await();

  unsigned long long ms = CUTEST_NS::tickCount64() - start;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
