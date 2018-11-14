#include "cutest/explicit_end_test.h"

#include "cutest/event.h"
#include "cutest/runner.h"

CUTEST_NS_BEGIN

ExplicitEndTest::ExplicitEndTest()
    : m_event(NULL) {}

void ExplicitEndTest::SetEvent(CUTEST_NS::Event* event) {
    m_event = event;
}

void ExplicitEndTest::EndTest() {
    if (IsTestEnd()) {
        return;  // 防止重入，CUTEST_NS::Runner::Stop()/超时/延迟/异步调用逻辑都可能调用EndTest()
    }

    CUTEST_NS::Runner::Instance()->UnregisterExplicitEndTest(this);
    if (m_event) {
        m_event->Post();
        m_event = NULL;
    }
}

bool ExplicitEndTest::IsTestEnd() {
    return (m_event == NULL);
}

CUTEST_NS_END
