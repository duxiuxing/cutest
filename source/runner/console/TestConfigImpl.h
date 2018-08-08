#pragma once

#include "TestConfig.h"

class TestConfigImpl : public TestConfig
{
public:
	TestConfigImpl();

	virtual BOOL Load();
	virtual LPCTSTR GetTitle();

protected:
	static BOOL IsFileExist(CString& filePath);
	static void loadFailedMsgBox(LPTSTR libName);

protected:
	CString m_title;
};
