#include "gtest/gtest.h"
#include "cutest/count_down_latch.h"
#include "simple_timer.h"

class CountDownLatchTest
  : public testing::Test
  , public SimpleTimer::Callback {
 public:
  CountDownLatchTest();

  virtual void SetUp() override;
  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void OnTimeUp();

  CUTEST_NS::CountDownLatch* m_countDownLatch;
};

CountDownLatchTest::CountDownLatchTest()
  : m_countDownLatch(NULL) {}

void CountDownLatchTest::SetUp() {
  m_countDownLatch = new CUTEST_NS::CountDownLatch(1);
}

void CountDownLatchTest::TearDown() {
  SimpleTimer::Instance()->RemoveCallback(this);
  delete m_countDownLatch;
  m_countDownLatch = NULL;
}

void CountDownLatchTest::OnTimeUp() {
	m_countDownLatch->CountDown();
}

TEST_F(CountDownLatchTest, EndTestAfterOneSecond) {
  unsigned long long msStart = CUTEST_NS::TickCount64();

  SimpleTimer::Instance()->SetCallback(1000, this);

  m_countDownLatch->Await();

  unsigned long long ms = CUTEST_NS::TickCount64() - msStart;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}
