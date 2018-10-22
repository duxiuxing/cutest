#include "gtest/gtest.h"
#include "simple_timer.h"

class GTestExplicitEndTest
  : public testing::ExplicitEndTest
  , public SimpleTimer::Callback {
 public:
  GTestExplicitEndTest();

  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void OnTimeUp();

  unsigned long long m_msStart;
};

GTestExplicitEndTest::GTestExplicitEndTest()
  : m_msStart(0) {}

void GTestExplicitEndTest::TearDown() {
  SimpleTimer::Instance()->RemoveCallback(this);
}

void GTestExplicitEndTest::OnTimeUp() {
  unsigned long long ms = CUTEST_NS::TickCount64() - m_msStart;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
  EndTest();
}

EXPLICIT_END_TEST_F(GTestExplicitEndTest, EndTestAfterOneSecond) {
  m_msStart = CUTEST_NS::TickCount64();
  SimpleTimer::Instance()->SetCallback(1000, this);
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(GTestExplicitEndTest, AutoEndTestAfterOneSecond, 1000) {}
