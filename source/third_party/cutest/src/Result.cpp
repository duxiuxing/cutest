#include "Result.h"

CUTEST_NS_BEGIN

Result::Result(SynchronizedObject::SynchronizationObject* listener_lock, SynchronizedObject::SynchronizationObject* stop_lock_in)
    : CPPUNIT_NS::TestResult(listener_lock)
    , stop_lock(stop_lock_in) {}

Result::~Result() {
    delete this->stop_lock;
}

void
Result::reset() {
    ExclusiveZone zone(this->stop_lock);
    CPPUNIT_NS::TestResult::m_stop = false;
}

bool
Result::shouldStop() const {
    ExclusiveZone zone(this->stop_lock);
    return CPPUNIT_NS::TestResult::m_stop;
}

void
Result::stop() {
    ExclusiveZone zone(this->stop_lock);
    CPPUNIT_NS::TestResult::m_stop = true;
}

CUTEST_NS_END
