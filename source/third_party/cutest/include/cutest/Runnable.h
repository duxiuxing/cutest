#pragma once

#include "cutest/Define.h"

CUTEST_NS_BEGIN

/*
  Runnable是一个抽象类

  用法见：
  - Runner::asyncRunOnMainThread()
  - Runner::delayRunOnMainThread()
*/
class Runnable
{
public:
  virtual ~Runnable() {}

  virtual void run() = 0;
};

CUTEST_NS_END
