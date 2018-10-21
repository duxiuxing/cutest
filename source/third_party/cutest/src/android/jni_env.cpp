#include "cutest/jni_env.h"

CUTEST_NS_BEGIN

JavaVM* JniEnv::javaVM = NULL;

JniEnv::JniEnv()
    : jniEnv(NULL) {
    JniEnv::javaVM->GetEnv((void**)&this->jniEnv, JNI_VERSION_1_4);

    if (!this->jniEnv) {
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.name = NULL;
        args.group = NULL;
        JniEnv::javaVM->AttachCurrentThread(&this->jniEnv, &args);
    }
}

JniEnv::~JniEnv() {
    if (this->jniEnv) {
        JniEnv::javaVM->DetachCurrentThread();
    }
}

CUTEST_NS_END
