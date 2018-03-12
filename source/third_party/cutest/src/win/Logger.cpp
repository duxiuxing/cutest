#include "../Logger.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/Runner.h"
#include "gtest/gtest-export.h"

#include <stdarg.h>
#include <stdio.h>

#include <Windows.h>

CUTEST_NS_BEGIN

void
printString( const char *format, ... )
{
  char strBuffer[4096] = {0};
  va_list args;
  va_start( args, format );
  vsnprintf_s( strBuffer, sizeof( strBuffer ), _TRUNCATE, format, args );
  va_end( args );

  ::OutputDebugStringA( strBuffer );
  ::OutputDebugStringA( "\r\n" );
}

Logger::Logger() : m_passedTestCases( 0 )
{}

void
Logger::onRunnerStart( CPPUNIT_NS::Test *test )
{
  m_passedTestCases = 0;
  m_failedTestCases.clear();

  printString( "[==========] Running %s from %s.",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str() );
}

void
Logger::onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  printString( "[==========] %s from %s ran. (%u ms)",
               testing::FormatTestCount( test->countTestCases() ).c_str(),
               test->getName().c_str(),
               elapsed_ms );

  printString( "[  PASSED  ] %s.", testing::FormatTestCount( m_passedTestCases ).c_str() );

  if ( m_failedTestCases.size() )
  {
    printString( "[  FAILED  ] %s, listed below:", testing::FormatTestCount( ( int )m_failedTestCases.size() ).c_str() );

    std::list<std::string>::iterator it = m_failedTestCases.begin();
    while ( it != m_failedTestCases.end() )
    {
      printString( "[  FAILED  ] %s", it->c_str() );
      ++it;
    }

    printString( "\n%2d FAILED %s", m_failedTestCases.size(),
                 m_failedTestCases.size() == 1 ? "TEST" : "TESTS" );
  }
}

void
Logger::onSuiteStart( CPPUNIT_NS::Test *suite )
{
  printString( "[----------] %s from %s",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str() );
}

void
Logger::onSuiteEnd( CPPUNIT_NS::Test *suite, unsigned int elapsed_ms )
{
  printString( "[----------] %s from %s (%u ms)\n",
               testing::FormatTestCount( suite->countTestCases() ).c_str(),
               suite->getName().c_str(),
               elapsed_ms );
}

void
Logger::onTestStart( CPPUNIT_NS::Test *test )
{
  printString( "[ RUN      ] %s", test->getName().c_str() );
}

void
Logger::onFailureAdd( unsigned int index, const CPPUNIT_NS::TestFailure &failure )
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
Logger::onTestEnd(
  CPPUNIT_NS::Test *test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms )
{
  if ( 0 == error_count && 0 == failure_count )
  {
    printString( "[       OK ] %s (%u ms)",
                 test->getName().c_str(),
                 elapsed_ms );
    ++m_passedTestCases;
  }
  else
  {
    printString( "[  FAILED  ] %s (%u ms)",
                 test->getName().c_str(),
                 elapsed_ms );
    m_failedTestCases.push_back( test->getName() );
  }
}

CUTEST_NS_END
