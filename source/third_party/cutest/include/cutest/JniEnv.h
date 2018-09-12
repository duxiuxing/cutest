#pragma once

#include "cutest/Define.h"
#include <jni.h>

CUTEST_NS_BEGIN

class JniEnv {
public:
    static JavaVM* java_vm;
    JniEnv();
    virtual ~JniEnv();

    JNIEnv* operator -> () {
        return this->jni_env;
    }

private:
    bool call_detach;;
    JNIEnv* jni_env;
};

CUTEST_NS_END
