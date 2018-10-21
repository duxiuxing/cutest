#include "../cppunit_core.cpp"

// 跨平台实现的部分
#include "../AutoEndTest.cpp"
#include "../MainTestRunnerBase.cpp"
#include "../ManualEndTestCase.cpp"
#include "../ListenerManager.cpp"

// Android实现的部分
#include "JClassManager.cpp"
#include "JniEnv.cpp"
#include "JniHelp.cpp"
#include "decorator_impl.cpp"
#include "MainTestRunnerImpl.cpp"
#include "SynchronizationObjectImpl.cpp"
#include "TestEventImpl.cpp"
#include "JniListener.cpp"
#include "Logger.cpp"
