#include "EventImpl.h"

CUTEST_NS_BEGIN

Event*
Event::createInstance() {
    return new EventImpl;
}

EventImpl::EventImpl() : hEvent(NULL) {
    this->hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

void
EventImpl::wait() {
    ::WaitForSingleObjectEx(this->hEvent, INFINITE, FALSE);
}

void
EventImpl::wait(unsigned int msTimeout) {
    ::WaitForSingleObjectEx(this->hEvent, msTimeout, FALSE);
}

void
EventImpl::reset() {
    ::ResetEvent(this->hEvent);
}

void
EventImpl::post() {
    ::SetEvent(this->hEvent);
}

void
EventImpl::destroy() {
    ::CloseHandle(this->hEvent);
    delete this;
}

CUTEST_NS_END
