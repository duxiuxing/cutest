// //////////////////////////////////////////////////////////////////////////
// Implementation file TestRunnerModel.cpp for class TestRunnerModel
// (c)Copyright 2000, Baptiste Lepilleur.
// Created: 2001/04/26
// //////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TestRunnerModel.h"
#include <cppunit/tools/Algorithm.h>
#include <algorithm>
#include <stdexcept>
#include <cppunit/testsuite.h>
#include <set>

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

LPCTSTR TestRunnerModel::PROFILE_SECTION = _T("TestRunnerModel");

LPCTSTR TestRunnerModel::ENTRY_AUTORUN_AT_STARTUP = _T("AutorunAtStartup");
LPCTSTR TestRunnerModel::ENTRY_ALWAYS_CALL_TEST_ON_MAIN_THREAD = _T("AlwaysCallTestOnMainThread");
LPCTSTR TestRunnerModel::ENTRY_TREAT_TIMEOUT_AS_ERROR = _T("TreatTimeoutAsError");
LPCTSTR TestRunnerModel::ENTRY_TYPE_COLUMN_WIDTH = _T("TypeColumnWidth");
LPCTSTR TestRunnerModel::ENTRY_NAME_COLUMN_WIDTH = _T("NameColumnWidth");
LPCTSTR TestRunnerModel::ENTRY_FAILED_CONDITION_COLUMN_WIDTH = _T("FailedConditionColumnWidth");
LPCTSTR TestRunnerModel::ENTRY_LINE_NUMBER_COLUMN_WIDTH = _T("LineNumberColumnWidth");

bool
TestHistory::SetCurSel(int index)
{
	if (index < 0
		|| index >= (int)size())
	{
		return false;
	}

	SetCurSel(at(index));
	return true;
}

void
TestHistory::SetCurSel(CPPUNIT_NS::Test* test)
{
	CPPUNIT_NS::removeFromSequence(*this, test);

	if (test != NULL)
	{
		push_front(test);
	}
}

CPPUNIT_NS::Test*
TestHistory::GetCurSel() const
{
	if (size() > 0)
	{
		return at(0);
	}
	return NULL;
}

CString
TestHistory::GetTestName(int index)
{
	CWinApp* app = AfxGetApp();
	ASSERT(app != NULL);

	return app->GetProfileString(TestRunnerModel::PROFILE_SECTION, TestHistory::GetEntryName(index));
}

void
TestHistory::Save()
{
	int index = 1;
	for (TestHistory::const_iterator it = begin();
		 it != end();
		 ++it, ++index)
	{
		CPPUNIT_NS::Test* test = *it;
		SaveTestName(index, test->getName().c_str());
	}
}

void
TestHistory::SaveTestName(int index, CString testName)
{
	CWinApp* app = AfxGetApp();
	ASSERT(app != NULL);

	app->WriteProfileString(TestRunnerModel::PROFILE_SECTION,
							TestHistory::GetEntryName(index),
							testName);
}

CString
TestHistory::GetEntryName(int index) const
{
	CString entry;
	entry.Format(_T("HistoryTest%d"), index);
	return entry;
}

TestRunnerModel::TestRunnerModel(CPPUNIT_NS::Test* rootTest)
	: m_rootTest(rootTest)
{
	LoadSettings();
	LoadTestHistory();
}

CPPUNIT_NS::Test*
TestRunnerModel::GetRootTest()
{
	return m_rootTest;
}

void
TestRunnerModel::SaveSettings()
{
	CWinApp* app = AfxGetApp();
	ASSERT(app != NULL);

	int value = this->AutorunOnStartup ? 1 : 0;
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_AUTORUN_AT_STARTUP, value);

	value = this->AlwaysCallTestOnMainThread ? 1 : 0;
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_ALWAYS_CALL_TEST_ON_MAIN_THREAD, value);

	value = this->TreatTimeoutAsError ? 1 : 0;
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_TREAT_TIMEOUT_AS_ERROR, value);

	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_TYPE_COLUMN_WIDTH,  this->TypeColumnWidth);
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_NAME_COLUMN_WIDTH,  this->NameColumnWidth);
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_FAILED_CONDITION_COLUMN_WIDTH,  this->FailedConditionColumnWidth);
	app->WriteProfileInt(TestRunnerModel::PROFILE_SECTION, TestRunnerModel::ENTRY_LINE_NUMBER_COLUMN_WIDTH,  this->LineNumberColumnWidth);
}

void
TestRunnerModel::LoadSettings()
{
	CWinApp* app = AfxGetApp();
	ASSERT(app != NULL);

	UINT value = app->GetProfileInt(
					 TestRunnerModel::PROFILE_SECTION,
					 TestRunnerModel::ENTRY_AUTORUN_AT_STARTUP,
					 1);
	this->AutorunOnStartup = (value == 1);

	value = app->GetProfileInt(
				TestRunnerModel::PROFILE_SECTION,
				TestRunnerModel::ENTRY_ALWAYS_CALL_TEST_ON_MAIN_THREAD,
				0);
	this->AlwaysCallTestOnMainThread = (value == 1);

	value = app->GetProfileInt(
				TestRunnerModel::PROFILE_SECTION,
				TestRunnerModel::ENTRY_TREAT_TIMEOUT_AS_ERROR,
				0);
	this->TreatTimeoutAsError = (value == 1);

	this->TypeColumnWidth = app->GetProfileInt(
								TestRunnerModel::PROFILE_SECTION,
								TestRunnerModel::ENTRY_TYPE_COLUMN_WIDTH,
								80);

	this->NameColumnWidth = app->GetProfileInt(
								TestRunnerModel::PROFILE_SECTION,
								TestRunnerModel::ENTRY_NAME_COLUMN_WIDTH,
								80);

	this->FailedConditionColumnWidth = app->GetProfileInt(
										   TestRunnerModel::PROFILE_SECTION,
										   TestRunnerModel::ENTRY_FAILED_CONDITION_COLUMN_WIDTH,
										   80);

	this->LineNumberColumnWidth = app->GetProfileInt(
									  TestRunnerModel::PROFILE_SECTION,
									  TestRunnerModel::ENTRY_LINE_NUMBER_COLUMN_WIDTH,
									  80);
}


const TestHistory&
TestRunnerModel::GetTestHistory() const
{
	return m_testHistory;
}

bool
TestRunnerModel::SelectTest(int index)
{
	return m_testHistory.SetCurSel(index);
}

void
TestRunnerModel::SelectTest(CPPUNIT_NS::Test* test)
{
	m_testHistory.SetCurSel(test);
}

CPPUNIT_NS::Test*
TestRunnerModel::GetSelectedTest() const
{
	return m_testHistory.GetCurSel();
}

void
TestRunnerModel::LoadTestHistory()
{
	m_testHistory.clear();
	int index = 1;

	// 用于去重
	std::set<CString> names;

	do
	{
		CString testName = m_testHistory.GetTestName(index++);
		if (testName.IsEmpty())
		{
			break;
		}

		// 去重逻辑
		if (names.find(testName) == names.end())
		{
			names.insert(testName);
		}
		else
		{
			continue;
		}

		try
		{
			m_testHistory.push_back(m_rootTest->findTest(ToAnsiString(testName)));
		}
		catch (std::invalid_argument&)
		{}
	} while (true);

	// 如果为空则视为第一次运行，默认执行All Tests
	if (m_testHistory.empty())
	{
		try
		{
			m_testHistory.push_back(m_rootTest->findTest("All Tests"));
		}
		catch (std::invalid_argument&)
		{}
	}
}

void
TestRunnerModel::SaveTestHistory()
{
	m_testHistory.Save();
}

// Utility method, should be moved somewhere else...
std::string
TestRunnerModel::ToAnsiString(const CString& text)
{
#ifdef _UNICODE
	int bufferLength = ::WideCharToMultiByte(CP_THREAD_ACP, 0,
					   text, text.GetLength(),
					   NULL, 0, NULL, NULL) + 1;
	char* ansiString = new char[bufferLength];
	::WideCharToMultiByte(CP_THREAD_ACP, 0,
						  text, text.GetLength(),
						  ansiString, bufferLength,
						  NULL,
						  NULL);

	std::string str(ansiString, bufferLength - 1);
	delete[] ansiString;

	return str;
#else
	return std::string((LPCTSTR)text);
#endif
}
