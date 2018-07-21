#pragma once

#include <cppunit/cppunit.h>
#include "cutest/Runnable.h"

class CppUnitExplicitEndTest
  : public CPPUNIT_NS::ExplicitEndTestFixture
  , public CUTEST_NS::Runnable
{
  CPPUNIT_TEST_SUITE( CppUnitExplicitEndTest );
  {
	CPPUNIT_EXPLICIT_END_TEST( explicit_end_test_after_1s );
    CPPUNIT_EXPLICIT_END_TEST_WITH_TIMEOUT( auto_end_test_after_1s, 1000 );
  }
  CPPUNIT_TEST_SUITE_END();

public:
  CppUnitExplicitEndTest();

  virtual void setUp() override;
  virtual void tearDown() override;

  // 实现Runnable::run()
  virtual void run();

  void explicit_end_test_after_1s();
  void auto_end_test_after_1s();

  unsigned long long tick_count_setup;
};

CPPUNIT_TEST_SUITE_REGISTRATION( CppUnitExplicitEndTest );
