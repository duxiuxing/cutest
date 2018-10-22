#include "../cppunit_core.cpp"

// 跨平台实现的部分
#include "../auto_end_test.cpp"
#include "../runner_base.cpp"
#include "../ManualEndTestCase.cpp"
#include "../listener_manager.cpp"

// Android实现的部分
#include "JClassManager.cpp"
#include "JniEnv.cpp"
#include "JniHelp.cpp"
#include "decorator_impl.cpp"
#include "runner_impl.cpp"
#include "synchronization_object_impl.cpp"
#include "event_impl.cpp"
#include "JniListener.cpp"
#include "logger.cpp"
