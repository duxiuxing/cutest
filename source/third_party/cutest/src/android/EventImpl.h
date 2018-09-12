#pragma once

#include "cutest/Event.h"
#include <pthread.h>

CUTEST_NS_BEGIN

class EventImpl : public Event {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool manual;
    bool signaled;

public:
    EventImpl();

    virtual void wait();
    virtual void wait(unsigned int timeout_ms);
    virtual void post();
    virtual void reset();
    virtual void destroy();
};

CUTEST_NS_END
