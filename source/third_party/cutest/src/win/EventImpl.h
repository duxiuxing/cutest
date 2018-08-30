#pragma once

#include "cutest/Event.h"
#include <WTypes.h>

CUTEST_NS_BEGIN

class EventImpl : public Event {
    HANDLE event_handle;

public:
    EventImpl();

    virtual void wait();
    virtual void wait(unsigned int timeout_ms);
    virtual void post();
    virtual void reset();
    virtual void destroy();
};

CUTEST_NS_END
