#pragma once

#include <gtest/gtest.h>
#include "SimpleTimer.h"

class GTestExplicitEndTest
  : public testing::ExplicitEndTest
  , public SimpleTimer::Callback {
 public:
  GTestExplicitEndTest();

  virtual void TearDown() override;

  // 实现SimpleTimer::Callback
  virtual void onTimeUp();

  unsigned long long tick_count_start;
};
