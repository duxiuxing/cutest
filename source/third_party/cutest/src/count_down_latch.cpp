#pragma once

#include "cutest/count_down_latch.h"
#include "count_down_latch_impl.h"

CUTEST_NS_BEGIN

CountDownLatch::CountDownLatch(int count) {
    m_impl = new CountDownLatchImpl(count);
}

CountDownLatch::~CountDownLatch() {
    delete m_impl;
}

void CountDownLatch::Await() {
    m_impl->Await();
}

bool CountDownLatch::Await(unsigned int msTimeout) {
    return m_impl->Await(msTimeout);
}

void CountDownLatch::CountDown() {
    m_impl->CountDown();
}

int CountDownLatch::Count() {
    return m_impl->Count();
}

CUTEST_NS_END
