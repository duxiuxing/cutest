#pragma once

#include "cutest/Define.h"
#include <jni.h>

CUTEST_NS_BEGIN

class JniEnv {
public:
    static JavaVM* javaVM;
    JniEnv();
    virtual ~JniEnv();

    JNIEnv* operator -> () {
        return this->jniEnv;
    }

private:
    JNIEnv* jniEnv;
};

CUTEST_NS_END
