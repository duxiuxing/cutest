#pragma once

#include <cppunit/SynchronizedObject.h>
#include <pthread.h>

CPPUNIT_NS_BEGIN

class SynchronizationObjectImpl : public SynchronizedObject::SynchronizationObject {
    pthread_mutex_t mutex;
public:
    SynchronizationObjectImpl();
    virtual ~SynchronizationObjectImpl();

    virtual void lock() override;
    virtual void unlock() override;
};

CPPUNIT_NS_END
