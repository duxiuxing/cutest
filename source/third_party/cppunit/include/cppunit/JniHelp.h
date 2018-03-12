#pragma once

#include <cppunit/Portability.h>
#include <string>

CPPUNIT_NS_BEGIN

class JniHelp {
public:
    static std::string makeFilePathShorter(std::string path);
};

CPPUNIT_NS_END