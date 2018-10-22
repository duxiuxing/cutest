#pragma once

#include <cppunit/Portability.h>
#include "cutest/define.h"

CUTEST_NS_BEGIN

class Event {
public:
    // 外部要通过CreateInstance()来创建Event对象
    GTEST_API_ static Event* CreateInstance();

protected:
    // 外部要通过Destroy()来销毁Event对象
    virtual ~Event() {}

public:
    virtual void Wait() = 0;
    virtual void Wait(unsigned int msTimeout) = 0;
    virtual void Post() = 0;
    virtual void Reset() = 0;
    virtual void Destroy() = 0;
};

CUTEST_NS_END
