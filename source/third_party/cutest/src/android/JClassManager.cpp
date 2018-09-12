#include "cppunit/JClassManager.h"
#include "cppunit/JniEnv.h"

#include <map>

CPPUNIT_NS_BEGIN

CJClassManager* CJClassManager::instance() {
    static CJClassManager s_jClassManager;
    return &s_jClassManager;
}

CJClassManager::CJClassManager() {
}

void CJClassManager::registerGlobalClassName(const char* class_name) {
    _jClassMap[class_name] = NULL;
}

void CJClassManager::newAllGlobalClassRef() {
    CJniEnv env;
    CJClassMap::iterator it = _jClassMap.begin();
    while (it != _jClassMap.end()) {
        it->second = (jclass)env->NewGlobalRef(env->FindClass(it->first.c_str()));
        ++it;
    }
}

void CJClassManager::deleteAllGlobalClassRef() {
    CJniEnv env;
    CJClassMap::iterator it = _jClassMap.begin();
    while (it != _jClassMap.end()) {
        env->DeleteGlobalRef(it->second);
        ++it;
    }
}

jclass CJClassManager::findGlobalClass(const char* class_name) {
    CJClassMap::iterator it = _jClassMap.find(class_name);
    if (it == _jClassMap.end())
        return NULL;
    else
        return it->second;
}

CPPUNIT_NS_END

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    CPPUNIT_NS::CJniEnv::s_javaVM = vm;
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM* vm, void* reserved) {
    CPPUNIT_NS::CJClassManager::instance()->deleteAllGlobalClassRef();
}