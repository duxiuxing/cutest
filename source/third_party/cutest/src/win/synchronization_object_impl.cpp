#include "stdafx.h"
#include "synchronization_object_impl.h"

CPPUNIT_NS_BEGIN

SynchronizationObjectImpl::SynchronizationObjectImpl() {
    ::InitializeCriticalSectionAndSpinCount(&m_section, 0);
}

SynchronizationObjectImpl::~SynchronizationObjectImpl() {
    ::DeleteCriticalSection(&m_section);
}

void
SynchronizationObjectImpl::lock() {
    ::EnterCriticalSection(&m_section);
}

void
SynchronizationObjectImpl::unlock() {
    ::LeaveCriticalSection(&m_section);
}

CPPUNIT_NS_END
