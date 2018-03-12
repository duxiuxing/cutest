#include "EventImpl.h"

CUTEST_NS_BEGIN

Event *
Event::createInstance()
{
  return new EventImpl;
}

EventImpl::EventImpl()
{
  m_event = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}

void
EventImpl::wait()
{
  ::WaitForSingleObjectEx( m_event, INFINITE, FALSE );
}

void
EventImpl::reset()
{
  ::ResetEvent( m_event );
}

void
EventImpl::post()
{
  ::SetEvent( m_event );
}

void
EventImpl::destroy()
{
  ::CloseHandle( m_event );
  delete this;
}

CUTEST_NS_END
