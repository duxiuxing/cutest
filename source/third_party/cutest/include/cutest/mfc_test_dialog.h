#pragma once

#include "explicit_end_test.h"
#include "event.h"

CUTEST_NS_BEGIN

template<class T>
class CMfcTestDialog
    : public T {
public:
    CMfcTestDialog(ExplicitEndTest* test);

    virtual void PostNcDestroy() override;
    virtual void OnOK() override;
    virtual void OnCancel() override;

protected:
    ExplicitEndTest* m_test;
};

template<class T>
CMfcTestDialog<T>::CMfcTestDialog(ExplicitEndTest* test)
    : T()
    , m_test(test) {
}

template<class T>
void CMfcTestDialog<T>::PostNcDestroy() {
    T::PostNcDestroy();

    if (m_test) {
        m_test->EndTest();
        m_test = NULL;
    }

    delete this;
}

template<class T>
void CMfcTestDialog<T>::OnOK() {
    T::OnOK();
    T::DestroyWindow();
}

template<class T>
void CMfcTestDialog<T>::OnCancel() {
    T::OnCancel();
    T::DestroyWindow();
}

CUTEST_NS_END
