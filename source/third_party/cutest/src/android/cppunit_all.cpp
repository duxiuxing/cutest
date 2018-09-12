#include "../cppunit_core.cpp"

// 跨平台实现的部分
#include "../AutoEndTest.cpp"
#include "../MainTestRunnerBase.cpp"
#include "../ManualEndTestCase.cpp"
#include "../TestProgressListenerManager.cpp"

// Android实现的部分
#include "JClassManager.cpp"
#include "JniEnv.cpp"
#include "JniHelp.cpp"
#include "MainTestDecoratorImpl.cpp"
#include "MainTestRunnerImpl.cpp"
#include "SynchronizationObjectImpl.cpp"
#include "TestEventImpl.cpp"
#include "JniProgressListener.cpp"
#include "Logger.cpp"
