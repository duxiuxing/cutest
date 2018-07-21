#pragma once

#include <cppunit/Portability.h>
#include "CUTestDefine.h"

CUTEST_NS_BEGIN

class Event;

class CPPUNIT_API ExplicitEndTest
{
public:
  ExplicitEndTest();

public:
  void setEvent( CUTEST_NS::Event *event );
  virtual void endTest();
  bool isTestEnd();

protected:
  Event *event;

private:
  ExplicitEndTest( const ExplicitEndTest &other );
  ExplicitEndTest &operator=( const ExplicitEndTest &other );
};

CUTEST_NS_END
