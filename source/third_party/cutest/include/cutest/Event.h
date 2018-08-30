#pragma once

#include <cppunit/Portability.h>
#include "cutest/Define.h"

CUTEST_NS_BEGIN

class Event {
public:
    // 外部要通过CreateInstance()来创建Event对象
    GTEST_API_ static Event* createInstance();

protected:
    // 外部要通过Destroy()来销毁Event对象
    virtual ~Event() {}

public:
    virtual void wait() = 0;
    virtual void wait(unsigned int timeout_ms) = 0;
    virtual void post() = 0;
    virtual void reset() = 0;
    virtual void destroy() = 0;
};

CUTEST_NS_END
