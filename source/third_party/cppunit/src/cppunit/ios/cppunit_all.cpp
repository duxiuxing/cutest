#include "src/gtest-all.cc"

#include "../cppunit_core.cpp"

// 跨平台实现的部分
#include "../ProgressListenerManager.cpp"
#include "../RunnerBase.cpp"

// iOS实现的部分
#include "DecoratorImpl.cpp"
//#include "MainTestRunnerImpl.mm"
#include "SynchronizationObjectImpl.cpp"
#include "EventImpl.cpp"
#include "ProgressLogger.cpp"
