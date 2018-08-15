#include "CppUnitExplicitEndTest.h"

CppUnitExplicitEndTest::CppUnitExplicitEndTest()
  : tick_count_start( 0 )
{}

void
CppUnitExplicitEndTest::tearDown()
{
  SimpleTimer::instance()->removeCallback( this );
}

void
CppUnitExplicitEndTest::explicit_end_test_after_1s()
{
  this->tick_count_start = CUTEST_NS::Runner::instance()->tickCount64();
  SimpleTimer::instance()->setCallback( 1000, this );
}

void
CppUnitExplicitEndTest::onTimeUp()
{
  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount64() - this->tick_count_start;
  EXPECT_GT( ms, 950 );
  EXPECT_LT( ms, 1200 );
  endTest();
}

void
CppUnitExplicitEndTest::auto_end_test_after_1s()
{}
