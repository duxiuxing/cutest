#include "stdafx.h"
#include "TestConfig.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cutest/Runner.h>

CPPUNIT_API int _tmain( int argc, _TCHAR *argv[] )
{
  ::CoInitialize( NULL );

  TestConfig::GetInstance()->Load();
  CUTEST_NS::Runner::instance()->runUntilAllTestEnd(
    CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest()
  );

  return 0;
}
