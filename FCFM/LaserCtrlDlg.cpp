// LaserCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FCFM.h"
#include "LaserCtrlDlg.h"


// CLaserCtrlDlg dialog

IMPLEMENT_DYNAMIC(CLaserCtrlDlg, CDialog)

CLaserCtrlDlg::CLaserCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLaserCtrlDlg::IDD, pParent)
{

}

CLaserCtrlDlg::~CLaserCtrlDlg()
{
}

void CLaserCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_PWR, m_slider);
	DDX_Control(pDX, IDC_MSCOMM1, m_cComm);
}


BEGIN_MESSAGE_MAP(CLaserCtrlDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PWR, &CLaserCtrlDlg::OnNMCustomdrawSliderPwr)
	ON_BN_CLICKED(IDC_BUTTON_SETPWR, &CLaserCtrlDlg::OnBnClickedButtonSetpwr)
	ON_CBN_SELCHANGE(IDC_COMBO_COM, &CLaserCtrlDlg::OnCbnSelchangeComboCom)
	ON_BN_CLICKED(IDC_BUTTON_TURNONOFF, &CLaserCtrlDlg::OnBnClickedButtonTurnonoff)
	ON_BN_CLICKED(IDC_BUTTON_TURNONOFF2, &CLaserCtrlDlg::OnBnClickedButtonTurnonoff2)
END_MESSAGE_MAP()


// CLaserCtrlDlg message handlers
	extern int hh;
BOOL CLaserCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//SetWindowSkin(m_hWnd, "Dialog");
	GetDlgItem(IDC_BUTTON_SETPWR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SLIDER_PWR)->EnableWindow(FALSE);
	HANDLE  hCom;
	int i,num,k;
	CString str;
	BOOL flag;
	((CComboBox *)GetDlgItem(IDC_COMBO_COM))->ResetContent();
	flag = FALSE;
	num = 0;
	for (i = 1;i <= 16;i++)
	{//�˳���֧��16������
		str.Format(_T("COM%d"),i);
		hCom = CreateFile(str, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(INVALID_HANDLE_VALUE != hCom )
		{//�ܴ򿪸ô��ڣ�����Ӹô���
			CloseHandle(hCom);
			//str = str.Mid(4);
			((CComboBox *)GetDlgItem(IDC_COMBO_COM))->AddString(str);
			if (flag == FALSE)
			{
				flag = TRUE;
				num = i;
			}
		}
	}
	i = ((CComboBox *)GetDlgItem(IDC_COMBO_COM))->GetCount();
	if(i == 0)
	{//���Ҳ������ô�������á��򿪴��ڡ�����
		GetDlgItem(IDC_COMBO_COM)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SETPWR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PWR)->EnableWindow(FALSE);
	}
	else
	{
		k = ((CComboBox *)GetDlgItem((IDC_COMBO_COM)))->GetCount();
		((CComboBox *)GetDlgItem(IDC_COMBO_COM))->SetCurSel(k - 1);
		//mCom.BindCommPort(num);
		m_slider.SetRange(1,70);	//0.1mw---7mw 0.1����
		m_slider.SetPageSize(10);
		m_slider.SetLineSize(1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLaserCtrlDlg::OnNMCustomdrawSliderPwr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	double iPos = m_slider.GetPos();
	double d = iPos/10;
	CString strMess;
	strMess.Format(_T("%.1f"),d);
	CWnd::GetDlgItem(IDC_EDIT_PWR)->SetWindowTextA(strMess);
	*pResult = 0;
}

void CLaserCtrlDlg::OnBnClickedButtonSetpwr()
{
	// TODO: Add your control notification handler code here
	CString m_strSendData;
	CWnd::GetDlgItem(IDC_EDIT_PWR)->GetWindowTextA(m_strSendData);
	double a = _tstof(m_strSendData);
	a /= 1000;
	if(a<0.0001 || a>0.007)
		AfxMessageBox(_T("���ʲ����޶���Χ��"));
	else
	{		
		m_strSendData.Format(_T("%f"),a);
		CString xxx = _T("source:power:level:immediate:amplitude ") + m_strSendData + _T("\n\r");
		m_cComm.put_Output(COleVariant(xxx));//��������
	}
}

void CLaserCtrlDlg::OnBnClickedButtonTurnonoff()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString s_on = _T("source:am:state on \n\r");
	m_cComm.put_Output(COleVariant(s_on));//��������
}

void CLaserCtrlDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_cComm.get_PortOpen())
		m_cComm.put_PortOpen(FALSE);
	CDialog::OnCancel();
}

void CLaserCtrlDlg::OnCbnSelchangeComboCom()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_SETPWR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PWR)->EnableWindow(TRUE);
	GetDlgItem(IDC_SLIDER_PWR)->EnableWindow(TRUE);
	CString error,data;
	int i;
	i = ((CComboBox *)GetDlgItem(IDC_COMBO_COM))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_COMBO_COM))->GetLBText(i,data);
	data = data.Mid(3);
	i = _ttoi(data);

	if(m_cComm.get_PortOpen()) //������ִ��ڱ����Ǵ򿪵ģ���رմ���
		m_cComm.put_PortOpen(FALSE);
	m_cComm.put_CommPort(i); //ѡ��COM��
	m_cComm.put_InputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_cComm.put_InBufferSize(1024); //�������뻺����
	m_cComm.put_OutBufferSize(512); //�������������
	m_cComm.put_Settings(TEXT("9600,n,8,1"));//�����ʣ���У�飬������λ����ֹͣλ
	if(!m_cComm.get_PortOpen())
	{
		m_cComm.put_PortOpen(TRUE); //�򿪴���
		m_cComm.put_RThreshold(1); //ÿ�����ջ������и��ַ�����մ�������
		m_cComm.put_InputLen(0); //���õ�ǰ����������Ϊ
		m_cComm.get_Input(); //Ԥ���������������������
	}
	else
		AfxMessageBox(_T("�򿪶˿�ʧ��!"),MB_ICONSTOP,0);

	//m_nOpenPortNum = i;
	//m_check_open_port = TRUE;
	//m_mscomm_Cctrl_com.SetCommPort(m_nOpenPortNum);
	//try
	//{
	//	m_mscomm_Cctrl_com.SetPortOpen(1);
	//}
	//catch(CException* e)
	//{
	//	MessageBox(_T("��ѡ��Ķ˿ڿ����ѱ�ռ�û򲻴���"),_T("���"),MB_OK);
	//	e->Delete();
	//	m_check_open_port  = FALSE;
	//}
	//UpdateData(0);
}

void CLaserCtrlDlg::OnBnClickedButtonTurnonoff2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString s_off = _T("source:am:state off \n\r");
	m_cComm.put_Output(COleVariant(s_off));//��������
}
