#include "cutest/listener.h"

std::string GetIndentString();

class EventRecordingListener : public CUTEST_NS::Listener {
public:
    EventRecordingListener(const char* prefix);

protected:
    virtual void OnRunnerStart(CPPUNIT_NS::Test* test) override;
    virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) override;

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

    virtual void OnSuiteStart(CPPUNIT_NS::Test* suite) override;
    virtual void OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) override;

    virtual void OnTestStart(CPPUNIT_NS::Test* test) override;

    virtual void OnTestEnd(
        CPPUNIT_NS::Test* test,
        unsigned int errorCount,
        unsigned int failureCount,
        unsigned int msElapsed) override;

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
    std::string GetFullMethodName(CPPUNIT_NS::Test* test, const char* name);

    std::string m_prefix;
};
