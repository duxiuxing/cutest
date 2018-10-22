#include "simple_timer.h"

SimpleTimer* SimpleTimer::Instance() {
    static SimpleTimer s_simpleTimer;
    return &s_simpleTimer;
}

SimpleTimer::SimpleTimer()
    : m_callback(NULL) {}

void SimpleTimer::SetCallback(unsigned int msElapse, Callback* callback) {
    m_callback = callback;

    CUTEST_NS::Runner::Instance()->DelayRunOnMainThread(msElapse, this, false);
}

void SimpleTimer::RemoveCallback(Callback* callback) {
    m_callback = NULL;
}

void SimpleTimer::Run() {
    if (m_callback) {
        m_callback->OnTimeUp();
        m_callback = NULL;
    }
}
