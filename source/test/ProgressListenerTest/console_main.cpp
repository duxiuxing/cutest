#include "stdafx.h"

#include "EventRecordingListener.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cutest/Runner.h>
#include "gtest/gtest.h"

void VerifyResults( const std::vector<std::string> &data,
                    const char *const *expected_data,
                    size_t expected_data_size )
{
  const size_t actual_size = data.size();
  // If the following assertion fails, a new entry will be appended to
  // data.  Hence we save data.size() first.
  EXPECT_EQ( expected_data_size, actual_size );

  // Compares the common prefix.
  const size_t shorter_size = expected_data_size <= actual_size ?
                              expected_data_size : actual_size;
  size_t i = 0;
  for ( ; i < shorter_size; ++i )
  {
    ASSERT_STREQ( expected_data[i], data[i].c_str() )
        << "at position " << i;
  }

  // Prints extra elements in the actual data.
  for ( ; i < actual_size; ++i )
  {
    printf( "  Actual event #%lu: %s\n",
            static_cast<unsigned long>( i ), data[i].c_str() );
  }
}

std::vector<std::string> *g_events = NULL;
int g_indent = 0;

class EnvironmentInvocationCatcher : public testing::Environment
{
protected:
  virtual void SetUp()
  {
    ++g_indent;
    g_events->push_back( GetFullMethodName( "Environment::SetUp" ) );
  }

  virtual void TearDown()
  {
    g_events->push_back( GetFullMethodName( "Environment::TearDown" ) );
    --g_indent;
  }

  std::string GetFullMethodName( const char *name )
  {
    return GetIndentString() + name;
  }
};

int _tmain( int argc, _TCHAR *argv[] )
{
  ::CoInitialize( NULL );
  std::vector<std::string> events;
  g_events = &events;

  EventRecordingListener listener1( "1st" );
  CUTEST_NS::Runner::instance()->addListener( &listener1 );

  EventRecordingListener listener2( "2nd" );
  CUTEST_NS::Runner::instance()->addListener( &listener2 );

  AddGlobalTestEnvironment( new EnvironmentInvocationCatcher );

  GTEST_CHECK_( events.size() == 0 )
      << "AddGlobalTestEnvironment should not generate any events itself.";

  CUTEST_NS::Runner::instance()->start(
    CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest()
  );

  CUTEST_NS::Runner::instance()->waitUntilAllTestEnd();

  std::vector<std::string>::iterator it = events.begin();
  while ( it != events.end() )
  {
    std::string str = "\"";
    str.append( *it );
    str.append( "\",\n" );
    ::OutputDebugStringA( str.c_str() );
    ++it;
  }

  const char *const expected_events[] =
  {
    "--1st.All Tests.onRunnerStart",
    "----2nd.All Tests.onRunnerStart",
    "------Environment::SetUp",
    "--------1st.All Tests.onSuiteStart",
    "----------2nd.All Tests.onSuiteStart",
    "------------ListenerTest::SetUpTestCase",
    "--------------1st.ListenerTest.onSuiteStart",
    "----------------2nd.ListenerTest.onSuiteStart",
    "------------------1st.ListenerTest::DoesFoo.onTestStart",
    "--------------------2nd.ListenerTest::DoesFoo.onTestStart",
    "----------------------ListenerTest::SetUp",
    "------------------------ListenerTest::DoesFoo",
    "----------------------ListenerTest::TearDown",
    "--------------------2nd.ListenerTest::DoesFoo.onTestEnd",
    "------------------1st.ListenerTest::DoesFoo.onTestEnd",
    "------------------1st.ListenerTest::DoesBar.onTestStart",
    "--------------------2nd.ListenerTest::DoesBar.onTestStart",
    "----------------------ListenerTest::SetUp",
    "------------------------ListenerTest::DoesBar",
    "----------------------ListenerTest::TearDown",
    "--------------------2nd.ListenerTest::DoesBar.onTestEnd",
    "------------------1st.ListenerTest::DoesBar.onTestEnd",
    "----------------2nd.ListenerTest.onSuiteEnd",
    "--------------1st.ListenerTest.onSuiteEnd",
    "------------ListenerTest::TearDownTestCase",
    "----------2nd.All Tests.onSuiteEnd",
    "--------1st.All Tests.onSuiteEnd",
    "------Environment::TearDown",
    "----2nd.All Tests.onRunnerEnd",
    "--1st.All Tests.onRunnerEnd"
  };

  VerifyResults( events,
                 expected_events,
                 sizeof( expected_events ) / sizeof( expected_events[0] ) );
  return 0;
}
