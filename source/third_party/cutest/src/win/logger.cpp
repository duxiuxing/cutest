#include "../Logger.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#include "cutest/runner.h"
#include "gtest/gtest-export.h"

using namespace testing::internal;

CUTEST_NS_BEGIN

void
printColorString(GTestColor color, const char* format, ...) {
    char buffer[1024] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    ::OutputDebugStringA(buffer);

    ColoredPrintf(color, buffer);
}

void
printString(const char* format, ...) {
    char buffer[4096] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    ::OutputDebugStringA(buffer);

    printf(buffer);
    fflush(stdout);
}

Logger::Logger()
    : passedTestCaseCount(0)
    , firstFailureOfCurrentTest(true) {}

void
Logger::onRunnerStart(CPPUNIT_NS::Test* test) {
    this->passedTestCaseCount = 0;
    this->failedTestCaseNames.clear();

    printColorString(COLOR_GREEN,  "[==========] ");
    printString("Running %s from %s.\n",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str());
}

void
Logger::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    printColorString(COLOR_GREEN,  "[==========] ");
    printString("%s from %s ran. (%u ms total)\n",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str(),
                msElapsed);

    printColorString(COLOR_GREEN,  "[  PASSED  ] ");
    printString("%s.\n", testing::FormatTestCount(this->passedTestCaseCount).c_str());

    if (this->failedTestCaseNames.size()) {
        printColorString(COLOR_RED,  "[  FAILED  ] ");
        printString("%s, listed below:\n", testing::FormatTestCount((int)this->failedTestCaseNames.size()).c_str());

        std::list<std::string>::iterator it = this->failedTestCaseNames.begin();
        while (it != this->failedTestCaseNames.end()) {
            printColorString(COLOR_RED,  "[  FAILED  ] ");
            printString("%s\n", it->c_str());
            ++it;
        }

        printString("\n%2d FAILED %s\n", this->failedTestCaseNames.size(),
                    this->failedTestCaseNames.size() == 1 ? "TEST" : "TESTS");
    }
}

void
Logger::onSuiteStart(CPPUNIT_NS::Test* suite) {
    printColorString(COLOR_GREEN, "[----------] ");
    printString("%s from %s\n",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str());
}

void
Logger::onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) {
    printColorString(COLOR_GREEN, "[----------] ");
    printString("%s from %s (%u ms total)\n\n",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str(),
                msElapsed);
}

void
Logger::onTestStart(CPPUNIT_NS::Test* test) {
    printColorString(COLOR_GREEN,  "[ RUN      ] ");
    printString("%s\n", test->getName().c_str());
    this->firstFailureOfCurrentTest = true;
}

void
Logger::onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
    if (this->firstFailureOfCurrentTest) {
        printString("\n");
    }

    if (failure.isError()) {
        printString("%s(%u): error : %s\n",
                    failure.sourceLine().fileName().c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    } else {
        printString("%s(%u): failure : %s\n",
                    failure.sourceLine().fileName().c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    }
    this->firstFailureOfCurrentTest = false;
}

void
Logger::onTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int errorCount,
    unsigned int failureCount,
    unsigned int msElapsed) {
    if (0 == errorCount && 0 == failureCount) {
        printColorString(COLOR_GREEN,  "[       OK ] ");
        printString("%s (%u ms)\n",
                    test->getName().c_str(),
                    msElapsed);
        ++this->passedTestCaseCount;
    } else {
        printColorString(COLOR_RED,  "[  FAILED  ] ");
        printString("%s (%u ms)\n",
                    test->getName().c_str(),
                    msElapsed);
        this->failedTestCaseNames.push_back(test->getName());
    }
}

CUTEST_NS_END
