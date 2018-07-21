#pragma once

#include <gtest/gtest.h>
#include "cutest/Runnable.h"

class GTestExplicitEndTest
  : public testing::ExplicitEndTest
  , public CUTEST_NS::Runnable {
 public:
  // 实现Runnable::run()
  virtual void run();
};
