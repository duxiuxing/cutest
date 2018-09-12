#pragma once

#include "cutest/Define.h"

#include <jni.h>
#include <map>
#include <string>

CUTEST_NS_BEGIN

class JClassManager {
public:
    static JClassManager* instance();

    void registerGlobalClassName(const char* class_name);
    void newAllGlobalClassRef();
    void deleteAllGlobalClassRef();
    jclass findGlobalClass(const char* class_name);

private:
    JClassManager();
    typedef std::map<std::string, jclass> JClassMap;
    JClassMap jclass_map;
};

CUTEST_NS_END
