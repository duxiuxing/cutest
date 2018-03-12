#pragma once

#include <cppunit/TestListener.h>
#include "cutest/ProgressListener.h"
#include "cutest/Runnable.h"

#include <jni.h>

#include <stack>
#include <string>

CPPUNIT_NS_BEGIN

class TestProgressListenerJni : public ProgressListener
{
public:
  TestProgressListenerJni();
  virtual ~TestProgressListenerJni();

  // 重载TestProgressListener的成员方法
  virtual void onRunnerStart( Test *test );
  virtual void onRunnerEnd( Test *test, unsigned int elapsed_ms );

  virtual void onTestStart( Test *test );
  virtual void onFailureAdd( unsigned int index, const TestFailure &failure );
  virtual void onTestEnd(
    Test *test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms );

public:
  void setJavaEntity( JNIEnv *env, jobject value );
protected:
  void deleteJavaRef();
  JNIEnv *_jniEnv;
  jobject _javaEntity;
  jclass _javaClass;
};

CPPUNIT_NS_END
