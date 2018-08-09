#include "../Logger.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/Runner.h"
#include "gtest/gtest-export.h"

#include <stdarg.h>
#include <stdio.h>

#include <Windows.h>

using namespace testing::internal;

CUTEST_NS_BEGIN

void
printColorString( GTestColor color, const char *format, ... )
{
  char buffer[1024] = {0};
  va_list args;
  va_start( args, format );
  vsnprintf_s( buffer, sizeof( buffer ), _TRUNCATE, format, args );
  va_end( args );

  ::OutputDebugStringA( buffer );

  ColoredPrintf( color, buffer );
}

void
printString( const char *format, ... )
{
  char buffer[4096] = {0};
  va_list args;
  va_start( args, format );
  vsnprintf_s( buffer, sizeof( buffer ), _TRUNCATE, format, args );
  va_end( args );

  ::OutputDebugStringA( buffer );

  printf( buffer );
  fflush( stdout );
}

Logger::Logger() : passed_test_cases( 0 ), first_failure_of_a_test( true )
{}

void
Logger::onRunnerStart( CPPUNIT_NS::Test *test )
{
  this->passed_test_cases = 0;
  this->failed_test_cases.clear();

  printColorString( COLOR_GREEN,  "[==========] " );
  printString( "Running %s from %s.\n",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str() );
}

void
Logger::onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  printColorString( COLOR_GREEN,  "[==========] " );
  printString( "%s from %s ran. (%u ms total)\n",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str(),
               elapsed_ms );

  printColorString( COLOR_GREEN,  "[  PASSED  ] " );
  printString( "%s.\n", testing::FormatTestCount( this->passed_test_cases ).c_str() );

  if ( this->failed_test_cases.size() )
  {
    printColorString( COLOR_RED,  "[  FAILED  ] " );
    printString( "%s, listed below:\n", testing::FormatTestCount( ( int )this->failed_test_cases.size() ).c_str() );

    std::list<std::string>::iterator it = this->failed_test_cases.begin();
    while ( it != this->failed_test_cases.end() )
    {
      printColorString( COLOR_RED,  "[  FAILED  ] " );
      printString( "%s\n", it->c_str() );
      ++it;
    }

    printString( "\n%2d FAILED %s\n", this->failed_test_cases.size(),
                 this->failed_test_cases.size() == 1 ? "TEST" : "TESTS" );
  }
}

void
Logger::onSuiteStart( CPPUNIT_NS::Test *suite )
{
  printColorString( COLOR_GREEN, "[----------] " );
  printString( "%s from %s\n",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str() );
}

void
Logger::onSuiteEnd( CPPUNIT_NS::Test *suite, unsigned int elapsed_ms )
{
  printColorString( COLOR_GREEN, "[----------] " );
  printString( "%s from %s (%u ms total)\n\n",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str(),
               elapsed_ms );
}

void
Logger::onTestStart( CPPUNIT_NS::Test *test )
{
  printColorString( COLOR_GREEN,  "[ RUN      ] " );
  printString( "%s\n", test->getName().c_str() );
  this->first_failure_of_a_test = true;
}

void
Logger::onFailureAdd( unsigned int index, const CPPUNIT_NS::TestFailure &failure )
{
  if ( this->first_failure_of_a_test )
  {
    printString( "\n" );
  }

  if ( failure.isError() )
  {
    printString( "%s(%u): error : %s\n",
                 failure.sourceLine().fileName().c_str(),
                 failure.sourceLine().lineNumber(),
                 failure.thrownException()->what() );
  }
  else
  {
    printString( "%s(%u): failure : %s\n",
                 failure.sourceLine().fileName().c_str(),
                 failure.sourceLine().lineNumber(),
                 failure.thrownException()->what() );
  }
  this->first_failure_of_a_test = false;
}

void
Logger::onTestEnd(
  CPPUNIT_NS::Test *test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms )
{
  if ( 0 == error_count && 0 == failure_count )
  {
    printColorString( COLOR_GREEN,  "[       OK ] " );
    printString( "%s (%u ms)\n",
                 test->getName().c_str(),
                 elapsed_ms );
    ++this->passed_test_cases;
  }
  else
  {
    printColorString( COLOR_RED,  "[  FAILED  ] " );
    printString( "%s (%u ms)\n",
                 test->getName().c_str(),
                 elapsed_ms );
    this->failed_test_cases.push_back( test->getName() );
  }
}

CUTEST_NS_END
