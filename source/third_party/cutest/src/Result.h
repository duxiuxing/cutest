#pragma once

#include <cppunit/SynchronizedObject.h>
#include <cppunit/TestResult.h>

#include "cutest/CUTestDefine.h"

CUTEST_NS_BEGIN

class CPPUNIT_API Result : public CPPUNIT_NS::TestResult
{
public:
  Result( CPPUNIT_NS::SynchronizedObject::SynchronizationObject *listenerLock, CPPUNIT_NS::SynchronizedObject::SynchronizationObject *stopLock );
  virtual ~Result();

  // 重载m_stop相关的方法
  virtual void reset();
  virtual void stop();
  virtual bool shouldStop() const;

protected:
  CPPUNIT_NS::SynchronizedObject::SynchronizationObject *m_stopLock;

private:
  Result( const Result &other );
  Result &operator =( const Result &other );
};

CUTEST_NS_END
