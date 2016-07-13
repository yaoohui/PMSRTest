
// PMSRTestDlg.cpp : 实现文件
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

#define MODEID	0x16		// 模块ID，串口通讯部分
#define MAX_BUFFER_SIZE		2048

typedef struct DataFlag
{
	BOOL bIsCommOpen = false;	// 串口打开标志
	UINT uiBytesSent;		// 发送的总字节数
	UINT uiBytesReceived;	// 接收的总字节数
	CString strReceivedData;	// 接收的数据
};

OVERLAPPED osRead;
OVERLAPPED osShare;

BYTE byRxBuffer[MAX_BUFFER_SIZE];
DWORD RxLength = 0;
BYTE RxFlag = 0;

struct DataFlag st_DataFlag;
HANDLE    m_hComm;	// 串口句柄
UINT ReadComm(LPVOID pParam);

UINT TestDelay;//开始测试延时
CString ComNum;//串口端口号
UINT BaudRate, ByteSize, StopBits, Parity;

CStatusBarCtrl m_StatusBar;

//数据库
_ConnectionPtr p_Connection;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CPMSRTestDlg 对话框


IMPLEMENT_DYNAMIC(CPMSRTestDlg, CDialogEx);

CPMSRTestDlg::CPMSRTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPMSRTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CPMSRTestDlg::~CPMSRTestDlg()
{
	// 如果该对话框有自动化代理，则
	//  将此代理指向该对话框的后向指针设置为 NULL，以便
	//  此代理知道该对话框已被删除。
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


// CPMSRTestDlg 消息处理程序

BOOL CPMSRTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	// TODO:  在此添加额外的初始化代码
	InitStatusBar(); //状态栏初始化
	ReadSettings();//读取设置
	OpenComm();//打开串口
	ConnectSQLServer();//连接数据库
	Initinterface();//初始化界面
	InitFont();//初始化界面字体
	SetTimer(1, 1000, NULL);//启动定时器

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPMSRTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPMSRTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

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
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

// 打开串口
void CPMSRTestDlg::OpenComm()
{
	CString str;

	if (st_DataFlag.bIsCommOpen == FALSE)
	{
		CString str1;
		str1.Format("\\\\.\\%s", ComNum);	// 串口号大于10时，无法打开，需要改串口名
		m_hComm = CreateFile(str1,  //串口号
			GENERIC_READ | GENERIC_WRITE, //指定可以对串口进行读写操作
			0, //表示串口为独占打开
			NULL,// 权限控制，表示返回的句柄不能被子进程继承。
			OPEN_EXISTING, //表示当指定串口不存在时，程序将返回失败
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //表示文件属性
			/*当打开串口时，必须指定 FILE_FLAG_OVERLAPPED（重叠方式），它表示文件或设备不会维护访问指针，则在读写时，必须使用OVERLAPPED 结构指定访问的文件偏移量。
			*/
			NULL);//临时文件的句柄，不使用。

		if (m_hComm == INVALID_HANDLE_VALUE)
		{
			//AfxMessageBox("串口建立失败！");
			str.Format("找不到%s",ComNum);
			m_StatusBar.SetText(str, 2, 0);
			return;
		}

		/********************输入缓冲区和输出缓冲区的大小***********/
		SetupComm(m_hComm, 4096, 4096);
		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);// 清空输入和输出缓冲区
		SetCommMask(m_hComm, EV_RXCHAR);// 设置通知事件。EV_RXCHAR: 输入缓冲区有数据时，通过WaitCommEvent函数可以获得通知

		DCB dcb;

		////设置超时时间
		//COMMTIMEOUTS TimeOuts;
		//TimeOuts.ReadIntervalTimeout = 0;
		//TimeOuts.ReadTotalTimeoutMultiplier = 0;
		//TimeOuts.ReadTotalTimeoutConstant = 50;
		//TimeOuts.WriteTotalTimeoutMultiplier = 0;
		//TimeOuts.WriteTotalTimeoutConstant = 0;
		//SetCommTimeouts(m_hComm, &TimeOuts);

		GetCommState(m_hComm, &dcb);//获得参数  

		dcb.BaudRate = BaudRate;

		dcb.ByteSize = ByteSize;

		dcb.StopBits = TWOSTOPBITS;

		dcb.Parity = Parity; //校验位

		dcb.fBinary = TRUE;// 指定是否允许二进制模式
		dcb.fParity = TRUE;// 指定是否允许奇偶校验

		//根据设备控制块配置通信设备
		if (!SetCommState(m_hComm, &dcb))
		{
			//AfxMessageBox("串口设置出错！");
			m_StatusBar.SetText("串口设置出错！", 2, 0); 
			CloseHandle(m_hComm);
			return;
		}

		st_DataFlag.bIsCommOpen = TRUE;
		m_StatusBar.SetText("串口打开成功", 2, 0);
		// 创建线程读取串口数据
		//AfxBeginThread(ReadComm, this); //开读串口线程
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
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	KillTimer(1);
}


//
// 读取软件设置
//
void CPMSRTestDlg::ReadSettings()
{
	char* pszFileName = "set.ini";
	CStdioFile settingFile;
	CFileException fileException;

	//写文件
	//if(settingFile.Open(pszFileName,CFile::typeText|CFile::modeCreate|CFile::modeReadWrite),&fileException)
	//{
	//	settingFile.WriteString("第1行\n");
	//	CString strOrder;
	//	strOrder.Format("%d,%.3f",66,88.88);
	//	settingFile.WriteString(strOrder);
	//}
	//else
	//{
	//	TRACE("Can't open file %s,error=%u\n",pszFileName,fileException.m_cause);
	//}

	//读文件
	if (settingFile.Open("set.ini", CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
	{
		CString strContent;
		CString temp;
		CString strCOMStatus;

		while (settingFile.ReadString(strContent))
		{
			if (strContent.Left(2) != _T("//")) //取前两个字符，如果不是“//”则往下运行
			{
				int n = 0, m = 0, l = 0;

				n = strContent.Find('='); //获得“=”的位置
				m = strContent.Find(';'); //获得“；”的位置
				l = strContent.GetLength(); //获得字符串长度 l-n-2

				if (strContent.Left(n - 1) == _T("ComNum")) //判断是否与要求的字符串相同
				{
					//strcpy(temp, strContent.Mid(n+1,l-n-2));
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					ComNum = temp; //获取串口号
					strCOMStatus = ComNum;
				}
				else if (strContent.Left(n - 1) == _T("BaudRate"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					BaudRate = atoi(temp); //获取比特率
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("ByteSize"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					ByteSize = atoi(temp); //获取数据位
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("StopBits"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					StopBits = atoi(temp); //获取停止位
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("Parity"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					Parity = atoi(temp); //获取传感器使能设置
					strCOMStatus = strCOMStatus + " " + temp;
				}
				else if (strContent.Left(n - 1) == _T("TestDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					TestDelay = atoi(temp); //获取传感器使能设置
					if ((TestDelay < 0) || (TestDelay > 90000))
					{
						TestDelay = 10000;
						AfxMessageBox(_T("set.ini文件中，延时时间设置错误！"), MB_OK);
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
// 状态栏初始化
//
void CPMSRTestDlg::InitStatusBar()
{
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	int strPartDim[6] = {130, 250, 350, 450, 550, -1}; //分割数量，数字为起始位置，不是宽度，-1表示到最右端。
	m_StatusBar.SetParts(6, strPartDim);
	//下面是在状态栏中加入图标
	//m_StatusBar.SetIcon(1,SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE));//为第二个分栏中加的图标

}



//
// 连接数据库
//
void CPMSRTestDlg::ConnectSQLServer()
{
	CString strSQL;
	char ComputerName[100];
	DWORD NameSize = 100;
	HRESULT hr;

	GetComputerName(ComputerName, &NameSize);//获取计算机名称
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

	catch (_com_error e)///捕捉异常
	{
		//CString errormessage;
		//errormessage.Format("连接数据库失败!\r\n错误信息:%s",e.ErrorMessage());
		AfxMessageBox(e.ErrorMessage());///显示错误信息
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
	// TODO:  在此添加控件通知处理程序代码
	CPMSRSet PMSRSet;
	PMSRSet.DoModal();
}


void CPMSRTestDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值
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
	f->CreateFont(36, 12, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
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
	// TODO:  在此添加控件通知处理程序代码
	ShellExecute(NULL, "open", "C:\\Program Files\\Common Files\\Microsoft Shared\\Ink\\TabTip.exe", NULL, NULL, SW_SHOWNORMAL);//打开软键盘
}
