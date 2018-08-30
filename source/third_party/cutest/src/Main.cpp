#include "stdafx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include "cutest/Runner.h"

int _tmain(int argc, _TCHAR* argv[]) {
    ::CoInitialize(NULL);

    CPPUNIT_NS::Test* allTests = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
    CUTEST_NS::Runner::instance()->start(allTests);
    CUTEST_NS::Runner::instance()->waitUntilAllTestEnd();
    delete allTests;

    return 0;
}
