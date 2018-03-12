#include "ExampleManualEndTest.h"

void
ExampleManualEndTest::manual_end_test_after_1s()
{
  // 框架会在主线程调用此方法
  // 1s之后再调用EndTest()来结束用例
  CUTEST_NS::Runner::instance()->delayRunOnMainThread( 1000, this, false );
}

void
ExampleManualEndTest::run()
{
  // 这里才结束测试用例
  endTest();
}

void
ExampleManualEndTest::auto_end_test_after_1s()
{}
