#pragma once

#include "../count_down_latch_impl.h"

#include "cutest/helper.h"
#include "cutest/runner.h"

#include <Windows.h>

CUTEST_NS_BEGIN

CountDownLatchImpl::CountDownLatchImpl(int count)
    : m_count(count)
    , m_event(NULL) {}

CountDownLatchImpl::~CountDownLatchImpl() {
    if (m_event) {
        m_event->Destroy();
        m_event = NULL;
    }
}

void CountDownLatchImpl::Await() {
    if (CUTEST_NS::IsOnMainThread()) {
        AwaitOnMainThread();
    } else {
        AwaitOnWorkerThread();
    }
}

bool CountDownLatchImpl::Await(unsigned int msTimeout) {
    if (CUTEST_NS::IsOnMainThread()) {
        return AwaitOnMainThread(msTimeout);
    } else {
        return AwaitOnWorkerThread(msTimeout);
    }
}

void CountDownLatchImpl::CountDown() {
    if (InterlockedDecrement(reinterpret_cast<volatile LONG*>(&m_count)) <= 0
        && m_event) {
        m_event->Post();
    }
}

int CountDownLatchImpl::Count() {
    return m_count;
}

void CountDownLatchImpl::AwaitOnMainThread() {
    MSG msg = {0};
    while (m_count > 0
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

bool CountDownLatchImpl::AwaitOnMainThread(unsigned int msTimeout) {
    unsigned long long start = CUTEST_NS::TickCount64();

    MSG msg = {0};
    while (m_count > 0
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if ((CUTEST_NS::TickCount64() - start) >= msTimeout) {
            return false;
        }
    }

    return true;
}

void CountDownLatchImpl::AwaitOnWorkerThread() {
    m_event = Event::CreateInstance();
    m_event->Wait();
}

bool CountDownLatchImpl::AwaitOnWorkerThread(unsigned int msTimeout) {
    m_event = Event::CreateInstance();
    m_event->Wait(msTimeout);
    return (m_count <= 0);
}

CUTEST_NS_END
