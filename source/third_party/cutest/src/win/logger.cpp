#include "../logger.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#include "cutest/runner.h"
#include "gtest/gtest-export.h"

using namespace testing::internal;

CUTEST_NS_BEGIN

void PrintColorString(GTestColor color, const char* format, ...) {
    char buffer[1024] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    ::OutputDebugStringA(buffer);

    ColoredPrintf(color, buffer);
}

void PrintString(const char* format, ...) {
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
    : m_passedTestCaseCount(0)
    , m_isFirstFailureOfCurrentTest(true) {}

void Logger::OnRunnerStart(CPPUNIT_NS::Test* test) {
    m_passedTestCaseCount = 0;
    m_failedTestCaseNames.clear();

    PrintColorString(COLOR_GREEN,  "[==========] ");
    PrintString("Running %s from %s.\n",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str());
}

void Logger::OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    PrintColorString(COLOR_GREEN,  "[==========] ");
    PrintString("%s from %s ran. (%u ms total)\n",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str(),
                msElapsed);

    PrintColorString(COLOR_GREEN,  "[  PASSED  ] ");
    PrintString("%s.\n", testing::FormatTestCount(m_passedTestCaseCount).c_str());

    if (m_failedTestCaseNames.size()) {
        PrintColorString(COLOR_RED,  "[  FAILED  ] ");
        PrintString("%s, listed below:\n", testing::FormatTestCount((int)m_failedTestCaseNames.size()).c_str());

        std::list<std::string>::iterator it = m_failedTestCaseNames.begin();
        while (it != m_failedTestCaseNames.end()) {
            PrintColorString(COLOR_RED,  "[  FAILED  ] ");
            PrintString("%s\n", it->c_str());
            ++it;
        }

        PrintString("\n%2d FAILED %s\n", m_failedTestCaseNames.size(),
                    m_failedTestCaseNames.size() == 1 ? "TEST" : "TESTS");
    }
}

void Logger::OnSuiteStart(CPPUNIT_NS::Test* suite) {
    PrintColorString(COLOR_GREEN, "[----------] ");
    PrintString("%s from %s\n",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str());
}

void
Logger::OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) {
    PrintColorString(COLOR_GREEN, "[----------] ");
    PrintString("%s from %s (%u ms total)\n\n",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str(),
                msElapsed);
}

void Logger::OnTestStart(CPPUNIT_NS::Test* test) {
    PrintColorString(COLOR_GREEN,  "[ RUN      ] ");
    PrintString("%s\n", test->getName().c_str());
    m_isFirstFailureOfCurrentTest = true;
}

void Logger::OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
    if (m_isFirstFailureOfCurrentTest) {
        PrintString("\n");
    }

    if (failure.isError()) {
        PrintString("%s(%u): error : %s\n",
                    failure.sourceLine().fileName().c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    } else {
        PrintString("%s(%u): failure : %s\n",
                    failure.sourceLine().fileName().c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    }
    m_isFirstFailureOfCurrentTest = false;
}

void Logger::OnTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int errorCount,
    unsigned int failureCount,
    unsigned int msElapsed) {
    if (0 == errorCount && 0 == failureCount) {
        PrintColorString(COLOR_GREEN,  "[       OK ] ");
        PrintString("%s (%u ms)\n",
                    test->getName().c_str(),
                    msElapsed);
        ++m_passedTestCaseCount;
    } else {
        PrintColorString(COLOR_RED,  "[  FAILED  ] ");
        PrintString("%s (%u ms)\n",
                    test->getName().c_str(),
                    msElapsed);
        m_failedTestCaseNames.push_back(test->getName());
    }
}

CUTEST_NS_END
