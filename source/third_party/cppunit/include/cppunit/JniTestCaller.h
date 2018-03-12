#pragma once

#include <cppunit/JClassManager.h>
#include <cppunit/Portability.h>
#include <string>

CPPUNIT_NS_BEGIN

template <class Fixture>
class JniTestCaller : public TestCase
{
public:
  JniTestCaller( std::string test_name, Fixture *fixture, std::string method_name )
    : TestCase( test_name )
    , m_fixture( fixture )
    , m_methodName( method_name )
  {}

  ~JniTestCaller()
  {
    if ( m_fixture )
    {
      delete m_fixture;
    }
  }

  // 重载TestCase::runTest()
  virtual void runTest()
  {
    jclass cls = CJClassManager::instance()->findGlobalClass( Fixture::jclassName() );

    CJniEnv env;

    jobject obj = NULL;
    // 创建Java对象
    {
      jmethodID id = env->GetMethodID( cls, "<init>", "()V" );
      obj = env->NewObject( cls, id );
    }

    // 调用Java方法
    {
      jmethodID id = env->GetMethodID( cls, m_methodName.c_str(), "()V" );
      env->CallVoidMethod( obj, id );
      env->DeleteLocalRef( obj );
    }
  }

  // 重载TestFixture::setUp()
  virtual void setUp()
  {
    m_fixture->setUp();
  }

  // 重载TestFixture::tearDown()
  virtual void tearDown()
  {
    m_fixture->tearDown();
  }

private:
  JniTestCaller( const JniTestCaller &other );
  JniTestCaller &operator =( const JniTestCaller &other );

private:
  Fixture *m_fixture;
  std::string m_methodName;
};

template<class TestCaseType>
class AutoRegisterJavaClass
{
public:
  AutoRegisterJavaClass()
  {
    CJClassManager::instance()->registerGlobalClassName( TestCaseType::jclassName() );
  }
};

CPPUNIT_NS_END

#define DEFINE_JCLASS_NAME(jclass_name) \
public: \
    static const char* jclassName() { \
        return jclass_name; \
    }

#define CPPUNIT_JNI_TEST(test_method_name)                  \
    CPPUNIT_TEST_SUITE_ADD_TEST(                            \
        (new CPPUNIT_NS::JniTestCaller<TestFixtureType>(    \
            context.getTestNameFor(#test_method_name),      \
            context.makeFixture(),                          \
            #test_method_name)))

#define CPPUNIT_JNI_TEST_SUITE_REGISTRATION(ATestFixtureType)   \
    CPPUNIT_TEST_SUITE_REGISTRATION(ATestFixtureType);          \
    static CPPUNIT_NS::AutoRegisterJavaClass<ATestFixtureType>  \
    CPPUNIT_MAKE_UNIQUE_NAME(autoRegisterJavaClass__)
