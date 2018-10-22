#pragma once

#include "cutest/count_down_latch.h"
#include "count_down_latch_impl.h"

CUTEST_NS_BEGIN

CountDownLatch::CountDownLatch(int count) {
    this->impl = new CountDownLatchImpl(count);
}

CountDownLatch::~CountDownLatch() {
    delete this->impl;
}

void
CountDownLatch::await() {
    this->impl->await();
}

bool
CountDownLatch::await(unsigned int msTimeout) {
    return this->impl->await(msTimeout);
}

void
CountDownLatch::countDown() {
    this->impl->countDown();
}

int
CountDownLatch::getCount() {
    return this->impl->getCount();
}

CUTEST_NS_END
