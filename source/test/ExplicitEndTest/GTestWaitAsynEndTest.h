#pragma once

#include "gtest/gtest.h"
#include "CountDownLatch.h"
#include "SimpleTimer.h"

class GTestWaitAsynEndTest
  : public testing::Test
  , public SimpleTimer::Callback {
 public:
  GTestWaitAsynEndTest();

  virtual void SetUp() override;
  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  CountDownLatch* time_up;
};
