#include "ProgressListenerJni.h"

#include <cppunit/SourceLine.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/Helper.h"

CUTEST_NS_BEGIN

ProgressListenerJni::ProgressListenerJni()
    : jni_env(NULL)
    , java_entity(NULL)
    , java_class(NULL) {
}

ProgressListenerJni::~ProgressListenerJni() {
    deleteJavaRef();
}

void
ProgressListenerJni::onRunnerStart(CPPUNIT_NS::Test* test) {
    jmethodID id = this->jni_env->GetMethodID(this->java_class, "onStartTestRun", "(I)V");
    this->jni_env->CallVoidMethod(this->java_entity, id, test->countTestCases());
}

void
ProgressListenerJni::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms) {
    jmethodID id = this->jni_env->GetMethodID(this->java_class, "onEndTestRun", "(I)V");
    this->jni_env->CallVoidMethod(this->java_entity, id, elapsed_ms);
    deleteJavaRef();
}

void
ProgressListenerJni::onTestStart(CPPUNIT_NS::Test* test) {
    jmethodID id = this->jni_env->GetMethodID(this->java_class, "onStartTest", "(Ljava/lang/String;)V");
    jstring name = this->jni_env->NewStringUTF(test->getName().c_str());
    this->jni_env->CallVoidMethod(this->java_entity, id, name);
    this->jni_env->DeleteLocalRef(name);
}

void
ProgressListenerJni::onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
    jmethodID id = this->jni_env->GetMethodID(this->java_class, "onAddFailure", "(IZLjava/lang/String;I)V");
    jstring name = this->jni_env->NewStringUTF(makeFilePathShorter(failure.sourceLine().fileName()).c_str());
    this->jni_env->CallVoidMethod(this->java_entity, id, index, failure.isError(), name, failure.sourceLine().lineNumber());
    this->jni_env->DeleteLocalRef(name);
}

void
ProgressListenerJni::onTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms) {
    jmethodID id = this->jni_env->GetMethodID(this->java_class, "onEndTest", "(Ljava/lang/String;III)V");
    jstring name = this->jni_env->NewStringUTF(test->getName().c_str());
    this->jni_env->CallVoidMethod(this->java_entity, id, name, error_count, failure_count, elapsed_ms);
    this->jni_env->DeleteLocalRef(name);
}

void
ProgressListenerJni::setJavaEntity(JNIEnv* env, jobject value) {
    deleteJavaRef();

    this->jni_env = env;
    this->java_entity = env->NewGlobalRef(value);
    this->java_class = (jclass)env->NewGlobalRef(env->GetObjectClass(this->java_entity));
}

void ProgressListenerJni::deleteJavaRef() {
    if (this->java_entity) {
        this->jni_env->DeleteGlobalRef(this->java_entity);
        this->java_entity = NULL;
    }

    if (this->java_class) {
        this->jni_env->DeleteGlobalRef(this->java_class);
        this->java_class = NULL;
    }
}

CUTEST_NS_END
