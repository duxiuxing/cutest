#pragma once

#include <cppunit/TestEvent.h>
#include <pthread.h>

CPPUNIT_NS_BEGIN

class TestEventImpl : public TestEvent {
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    bool _manual;
    bool _signaled;

public:
    TestEventImpl();

    virtual void wait();
    virtual void post();
    virtual void reset();
    virtual void destroy();
};

CPPUNIT_NS_END