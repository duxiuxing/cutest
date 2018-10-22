#pragma once

#include "cutest/define.h"

CUTEST_NS_BEGIN

/*
    Runnable是一个抽象类

    用法见：
    - Runner::AsyncRunOnMainThread()
    - Runner::DelayRunOnMainThread()
*/
class Runnable {
public:
    virtual ~Runnable() {}

    virtual void Run() = 0;
};

CUTEST_NS_END
