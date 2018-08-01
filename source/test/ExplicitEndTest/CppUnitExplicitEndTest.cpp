#include "CppUnitExplicitEndTest.h"

CppUnitExplicitEndTest::CppUnitExplicitEndTest()
  : tick_count_setup(0)
{}

void
CppUnitExplicitEndTest::setUp()
{
  this->tick_count_setup = CUTEST_NS::Runner::instance()->tickCount();
}

void
CppUnitExplicitEndTest::tearDown()
{
  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount() - this->tick_count_setup;
  EXPECT_GT(ms, 950);
  EXPECT_LT(ms, 1200);
}

void
CppUnitExplicitEndTest::explicit_end_test_after_1s()
{
  // 框架会在主线程调用此方法
  // 1s之后再调用EndTest()来结束用例
  CUTEST_NS::Runner::instance()->delayRunOnMainThread( 1000, this, false );
}

void
CppUnitExplicitEndTest::run()
{
  // 这里才结束测试用例
  endTest();
}

void
CppUnitExplicitEndTest::auto_end_test_after_1s()
{}

