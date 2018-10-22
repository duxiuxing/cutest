#include "gtest/gtest.h"
#include "cutest/count_down_latch.h"
#include "simple_timer.h"

class GTestWaitAsynEndTest
  : public testing::Test
  , public SimpleTimer::Callback {
 public:
  GTestWaitAsynEndTest();

  virtual void SetUp() override;
  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void OnTimeUp();

  CUTEST_NS::CountDownLatch* m_countDownLatch;
};

GTestWaitAsynEndTest::GTestWaitAsynEndTest()
  : m_countDownLatch(NULL) {}

void GTestWaitAsynEndTest::SetUp() {
  m_countDownLatch = new CUTEST_NS::CountDownLatch(1);
}

void GTestWaitAsynEndTest::TearDown() {
  SimpleTimer::Instance()->RemoveCallback(this);
  delete m_countDownLatch;
  m_countDownLatch = NULL;
}

void GTestWaitAsynEndTest::OnTimeUp() {
	m_countDownLatch->CountDown();
}

TEST_F(GTestWaitAsynEndTest, EndTestAfterOneSecond) {
  unsigned long long msStart = CUTEST_NS::TickCount64();

  SimpleTimer::Instance()->SetCallback(1000, this);

  m_countDownLatch->Await();

  unsigned long long ms = CUTEST_NS::TickCount64() - msStart;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
