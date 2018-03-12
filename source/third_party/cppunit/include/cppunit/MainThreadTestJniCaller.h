#pragma once

#include "main_thread_test_jni.h"

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

CPPUNIT_NS_BEGIN

template<class T>
class MainThreadTestJniCaller : public CUTEST_NS::Runnable
{
public:
  MainThreadTestJniCaller( CUTEST_NS::Event *event, const char *method_name )
    : _event( event )
    , _javaMethodName( method_name )
    , _testMethod( NULL )
  {}

  typedef void ( T::*TEST_METHOD )();

  void Hook_TestMethod( TEST_METHOD method )
  {
    _testMethod = method;
  }

  virtual void run()
  {
    T *test = new T;
    test->SetEvent( _event );
    test->SetJaveMethodName( _javaMethodName.c_str() );
    ( test->*_testMethod )();
    delete test;
  }

private:
  MainThreadTestJniCaller( const MainThreadTestJniCaller & );
  MainThreadTestJniCaller &operator =( const MainThreadTestJniCaller & );

  CUTEST_NS::Event *_event;
  std::string _javaMethodName;
  TEST_METHOD _testMethod;
};

CPPUNIT_NS_END

/*
@brief 在主线程执行的测试用例方法，方法执行完则当前用例自动结束。

@param test_class 测试代码对应的class的名字，它必须继承于CPPUNIT_NS::MainThreadTest；
@param test_method 框架会在主线程创建test_class的实例，然后调用test_class::test_method()方法；
@remark 测试框架会：
  - 在主线程创建test_class的实例；
  - 调用test_class::test_method()方法；
  - 方法执行完则当前测试用例自动结束。
*/
#define CPPUNIT_MAIN_THREAD_JNI_TEST(test_class, java_test_method) \
    void java_test_method() { \
        CUTEST_NS::Runner* runner = CUTEST_NS::Runner::instance(); \
        CUTEST_NS::Event* event = CUTEST_NS::Event::createInstance(); \
        CPPUNIT_NS::MainThreadTestJniCaller<test_class> caller(event, #java_test_method); \
        caller.Hook_TestMethod(&test_class::run); \
        runner->asyncRunOnMainThread(&caller, false); \
        event->wait(); event->destroy(); \
    }
