#include "stdafx.h"
#include "about_dlg.h"

// gtest
#include "gtest/gtest.h"

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{}

CAboutDlg::~CAboutDlg()
{}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->SetFocus();
	return FALSE;
}

void CAboutDlg::OnOK()
{
	EXPECT_FALSE(IsWindowVisible());
	CDialog::OnOK();
}
