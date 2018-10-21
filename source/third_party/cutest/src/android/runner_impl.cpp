#include "RunnerImpl.h"
#include "JniListener.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include "cutest/jclass_manager.h"
#include "cutest/jni_env.h"

CUTEST_NS_BEGIN

void
initGoogleMock() {
}

unsigned long long
tickCount64() {
    struct timeval current;
    gettimeofday(&current, NULL);
    unsigned long long sec = current.tv_sec;
    return (sec * 1000 + current.tv_usec / 1000);
}

thread_id
currentThreadId() {
    return ::gettid();
}

Runner*
Runner::instance() {
    static RunnerImpl runner_impl;
    return &runner_impl;
}

RunnerImpl::RunnerImpl() {
    JClassManager::instance()->registerGlobalClassName(RunnerImpl::jclassName());
    JClassManager::instance()->newAllGlobalClassRef();
    this->listener_manager.add(&test_progress_logger);

    // 通过这个异步方法给main_thread_id赋值
    asyncRunOnMainThread(this, false);
}

void
RunnerImpl::asyncRunOnMainThread(Runnable* runnable, bool is_auto_delete) {
    jclass cls = JClassManager::instance()->findGlobalClass(RunnerImpl::jclassName());

    JniEnv env;
    jmethodID id = env->GetStaticMethodID(cls, "asyncRunOnMainThread", "(JZ)V");
    env->CallStaticVoidMethod(cls, id, (jlong)runnable, (jboolean)is_auto_delete);
}

void
RunnerImpl::delayRunOnMainThread(
    unsigned int delay_ms, Runnable* runnable, bool is_auto_delete) {
    jclass cls = JClassManager::instance()->findGlobalClass(RunnerImpl::jclassName());

    JniEnv env;
    jmethodID id = env->GetStaticMethodID(cls, "delayRunOnMainThread", "(JJZ)V");
    env->CallStaticVoidMethod(cls, id, (jlong)delay_ms, (jlong)runnable, (jboolean)is_auto_delete);
}

void
RunnerImpl::waitUntilAllTestEnd() {
}

CUTEST_NS_END

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cutest_Runner_version(
    JNIEnv* env, jobject thiz) {
    return env->NewStringUTF(CUTEST_NS::version());
}

CUTEST_NS::JniListener jniListener;

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cutest_Runner_start(
    JNIEnv* env, jobject thiz, jobject listener) {
    jniListener.setJavaEntity(env, listener);

    CUTEST_NS::Runner* runner = CUTEST_NS::Runner::instance();
    runner->addListener(&jniListener);
    runner->start(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cppunit_Runner_stop(
    JNIEnv* env, jobject thiz) {
    CUTEST_NS::Runner::instance()->stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_cutest_Runner_internalRun(
    JNIEnv* env, jobject thiz,
    jlong native_runnable,
    jboolean is_auto_delete) {
    CUTEST_NS::Runnable* runnable = (CUTEST_NS::Runnable*)native_runnable;
    runnable->run();
    if (is_auto_delete) {
        delete runnable;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_tencent_cutest_Runner_failureDetails(
    JNIEnv* env, jobject thiz, jint index) {
    const CPPUNIT_NS::TestFailure* failure =
        CUTEST_NS::Runner::instance()->failureAt(index);

    return env->NewStringUTF(failure->thrownException()->what());
}
