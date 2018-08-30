#pragma once

#include "gtest/internal/gtest-port.h"

#define CUTEST_API_ GTEST_API_

#define CUTEST_NS_BEGIN namespace CUTest {
#define CUTEST_NS_END }
#define CUTEST_NS CUTest

CUTEST_NS_BEGIN

// 主版本号
#define MAJOR_VERSION 0

// 子版本号
#define MINOR_VERSION 0

// 修订版本号
#define FIX_VERSION 0

// 编译版本号
#define BUILD_NUMBER 0

CUTEST_NS_END

#if (defined(_WIN32) || defined(_WIN64))

    CUTEST_NS_BEGIN
    typedef unsigned long thread_id;
    CUTEST_NS_END

#else

    #include <pthread.h>
    #include <unistd.h>
    CUTEST_NS_BEGIN
    typedef pid_t thread_id;
    CUTEST_NS_END

#endif
