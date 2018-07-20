#pragma once

#include "CUTestDefine.h"

// cppunit
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

// std
#include <list>
#include <stack>
#include <string>

CUTEST_NS_BEGIN

class ProgressListener
{
public:
  virtual ~ProgressListener() {}

  virtual void onRunnerStart( CPPUNIT_NS::Test *test ) {}
  virtual void onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms ) {}

  virtual void onSuiteStart( CPPUNIT_NS::Test *suite ) {}
  virtual void onSuiteEnd( CPPUNIT_NS::Test *suite, unsigned int elapsed_ms ) {}

  virtual void onTestStart( CPPUNIT_NS::Test *test ) {}
  virtual void onFailureAdd( unsigned int index, const CPPUNIT_NS::TestFailure &failure ) {}
  virtual void onTestEnd(
    CPPUNIT_NS::Test *test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms ) {}
};

CUTEST_NS_END
