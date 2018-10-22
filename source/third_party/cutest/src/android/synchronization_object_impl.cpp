#include "synchronization_object_impl.h"

CPPUNIT_NS_BEGIN

SynchronizationObjectImpl::SynchronizationObjectImpl() {
    ::pthread_mutex_init(&this->mutex, NULL);
}

SynchronizationObjectImpl::~SynchronizationObjectImpl() {
    ::pthread_mutex_destroy(&this->mutex);
}

void SynchronizationObjectImpl::lock() {
    ::pthread_mutex_lock(&this->mutex);
}

void SynchronizationObjectImpl::unlock() {
    ::pthread_mutex_unlock(&this->mutex);
}

CPPUNIT_NS_END
