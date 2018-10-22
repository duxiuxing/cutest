#pragma once

#include "../count_down_latch_impl.h"

#include "cutest/helper.h"
#include "cutest/runner.h"

#include <Windows.h>

CUTEST_NS_BEGIN

CountDownLatchImpl::CountDownLatchImpl(int countParam)
    : count(countParam)
    , event(NULL) {}

CountDownLatchImpl::~CountDownLatchImpl() {
    if (this->event) {
        this->event->destroy();
        this->event = NULL;
    }
}

void
CountDownLatchImpl::await() {
    if (CUTEST_NS::isOnMainThread()) {
        awaitOnMainThread();
    } else {
        awaitOnWorkerThread();
    }
}

bool
CountDownLatchImpl::await(unsigned int msTimeout) {
    if (CUTEST_NS::isOnMainThread()) {
        return awaitOnMainThread(msTimeout);
    } else {
        return awaitOnWorkerThread(msTimeout);
    }
}

void
CountDownLatchImpl::countDown() {
    if (InterlockedDecrement(reinterpret_cast<volatile LONG*>(&this->count)) <= 0
        && this->event) {
        this->event->post();
    }
}

int
CountDownLatchImpl::getCount() {
    return this->count;
}

void
CountDownLatchImpl::awaitOnMainThread() {
    MSG msg = {0};
    while (this->count > 0
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

bool
CountDownLatchImpl::awaitOnMainThread(unsigned int msTimeout) {
    unsigned long long start = CUTEST_NS::tickCount64();

    MSG msg = {0};
    while (this->count > 0
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if ((CUTEST_NS::tickCount64() - start) >= msTimeout) {
            return false;
        }
    }

    return true;
}

void
CountDownLatchImpl::awaitOnWorkerThread() {
    this->event = Event::createInstance();
    this->event->wait();
}

bool
CountDownLatchImpl::awaitOnWorkerThread(unsigned int msTimeout) {
    this->event = Event::createInstance();
    this->event->wait(msTimeout);
    return (this->count <= 0);
}

CUTEST_NS_END
