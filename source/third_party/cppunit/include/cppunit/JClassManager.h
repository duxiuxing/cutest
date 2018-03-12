#pragma once

#include <cppunit/Portability.h>
#include <jni.h>
#include <map>
#include <string>

CPPUNIT_NS_BEGIN

class JClassManager
{
public:
  static JClassManager *instance();

  void registerGlobalClassName( const char *class_name );
  void newAllGlobalClassRef();
  void deleteAllGlobalClassRef();
  jclass findGlobalClass( const char *class_name );

private:
  JClassManager();
  typedef std::map<std::string, jclass> JClassMap;
  JClassMap _jClassMap;
};

CPPUNIT_NS_END
