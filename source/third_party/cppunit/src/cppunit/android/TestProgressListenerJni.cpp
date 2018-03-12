#include "TestProgressListenerJni.h"

#include "MainTestRunner.h"

#include <cppunit/JniHelp.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

CPPUNIT_NS_BEGIN

TestProgressListenerJni::TestProgressListenerJni()
  : _jniEnv( NULL )
  , _javaEntity( NULL )
  , _javaClass( NULL )
{}

TestProgressListenerJni::~TestProgressListenerJni()
{
  deleteJavaRef();
}

void
TestProgressListenerJni::onRunnerStart( Test *test )
{
  jmethodID id = _jniEnv->GetMethodID( _javaClass, "onStartTestRun", "(I)V" );
  _jniEnv->CallVoidMethod( _javaEntity, id, test->countTestCases() );
}

void
TestProgressListenerJni::onRunnerEnd( Test *test, unsigned int elapsed_ms )
{
  jmethodID id = _jniEnv->GetMethodID( _javaClass, "onEndTestRun", "(I)V" );
  _jniEnv->CallVoidMethod( _javaEntity, id, elapsed_ms );
  deleteJavaRef();
}

void
TestProgressListenerJni::onTestStart( Test *test )
{
  jmethodID id = _jniEnv->GetMethodID( _javaClass, "onStartTest", "(Ljava/lang/String;)V" );
  jstring name = _jniEnv->NewStringUTF( test->getName().c_str() );
  _jniEnv->CallVoidMethod( _javaEntity, id, name );
  _jniEnv->DeleteLocalRef( name );
}

void
TestProgressListenerJni::onFailureAdd( unsigned int index, const TestFailure &failure )
{
  jmethodID id = _jniEnv->GetMethodID( _javaClass, "onAddFailure", "(IZLjava/lang/String;I)V" );
  jstring name = _jniEnv->NewStringUTF( JniHelp::makeFilePathShorter( failure.sourceLine().fileName() ).c_str() );
  _jniEnv->CallVoidMethod( _javaEntity, id, index, failure.isError(), name, failure.sourceLine().lineNumber() );
  _jniEnv->DeleteLocalRef( name );
}

void
TestProgressListenerJni::onTestEnd(
  Test *test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms )
{
  jmethodID id = _jniEnv->GetMethodID( _javaClass, "onEndTest", "(Ljava/lang/String;III)V" );
  jstring name = _jniEnv->NewStringUTF( test->getName().c_str() );
  _jniEnv->CallVoidMethod( _javaEntity, id, name, error_count, failure_count, elapsed_ms );
  _jniEnv->DeleteLocalRef( name );
}

void
TestProgressListenerJni::setJavaEntity( JNIEnv *env, jobject value )
{
  deleteJavaRef();

  _jniEnv = env;
  _javaEntity = env->NewGlobalRef( value );
  _javaClass = ( jclass )env->NewGlobalRef( env->GetObjectClass( _javaEntity ) );
}

void
TestProgressListenerJni::deleteJavaRef()
{
  if ( _javaEntity )
  {
    _jniEnv->DeleteGlobalRef( _javaEntity );
    _javaEntity = NULL;
  }

  if ( _javaClass )
  {
    _jniEnv->DeleteGlobalRef( _javaClass );
    _javaClass = NULL;
  }
}

CPPUNIT_NS_END
