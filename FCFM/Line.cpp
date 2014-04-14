// Line.cpp : implementation file
//

#include "stdafx.h"
#include "FCFM.h"
#include "Line.h"

extern int line_a;
extern int line_b;
extern int line_c;
extern int line_d;
extern float line_k;
// Line dialog

IMPLEMENT_DYNAMIC(Line, CDialog)

Line::Line(CWnd* pParent /*=NULL*/)
	: CDialog(Line::IDD, pParent)
{

}

Line::~Line()
{
}

void Line::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LINE, m_staticbmpline);
}


BEGIN_MESSAGE_MAP(Line, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LINE, &Line::OnBnClickedButtonLine)
END_MESSAGE_MAP()


// Line message handlers

BOOL Line::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
	HBITMAP hBmp;
	hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 250, 250, LR_LOADMAP3DCOLORS);
	m_staticbmpline.SetBitmap(hBmp);
	CWnd::GetDlgItem(IDC_EDIT1)->SetWindowTextA(_T("20"));
	CWnd::GetDlgItem(IDC_EDIT2)->SetWindowTextA(_T("0"));
	CWnd::GetDlgItem(IDC_EDIT3)->SetWindowTextA(_T("180"));
	CWnd::GetDlgItem(IDC_EDIT4)->SetWindowTextA(_T("230"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Line::OnBnClickedButtonLine()
{
	// TODO: Add your control notification handler code here
	CWnd::GetDlgItem(IDC_EDIT1)->GetWindowTextA(m_A);
	CWnd::GetDlgItem(IDC_EDIT2)->GetWindowTextA(m_B);
	CWnd::GetDlgItem(IDC_EDIT3)->GetWindowTextA(m_C);
	CWnd::GetDlgItem(IDC_EDIT4)->GetWindowTextA(m_D);
	line_a=atoi(m_A);
	line_b=atoi(m_B);
	line_c=atoi(m_C);
	line_d=atoi(m_D);
	line_k=(float)((line_d - line_b)/(line_c - line_a));
}
