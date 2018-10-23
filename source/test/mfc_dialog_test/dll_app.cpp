#include "stdafx.h"
#include "dll_app.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDllApp, CWinApp)
END_MESSAGE_MAP()

CDllApp::CDllApp()
{}

CDllApp theApp;

BOOL CDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
