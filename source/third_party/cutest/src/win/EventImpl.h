#pragma once

#include "cutest/Event.h"
#include <WTypes.h>

CUTEST_NS_BEGIN

class EventImpl : public Event {
    HANDLE hEvent;

public:
    EventImpl();

    virtual void wait();
    virtual void wait(unsigned int msTimeout);
    virtual void post();
    virtual void reset();
    virtual void destroy();
};

CUTEST_NS_END
