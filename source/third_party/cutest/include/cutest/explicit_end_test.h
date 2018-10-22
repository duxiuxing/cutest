﻿#pragma once

#include "cutest/define.h"
#include "gtest/internal/gtest-port.h"

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4275 \
                                 /* class A needs to have dll-interface to be used by clients of class B */)

CUTEST_NS_BEGIN

class Event;

class GTEST_API_ ExplicitEndTest {
public:
    ExplicitEndTest();

public:
    void SetEvent(CUTEST_NS::Event* event);
    virtual void EndTest();
    bool IsTestEnd();

protected:
    Event* m_event;

private:
    ExplicitEndTest(const ExplicitEndTest& other);
    ExplicitEndTest& operator=(const ExplicitEndTest& other);
};

CUTEST_NS_END

GTEST_DISABLE_MSC_WARNINGS_POP_() //  4275
