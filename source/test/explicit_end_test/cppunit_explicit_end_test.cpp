#include "cppunit_explicit_end_test.h"

CppUnitExplicitEndTest::CppUnitExplicitEndTest()
  : msStart( 0 )
{}

void
CppUnitExplicitEndTest::tearDown()
{
  SimpleTimer::instance()->removeCallback( this );
}

void
CppUnitExplicitEndTest::endTestAfterOneSecond()
{
  this->msStart = CUTEST_NS::tickCount64();
  SimpleTimer::instance()->setCallback( 1000, this );
}

void
CppUnitExplicitEndTest::onTimeUp()
{
  unsigned long long ms = CUTEST_NS::tickCount64() - this->msStart;
  EXPECT_GT( ms, 950 );
  EXPECT_LT( ms, 1200 );
  endTest();
}

void
CppUnitExplicitEndTest::autoEndTestAfterOneSecond()
{}
