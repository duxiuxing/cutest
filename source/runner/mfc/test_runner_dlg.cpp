#include "stdafx.h"
#include "test_runner_dlg.h"

#include "ListCtrlFormatter.h"
#include "ListCtrlSetter.h"
#include "test_config.h"
#include "TreeHierarchyDlg.h"

#include <cppunit/TestFailure.h>

#include "cutest/helper.h"
#include "cutest/runner.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

CTestRunnerDlg::CTestRunnerDlg(CPPUNIT_NS::Test* rootTest, CWnd* pParent)
	: cdxCDynamicDialog(CTestRunnerDlg::IDD, pParent)
	, m_model(rootTest)
	, m_uiState(UI_STATE_NONE)
	, m_hAccelerator(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_testCountTotal = 0;
	m_testCountRan = 0;
	m_errorCount = 0;
	m_failureCount = 0;

	m_msRunnerStart = 0;

	ModifyFlags(flSWPCopyBits, 0);      // anti-flickering option for resizing
}

void CTestRunnerDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_COMBO, m_historyCombo);
	DDX_Control(pDX, IDC_BROWSE_BUTTON, m_browseButton);
	DDX_Control(pDX, IDC_RUN_BUTTON, m_runButton);
	DDX_Control(pDX, IDC_ERROR_LIST, m_errorList);
	DDX_Control(pDX, IDC_STOP_BUTTON, m_stopButton);
	DDX_Control(pDX, IDOK, m_closeButton);
	DDX_Control(pDX, IDC_ERROR_DETAIL, m_errorDetail);
}

BEGIN_MESSAGE_MAP(CTestRunnerDlg, cdxCDynamicDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, OnStopButton)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_COMMAND(ID_QUIT_APPLICATION, OnQuitApplication)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ERROR_LIST, OnSelectedItemChangeInErrorList)
	ON_CBN_SELCHANGE(IDC_HISTORY_COMBO, OnSelectTestInHistoryCombo)
	ON_WM_MENUSELECT()
	ON_COMMAND(ID_AUTORUN_AT_STARTUP, OnAutorunAtStartup)
	ON_COMMAND(ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, OnAlwaysCallTestOnMainThread)
	ON_COMMAND(ID_TREAT_TIMEOUT_AS_ERROR, OnTreatTimeoutAsError)
END_MESSAGE_MAP()

// CTestRunnerDlg message handlers
BOOL CTestRunnerDlg::OnInitDialog()
{
	cdxCDynamicDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

	// 根据项目名、版本信息拼接窗口标题
	{
		CString textOld;
		GetWindowText(textOld);

		CString textNew;
		textNew.Format(_T("%s - %s"), textOld, TestConfig::Instance()->Title());

		SetWindowText(textNew);
	}

	m_hAccelerator = ::LoadAccelerators(AfxGetResourceHandle(),
										MAKEINTRESOURCE(IDR_ACCELERATOR_TEST_RUNNER));

	// It always fails!!! I don't understand why. Complain about not finding the resource name!
	ASSERT(m_hAccelerator != NULL);

	ResetHistoryCombo();

	m_progressBar.Create(NULL, NULL, WS_CHILD | WS_VISIBLE,
						 GetItemClientRect(IDC_STATIC_PROGRESS_BAR), this, 0);

	CUTEST_NS::Runner::Instance()->SetAlwaysCallTestOnMainThread(m_model.AlwaysCallTestOnMainThread);
	CUTEST_NS::Runner::Instance()->SetTreatTimeoutAsError(m_model.TreatTimeoutAsError);

	m_errorIcons.Create(IDB_ERROR_ICONS, 16, 1, RGB(255, 0, 255));
	m_errorList.SetImageList(&m_errorIcons, LVSIL_SMALL);
	m_errorList.SetExtendedStyle(m_errorList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CRect listBounds;
	m_errorList.GetClientRect(&listBounds);
	int nLastColumnWidth = listBounds.Width() - m_model.TypeColumnWidth - m_model.NameColumnWidth
						   - m_model.FailedConditionColumnWidth - m_model.LineNumberColumnWidth;
	ListCtrlFormatter formatter(m_errorList);
	formatter.AddColumn(IDS_ERRORLIST_TYPE, m_model.TypeColumnWidth, LVCFMT_LEFT, 0);
	formatter.AddColumn(IDS_ERRORLIST_NAME, m_model.NameColumnWidth, LVCFMT_LEFT, 1);
	formatter.AddColumn(IDS_ERRORLIST_FAILED_CONDITION, m_model.FailedConditionColumnWidth, LVCFMT_LEFT, 2);
	m_errorList.setLineNumberSubItem(formatter.GetNextColumnIndex());
	formatter.AddColumn(IDS_ERRORLIST_LINE_NUMBER, m_model.LineNumberColumnWidth, LVCFMT_LEFT, 3);
	m_errorList.setFileNameSubItem(formatter.GetNextColumnIndex());
	formatter.AddColumn(IDS_ERRORLIST_FILE_NAME, nLastColumnWidth, LVCFMT_LEFT, 4);

	LOGFONT font;
	GetFont()->GetLogFont(&font);
	font.lfPitchAndFamily = FIXED_PITCH | //VARIABLE_PITCH
							(font.lfPitchAndFamily & ~15);   // font family
	m_fixedSizeFont.CreateFontIndirect(&font);
	m_errorDetail.SetFont(&m_fixedSizeFont);   // Does not work. Need to investigate...

	const int listGrowthRatio = 80;
	AddSzXControl(IDC_HISTORY_COMBO, mdResize);
	AddSzXControl(IDC_BROWSE_BUTTON, mdRepos);
	AddSzXControl(IDC_RUNNING_TEST_NAME, mdResize);
	AddSzXControl(IDC_RUN_BUTTON, mdRepos);
	AddSzXControl(m_progressBar, mdResize);
	AddSzControl(IDC_ERROR_LIST, 0, 0, 100, listGrowthRatio);
	AddSzXControl(IDC_STOP_BUTTON, mdRepos);
	AddSzXControl(IDOK, mdRepos);
	AddSzYControl(IDC_STATIC_ERROR_DETAIL, listGrowthRatio, listGrowthRatio);
	AddSzControl(IDC_ERROR_DETAIL, 0, listGrowthRatio, 100, 100);
	AddSzControl(IDC_EXECUTION_TIME, mdResize, mdRepos);

	UpdateErrorListColumnWidth();

	if (m_model.AutorunOnStartup)
	{
		OnRunButton();
	}

	return TRUE; // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CTestRunnerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);   // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		cdxCDynamicDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestRunnerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestRunnerDlg::OnRunButton()
{
	if (UI_STATE_NONE != m_uiState)
	{
		return;
	}

	CPPUNIT_NS::Test* selectedTest = m_model.GetSelectedTest();

	if (!selectedTest)
	{
		return;
	}

	m_progressBar.reset();

	m_testCountRan = 0;
	m_errorCount = 0;
	m_failureCount = 0;
	UpdateAllCountsToUI();

	CListCtrl* listCtrl = (CListCtrl*)GetDlgItem(IDC_ERROR_LIST);
	listCtrl->DeleteAllItems();

	UpdateErrorDetailToUI(-1);

	m_msRunnerStart = CUTEST_NS::TickCount64();
	UpdateExecutionTimeToUI();

	SetUIState(UI_STATE_RUNING);

	CUTEST_NS::Runner* runner = CUTEST_NS::Runner::Instance();
	runner->AddListener(this);

	m_testCountTotal = selectedTest->countTestCases();
	m_progressBar.start(m_testCountTotal);

	runner->Start(selectedTest);
}

void CTestRunnerDlg::AddItemToErrorList(const CPPUNIT_NS::TestFailure& failure)
{
	CListCtrl* listCtrl = (CListCtrl*)GetDlgItem(IDC_ERROR_LIST);
	int currentEntry = listCtrl->GetItemCount();

	ListCtrlSetter setter(*listCtrl);
	setter.insertLine(currentEntry);
	if (failure.isError())
	{
		setter.addSubItem(_T("Error"), ERROR_ICON_ERROR);
	}
	else
	{
		setter.addSubItem(_T("Failure"), ERROR_ICON_FAILURE);
	}

	// Set test name
	setter.addSubItem(failure.failedTestName().c_str());

	// Set the asserted text
	CString message(failure.thrownException()->message().shortDescription().c_str());
	message.Replace(_T('\n'), _T(' '));       // should only print the short description there,
	setter.addSubItem(message);   // and dump the detail on an edit control when clicked.

	// Set the line number
	if (failure.sourceLine().isValid())
	{
		CString lineNumber;
		lineNumber.Format(_T("%ld"), failure.sourceLine().lineNumber());
		setter.addSubItem(lineNumber);
	}
	else
	{
		setter.addSubItem(_T(""));
	}

	// Set the file name
	setter.addSubItem(failure.sourceLine().fileName().c_str());

	if (!listCtrl->GetFirstSelectedItemPosition())
	{
		// Select first entry => display details of first entry.
		listCtrl->SetItemState(currentEntry, LVIS_SELECTED, LVIS_SELECTED);
		listCtrl->SetFocus();   // Does not work ?!?
	}

	listCtrl->RedrawItems(currentEntry, currentEntry);
	listCtrl->UpdateWindow();
}

void CTestRunnerDlg::OnTestStart(CPPUNIT_NS::Test* test)
{
	CWnd* dlgItem = GetDlgItem(IDC_RUNNING_TEST_NAME);
	if (dlgItem)
	{
		dlgItem->SetWindowText(CString(test->getName().c_str()));
	}
}

void CTestRunnerDlg::OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure)
{
	AddItemToErrorList(failure);
	if (failure.isError())
	{
		++m_errorCount;
	}
	else
	{
		++m_failureCount;
	}

	UpdateAllCountsToUI();
}

void CTestRunnerDlg::OnTestEnd(
	CPPUNIT_NS::Test* test,
	unsigned int error_count,
	unsigned int failure_count,
	unsigned int msElapsed)
{
	++m_testCountRan;
	UpdateAllCountsToUI();
	m_progressBar.step(m_failureCount == 0 && m_errorCount == 0);

	UpdateExecutionTimeToUI();

	CWnd* dlgItem = GetDlgItem(IDC_RUNNING_TEST_NAME);
	if (dlgItem)
	{
		CString text;
		text.LoadString(IDS_NONE);
		dlgItem->SetWindowText(text);
	}
}

void CTestRunnerDlg::OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed)
{
	if (UI_STATE_CLOSING == m_uiState)
	{
		SetUIState(UI_STATE_NONE);
		PostMessage(WM_CLOSE);
	}
	else
	{
		SetUIState(UI_STATE_NONE);
	}
}

void CTestRunnerDlg::UpdateAllCountsToUI()
{
	CString text;

	text.Format(_T("%d/%d"), m_testCountRan, m_testCountTotal);
	GetDlgItem(IDC_TEST_COUNT)->SetWindowText(text);

	text.Format(_T("%d"), m_errorCount);
	GetDlgItem(IDC_ERROR_COUNT)->SetWindowText(text);

	text.Format(_T("%d"), m_failureCount);
	GetDlgItem(IDC_FAILURE_COUNT)->SetWindowText(text);
}

void CTestRunnerDlg::UpdateExecutionTimeToUI()
{
	unsigned int msElapsed = 0;
	if (m_msRunnerStart)
	{
		msElapsed = (unsigned int)(CUTEST_NS::TickCount64() - m_msRunnerStart);
	}

	CString text;
	text.Format(_T("Execution time: %lu ms"), msElapsed);
	GetDlgItem(IDC_EXECUTION_TIME)->SetWindowText(text);
}

void CTestRunnerDlg::OnStopButton()
{
	if (UI_STATE_RUNING != m_uiState)
	{
		return;
	}

	SetUIState(UI_STATE_STOPPING);
	CUTEST_NS::Runner::Instance()->Stop();
}

void CTestRunnerDlg::OnOK()
{
	SafeCloseDialog();
}

void CTestRunnerDlg::OnSelectTestInHistoryCombo()
{
	unsigned int index = m_historyCombo.GetCurSel();

	if (m_model.SelectTest(index))
	{
		ResetHistoryCombo();
		m_model.SaveTestHistory();
	}
}

void CTestRunnerDlg::ResetHistoryCombo()
{
	m_historyCombo.LockWindowUpdate();

	m_historyCombo.ResetContent();

	const TestHistory& history = m_model.GetTestHistory();
	for (TestHistory::const_iterator it = history.begin();
		 it != history.end();
		 ++it)
	{
		CPPUNIT_NS::Test* test = *it;
		m_historyCombo.AddString(CString(test->getName().c_str()));
	}

	if (history.size() > 0)
	{
		m_historyCombo.SetCurSel(0);
	}

	SetUIState(UI_STATE_NONE);

	m_historyCombo.UnlockWindowUpdate();
}

void CTestRunnerDlg::OnBrowseButton()
{
	TreeHierarchyDlg dlg;
	dlg.setRootTest(m_model.GetRootTest());
	if (dlg.DoModal() == IDOK)
	{
		m_model.SelectTest(dlg.getSelectedTest());
		ResetHistoryCombo();
		m_model.SaveTestHistory();
	}
}

BOOL CTestRunnerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(m_hWnd,
							   m_hAccelerator,
							   pMsg))
	{
		return TRUE;
	}
	return cdxCDynamicDialog::PreTranslateMessage(pMsg);
}

void CTestRunnerDlg::OnQuitApplication()
{
	SafeCloseDialog();
}

void CTestRunnerDlg::OnClose()
{
	SafeCloseDialog();
}

CRect CTestRunnerDlg::GetItemWindowRect(int itemID)
{
	CWnd* pItem = GetDlgItem(itemID);
	CRect rect;
	if (pItem)
	{
		pItem->GetWindowRect(&rect);
	}
	return rect;
}

CRect CTestRunnerDlg::GetItemClientRect(int itemID)
{
	CRect rect = GetItemWindowRect(itemID);
	if (!rect.IsRectNull())
	{
		CPoint clientTopLeft = rect.TopLeft();
		ScreenToClient(&clientTopLeft);
		rect = CRect(clientTopLeft, rect.Size());
	}

	return rect;
}

void CTestRunnerDlg::OnSize(UINT nType, int cx, int cy)
{
	cdxCDynamicDialog::OnSize(nType, cx, cy);
	UpdateErrorListColumnWidth();
}

void CTestRunnerDlg::UpdateErrorListColumnWidth()
{
	if (!m_errorList.GetSafeHwnd())
	{
		return;
	}

	// resize to fit last column
	CRect listBounds = GetItemClientRect(IDC_ERROR_LIST);

	int width_1_4 = 0;
	for (int i = 0; i < 4; ++i)
	{
		width_1_4 += m_errorList.GetColumnWidth(i);
	}

	// the 4 offset is so no horiz scroll bar will appear
	m_errorList.SetColumnWidth(4, listBounds.Width() - width_1_4 - 4);
}

void CTestRunnerDlg::OnSelectedItemChangeInErrorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0)     // item selected
	{
		UpdateErrorDetailToUI(pNMListView->iItem);
	}

	*pResult = 0;
}

void CTestRunnerDlg::UpdateErrorDetailToUI(unsigned int failureIndex)
{
	const CPPUNIT_NS::TestFailure* failure = CUTEST_NS::Runner::Instance()->FailureAt(failureIndex);

	CString text;
	if (failure)
	{
		text = failure->thrownException()->what();
	}
	text.Replace(_T("\n"), _T("\r\n"));

	m_errorDetail.SetWindowText(text);
}

void CTestRunnerDlg::SetUIState(UIState state)
{
	m_uiState = state;

	switch (state)
	{
	case UI_STATE_NONE:
		{
			CPPUNIT_NS::Test* rootTest = m_model.GetRootTest();
			if (rootTest && rootTest->countTestCases() > 0)
			{
				m_browseButton.EnableWindow(TRUE);
			}
			else
			{
				m_browseButton.EnableWindow(FALSE);
			}

			if (CB_ERR == m_historyCombo.GetCurSel())
			{
				m_runButton.EnableWindow(FALSE);
			}
			else
			{
				m_runButton.EnableWindow(TRUE);
			}

			m_stopButton.EnableWindow(FALSE);
			m_closeButton.EnableWindow(TRUE);
		}
		break;
	case UI_STATE_RUNING:
		{
			m_browseButton.EnableWindow(FALSE);
			m_runButton.EnableWindow(FALSE);
			m_stopButton.EnableWindow(TRUE);
			m_closeButton.EnableWindow(FALSE);
		}
		break;
	default:
		{
			m_browseButton.EnableWindow(FALSE);
			m_runButton.EnableWindow(FALSE);
			m_stopButton.EnableWindow(FALSE);
			m_closeButton.EnableWindow(FALSE);
		}
		break;
	}
}

void CTestRunnerDlg::SafeCloseDialog()
{
	m_model.TypeColumnWidth = m_errorList.GetColumnWidth(0);
	m_model.NameColumnWidth = m_errorList.GetColumnWidth(1);
	m_model.FailedConditionColumnWidth = m_errorList.GetColumnWidth(2);
	m_model.LineNumberColumnWidth = m_errorList.GetColumnWidth(3);
	m_model.SaveSettings();


	switch (m_uiState)
	{
	case UI_STATE_NONE:
		EndDialog(IDOK);
		break;
	case UI_STATE_RUNING:
		{
			SetUIState(UI_STATE_CLOSING);
			CUTEST_NS::Runner::Instance()->Stop();
		}
		break;
	default:
		SetUIState(UI_STATE_CLOSING);
		break;
	}
}

void CTestRunnerDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	cdxCDynamicDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);

	CMenu* mainMenu = GetMenu();
	if (mainMenu && mainMenu->GetSafeHmenu())
	{
		UINT check = m_model.AutorunOnStartup ? MF_CHECKED : MF_UNCHECKED;
		mainMenu->CheckMenuItem(ID_AUTORUN_AT_STARTUP, MF_BYCOMMAND | check);
		UINT enable = (m_uiState == UI_STATE_NONE) ? MF_ENABLED : MF_GRAYED;
		mainMenu->EnableMenuItem(ID_AUTORUN_AT_STARTUP, MF_BYCOMMAND | enable);

		check = CUTEST_NS::Runner::Instance()->IsAlwaysCallTestOnMainThread() ? MF_CHECKED : MF_UNCHECKED;
		mainMenu->CheckMenuItem(ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, MF_BYCOMMAND | check);
		mainMenu->EnableMenuItem(ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, MF_BYCOMMAND | enable);

		check = CUTEST_NS::Runner::Instance()->IsTreatTimeoutAsError() ? MF_CHECKED : MF_UNCHECKED;
		mainMenu->CheckMenuItem(ID_TREAT_TIMEOUT_AS_ERROR, MF_BYCOMMAND | check);
		mainMenu->EnableMenuItem(ID_TREAT_TIMEOUT_AS_ERROR, MF_BYCOMMAND | enable);
	}
}

void CTestRunnerDlg::OnAutorunAtStartup()
{
	m_model.AutorunOnStartup = !m_model.AutorunOnStartup;
	m_model.SaveSettings();
}

void CTestRunnerDlg::OnAlwaysCallTestOnMainThread()
{
	m_model.AlwaysCallTestOnMainThread = !m_model.AlwaysCallTestOnMainThread;
	CUTEST_NS::Runner::Instance()->SetAlwaysCallTestOnMainThread(m_model.AlwaysCallTestOnMainThread);
	m_model.SaveSettings();
}

void CTestRunnerDlg::OnTreatTimeoutAsError()
{
	m_model.TreatTimeoutAsError = !m_model.TreatTimeoutAsError;
	CUTEST_NS::Runner::Instance()->SetTreatTimeoutAsError(m_model.TreatTimeoutAsError);
	m_model.SaveSettings();
}
