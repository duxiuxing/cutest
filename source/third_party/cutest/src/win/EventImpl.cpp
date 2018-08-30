#include "EventImpl.h"

CUTEST_NS_BEGIN

Event*
Event::createInstance() {
    return new EventImpl;
}

EventImpl::EventImpl() : event_handle(NULL) {
    this->event_handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

void
EventImpl::wait() {
    ::WaitForSingleObjectEx(this->event_handle, INFINITE, FALSE);
}

void
EventImpl::wait(unsigned int timeout_ms) {
    ::WaitForSingleObjectEx(this->event_handle, timeout_ms, FALSE);
}

void
EventImpl::reset() {
    ::ResetEvent(this->event_handle);
}

void
EventImpl::post() {
    ::SetEvent(this->event_handle);
}

void
EventImpl::destroy() {
    ::CloseHandle(this->event_handle);
    delete this;
}

CUTEST_NS_END
