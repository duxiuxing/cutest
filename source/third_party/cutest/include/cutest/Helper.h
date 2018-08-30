#pragma once

#include "cutest/Define.h"

CUTEST_NS_BEGIN

/*
    版本信息:
    - 本地编译版本返回编译时间的字符串
    - 持续集成版本返回的内容为：“主版本号.子版本号.修订版本号.编译版本号”
*/
GTEST_API_ const char* version();

GTEST_API_ thread_id currentThreadId();
GTEST_API_ thread_id mainThreadId();
GTEST_API_ bool isOnMainThread();

// 返回当前系统的时间，单位为ms
GTEST_API_ unsigned long long tickCount64();

CUTEST_NS_END
