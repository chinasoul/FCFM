// FCFMDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "LaserCtrlDlg.h"
#include "Line.h"
#include "mscomm1.h"

// CFCFMDlg �Ի���
class CFCFMDlg : public CDialog
{
// ����
public:
	CFCFMDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FCFM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int bmpdirset;
	CString bmpdir;
	CString strTime;
	CString strPath;
	CString strPath1;
	SOCKADDR_IN addr_PC;
	SOCKADDR_IN addr_FPGA;

	CWinThread* HandleData0;
	CWinThread* HandleData1;
	CWinThread* RecvData;
	CWinThread* _512HandleData0;
	CWinThread* _512HandleData1;
	CWinThread* _512RecvData;

	CWinThread* contishot;
	CWinThread* contibmp;


	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonShot();
	afx_msg void OnBnClickedButtonContistart();
	afx_msg void OnOpen();
	afx_msg void OnShotpath();
	afx_msg void OnLaserctrl();
	afx_msg void OnAbout();
	CComboBox m_speed;
	CString szspeed;
	int i;

	CStatic m_staticbmp1;
	CStatic m_staticbmp2;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CLaserCtrlDlg* pwr;
	CStatusBar m_bar;
	HACCEL hAccTable;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLine();
	Line* line;
	afx_msg void OnBnClickedButton0xff();
	CMscomm1 m_cComm1;
	afx_msg void OnBnClickedButtonPwr();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CStatic m_staticbmp4;
	CStatic m_staticbmp3;
	CStatic m_staticbmp5;
	int c3;
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
};
