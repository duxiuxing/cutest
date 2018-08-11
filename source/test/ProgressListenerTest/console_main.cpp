#include "stdafx.h"

#include "EventRecordingListener.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cutest/Runner.h>
#include "gtest/gtest.h"

/*
  void VerifyResults(const std::vector<std::string>& data,
                   const char* const* expected_data,
                   size_t expected_data_size) {
                       const size_t actual_size = data.size();
                       // If the following assertion fails, a new entry will be appended to
                       // data.  Hence we save data.size() first.
                       EXPECT_EQ(expected_data_size, actual_size);

                       // Compares the common prefix.
                       const size_t shorter_size = expected_data_size <= actual_size ?
  expected_data_size : actual_size;
                       size_t i = 0;
                       for (; i < shorter_size; ++i) {
                           ASSERT_STREQ(expected_data[i], data[i].c_str())
                               << "at position " << i;
                       }

                       // Prints extra elements in the actual data.
                       for (; i < actual_size; ++i) {
                           printf("  Actual event #%lu: %s\n",
                               static_cast<unsigned long>(i), data[i].c_str());
                       }
  }



  int main(int argc, char **argv) {
  std::vector<std::string> events;
  g_events = &events;
  InitGoogleTest(&argc, argv);

  UnitTest::GetInstance()->listeners().Append(
      new EventRecordingListener("1st"));
  UnitTest::GetInstance()->listeners().Append(
      new EventRecordingListener("2nd"));

  AddGlobalTestEnvironment(new EnvironmentInvocationCatcher);

  GTEST_CHECK_(events.size() == 0)
      << "AddGlobalTestEnvironment should not generate any events itself.";

  ::testing::GTEST_FLAG(repeat) = 2;
  int ret_val = RUN_ALL_TESTS();

  const char* const expected_events[] = {
    "1st.OnTestProgramStart",
    "2nd.OnTestProgramStart",
    "1st.OnTestIterationStart(0)",
    "2nd.OnTestIterationStart(0)",
    "1st.OnEnvironmentsSetUpStart",
    "2nd.OnEnvironmentsSetUpStart",
    "Environment::SetUp",
    "2nd.OnEnvironmentsSetUpEnd",
    "1st.OnEnvironmentsSetUpEnd",
    "1st.OnTestCaseStart",
    "2nd.OnTestCaseStart",
    "ListenerTest::SetUpTestCase",
    "1st.OnTestStart",
    "2nd.OnTestStart",
    "ListenerTest::SetUp",
    "ListenerTest::* Test Body",
    "1st.OnTestPartResult",
    "2nd.OnTestPartResult",
    "ListenerTest::TearDown",
    "2nd.OnTestEnd",
    "1st.OnTestEnd",
    "1st.OnTestStart",
    "2nd.OnTestStart",
    "ListenerTest::SetUp",
    "ListenerTest::* Test Body",
    "1st.OnTestPartResult",
    "2nd.OnTestPartResult",
    "ListenerTest::TearDown",
    "2nd.OnTestEnd",
    "1st.OnTestEnd",
    "ListenerTest::TearDownTestCase",
    "2nd.OnTestCaseEnd",
    "1st.OnTestCaseEnd",
    "1st.OnEnvironmentsTearDownStart",
    "2nd.OnEnvironmentsTearDownStart",
    "Environment::TearDown",
    "2nd.OnEnvironmentsTearDownEnd",
    "1st.OnEnvironmentsTearDownEnd",
    "2nd.OnTestIterationEnd(0)",
    "1st.OnTestIterationEnd(0)",
    "1st.OnTestIterationStart(1)",
    "2nd.OnTestIterationStart(1)",
    "1st.OnEnvironmentsSetUpStart",
    "2nd.OnEnvironmentsSetUpStart",
    "Environment::SetUp",
    "2nd.OnEnvironmentsSetUpEnd",
    "1st.OnEnvironmentsSetUpEnd",
    "1st.OnTestCaseStart",
    "2nd.OnTestCaseStart",
    "ListenerTest::SetUpTestCase",
    "1st.OnTestStart",
    "2nd.OnTestStart",
    "ListenerTest::SetUp",
    "ListenerTest::* Test Body",
    "1st.OnTestPartResult",
    "2nd.OnTestPartResult",
    "ListenerTest::TearDown",
    "2nd.OnTestEnd",
    "1st.OnTestEnd",
    "1st.OnTestStart",
    "2nd.OnTestStart",
    "ListenerTest::SetUp",
    "ListenerTest::* Test Body",
    "1st.OnTestPartResult",
    "2nd.OnTestPartResult",
    "ListenerTest::TearDown",
    "2nd.OnTestEnd",
    "1st.OnTestEnd",
    "ListenerTest::TearDownTestCase",
    "2nd.OnTestCaseEnd",
    "1st.OnTestCaseEnd",
    "1st.OnEnvironmentsTearDownStart",
    "2nd.OnEnvironmentsTearDownStart",
    "Environment::TearDown",
    "2nd.OnEnvironmentsTearDownEnd",
    "1st.OnEnvironmentsTearDownEnd",
    "2nd.OnTestIterationEnd(1)",
    "1st.OnTestIterationEnd(1)",
    "2nd.OnTestProgramEnd",
    "1st.OnTestProgramEnd"
  };
  VerifyResults(events,
                expected_events,
                sizeof(expected_events)/sizeof(expected_events[0]));

  // We need to check manually for ad hoc test failures that happen after
  // RUN_ALL_TESTS finishes.
  if (UnitTest::GetInstance()->Failed())
    ret_val = 1;

  return ret_val;
  }*/

std::vector<std::string> *g_events = NULL;

class EnvironmentInvocationCatcher : public testing::Environment
{
protected:
  virtual void SetUp()
  {
    g_events->push_back( "Environment::SetUp" );
  }

  virtual void TearDown()
  {
    g_events->push_back( "Environment::TearDown" );
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

  return 0;
}
