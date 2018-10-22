#pragma once

#include "cutest/event.h"
#include <WTypes.h>

CUTEST_NS_BEGIN

class EventImpl : public Event {
    HANDLE m_hEvent;

public:
    EventImpl();

    virtual void Wait();
    virtual void Wait(unsigned int msTimeout);
    virtual void Post();
    virtual void Reset();
    virtual void Destroy();
};

CUTEST_NS_END
