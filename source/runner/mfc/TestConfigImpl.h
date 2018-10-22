﻿#pragma once

#include "TestConfig.h"

class TestConfigImpl : public TestConfig
{
public:
	TestConfigImpl();

	virtual BOOL Load();
	virtual LPCTSTR Title();

protected:
	static void LoadFailedMsgBox(LPCTSTR libName);

protected:
	CString m_title;
};
