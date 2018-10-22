#include "stdafx.h"
#include "TestConfig.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include "cutest/runner.h"

int _tmain(int argc, _TCHAR* argv[]) {
    ::CoInitialize(NULL);

    TestConfig::GetInstance()->Load();
    CPPUNIT_NS::Test* allTests = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
    CUTEST_NS::Runner::instance()->start(allTests);
    CUTEST_NS::Runner::instance()->waitUntilAllTestEnd();
    delete allTests;

    return 0;
}
