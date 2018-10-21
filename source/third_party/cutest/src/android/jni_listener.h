#pragma once

#include "cutest/listener.h"
#include "cutest/runnable.h"

#include <jni.h>

#include <stack>
#include <string>

CUTEST_NS_BEGIN

class JniListener : public Listener {
public:
    JniListener();
    virtual ~JniListener();

    // 重载Listener的成员方法
    virtual void onRunnerStart(CPPUNIT_NS::Test* test);
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed);

    virtual void onTestStart(CPPUNIT_NS::Test* test);
    virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
    virtual void onTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed);

public:
    void setJavaEntity(JNIEnv* env, jobject value);

protected:
    void deleteJavaRef();
    JNIEnv* jniEnv;
    jobject javaEntity;
    jclass javaClass;
};

CUTEST_NS_END
