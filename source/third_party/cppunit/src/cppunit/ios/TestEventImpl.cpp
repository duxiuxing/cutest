#include "TestEventImpl.h"

CUTEST_NS_BEGIN

CUTEST_NS::Event *
Event::createInstance()
{
  return new EventImpl;
}

EventImpl::EventImpl()
{
  ::pthread_mutex_init( &_mutex, NULL );
  ::pthread_cond_init( &_cond, NULL );
  _manual = false;
  _signaled = false;
}

void
EventImpl::wait()
{
  ::pthread_mutex_lock( &_mutex );
  while ( !_signaled )
  {
    ::pthread_cond_wait( &_cond, &_mutex );
  }
  if ( !_manual )
  {
    _signaled = false;
  }
  ::pthread_mutex_unlock( &_mutex );
}

void
EventImpl::post()
{
  ::pthread_mutex_lock( &_mutex );
  ::pthread_cond_signal( &_cond );
  _signaled = true;
  ::pthread_mutex_unlock( &_mutex );
}

void
EventImpl::reset()
{
  ::pthread_mutex_lock( &_mutex );
  _signaled = false;
  ::pthread_mutex_unlock( &_mutex );
}

void
EventImpl::destroy()
{
  ::pthread_mutex_destroy( &_mutex );
  ::pthread_cond_destroy( &_cond );
  _manual = true;
  _signaled = false;
  delete this;
}

CUTEST_NS_END
