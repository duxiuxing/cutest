#pragma once

#include <cppunit/SynchronizedObject.h>
#include <windows.h>

CPPUNIT_NS_BEGIN

class SynchronizationObjectImpl : public SynchronizedObject::SynchronizationObject {
    CRITICAL_SECTION m_section;
public:
    SynchronizationObjectImpl();
    virtual ~SynchronizationObjectImpl();

    virtual void lock() override;
    virtual void unlock() override;
};

CPPUNIT_NS_END
