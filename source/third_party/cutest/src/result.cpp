#include "Result.h"

CUTEST_NS_BEGIN

Result::Result(SynchronizedObject::SynchronizationObject* listenerLock, SynchronizedObject::SynchronizationObject* stopLockParam)
    : CPPUNIT_NS::TestResult(listenerLock)
    , stopLock(stopLockParam) {}

Result::~Result() {
    delete this->stopLock;
}

void
Result::reset() {
    ExclusiveZone zone(this->stopLock);
    CPPUNIT_NS::TestResult::m_stop = false;
}

bool
Result::shouldStop() const {
    ExclusiveZone zone(this->stopLock);
    return CPPUNIT_NS::TestResult::m_stop;
}

void
Result::stop() {
    ExclusiveZone zone(this->stopLock);
    CPPUNIT_NS::TestResult::m_stop = true;
}

CUTEST_NS_END
