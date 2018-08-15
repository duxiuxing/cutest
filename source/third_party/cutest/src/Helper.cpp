#include "cutest/Helper.h"
#include "gtest/gtest-message.h"

#include <string>

const char *
CUTEST_NS::version()
{
  static std::string version;

  if ( version.empty() )
  {
    if ( 0 == BUILD_NUMBER )
    {
      // 本地编译会进入这个分支，这个时候取当前时刻作为版本号。
      version = __TIME__;
      version.append( " " );
      version.append( __DATE__ );
    }
    else
    {
      // 持续集成会进入这个分支，这个时候按照实际版本信息来拼接版本号。
      int major   = MAJOR_VERSION;
      int minor   = MINOR_VERSION;
      int fix     = FIX_VERSION;
      int build   = BUILD_NUMBER;
      version = testing::internal::StreamableToString( major ) + "."
                + testing::internal::StreamableToString( minor ) + "."
                + testing::internal::StreamableToString( fix ) + "."
                + testing::internal::StreamableToString( build );
    }
  }

  return version.c_str();
}

bool
CUTEST_NS::isOnMainThread()
{
  return ( CUTEST_NS::mainThreadId() == CUTEST_NS::currentThreadId() );
}
