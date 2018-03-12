#include "../Logger.h"

#include "gtest/gtest-export.h"
#include "Runner.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include <stdarg.h>
#include <stdio.h>

CPPUNIT_NS_BEGIN

void
printString( const char *format, ... )
{
  char strBuffer[4096] = {0};
  va_list args;
  va_start( args, format );
  vsnprintf( strBuffer, sizeof( strBuffer ), format, args );
  va_end( args );

  printf( "%s\r\n", strBuffer );
}

TestProgressLogger::TestProgressLogger() : _passedTestCases( 0 )
{
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

    printString( "\n%2d FAILED %s", _failedTestCases.size(),
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
                 failure.sourceLine().fileName().c_str(),
                 failure.sourceLine().lineNumber(),
                 failure.thrownException()->what() );
  }
  else
  {
    printString( "%s(%u): failure : %s",
                 failure.sourceLine().fileName().c_str(),
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
