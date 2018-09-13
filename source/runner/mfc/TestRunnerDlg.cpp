#include "stdafx.h"
#include "TestRunnerDlg.h"

#include "ListCtrlFormatter.h"
#include "ListCtrlSetter.h"
#include "TestConfig.h"
#include "TreeHierarchyDlg.h"

#include <cppunit/TestFailure.h>

#include "cutest/Helper.h"
#include "cutest/Runner.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
#endif

// CTestRunnerDlg dialog
CTestRunnerDlg::CTestRunnerDlg( CPPUNIT_NS::Test *rootTest, CWnd *pParent )
  : cdxCDynamicDialog( CTestRunnerDlg::IDD, pParent )
  , m_model( rootTest )
  , m_uiState( UI_STATE_NONE )
  , m_hAccelerator( NULL )
{
  m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

  m_testsCount = 0;
  m_testsRun = 0;
  m_errors = 0;
  m_failures = 0;

  m_bAutorunAtStartup = FALSE;

  m_testStartTime = CUTEST_NS::tickCount64();
  m_testEndTime = m_testStartTime;

  ModifyFlags( flSWPCopyBits, 0 );    // anti-flickering option for resizing
}

void
CTestRunnerDlg::DoDataExchange( CDataExchange *pDX )
{
  cdxCDynamicDialog::DoDataExchange( pDX );
  DDX_Control( pDX, IDC_COMBO_TEST, m_comboHistory );
  DDX_Control( pDX, IDC_BROWSE_TEST, m_buttonBrowse );
  DDX_Control( pDX, ID_RUN, m_buttonRun );
  DDX_Control( pDX, IDC_LIST, m_listCtrl );
  DDX_Control( pDX, ID_STOP, m_buttonStop );
  DDX_Control( pDX, IDOK, m_buttonClose );
  DDX_Control( pDX, IDC_DETAILS, m_details );
}

void
CTestRunnerDlg::PostNcDestroy()
{}

BEGIN_MESSAGE_MAP( CTestRunnerDlg, cdxCDynamicDialog )
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED( ID_RUN, OnRun )
  ON_BN_CLICKED( ID_STOP, OnStop )
  ON_BN_CLICKED( IDC_BROWSE_TEST, OnBrowseTest )
  ON_COMMAND( ID_QUIT_APPLICATION, OnQuitApplication )
  ON_WM_CLOSE()
  ON_WM_SIZE()
  ON_NOTIFY( LVN_ITEMCHANGED, IDC_LIST, OnSelectedFailureChange )
  ON_CBN_SELCHANGE( IDC_COMBO_TEST, OnSelectTestInHistoryCombo )
  ON_WM_MENUSELECT()
  ON_COMMAND( ID_AUTORUN_AT_STARTUP, OnAutorunAtStartup )
  ON_COMMAND( ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, OnAlwaysCallTestOnMainThread )
  ON_COMMAND( ID_TREAT_TIMEOUT_AS_ERROR, OnTreatTimeoutAsError )
END_MESSAGE_MAP()

// CTestRunnerDlg message handlers
BOOL
CTestRunnerDlg::OnInitDialog()
{
  cdxCDynamicDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon( m_hIcon, TRUE );       // Set big icon
  SetIcon( m_hIcon, FALSE );      // Set small icon

  // 根据项目名、版本信息拼接窗口标题
  {
    CString caption;
    GetWindowText( caption );

    CString text;
    text.Format( _T( "%s - %s" ),
                 caption, TestConfig::GetInstance()->GetTitle() );

    SetWindowText( text );
  }

  m_model.loadSettings( m_settings );

  m_hAccelerator = ::LoadAccelerators( AfxGetResourceHandle(),
                                       MAKEINTRESOURCE( IDR_ACCELERATOR_TEST_RUNNER ) );

  // It always fails!!! I don't understand why. Complain about not finding the resource name!
  ASSERT( m_hAccelerator != NULL );

  ResetHistoryCombo();

  m_testsProgress.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, getItemClientRect( IDC_STATIC_PROGRESS_BAR ), this, 0 );

  m_bAutorunAtStartup = m_settings.autorun_on_startup;
  CUTEST_NS::Runner::instance()->setAlwaysCallTestOnMainThread( m_settings.always_call_test_on_main_thread );
  CUTEST_NS::Runner::instance()->setTreatTimeoutAsError( m_settings.treat_timeout_as_error );

  m_errorListBitmap.Create( IDB_ERROR_TYPE, 16, 1, RGB( 255, 0, 255 ) );
  m_listCtrl.SetImageList( &m_errorListBitmap, LVSIL_SMALL );
  m_listCtrl.SetExtendedStyle( m_listCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
  int total_col_1_4 = m_settings.col_1 + m_settings.col_2 +
                      m_settings.col_3 + m_settings.col_4;
  CRect listBounds;
  m_listCtrl.GetClientRect( &listBounds );
  int col_5_width = listBounds.Width() - total_col_1_4; // 5th column = rest of listview space
  ListCtrlFormatter formatter( m_listCtrl );
  formatter.AddColumn( IDS_ERRORLIST_TYPE, m_settings.col_1, LVCFMT_LEFT, 0 );
  formatter.AddColumn( IDS_ERRORLIST_NAME, m_settings.col_2, LVCFMT_LEFT, 1 );
  formatter.AddColumn( IDS_ERRORLIST_FAILED_CONDITION, m_settings.col_3, LVCFMT_LEFT, 2 );
  m_listCtrl.setLineNumberSubItem( formatter.GetNextColumnIndex() );
  formatter.AddColumn( IDS_ERRORLIST_LINE_NUMBER, m_settings.col_4, LVCFMT_LEFT, 3 );
  m_listCtrl.setFileNameSubItem( formatter.GetNextColumnIndex() );
  formatter.AddColumn( IDS_ERRORLIST_FILE_NAME, col_5_width, LVCFMT_LEFT, 4 );

  LOGFONT font;
  GetFont()->GetLogFont( &font );
  font.lfPitchAndFamily = FIXED_PITCH | //VARIABLE_PITCH
                          ( font.lfPitchAndFamily & ~15 ); // font family
  m_fixedSizeFont.CreateFontIndirect( &font );
  m_details.SetFont( &m_fixedSizeFont ); // Does not work. Need to investigate...

  const int listGrowthRatio = 80;
  AddSzXControl( IDC_COMBO_TEST, mdResize );
  AddSzXControl( IDC_BROWSE_TEST, mdRepos );
  AddSzXControl( IDC_RUNNING_TEST_CASE_LABEL, mdResize );
  AddSzXControl( ID_RUN, mdRepos );
  AddSzXControl( m_testsProgress, mdResize );
  AddSzControl( IDC_LIST, 0, 0, 100, listGrowthRatio );
  AddSzXControl( ID_STOP, mdRepos );
  AddSzXControl( IDOK, mdRepos );
  AddSzYControl( IDC_STATIC_DETAILS, listGrowthRatio, listGrowthRatio );
  AddSzControl( IDC_DETAILS, 0, listGrowthRatio, 100, 100 );
  AddSzControl( IDC_EDIT_TIME, mdResize, mdRepos );
  RestoreWindowPosition( TestRunnerModel::settingKey, TestRunnerModel::settingMainDialogKey );

  updateListColumnSize();

  if ( m_bAutorunAtStartup )
  {
    OnRun();
  }

  return TRUE; // return TRUE unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void
CTestRunnerDlg::OnPaint()
{
  if ( IsIconic() )
  {
    CPaintDC dc( this ); // device context for painting

    SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics( SM_CXICON );
    int cyIcon = GetSystemMetrics( SM_CYICON );
    CRect rect;
    GetClientRect( &rect );
    int x = ( rect.Width() - cxIcon + 1 ) / 2;
    int y = ( rect.Height() - cyIcon + 1 ) / 2;

    // Draw the icon
    dc.DrawIcon( x, y, m_hIcon );
  }
  else
  {
    cdxCDynamicDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR
CTestRunnerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>( m_hIcon );
}

void
CTestRunnerDlg::OnRun()
{
  if ( UI_STATE_NONE != m_uiState )
  {
    return;
  }

  CPPUNIT_NS::Test *selectedTest = m_model.selectedTest();

  if ( !selectedTest )
  {
    return;
  }

  m_testsProgress.reset();

  m_testsRun = 0;
  m_errors = 0;
  m_failures = 0;
  UpdateUI_Counts();

  CListCtrl *listCtrl = ( CListCtrl * )GetDlgItem( IDC_LIST );
  listCtrl->DeleteAllItems();

  displayFailureDetailsFor( -1 );
  m_testEndTime = m_testStartTime;
  UpdateUI_Time();

  SetUIState( UI_STATE_RUNING );

  CUTEST_NS::Runner *runner = CUTEST_NS::Runner::instance();
  runner->addListener( this );

  m_testsCount = selectedTest->countTestCases();
  m_testsProgress.start( m_testsCount );
  m_testStartTime = CUTEST_NS::tickCount64();
  runner->start( selectedTest );
}

void
CTestRunnerDlg::addListEntry( const CPPUNIT_NS::TestFailure &failure )
{
  CListCtrl *listCtrl = ( CListCtrl * )GetDlgItem( IDC_LIST );
  int currentEntry = listCtrl->GetItemCount();

  ErrorTypeBitmaps errorType;
  if ( failure.isError() )
  {
    errorType = errorTypeError;
  }
  else
  {
    errorType = errorTypeFailure;
  }

  ListCtrlSetter setter( *listCtrl );
  setter.insertLine( currentEntry );
  setter.addSubItem( failure.isError() ? _T( "Error" ) : _T( "Failure" ), errorType );

  // Set test name
  setter.addSubItem( failure.failedTestName().c_str(), errorType );

  // Set the asserted text
  CString message( failure.thrownException()->message().shortDescription().c_str() );
  message.Replace( _T( '\n' ), _T( ' ' ) ); // should only print the short description there,
  setter.addSubItem( message ); // and dump the detail on an edit control when clicked.

  // Set the line number
  if ( failure.sourceLine().isValid() )
  {
    CString lineNumber;
    lineNumber.Format( _T( "%ld" ), failure.sourceLine().lineNumber() );
    setter.addSubItem( lineNumber );
  }
  else
  {
    setter.addSubItem( _T( "" ) );
  }

  // Set the file name
  setter.addSubItem( failure.sourceLine().fileName().c_str() );

  if ( !listCtrl->GetFirstSelectedItemPosition() )
  {
    // Select first entry => display details of first entry.
    listCtrl->SetItemState( currentEntry, LVIS_SELECTED, LVIS_SELECTED );
    listCtrl->SetFocus();   // Does not work ?!?
  }

  listCtrl->RedrawItems( currentEntry, currentEntry );
  listCtrl->UpdateWindow();
}

void
CTestRunnerDlg::onTestStart( CPPUNIT_NS::Test *test )
{
  CWnd *runningTestCaseLabel = GetDlgItem( IDC_RUNNING_TEST_CASE_LABEL );
  if ( runningTestCaseLabel )
  {
    runningTestCaseLabel->SetWindowText( CString( test->getName().c_str() ) );
  }
}

void
CTestRunnerDlg::onFailureAdd( unsigned int index, const CPPUNIT_NS::TestFailure &failure )
{
  addListEntry( failure );
  if ( failure.isError() )
  {
    m_errors++;
  }
  else
  {
    m_failures++;
  }

  UpdateUI_Counts();
}

void
CTestRunnerDlg::onTestEnd(
  CPPUNIT_NS::Test *test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms )
{
  m_testsRun++;
  UpdateUI_Counts();
  m_testsProgress.step( m_failures == 0 && m_errors == 0 );

  m_testEndTime = CUTEST_NS::tickCount64();
  UpdateUI_Time();

  CWnd *runningTestCaseLabel = GetDlgItem( IDC_RUNNING_TEST_CASE_LABEL );
  if ( runningTestCaseLabel )
  {
    CString text;
    text.LoadString( IDS_PROGRESS_NONE );
    runningTestCaseLabel->SetWindowText( text );
  }
}

void
CTestRunnerDlg::onRunnerEnd( CPPUNIT_NS::Test *test, unsigned int elapsed_ms )
{
  if ( UI_STATE_CLOSING == m_uiState )
  {
    SetUIState( UI_STATE_NONE );
    PostMessage( WM_CLOSE );
  }
  else
  {
    SetUIState( UI_STATE_NONE );
  }
}

void
CTestRunnerDlg::UpdateUI_Counts()
{
  CString argumentString;

  argumentString.Format( _T( "%d/%d" ), m_testsRun, m_testsCount );
  GetDlgItem( IDC_STATIC_RUNS )->SetWindowText( argumentString );

  argumentString.Format( _T( "%d" ), m_errors );
  GetDlgItem( IDC_STATIC_ERRORS )->SetWindowText( argumentString );

  argumentString.Format( _T( "%d" ), m_failures );
  GetDlgItem( IDC_STATIC_FAILURES )->SetWindowText( argumentString );
}

void
CTestRunnerDlg::UpdateUI_Time()
{
  CString argumentString;
  argumentString.Format( _T( "Execution time: %lu ms" ),
                         ( unsigned int )( m_testEndTime - m_testStartTime ) );
  GetDlgItem( IDC_EDIT_TIME )->SetWindowText( argumentString );
}

void
CTestRunnerDlg::OnStop()
{
  if ( UI_STATE_RUNING != m_uiState )
  {
    return;
  }

  SetUIState( UI_STATE_STOPPING );
  CUTEST_NS::Runner::instance()->stop();
}

void
CTestRunnerDlg::OnOK()
{
  SafeCloseDialog();
}

void
CTestRunnerDlg::OnSelectTestInHistoryCombo()
{
  unsigned int currentSelection = m_comboHistory.GetCurSel();

  if ( currentSelection >= 0  &&
       currentSelection < m_model.history().size() )
  {
    CPPUNIT_NS::Test *selectedTest = m_model.history()[currentSelection];
    m_model.selectHistoryTest( selectedTest );
    ResetHistoryCombo();
    saveSettings();
  }
}

void
CTestRunnerDlg::ResetHistoryCombo()
{
  m_comboHistory.LockWindowUpdate();

  m_comboHistory.ResetContent();

  const TestRunnerModel::History &history = m_model.history();
  for ( TestRunnerModel::History::const_iterator it = history.begin();
        it != history.end();
        ++it )
  {
    CPPUNIT_NS::Test *test = *it;
    m_comboHistory.AddString( CString( test->getName().c_str() ) );
  }

  if ( history.size() > 0 )
  {
    m_comboHistory.SetCurSel( 0 );
  }

  SetUIState( UI_STATE_NONE );

  m_comboHistory.UnlockWindowUpdate();
}

void
CTestRunnerDlg::OnBrowseTest()
{
  TreeHierarchyDlg dlg;
  dlg.setRootTest( m_model.rootTest() );
  if ( dlg.DoModal() == IDOK )
  {
    m_model.selectHistoryTest( dlg.getSelectedTest() );
    ResetHistoryCombo();
    saveSettings();
  }
}

BOOL
CTestRunnerDlg::PreTranslateMessage( MSG *pMsg )
{
  if ( ::TranslateAccelerator( m_hWnd,
                               m_hAccelerator,
                               pMsg ) )
  {
    return TRUE;
  }
  return cdxCDynamicDialog::PreTranslateMessage( pMsg );
}

void
CTestRunnerDlg::saveSettings()
{
  m_settings.autorun_on_startup = ( m_bAutorunAtStartup != 0 );
  m_settings.always_call_test_on_main_thread = CUTEST_NS::Runner::instance()->alwaysCallTestOnMainThread();
  m_settings.treat_timeout_as_error = CUTEST_NS::Runner::instance()->treatTimeoutAsError();
  StoreWindowPosition( TestRunnerModel::settingKey,
                       TestRunnerModel::settingMainDialogKey );

  m_settings.col_1 = m_listCtrl.GetColumnWidth( 0 );
  m_settings.col_2 = m_listCtrl.GetColumnWidth( 1 );
  m_settings.col_3 = m_listCtrl.GetColumnWidth( 2 );
  m_settings.col_4 = m_listCtrl.GetColumnWidth( 3 );

  m_model.saveSettings( m_settings );
}

void CTestRunnerDlg::OnQuitApplication()
{
  SafeCloseDialog();
}

void
CTestRunnerDlg::OnClose()
{
  SafeCloseDialog();
}

CRect
CTestRunnerDlg::getItemWindowRect( int nID )
{
  CWnd *pItem = GetDlgItem( nID );
  CRect rect;
  if ( pItem )
  {
    pItem->GetWindowRect( &rect );
  }
  return rect;
}

CRect
CTestRunnerDlg::getItemClientRect( int nID )
{
  CRect rect = getItemWindowRect( nID );
  if ( !rect.IsRectNull() )
  {
    CPoint clientTopLeft = rect.TopLeft();
    ScreenToClient( &clientTopLeft );
    rect = CRect( clientTopLeft, rect.Size() );
  }

  return rect;
}

void
CTestRunnerDlg::OnSize( UINT nType, int cx, int cy )
{
  cdxCDynamicDialog::OnSize( nType, cx, cy );
  updateListColumnSize();
}

void
CTestRunnerDlg::updateListColumnSize()
{
  if ( !m_listCtrl.GetSafeHwnd() )
  {
    return;
  }

  // resize to fit last column
  CRect listBounds = getItemClientRect( IDC_LIST );

  int width_1_4 = 0;
  for ( int i = 0; i < 4; ++i )
  {
    width_1_4 += m_listCtrl.GetColumnWidth( i );
  }

  // the 4 offset is so no horiz scroll bar will appear
  m_listCtrl.SetColumnWidth( 4, listBounds.Width() - width_1_4 - 4 );
}

void
CTestRunnerDlg::OnSelectedFailureChange( NMHDR *pNMHDR, LRESULT *pResult )
{
  NM_LISTVIEW *pNMListView = ( NM_LISTVIEW * )pNMHDR;

  if ( ( pNMListView->uNewState & LVIS_SELECTED ) != 0 ) // item selected
  {
    displayFailureDetailsFor( pNMListView->iItem );
  }

  *pResult = 0;
}

void
CTestRunnerDlg::displayFailureDetailsFor( unsigned int failureIndex )
{
  const CPPUNIT_NS::TestFailure *failure = CUTEST_NS::Runner::instance()->failureAt( failureIndex );

  CString details;
  if ( failure )
  {
    details = failure->thrownException()->what();
  }
  details.Replace( _T( "\n" ), _T( "\r\n" ) );

  m_details.SetWindowText( details );
}

void
CTestRunnerDlg::SetUIState( UIState state )
{
  m_uiState = state;

  switch ( state )
  {
  case UI_STATE_NONE:
    {
      CPPUNIT_NS::Test *rootTest = m_model.rootTest();
      if ( rootTest && rootTest->countTestCases() > 0 )
      {
        m_buttonBrowse.EnableWindow( TRUE );
      }
      else
      {
        m_buttonBrowse.EnableWindow( FALSE );
      }

      if ( CB_ERR == m_comboHistory.GetCurSel() )
      {
        m_buttonRun.EnableWindow( FALSE );
      }
      else
      {
        m_buttonRun.EnableWindow( TRUE );
      }

      m_buttonStop.EnableWindow( FALSE );
      m_buttonClose.EnableWindow( TRUE );
    }
    break;
  case UI_STATE_RUNING:
    {
      m_buttonBrowse.EnableWindow( FALSE );
      m_buttonRun.EnableWindow( FALSE );
      m_buttonStop.EnableWindow( TRUE );
      m_buttonClose.EnableWindow( FALSE );
    }
    break;
  default:
    {
      m_buttonBrowse.EnableWindow( FALSE );
      m_buttonRun.EnableWindow( FALSE );
      m_buttonStop.EnableWindow( FALSE );
      m_buttonClose.EnableWindow( FALSE );
    }
    break;
  }
}

void
CTestRunnerDlg::SafeCloseDialog()
{
  saveSettings();

  switch ( m_uiState )
  {
  case UI_STATE_NONE:
    EndDialog( IDOK );
    break;
  case UI_STATE_RUNING:
    {
      SetUIState( UI_STATE_CLOSING );
      CUTEST_NS::Runner::instance()->stop();
    }
    break;
  default:
    SetUIState( UI_STATE_CLOSING );
    break;
  }
}

void
CTestRunnerDlg::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
{
  cdxCDynamicDialog::OnMenuSelect( nItemID, nFlags, hSysMenu );

  CMenu *mainMenu = GetMenu();
  if ( mainMenu && mainMenu->GetSafeHmenu() )
  {
    UINT check = m_bAutorunAtStartup ? MF_CHECKED : MF_UNCHECKED;
    mainMenu->CheckMenuItem( ID_AUTORUN_AT_STARTUP, MF_BYCOMMAND | check );
    UINT enable = ( m_uiState == UI_STATE_NONE ) ? MF_ENABLED : MF_GRAYED;
    mainMenu->EnableMenuItem( ID_AUTORUN_AT_STARTUP, MF_BYCOMMAND | enable );

    check = CUTEST_NS::Runner::instance()->alwaysCallTestOnMainThread() ? MF_CHECKED : MF_UNCHECKED;
    mainMenu->CheckMenuItem( ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, MF_BYCOMMAND | check );
    mainMenu->EnableMenuItem( ID_ALWAYS_CALL_TEST_ON_MAIN_THREAD, MF_BYCOMMAND | enable );

    check = CUTEST_NS::Runner::instance()->treatTimeoutAsError() ? MF_CHECKED : MF_UNCHECKED;
    mainMenu->CheckMenuItem( ID_TREAT_TIMEOUT_AS_ERROR, MF_BYCOMMAND | check );
    mainMenu->EnableMenuItem( ID_TREAT_TIMEOUT_AS_ERROR, MF_BYCOMMAND | enable );
  }
}

void
CTestRunnerDlg::OnAutorunAtStartup()
{
  m_bAutorunAtStartup = !m_bAutorunAtStartup;
  saveSettings();
}

void
CTestRunnerDlg::OnAlwaysCallTestOnMainThread()
{
  CUTEST_NS::Runner::instance()->setAlwaysCallTestOnMainThread(
    !CUTEST_NS::Runner::instance()->alwaysCallTestOnMainThread()
  );
  saveSettings();
}

void
CTestRunnerDlg::OnTreatTimeoutAsError()
{
  CUTEST_NS::Runner::instance()->setTreatTimeoutAsError(
    !CUTEST_NS::Runner::instance()->treatTimeoutAsError()
  );
  saveSettings();
}