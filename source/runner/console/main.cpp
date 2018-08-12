#include "stdafx.h"
#include "TestConfig.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cutest/Runner.h>

int _tmain( int argc, _TCHAR *argv[] )
{
  ::CoInitialize( NULL );

  TestConfig::GetInstance()->Load();
  CUTEST_NS::Runner::instance()->start(
    CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest()
  );
  CUTEST_NS::Runner::instance()->waitUntilAllTestEnd();

  return 0;
}
