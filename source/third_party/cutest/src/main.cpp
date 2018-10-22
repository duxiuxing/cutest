#include "stdafx.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include "cutest/runner.h"

int _tmain(int argc, _TCHAR* argv[]) {
    ::CoInitialize(NULL);

    CPPUNIT_NS::Test* allTests = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
    CUTEST_NS::Runner::Instance()->Start(allTests);
    CUTEST_NS::Runner::Instance()->WaitUntilAllTestEnd();
    delete allTests;

    return 0;
}
