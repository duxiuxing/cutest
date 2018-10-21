#include "gtest/gtest.h"
#include "cutest/count_down_latch.h"
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

  CUTEST_NS::CountDownLatch* countDownLatch;
};

GTestWaitAsynEndTest::GTestWaitAsynEndTest()
  : countDownLatch(NULL) {}

void GTestWaitAsynEndTest::SetUp() {
  this->countDownLatch = new CUTEST_NS::CountDownLatch(1);
}

void GTestWaitAsynEndTest::TearDown() {
  SimpleTimer::instance()->removeCallback(this);
  delete this->countDownLatch;
  this->countDownLatch = NULL;
}

void GTestWaitAsynEndTest::onTimeUp() {
	this->countDownLatch->countDown();
}

TEST_F(GTestWaitAsynEndTest, EndTestAfterOneSecond) {
  unsigned long long msStart = CUTEST_NS::tickCount64();

  SimpleTimer::instance()->setCallback(1000, this);

  countDownLatch->await();

  unsigned long long ms = CUTEST_NS::tickCount64() - msStart;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
