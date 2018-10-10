#pragma once

// local
#include "DynamicWindow/cdxCDynamicDialog.h"
#include "MsDevCallerListCtrl.h"
#include "ProgressBar.h"
#include "resource.h"
#include "TestRunnerModel.h"

#include <cppunit/TestSuite.h>
#include <cppunit/Exception.h>
#include <cppunit/TestResultCollector.h>

#include "cutest/ProgressListener.h"

#include <afxwin.h>
#include <vector>

class CTestRunnerDlg
  : public cdxCDynamicDialog
  , public CUTEST_NS::ProgressListener
{
  // Construction
public:
  CTestRunnerDlg( CPPUNIT_NS::Test *rootTest, CWnd *pParent = NULL ); // standard constructor

  // Dialog Data
  enum
  {
    IDD = IDD_TEST_RUNNER,
  };

protected:
  virtual void DoDataExchange( CDataExchange *pDX );  // DDX/DDV support
  virtual void PostNcDestroy();
  virtual BOOL PreTranslateMessage( MSG *pMsg );

  // Implementation
protected:
  HICON m_hIcon;
  HACCEL m_hAccelerator;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSize( UINT nType, int cx, int cy );
  afx_msg void OnQuitApplication();
  afx_msg void OnClose();
  afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );

  DECLARE_MESSAGE_MAP()

public: // overrided from TestProgressListener
  virtual void onTestStart( CPPUNIT_NS::Test *test ) override;
  virtual void onFailureAdd( unsigned int index, const CPPUNIT_NS::TestFailure &failure ) override;
  virtual void onTestEnd(
    CPPUNIT_NS::Test *test,
    unsigned int error_count,
    unsigned int failure_count,
    unsigned int elapsed_ms ) override;
  virtual void onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms ) override;

protected: // Helper
  CRect getItemWindowRect( int nID );
  CRect getItemClientRect( int nID );

protected:
  CComboBox m_comboHistory;
  afx_msg void OnSelectTestInHistoryCombo();
  void ResetHistoryCombo();

  ProgressBar m_testsProgress;

  CButton m_buttonBrowse;
  afx_msg void OnBrowseTest();

  CButton m_buttonRun;
  afx_msg void OnRun();

  int m_testsCount;
  int m_testsRun;
  int m_errors;
  int m_failures;
  void UpdateUI_Counts();

  afx_msg void OnAutorunAtStartup();

  afx_msg void OnAlwaysCallTestOnMainThread();
  afx_msg void OnTreatTimeoutAsError();

  enum ErrorTypeBitmaps
  {
    errorTypeFailure = 0,
    errorTypeError
  };
  CImageList m_errorListBitmap;
  MsDevCallerListCtrl m_listCtrl;
  afx_msg void OnSelectedFailureChange( NMHDR *pNMHDR, LRESULT *pResult );
  void addListEntry( const CPPUNIT_NS::TestFailure &failure );

  CButton m_buttonStop;
  afx_msg void OnStop();

  CButton m_buttonClose;
  virtual void OnOK();
  void SafeCloseDialog();

  CEdit m_details;
  CFont m_fixedSizeFont;
  void displayFailureDetailsFor( unsigned int failureIndex );

  unsigned long long m_testStartTime;
  unsigned long long m_testEndTime;
  void UpdateUI_Time();

  enum UIState
  {
    UI_STATE_NONE = 0,  // 空闲状态
    UI_STATE_RUNING,    // 正在执行测试用例
    UI_STATE_STOPPING,  // 点击Stop按钮，正在结束当前用例
    UI_STATE_CLOSING,   // 如果Stop的过程中触发了关闭窗口操作，会进入此状态
  };
  UIState m_uiState;
  void SetUIState( UIState state );

  void updateListColumnSize();

private:
  TestRunnerModel m_model;

  void saveSettings();
};
