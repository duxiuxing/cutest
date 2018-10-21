#pragma once

#include "explicit_end_test.h"
#include <wtypes.h>

GTEST_DISABLE_MSC_WARNINGS_PUSH_(4275 \
                                 /* class A needs to have dll-interface to be used by clients of class B */)

CUTEST_NS_BEGIN

class GTEST_API_ MfcDialogTest : public ExplicitEndTest {
public:
    MfcDialogTest();

    virtual void endTest() override;

    void attach(HWND dialog);
    HWND detach();

protected:
    HWND dialog;

private:
    MfcDialogTest(const MfcDialogTest& other);
    MfcDialogTest& operator=(const MfcDialogTest& other);
};

CUTEST_NS_END

GTEST_DISABLE_MSC_WARNINGS_POP_() //  4275
