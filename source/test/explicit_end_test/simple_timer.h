#pragma once

#include "gtest/gtest.h"
#include "cutest/runnable.h"

/*
    SimpleTimer是一个单例对象，用于在指定时长之后触发回调操作，使用步骤如下：
    1. 调用SimpleTimer::SetCallback()，指定时长，设置回调接口
    2. 到时间之后，SimpleTimer会触发一次回调
    3. 在超时之前，可以调用SimpleTimer::RemoveCallback()取消回调

    注意：SimpleTimer的计时器不是周期性的，一次SetCallback()对应一次回调
*/
class SimpleTimer
    : public CUTEST_NS::Runnable {
public:
    static SimpleTimer* Instance();

    class Callback {
    public:
        virtual void OnTimeUp() = 0;
    };

    void SetCallback(unsigned int msElapse, Callback* callback);
    void RemoveCallback(Callback* callback);

private:
    SimpleTimer();
    Callback* m_callback;

    // 实现Runnable::Run()
    virtual void Run();

    GTEST_DISALLOW_COPY_AND_ASSIGN_(SimpleTimer);
};
