#include "../cppunit_core.cpp"

// 跨平台实现的部分
#include "../ProgressListenerManager.cpp"
#include "../RunnerBase.cpp"

// Android实现的部分
#include "JClassManager.cpp"
#include "JniEnv.cpp"
#include "JniHelp.cpp"
#include "TestDecoratorImpl.cpp"
#include "TestRunnerImpl.cpp"
#include "SynchronizationObjectImpl.cpp"
#include "EventImpl.cpp"
#include "ProgressListenerJni.cpp"
#include "ProgressLogger.cpp"
