#include "stdafx.h"
#include "TestRunnerApp.h"
#include "TestRunnerDlg.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include "cutest/runner.h"

#include "TestConfig.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestRunnerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestRunnerApp::CTestRunnerApp()
{
	// support Restart Manager
#if _MSC_VER >= 1500
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#endif
}

CTestRunnerApp theApp;

BOOL CTestRunnerApp::InitInstance()
{
	// _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(4022);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	::CoInitialize(NULL);

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
#if _MSC_VER >= 1500
	CShellManager* pShellManager = new CShellManager;
#endif

	VERIFY(TestConfig::Instance()->Load());

	// HKEY_CURRENT_USER\Software\title
	SetRegistryKey(TestConfig::Instance()->Title());

	CPPUNIT_NS::Test* allTests = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
	CTestRunnerDlg dlg(allTests);
	m_pMainWnd = &dlg;
	dlg.DoModal();
	CUTEST_NS::Runner::Instance()->WaitUntilAllTestEnd();
	delete allTests;
#if _MSC_VER >= 1500
	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
#endif

	return FALSE;
}

int CTestRunnerApp::ExitInstance()
{
	::CoUninitialize();
	return CWinApp::ExitInstance();
}
