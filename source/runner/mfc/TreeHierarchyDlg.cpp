// TreeHierarchyDlg.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "TreeHierarchyDlg.h"
#include "TestRunnerModel.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TreeHierarchyDlg dialog
TreeHierarchyDlg::TreeHierarchyDlg( CWnd *pParent )
  : cdxCDynamicDialog( IDD_TEST_HIERARCHY, pParent )
  , m_selectedTest( NULL )
  , m_rootTest( NULL )
{
  ModifyFlags( flSWPCopyBits, 0 );      // anti-flickering option for resizing

  //{{AFX_DATA_INIT(TreeHierarchyDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void
TreeHierarchyDlg::DoDataExchange( CDataExchange *pDX )
{
  cdxCDynamicDialog::DoDataExchange( pDX );
  //{{AFX_DATA_MAP(TreeHierarchyDlg)
  DDX_Control( pDX, IDC_TREE_TEST, m_treeTests );
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( TreeHierarchyDlg, cdxCDynamicDialog )
  //{{AFX_MSG_MAP(TreeHierarchyDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
TreeHierarchyDlg::setRootTest( CPPUNIT_NS::Test *test )
{
  m_rootTest = test;
}

BOOL
TreeHierarchyDlg::OnInitDialog()
{
  cdxCDynamicDialog::OnInitDialog();

  fillTree();
  initializeLayout();

  return TRUE;
}

void
TreeHierarchyDlg::initializeLayout()
{
  // see DynamicWindow/doc for documentation
  AddSzControl( IDC_TREE_TEST, mdResize, mdResize );
  AddSzControl( IDOK, mdRepos, mdNone );
  AddSzControl( IDCANCEL, mdRepos, mdNone );
}

void
TreeHierarchyDlg::fillTree()
{
  VERIFY( m_imageList.Create( IDB_TEST_ICONS, 16, 1, RGB( 255, 0, 255 ) ) );

  m_treeTests.SetImageList( &m_imageList, TVSIL_NORMAL );

  HTREEITEM hSuite = addTest( m_rootTest, TVI_ROOT );
  m_treeTests.Expand( hSuite, TVE_EXPAND );
}

HTREEITEM
TreeHierarchyDlg::addTest( CPPUNIT_NS::Test *test,
                           HTREEITEM hParent )
{
  int testType = isSuite( test ) ? imgSuite : imgUnitTest;

  CString itemName = CString( test->getName().c_str() );
  if ( TVI_ROOT != hParent )
  {
    // 将Test名字中Suite.的部分精简掉
    // 比如："ExampleTestCase.testAdd"精简为"testAdd"
    CString prefix;
    prefix.Format( _T( "%s." ), ( LPCTSTR )m_treeTests.GetItemText( hParent ) );
    itemName.Replace( prefix, _T( "" ) );
  }

  HTREEITEM hItem = m_treeTests.InsertItem( itemName,
                    testType,
                    testType,
                    hParent );
  if ( hItem != NULL )
  {
    VERIFY( m_treeTests.SetItemData( hItem, ( DWORD_PTR )test ) );
    if ( isSuite( test ) )
    {
      addTestSuiteChildrenTo( test, hItem );
    }
  }
  return hItem;
}

void
TreeHierarchyDlg::addTestSuiteChildrenTo( CPPUNIT_NS::Test *suite,
    HTREEITEM hItemSuite )
{
  Tests tests;
  int childIndex = 0;
  for ( ; childIndex < suite->getChildTestCount(); ++childIndex )
  {
    tests.push_back( suite->getChildTestAt( childIndex ) );
  }
  sortByName( tests );

  for ( childIndex = 0; childIndex < suite->getChildTestCount(); ++childIndex )
  {
    addTest( suite->getChildTestAt( childIndex ), hItemSuite );
  }
}

bool
TreeHierarchyDlg::isSuite( CPPUNIT_NS::Test *test )
{
  return ( test->getChildTestCount() > 0  ||    // suite with test
           test->countTestCases() == 0 );       // empty suite
}

struct PredSortTest
{
  bool operator()( CPPUNIT_NS::Test *test1, CPPUNIT_NS::Test *test2 ) const
  {
    bool isTest1Suite = TreeHierarchyDlg::isSuite( test1 );
    bool isTest2Suite = TreeHierarchyDlg::isSuite( test2 );
    if ( isTest1Suite  &&  !isTest2Suite )
    {
      return true;
    }
    if ( isTest1Suite  &&  isTest2Suite )
    {
      return test1->getName() < test2->getName();
    }
    return false;
  }
};

void
TreeHierarchyDlg::sortByName( Tests &tests ) const
{
  std::stable_sort( tests.begin(), tests.end(), PredSortTest() );
}

void
TreeHierarchyDlg::OnOK()
{
  CPPUNIT_NS::Test *test = findSelectedTest();
  if ( test == NULL )
  {
    AfxMessageBox( IDS_ERROR_SELECT_TEST, MB_OK );
    return;
  }

  m_selectedTest = test;
  cdxCDynamicDialog::OnOK();
}

CPPUNIT_NS::Test *
TreeHierarchyDlg::findSelectedTest()
{
  HTREEITEM hItem = m_treeTests.GetSelectedItem();
  if ( hItem != NULL )
  {
    DWORD_PTR data;
    VERIFY( data = m_treeTests.GetItemData( hItem ) );
    return reinterpret_cast<CPPUNIT_NS::Test *>( data );
  }
  return NULL;
}

CPPUNIT_NS::Test *
TreeHierarchyDlg::getSelectedTest() const
{
  return m_selectedTest;
}
