#include "Result.h"

CUTEST_NS_BEGIN

Result::Result( SynchronizedObject::SynchronizationObject *listenerLock, SynchronizedObject::SynchronizationObject *stopLock )
  : CPPUNIT_NS::TestResult( listenerLock )
  , m_stopLock( stopLock )
{}

Result::~Result()
{
  delete m_stopLock;
}

void
Result::reset()
{
  ExclusiveZone zone( m_stopLock );
  m_stop = false;
}

bool
Result::shouldStop() const
{
  ExclusiveZone zone( m_stopLock );
  return m_stop;
}

void
Result::stop()
{
  ExclusiveZone zone( m_stopLock );
  m_stop = true;
}

CUTEST_NS_END
