#include "MainTestRunnerImpl.h"
#include "TestProgressListenerJni.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/JClassManager.h>

CPPUNIT_NS_BEGIN

unsigned long long 
Runner::tickCount()
{
  struct timeval current;
  gettimeofday( &current, NULL );
  unsigned long long sec = current.tv_sec;
  return ( sec * 1000 + current.tv_usec / 1000 );
}

thread_id 
Runner::currentThreadId()
{
  return ::gettid();
}

thread_id 
Runner::mainThreadId()
{
  return MainTestRunnerImpl::main_thread_id;
}

MainTestRunner *
Runner::instance()
{
  static MainTestRunnerImpl main_test_runner;
  return &main_test_runner;
}

thread_id MainTestRunnerImpl::main_thread_id = 0;

MainTestRunnerImpl::MainTestRunnerImpl()
{
  CJClassManager::instance()->registerGlobalClassName( MainTestRunnerImpl::jclassName() );
  CJClassManager::instance()->newAllGlobalClassRef();
  _listenerManager.add( &test_progress_logger );

  // 通过这个异步方法给MainTestRunnerImpl::main_thread_id赋值
  asyncRunOnMainThread( this, false );
}

void 
MainTestRunnerImpl::asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete )
{
  jclass cls = CJClassManager::instance()->findGlobalClass( MainTestRunnerImpl::jclassName() );

  CJniEnv env;
  jmethodID id = env->GetStaticMethodID( cls, "asyncRunOnMainThread", "(JZ)V" );
  env->CallStaticVoidMethod( cls, id, ( jlong )runnable, ( jboolean )is_auto_delete );
}

void 
MainTestRunnerImpl::delayRunOnMainThread(
  unsigned int delay_ms, Runnable *runnable, bool is_auto_delete )
{
  jclass cls = CJClassManager::instance()->findGlobalClass( MainTestRunnerImpl::jclassName() );

  CJniEnv env;
  jmethodID id = env->GetStaticMethodID( cls, "delayRunOnMainThread", "(JJZ)V" );
  env->CallStaticVoidMethod( cls, id, ( jlong )delay_ms, ( jlong )runnable, ( jboolean )is_auto_delete );
}

void 
MainTestRunnerImpl::run()
{
  MainTestRunnerImpl::main_thread_id = Runner::currentThreadId();
}

CPPUNIT_NS_END

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cppunit_MainTestRunner_version(
  JNIEnv *env, jobject thiz )
{
  return env->NewStringUTF( CUTEST_NS::Runner::version() );
}

CPPUNIT_NS::TestProgressListenerJni g_testProgressListener;

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_start(
  JNIEnv *env, jobject thiz, jobject listener )
{
  g_testProgressListener.setJavaEntity( env, listener );

  CUTEST_NS::Runner *runner = CUTEST_NS::Runner::instance();
  runner->addListener( &g_testProgressListener );
  runner->start( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_stop(
  JNIEnv *env, jobject thiz )
{
  CUTEST_NS::Runner::instance()->stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_internalRun(
  JNIEnv *env, jobject thiz,
  jlong native_runnable,
  jboolean is_auto_delete )
{
  CPPUNIT_NS::Runnable *runnable = ( CPPUNIT_NS::Runnable * )native_runnable;
  runnable->run();
  if ( is_auto_delete )
  {
    delete runnable;
  }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cppunit_MainTestRunner_failureDetails(
  JNIEnv *env, jobject thiz, jint index )
{
  const CPPUNIT_NS::TestFailure *failure =
    CUTEST_NS::Runner::instance()->failureAt( index );

  return env->NewStringUTF( failure->thrownException()->what() );
}
