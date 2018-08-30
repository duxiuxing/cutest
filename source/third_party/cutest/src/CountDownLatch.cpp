#pragma once

#include "cutest/CountDownLatch.h"
#include "CountDownLatchImpl.h"

CUTEST_NS_BEGIN

CountDownLatch::CountDownLatch(int count) {
    m_impl = new CountDownLatchImpl(count);
}

CountDownLatch::~CountDownLatch() {
    delete m_impl;
}

void
CountDownLatch::await() {
    m_impl->await();
}

bool
CountDownLatch::await(unsigned int timeout_ms) {
    return m_impl->await(timeout_ms);
}

void
CountDownLatch::countDown() {
    m_impl->countDown();
}

int
CountDownLatch::getCount() {
    return m_impl->getCount();
}

CUTEST_NS_END
