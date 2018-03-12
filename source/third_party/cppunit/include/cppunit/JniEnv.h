#pragma once

#include <cppunit/Portability.h>
#include <jni.h>

CPPUNIT_NS_BEGIN

class CJniEnv
{
public:
  static JavaVM *s_javaVM;
  CJniEnv();
  virtual ~CJniEnv();

  JNIEnv *operator -> ()
  {
    return _env;
  }

private:
  bool _callDetach;;
  JNIEnv *_env;
};

CPPUNIT_NS_END
