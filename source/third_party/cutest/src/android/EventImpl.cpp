#include "TestEventImpl.h"

CPPUNIT_NS_BEGIN

TestEvent* TestEvent::createInstance() {
    return new TestEventImpl;
}

TestEventImpl::TestEventImpl() {
    ::pthread_mutex_init(&_mutex, NULL);
    ::pthread_cond_init(&_cond, NULL);
    _manual = false;
    _signaled = false;
}

void TestEventImpl::wait() {
    ::pthread_mutex_lock(&_mutex);
    while (!_signaled)
        ::pthread_cond_wait(&_cond, &_mutex);
    if (!_manual)
        _signaled = false;
    ::pthread_mutex_unlock(&_mutex);
}

void TestEventImpl::post() {
    ::pthread_mutex_lock(&_mutex);
    ::pthread_cond_signal(&_cond);
    _signaled = true;
    ::pthread_mutex_unlock(&_mutex);
}

void TestEventImpl::reset() {
    ::pthread_mutex_lock(&_mutex);
    _signaled = false;
    ::pthread_mutex_unlock(&_mutex);
}

void TestEventImpl::destroy() {
    ::pthread_mutex_destroy(&_mutex);
    ::pthread_cond_destroy(&_cond);
    _manual = true;
    _signaled = false;
    delete this;
}

CPPUNIT_NS_END