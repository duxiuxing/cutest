#include "stdafx.h"
#include "SynchronizationObjectImpl.h"

CPPUNIT_NS_BEGIN

SynchronizationObjectImpl::SynchronizationObjectImpl()
{
  ::InitializeCriticalSectionAndSpinCount( &m_sect, 0 );
}

SynchronizationObjectImpl::~SynchronizationObjectImpl()
{
  ::DeleteCriticalSection( &m_sect );
}

void
SynchronizationObjectImpl::lock()
{
  ::EnterCriticalSection( &m_sect );
}

void
SynchronizationObjectImpl::unlock()
{
  ::LeaveCriticalSection( &m_sect );
}

CPPUNIT_NS_END
