#pragma once

#define TEST_CONFIG_FILE _T("test_config.xml")

class TestConfig
{
public:
	static TestConfig* GetInstance();

	virtual BOOL Load() PURE;
	virtual LPCTSTR GetTitle() PURE;
};
