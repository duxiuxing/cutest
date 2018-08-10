#include "cutest/ProgressListener.h"
#include <vector>

extern std::vector<std::string> *g_events;

class EventRecordingListener : public CUTEST_NS::ProgressListener
{
public:
  explicit EventRecordingListener( const char *name ) : name_( name ) {}

protected:
  virtual void onRunnerStart( CPPUNIT_NS::Test *test ) override
  {
    g_events->push_back( GetFullMethodName( "onRunnerStart" ) );
  }

  virtual void onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms ) override
  {
    g_events->push_back( GetFullMethodName( "onRunnerEnd" ) );
  }

  //virtual void OnTestIterationStart(const UnitTest& /*unit_test*/,
  //                                  int iteration) {
  //  Message message;
  //  message << GetFullMethodName("OnTestIterationStart")
  //          << "(" << iteration << ")";
  //  g_events->push_back(message.GetString());
  //}

  //virtual void OnEnvironmentsSetUpStart(const UnitTest& /*unit_test*/) {
  //  g_events->push_back(GetFullMethodName("OnEnvironmentsSetUpStart"));
  //}

  //virtual void OnEnvironmentsSetUpEnd(const UnitTest& /*unit_test*/) {
  //  g_events->push_back(GetFullMethodName("OnEnvironmentsSetUpEnd"));
  //}

  virtual void onSuiteStart( CPPUNIT_NS::Test *suite ) override
  {
    g_events->push_back( GetFullMethodName( "onSuiteStart" ) );
  }

  virtual void onSuiteEnd( CPPUNIT_NS::Test *suite, unsigned int elapsed_ms ) override
  {
    g_events->push_back( GetFullMethodName( "onSuiteEnd" ) );
  }

  virtual void onTestStart( CPPUNIT_NS::Test *test ) override
  {
    g_events->push_back( GetFullMethodName( "onTestStart" ) );
  }

  virtual void onTestEnd(
    CPPUNIT_NS::Test *test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms ) override
  {
    g_events->push_back( GetFullMethodName( "onTestEnd" ) );
  }

  //virtual void OnTestPartResult(const TestPartResult& /*test_part_result*/) {
  //  g_events->push_back(GetFullMethodName("OnTestPartResult"));
  //}

  //virtual void OnEnvironmentsTearDownStart(const UnitTest& /*unit_test*/) {
  //  g_events->push_back(GetFullMethodName("OnEnvironmentsTearDownStart"));
  //}

  //virtual void OnEnvironmentsTearDownEnd(const UnitTest& /*unit_test*/) {
  //  g_events->push_back(GetFullMethodName("OnEnvironmentsTearDownEnd"));
  //}

  //virtual void OnTestIterationEnd(const UnitTest& /*unit_test*/,
  //                                int iteration) {
  //  Message message;
  //  message << GetFullMethodName("OnTestIterationEnd")
  //          << "("  << iteration << ")";
  //  g_events->push_back(message.GetString());
  //}

private:
  std::string GetFullMethodName( const char *name )
  {
    return name_ + "." + name;
  }

  std::string name_;
};
