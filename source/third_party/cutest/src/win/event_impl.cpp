#include "event_impl.h"

CUTEST_NS_BEGIN

Event* Event::CreateInstance() {
    return new EventImpl;
}

EventImpl::EventImpl() : m_hEvent(NULL) {
    m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

void EventImpl::Wait() {
    ::WaitForSingleObjectEx(m_hEvent, INFINITE, FALSE);
}

void EventImpl::Wait(unsigned int msTimeout) {
    ::WaitForSingleObjectEx(m_hEvent, msTimeout, FALSE);
}

void EventImpl::Reset() {
    ::ResetEvent(m_hEvent);
}

void EventImpl::Post() {
    ::SetEvent(m_hEvent);
}

void EventImpl::Destroy() {
    ::CloseHandle(m_hEvent);
    delete this;
}

CUTEST_NS_END
