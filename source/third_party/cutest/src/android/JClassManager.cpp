#include "cutest/JClassManager.h"
#include "cutest/JniEnv.h"

#include <map>

CUTEST_NS_BEGIN

JClassManager* JClassManager::instance() {
    static JClassManager jclass_manager;
    return &jclass_manager;
}

JClassManager::JClassManager() {
}

void
JClassManager::registerGlobalClassName(const char* class_name) {
    this->jclass_map[class_name] = NULL;
}

void
JClassManager::newAllGlobalClassRef() {
    JniEnv env;
    JClassMap::iterator it = this->jclass_map.begin();
    while (it != this->jclass_map.end()) {
        it->second = (jclass)env->NewGlobalRef(env->FindClass(it->first.c_str()));
        ++it;
    }
}

void
JClassManager::deleteAllGlobalClassRef() {
    JniEnv env;
    JClassMap::iterator it = this->jclass_map.begin();
    while (it != this->jclass_map.end()) {
        env->DeleteGlobalRef(it->second);
        ++it;
    }
}

jclass JClassManager::findGlobalClass(const char* class_name) {
    JClassMap::iterator it = this->jclass_map.find(class_name);
    if (it == this->jclass_map.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

CUTEST_NS_END

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    CUTEST_NS::JniEnv::java_vm = vm;
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM* vm, void* reserved) {
    CUTEST_NS::JClassManager::instance()->deleteAllGlobalClassRef();
}
