#include "gtest/gtest.h"

extern std::vector<std::string> *g_events;

class ListenerTest : public testing::ExplicitEndTest
{
protected:
  static void SetUpTestCase()
  {
    g_events->push_back( "ListenerTest::SetUpTestCase" );
  }

  static void TearDownTestCase()
  {
    g_events->push_back( "ListenerTest::TearDownTestCase" );
  }

  virtual void SetUp()
  {
    g_events->push_back( "ListenerTest::SetUp" );
  }

  virtual void TearDown()
  {
    g_events->push_back( "ListenerTest::TearDown" );
  }
};

TEST_F( ListenerTest, DoesFoo )
{
  // Test execution order within a test case is not guaranteed so we are not
  // recording the test name.
  g_events->push_back( "ListenerTest::DoesFoo" );
  //SUCCEED();  // Triggers OnTestPartResult.
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F( ListenerTest, DoesBar, 1000 )
{
  g_events->push_back( "ListenerTest::DoesBar" );
  //SUCCEED();  // Triggers OnTestPartResult.
}
