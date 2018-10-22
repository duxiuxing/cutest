#pragma once

#include <cppunit/cppunit.h>
#include "simple_timer.h"

class CppUnitExplicitEndTest
  : public CPPUNIT_NS::ExplicitEndTestFixture
  , public SimpleTimer::Callback
{
  CPPUNIT_TEST_SUITE( CppUnitExplicitEndTest );
  {
    CPPUNIT_EXPLICIT_END_TEST( endTestAfterOneSecond );
    CPPUNIT_EXPLICIT_END_TEST_WITH_TIMEOUT( autoEndTestAfterOneSecond, 1000 );
  }
  CPPUNIT_TEST_SUITE_END();

public:
  CppUnitExplicitEndTest();

  virtual void tearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  void endTestAfterOneSecond();
  void autoEndTestAfterOneSecond();

  unsigned long long msStart;
};

CPPUNIT_TEST_SUITE_REGISTRATION( CppUnitExplicitEndTest );
