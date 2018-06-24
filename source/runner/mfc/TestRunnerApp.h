#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CTestRunnerApp : public CWinApp
{
public:
	CTestRunnerApp();

	// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestRunnerApp theApp;
