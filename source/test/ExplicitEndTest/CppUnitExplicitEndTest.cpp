#include "CppUnitExplicitEndTest.h"

CppUnitExplicitEndTest::CppUnitExplicitEndTest()
  : tick_count_setup( 0 )
{}

void
CppUnitExplicitEndTest::setUp()
{
  this->tick_count_setup = CUTEST_NS::Runner::instance()->tickCount64();
}

void
CppUnitExplicitEndTest::tearDown()
{
  SimpleTimer::instance()->removeCallback( this );
}

void
CppUnitExplicitEndTest::explicit_end_test_after_1s()
{
  SimpleTimer::instance()->setCallback( 1000, this );
}

void
CppUnitExplicitEndTest::onTimeUp()
{
  unsigned long long ms = CUTEST_NS::Runner::instance()->tickCount64() - this->tick_count_setup;
  EXPECT_GT( ms, 950 );
  EXPECT_LT( ms, 1200 );
  endTest();
}

void
CppUnitExplicitEndTest::auto_end_test_after_1s()
{}
