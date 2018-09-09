#include "stdafx.h"
#include "AboutDlg.h"

// gtest
#include "gtest/gtest.h"

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAboutDlg::IDD, pParent)
{}

CAboutDlg::~CAboutDlg()
{}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDOK)->SetFocus();
	return FALSE;
}

void CAboutDlg::OnOK()
{
	EXPECT_FALSE(IsWindowVisible());
	CDialogEx::OnOK();
}
