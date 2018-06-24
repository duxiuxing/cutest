#include "stdafx.h"
#include "TestRunnerApp.h"
#include "TestRunnerDlg.h"

#include <cppunit/extensions/TestFactoryRegistry.h>

#include "TestConfig.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

// CTestRunnerApp
BEGIN_MESSAGE_MAP(CTestRunnerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CTestRunnerApp construction
CTestRunnerApp::CTestRunnerApp()
{
	// support Restart Manager
#if _MSC_VER >= 1500
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#endif
}

CTestRunnerApp theApp;

// CTestRunnerApp initialization
BOOL CTestRunnerApp::InitInstance()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(4022);
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

	VERIFY(TestConfig::GetInstance()->Load());

	// HKEY_CURRENT_USER\Software\title
	SetRegistryKey(TestConfig::GetInstance()->GetTitle());

	CPPUNIT_NS::Test* rootTest = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
	CTestRunnerDlg dlg(rootTest);
	m_pMainWnd = &dlg;
	dlg.DoModal();
	delete rootTest;
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