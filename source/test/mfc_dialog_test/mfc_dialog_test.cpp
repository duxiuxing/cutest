#include "stdafx.h"

#include <cppunit/cppunit.h>
#include "about_dlg.h"

class MfcDialogTest
	: public CPPUNIT_NS::MfcDialogTestFixture
{
	CPPUNIT_TEST_SUITE(MfcDialogTest);
	{
		CPPUNIT_MFC_DIALOG_TEST(SimpleDialog, CAboutDlg);
		CPPUNIT_MFC_DIALOG_TEST_WITH_TIMEOUT(AutoCloseDialog, CAboutDlg, 2000);
	}
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MfcDialogTest);
