#include "cppunit/JClassManager.h"
#include "cppunit/JniEnv.h"

#include <map>

CPPUNIT_NS_BEGIN

JClassManager *JClassManager::instance()
{
  static JClassManager jclass_manager;
  return &jclass_manager;
}

JClassManager::JClassManager()
{}

void
JClassManager::registerGlobalClassName( const char *class_name )
{
  _jClassMap[class_name] = NULL;
}

void
JClassManager::newAllGlobalClassRef()
{
  CJniEnv env;
  JClassMap::iterator it = _jClassMap.begin();
  while ( it != _jClassMap.end() )
  {
    it->second = ( jclass )env->NewGlobalRef( env->FindClass( it->first.c_str() ) );
    ++it;
  }
}

void
JClassManager::deleteAllGlobalClassRef()
{
  CJniEnv env;
  JClassMap::iterator it = _jClassMap.begin();
  while ( it != _jClassMap.end() )
  {
    env->DeleteGlobalRef( it->second );
    ++it;
  }
}

jclass
JClassManager::findGlobalClass( const char *class_name )
{
  JClassMap::iterator it = _jClassMap.find( class_name );
  if ( it == _jClassMap.end() )
  {
    return NULL;
  }
  else
  {
    return it->second;
  }
}

CPPUNIT_NS_END

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad( JavaVM *vm, void *reserved )
{
  CPPUNIT_NS::CJniEnv::s_javaVM = vm;
  return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL
JNI_OnUnload( JavaVM *vm, void *reserved )
{
  CPPUNIT_NS::JClassManager::instance()->deleteAllGlobalClassRef();
}
