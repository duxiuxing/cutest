#include "stdafx.h"
#include "synchronization_object_impl.h"

CPPUNIT_NS_BEGIN

SynchronizationObjectImpl::SynchronizationObjectImpl() {
    ::InitializeCriticalSectionAndSpinCount(&this->section, 0);
}

SynchronizationObjectImpl::~SynchronizationObjectImpl() {
    ::DeleteCriticalSection(&this->section);
}

void
SynchronizationObjectImpl::lock() {
    ::EnterCriticalSection(&this->section);
}

void
SynchronizationObjectImpl::unlock() {
    ::LeaveCriticalSection(&this->section);
}

CPPUNIT_NS_END
