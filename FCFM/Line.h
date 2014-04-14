#pragma once
#include "afxwin.h"


// Line dialog

class Line : public CDialog
{
	DECLARE_DYNAMIC(Line)

public:
	Line(CWnd* pParent = NULL);   // standard constructor
	virtual ~Line();

// Dialog Data
	enum { IDD = IDD_DIALOG_LINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_staticbmpline;
	afx_msg void OnBnClickedButtonLine();
	CString m_A;
	CString m_B;
	CString m_C;
	CString m_D;
};
