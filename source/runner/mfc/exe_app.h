#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CExeApp : public CWinApp
{
public:
	CExeApp();

	// Overrides
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;

	// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CExeApp theApp;
