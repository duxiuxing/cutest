#include "cppunit_explicit_end_test.h"

CppUnitExplicitEndTest::CppUnitExplicitEndTest()
  : m_msStart( 0 )
{}

void CppUnitExplicitEndTest::tearDown()
{
  SimpleTimer::Instance()->RemoveCallback( this );
}

void CppUnitExplicitEndTest::endTestAfterOneSecond()
{
  m_msStart = CUTEST_NS::TickCount64();
  SimpleTimer::Instance()->SetCallback( 1000, this );
}

void CppUnitExplicitEndTest::OnTimeUp()
{
  unsigned long long ms = CUTEST_NS::TickCount64() - m_msStart;
  EXPECT_GT( ms, 950 );
  EXPECT_LT( ms, 1200 );
  EndTest();
}

void CppUnitExplicitEndTest::autoEndTestAfterOneSecond()
{}
