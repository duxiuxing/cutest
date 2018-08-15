#pragma once

#include <cppunit/cppunit.h>
#include "SimpleTimer.h"

class CppUnitExplicitEndTest
  : public CPPUNIT_NS::ExplicitEndTestFixture
  , public SimpleTimer::Callback
{
  CPPUNIT_TEST_SUITE( CppUnitExplicitEndTest );
  {
    CPPUNIT_EXPLICIT_END_TEST( explicit_end_test_after_1s );
    CPPUNIT_EXPLICIT_END_TEST_WITH_TIMEOUT( auto_end_test_after_1s, 1000 );
  }
  CPPUNIT_TEST_SUITE_END();

public:
  CppUnitExplicitEndTest();

  virtual void tearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  void explicit_end_test_after_1s();
  void auto_end_test_after_1s();

  unsigned long long tick_count_start;
};

CPPUNIT_TEST_SUITE_REGISTRATION( CppUnitExplicitEndTest );
