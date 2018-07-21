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
  void explicit_end_test_after_1s();

  // 实现Runnable::run()
  virtual void run();

  void auto_end_test_after_1s();
};

CPPUNIT_TEST_SUITE_REGISTRATION( CppUnitExplicitEndTest );
