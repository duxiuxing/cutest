#pragma once

#include <cppunit/TestFixture.h>

#ifdef _MSC_VER

#include "cutest/MfcDialogTest.h"

CPPUNIT_NS_BEGIN

class MfcDialogTestFixture
  : public TestFixture
  , public CUTEST_NS::MfcDialogTest
{
public:
  MfcDialogTestFixture()
  {}

private:
  MfcDialogTestFixture( const MfcDialogTestFixture &other );
  MfcDialogTestFixture &operator=( const MfcDialogTestFixture &other );
};

CPPUNIT_NS_END

#endif // #ifdef _MSC_VER
