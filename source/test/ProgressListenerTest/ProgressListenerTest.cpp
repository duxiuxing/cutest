#include "gtest/gtest.h"

#include "EventRecordingListener.h"

extern std::vector<std::string>* g_events;
extern int g_indent;

class ListenerTest : public testing::ExplicitEndTest {
 protected:
  static void SetUpTestCase() {
    ++g_indent;
    g_events->push_back(GetFullMethodName("ListenerTest::SetUpTestCase"));
  }

  static void TearDownTestCase() {
    g_events->push_back(GetFullMethodName("ListenerTest::TearDownTestCase"));
    --g_indent;
  }

  virtual void SetUp() {
    ++g_indent;
    g_events->push_back(GetFullMethodName("ListenerTest::SetUp"));
  }

  virtual void TearDown() {
    g_events->push_back(GetFullMethodName("ListenerTest::TearDown"));
    --g_indent;
  }

 protected:
  static std::string GetFullMethodName(const char* name) {
    return GetIndentString() + name;
  }
};

TEST_F(ListenerTest, DoesFoo) {
  ++g_indent;
  g_events->push_back(GetFullMethodName("ListenerTest.DoesFoo"));
  --g_indent;
  //SUCCEED();  // Triggers OnTestPartResult.
}

EXPLICIT_END_TEST_WITH_TIMEOUT_F(ListenerTest, DoesBar, 1000) {
  ++g_indent;
  g_events->push_back(GetFullMethodName("ListenerTest.DoesBar"));
  --g_indent;
  //SUCCEED();  // Triggers OnTestPartResult.
}
