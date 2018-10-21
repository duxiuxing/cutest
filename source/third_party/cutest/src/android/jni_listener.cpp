#include "JniListener.h"

#include <cppunit/SourceLine.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include "cutest/helper.h"

CUTEST_NS_BEGIN

JniListener::JniListener()
    : jniEnv(NULL)
    , javaEntity(NULL)
    , javaClass(NULL) {
}

JniListener::~JniListener() {
    deleteJavaRef();
}

void JniListener::onRunnerStart(CPPUNIT_NS::Test* test) {
    jmethodID id = this->jniEnv->GetMethodID(this->javaClass, "onRunnerStart", "(I)V");
    this->jniEnv->CallVoidMethod(this->javaEntity, id, test->countTestCases());
}

void JniListener::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
    jmethodID id = this->jniEnv->GetMethodID(this->javaClass, "onRunnerEnd", "(I)V");
    this->jniEnv->CallVoidMethod(this->javaEntity, id, msElapsed);
    deleteJavaRef();
}

void JniListener::onTestStart(CPPUNIT_NS::Test* test) {
    jmethodID id = this->jniEnv->GetMethodID(this->javaClass, "onTestStart", "(Ljava/lang/String;)V");
    jstring name = this->jniEnv->NewStringUTF(test->getName().c_str());
    this->jniEnv->CallVoidMethod(this->javaEntity, id, name);
    this->jniEnv->DeleteLocalRef(name);
}

void JniListener::onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
    jmethodID id = this->jniEnv->GetMethodID(this->javaClass, "onFailureAdd", "(IZLjava/lang/String;I)V");
    jstring name = this->jniEnv->NewStringUTF(makeFilePathShorter(failure.sourceLine().fileName()).c_str());
    this->jniEnv->CallVoidMethod(this->javaEntity, id, index, failure.isError(), name, failure.sourceLine().lineNumber());
    this->jniEnv->DeleteLocalRef(name);
}

void JniListener::onTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int errorCount,
    unsigned int failureCount,
    unsigned int msElapsed) {
    jmethodID id = this->jniEnv->GetMethodID(this->javaClass, "onTestEnd", "(Ljava/lang/String;III)V");
    jstring name = this->jniEnv->NewStringUTF(test->getName().c_str());
    this->jniEnv->CallVoidMethod(this->javaEntity, id, name, errorCount, failureCount, msElapsed);
    this->jniEnv->DeleteLocalRef(name);
}

void JniListener::setJavaEntity(JNIEnv* env, jobject value) {
    deleteJavaRef();

    this->jniEnv = env;
    this->javaEntity = env->NewGlobalRef(value);
    this->javaClass = (jclass)env->NewGlobalRef(env->GetObjectClass(this->javaEntity));
}

void JniListener::deleteJavaRef() {
    if (this->javaEntity) {
        this->jniEnv->DeleteGlobalRef(this->javaEntity);
        this->javaEntity = NULL;
    }

    if (this->javaClass) {
        this->jniEnv->DeleteGlobalRef(this->javaClass);
        this->javaClass = NULL;
    }
}

CUTEST_NS_END
