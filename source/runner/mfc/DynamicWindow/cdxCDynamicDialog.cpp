// cdxCDynamicChildDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cdxCDynamicDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cdxCDynamicDialog dialog
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(cdxCDynamicDialog,CDialogEx);

/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(cdxCDynamicDialog, CDialogEx)
	//{{AFX_MSG_MAP(cdxCDynamicDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_PARENTNOTIFY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cdxCDynamicDialog message handlers: redirect stuff to my class :)=
/////////////////////////////////////////////////////////////////////////////

BOOL cdxCDynamicDialog::OnInitDialog() 
{
	BOOL	bOK	=	CDialogEx::OnInitDialog();
	DoInitWindow(*this);

	return bOK;
}

BOOL cdxCDynamicDialog::DestroyWindow()
{
	DoOnDestroy();
	return CDialogEx::DestroyWindow();
}

void cdxCDynamicDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CDialogEx::OnGetMinMaxInfo(lpMMI);
	DoOnGetMinMaxInfo(lpMMI);
}

void cdxCDynamicDialog::OnDestroy() 
{
	DoOnDestroy();
	CDialogEx::OnDestroy();
}

void cdxCDynamicDialog::OnParentNotify(UINT message, LPARAM lParam) 
{
	CDialogEx::OnParentNotify(message, lParam);
	DoOnParentNotify(message, lParam);
}

void cdxCDynamicDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	DoOnSize(nType, cx, cy);
}

void cdxCDynamicDialog::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CDialogEx::OnSizing(fwSide, pRect);
	DoOnSizing(fwSide, pRect);
}

void cdxCDynamicDialog::OnTimer(UINT_PTR idEvent)
{
	CDialogEx::OnTimer(idEvent);
	DoOnTimer(idEvent);
}


/////////////////////////////////////////////////////////////////////////////
// cdxCDynamicChildDlg dialog
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(cdxCDynamicChildDlg,cdxCDynamicDialog);

/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////

