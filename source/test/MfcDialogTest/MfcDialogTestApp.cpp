#include "stdafx.h"
#include "MfcDialogTestApp.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMfcDialogTestApp, CWinApp)
END_MESSAGE_MAP()

CMfcDialogTestApp::CMfcDialogTestApp()
{}

CMfcDialogTestApp theApp;

BOOL CMfcDialogTestApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
