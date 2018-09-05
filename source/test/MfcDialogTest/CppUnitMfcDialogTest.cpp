#include "stdafx.h"

#include <cppunit/cppunit.h>
#include "AboutDlg.h"

class CppUnitMfcDialogTest
  : public CPPUNIT_NS::MfcDialogTestFixture
{
  CPPUNIT_TEST_SUITE( CppUnitMfcDialogTest );
  {
    CPPUNIT_MFC_DIALOG_TEST( AboutDlg, CAboutDlg );
    CPPUNIT_MFC_DIALOG_TEST_WITH_TIMEOUT( AboutDlg2, CAboutDlg, 2000 );
  }
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( CppUnitMfcDialogTest );
