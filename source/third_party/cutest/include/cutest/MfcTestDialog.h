#pragma once

#include "ExplicitEndTest.h"
#include "Event.h"

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
	ExplicitEndTest* test;
};

template<class T>
CMfcTestDialog<T>::CMfcTestDialog(ExplicitEndTest* test_in)
 : T()
, test(test_in) {
}

template<class T>
void
CMfcTestDialog<T>::PostNcDestroy() {
    T::PostNcDestroy();

    if (this->test) {
		this->test->endTest();
		this->test = NULL;
    }

    delete this;
}

template<class T>
void
CMfcTestDialog<T>::OnOK() {
    T::OnOK();
    T::DestroyWindow();
}

template<class T>
void
CMfcTestDialog<T>::OnCancel() {
    T::OnCancel();
	T::DestroyWindow();
}

CUTEST_NS_END
