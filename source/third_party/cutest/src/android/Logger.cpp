#include "../Logger.h"

#include <android/log.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/Helper.h"
#include "cutest/Runner.h"
#include "gtest/gtest-export.h"

CUTEST_NS_BEGIN

#define printString(...) __android_log_print(ANDROID_LOG_INFO, "cutest", __VA_ARGS__)

Logger::Logger()
    : passed_test_cases(0)
    , first_failure_of_a_test(true) {
#if defined(__arm__)
#if defined(__ARM_ARCH_7A__)
#if defined(__ARM_NEON__)
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a/NEON (hard-float)"
#else
#define ABI "armeabi-v7a/NEON"
#endif
#else
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a (hard-float)"
#else
#define ABI "armeabi-v7a"
#endif
#endif
#else
#define ABI "armeabi"
#endif
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI "mips64"
#elif defined(__mips__)
#define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif
    printString("libcutest.so is compiled at %s with API %d & ABI %s.", __TIME__, __ANDROID_API__, ABI);
}

void
Logger::onRunnerStart(CPPUNIT_NS::Test* test) {
    this->passed_test_cases = 0;
    this->failed_test_cases.clear();

    printString("[==========] Running %s from %s.",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str());
}

void
Logger::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms) {
    printString("[==========] %s from %s ran. (%u ms total)",
                testing::FormatTestCount(test->countTestCases()).c_str(),
                test->getName().c_str(),
                elapsed_ms);

    printString("[  PASSED  ] %s.", testing::FormatTestCount(this->passed_test_cases).c_str());

    if (this->failed_test_cases.size()) {
        printString("[  FAILED  ] %s, listed below:", testing::FormatTestCount((int)this->failed_test_cases.size()).c_str());

        std::list<std::string>::iterator it = this->failed_test_cases.begin();
        while (it != this->failed_test_cases.end()) {
            printString("[  FAILED  ] %s", it->c_str());
            ++it;
        }

        printString("\n%2d FAILED %s", (int)this->failed_test_cases.size(),
                    this->failed_test_cases.size() == 1 ? "TEST" : "TESTS");
    }
}

void
Logger::onSuiteStart(CPPUNIT_NS::Test* suite) {
    printString("[----------] %s from %s",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str());
}

void
Logger::onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int elapsed_ms) {
    printString("[----------] %s from %s (%u ms total)\n",
                testing::FormatTestCount(suite->countTestCases()).c_str(),
                suite->getName().c_str(),
                elapsed_ms);
}

void
Logger::onTestStart(CPPUNIT_NS::Test* test) {
    printString("[ RUN      ] %s", test->getName().c_str());
    this->first_failure_of_a_test = true;
}

void
Logger::onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
    if (this->first_failure_of_a_test) {
        printString("");
    }

    if (failure.isError()) {
        printString("%s(%u): error : %s",
                    makeFilePathShorter(failure.sourceLine().fileName()).c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    } else {
        printString("%s(%u): failure : %s",
                    makeFilePathShorter(failure.sourceLine().fileName()).c_str(),
                    failure.sourceLine().lineNumber(),
                    failure.thrownException()->what());
    }
    this->first_failure_of_a_test = false;
}

void
Logger::onTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms) {
    if (0 == error_count && 0 == failure_count) {
        printString("[       OK ] %s (%u ms)",
                    test->getName().c_str(),
                    elapsed_ms);
        ++this->passed_test_cases;
    } else {
        printString("[  FAILED  ] %s (%u ms)",
                    test->getName().c_str(),
                    elapsed_ms);
        this->failed_test_cases.push_back(test->getName());
    }
}

CUTEST_NS_END
