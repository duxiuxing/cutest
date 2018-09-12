#pragma once

#include "cutest/ProgressListener.h"
#include "cutest/Runnable.h"

#include <jni.h>

#include <stack>
#include <string>

CUTEST_NS_BEGIN

class JniProgressListener : public ProgressListener {
public:
    JniProgressListener();
    virtual ~JniProgressListener();

    // 重载ProgressListener的成员方法
    virtual void onRunnerStart(CPPUNIT_NS::Test* test);
    virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms);

    virtual void onTestStart(CPPUNIT_NS::Test* test);
    virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
    virtual void onTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int error_count,
        unsigned int failure_count,
        unsigned int elapsed_ms);

public:
    void setJavaEntity(JNIEnv* env, jobject value);

protected:
    void deleteJavaRef();
    JNIEnv* jni_env;
    jobject java_entity;
    jclass java_class;
};

CUTEST_NS_END
