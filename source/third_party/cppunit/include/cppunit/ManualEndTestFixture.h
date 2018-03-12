#pragma once

#include <cppunit/TestFixture.h>
#include "cutest/ManualEndTest.h"

CPPUNIT_NS_BEGIN

class ManualEndTestFixture
  : public TestFixture
  , public CUTEST_NS::ManualEndTest
{
public:
  ManualEndTestFixture()
  {}

private:
  ManualEndTestFixture( const ManualEndTestFixture &other );
  ManualEndTestFixture &operator=( const ManualEndTestFixture &other );
};

CPPUNIT_NS_END
