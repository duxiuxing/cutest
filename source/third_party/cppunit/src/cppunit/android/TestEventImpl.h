#pragma once

#include "cutest/Event.h"
#include <pthread.h>

CUTEST_NS_BEGIN

class EventImpl : public Event
{
  pthread_mutex_t _mutex;
  pthread_cond_t _cond;
  bool _manual;
  bool _signaled;

public:
  EventImpl();

  virtual void wait();
  virtual void post();
  virtual void reset();
  virtual void destroy();
};

CUTEST_NS_END
