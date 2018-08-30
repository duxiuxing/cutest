#pragma once

#include <cppunit/SynchronizedObject.h>
#include <cppunit/TestResult.h>

#include "cutest/Define.h"

CUTEST_NS_BEGIN

class GTEST_API_ Result : public CPPUNIT_NS::TestResult {
public:
    Result(CPPUNIT_NS::SynchronizedObject::SynchronizationObject* listener_lock, CPPUNIT_NS::SynchronizedObject::SynchronizationObject* stop_lock);
    virtual ~Result();

    // 重载CPPUNIT_NS::TestResult::m_stop相关的方法，通过stop_lock加锁
    virtual void reset();
    virtual void stop();
    virtual bool shouldStop() const;

protected:
    CPPUNIT_NS::SynchronizedObject::SynchronizationObject* stop_lock;

private:
    Result(const Result& other);
    Result& operator =(const Result& other);
};

CUTEST_NS_END
