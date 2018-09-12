#pragma once

#include <cppunit/SynchronizedObject.h>
#include <pthread.h>

CPPUNIT_NS_BEGIN

class SynchronizationObjectImpl : public SynchronizedObject::SynchronizationObject {
    pthread_mutex_t _mutex;
public:
    SynchronizationObjectImpl();
    virtual ~SynchronizationObjectImpl();

    // 重载SynchronizationObject的成员方法
    virtual void lock();
    virtual void unlock();
};

CPPUNIT_NS_END