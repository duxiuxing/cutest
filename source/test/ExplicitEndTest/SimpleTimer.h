#pragma once

#include <gtest/gtest.h>
#include "cutest/Runnable.h"

/*
    SimpleTimer是一个单例对象，用于在指定时长之后触发回调操作，使用步骤如下：
    1. 调用SimpleTimer::setCallback()，指定时长，设置回调接口
    2. 到时间之后，SimpleTimer会触发一次回调
    3. 在超时之前，可以调用SimpleTimer::removeCallback()取消回调

    注意：SimpleTimer的计时器不是周期性的，一次setCallback()对应一次回调
*/
class SimpleTimer
    : public CUTEST_NS::Runnable {
public:
    static SimpleTimer* instance();

    class Callback {
    public:
        virtual void onTimeUp() = 0;
    };

    void setCallback(unsigned int elapse_ms, Callback* callback);
    void removeCallback(Callback* callback);

private:
    SimpleTimer();
    Callback* callback;

    // 实现Runnable::run()
    virtual void run();

    GTEST_DISALLOW_COPY_AND_ASSIGN_(SimpleTimer);
};
