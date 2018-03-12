#include "../Logger.h"

#include "gtest/gtest-export.h"
#include "MainTestRunner.h"

#include <android/log.h>

#include <cppunit/JniHelp.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

CPPUNIT_NS_BEGIN

#define printString(...) __android_log_print(ANDROID_LOG_INFO, "cppunit", __VA_ARGS__)

TestProgressLogger::TestProgressLogger() : _passedTestCases( 0 )
{
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
  printString( "libcppunit.so is compiled at %s with API %d & ABI %s.", __TIME__, __ANDROID_API__, ABI );
}

void
TestProgressLogger::onRunnerStart( Test *test )
{
  _passedTestCases = 0;
  _failedTestCases.clear();

  printString( "[==========] Running %s from %s.",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str() );
}

void
TestProgressLogger::onRunnerEnd( Test *test, unsigned int elapsed_ms )
{
  printString( "[==========] %s from %s ran. (%u ms)",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str(),
               elapsed_ms );

  printString( "[  PASSED  ] %s.", testing::FormatTestCount( _passedTestCases ).c_str() );

  if ( _failedTestCases.size() )
  {
    printString( "[  FAILED  ] %s, listed below:", testing::FormatTestCount( ( int )_failedTestCases.size() ).c_str() );

    std::list<std::string>::iterator it = _failedTestCases.begin();
    while ( it != _failedTestCases.end() )
    {
      printString( "[  FAILED  ] %s", it->c_str() );
      ++it;
    }

    printString( "\n%2d FAILED %s", ( int )_failedTestCases.size(),
                 _failedTestCases.size() == 1 ? "TEST" : "TESTS" );
  }
}

void
TestProgressLogger::onSuiteStart( Test *suite )
{
  printString( "[----------] %s from %s",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str() );
}

void
TestProgressLogger::onSuiteEnd( Test *suite, unsigned int elapsed_ms )
{
  printString( "[----------] %s from %s (%u ms)\n",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str(),
               elapsed_ms );
}

void
TestProgressLogger::onTestStart( Test *test )
{
  printString( "[ RUN      ] %s", test->getName().c_str() );
}

void
TestProgressLogger::onFailureAdd( unsigned int index, const TestFailure &failure )
{
  if ( failure.isError() )
  {
    printString( "%s(%u): error : %s",
                 JniHelp::makeFilePathShorter( failure.sourceLine().fileName() ).c_str(),
                 failure.sourceLine().lineNumber(),
                 failure.thrownException()->what() );
  }
  else
  {
    printString( "%s(%u): failure : %s",
                 JniHelp::makeFilePathShorter( failure.sourceLine().fileName() ).c_str(),
                 failure.sourceLine().lineNumber(),
                 failure.thrownException()->what() );
  }
}

void
TestProgressLogger::onTestEnd(
  Test *test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms )
{
  if ( 0 == error_count && 0 == failure_count )
  {
    printString( "[       OK ] %s (%u ms)",
                 test->getName().c_str(),
                 elapsed_ms );
    ++_passedTestCases;
  }
  else
  {
    printString( "[  FAILED  ] %s (%u ms)",
                 test->getName().c_str(),
                 elapsed_ms );
    _failedTestCases.push_back( test->getName() );
  }
}

CPPUNIT_NS_END
