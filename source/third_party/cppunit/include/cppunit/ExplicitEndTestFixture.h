#pragma once

#include <cppunit/TestFixture.h>
#include "cutest/explicit_end_test.h"

CPPUNIT_NS_BEGIN // _CUTEST_IMPL

class ExplicitEndTestFixture
  : public TestFixture
  , public CUTEST_NS::ExplicitEndTest
{
public:
  ExplicitEndTestFixture()
  {}

private:
  ExplicitEndTestFixture( const ExplicitEndTestFixture &other );
  ExplicitEndTestFixture &operator=( const ExplicitEndTestFixture &other );
};

CPPUNIT_NS_END
