#include "SimpleTimer.h"

SimpleTimer* SimpleTimer::instance() {
    static SimpleTimer simple_timer;
    return &simple_timer;
}

SimpleTimer::SimpleTimer()
    : callback(NULL) {}

void SimpleTimer::setCallback(unsigned int elapse_ms, Callback* callback) {
    this->callback = callback;

    CUTEST_NS::Runner::instance()->delayRunOnMainThread(elapse_ms, this, false);
}

void SimpleTimer::removeCallback(Callback* callback) {
    this->callback = NULL;
}

void SimpleTimer::run() {
    if (this->callback) {
        this->callback->onTimeUp();
        this->callback = NULL;
    }
}
