#pragma once

#include <cppunit/Portability.h>
#include "CUTestDefine.h"

CUTEST_NS_BEGIN

class Event;

class CPPUNIT_API ManualEndTest
{
public:
  ManualEndTest();

public:
  void setEvent( CUTEST_NS::Event *event );
  virtual void endTest();
  bool isTestEnd();

protected:
  Event *event;

private:
  ManualEndTest( const ManualEndTest &other );
  ManualEndTest &operator=( const ManualEndTest &other );
};

CUTEST_NS_END
