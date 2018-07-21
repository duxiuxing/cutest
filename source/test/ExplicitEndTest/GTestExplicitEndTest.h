#pragma once

#include <gtest/gtest.h>
#include "cutest/Runnable.h"

class GTestExplicitEndTest
  : public testing::ExplicitEndTest
  , public CUTEST_NS::Runnable {
 public:
  GTestExplicitEndTest();

  virtual void SetUp() override;
  virtual void TearDown() override;

  // 实现Runnable::run()
  virtual void run();

  unsigned long long tick_count_setup;
};
