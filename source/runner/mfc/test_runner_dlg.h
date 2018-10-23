#pragma once

// local
#include "dynamic_window/cdxCDynamicDialog.h"
#include "MsDevCallerListCtrl.h"
#include "ProgressBar.h"
#include "resource.h"
#include "test_runner_model.h"

#include <cppunit/TestSuite.h>
#include <cppunit/Exception.h>
#include <cppunit/TestResultCollector.h>

#include "cutest/listener.h"

#include <afxwin.h>
#include <vector>

class CTestRunnerDlg
	: public cdxCDynamicDialog
	, public CUTEST_NS::Listener
{
	// Construction
public:
	CTestRunnerDlg(CPPUNIT_NS::Test* rootTest, CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	enum
	{
		IDD = IDD_TEST_RUNNER,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Implementation
protected:
	HICON m_hIcon;
	HACCEL m_hAccelerator;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuitApplication();
	afx_msg void OnClose();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);

	DECLARE_MESSAGE_MAP()

public: // overrided from CUTEST_NS::Listener
	virtual void OnTestStart(CPPUNIT_NS::Test* test) override;
	virtual void OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) override;
	virtual void OnTestEnd(
		CPPUNIT_NS::Test* test,
		unsigned int errorCount,
		unsigned int failureCount,
		unsigned int msElapsed) override;
	virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) override;

protected: // Helper
	CRect GetItemWindowRect(int itemID);
	CRect GetItemClientRect(int itemID);

protected:
	CComboBox m_historyCombo;
	afx_msg void OnSelectTestInHistoryCombo();
	void ResetHistoryCombo();

	ProgressBar m_progressBar;

	CButton m_browseButton;
	afx_msg void OnBrowseButton();

	CButton m_runButton;
	afx_msg void OnRunButton();

	int m_testCountTotal;
	int m_testCountRan;
	int m_errorCount;
	int m_failureCount;
	void UpdateAllCountsToUI();

	afx_msg void OnAutorunAtStartup();

	afx_msg void OnAlwaysCallTestOnMainThread();
	afx_msg void OnTreatTimeoutAsError();

	enum ErrorIconIndex
	{
		ERROR_ICON_FAILURE = 0,
		ERROR_ICON_ERROR
	};
	CImageList m_errorIcons;
	MsDevCallerListCtrl m_errorList;
	afx_msg void OnSelectedItemChangeInErrorList(NMHDR* pNMHDR, LRESULT* pResult);
	void AddItemToErrorList(const CPPUNIT_NS::TestFailure& failure);

	CButton m_stopButton;
	afx_msg void OnStopButton();

	CButton m_closeButton;
	virtual void OnOK();
	void SafeCloseDialog();

	CEdit m_errorDetail;
	CFont m_fixedSizeFont;
	void UpdateErrorDetailToUI(unsigned int failureIndex);

	unsigned long long m_msRunnerStart;
	void UpdateExecutionTimeToUI();

	enum UIState
	{
		UI_STATE_NONE = 0,  // 空闲状态
		UI_STATE_RUNING,    // 正在执行测试用例
		UI_STATE_STOPPING,  // 点击Stop按钮，正在结束当前用例
		UI_STATE_CLOSING,   // 如果Stop的过程中触发了关闭窗口操作，会进入此状态
	};
	UIState m_uiState;
	void SetUIState(UIState state);

	void UpdateErrorListColumnWidth();

private:
	TestRunnerModel m_model;
};
