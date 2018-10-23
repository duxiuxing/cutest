#pragma once

#define TEST_CONFIG_FILE _T("test_config.xml")

class TestConfig
{
public:
	static TestConfig* Instance();

	virtual BOOL Load() PURE;
	virtual LPCTSTR Title() PURE;
};
