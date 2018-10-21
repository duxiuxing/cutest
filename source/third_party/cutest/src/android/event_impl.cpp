#include "EventImpl.h"

CUTEST_NS_BEGIN

Event*
Event::createInstance() {
    return new EventImpl;
}

EventImpl::EventImpl() {
    ::pthread_mutex_init(&this->mutex, NULL);
    ::pthread_cond_init(&this->cond, NULL);
    this->manual = false;
    this->signaled = false;
}

void
EventImpl::wait() {
    ::pthread_mutex_lock(&this->mutex);
    while (!this->signaled) {
        ::pthread_cond_wait(&this->cond, &this->mutex);
    }
    if (!this->manual) {
        this->signaled = false;
    }
    ::pthread_mutex_unlock(&this->mutex);
}

void
EventImpl::wait(unsigned int timeout_ms) {
}

void
EventImpl::post() {
    ::pthread_mutex_lock(&this->mutex);
    ::pthread_cond_signal(&this->cond);
    this->signaled = true;
    ::pthread_mutex_unlock(&this->mutex);
}

void
EventImpl::reset() {
    ::pthread_mutex_lock(&this->mutex);
    this->signaled = false;
    ::pthread_mutex_unlock(&this->mutex);
}

void
EventImpl::destroy() {
    ::pthread_mutex_destroy(&this->mutex);
    ::pthread_cond_destroy(&this->cond);
    this->manual = true;
    this->signaled = false;
    delete this;
}

CUTEST_NS_END
