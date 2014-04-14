#pragma once
#include "afxcmn.h"
#include "mscomm1.h"


// CLaserCtrlDlg dialog

class CLaserCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLaserCtrlDlg)

public:
	CLaserCtrlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLaserCtrlDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LASERCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CSliderCtrl m_slider;
	afx_msg void OnNMCustomdrawSliderPwr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetpwr();
	CMscomm1 m_cComm;
protected:
	virtual void OnCancel();
public:
	afx_msg void OnCbnSelchangeComboCom();
	afx_msg void OnBnClickedButtonTurnonoff();
	afx_msg void OnBnClickedButtonTurnonoff2();
};
