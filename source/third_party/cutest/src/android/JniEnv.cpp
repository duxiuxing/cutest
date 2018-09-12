#include "cutest/JniEnv.h"

CUTEST_NS_BEGIN

JavaVM* JniEnv::java_vm = NULL;

JniEnv::JniEnv()
    : call_detach(false)
    , jni_env(NULL) {
    JniEnv::java_vm->GetEnv((void**)&this->jni_env, JNI_VERSION_1_4);

    if (!this->jni_env) {
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.name = NULL;
        args.group = NULL;
        if (JNI_OK == JniEnv::java_vm->AttachCurrentThread(&this->jni_env, &args)) {
            this->call_detach = true;
        }
    }
}

JniEnv::~JniEnv() {
    if (this->call_detach) {
        JniEnv::java_vm->DetachCurrentThread();
    }
}

CUTEST_NS_END
