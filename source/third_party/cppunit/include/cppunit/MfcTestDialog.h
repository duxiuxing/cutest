#pragma once

#include <cppunit/ExplicitEndTest.h>
#include "MainTestRunner.h"

template<class T>
class CMfcTestDialog
  : public CPPUNIT_NS::ExplicitEndTest
  , public T
{
public:
  CMfcTestDialog( unsigned int timeout_ms = 0 );

  // 重载ExplicitEndTest
  virtual void EndTest() override;

  virtual void PostNcDestroy() override;
  virtual void OnOK() override;
  virtual void OnCancel() override;
};

template<class T>
CMfcTestDialog<T>::CMfcTestDialog( unsigned int timeout_ms )
  : CPPUNIT_NS::ExplicitEndTest( timeout_ms )
{
}

template<class T>
void
CMfcTestDialog<T>::PostNcDestroy()
{
  T::PostNcDestroy();

  if ( _event )
  {
    _event->post();
    _event = NULL;
  }

  delete this;
}

template<class T>
void
CMfcTestDialog<T>::OnOK()
{
  T::OnOK();
  DestroyWindow();
}

template<class T>
void
CMfcTestDialog<T>::OnCancel()
{
  T::OnCancel();
  DestroyWindow();
}

template<class T>
void
CMfcTestDialog<T>::EndTest()
{
  if ( _event )
  {
    _event->post();
    _event = NULL;
  }

  PostMessage( WM_CLOSE );
}
