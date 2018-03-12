#include "cppunit/JniEnv.h"

#include <map>

CPPUNIT_NS_BEGIN

JavaVM *CJniEnv::s_javaVM = NULL;

CJniEnv::CJniEnv()
  : _callDetach( false )
  , _env( NULL )
{
  s_javaVM->GetEnv( ( void ** )&_env, JNI_VERSION_1_4 );

  if ( !_env )
  {
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_4;
    args.name = NULL;
    args.group = NULL;
    if ( JNI_OK == s_javaVM->AttachCurrentThread( &_env, &args ) )
    {
      _callDetach = true;
    }
  }
}

CJniEnv::~CJniEnv()
{
  if ( _callDetach )
  {
    s_javaVM->DetachCurrentThread();
  }
}

CPPUNIT_NS_END
