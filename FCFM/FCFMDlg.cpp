// FCFMDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FCFM.h"
#include "FCFMDlg.h"

#pragma comment(lib,"Iphlpapi.lib")	//getadaptersinfo�õ�
#include "Iphlpapi.h"
//#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int line_a=20;
int line_b=0;
int line_c=180;
int line_d=230;
float line_k=(float)((line_d-line_b)/(line_c-line_a));

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFCFMDlg �Ի���
HANDLE g_event;
HANDLE thr0_event;
HANDLE thr1_event;
SOCKET socket_PC;

//CFile myfile0;
//CFile myfile1;
//int myfile0_inuse = 0;
//int myfile1_inuse = 0;
int bmp0_save = 0;
int bmp1_save = 0;

//FILE* bmp0fp;
//FILE* bmp1fp;
unsigned char buf0[1029]={0};
unsigned char buf1[1029]={0};

unsigned char BMP_HEAD[1079]={0};
unsigned char BMP512_HEAD[1079]={0};

int startstop_flag = 0;	//���ݽ����߳�whileѭ����־
int thr_close = 0;	//���ݴ����߳�return��־�������ֹͣ�������˳�
int ETHERNET_flag = 0;

int t0_flag = 1;
int t1_flag = 1;
//int count0 = 0;
//int count1 = 0;

static int ss_status = 0;
static int laser_status = 0;
static int cts_status = 0;
static int fps = 0;
static int fps_h = 0;
int contistop = 0;	//������ͼ��־
int b0_flag = 0;	//0Ϊ��д��1Ϊ����д����ʼʱbuf0Ҫд��buf1��Ҫд
int b1_flag = 1;
int ofl_flag = 1;
int bhang = 0;
int len=sizeof(SOCKADDR_IN);

int status_1_2=0;

char rxd[1023][1029]={0};
char rxd2[1023][1029]={0};
int i1=0;
int i2=0;
FILE* bmpxxfp;
FILE* bmpyyfp;
//FILE* bmpxx_hfp;
//FILE* bmpyy_hfp;

char rxd_h[511][1029]={0};
char rxd2_h[511][1029]={0};
int i1_h=0;
int i2_h=0;

unsigned char buf_bk[1025] = {0};
unsigned char buf_h_bk[513] = {0};

static UINT indicators[] =
{
	IDS_TIME,
	IDS_FPS,
	IDS_RES,
	IDS_MESSAGE
};

CFCFMDlg::CFCFMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCFMDlg::IDD, pParent)
	, line(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFCFMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_BMP1, m_staticbmp1);
	DDX_Control(pDX, IDC_STATIC_BMP2, m_staticbmp2);
	DDX_Control(pDX, IDC_MSCOMM2, m_cComm1);
	DDX_Control(pDX, IDC_STATIC_bmp4, m_staticbmp4);
	DDX_Control(pDX, IDC_STATIC_LASER, m_staticbmp3);
	DDX_Control(pDX, IDC_STATIC_bmp5, m_staticbmp5);
}

BEGIN_MESSAGE_MAP(CFCFMDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CFCFMDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SHOT, &CFCFMDlg::OnBnClickedButtonShot)
	ON_BN_CLICKED(IDC_BUTTON_CONTISTART, &CFCFMDlg::OnBnClickedButtonContistart)
	ON_COMMAND(ID_OPEN, &CFCFMDlg::OnOpen)
	ON_COMMAND(ID_SHOTPATH, &CFCFMDlg::OnShotpath)
	ON_COMMAND(ID_LASERCTRL, &CFCFMDlg::OnLaserctrl)
	ON_COMMAND(ID_ABOUT, &CFCFMDlg::OnAbout)
	ON_WM_TIMER()
	ON_COMMAND(ID_LINE, &CFCFMDlg::OnLine)
	ON_BN_CLICKED(IDC_BUTTON_0xff, &CFCFMDlg::OnBnClickedButton0xff)
	ON_BN_CLICKED(IDC_BUTTON_PWR, &CFCFMDlg::OnBnClickedButtonPwr)
	ON_BN_CLICKED(IDC_RADIO1, &CFCFMDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CFCFMDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CFCFMDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CFCFMDlg::OnBnClickedRadio4)
END_MESSAGE_MAP()


// CFCFMDlg ��Ϣ�������
BOOL CFCFMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowSkin(m_hWnd, "Dialog");
	SetDialogSkin("Dialog");
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	hAccTable=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));

	m_bar.Create(this);
	m_bar.SetIndicators(indicators,4);
	CRect rect;
	GetClientRect(&rect);
	m_bar.SetPaneInfo(0,IDS_TIME, SBPS_NORMAL, rect.Width()/4);
	m_bar.SetPaneInfo(1,IDS_FPS, SBPS_NORMAL ,rect.Width()/4);
	m_bar.SetPaneInfo(2,IDS_RES, SBPS_NORMAL ,rect.Width()/4);
	m_bar.SetPaneInfo(3,IDS_MESSAGE, SBPS_STRETCH, 0);
	//RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, IDS_TIME);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0);
	SetTimer(0,1000,NULL);
	SetTimer(1,1000,NULL);

	CTime t;
	t=CTime::GetTickCount();
	m_bar.SetPaneText(0,t.Format(_T("ʱ��:%Y-%m-%d %H:%M:%S")));
	m_bar.SetPaneText(1, _T("֡Ƶ:8fps"));
	m_bar.SetPaneText(2,_T("ͼ����ʾ�ֱ���:1024 x 1024"));
	m_bar.SetPaneText(3,_T("̽ͷ���:U-240"));

	//CString ipaddr;
	//CString ipaddr_nac = _T("0.0.0.0");
	//CString ipaddr_ac = _T("192.168.1");
	//PIP_ADAPTER_INFO pAdapterInfo;
 //   PIP_ADAPTER_INFO pAdapter = NULL;
 //   DWORD dwRetVal = 0;

 //   ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
 //   pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

	//if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	//{
 //       FREE(pAdapterInfo);
 //       pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
 //       if (pAdapterInfo == NULL)
	//	{
 //           AfxMessageBox(_T("Error allocating memory needed to call GetAdaptersInfo"));
 //           exit(-1);
 //       }
 //   }

	//if((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	//{
 //       pAdapter = pAdapterInfo;
	//	while(pAdapter)
	//	{
	//		if(pAdapter->Type == MIB_IF_TYPE_ETHERNET)	//IF_TYPE_IEEE80211 == 71
	//		{
	//			ETHERNET_flag = 1;
	//			ipaddr = pAdapter->IpAddressList.IpAddress.String;
	//			pAdapter = pAdapter->Next;
	//		}
	//		else
	//			pAdapter = pAdapter->Next;
	//	}
 //   }
	//if(pAdapterInfo)
	//	FREE(pAdapterInfo);



	////if(ETHERNET_flag == 1)
	////{
	////	if(ipaddr.Left(8) == ipaddr_nac)
	////	{
	////		AfxMessageBox(_T("�������ڷǻ״̬\n�����������Ӻ͵�Դ"));
	////		exit(-1);
	////	}
	////	else if(ipaddr.Left(9) != ipaddr_ac)
	////	{
	////		AfxMessageBox(_T("IP��ַ��������\n���ֶ�����IP"));
	////		exit(-1);
	////	}
	////	/*else
	////		AfxMessageBox(_T("Ethernet card ready"), MB_ICONINFORMATION);*/
	////}
	////else
	////{
	////	AfxMessageBox(_T("����������\n������"), MB_ICONSTOP);
	////	exit(-1);
	////}



	//bmpdirset = 0;
	//char szhostname[128];
	//CString s_pcip,s_pcport;
	//if(gethostname(szhostname,128)==0)
	//{
	//	struct hostent *phost;
	//	phost=gethostbyname(szhostname);
	//	for(int j=0;j<4;j++)
	//	{
	//		CString addr;
	//		if(j>0)
	//			s_pcip+=".";
	//			addr.Format(_T("%u"),(unsigned int)((unsigned char*)phost->h_addr_list[0])[j]);
	//			s_pcip+=addr;
	//	}
	//}

	//HINSTANCE se = ::ShellExecute(this->m_hWnd, _T("open"), _T("cmd.exe"), _T("/c arp -s 192.168.1.200 6e-b8-c0-02-1e-03"), NULL, SW_HIDE);
	//if((int)se < 32)
	//	AfxMessageBox(_T("ARP bind error"));

	//GetDlgItem(IDC_EDIT_PCIP)->SetWindowText(s_pcip);
	//GetDlgItem(IDC_EDIT_PCPORT)->SetWindowText(_T("33214"));
	//GetDlgItem(IDC_EDIT_PCPORT)->GetWindowText(s_pcport);
	//int port = atoi((const char*)s_pcport.GetBuffer(1));

	//socket_PC = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	//addr_PC.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//addr_PC.sin_family = AF_INET;
	//addr_PC.sin_port = htons(port);

	//addr_FPGA.sin_addr.S_un.S_addr=inet_addr("192.168.1.200");
	//addr_FPGA.sin_family=AF_INET;
	//addr_FPGA.sin_port=htons(4426);

	//int nRecvBuf=30*1029;
	//setsockopt(socket_PC, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

	//int nNetTimeout_command=100;
	//setsockopt(socket_PC, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout_command, sizeof(int));

	//int len = sizeof(SOCKADDR_IN);
	//if(bind(socket_PC, (SOCKADDR*)&addr_PC, len) == SOCKET_ERROR)
	//{
	//	AfxMessageBox(_T("Bind Socket Error!"));
	//	exit(-1);
	//}

	GetDlgItem(IDC_EDIT_PWR)->SetWindowText(_T("5"));

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_0xff)->EnableWindow(TRUE);

	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);	//Ĭ��ѡ��1024 x 1024
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);

	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	BMP_HEAD[0]=0x42;
	BMP_HEAD[1]=0x4D;

	BMP_HEAD[2]=0x36;
	BMP_HEAD[3]=0x04;
	BMP_HEAD[4]=0x10;
	BMP_HEAD[5]=0x00;

	BMP_HEAD[6]=0x00;
	BMP_HEAD[7]=0x00;
	BMP_HEAD[8]=0x00;
	BMP_HEAD[9]=0x00;

	BMP_HEAD[10]=0x36;
	BMP_HEAD[11]=0x04;
	BMP_HEAD[12]=0x00;
	BMP_HEAD[13]=0x00;
    //infohead 40B
	BMP_HEAD[14]=0x28;
	BMP_HEAD[15]=0x00;
	BMP_HEAD[16]=0x00;
	BMP_HEAD[17]=0x00;
	//width
	BMP_HEAD[18]=0x00;
	BMP_HEAD[19]=0x04;
    BMP_HEAD[20]=0x00;
	BMP_HEAD[21]=0x00;
	//height
	BMP_HEAD[22]=0x00;
    BMP_HEAD[23]=0x04;
    BMP_HEAD[24]=0x00;
	BMP_HEAD[25]=0x00;
	//page
	BMP_HEAD[26]=0x01;
    BMP_HEAD[27]=0x00;
	//byte per pix
    BMP_HEAD[28]=0x08;
    BMP_HEAD[29]=0x00;
    //
	BMP_HEAD[30]=0x00;
    BMP_HEAD[31]=0x00;
	BMP_HEAD[32]=0x00;
    BMP_HEAD[33]=0x00;

	//size of pic
	BMP_HEAD[34]=0x00;
    BMP_HEAD[35]=0x00;
	BMP_HEAD[36]=0x10;
    BMP_HEAD[37]=0x00;

	BMP_HEAD[38]=0x00;
    BMP_HEAD[39]=0x00;
	BMP_HEAD[40]=0x00;
    BMP_HEAD[41]=0x00;
    //
	BMP_HEAD[42]=0x00;
    BMP_HEAD[43]=0x00;
	BMP_HEAD[44]=0x00;
    BMP_HEAD[45]=0x00;
    
	BMP_HEAD[46]=0x00;
    BMP_HEAD[47]=0x00;
	BMP_HEAD[48]=0x00;
    BMP_HEAD[49]=0x00;

	BMP_HEAD[50]=0x00;
    BMP_HEAD[51]=0x00;
	BMP_HEAD[52]=0x00;
    BMP_HEAD[53]=0x00;

	for(UINT i=0;i<256;i++)
	{   
		int j=i*4;
		BMP_HEAD[54+j]=i;
        BMP_HEAD[54+j+1]=i;
		BMP_HEAD[54+j+2]=i;
		BMP_HEAD[54+j+3]=0;
	}
	::memcpy(BMP512_HEAD, BMP_HEAD, 1079);	//512x512�ļ�ͷ
	BMP512_HEAD[4]=0x04;
	BMP512_HEAD[19]=0x02;
	BMP512_HEAD[23]=0x02;
	BMP512_HEAD[36]=0x04;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}





void CFCFMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFCFMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFCFMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT ThreadHandleData0(LPVOID ThreadParam)
{
	CFCFMDlg* pd0=(CFCFMDlg*)ThreadParam;
	//unsigned char buf0_t[1029] = {0};
	//unsigned char buf0_out[1025] = {0};
	unsigned char rxd_out[1025]={0};
	//unsigned int  rxd_out[1025]={0};
	
	while(1)
	{
		::WaitForSingleObject(thr0_event, INFINITE);
		t0_flag = 0;
		//count0 = 0;
		//myfile0.SeekToBegin();

		bmp0_save = 0;
		//fopen_s(&bmp0fp, "C:\\0.bmp", "wb+");
		//fwrite(BMP_HEAD,1,1078,bmp0fp);

		i1=0;
		fopen_s(&bmpxxfp, "D:\\xx.bmp", "wb+");
		fwrite(BMP_HEAD,1,1078,bmpxxfp);
		for(int x=0; x<1023; x++)
		{
			for(int c=0,v=4;c<1024;c++,v++)
			{
				rxd_out[c]=~rxd[x][v];
				rxd_out[c]-=120;
				if(rxd_out[c]<0)
					rxd_out[c]=0;
				rxd_out[c]*=2;
				//rxd_out[c]=255-rxd_out[c];
				//if( (x-511)*(x-511)+(c-511)*(c-511)>150000 )
				//	rxd_out[c]=0;

				if(rxd_out[c]<=line_a)
					rxd_out[c]=line_b*rxd_out[c]/line_a;
				else if(rxd_out[c]>line_a && rxd_out[c]<=line_c)
					rxd_out[c]=line_k*(rxd_out[c]-line_a)+line_b;
				else
					rxd_out[c]=(255-line_d)*(rxd_out[c]-line_c)/(255-line_c)+line_d;
			}
			fwrite(rxd_out,1,1024,bmpxxfp);
		}
		fwrite(buf_bk,1,1024,bmpxxfp);
		fclose(bmpxxfp);
		
		
		
		//myfile0_inuse = 1;
		//for(int a=0; a<1023; a++)
		//{
		//	myfile0.Read(buf0_t,1028);
		//	myfile0.Seek(0,CFile::current);
		//	for(int k=0,b=4;k<1024;k++,b++)	//loc:(a,k)  r=400pix
		//	{
		//		//buf0_out[k]=~buf0_t[b];
		//		buf0_out[k]=buf0_t[b];
		//		buf0_out[k]-=135;
		//		if(buf0_out[k]<0)
		//			buf0_out[k] = 0;
		//		buf0_out[k]*=2;
		//		buf0_out[k]=255-buf0_out[k];
		//	}
		//	fwrite(buf0_out,1,1024,bmp0fp);
		//}

		//fwrite(buf0_bk,1,1024,bmp0fp);

		//fclose(bmp0fp);

		bmp0_save = 1;

		PulseEvent(g_event);

		if(fps == 4)
			fps = 0;
		fps++;
		
		//myfile0.SetLength(0);
		//myfile0_inuse = 0;
		HBITMAP hBmp0 = (HBITMAP)::LoadImage(0, _T("D:\\xx.bmp"), IMAGE_BITMAP, 600, 600, LR_LOADFROMFILE);
		pd0->m_staticbmp1.SetBitmap(hBmp0);
		HBITMAP hBmp_bc;
		
		if(pd0->c3)
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd0->m_staticbmp4.SetBitmap(hBmp_bc);
		}
		else
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd0->m_staticbmp5.SetBitmap(hBmp_bc);
		}

		DeleteObject(hBmp0);

		t0_flag = 1;
		ResetEvent(thr0_event);
		if(thr_close)
			return 0;
	}
}

UINT ThreadHandleData1(LPVOID ThreadParam)
{
	CFCFMDlg* pd1=(CFCFMDlg*)ThreadParam;
	//unsigned char buf1_t[1029]={0};
	//unsigned char buf1_out[1025]={0};
	unsigned char rxd2_out[1025]={0};

	while(1)
	{
		::WaitForSingleObject(thr1_event, INFINITE);
		t1_flag = 0;
		//count1 = 0;
		bmp1_save = 0;
		//fopen_s(&bmp1fp, "C:\\1.bmp", "wb+");
		//fwrite(BMP_HEAD,1,1078,bmp1fp);



		i2=0;
		fopen_s(&bmpyyfp, "D:\\yy.bmp", "wb+");
		fwrite(BMP_HEAD,1,1078,bmpyyfp);
		for(int y=1023; y>0; y--)
		{
			for(int c2=0,v2=4;c2<1024;c2++,v2++)
			{
				rxd2_out[c2]=~rxd2[y][v2];
				rxd2_out[c2]-=120;
				if(rxd2_out[c2]<0)
					rxd2_out[c2]=0;
				rxd2_out[c2]*=2;
				//rxd2_out[c2]=255-rxd2_out[c2];
				//if( (y-511)*(y-511)+(c2-511)*(c2-511)>150000 )
				//	rxd2_out[c2]=0;

				if(rxd2_out[c2]<=line_a)
					rxd2_out[c2]=line_b*rxd2_out[c2]/line_a;
				else if(rxd2_out[c2]>line_a && rxd2_out[c2]<=line_c)
					rxd2_out[c2]=line_k*(rxd2_out[c2]-line_a)+line_b;
				else
					rxd2_out[c2]=(255-line_d)*(rxd2_out[c2]-line_c)/(255-line_c)+line_d;
			}
			fwrite(rxd2_out,1,1024,bmpyyfp);
		}
		fwrite(buf_bk,1,1024,bmpyyfp);
		fclose(bmpyyfp);


		//fwrite(buf1_bk,1,1024,bmp1fp);

		//myfile1_inuse = 1;
		//myfile1.SeekToEnd();
		//for(int a=0; a<1023; a++)
		//{
		//	myfile1.Seek(-1028, CFile::current);	//		myfile1.Seek(-1028, CFile::current);
		//	myfile1.Read(buf1_t,1028);
		//	for(int k=0,b=4;k<1024;k++,b++)
		//	{
		//		//buf1_out[k]=~buf1_t[b];
		//		buf1_out[k]=buf1_t[b];
		//		buf1_out[k]-=135;
		//		if(buf1_out[k]<0)
		//			buf1_out[k] = 0;
		//		buf1_out[k]*=2;
		//		buf1_out[k]=255-buf1_out[k];
		//	}
		//	fwrite(buf1_out,1,1024,bmp1fp);
		//	myfile1.Seek(-1028, CFile::current);
		//}

		//fclose(bmp1fp);
		bmp1_save = 1;

		PulseEvent(g_event);

		if(fps == 4)
			fps = 0;
		fps++;

		//myfile1.SetLength(0);
		//myfile1_inuse = 0;
		HBITMAP hBmp1 = (HBITMAP)::LoadImage(0, _T("D:\\yy.bmp"), IMAGE_BITMAP, 600, 600, LR_LOADFROMFILE);
		pd1->m_staticbmp1.SetBitmap(hBmp1);
		HBITMAP hBmp_bc;
		if(pd1->c3)
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd1->m_staticbmp4.SetBitmap(hBmp_bc);
		}
		else
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd1->m_staticbmp5.SetBitmap(hBmp_bc);
		}

		DeleteObject(hBmp1);

		t1_flag = 1;
		ResetEvent(thr1_event);
		if(thr_close)
			return 0;
	}
}

UINT ThreadReceiveData(LPVOID ThreadParam)
{
	CFCFMDlg* pd = (CFCFMDlg*)ThreadParam;
	while(startstop_flag)
	{
		if(!b0_flag)
		{
			if(!ofl_flag)
			{
				bhang = buf1[2]*256 + buf1[3] + 1;
				//count0 = bhang;
				while(bhang--)
				{
					//myfile0.Write(buf1,1028);
					//myfile0.SeekToEnd();
					::memcpy(&rxd[i1], buf1, 1029);
					i1++;
				}
				ofl_flag = 1;
			}

			recvfrom(socket_PC, (char*)buf0, 1029, 0, (SOCKADDR*)&pd->addr_FPGA, &len);
			//recvfrom(socket_PC, (char*)buf0, 1029, 0, (SOCKADDR*)&addr_FPGAa, &len);
			if(buf0[1] == 0x00)
			{
				//if(out0_oc == 1)
				//myfile0.Write(buf0,1028);
				//myfile0.SeekToEnd();
				//count0++;
				::memcpy(&rxd[i1], buf0, 1029);
				i1++;
			}
			else if(buf0[1] == 0x01)
			{
				b0_flag = 1;
				b1_flag = 0;
				ofl_flag = 0;
				//myfile0.Close();
				if(t0_flag && (i1==1023))   // && i1==1023
					SetEvent(thr0_event);
				else
				{
					//count0 = 0;
					i1=0;
					//if(myfile0_inuse != 1)
						//myfile0.SetLength(0);
				}
			}
		}
		
		if(!b1_flag)
		{
			if(!ofl_flag)
			{
				bhang = buf0[2]*256 + buf0[3] + 1;
				//count1 = bhang;
				while(bhang--)
				{
					//if(out1_oc == 1)
					//myfile1.Write(buf0,1028);
					//myfile1.SeekToEnd();
					::memcpy(&rxd2[i2], buf0, 1029);
					i2++;
				}
				ofl_flag = 1;
			}

			recvfrom(socket_PC, (char*)buf1, 1029, 0, (SOCKADDR*)&pd->addr_FPGA, &len);
			//recvfrom(socket_PC, (char*)buf1, 1029, 0, (SOCKADDR*)&addr_FPGAa, &len);
			if(buf1[1] == 0x01)
			{
				//if(out1_oc == 1)
				//myfile1.Write(buf1,1028);
				//myfile1.SeekToEnd();
				//count1++;
				::memcpy(&rxd2[i2], buf1, 1029);
				i2++;
			}
			else if(buf1[1] == 0x00)
			{
				b0_flag = 0;
				b1_flag = 1;
				ofl_flag = 0;
				//myfile1.Close();
				if(t1_flag && (i2==1023))
					SetEvent(thr1_event);
				else
				{
					//count1 = 0;
					i2=0;
					//if(myfile1_inuse != 1)
						//myfile1.SetLength(0);
				}
			}
		}
	}
	return 0;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT _512ThreadHandleData0(LPVOID ThreadParam)
{
	CFCFMDlg* pd0=(CFCFMDlg*)ThreadParam;
	//unsigned char buf0_t[1029]={0};
	//unsigned char buf0_out[513]={0};
	//unsigned char buf0_bk[513]={0};
	//memset(buf0_bk,0x00,512);
	unsigned char rxd_h_out[513]={0};
	while(1)
	{
		::WaitForSingleObject(thr0_event, INFINITE);
		t0_flag = 0;
		//count0 = 0;
		//myfile0.SeekToBegin();

		bmp0_save = 0;
		//fopen_s(&bmp0fp, "C:\\0.bmp", "wb+");
		//fwrite(BMP512_HEAD,1,1078,bmp0fp);

		
		i1_h=0;
		fopen_s(&bmpxxfp, "D:\\xx.bmp", "wb+");
		fwrite(BMP512_HEAD,1,1078,bmpxxfp);
		for(int x=0; x<511; x++)
		{
			for(int c=0,v=5;c<512;c++,v+=2)
			{
				rxd_h_out[c]=~rxd_h[x][v];
				rxd_h_out[c]-=120;
				if(rxd_h_out[c]<0)
					rxd_h_out[c]=0;
				rxd_h_out[c]*=2;
				//rxd_h_out[c]=255-rxd_h_out[c];

				if(rxd_h_out[c]<=line_a)
					rxd_h_out[c]=line_b*rxd_h_out[c]/line_a;
				else if(rxd_h_out[c]>line_a && rxd_h_out[c]<=line_c)
					rxd_h_out[c]=line_k*(rxd_h_out[c]-line_a)+line_b;
				else
					rxd_h_out[c]=(255-line_d)*(rxd_h_out[c]-line_c)/(255-line_c)+line_d;
			}
			fwrite(rxd_h_out,1,512,bmpxxfp);
		}
		fwrite(buf_h_bk,1,512,bmpxxfp);
		fclose(bmpxxfp);


		//myfile0_inuse = 1;
		//for(int a=0; a<2046; a++)
		//{
		//	myfile0.Read(buf0_t,1028);
		//	myfile0.Seek(0,CFile::current);
		//	for(int k=0,b=5;k<512;k++,b+=2)
		//		buf0_out[k]=buf0_t[b];
		//	fwrite(buf0_out,1,512,bmp0fp);
		//}
		//for(int a=0; a<511; a++)
		//{
		//	myfile0.Read(buf0_t,1028);
		//	myfile0.Seek(0,CFile::current);
		//	for(int k=0,b=5;k<512;k++,b+=2)
		//	{
		//		buf0_out[k]=buf0_t[b];
		//		buf0_out[k]-=135;
		//		if(buf0_out[k]<0)
		//			buf0_out[k] = 0;
		//		buf0_out[k]*=2;
		//		buf0_out[k]=255-buf0_out[k];
		//	}
		//	fwrite(buf0_out,1,512,bmp0fp);
		//}

		//fwrite(buf0_bk,1,512,bmp0fp);

		//fclose(bmp0fp);

		bmp0_save = 1;

		PulseEvent(g_event);

		if(fps_h == 8)
			fps_h = 0;
		fps_h++;
		
		//myfile0.SetLength(0);
		//myfile0_inuse = 0;
		HBITMAP hBmp0 = (HBITMAP)::LoadImage(0, _T("D:\\xx.bmp"), IMAGE_BITMAP, 600, 600, LR_LOADFROMFILE);
		pd0->m_staticbmp1.SetBitmap(hBmp0);
		HBITMAP hBmp_bc;
		if(pd0->c3)
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd0->m_staticbmp4.SetBitmap(hBmp_bc);
		}
		else
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd0->m_staticbmp5.SetBitmap(hBmp_bc);
		}

		DeleteObject(hBmp0);

		t0_flag = 1;
		ResetEvent(thr0_event);
		if(thr_close)
			return 0;
	}
}

UINT _512ThreadHandleData1(LPVOID ThreadParam)
{
	CFCFMDlg* pd1=(CFCFMDlg*)ThreadParam;
	//unsigned char buf1_t[1029]={0};
	//unsigned char buf1_out[513]={0};
	//unsigned char buf1_bk[513]={0};
	//memset(buf1_bk,0x00,512);
	unsigned char rxd2_h_out[513]={0};
	while(1)
	{
		::WaitForSingleObject(thr1_event, INFINITE);
		t1_flag = 0;
		//count1 = 0;
		bmp1_save = 0;
		//fopen_s(&bmp1fp, "C:\\1.bmp", "wb+");
		//fwrite(BMP512_HEAD,1,1078,bmp1fp);


		i2_h=0;
		fopen_s(&bmpyyfp, "D:\\yy.bmp", "wb+");
		fwrite(BMP512_HEAD,1,1078,bmpyyfp);
		for(int x=511; x>0; x--)
		{
			for(int c=0,v=5;c<512;c++,v+=2)
			{
				rxd2_h_out[c]=~rxd2_h[x][v];
				rxd2_h_out[c]-=120;
				if(rxd2_h_out[c]<0)
					rxd2_h_out[c]=0;
				rxd2_h_out[c]*=2;
				//rxd2_h_out[c]=255-rxd2_h_out[c];
				if(rxd2_h_out[c]<=line_a)
					rxd2_h_out[c]=line_b*rxd2_h_out[c]/line_a;
				else if(rxd2_h_out[c]>line_a && rxd2_h_out[c]<=line_c)
					rxd2_h_out[c]=line_k*(rxd2_h_out[c]-line_a)+line_b;
				else
					rxd2_h_out[c]=(255-line_d)*(rxd2_h_out[c]-line_c)/(255-line_c)+line_d;
			}
			fwrite(rxd2_h_out,1,512,bmpyyfp);
		}
		fwrite(buf_h_bk,1,512,bmpyyfp);
		fclose(bmpyyfp);



		//fwrite(buf1_bk,1,512,bmp1fp);
		//fwrite(buf1_bk,1,512,bmp1fp);
		//myfile1_inuse = 1;
		//myfile1.SeekToEnd();
	//	for(int a=0; a<2046; a++)
	//	{
	//		myfile1.Seek(-1028, CFile::current);	//		myfile1.Seek(-1028, CFile::current);
	//		myfile1.Read(buf1_t,1028);
	//		for(int k=0,b=5;k<512;k++,b+=2)
	//			buf1_out[k]=buf1_t[b]; 
	//		if(a%2 == 0)
	//			memcpy(buf1_out_tmp, buf1_out, 512);
	//		//if(a%2 == 1)
	//		else
	//		{
	//			fwrite(buf1_out,1,512,bmp1fp);
	//			fwrite(buf1_out_tmp,1,512,bmp1fp);
	//		}
	//		myfile1.Seek(-1028, CFile::current);
	//	}
		//for(int a=0; a<511; a++)
		//{
		//	myfile1.Seek(-1028, CFile::current);	//		myfile1.Seek(-1028, CFile::current);
		//	myfile1.Read(buf1_t,1028);
		//	for(int k=0,b=5;k<512;k++,b+=2)
		//	{
		//		buf1_out[k]=buf1_t[b];
		//		buf1_out[k]-=135;
		//		if(buf1_out[k]<0)
		//			buf1_out[k] = 0;
		//		buf1_out[k]*=2;
		//		buf1_out[k]=255-buf1_out[k];
		//	}
		//	fwrite(buf1_out,1,512,bmp1fp);
		//	myfile1.Seek(-1028, CFile::current);
		//}
		//fclose(bmp1fp);
		bmp1_save = 1;

		PulseEvent(g_event);

		if(fps_h == 8)
			fps_h = 0;
		fps_h++;

		//myfile1.SetLength(0);
		//myfile1_inuse = 0;
		HBITMAP hBmp1 = (HBITMAP)::LoadImage(0, _T("D:\\yy.bmp"), IMAGE_BITMAP, 600, 600, LR_LOADFROMFILE);
		pd1->m_staticbmp1.SetBitmap(hBmp1);
		HBITMAP hBmp_bc;
		if(pd1->c3)
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd1->m_staticbmp4.SetBitmap(hBmp_bc);
		}
		else
		{
			hBmp_bc = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 40, 40, LR_LOADMAP3DCOLORS);
			pd1->m_staticbmp5.SetBitmap(hBmp_bc);
		}
		DeleteObject(hBmp1);

		t1_flag = 1;
		ResetEvent(thr1_event);
		if(thr_close)
			return 0;
	}
	return 0;
}

UINT _512ThreadReceiveData(LPVOID ThreadParam)
{
	CFCFMDlg* pd = (CFCFMDlg*)ThreadParam;
	while(startstop_flag)
	{
		if(!b0_flag)
		{
			if(!ofl_flag)
			{
				bhang = buf1[2]*256 + buf1[3] + 1;
				//count0 = bhang;
				while(bhang--)
				{
					//myfile0.Write(buf1,1028);
					//myfile0.SeekToEnd();
					::memcpy(&rxd_h[i1_h], buf1, 1029);
					i1_h++;
				}
				ofl_flag = 1;
			}

			recvfrom(socket_PC, (char*)buf0, 1029, 0, (SOCKADDR*)&pd->addr_FPGA, &len);
			//recvfrom(socket_PC, (char*)buf0, 1029, 0, (SOCKADDR*)&addr_FPGAa, &len);
			if(buf0[1] == 0x10)
			{
				//if(out0_oc == 1)
				//myfile0.Write(buf0,1028);
				//myfile0.SeekToEnd();
				//count0++;
				::memcpy(&rxd_h[i1_h], buf0, 1029);
				i1_h++;
			}
			else if(buf0[1] == 0x11)
			{
				b0_flag = 1;
				b1_flag = 0;
				ofl_flag = 0;
				//myfile0.Close();
				if((i1_h == 511) && t0_flag)
					SetEvent(thr0_event);
				else
				{
					//count0 = 0;
					i1_h=0;
					//if(myfile0_inuse != 1)
						//myfile0.SetLength(0);
				}
			}
		}
		
		if(!b1_flag)
		{
			if(!ofl_flag)
			{
				bhang = buf0[2]*256 + buf0[3] + 1;
				//count1 = bhang;
				while(bhang--)
				{
					//if(out1_oc == 1)
					//myfile1.Write(buf0,1028);
					//myfile1.SeekToEnd();
					::memcpy(&rxd2_h[i2_h], buf0, 1029);
					i2_h++;
				}
				ofl_flag = 1;
			}

			recvfrom(socket_PC, (char*)buf1, 1029, 0, (SOCKADDR*)&pd->addr_FPGA, &len);
			//recvfrom(socket_PC, (char*)buf1, 1029, 0, (SOCKADDR*)&addr_FPGAa, &len);
			if(buf1[1] == 0x11)
			{
				//if(out1_oc == 1)
				//myfile1.Write(buf1,1028);
				//myfile1.SeekToEnd();
				//count1++;
				::memcpy(&rxd2_h[i2_h], buf1, 1029);
				i2_h++;
			}
			else if(buf1[1] == 0x10)
			{
				b0_flag = 0;
				b1_flag = 1;
				ofl_flag = 0;
				//myfile1.Close();
				if((i2_h == 511) && t1_flag)
					SetEvent(thr1_event);
				else
				{
					//count1 = 0;
					i2_h=0;
					//if(myfile1_inuse != 1)
						//myfile1.SetLength(0);
				}
			}
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CFCFMDlg::OnBnClickedButtonStart()
{
	int c1 = ((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();	//����ֵΪ 1 ���ʾ��ѡ��
	int c2 = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck();

	c3 = ((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck();	//����ֵΪ 1 ���ʾ��ѡ��
	//int c2 = ((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck();

	if(!ss_status)
	{
		GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PCIP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PCPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextA(_T("ֹͣ�ɼ�"));
		CTime t;
		t=CTime::GetTickCount();
		m_bar.SetPaneText(3,t.Format(_T("��ʼʱ�� %H:%M:%S")));
		memset(buf0, 0, 1029);
		memset(buf1, 0, 1029);
		i1=0;
		i2=0;
		i1_h=0;
		i2_h=0;

		startstop_flag = 1;
		thr_close = 0;
			
		char SEND[17] = {0x30,'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
		char SEND512[17] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
		if(c1)
			sendto(socket_PC, (char*)(&SEND), sizeof(SEND), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
		else if(c2)
			sendto(socket_PC, (char*)(&SEND512), sizeof(SEND512), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
		else
			AfxMessageBox(_T("ģʽѡ����ִ���"));

		thr0_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		thr1_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);

		if(c2)
		{
			status_1_2 = 2;
			_512HandleData0 = AfxBeginThread(_512ThreadHandleData0, this);
			_512HandleData1 = AfxBeginThread(_512ThreadHandleData1, this);
			_512RecvData = AfxBeginThread(_512ThreadReceiveData, this);
		}
		else if(c1)
		{
			status_1_2 = 1;
			HandleData0 = AfxBeginThread(ThreadHandleData0, this);
			HandleData1 = AfxBeginThread(ThreadHandleData1, this);
			RecvData = AfxBeginThread(ThreadReceiveData, this);
		}
		g_event = ::CreateEvent(NULL, TRUE, FALSE, _T("event"));
		ss_status = 1;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextA(_T("��ʼ�ɼ�"));
		CTime t;
		t=CTime::GetTickCount();
		m_bar.SetPaneText(3,t.Format(_T("ֹͣʱ�� %H:%M:%S")));

		startstop_flag = 0;
		thr_close = 1;
		contistop = 0;
	
		char STOP[17]={0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};
		sendto(socket_PC, (char*)(&STOP), sizeof(STOP), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
	
		b0_flag = 0;
		b1_flag = 1;
		ofl_flag = 1;
		bhang = 0;
		ss_status = 0;
	}
}

void CFCFMDlg::OnBnClickedButton0xff()
{
	// TODO: Add your control notification handler code here
	if(!laser_status)
	{
		HBITMAP hBmp;
		hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 30, 30, LR_LOADMAP3DCOLORS);
		m_staticbmp3.SetBitmap(hBmp);

		startstop_flag = 0;
		thr_close = 1;
		contistop = 0;
		b0_flag = 0;
		b1_flag = 1;
		ofl_flag = 1;
		bhang = 0;
		ss_status = 0;
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_0xff)->SetWindowTextA(_T("�رռ�����"));

		CString s_on = _T("source:am:state on \n\r");
		m_cComm1.put_Output(COleVariant(s_on));//��������

		char laserff[17]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		thr0_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		thr1_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		sendto(socket_PC, (char*)(&laserff), sizeof(laserff), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
		HandleData0 = AfxBeginThread(ThreadHandleData0, this);
		HandleData1 = AfxBeginThread(ThreadHandleData1, this);
		RecvData = AfxBeginThread(ThreadReceiveData, this);
		g_event = ::CreateEvent(NULL, TRUE, FALSE, _T("event"));
		laser_status = 1;
	}
	else
	{
		m_staticbmp1.SetBitmap(NULL);
		m_staticbmp4.SetBitmap(NULL);
		m_staticbmp5.SetBitmap(NULL);

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_0xff)->SetWindowTextA(_T("�򿪼�����")); 
		m_staticbmp3.SetBitmap(NULL);

		CString s_on = _T("source:am:state off \n\r");
		m_cComm1.put_Output(COleVariant(s_on));//��������

		GetDlgItem(IDC_BUTTON_START)->SetWindowTextA(_T("��ʼ�ɼ�"));
		char laseree[17]={0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee};
		sendto(socket_PC, (char*)(&laseree), sizeof(laseree), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
		laser_status = 0;
	}
}
//void CFCFMDlg::OnBnClickedButtonStop()
//{
//	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_CONTISTOP)->EnableWindow(FALSE);
//
//	startstop_flag = 0;
//	thr_close = 1;
//	contistop = 0;
//
//	char STOP[16]={0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,};
//	sendto(socket_PC, (char*)(&STOP), sizeof(STOP), 0, (SOCKADDR*)&addr_FPGA, sizeof(SOCKADDR));
//
//	b0_flag = 0;
//	b1_flag = 1;
//	ofl_flag = 1;
//	bhang = 0;
//}

void CFCFMDlg::OnBnClickedButtonShot()
{
	// TODO: Add your control notification handler code here
	if(bmpdirset == 1)
	{
		CTime m_time=CTime::GetTickCount();
		CString m_strTime = m_time.Format("%Y-%m-%d_%H-%M-%S");		//filename can't include ':'
		CString destpath = bmpdir + _T("\\���Ž�ͼĿ¼");
		CreateDirectory(destpath, NULL);
		destpath += _T("\\") + m_strTime + _T(".bmp");

		if((bmp0_save == 1) && (bmp1_save == 0))
		{
			CString bmpname = _T("D:\\xx.bmp");
			CopyFile(bmpname,destpath,FALSE);
		}
		else if((bmp0_save == 0) && (bmp1_save == 1))
		{
			CString bmpname = _T("D:\\yy.bmp");
			CopyFile(bmpname,destpath,FALSE);
		}
		else if((bmp0_save == 1) && (bmp1_save == 1))
		{
			CString bmpname = _T("D:\\xx.bmp");
			CopyFile(bmpname,destpath,FALSE);
		}
		else
			AfxMessageBox(_T("��ͼʧ��"));
	}
	else
	{
		AfxMessageBox(_T("�������ý�ͼ����·��\n�˵���->����->��ͼ·��(Ctrl+P)"));
	}
}

UINT contibmp2(LPVOID ThreadParam)
{
	CFCFMDlg* pp = (CFCFMDlg*)ThreadParam;
	while(contistop)
	{
		HBITMAP hBmp;
		//hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		//pp->m_bmp2.SetBitmap(hBmp);
		//Sleep(500);
		hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 30, 30, LR_LOADMAP3DCOLORS);
		pp->m_staticbmp2.SetBitmap(hBmp);
		Sleep(500);
		pp->m_staticbmp2.SetBitmap(NULL);
		Sleep(400);
	}
	pp->m_staticbmp2.SetBitmap(NULL);
	return 0;
}

UINT ThreadContiShot(LPVOID ThreadParam)
{	
	CFCFMDlg* ps = (CFCFMDlg*)ThreadParam;
	CString time, path, bmpname;

	if(status_1_2 == 1)
	{
			while(contistop)
			{
				::WaitForSingleObject(g_event, INFINITE);
				time = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S");
				switch(fps)
				{
					case 1:path = ps->strPath1 + _T("\\") + time + _T("_1.bmp");break;
					case 2:path = ps->strPath1 + _T("\\") + time + _T("_2.bmp");break;
					case 3:path = ps->strPath1 + _T("\\") + time + _T("_3.bmp");break;
					case 4:path = ps->strPath1 + _T("\\") + time + _T("_4.bmp");break;
					default:break;
				}

				if(bmp0_save == 1 && (fps == 1 || fps == 3))
				{
					bmpname = _T("D:\\xx.bmp");
					CopyFile(bmpname, path, FALSE);
				}
				else if(bmp1_save == 1 && (fps == 2 || fps == 4))

				{
					bmpname = _T("D:\\yy.bmp");
					CopyFile(bmpname, path, FALSE);
				}
				else
					AfxMessageBox(_T("��ͼʧ��"));
				ResetEvent(g_event);
			}
	}
	else if(status_1_2 == 2)
	{
			while(contistop)
			{
				::WaitForSingleObject(g_event, INFINITE);
				time = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S");
				switch(fps_h)
				{
					case 1:path = ps->strPath1 + _T("\\") + time + _T("_1.bmp");break;
					case 2:path = ps->strPath1 + _T("\\") + time + _T("_2.bmp");break;
					case 3:path = ps->strPath1 + _T("\\") + time + _T("_3.bmp");break;
					case 4:path = ps->strPath1 + _T("\\") + time + _T("_4.bmp");break;
					case 5:path = ps->strPath1 + _T("\\") + time + _T("_5.bmp");break;
					case 6:path = ps->strPath1 + _T("\\") + time + _T("_6.bmp");break;
					case 7:path = ps->strPath1 + _T("\\") + time + _T("_7.bmp");break;
					case 8:path = ps->strPath1 + _T("\\") + time + _T("_8.bmp");break;
					default:break;
				}

				if(bmp0_save == 1 && (fps_h == 1 || fps_h == 3 || fps_h == 5 || fps_h == 7))
				{
					bmpname = _T("D:\\xx.bmp");
					CopyFile(bmpname, path, FALSE);
				}
				else if(bmp1_save == 1 && (fps_h == 2 || fps_h == 4 || fps_h == 6 || fps_h == 8))

				{
					bmpname = _T("D:\\yy.bmp");
					CopyFile(bmpname, path, FALSE);
				}
				else
					AfxMessageBox(_T("��ͼʧ��"));
				ResetEvent(g_event);
			}
	}
	else
		AfxMessageBox(_T("��ͼʧ��"));
	return 0;
}

void CFCFMDlg::OnBnClickedButtonContistart()
{
	// TODO: Add your control notification handler code here
	if(!cts_status)
	{
		if(startstop_flag)
		{
			if(bmpdirset == 0)
				//AfxMessageBox(_T("��δ��ʼ�ɼ�ͼ��\n��������ʼ����ť"));
				AfxMessageBox(_T("�������ý�ͼ����·��\n�˵���->����->��ͼ·��(Ctrl+P)"));
			else
			{
				contistop = 1;
				cts_status = 1;
				contibmp = AfxBeginThread(contibmp2, this);

				strTime = CTime::GetTickCount().Format("%H-%M-%S");
				strPath = bmpdir + _T("\\") + _T("������ͼĿ¼");
				CreateDirectory(strPath, NULL);
				strPath1 = strPath + _T("\\") + strTime;
				CreateDirectory(strPath1, NULL);

				//GetDlgItem(IDC_BUTTON_CONTISTART)->EnableWindow(FALSE); 
				//GetDlgItem(IDC_BUTTON_CONTISTOP)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_CONTISTART)->SetWindowTextA(_T("ֹͣ��ͼ"));
				contishot = AfxBeginThread(ThreadContiShot, this);
			}
		}
		else
		{
			//AfxMessageBox(_T("�������ý�ͼ����·��\n�˵���->����->��ͼ·��(Ctrl+P)"));
			AfxMessageBox(_T("��δ��ʼ�ɼ�ͼ��\n��������ʼ����ť"));
		}
	}
	else
	{
		contistop = 0;
		cts_status = 0;
		GetDlgItem(IDC_BUTTON_CONTISTART)->SetWindowTextA(_T("��ʼ��ͼ"));
	}
}

void CFCFMDlg::OnOpen()
{
	// TODO: Add your command handler code here
	CString filename;
	CFileDialog dlg(TRUE, _T("bmp"), _T("*.bmp"), OFN_EXPLORER, _T("Bitmap Files(*.bmp)|*.bmp|All Files(*.*)|*.*|"));
	if(dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName();
		ShellExecute(NULL, _T("open"), filename, NULL, NULL, SW_SHOWNORMAL);
	}
}

void CFCFMDlg::OnShotpath()
{
	// TODO: Add your command handler code here
	ITEMIDLIST *ppidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl);
	if (ppidl == NULL)
	{
		AfxMessageBox(_T("����·��ʧ��"));
		return;
	}
	TCHAR path[255] = _T("");
	BROWSEINFO *bi = new BROWSEINFO;
	bi->hwndOwner = this->GetSafeHwnd();
	bi->pidlRoot = ppidl;
	bi->pszDisplayName = NULL;
	//bi->pszDisplayName = _T("test");		//die when setfolder
	bi->lpszTitle = _T("��ѡ�񱣴��ͼ·��");
	bi->lpfn = NULL;
	bi->ulFlags = BIF_BROWSEINCLUDEURLS | BIF_SHAREABLE | BIF_USENEWUI;
	ppidl = SHBrowseForFolder(bi);
	if (!SHGetPathFromIDList(ppidl, path))
	{
		delete bi;
		return ;
	}
	delete bi;
	bmpdir = path;
	bmpdirset = 1;
}

void CFCFMDlg::OnLaserctrl()
{
	// TODO: Add your command handler code here
	pwr = new CLaserCtrlDlg;
	pwr->Create(IDD_DIALOG_LASERCTRL);
	pwr->ShowWindow(SW_SHOW);
}

void CFCFMDlg::OnAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}

void CFCFMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//CString strTime;
	//strTime.Format(_T("%dʱ%d��%d��"), st.wHour, st.wMinute, st.wSecond);
	//CWnd::GetDlgItem(IDC_EDIT_SYSTIME)->SetWindowTextA(strTime);
	//CTime t;
	//t=CTime::GetTickCount();
	//m_bar.SetPaneText(0,t.Format(_T("%Y-%m-%d %H:%M:%S")));
	//CDialog::OnTimer(nIDEvent);
	switch(nIDEvent)
	{
		case 0:
			{
				CTime t;
				t=CTime::GetTickCount();
				m_bar.SetPaneText(0,t.Format(_T("ʱ��:%Y-%m-%d %H:%M:%S")));
				CDialog::OnTimer(nIDEvent);
			}
			break;
		case 1:
			{
				CString str1;
				HANDLE hCom1;
				int num = 0;
				for (i = 1;i <= 5;i++)
				{//�˳���֧��16(5)������
					str1.Format(_T("COM%d"),i);
					hCom1 = CreateFile(str1, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
					if(INVALID_HANDLE_VALUE != hCom1 )
					{//�ܴ򿪸ô��ڣ�����Ӹô���
						CloseHandle(hCom1);
						num = i;
						if(num == 3)
						{
							if(m_cComm1.get_PortOpen()) //������ִ��ڱ����Ǵ򿪵ģ���رմ���
								m_cComm1.put_PortOpen(FALSE);
							m_cComm1.put_CommPort(3); //ѡ��COM��
							m_cComm1.put_InputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
							m_cComm1.put_InBufferSize(1024); //�������뻺����
							m_cComm1.put_OutBufferSize(512); //�������������
							m_cComm1.put_Settings(TEXT("9600,n,8,1"));//�����ʣ���У�飬������λ����ֹͣλ
							if(!m_cComm1.get_PortOpen())
							{
								m_cComm1.put_PortOpen(TRUE); //�򿪴���
								m_cComm1.put_RThreshold(1); //ÿ�����ջ������и��ַ�����մ�������
								m_cComm1.put_InputLen(0); //���õ�ǰ����������Ϊ
								m_cComm1.get_Input(); //Ԥ���������������������
							}
							else
								AfxMessageBox(_T("�򿪶˿�ʧ��!"),MB_ICONSTOP,0);
							CString s_off = _T("source:am:state off \n\r");
								m_cComm1.put_Output(COleVariant(s_off));//��������

							CString ipaddr;
							CString ipaddr_nac = _T("0.0.0.0");
							CString ipaddr_ac = _T("192.168.1");
							PIP_ADAPTER_INFO pAdapterInfo;
							PIP_ADAPTER_INFO pAdapter = NULL;
							DWORD dwRetVal = 0;

							ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
							pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

							if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
							{
								FREE(pAdapterInfo);
								pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
								if (pAdapterInfo == NULL)
								{
									AfxMessageBox(_T("Error allocating memory needed to call GetAdaptersInfo"));
									exit(-1);
								}
							}

							if((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
							{
								pAdapter = pAdapterInfo;
								while(pAdapter)
								{
									if(pAdapter->Type == MIB_IF_TYPE_ETHERNET)	//IF_TYPE_IEEE80211 == 71
									{
										ETHERNET_flag = 1;
										ipaddr = pAdapter->IpAddressList.IpAddress.String;
										pAdapter = pAdapter->Next;
									}
									else
										pAdapter = pAdapter->Next;
								}
							}
							if(pAdapterInfo)
								FREE(pAdapterInfo);



							//if(ETHERNET_flag == 1)
							//{
							//	if(ipaddr.Left(8) == ipaddr_nac)
							//	{
							//		AfxMessageBox(_T("�������ڷǻ״̬\n�����������Ӻ͵�Դ"));
							//		exit(-1);
							//	}
							//	else if(ipaddr.Left(9) != ipaddr_ac)
							//	{
							//		AfxMessageBox(_T("IP��ַ��������\n���ֶ�����IP"));
							//		exit(-1);
							//	}
							//	/*else
							//		AfxMessageBox(_T("Ethernet card ready"), MB_ICONINFORMATION);*/
							//}
							//else
							//{
							//	AfxMessageBox(_T("����������\n������"), MB_ICONSTOP);
							//	exit(-1);
							//}



							bmpdirset = 0;
							char szhostname[128];
							CString s_pcip,s_pcport;
							if(gethostname(szhostname,128)==0)
							{
								struct hostent *phost;
								phost=gethostbyname(szhostname);
								for(int j=0;j<4;j++)
								{
									CString addr;
									if(j>0)
										s_pcip+=".";
										addr.Format(_T("%u"),(unsigned int)((unsigned char*)phost->h_addr_list[0])[j]);
										s_pcip+=addr;
								}
							}

							HINSTANCE se = ::ShellExecute(this->m_hWnd, _T("open"), _T("cmd.exe"), _T("/c arp -s 192.168.1.200 6e-b8-c0-02-1e-03"), NULL, SW_HIDE);
							if((int)se < 32)
								AfxMessageBox(_T("ARP bind error"));

							GetDlgItem(IDC_EDIT_PCIP)->SetWindowText(s_pcip);
							GetDlgItem(IDC_EDIT_PCPORT)->SetWindowText(_T("33214"));
							GetDlgItem(IDC_EDIT_PCPORT)->GetWindowText(s_pcport);
							int port = atoi((const char*)s_pcport.GetBuffer(1));

							socket_PC = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
							addr_PC.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
							addr_PC.sin_family = AF_INET;
							addr_PC.sin_port = htons(port);

							addr_FPGA.sin_addr.S_un.S_addr=inet_addr("192.168.1.200");
							addr_FPGA.sin_family=AF_INET;
							addr_FPGA.sin_port=htons(4426);

							int nRecvBuf=30*1029;
							setsockopt(socket_PC, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

							int nNetTimeout_command=100;
							setsockopt(socket_PC, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout_command, sizeof(int));

							int len = sizeof(SOCKADDR_IN);
							if(bind(socket_PC, (SOCKADDR*)&addr_PC, len) == SOCKET_ERROR)
							{
								AfxMessageBox(_T("Bind Socket Error!"));
								exit(-1);
							}

							KillTimer(1);
						}
					}
				}
			}
	}
}

BOOL CFCFMDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(::TranslateAccelerator(GetSafeHwnd(),hAccTable,pMsg)) 
       return   true;
	return CDialog::PreTranslateMessage(pMsg);
}

void CFCFMDlg::OnLine()
{
	// TODO: Add your command handler code here
	line = new Line;
	line->Create(IDD_DIALOG_LINE);
	line->ShowWindow(SW_SHOW);
}

void CFCFMDlg::OnBnClickedButtonPwr()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString m_strSendData;
	CWnd::GetDlgItem(IDC_EDIT_PWR)->GetWindowTextA(m_strSendData);
	double a = _tstof(m_strSendData);
	a /= 1000;
	if(a<0.0001 || a>0.05)
		AfxMessageBox(_T("���ʲ����޶���Χ��"));
	else
	{		
		m_strSendData.Format(_T("%f"),a);
		CString xxx = _T("source:power:level:immediate:amplitude ") + m_strSendData + _T("\n\r");
		m_cComm1.put_Output(COleVariant(xxx));//��������
	}
}

void CFCFMDlg::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bar.SetPaneText(1,_T("֡Ƶ:8fps"));
	m_bar.SetPaneText(2,_T("ͼ����ʾ�ֱ���:1024 x 1024"));
}

void CFCFMDlg::OnBnClickedRadio2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bar.SetPaneText(1,_T("֡Ƶ:4fps"));
	m_bar.SetPaneText(2,_T("ͼ����ʾ�ֱ���:512 x 512"));
}

void CFCFMDlg::OnBnClickedRadio3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bar.SetPaneText(3,_T("̽ͷ���:U-240"));
}

void CFCFMDlg::OnBnClickedRadio4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bar.SetPaneText(3,_T("̽ͷ���:S-500"));
}
