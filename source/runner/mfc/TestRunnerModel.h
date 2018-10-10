#pragma once

#include <deque>
#include <cppunit/Test.h>

class Settings
{
public:
	Settings()
		: AutorunOnStartup(false)
		, AlwaysCallTestOnMainThread(false)
		, TreatTimeoutAsError(false)
		, TypeColumnWidth(0)
		, NameColumnWidth(0)
		, FailedConditionColumnWidth(0)
		, LineNumberColumnWidth(0)
	{}

	bool AutorunOnStartup;
	bool AlwaysCallTestOnMainThread;
	bool TreatTimeoutAsError;

	int TypeColumnWidth;
	int NameColumnWidth;
	int FailedConditionColumnWidth;
	int LineNumberColumnWidth;
};

class TestHistory : public std::deque<CPPUNIT_NS::Test*>
{
public:
	bool SetCurSel(int index);
	void SetCurSel(CPPUNIT_NS::Test* test);
	CPPUNIT_NS::Test* GetCurSel() const;

	CString GetEntryName(int index) const;
	CString GetTestName(int index);

public:
	void Save();
private:
	void SaveTestName(int index, CString testName);
};

class TestRunnerModel : public Settings
{
	// Prevents the use of the copy constructor.
	TestRunnerModel(const TestRunnerModel& copy);
	/// Prevents the use of the copy operator.
	TestRunnerModel& operator =(const TestRunnerModel& copy);

public:
	// HKEY_CURRENT_USER\Software\<TestConfig::GetInstance()->GetTitle()>\MfcTestRunnerApp\<PROFILE_SECTION>
	static LPCTSTR PROFILE_SECTION;

	TestRunnerModel(CPPUNIT_NS::Test* rootTest);

	CPPUNIT_NS::Test* GetRootTest();

private:
	CPPUNIT_NS::Test* m_rootTest;


	// Settings相关的
public:
	// PROFILE_SECTION下Settings相关的Entry名称定义
	static LPCTSTR ENTRY_AUTORUN_AT_STARTUP;
	static LPCTSTR ENTRY_ALWAYS_CALL_TEST_ON_MAIN_THREAD;
	static LPCTSTR ENTRY_TREAT_TIMEOUT_AS_ERROR;
	static LPCTSTR ENTRY_TYPE_COLUMN_WIDTH;
	static LPCTSTR ENTRY_NAME_COLUMN_WIDTH;
	static LPCTSTR ENTRY_FAILED_CONDITION_COLUMN_WIDTH;
	static LPCTSTR ENTRY_LINE_NUMBER_COLUMN_WIDTH;

	void SaveSettings();

private:
	void LoadSettings();


	// TestHistory相关的
public:
	const TestHistory& GetTestHistory() const;

	bool SelectTest(int index);
	void SelectTest(CPPUNIT_NS::Test* test);
	CPPUNIT_NS::Test* GetSelectedTest() const;

	void SaveTestHistory();

private:
	void LoadTestHistory();
	static std::string ToAnsiString(const CString& text);

	TestHistory m_testHistory;
};
