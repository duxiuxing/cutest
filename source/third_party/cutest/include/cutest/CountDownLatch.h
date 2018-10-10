#pragma once

#include "cutest/Define.h"

CUTEST_NS_BEGIN

class CountDownLatchImpl;

class CUTEST_API_ CountDownLatch {
public:
    // count用于指定计数的初值
    CountDownLatch(int count);
    virtual ~CountDownLatch();

    /*
        调用此方法时：
        - 如果计数为0，则直接返回；
        - 如果计数比0大，会一直等待，直到计数变成0
    */
    void await();

    /*
        msTimeout表示最长等待时间，单位为毫秒。
        调用此方法时：
        - 如果计数为0，则直接返回true；
        - 如果计数比0大，会等待一段时间，等待时间内如果计数变成0，返回true；
        - 如果超出等待时间，计数还没有变成0，返回false；
    */
    bool await(unsigned int msTimeout);

    // 内部会对计数减1
    void countDown();

    // 返回当前计数的值
    int getCount();

protected:
    CountDownLatchImpl* impl;
};

CUTEST_NS_END
