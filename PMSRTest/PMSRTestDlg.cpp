
// PMSRTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRTestDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "PMSRSet.h"
#include "PMSRType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MODEID	0x16		// ģ��ID������ͨѶ����
#define MAX_BUFFER_SIZE		2048

typedef struct DataFlag
{
	BOOL bIsCommOpen = false;	// ���ڴ򿪱�־
	UINT uiBytesSent;		// ���͵����ֽ���
	UINT uiBytesReceived;	// ���յ����ֽ���
	CString strReceivedData;	// ���յ�����
};

OVERLAPPED osRead;
OVERLAPPED osShare;

BYTE byRxBuffer[MAX_BUFFER_SIZE];
DWORD RxLength = 0;
BYTE RxFlag = 0;

struct DataFlag st_DataFlag;
HANDLE    m_hComm;	// ���ھ��
UINT ReadComm(LPVOID pParam);

UINT TestDelay;//��ʼ������ʱ
CString ComNum;//���ڶ˿ں�
UINT BaudRate, ByteSize, StopBits, Parity;

CStatusBarCtrl m_StatusBar;

//���ݿ�
_ConnectionPtr p_Connection;

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

	ShowWindow(SW_MAXIMIZE);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	InitStatusBar(); //״̬����ʼ��
	ReadSettings();//��ȡ����
	OpenComm();//�򿪴���
	ConnectSQLServer();//�������ݿ�
	Initinterface();//��ʼ������
	InitFont();//��ʼ����������
	SetTimer(1, 1000, NULL);//������ʱ��

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
	CString str;

	if (st_DataFlag.bIsCommOpen == FALSE)
	{
		CString str1;
		str1.Format("\\\\.\\%s", ComNum);	// ���ںŴ���10ʱ���޷��򿪣���Ҫ�Ĵ�����
		m_hComm = CreateFile(str1,  //���ں�
			GENERIC_READ | GENERIC_WRITE, //ָ�����ԶԴ��ڽ��ж�д����
			0, //��ʾ����Ϊ��ռ��
			NULL,// Ȩ�޿��ƣ���ʾ���صľ�����ܱ��ӽ��̼̳С�
			OPEN_EXISTING, //��ʾ��ָ�����ڲ�����ʱ�����򽫷���ʧ��
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //��ʾ�ļ�����
			/*���򿪴���ʱ������ָ�� FILE_FLAG_OVERLAPPED���ص���ʽ��������ʾ�ļ����豸����ά������ָ�룬���ڶ�дʱ������ʹ��OVERLAPPED �ṹָ�����ʵ��ļ�ƫ������
			*/
			NULL);//��ʱ�ļ��ľ������ʹ�á�

		if (m_hComm == INVALID_HANDLE_VALUE)
		{
			//AfxMessageBox("���ڽ���ʧ�ܣ�");
			str.Format("�Ҳ���%s",ComNum);
			m_StatusBar.SetText(str, 2, 0);
			return;
		}

		/********************���뻺����������������Ĵ�С***********/
		SetupComm(m_hComm, 4096, 4096);
		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);// �����������������
		SetCommMask(m_hComm, EV_RXCHAR);// ����֪ͨ�¼���EV_RXCHAR: ���뻺����������ʱ��ͨ��WaitCommEvent�������Ի��֪ͨ

		DCB dcb;

		////���ó�ʱʱ��
		//COMMTIMEOUTS TimeOuts;
		//TimeOuts.ReadIntervalTimeout = 0;
		//TimeOuts.ReadTotalTimeoutMultiplier = 0;
		//TimeOuts.ReadTotalTimeoutConstant = 50;
		//TimeOuts.WriteTotalTimeoutMultiplier = 0;
		//TimeOuts.WriteTotalTimeoutConstant = 0;
		//SetCommTimeouts(m_hComm, &TimeOuts);

		GetCommState(m_hComm, &dcb);//��ò���  

		dcb.BaudRate = BaudRate;

		dcb.ByteSize = ByteSize;

		dcb.StopBits = TWOSTOPBITS;

		dcb.Parity = Parity; //У��λ

		dcb.fBinary = TRUE;// ָ���Ƿ����������ģʽ
		dcb.fParity = TRUE;// ָ���Ƿ�������żУ��

		//�����豸���ƿ�����ͨ���豸
		if (!SetCommState(m_hComm, &dcb))
		{
			//AfxMessageBox("�������ó���");
			m_StatusBar.SetText("�������ó���", 2, 0); 
			CloseHandle(m_hComm);
			return;
		}

		st_DataFlag.bIsCommOpen = TRUE;
		m_StatusBar.SetText("���ڴ򿪳ɹ�", 2, 0);
		// �����̶߳�ȡ��������
		//AfxBeginThread(ReadComm, this); //���������߳�
	}
	else
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		st_DataFlag.bIsCommOpen = FALSE;
	}
}

void CPMSRTestDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	KillTimer(1);
}


//
// ��ȡ�������
//
void CPMSRTestDlg::ReadSettings()
{
	char* pszFileName = "set.ini";
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
	if (settingFile.Open("set.ini", CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
	{
		CString strContent;
		CString temp;
		CString strCOMStatus;

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
					ComNum = temp; //��ȡ���ں�
					strCOMStatus = ComNum;
				}
				else if (strContent.Left(n - 1) == _T("BaudRate"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					BaudRate = atoi(temp); //��ȡ������
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("ByteSize"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					ByteSize = atoi(temp); //��ȡ����λ
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("StopBits"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					StopBits = atoi(temp); //��ȡֹͣλ
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("Parity"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					Parity = atoi(temp); //��ȡ������ʹ������
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("TestDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					TestDelay = atoi(temp); //��ȡ������ʹ������
					if ((TestDelay < 0) || (TestDelay > 90000))
					{
						TestDelay = 10000;
						AfxMessageBox(_T("set.ini�ļ��У���ʱʱ�����ô���"), MB_OK);
					}
				}
				m_StatusBar.SetText(strCOMStatus, 1, 0);
			}
		}
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
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	int strPartDim[6] = {130, 250, 350, 450, 550, -1}; //�ָ�����������Ϊ��ʼλ�ã����ǿ�ȣ�-1��ʾ�����Ҷˡ�
	m_StatusBar.SetParts(6, strPartDim);
	//��������״̬���м���ͼ��
	//m_StatusBar.SetIcon(1,SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE));//Ϊ�ڶ��������мӵ�ͼ��

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
	m_StatusBar.SetText(ComputerName, 5, 0);
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

void CPMSRTestDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CPMSRSet PMSRSet;
	PMSRSet.DoModal();
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

	m_StatusBar.SetText(date + " " + time, 0, 0);

	CDialogEx::OnTimer(nIDEvent);
}


void CPMSRTestDlg::InitFont()
{
	CFont *f = new CFont;
	f->CreateFont(36, 12, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("΢���ź�"));
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
