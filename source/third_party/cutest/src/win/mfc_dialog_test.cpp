#include "cutest/mfc_dialog_test.h"

CUTEST_NS_BEGIN

MfcDialogTest::MfcDialogTest()
    : m_dialog(NULL) {}

void MfcDialogTest::EndTest() {
    if (m_dialog) {
        HWND wnd = m_dialog;
        Detach();
        ::DestroyWindow(wnd);
    }

    ExplicitEndTest::EndTest();
}

void MfcDialogTest::Attach(HWND dialog) {
    m_dialog = dialog;
}

HWND MfcDialogTest::Detach() {
    HWND ret = m_dialog;
    m_dialog = NULL;
    return ret;
}

CUTEST_NS_END
