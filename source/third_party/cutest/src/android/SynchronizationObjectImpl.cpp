#include "SynchronizationObjectImpl.h"

CPPUNIT_NS_BEGIN

SynchronizationObjectImpl::SynchronizationObjectImpl() {
    ::pthread_mutex_init(&_mutex, NULL);
}

SynchronizationObjectImpl::~SynchronizationObjectImpl() {
    ::pthread_mutex_destroy(&_mutex);
}

void SynchronizationObjectImpl::lock() {
    ::pthread_mutex_lock(&_mutex);
}

void SynchronizationObjectImpl::unlock() {
    ::pthread_mutex_unlock(&_mutex);
}

CPPUNIT_NS_END