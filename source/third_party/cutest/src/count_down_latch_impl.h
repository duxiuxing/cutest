#pragma once

#include "cutest/event.h"

CUTEST_NS_BEGIN

class CountDownLatchImpl {
public:
    // count用于指定计数的初值
    CountDownLatchImpl(int count);
    virtual ~CountDownLatchImpl();

    /*
        调用此方法时：
        - 如果计数为0，则直接返回；
        - 如果计数比0大，会一直等待，直到计数变成0
    */
    void Await();

    /*
        msTimeout表示最长等待时间，单位为毫秒。
        调用此方法时：
        - 如果计数为0，则直接返回true；
        - 如果计数比0大，会等待一段时间，等待时间内如果计数变成0，返回true；
        - 如果超出等待时间，计数还没有变成0，返回false；
    */
    bool Await(unsigned int msTimeout);

    // 内部会对计数减1
    void CountDown();

    // 返回当前计数的值
    int Count();

protected:
    mutable int m_count; // 保存构造函数的入参
    void AwaitOnMainThread();
    bool AwaitOnMainThread(unsigned int msTimeout);

    Event* m_event; // 如果是在工作线程调用Await()，需要通过事件来实现等待
    void AwaitOnWorkerThread();
    bool AwaitOnWorkerThread(unsigned int msTimeout);
};

CUTEST_NS_END
