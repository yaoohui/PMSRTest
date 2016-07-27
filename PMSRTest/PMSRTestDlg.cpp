
// PMSRTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRTestDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "PMSRSet.h"
#include "PMSRType.h"
#include "CommunicationProtocol.h"
#include "CSeries.h"
#include "CTChart.h"
#include "CEnvironment.h"
#include "CAxes.h"
#include "CAxis.h"
#include "CAspect.h"// ����3D��ͼ��ʾ
#include "CSurfaceSeries.h"
#include "CIsoSurfaceSeries.h"// 3d����
#include "CWaterfallSeries.h"
#include "CPen0.h"
#include "CBrush0.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum EM_RunState
{
	RUNSTATE_IDLE = 0,	// ��
	RUNSTATE_STOP,		// ֹͣ
	RUNSTATE_START0,	// �������������ȴ�������ʱ
	RUNSTATE_START,		// 
	RUNSTATE_ROTAING,	// ����ת��
	RUNSTATE_SAMPLE,	// �ɼ�����
};
enum EM_WaringMode
{
	WARNING_NONE = 0,	// ��
	WARNING_LIMIT,		// �ﵽ��λ
	WARNING_PASS,		// �ϸ�
	WARNING_NOGOOD,		// ���ϸ�
};
//#define MODEID	0x17		// ģ��ID������ͨѶ����
#define MAX_BUFFER_SIZE		2048
#define ID_STATUS_BAR_CTRL  102	// ״̬��ID

struct FlagDataTypeDef// ��־�ṹ��
{
	UCHAR state;		// ����״̬��ȡֵ��ö�� EM_RunState
	UCHAR warningmode;	// �������ͣ�01���ﵽ��λ��02���ϸ�03�����ϸ�ȡֵ��ö�� EM_WaringMode
	FLOAT rotating_speed;// ת�٣���λ��r/min ת/����
	UCHAR data[MAX_BUFFER_SIZE];			// ��������
	UINT datalen;		// �������ݳ���
	USHORT steplen;		// ��������ƶ�������2�ֽڣ���λ��mm
	UINT countdowncnt;	// ����ʱ���� 
	BOOL is3D;			// �Ƿ�3D��ʾ
	UINT remaintimes;	// ʣ�����
	BOOL ispass;		// �ж������trueΪ�ϸ�falseΪ���ϸ�
};


BYTE byRxBuffer[MAX_BUFFER_SIZE];
UINT RxLength = 0;
BYTE RxFlag = 0;
BYTE byTxBuffer[MAX_BUFFER_SIZE];
UINT TxLength = 0;

//HANDLE    m_hComm;	// ���ھ��
//UINT ReadComm(LPVOID pParam);

//UINT TestDelay;//��ʼ������ʱ
//CString ComNum;//���ڶ˿ں�
//UINT BaudRate, ByteSize, StopBits, Parity;

CStatusBarCtrl m_StatusBar;

//���ݿ�
_ConnectionPtr p_Connection;

// ȫ�ֽṹ��������������õı���
struct CommPara st_CommPara;
struct ConfigVal st_ConfigData;
struct FlagDataTypeDef st_FlagData;
CString strIniFileName = "set.ini";

CPMSRSet dlgSettings;
CommunicationProtocol CPInvokeInstance;// ͨ��Э�����
CEditLog  m_EditLogger;// ������ʾ�������ݵ�ʵ��

//״̬���ֿ�����
enum EM_StatusBarParts
{
	SBPART_TIME = 0,	// ʱ����
	SBPART_COMM,		// ͨ�Žӿ�����������
	SBPART_COMM_STATE,	// ͨ�Žӿڴ�״̬
	SBPART_RCOUNT,		// �������ݸ���
	SBPART_SCOUNT,		// �������ݸ���
	SBPART_DATABASE,	// ���ݿ�
	SBPART_PCNAME,		// ��ǰ�����
};

#define SHOW_DEBUG	// ����ˣ�����ʾ���ڵ��Բ��ֿؼ�
BOOL bExtiCountDown=FALSE;	// �Ƿ��˳�����ʱ


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPMSRTestDlg �Ի���


IMPLEMENT_DYNAMIC(CPMSRTestDlg, CDialogEx);

CPMSRTestDlg::CPMSRTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPMSRTestDlg::IDD, pParent)
//	, m_tchart(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CPMSRTestDlg::~CPMSRTestDlg()
{
	// ����öԻ������Զ���������
	//  ���˴���ָ��öԻ���ĺ���ָ������Ϊ NULL���Ա�
	//  �˴���֪���öԻ����ѱ�ɾ����
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CPMSRTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo_type);
	DDX_Control(pDX, IDC_EDIT1, m_edit_length);
	DDX_Control(pDX, IDC_EDIT2, m_edit_TestTimes);
	DDX_Control(pDX, IDC_EDIT5, m_edit_Weight);
	DDX_Control(pDX, IDC_EDIT8, m_edit_Flux);
	DDX_Control(pDX, IDC_EDIT9, m_edit_Pole);
	DDX_Control(pDX, IDC_EDIT10, m_edit_SN);
	DDX_Control(pDX, IDC_STATIC_GROUP_PARAMETER, m_group_Parameter);
	DDX_Control(pDX, IDC_STATIC_GROUP_RESULT, m_group_Result);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_static_Type);
	DDX_Control(pDX, IDC_STATIC_LENGTH, m_static_Length);
	DDX_Control(pDX, IDC_STATIC_TESTTIMES, m_static_TestTimes);
	DDX_Control(pDX, IDC_STATIC_FLUX, m_static_Flux);
	DDX_Control(pDX, IDC_STATIC_POLE, m_static_Pole);
	DDX_Control(pDX, IDC_STATIC_SN, m_static_SN);
	DDX_Control(pDX, IDC_STATIC_WEIGHT, m_static_Weight);
	DDX_Control(pDX, IDC_TCHART1, m_tchart);
	DDX_Control(pDX, IDC_BUTTON8, m_button_Keyboard);
	DDX_Control(pDX, IDC_BUTTON7, m_button_ScreenShot);
	DDX_Control(pDX, IDC_BUTTON6, m_button_Save);
	DDX_Control(pDX, IDC_BUTTON5, m_button_Print);
	DDX_Control(pDX, IDC_BUTTON3, m_button_2D3D);
	DDX_Control(pDX, IDC_BUTTON2, m_button_Type);
	DDX_Control(pDX, IDC_BUTTON1, m_button_Settings);
	DDX_Control(pDX, IDOK, m_button_Exit);
	DDX_Control(pDX, IDC_EDIT12, m_edit_ReceiveData);
	DDX_Control(pDX, IDC_CHECK1, m_check_ReceiveHex);
	DDX_Control(pDX, IDC_BUTTON9, m_button_Clear);
	DDX_Control(pDX, IDC_EDIT13, m_edit_SendData);
	DDX_Control(pDX, IDC_CHECK3, m_check_SendHex);
	DDX_Control(pDX, IDC_BUTTON10, m_button_Send);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_static_Result);
	DDX_Control(pDX, IDC_EDIT14, m_edit_Result);
}

BEGIN_MESSAGE_MAP(CPMSRTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CPMSRTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CPMSRTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPMSRTestDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPMSRTestDlg::OnCbnSelchangeCombo1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON8, &CPMSRTestDlg::OnBnClickedButton8)
	ON_WM_SIZE()
	ON_WM_CREATE()
//	ON_WM_SIZING()
ON_WM_GETMINMAXINFO()
ON_MESSAGE(WM_COMM_RXCHAR, &CPMSRTestDlg::OnCommRxchar)
ON_BN_CLICKED(IDC_BUTTON9, &CPMSRTestDlg::OnBnClickedButton9)
ON_MESSAGE(WM_COMM_BREAK_DETECTED, &CPMSRTestDlg::OnCommBreakDetected)
ON_MESSAGE(WM_COMM_RXFLAG_DETECTED, &CPMSRTestDlg::OnCommRxflagDetected)
ON_BN_CLICKED(IDC_BUTTON10, &CPMSRTestDlg::OnBnClickedButton10)
ON_MESSAGE(WM_ENABLESETCONTROLS, &CPMSRTestDlg::OnEnablesetcontrols)
ON_MESSAGE(WM_DELETEDLGCOUNTDOWN, &CPMSRTestDlg::OnDeletedlgcountdown)
ON_BN_CLICKED(IDC_BUTTON3, &CPMSRTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPMSRTestDlg ��Ϣ�������

BOOL CPMSRTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CWnd::SetWindowPos(NULL, 0, 0, 1350, 850, SWP_NOZORDER | SWP_NOMOVE);// �����ʼ��С
	ShowWindow(SW_MAXIMIZE);
	ShowDebugControls();// ���Կؼ���ʾ/����ʾ
	m_check_ReceiveHex.SetCheck(BST_CHECKED);
	m_check_SendHex.SetCheck(BST_CHECKED);

	LayoutFrame();	// ���岼��
	InitStatusBar(); //״̬����ʼ��
	InitVariable();	 // ��ʼ������
	st_CommPara.m_Comm.FindComPort();	// ���ҿ��õĴ���
	ReadSettings(); //��ȡ����
	OpenComm();//�򿪴��ڡ������ڶ�ȡ����ReadSettings()���к��ٵ���
	ConnectSQLServer();//�������ݿ�
	Initinterface();//��ʼ������
	InitFont();//��ʼ����������
	SetTimer(1, 1000, NULL);//������ʱ��
	TChartInit();	// ͼ��ؼ���ʼ������Ҫ������������

	m_EditLogger.SetEditCtrl(m_edit_ReceiveData.m_hWnd);	// CEditLog��������������ı���

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPMSRTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPMSRTestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPMSRTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ���û��ر� UI ʱ������������Ա���������ĳ��
//  �������Զ�����������Ӧ�˳���  ��Щ
//  ��Ϣ�������ȷ����������: �����������ʹ�ã�
//  ������ UI�������ڹرնԻ���ʱ��
//  �Ի�����Ȼ�ᱣ�������

void CPMSRTestDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CPMSRTestDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CPMSRTestDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CPMSRTestDlg::CanExit()
{
	// �����������Ա�����������Զ���
	//  �������Իᱣ�ִ�Ӧ�ó���
	//  ʹ�Ի���������������� UI ����������
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

// �򿪴���
void CPMSRTestDlg::OpenComm()
{
	if (!st_CommPara.bIsCommOpen)// ����δ��
	{
		if (st_CommPara.m_Comm.InitPort(this->m_hWnd, st_CommPara.uiCommName, st_CommPara.uiBaudRate, st_CommPara.uiParity, st_CommPara.uiByteSize,
			st_CommPara.uiStopBits, EV_RXCHAR | EV_BREAK, MAX_BUFFER_SIZE))
		{
			st_CommPara.bIsCommOpen = TRUE;
			m_StatusBar.SetText("���ڴ򿪳ɹ�", SBPART_COMM_STATE, 0);
			CString strCOMStatus;
			CString str;
			str.Format("%d", st_CommPara.uiBaudRate);
			strCOMStatus = st_CommPara.strCommName + ", " + str + "bps, N81";
			m_StatusBar.SetText(strCOMStatus, SBPART_COMM, 0);

			st_CommPara.m_Comm.StartMonitoring();// �������������
		}
		else
		{
			m_StatusBar.SetText("�������ó���", SBPART_COMM_STATE, 0);
		}

	}
	else// �����Ѵ�
	{
		st_CommPara.m_Comm.ClosePort();
		st_CommPara.bIsCommOpen = FALSE;
	}
}
//void CPMSRTestDlg::OpenComm()
//{
//	CString str;
//
//	if (st_CommPara.bIsCommOpen == FALSE)
//	{
//		CString str1;
//		if (st_CommPara.strCommName == "")
//		{
//			AfxMessageBox("�޴��ڣ�");
//			m_StatusBar.SetText("�޴��ڣ�", 2, 0);
//			return;
//		}
//
//		str1.Format("\\\\.\\%s", st_CommPara.strCommName);	// ���ںŴ���10ʱ���޷��򿪣���Ҫ�Ĵ�����
//		m_hComm = CreateFile(str1,  //���ں�
//			GENERIC_READ | GENERIC_WRITE, //ָ�����ԶԴ��ڽ��ж�д����
//			0, //��ʾ����Ϊ��ռ��
//			NULL,// Ȩ�޿��ƣ���ʾ���صľ�����ܱ��ӽ��̼̳С�
//			OPEN_EXISTING, //��ʾ��ָ�����ڲ�����ʱ�����򽫷���ʧ��
//			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //��ʾ�ļ�����
//			/*���򿪴���ʱ������ָ�� FILE_FLAG_OVERLAPPED���ص���ʽ��������ʾ�ļ����豸����ά������ָ�룬���ڶ�дʱ������ʹ��OVERLAPPED �ṹָ�����ʵ��ļ�ƫ������
//			*/
//			NULL);//��ʱ�ļ��ľ������ʹ�á�
//
//		if (m_hComm == INVALID_HANDLE_VALUE)
//		{
//			//AfxMessageBox("���ڽ���ʧ�ܣ�");
//			str.Format("�Ҳ���%s", st_CommPara.strCommName);
//			m_StatusBar.SetText(str, 2, 0);
//			return;
//		}
//
//		/********************���뻺����������������Ĵ�С***********/
//		SetupComm(m_hComm, 4096, 4096);
//		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);// �����������������
//		SetCommMask(m_hComm, EV_RXCHAR);// ����֪ͨ�¼���EV_RXCHAR: ���뻺����������ʱ��ͨ��WaitCommEvent�������Ի��֪ͨ
//
//		DCB dcb;
//
//		////���ó�ʱʱ��
//		//COMMTIMEOUTS TimeOuts;
//		//TimeOuts.ReadIntervalTimeout = 0;
//		//TimeOuts.ReadTotalTimeoutMultiplier = 0;
//		//TimeOuts.ReadTotalTimeoutConstant = 50;
//		//TimeOuts.WriteTotalTimeoutMultiplier = 0;
//		//TimeOuts.WriteTotalTimeoutConstant = 0;
//		//SetCommTimeouts(m_hComm, &TimeOuts);
//
//		GetCommState(m_hComm, &dcb);//��ò���  
//
//		dcb.BaudRate = st_CommPara.uiBaudRate;
//
//		dcb.ByteSize = 8;// st_CommPara.uiByteSize;
//
//		dcb.StopBits = ONESTOPBIT;// st_CommPara.uiStopBits;// TWOSTOPBITS;
//
//		dcb.Parity = NOPARITY;// st_CommPara.uiParity; //У��λ
//
//		dcb.fBinary = TRUE;// ָ���Ƿ����������ģʽ
//		dcb.fParity = TRUE;// ָ���Ƿ�������żУ��
//
//		//�����豸���ƿ�����ͨ���豸
//		if (!SetCommState(m_hComm, &dcb))
//		{
//			//AfxMessageBox("�������ó���");
//			m_StatusBar.SetText("�������ó���", 2, 0); 
//			CloseHandle(m_hComm);
//			return;
//		}
//
//		st_CommPara.bIsCommOpen = TRUE;
//		m_StatusBar.SetText("���ڴ򿪳ɹ�", 2, 0);
//		// �����̶߳�ȡ��������
//		//AfxBeginThread(ReadComm, this); //���������߳�
//	}
//	else
//	{
//		CloseHandle(m_hComm);
//		m_hComm = INVALID_HANDLE_VALUE;
//		st_CommPara.bIsCommOpen = FALSE;
//	}
//}

void CPMSRTestDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
	CDialogEx::OnOK();
}


//
// ��ȡ�������
//
void CPMSRTestDlg::ReadSettings()
{
	CStdioFile settingFile;
	CFileException fileException;

	//д�ļ�
	//if(settingFile.Open(pszFileName,CFile::typeText|CFile::modeCreate|CFile::modeReadWrite),&fileException)
	//{
	//	settingFile.WriteString("��1��\n");
	//	CString strOrder;
	//	strOrder.Format("%d,%.3f",66,88.88);
	//	settingFile.WriteString(strOrder);
	//}
	//else
	//{
	//	TRACE("Can't open file %s,error=%u\n",pszFileName,fileException.m_cause);
	//}

	//���ļ�
	if (settingFile.Open(strIniFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
	{
		CString strContent;
		CString temp;
		//CString strCOMStatus;

		while (settingFile.ReadString(strContent))
		{
			if (strContent.Left(2) != _T("//")) //ȡǰ�����ַ���������ǡ�//������������
			{
				int n = 0, m = 0, l = 0;

				n = strContent.Find('='); //��á�=����λ��
				m = strContent.Find(';'); //��á�������λ��
				l = strContent.GetLength(); //����ַ������� l-n-2

				if (strContent.Left(n - 1) == _T("ComNum")) //�ж��Ƿ���Ҫ����ַ�����ͬ
				{
					//strcpy(temp, strContent.Mid(n+1,l-n-2));
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					st_CommPara.strCommName = temp; //��ȡ���ں�
					//strCOMStatus = temp;
					st_CommPara.uiCommName = atoi(temp.Mid(3));
				}
				else if (strContent.Left(n - 1) == _T("BaudRate"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					st_CommPara.uiBaudRate = atoi(temp); //��ȡ������
					//strCOMStatus = strCOMStatus + ", " + temp + "bps, N81";
				}
				//else if (strContent.Left(n - 1) == _T("ByteSize"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
				//	st_CommPara.uiByteSize = atoi(temp); //��ȡ����λ
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				//else if (strContent.Left(n - 1) == _T("StopBits"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
				//	StopBits = atoi(temp); //��ȡֹͣλ
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				//else if (strContent.Left(n - 1) == _T("Parity"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
				//	Parity = atoi(temp); //��ȡ������ʹ������
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				else if (strContent.Left(n - 1) == _T("StartDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					st_ConfigData.StartDelay = atoi(temp); //��ȡ������ʹ������
					if ((st_ConfigData.StartDelay < 0) || (st_ConfigData.StartDelay > 900))
					{
						st_ConfigData.StartDelay = 100;
						AfxMessageBox(_T("set.ini�ļ��У���ʱʱ�����ô���"), MB_OK);
					}
				}
				//m_StatusBar.SetText(strCOMStatus, SBPART_COMM, 0);
			}
		}
		// �ж��Ƿ���ȷ��ȡ���ںţ���Чʱ�����´��ں�
		if (st_CommPara.strCommName == "" || st_CommPara.uiCommName == 0)
		{
			if (st_CommPara.m_Comm.m_ComCount > 0)// ע������п��ô���
			{
				CString str;
				str.Format("COM%d", st_CommPara.m_Comm.m_ComArray[0]);
				st_CommPara.strCommName = str;
				st_CommPara.uiCommName = st_CommPara.m_Comm.m_ComArray[0];
				st_CommPara.uiBaudRate = 9600;
				//strCOMStatus = str + ", " + "9600bps, N81";
			}
			st_ConfigData.StartDelay = 100;
		}
		st_CommPara.uiByteSize = 8;
		st_CommPara.uiParity = 'N';
		st_CommPara.uiStopBits = 0;
	}
	else
	{
		TRACE("Can't open file");
	}

	settingFile.Close();
}

//
// ״̬����ʼ��
//
void CPMSRTestDlg::InitStatusBar()
{
	m_StatusBar.SetText("RX:0", SBPART_RCOUNT, 0);
	m_StatusBar.SetText("TX:0", SBPART_SCOUNT, 0);

	//m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	//int strPartDim[6] = {130, 250, 350, 450, 550, -1}; //�ָ�����������Ϊ��ʼλ�ã����ǿ�ȣ�-1��ʾ�����Ҷˡ�
	//m_StatusBar.SetParts(6, strPartDim);
	////��������״̬���м���ͼ��
	////m_StatusBar.SetIcon(1,SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE));//Ϊ�ڶ��������мӵ�ͼ��

}



//
// �������ݿ�
//
void CPMSRTestDlg::ConnectSQLServer()
{
	CString strSQL;
	char ComputerName[100];
	DWORD NameSize = 100;
	HRESULT hr;

	GetComputerName(ComputerName, &NameSize);//��ȡ���������
	m_StatusBar.SetText(ComputerName, SBPART_PCNAME, 0);
	//m_list.InsertString(m_list.GetCount(), ComputerName);
	//m_list.SetTopIndex(m_list.GetCount() - 1);

	strSQL.Format(_T("Provider=SQLOLEDB.1;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=PMSR;Data Source=%s\\SQLEXPRESS"), ComputerName);

	try
	{
		hr = p_Connection.CreateInstance(__uuidof(Connection));
		p_Connection->CursorLocation = adUseClient;
		if (SUCCEEDED(hr))
		{
			hr = p_Connection->Open(_bstr_t(strSQL), "", "", adConnectUnspecified);
		}
	}

	catch (_com_error e)///��׽�쳣
	{
		//CString errormessage;
		//errormessage.Format("�������ݿ�ʧ��!\r\n������Ϣ:%s",e.ErrorMessage());
		AfxMessageBox(e.ErrorMessage());///��ʾ������Ϣ
		//return FALSE;
	}
}

void CPMSRTestDlg::Initinterface()
{
	_bstr_t sql;

	sql = _T("select * from ParameterDB order by id");

	_RecordsetPtr p_Recordset;
	p_Recordset.CreateInstance(__uuidof(Recordset));

	p_Recordset->Open(sql, p_Connection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);

	try
	{
		while (!p_Recordset->adoBOF)
		{
			m_combo_type.AddString((_bstr_t)p_Recordset->GetCollect(_T("type")));

			p_Recordset->MoveNext();
		}
	}

	catch (_com_error e)
	{
		//AfxMessageBox(e.Description());
	}

	p_Recordset->Close();
}
// ���� ��ť
void CPMSRTestDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CPMSRSet PMSRSet;
	PMSRSet.DoModal();

	// �������ô���
	st_CommPara.m_Comm.ClosePort();
	st_CommPara.bIsCommOpen = FALSE;
	OpenComm();
}


void CPMSRTestDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CPMSRType PMSRType;
	PMSRType.DoModal();
}


void CPMSRTestDlg::OnCbnSelchangeCombo1()
{
	CString str, sql;

	UpdateData();

	int nIndex = m_combo_type.GetCurSel();
	m_combo_type.GetLBText(nIndex, str);

	sql.Format(_T("select * from ParameterDB where type = '%s'"), str);

	_RecordsetPtr p_Recordset;
	p_Recordset.CreateInstance(__uuidof(Recordset));

	p_Recordset->Open((_bstr_t)sql, p_Connection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);

	try
	{
		m_edit_length.SetWindowTextA((_bstr_t)p_Recordset->GetCollect(_T("length")));
		m_edit_TestTimes.SetWindowTextA((_bstr_t)p_Recordset->GetCollect(_T("testtime")));
	}

	catch (_com_error e)
	{
		//AfxMessageBox(e.Description());
	}

	UpdateData(FALSE);

	p_Recordset->Close();
}


void CPMSRTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CTime NowTime = CTime::GetCurrentTime();
	CString time, date;
	time.Format(_T("%02d:%02d:%02d"), NowTime.GetHour(), NowTime.GetMinute(), NowTime.GetSecond());
	date.Format(_T("%04d/%02d/%02d"), NowTime.GetYear(), NowTime.GetMonth(), NowTime.GetDay());

	m_StatusBar.SetText(date + " " + time, SBPART_TIME, 0);

	// ��������ӳ�ʱ�䵹��ʱ
	if (st_FlagData.state == RUNSTATE_START0)
	{
		if ((st_FlagData.countdowncnt > 0) && DlgWaiting && bExtiCountDown)
		{
			st_FlagData.countdowncnt--;
			DlgWaiting->SendMessage(WM_WAITCOUNTDOWN, st_FlagData.countdowncnt, 0);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CPMSRTestDlg::InitFont()
{
	CFont *f = new CFont;
	f->CreateFont(30, 0, 0, 0, 0, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("΢���ź�"));
	//f->CreateFont(36, 12, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("΢���ź�"));
	((CStatic *)GetDlgItem(IDC_STATIC_TYPE))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_LENGTH))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_TESTTIMES))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_WEIGHT))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_FLUX))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_POLE))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_SN))->SetFont(f, TRUE);
	m_combo_type.SetFont(f, TRUE);
	m_edit_length.SetFont(f, TRUE);
	m_edit_TestTimes.SetFont(f, TRUE);
	m_edit_Weight.SetFont(f, TRUE);
	m_edit_Flux.SetFont(f, TRUE);
	m_edit_Pole.SetFont(f, TRUE);
	m_edit_SN.SetFont(f, TRUE);
	m_edit_Result.SetFont(f, TRUE);
	m_static_Result.SetFont(f, TRUE);

	((CStatic *)GetDlgItem(IDOK))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON1))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON2))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON3))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON4))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON5))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON6))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON7))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_BUTTON8))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_GROUP_PARAMETER))->SetFont(f, TRUE);
	((CStatic *)GetDlgItem(IDC_STATIC_GROUP_RESULT))->SetFont(f, TRUE);

}


void CPMSRTestDlg::OnBnClickedButton8()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ShellExecute(NULL, "open", "C:\\Program Files\\Common Files\\Microsoft Shared\\Ink\\TabTip.exe", NULL, NULL, SW_SHOWNORMAL);//�������
}



// �����岼��
void CPMSRTestDlg::LayoutFrame()
{
	CRect RectClient, RectTemp; 
	UINT uiLeft = 0, uiTop = 0, uiWidth = 0, uiHeight = 0;

	GetClientRect(&RectClient); 
	// group"���Բ���"
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop = 40;
	uiWidth = RectClient.Width() / 3 - 20;
	uiHeight = 200;
	m_group_Parameter.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	
	// "�ͺ�"
	uiLeft += 20;
	uiTop += 50;
	m_static_Type.SetWindowPos(this, uiLeft, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Type.GetClientRect(&RectTemp);
	m_combo_type.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "����"
	uiTop += 50;
	m_static_Length.SetWindowPos(this, uiLeft, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Length.GetClientRect(&RectTemp);
	m_edit_length.SetWindowPos(this, uiLeft+RectTemp.Width()+2, uiTop, 0,0, SWP_NOSIZE | SWP_NOZORDER);
	// "��ͨ��"
	m_static_Flux.SetWindowPos(this, uiLeft + uiWidth / 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Flux.GetClientRect(&RectTemp);
	m_edit_Flux.SetWindowPos(this, uiLeft + uiWidth / 2 + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "���Դ���"
	uiTop += 50;
	m_static_TestTimes.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_TestTimes.GetClientRect(&RectTemp);
	m_edit_TestTimes.SetWindowPos(this, uiLeft+RectTemp.Width()+2, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	// "�ż���"
	m_static_Pole.SetWindowPos(this, uiLeft + uiWidth / 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Pole.GetClientRect(&RectTemp);
	m_edit_Pole.SetWindowPos(this, uiLeft + uiWidth / 2 + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


	// group"���Խ��"
	m_group_Parameter.GetWindowRect(&RectTemp);
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop = RectTemp.bottom + 6;//uiHeight + 50;
	uiWidth = RectTemp.Width();//RectClient.Width() / 3 - 20;
	uiHeight = 200;
	m_group_Result.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);

	// "ת�ӱ��"
	uiLeft += 20;
	uiTop += 50;
	m_static_SN.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_SN.GetClientRect(&RectTemp);
	m_edit_SN.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "����"
	uiTop += 50;
	m_static_Weight.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Weight.GetClientRect(&RectTemp);
	m_edit_Weight.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	// "���Խ��"
	uiTop += 50;
	m_static_Result.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Result.GetClientRect(&RectTemp);
	m_edit_Result.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// ��ť
	m_group_Result.GetWindowRect(&RectTemp);
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop = RectTemp.bottom + 10;
	uiWidth = 88; uiHeight = 80;
	m_button_Keyboard.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_ScreenShot.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_Save.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_Print.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop += uiHeight + 10;
	m_button_2D3D.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_Type.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_Settings.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	uiLeft += uiWidth + 20;
	m_button_Exit.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);

	// ======������===========
#ifdef SHOW_DEBUG
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop += 100;
	uiWidth = RectClient.Width() / 3 - 200;
	m_edit_ReceiveData.SetWindowPos(this, uiLeft, uiTop, uiWidth, 50, SWP_NOZORDER);
	m_check_ReceiveHex.SetWindowPos(this, uiLeft + uiWidth + 10, uiTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_button_Clear.SetWindowPos(this, uiLeft + uiWidth + 10, uiTop + 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_edit_SendData.SetWindowPos(this, uiLeft, uiTop+60, uiWidth, 20, SWP_NOZORDER);
	m_check_SendHex.SetWindowPos(this, uiLeft + uiWidth + 10, uiTop + 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_button_Send.SetWindowPos(this, uiLeft + uiWidth + 60, uiTop + 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#endif
	// END======������===========

	// ͼ��
	m_tchart.SetWindowPos(this, 50, 50, RectClient.Width()*2/3 - 100, RectClient.Height() - 100, SWP_NOZORDER);

	// ״̬��
	uiWidth = RectClient.Width()/6;
	int strPartDim[7] = { uiWidth, uiWidth * 2, uiWidth * 3, uiWidth/2 * 7, uiWidth/2*8, uiWidth * 5, -1 }; //�ָ�����������Ϊ��ʼλ�ã����ǿ�ȣ�-1��ʾ�����Ҷˡ�
	m_StatusBar.SetParts(7, strPartDim);
	m_StatusBar.SetWindowPos(0, RectClient.left, RectClient.top, RectClient.Width(), RectClient.Height() - 20, 0);
}


void CPMSRTestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (::IsWindow(m_static_Type.m_hWnd))// �ؼ���Ч��
		LayoutFrame();

	CRect Rect;
	GetClientRect(&Rect);
	m_StatusBar.SetWindowPos(0, Rect.left,Rect.top, Rect.Width(), Rect.Height()-20, 0);
}


int CPMSRTestDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW | CCS_BOTTOM | SBARS_SIZEGRIP, CRect(0, 0, 0, 0), this, ID_STATUS_BAR_CTRL);//CCS_NOPARENTALIGN
	return 0;
}


void CPMSRTestDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// ���ô������С�ߴ�
	lpMMI->ptMinTrackSize.x = 1350;
	lpMMI->ptMinTrackSize.y = 850;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}




// ͨ��Э�����ʼ��
void CPMSRTestDlg::CommandProcessInit()
{
	CPInvokeInstance.Init(&CPInvokeInstance, NULL);// ����ͨ��Э�����ʵ��
}


// ���ڽ������ݴ���
void CPMSRTestDlg::CommandPress()
{
	UCHAR verfy;

	if (RxLength == 0)
		return;

	verfy = CPInvokeInstance.IsValidProtocol(byRxBuffer, RxLength);
	if (verfy == 1)//���Ȳ���
	{
		return;
	}
	else if (verfy > 1)// ��������
	{
		RxLength = 0;
		return;
	}

	if (CPInvokeInstance.rcvHeader->modeid != MODEID)	// ģ���Ŵ��󣬷���
	{
		RxLength = 0;
		return;
	}

	BOOL bNeedResponse = FALSE;
	unsigned int respLength = 0;
	unsigned char tmpCharValue = 0;

	bNeedResponse = TRUE;
	switch (CPInvokeInstance.rcvHeader->cmd)
	{
		case 0xF1://���������������㲽������ƶ�����
			bNeedResponse = FALSE;
			if (!ReadParameters())// ��ȡ���ò����ɹ�
			{
				// ��������������ָ��
				CPInvokeInstance.rcvHeader->cmd = 0xF8;
				CPInvokeInstance.PackProtocol(CPInvokeInstance.rcvHeader, CPInvokeInstance.payload, CPInvokeInstance.payloadLength, byRxBuffer, &respLength);
				st_CommPara.m_Comm.WriteToPort(byRxBuffer, respLength);

				st_FlagData.countdowncnt = st_ConfigData.StartDelay;
				if (st_FlagData.countdowncnt == 0)// ����ʱ�����쳣
					return (void)AfxMessageBox("������ʱʱ��Ϊ0��");

				// ��ʾ����ʱ����
				DlgWaiting = new CDlgWaiting();
				DlgWaiting->Create(IDD_DLG_COUNTDOWN);
				if (DlgWaiting == NULL)
					return (void)AfxMessageBox("����ʱ���ھ������ʧ�ܣ�");

				bExtiCountDown = TRUE;
				this->EnableWindow(FALSE);
				DlgWaiting->ShowWindow(SW_SHOW);
				OnEnablesetcontrols(FALSE,0);//EnableSettings(FALSE);// ����ؼ�Ϊ��ɫ
				DlgWaiting->SendMessage(WM_WAITCOUNTDOWN, st_FlagData.countdowncnt, 0);

				st_FlagData.state = RUNSTATE_START0;	// ��������������ʼ��ʱ����ʱ�趨ʱ���󣬷��Ͳ�������ƶ�����
				// �������̴����߳�
				AfxBeginThread(FlowProcessThread, this);
			}
			else// ��ȡʧ��
			{
				AfxMessageBox("����δ���ã�\r\n�밴��ֹͣ��ť�����������������ٰ���������", MB_OK | MB_ICONEXCLAMATION);
			}
			break;

		case 0xF3:// ����
			if (CPInvokeInstance.payload[0] == WARNING_LIMIT)// �ﵽ����λ
				st_FlagData.warningmode = WARNING_LIMIT;
			break;

		case 0xF2:// ֹͣ
			st_FlagData.state = RUNSTATE_STOP;
			InitVariable();
			OnEnablesetcontrols(TRUE, 0);// ����ؼ�Ϊʹ��
			m_tchart.RemoveAllSeries();	// ͼ�������������
			// �˳��߳�
			UINT code;
			GetExitCodeThread(FlowProcessThread, (LPDWORD)&code);
			AfxEndThread(code, TRUE);
			break;

		case 0xF4:// ת��
			bNeedResponse = FALSE;
			st_FlagData.rotating_speed = (FLOAT)(*(USHORT*)CPInvokeInstance.payload) / (FLOAT)10.0;
			st_FlagData.state = RUNSTATE_ROTAING;
			break;
		
		case 0xF5:// �ɼ�����
			bNeedResponse = FALSE;
			st_FlagData.datalen = CPInvokeInstance.rcvHeader->len;
			memcpy(st_FlagData.data, CPInvokeInstance.payload, st_FlagData.datalen);
			st_FlagData.state = RUNSTATE_SAMPLE;
			break;

		default:
			bNeedResponse = FALSE;
			break;
	}

	if (bNeedResponse)  //�����Ҫ���ⲿͳһ������Ӧ�����������ĺ���
	{
		CPInvokeInstance.PackResponse(CPInvokeInstance.rcvHeader, CPInvokeInstance.payload, CPInvokeInstance.payloadLength, byRxBuffer, &respLength);
		st_CommPara.m_Comm.WriteToPort(byRxBuffer, respLength);
	}

	RxLength = 0;

}

// �����������Ϣ
afx_msg LRESULT CPMSRTestDlg::OnCommRxchar(WPARAM ch, LPARAM port)
{
	CString str;
	if (m_check_ReceiveHex.GetCheck())
	{
		str.Format("%02X ", ch);
	}
	else
	{
		str.Format("%c", ch);
	}
#ifdef SHOW_DEBUG
	m_EditLogger.AddText(str);
	UpdateData(FALSE);
	m_edit_ReceiveData.LineScroll(m_edit_ReceiveData.GetLineCount());
#endif
	st_CommPara.strReceivedData += str;
	
	// �����ݱ��浽������
	byRxBuffer[RxLength] = ch;
	RxLength++;

	st_CommPara.uiBytesReceived++;
	str.Format("RX:%d", st_CommPara.uiBytesReceived);
	m_StatusBar.SetText(str, SBPART_RCOUNT, 0);

	return 0;
}

// ��������ռ�⵽break�����չ����г��ֶ��break
afx_msg LRESULT CPMSRTestDlg::OnCommBreakDetected(WPARAM wParam, LPARAM lParam)
{
#ifdef SHOW_DEBUG
	m_EditLogger.AddText("$$");
	UpdateData(FALSE);
#endif
	CommandPress();

	return 0;
}

// ��������յ�0xAA ���Կ�����Ч��
afx_msg LRESULT CPMSRTestDlg::OnCommRxflagDetected(WPARAM wParam, LPARAM lParam)
{
#ifdef SHOW_DEBUG
	m_EditLogger.AddText("@@");
	UpdateData(FALSE);
#endif
	return 0;
}


// ������Կؼ�
void CPMSRTestDlg::OnBnClickedButton9()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	st_CommPara.uiBytesReceived = 0;
	st_CommPara.uiBytesSent = 0;
	m_StatusBar.SetText("RX:0", SBPART_RCOUNT, 0);
	m_StatusBar.SetText("TX:0", SBPART_SCOUNT, 0);
	
	HWND hEdit = m_edit_ReceiveData;
	BOOL bReadOnly = ::GetWindowLong(hEdit, GWL_STYLE) & ES_READONLY;
	if (bReadOnly)
	{
		::SendMessage(hEdit, EM_SETREADONLY, FALSE, 0);
	}
	::SendMessage(hEdit, EM_SETSEL, 0, -1);
	::SendMessage(hEdit, WM_CLEAR, 0, 0);
	if (bReadOnly)
		::SendMessage(hEdit, EM_SETREADONLY, TRUE, 0);

	RxLength = 0;

	// ���ͼ��
	CSeries LineSeries1 = m_tchart.Series(0);// ֱ������
	LineSeries1.Clear();	// �������

}


// ��ʾ�����ص����ÿؼ��������Ƿ��к궨��SHOW_DEBUGȷ�������ÿؼ��Ƿ���ʾ
void CPMSRTestDlg::ShowDebugControls()
{
#ifdef SHOW_DEBUG
	m_edit_ReceiveData.ShowWindow(SW_SHOW);
	m_edit_SendData.ShowWindow(SW_SHOW);
	m_check_ReceiveHex.ShowWindow(SW_SHOW);
	m_check_SendHex.ShowWindow(SW_SHOW);
	m_button_Clear.ShowWindow(SW_SHOW);
	m_button_Send.ShowWindow(SW_SHOW);
#else
	m_edit_ReceiveData.ShowWindow(SW_HIDE );
	m_edit_SendData.ShowWindow(SW_HIDE );
	m_check_ReceiveHex.ShowWindow(SW_HIDE );
	m_check_SendHex.ShowWindow(SW_HIDE );
	m_button_Clear.ShowWindow(SW_HIDE );
	m_button_Send.ShowWindow(SW_HIDE ); 
#endif
}


// ��ͼ��tchart����ʾ����
void CPMSRTestDlg::ShowWave()
{
	//CSeries LineSeries1;// = m_tchart.Series(0);// ֱ������
	//CSeries SurfaceSeries;// = m_tchart.Series(1);// surface����
	//CIsoSurfaceSeries isoSurfaceSeries;
	//UINT i,j;

	CSeries LineSeries;
	CSeries SurfaceSeries;
	CIsoSurfaceSeries IsoSurfaceSeries;
	CWaterfallSeries WaterfallSeries;

	// �����������
	if (st_FlagData.is3D)
	{
		//m_tchart.AddSeries(scIsoSurface);
		m_tchart.AddSeries(scWaterfall);
		SurfaceSeries = m_tchart.Series(st_ConfigData.testtimes - st_FlagData.remaintimes - 1);
		
		//IsoSurfaceSeries = SurfaceSeries.get_asIsoSurface();// 3D���й���2D
		WaterfallSeries = SurfaceSeries.get_asWaterfall();
		((CBrush0)WaterfallSeries.get_Brush()).put_Style(bsClear);// �����
		((CPen0)WaterfallSeries.get_WaterLines()).put_Visible(false);// ��֧����
		((CPen0)SurfaceSeries.get_Pen()).put_Color(RGB(255, 0, 0));// ������ɫ
	}
	else
	{
		m_tchart.AddSeries(scLine);
		LineSeries = m_tchart.Series(st_ConfigData.testtimes - st_FlagData.remaintimes - 1);
	}

	// ����ʹ��AddArray������ͼ����AddXY��ͼ�ٶȿ죬����Ҫÿ��ˢ��
	COleSafeArray XValues, YValues;
	COleSafeArray ZValues;
	DWORD wLength = st_FlagData.datalen;
	DWORD zLength = 5;
	XValues.Create(VT_R8, 1, &wLength);//VT_R8����ָdouble 
	YValues.Create(VT_R8, 1, &wLength);
	long i;
	double temp;
	for ( i = 0; i < wLength; i++)
	{
		temp = i;
		XValues.PutElement(&i, &temp);
		temp = st_FlagData.data[i];
		YValues.PutElement(&i, &temp);
	}
	if (st_FlagData.is3D)
	{
		//ZValues.Create(VT_R8, 1, &wLength);
		//for (i = 0; i < wLength; i++)
		//{
		//	temp = i;
		//	ZValues.PutElement(&i, &temp);
		//}
		//IsoSurfaceSeries.AddArrayXYZ(XValues, YValues, ZValues);
		
		for (i = 0; i < wLength; i++)
		{
			WaterfallSeries.AddXYZ(i, st_FlagData.data[i], st_FlagData.remaintimes, 0, RGB(255, st_FlagData.remaintimes*80, 0));
		}
	}
	else
	{
		LineSeries.AddArray(wLength, YValues, XValues);
	}

	//LineSeries1.Clear();	// �������
	//SurfaceSeries.Clear();

	//if (st_FlagData.is3D)	// 3d��ʾ
	//{
	//	for (i = 0; i < st_FlagData.datalen; i++)
	//	{
	//		for (j = 0; j < 5; j++)	// Z����Ϊһ����ֵ��������һ�����
	//			isoSurfaceSeries.AddXYZ(i, st_FlagData.data[i], j + 5 * (st_ConfigData.testtimes - st_FlagData.remaintimes), 0, RGB(255, st_FlagData.data[i], 0));
	//	}
	//}
	//else
	//{
	//	for (i = 0; i < st_FlagData.datalen; i++)
	//	{
	//		LineSeries1.AddXY(i, st_FlagData.data[i], 0, RGB(255, 0, 0));
	//		//SurfaceSeries.AddXY(i, st_FlagData.data[i], 0, RGB(255, 0, 0));
	//	}
	//}
	//UpdateWindow();
}


// ͼ��tchart�ؼ���ʼ��
void CPMSRTestDlg::TChartInit()
{
	// ��ֹ������
	CEnvironment env = m_tchart.get_Environment();
	env.put_MouseWheelScroll(false);
	// �������ʼ��
	CAxes axes = m_tchart.get_Axis();
	CAxis leftAxis = (CAxis)axes.get_Left();
	CAxis bottomAxis = (CAxis)axes.get_Bottom();
	leftAxis.put_Visible(TRUE);
	leftAxis.put_Automatic(FALSE);
	leftAxis.put_Maximum(256);// ���������ֵ
	leftAxis.put_Minimum(0);
	leftAxis.put_Increment(10);
	bottomAxis.put_Automatic(FALSE);
	bottomAxis.put_Maximum(401);// ���������ֵ
	bottomAxis.put_Minimum(0);
	bottomAxis.put_Increment(1);
	((CAxis)axes.get_Depth()).put_Visible(TRUE);// ��ʾz��
	((CAxis)axes.get_Depth()).put_Automatic(false);
	((CAxis)axes.get_Depth()).put_Minimum(0);
	((CAxis)axes.get_Depth()).put_Maximum(20);

	((CAspect)m_tchart.get_Aspect()).put_View3D(st_FlagData.is3D);// 2D
	m_tchart.SetFocus();
}


// ��ʼ������
void CPMSRTestDlg::InitVariable()
{
	// ȫ�ֱ�����ʼ��
	st_FlagData.warningmode = WARNING_NONE;
	st_FlagData.state = RUNSTATE_IDLE;
	st_FlagData.rotating_speed = 0;
	memset(st_FlagData.data, MAX_BUFFER_SIZE, 0);
	st_FlagData.datalen = 0;
	st_FlagData.steplen = 0;
	st_FlagData.countdowncnt = 0;
	st_FlagData.is3D = FALSE;// Ĭ��2D��ʾ

	st_CommPara.bIsCommOpen = FALSE;
	st_CommPara.strReceivedData = "";
	st_CommPara.uiBytesReceived = 0;
	st_CommPara.uiBytesSent = 0;
	st_CommPara.usCommNum = 0;

	// ͨ��Э�����ʼ��
	CommandProcessInit();

	// �ؼ���ʼ��
	m_edit_Flux.SetWindowTextA("");
	m_edit_Pole.SetWindowTextA("");
}


// ��ȡ�������õĲ��������浽ȫ�ֱ�����
// �ɹ�����0��ʧ�ܷ���1
int CPMSRTestDlg::ReadParameters()
{
	CString str;
	UINT i;
	int result = 0;

	UpdateData(TRUE);

	m_edit_Flux.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.flux = i;
	if (st_ConfigData.flux == 0)
		result |= 1;
	// TODO:�������Χ�����趨Ĭ��ֵ
	// ....

	m_edit_length.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.motorlen = i;
	if (st_ConfigData.motorlen == 0)
		result |= 2;
	// TODO:�������Χ�����趨Ĭ��ֵ
	// ....

	m_edit_Pole.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.pole = i;
	if (st_ConfigData.pole == 0)
		result |= 4;
	// TODO:�������Χ�����趨Ĭ��ֵ
	// ....

	m_edit_TestTimes.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.testtimes = i;
	if (st_ConfigData.testtimes == 0)
		result |= 8;
	// TODO:�������Χ�����趨Ĭ��ֵ
	// ....

	return result;
}


// ���̿����߳�
UINT CPMSRTestDlg::FlowProcessThread(LPVOID pParam)
{
	CPMSRTestDlg* dlg = (CPMSRTestDlg*)pParam;

	while (1)
	{
		switch (st_FlagData.state)
		{
			// ��������ӳ�ʱ�䵹��ʱ
			case RUNSTATE_START0:
			{
				if (st_FlagData.countdowncnt == 0)	// �ӳ�ʱ�䵽
				{
					//::SendMessage(DlgWaiting->GetSafeHwnd(), WM_CLOSE,0,0);
					st_FlagData.remaintimes = st_ConfigData.testtimes;// ���ò��Դ�������

					// ���Ͳ�������ƶ�����
					if (st_FlagData.remaintimes > 0)
					{
						dlg->m_tchart.RemoveAllSeries();// ���ͼ����������

						dlg->SendStepLength();
						st_FlagData.remaintimes--;
						
						st_FlagData.state = RUNSTATE_ROTAING;
					}
						
					/*st_FlagData.steplen = st_ConfigData.motorlen / (st_ConfigData.testtimes + 1);
					PHeader header;
					UINT respLength = 0;
					CPInvokeInstance.HeaderInit(&header);
					header.cmd = 0xF1;
					header.len = 2;
					CPInvokeInstance.PackProtocol(&header, (UCHAR*)&st_FlagData.steplen, 2, byTxBuffer, &respLength);
					st_CommPara.m_Comm.WriteToPort(byTxBuffer, respLength);*/

					
					//// �˳��߳�
					//UINT code;
					//GetExitCodeThread(FlowProcessThread, (LPDWORD)&code);
					//AfxEndThread(code, TRUE);//return 0;
				}
			}
			break;

			case RUNSTATE_START:
			{
				// ���Ͳ�������ƶ�����
				if (st_FlagData.remaintimes > 0)
				{
					dlg->SendStepLength();
					st_FlagData.remaintimes--;

					st_FlagData.state = RUNSTATE_ROTAING;
				}
			}
			break;

			// ���ת��
			case RUNSTATE_ROTAING:
			{

			}
			break;

			// ��ò�������
			case RUNSTATE_SAMPLE:
			{
				if (st_FlagData.datalen > 0)	// ������ʱ����ʾ����
					dlg->ShowWave();
				if (st_FlagData.remaintimes == 0)
				{
					// ���н���ж�
					st_FlagData.ispass = FALSE;
					// .....

					// ֹͣ

				}
				else
				{
					st_FlagData.state = RUNSTATE_START;
				}
			}
			break;
		}


	}

	return 0;
}


// ������Ϣ����������
void CPMSRTestDlg::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;

	UpdateData(TRUE);
	m_edit_SendData.GetWindowTextA(str);
	if(m_check_SendHex.GetCheck())
	{
		TxLength = st_CommPara.m_Comm.String2Hex(str, &byTxBuffer[0]);
		st_CommPara.m_Comm.WriteToPort(&byTxBuffer[0], TxLength);
	}
	else
	{
		TxLength = str.GetLength();
		st_CommPara.m_Comm.WriteToPort(str);

	}
	st_CommPara.uiBytesSent += TxLength;
}


// ʹ�ܻ��ֹ���ò����ؼ�������
//int CPMSRTestDlg::EnableSettings(bool bEnable)
//{
//	if (bEnable)	// ʹ��
//	{
//		m_combo_type.EnableWindow(TRUE);
//		m_edit_length.EnableWindow(TRUE);
//		m_edit_TestTimes.EnableWindow(TRUE);
//		m_edit_Flux.EnableWindow(TRUE);
//		m_edit_Pole.EnableWindow(TRUE);
//		m_edit_SN.EnableWindow(TRUE);
//	}
//	else// ����
//	{
//		m_combo_type.EnableWindow(FALSE);
//		m_edit_length.EnableWindow(FALSE);
//		m_edit_TestTimes.EnableWindow(FALSE);
//		m_edit_Flux.EnableWindow(FALSE);
//		m_edit_Pole.EnableWindow(FALSE);
//		m_edit_SN.EnableWindow(FALSE);
//	}
//	return 0;
//}

// ʹ�ܻ��ֹ���ò����ؼ�������
afx_msg LRESULT CPMSRTestDlg::OnEnablesetcontrols(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnable = (BOOL)wParam;

	if (bEnable)	// ʹ��
	{
		m_combo_type.EnableWindow(TRUE);
		m_edit_length.EnableWindow(TRUE);
		m_edit_TestTimes.EnableWindow(TRUE);
		m_edit_Flux.EnableWindow(TRUE);
		m_edit_Pole.EnableWindow(TRUE);
		m_edit_SN.EnableWindow(TRUE);
	}
	else// ����
	{
		m_combo_type.EnableWindow(FALSE);
		m_edit_length.EnableWindow(FALSE);
		m_edit_TestTimes.EnableWindow(FALSE);
		m_edit_Flux.EnableWindow(FALSE);
		m_edit_Pole.EnableWindow(FALSE);
		m_edit_SN.EnableWindow(FALSE);
	}
	return 0;
}


// ����λ�����Ͳ�������ƶ�����
void CPMSRTestDlg::SendStepLength()
{
	st_FlagData.steplen = st_ConfigData.motorlen / (st_ConfigData.testtimes + 1);
	if (st_FlagData.steplen == 0)
		return;

	PHeader header;
	UINT respLength = 0;
	CPInvokeInstance.HeaderInit(&header);
	header.cmd = 0xF1;
	header.len = 2;
	CPInvokeInstance.PackProtocol(&header, (UCHAR*)&st_FlagData.steplen, 2, byTxBuffer, &respLength);
	st_CommPara.m_Comm.WriteToPort(byTxBuffer, respLength);
}

// ɾ������
afx_msg LRESULT CPMSRTestDlg::OnDeletedlgcountdown(WPARAM wParam, LPARAM lParam)
{
	//delete DlgWaiting;
	bExtiCountDown = FALSE;
	return 0;
}

// 3D��ʾ�л�
void CPMSRTestDlg::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (st_FlagData.is3D)
	{
		// �л�Ϊ2D��ʾ
		m_button_2D3D.SetWindowTextA("3D");
		((CAspect)m_tchart.get_Aspect()).put_View3D(FALSE);
		st_FlagData.is3D = FALSE;
	}
	else
	{
		// �л�Ϊ3D��ʾ
		m_button_2D3D.SetWindowTextA("2D");
		((CAspect)m_tchart.get_Aspect()).put_View3D(TRUE);
		st_FlagData.is3D = TRUE;
	}
}


// ����λ�������ж����������ָֹͣ��
void CPMSRTestDlg::SendWarningAndStop()
{
	st_FlagData.steplen = st_ConfigData.motorlen / (st_ConfigData.testtimes + 1);
	if (st_FlagData.steplen == 0)
		return;

	PHeader header;
	UINT respLength = 0;
	CPInvokeInstance.HeaderInit(&header);
	header.cmd = 0xF1;
	header.len = 2;
	CPInvokeInstance.PackProtocol(&header, (UCHAR*)&st_FlagData.steplen, 2, byTxBuffer, &respLength);
	st_CommPara.m_Comm.WriteToPort(byTxBuffer, respLength);
}
