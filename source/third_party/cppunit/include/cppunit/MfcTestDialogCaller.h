#pragma once

#include "MfcTestDialog.h"

#include "cutest/Event.h"
#include "cutest/Runnable.h"
#include "cutest/Runner.h"

CPPUNIT_NS_BEGIN

template<class T>
class MfcTestDialogCaller
  : public TestCase
  , public Runnable
{
public:
  typedef void ( T::*TEST_METHOD )();

  MfcTestDialogCaller( std::string name, TEST_METHOD test_method )
    : TestCase( name )
    , _event( NULL )
    , _testMethod( test_method )
  {}

  // 重载TestCase::runTest()
  virtual void runTest()
  {
    _event = CUTEST_NS::Event::createInstance();
    CUTEST_NS::Runner::instance()->asyncRunOnMainThread( this, false );
    _event->wait();
    _event->destroy();
    _event = NULL;
  }

  // 实现Runnable::run()
  virtual void run()
  {
    AFX_MANAGE_STATE( AfxGetStaticModuleState() );
    T *testDlg = new T;
    testDlg->SetEvent( _event );
    testDlg->Create( T::IDD );
    testDlg->ShowWindow( SW_NORMAL );
    ( testDlg->*_testMethod )();
  }

private:
  MfcTestDialogCaller( const MfcTestDialogCaller & );
  MfcTestDialogCaller &operator =( const MfcTestDialogCaller & );

  CUTEST_NS::Event *_event;
  TEST_METHOD _testMethod;
};

CPPUNIT_NS_END

#define CPPUNIT_TEST_WITH_MFC_DIALOG(dialog_class, test_method) \
    CPPUNIT_TEST_SUITE_ADD_TEST( \
        (new CPPUNIT_NS::MfcTestDialogCaller<CMfcTestDialog<dialog_class> >( \
            context.getTestNameFor(#test_method), \
            &CMfcTestDialog<dialog_class>::test_method)))

#if CPPUNIT_ENABLE_CU_TEST_MACROS
#define CU_TEST_WITH_MFC_DIALOG(tc, tm) CPPUNIT_TEST_WITH_MFC_DIALOG(tc, tm)
#endif
