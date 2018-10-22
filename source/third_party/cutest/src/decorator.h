#pragma once

// cppunit
#include <cppunit/Exception.h>
#include <cppunit/TestListener.h>
#include <cppunit/TestResultCollector.h>

#include "cutest/define.h"

CUTEST_NS_BEGIN

class Decorator {
public:
    // 工厂方法，外部要通过它来创建Decorator对象
    static Decorator* CreateInstance(CPPUNIT_NS::Test* test);

protected:
    // 外部要通过MainTestDecorator::Destroy()来销毁MainTestDecorator对象
    virtual ~Decorator() {}

public:
    // 销毁Decorator对象
    virtual void Destroy() = 0;

    virtual void AddListener(CPPUNIT_NS::TestListener* listener) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual void AddFailure(bool isError, CPPUNIT_NS::Exception* exception) = 0;
    virtual const CPPUNIT_NS::TestResultCollector* TestResultCollector() = 0;
};

CUTEST_NS_END
