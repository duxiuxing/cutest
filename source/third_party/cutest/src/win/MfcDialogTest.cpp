#include "cutest/MfcDialogTest.h"

CUTEST_NS_BEGIN

MfcDialogTest::MfcDialogTest()
    : dialog(NULL) {}

void
MfcDialogTest::endTest() {
    if (this->dialog) {
        HWND wnd = this->dialog;
        detach();
        ::DestroyWindow(wnd);
    }

    ExplicitEndTest::endTest();
}

void
MfcDialogTest::attach(HWND dialog) {
    this->dialog = dialog;
}

HWND
MfcDialogTest::detach() {
    HWND ret = this->dialog;
    this->dialog = NULL;
    return ret;
}

CUTEST_NS_END
