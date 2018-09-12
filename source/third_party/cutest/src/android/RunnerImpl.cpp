#include "MainTestRunnerImpl.h"
#include "TestProgressListenerJni.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/JClassManager.h>

CPPUNIT_NS_BEGIN

unsigned long long MainTestRunner::tickCount() {
    struct timeval current;
    gettimeofday(&current, NULL);
    unsigned long long sec = current.tv_sec;
    return (sec * 1000 + current.tv_usec / 1000);
}

thread_id MainTestRunner::currentThreadId() {
    return ::gettid();
}

thread_id MainTestRunner::mainThreadId() {
    return MainTestRunnerImpl::s_mainThreadId;
}

MainTestRunner* MainTestRunner::instance() {
    static MainTestRunnerImpl s_mainTestRunner;
    return &s_mainTestRunner;
}

thread_id MainTestRunnerImpl::s_mainThreadId = 0;

MainTestRunnerImpl::MainTestRunnerImpl() {
    CJClassManager::instance()->registerGlobalClassName(MainTestRunnerImpl::jclassName());
    CJClassManager::instance()->newAllGlobalClassRef();
    _listenerManager.add(&_testProgressLogger);

    // 通过这个异步方法给s_mainThreadId赋值
    asyncRunOnMainThread(this, false);
}

void MainTestRunnerImpl::asyncRunOnMainThread(Runnable* runnable, bool is_auto_delete) {
    jclass cls = CJClassManager::instance()->findGlobalClass(MainTestRunnerImpl::jclassName());

    CJniEnv env;
    jmethodID id = env->GetStaticMethodID(cls, "asyncRunOnMainThread", "(JZ)V");
    env->CallStaticVoidMethod(cls, id, (jlong)runnable, (jboolean)is_auto_delete);
}

void MainTestRunnerImpl::delayRunOnMainThread(
    unsigned int delay_ms, Runnable* runnable, bool is_auto_delete) {
    jclass cls = CJClassManager::instance()->findGlobalClass(MainTestRunnerImpl::jclassName());

    CJniEnv env;
    jmethodID id = env->GetStaticMethodID(cls, "delayRunOnMainThread", "(JJZ)V");
    env->CallStaticVoidMethod(cls, id, (jlong)delay_ms, (jlong)runnable, (jboolean)is_auto_delete);
}

void MainTestRunnerImpl::run() {
    MainTestRunnerImpl::s_mainThreadId = MainTestRunner::currentThreadId();
}

CPPUNIT_NS_END

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cppunit_MainTestRunner_version(
    JNIEnv* env, jobject thiz) {
    return env->NewStringUTF(CPPUNIT_NS::MainTestRunner::version());
}

CPPUNIT_NS::TestProgressListenerJni g_testProgressListener;

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_start(
    JNIEnv* env, jobject thiz, jobject listener) {
    g_testProgressListener.setJavaEntity(env, listener);

    CPPUNIT_NS::MainTestRunner* runner = CPPUNIT_NS::MainTestRunner::instance();
    runner->addListener(&g_testProgressListener);
    runner->start(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_stop(
        JNIEnv* env, jobject thiz) {
    CPPUNIT_NS::MainTestRunner::instance()->stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_MainTestRunner_internalRun(
    JNIEnv* env, jobject thiz,
    jlong native_runnable,
    jboolean is_auto_delete) {
    CPPUNIT_NS::Runnable* runnable = (CPPUNIT_NS::Runnable*)native_runnable;
    runnable->run();
    if (is_auto_delete)
        delete runnable;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cppunit_MainTestRunner_failureDetails(
    JNIEnv* env, jobject thiz, jint index) {
    const CPPUNIT_NS::TestFailure* failure =
        CPPUNIT_NS::MainTestRunner::instance()->failureAt(index);

    return env->NewStringUTF(failure->thrownException()->what());
}