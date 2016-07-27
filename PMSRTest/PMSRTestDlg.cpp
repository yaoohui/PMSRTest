
// PMSRTestDlg.cpp : 实现文件
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
#include "CAspect.h"// 用于3D视图显示
#include "CSurfaceSeries.h"
#include "CIsoSurfaceSeries.h"// 3d序列
#include "CWaterfallSeries.h"
#include "CPen0.h"
#include "CBrush0.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum EM_RunState
{
	RUNSTATE_IDLE = 0,	// 无
	RUNSTATE_STOP,		// 停止
	RUNSTATE_START0,	// 启动被测电机，等待启动延时
	RUNSTATE_START,		// 
	RUNSTATE_ROTAING,	// 测量转速
	RUNSTATE_SAMPLE,	// 采集数据
};
enum EM_WaringMode
{
	WARNING_NONE = 0,	// 无
	WARNING_LIMIT,		// 达到限位
	WARNING_PASS,		// 合格
	WARNING_NOGOOD,		// 不合格
};
//#define MODEID	0x17		// 模块ID，串口通讯部分
#define MAX_BUFFER_SIZE		2048
#define ID_STATUS_BAR_CTRL  102	// 状态栏ID

struct FlagDataTypeDef// 标志结构体
{
	UCHAR state;		// 运行状态。取值见枚举 EM_RunState
	UCHAR warningmode;	// 报警类型，01：达到限位，02：合格，03：不合格。取值见枚举 EM_WaringMode
	FLOAT rotating_speed;// 转速，单位：r/min 转/分钟
	UCHAR data[MAX_BUFFER_SIZE];			// 采样数据
	UINT datalen;		// 采样数据长度
	USHORT steplen;		// 步进电机移动步长，2字节，单位：mm
	UINT countdowncnt;	// 倒计时计数 
	BOOL is3D;			// 是否3D显示
	UINT remaintimes;	// 剩余次数
	BOOL ispass;		// 判定结果，true为合格，false为不合格
};


BYTE byRxBuffer[MAX_BUFFER_SIZE];
UINT RxLength = 0;
BYTE RxFlag = 0;
BYTE byTxBuffer[MAX_BUFFER_SIZE];
UINT TxLength = 0;

//HANDLE    m_hComm;	// 串口句柄
//UINT ReadComm(LPVOID pParam);

//UINT TestDelay;//开始测试延时
//CString ComNum;//串口端口号
//UINT BaudRate, ByteSize, StopBits, Parity;

CStatusBarCtrl m_StatusBar;

//数据库
_ConnectionPtr p_Connection;

// 全局结构体变量，保存设置的变量
struct CommPara st_CommPara;
struct ConfigVal st_ConfigData;
struct FlagDataTypeDef st_FlagData;
CString strIniFileName = "set.ini";

CPMSRSet dlgSettings;
CommunicationProtocol CPInvokeInstance;// 通信协议对象
CEditLog  m_EditLogger;// 用于显示接收数据的实例

//状态栏分块名称
enum EM_StatusBarParts
{
	SBPART_TIME = 0,	// 时间栏
	SBPART_COMM,		// 通信接口名及波特率
	SBPART_COMM_STATE,	// 通信接口打开状态
	SBPART_RCOUNT,		// 接收数据个数
	SBPART_SCOUNT,		// 发送数据个数
	SBPART_DATABASE,	// 数据库
	SBPART_PCNAME,		// 当前计算机
};

#define SHOW_DEBUG	// 定义此，则显示串口调试部分控件
BOOL bExtiCountDown=FALSE;	// 是否退出倒计时


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
//	, m_tchart(0)
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

	// TODO:  在此添加额外的初始化代码
	CWnd::SetWindowPos(NULL, 0, 0, 1350, 850, SWP_NOZORDER | SWP_NOMOVE);// 窗体初始大小
	ShowWindow(SW_MAXIMIZE);
	ShowDebugControls();// 调试控件显示/不显示
	m_check_ReceiveHex.SetCheck(BST_CHECKED);
	m_check_SendHex.SetCheck(BST_CHECKED);

	LayoutFrame();	// 窗体布局
	InitStatusBar(); //状态栏初始化
	InitVariable();	 // 初始化变量
	st_CommPara.m_Comm.FindComPort();	// 查找可用的串口
	ReadSettings(); //读取设置
	OpenComm();//打开串口。必须在读取设置ReadSettings()运行后，再调用
	ConnectSQLServer();//连接数据库
	Initinterface();//初始化界面
	InitFont();//初始化界面字体
	SetTimer(1, 1000, NULL);//启动定时器
	TChartInit();	// 图表控件初始化，主要是坐标轴设置

	m_EditLogger.SetEditCtrl(m_edit_ReceiveData.m_hWnd);	// CEditLog类变量关联接收文本框

	
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
	if (!st_CommPara.bIsCommOpen)// 串口未打开
	{
		if (st_CommPara.m_Comm.InitPort(this->m_hWnd, st_CommPara.uiCommName, st_CommPara.uiBaudRate, st_CommPara.uiParity, st_CommPara.uiByteSize,
			st_CommPara.uiStopBits, EV_RXCHAR | EV_BREAK, MAX_BUFFER_SIZE))
		{
			st_CommPara.bIsCommOpen = TRUE;
			m_StatusBar.SetText("串口打开成功", SBPART_COMM_STATE, 0);
			CString strCOMStatus;
			CString str;
			str.Format("%d", st_CommPara.uiBaudRate);
			strCOMStatus = st_CommPara.strCommName + ", " + str + "bps, N81";
			m_StatusBar.SetText(strCOMStatus, SBPART_COMM, 0);

			st_CommPara.m_Comm.StartMonitoring();// 启动串口类监听
		}
		else
		{
			m_StatusBar.SetText("串口设置出错！", SBPART_COMM_STATE, 0);
		}

	}
	else// 串口已打开
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
//			AfxMessageBox("无串口！");
//			m_StatusBar.SetText("无串口！", 2, 0);
//			return;
//		}
//
//		str1.Format("\\\\.\\%s", st_CommPara.strCommName);	// 串口号大于10时，无法打开，需要改串口名
//		m_hComm = CreateFile(str1,  //串口号
//			GENERIC_READ | GENERIC_WRITE, //指定可以对串口进行读写操作
//			0, //表示串口为独占打开
//			NULL,// 权限控制，表示返回的句柄不能被子进程继承。
//			OPEN_EXISTING, //表示当指定串口不存在时，程序将返回失败
//			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //表示文件属性
//			/*当打开串口时，必须指定 FILE_FLAG_OVERLAPPED（重叠方式），它表示文件或设备不会维护访问指针，则在读写时，必须使用OVERLAPPED 结构指定访问的文件偏移量。
//			*/
//			NULL);//临时文件的句柄，不使用。
//
//		if (m_hComm == INVALID_HANDLE_VALUE)
//		{
//			//AfxMessageBox("串口建立失败！");
//			str.Format("找不到%s", st_CommPara.strCommName);
//			m_StatusBar.SetText(str, 2, 0);
//			return;
//		}
//
//		/********************输入缓冲区和输出缓冲区的大小***********/
//		SetupComm(m_hComm, 4096, 4096);
//		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);// 清空输入和输出缓冲区
//		SetCommMask(m_hComm, EV_RXCHAR);// 设置通知事件。EV_RXCHAR: 输入缓冲区有数据时，通过WaitCommEvent函数可以获得通知
//
//		DCB dcb;
//
//		////设置超时时间
//		//COMMTIMEOUTS TimeOuts;
//		//TimeOuts.ReadIntervalTimeout = 0;
//		//TimeOuts.ReadTotalTimeoutMultiplier = 0;
//		//TimeOuts.ReadTotalTimeoutConstant = 50;
//		//TimeOuts.WriteTotalTimeoutMultiplier = 0;
//		//TimeOuts.WriteTotalTimeoutConstant = 0;
//		//SetCommTimeouts(m_hComm, &TimeOuts);
//
//		GetCommState(m_hComm, &dcb);//获得参数  
//
//		dcb.BaudRate = st_CommPara.uiBaudRate;
//
//		dcb.ByteSize = 8;// st_CommPara.uiByteSize;
//
//		dcb.StopBits = ONESTOPBIT;// st_CommPara.uiStopBits;// TWOSTOPBITS;
//
//		dcb.Parity = NOPARITY;// st_CommPara.uiParity; //校验位
//
//		dcb.fBinary = TRUE;// 指定是否允许二进制模式
//		dcb.fParity = TRUE;// 指定是否允许奇偶校验
//
//		//根据设备控制块配置通信设备
//		if (!SetCommState(m_hComm, &dcb))
//		{
//			//AfxMessageBox("串口设置出错！");
//			m_StatusBar.SetText("串口设置出错！", 2, 0); 
//			CloseHandle(m_hComm);
//			return;
//		}
//
//		st_CommPara.bIsCommOpen = TRUE;
//		m_StatusBar.SetText("串口打开成功", 2, 0);
//		// 创建线程读取串口数据
//		//AfxBeginThread(ReadComm, this); //开读串口线程
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
	// TODO:  在此添加控件通知处理程序代码
	KillTimer(1);
	CDialogEx::OnOK();
}


//
// 读取软件设置
//
void CPMSRTestDlg::ReadSettings()
{
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
	if (settingFile.Open(strIniFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
	{
		CString strContent;
		CString temp;
		//CString strCOMStatus;

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
					st_CommPara.strCommName = temp; //获取串口号
					//strCOMStatus = temp;
					st_CommPara.uiCommName = atoi(temp.Mid(3));
				}
				else if (strContent.Left(n - 1) == _T("BaudRate"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					st_CommPara.uiBaudRate = atoi(temp); //获取比特率
					//strCOMStatus = strCOMStatus + ", " + temp + "bps, N81";
				}
				//else if (strContent.Left(n - 1) == _T("ByteSize"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
				//	st_CommPara.uiByteSize = atoi(temp); //获取数据位
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				//else if (strContent.Left(n - 1) == _T("StopBits"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
				//	StopBits = atoi(temp); //获取停止位
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				//else if (strContent.Left(n - 1) == _T("Parity"))
				//{
				//	temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
				//	Parity = atoi(temp); //获取传感器使能设置
				//	strCOMStatus = strCOMStatus + " " + temp;
				//}
				else if (strContent.Left(n - 1) == _T("StartDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					st_ConfigData.StartDelay = atoi(temp); //获取传感器使能设置
					if ((st_ConfigData.StartDelay < 0) || (st_ConfigData.StartDelay > 900))
					{
						st_ConfigData.StartDelay = 100;
						AfxMessageBox(_T("set.ini文件中，延时时间设置错误！"), MB_OK);
					}
				}
				//m_StatusBar.SetText(strCOMStatus, SBPART_COMM, 0);
			}
		}
		// 判断是否正确读取串口号，无效时，更新串口号
		if (st_CommPara.strCommName == "" || st_CommPara.uiCommName == 0)
		{
			if (st_CommPara.m_Comm.m_ComCount > 0)// 注册表中有可用串口
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
// 状态栏初始化
//
void CPMSRTestDlg::InitStatusBar()
{
	m_StatusBar.SetText("RX:0", SBPART_RCOUNT, 0);
	m_StatusBar.SetText("TX:0", SBPART_SCOUNT, 0);

	//m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	//int strPartDim[6] = {130, 250, 350, 450, 550, -1}; //分割数量，数字为起始位置，不是宽度，-1表示到最右端。
	//m_StatusBar.SetParts(6, strPartDim);
	////下面是在状态栏中加入图标
	////m_StatusBar.SetIcon(1,SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE));//为第二个分栏中加的图标

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
// 设置 按钮
void CPMSRTestDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	CPMSRSet PMSRSet;
	PMSRSet.DoModal();

	// 重新设置串口
	st_CommPara.m_Comm.ClosePort();
	st_CommPara.bIsCommOpen = FALSE;
	OpenComm();
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

	m_StatusBar.SetText(date + " " + time, SBPART_TIME, 0);

	// 电机启动延迟时间倒计时
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
	f->CreateFont(30, 0, 0, 0, 0, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
	//f->CreateFont(36, 12, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
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
	// TODO:  在此添加控件通知处理程序代码
	ShellExecute(NULL, "open", "C:\\Program Files\\Common Files\\Microsoft Shared\\Ink\\TabTip.exe", NULL, NULL, SW_SHOWNORMAL);//打开软键盘
}



// 主窗体布局
void CPMSRTestDlg::LayoutFrame()
{
	CRect RectClient, RectTemp; 
	UINT uiLeft = 0, uiTop = 0, uiWidth = 0, uiHeight = 0;

	GetClientRect(&RectClient); 
	// group"测试参数"
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop = 40;
	uiWidth = RectClient.Width() / 3 - 20;
	uiHeight = 200;
	m_group_Parameter.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);
	
	// "型号"
	uiLeft += 20;
	uiTop += 50;
	m_static_Type.SetWindowPos(this, uiLeft, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Type.GetClientRect(&RectTemp);
	m_combo_type.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "长度"
	uiTop += 50;
	m_static_Length.SetWindowPos(this, uiLeft, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Length.GetClientRect(&RectTemp);
	m_edit_length.SetWindowPos(this, uiLeft+RectTemp.Width()+2, uiTop, 0,0, SWP_NOSIZE | SWP_NOZORDER);
	// "磁通量"
	m_static_Flux.SetWindowPos(this, uiLeft + uiWidth / 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Flux.GetClientRect(&RectTemp);
	m_edit_Flux.SetWindowPos(this, uiLeft + uiWidth / 2 + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "测试次数"
	uiTop += 50;
	m_static_TestTimes.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_TestTimes.GetClientRect(&RectTemp);
	m_edit_TestTimes.SetWindowPos(this, uiLeft+RectTemp.Width()+2, uiTop, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	// "磁极数"
	m_static_Pole.SetWindowPos(this, uiLeft + uiWidth / 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Pole.GetClientRect(&RectTemp);
	m_edit_Pole.SetWindowPos(this, uiLeft + uiWidth / 2 + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


	// group"测试结果"
	m_group_Parameter.GetWindowRect(&RectTemp);
	uiLeft = RectClient.Width() * 2 / 3;
	uiTop = RectTemp.bottom + 6;//uiHeight + 50;
	uiWidth = RectTemp.Width();//RectClient.Width() / 3 - 20;
	uiHeight = 200;
	m_group_Result.SetWindowPos(this, uiLeft, uiTop, uiWidth, uiHeight, SWP_NOZORDER);

	// "转子编号"
	uiLeft += 20;
	uiTop += 50;
	m_static_SN.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_SN.GetClientRect(&RectTemp);
	m_edit_SN.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// "重量"
	uiTop += 50;
	m_static_Weight.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Weight.GetClientRect(&RectTemp);
	m_edit_Weight.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	// "测试结果"
	uiTop += 50;
	m_static_Result.SetWindowPos(this, uiLeft, uiTop, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_static_Result.GetClientRect(&RectTemp);
	m_edit_Result.SetWindowPos(this, uiLeft + RectTemp.Width() + 2, uiTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 按钮
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

	// ======测试用===========
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
	// END======测试用===========

	// 图表
	m_tchart.SetWindowPos(this, 50, 50, RectClient.Width()*2/3 - 100, RectClient.Height() - 100, SWP_NOZORDER);

	// 状态栏
	uiWidth = RectClient.Width()/6;
	int strPartDim[7] = { uiWidth, uiWidth * 2, uiWidth * 3, uiWidth/2 * 7, uiWidth/2*8, uiWidth * 5, -1 }; //分割数量，数字为起始位置，不是宽度，-1表示到最右端。
	m_StatusBar.SetParts(7, strPartDim);
	m_StatusBar.SetWindowPos(0, RectClient.left, RectClient.top, RectClient.Width(), RectClient.Height() - 20, 0);
}


void CPMSRTestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (::IsWindow(m_static_Type.m_hWnd))// 控件有效后
		LayoutFrame();

	CRect Rect;
	GetClientRect(&Rect);
	m_StatusBar.SetWindowPos(0, Rect.left,Rect.top, Rect.Width(), Rect.Height()-20, 0);
}


int CPMSRTestDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW | CCS_BOTTOM | SBARS_SIZEGRIP, CRect(0, 0, 0, 0), this, ID_STATUS_BAR_CTRL);//CCS_NOPARENTALIGN
	return 0;
}


void CPMSRTestDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	// 设置窗体的最小尺寸
	lpMMI->ptMinTrackSize.x = 1350;
	lpMMI->ptMinTrackSize.y = 850;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}




// 通信协议类初始化
void CPMSRTestDlg::CommandProcessInit()
{
	CPInvokeInstance.Init(&CPInvokeInstance, NULL);// 用于通信协议关联实例
}


// 串口接收数据处理
void CPMSRTestDlg::CommandPress()
{
	UCHAR verfy;

	if (RxLength == 0)
		return;

	verfy = CPInvokeInstance.IsValidProtocol(byRxBuffer, RxLength);
	if (verfy == 1)//长度不足
	{
		return;
	}
	else if (verfy > 1)// 其他错误
	{
		RxLength = 0;
		return;
	}

	if (CPInvokeInstance.rcvHeader->modeid != MODEID)	// 模块编号错误，返回
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
		case 0xF1://启动被测电机，计算步进电机移动步长
			bNeedResponse = FALSE;
			if (!ReadParameters())// 读取设置参数成功
			{
				// 发送启动被测电机指令
				CPInvokeInstance.rcvHeader->cmd = 0xF8;
				CPInvokeInstance.PackProtocol(CPInvokeInstance.rcvHeader, CPInvokeInstance.payload, CPInvokeInstance.payloadLength, byRxBuffer, &respLength);
				st_CommPara.m_Comm.WriteToPort(byRxBuffer, respLength);

				st_FlagData.countdowncnt = st_ConfigData.StartDelay;
				if (st_FlagData.countdowncnt == 0)// 倒计时数据异常
					return (void)AfxMessageBox("启动延时时间为0！");

				// 显示倒计时界面
				DlgWaiting = new CDlgWaiting();
				DlgWaiting->Create(IDD_DLG_COUNTDOWN);
				if (DlgWaiting == NULL)
					return (void)AfxMessageBox("倒计时窗口句柄创建失败！");

				bExtiCountDown = TRUE;
				this->EnableWindow(FALSE);
				DlgWaiting->ShowWindow(SW_SHOW);
				OnEnablesetcontrols(FALSE,0);//EnableSettings(FALSE);// 输入控件为灰色
				DlgWaiting->SendMessage(WM_WAITCOUNTDOWN, st_FlagData.countdowncnt, 0);

				st_FlagData.state = RUNSTATE_START0;	// 启动被测电机，开始计时，延时设定时长后，发送步进电机移动命令
				// 创建流程处理线程
				AfxBeginThread(FlowProcessThread, this);
			}
			else// 读取失败
			{
				AfxMessageBox("参数未设置！\r\n请按下停止按钮，设置完整参数后，再按下启动。", MB_OK | MB_ICONEXCLAMATION);
			}
			break;

		case 0xF3:// 报警
			if (CPInvokeInstance.payload[0] == WARNING_LIMIT)// 达到右限位
				st_FlagData.warningmode = WARNING_LIMIT;
			break;

		case 0xF2:// 停止
			st_FlagData.state = RUNSTATE_STOP;
			InitVariable();
			OnEnablesetcontrols(TRUE, 0);// 输入控件为使能
			m_tchart.RemoveAllSeries();	// 图表清除所有曲线
			// 退出线程
			UINT code;
			GetExitCodeThread(FlowProcessThread, (LPDWORD)&code);
			AfxEndThread(code, TRUE);
			break;

		case 0xF4:// 转速
			bNeedResponse = FALSE;
			st_FlagData.rotating_speed = (FLOAT)(*(USHORT*)CPInvokeInstance.payload) / (FLOAT)10.0;
			st_FlagData.state = RUNSTATE_ROTAING;
			break;
		
		case 0xF5:// 采集数据
			bNeedResponse = FALSE;
			st_FlagData.datalen = CPInvokeInstance.rcvHeader->len;
			memcpy(st_FlagData.data, CPInvokeInstance.payload, st_FlagData.datalen);
			st_FlagData.state = RUNSTATE_SAMPLE;
			break;

		default:
			bNeedResponse = FALSE;
			break;
	}

	if (bNeedResponse)  //如果需要由外部统一给出响应，则调用下面的函数
	{
		CPInvokeInstance.PackResponse(CPInvokeInstance.rcvHeader, CPInvokeInstance.payload, CPInvokeInstance.payloadLength, byRxBuffer, &respLength);
		st_CommPara.m_Comm.WriteToPort(byRxBuffer, respLength);
	}

	RxLength = 0;

}

// 串口类接收消息
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
	
	// 把数据保存到缓冲区
	byRxBuffer[RxLength] = ch;
	RxLength++;

	st_CommPara.uiBytesReceived++;
	str.Format("RX:%d", st_CommPara.uiBytesReceived);
	m_StatusBar.SetText(str, SBPART_RCOUNT, 0);

	return 0;
}

// 串口类接收检测到break。接收过程中出现多次break
afx_msg LRESULT CPMSRTestDlg::OnCommBreakDetected(WPARAM wParam, LPARAM lParam)
{
#ifdef SHOW_DEBUG
	m_EditLogger.AddText("$$");
	UpdateData(FALSE);
#endif
	CommandPress();

	return 0;
}

// 串口类接收到0xAA 测试看不出效果
afx_msg LRESULT CPMSRTestDlg::OnCommRxflagDetected(WPARAM wParam, LPARAM lParam)
{
#ifdef SHOW_DEBUG
	m_EditLogger.AddText("@@");
	UpdateData(FALSE);
#endif
	return 0;
}


// 清除调试控件
void CPMSRTestDlg::OnBnClickedButton9()
{
	// TODO:  在此添加控件通知处理程序代码
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

	// 清除图表
	CSeries LineSeries1 = m_tchart.Series(0);// 直线序列
	LineSeries1.Clear();	// 清空数据

}


// 显示或隐藏调试用控件，根据是否有宏定义SHOW_DEBUG确定调试用控件是否显示
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


// 在图表tchart上显示波形
void CPMSRTestDlg::ShowWave()
{
	//CSeries LineSeries1;// = m_tchart.Series(0);// 直线序列
	//CSeries SurfaceSeries;// = m_tchart.Series(1);// surface序列
	//CIsoSurfaceSeries isoSurfaceSeries;
	//UINT i,j;

	CSeries LineSeries;
	CSeries SurfaceSeries;
	CIsoSurfaceSeries IsoSurfaceSeries;
	CWaterfallSeries WaterfallSeries;

	// 添加曲线序列
	if (st_FlagData.is3D)
	{
		//m_tchart.AddSeries(scIsoSurface);
		m_tchart.AddSeries(scWaterfall);
		SurfaceSeries = m_tchart.Series(st_ConfigData.testtimes - st_FlagData.remaintimes - 1);
		
		//IsoSurfaceSeries = SurfaceSeries.get_asIsoSurface();// 3D序列关联2D
		WaterfallSeries = SurfaceSeries.get_asWaterfall();
		((CBrush0)WaterfallSeries.get_Brush()).put_Style(bsClear);// 无填充
		((CPen0)WaterfallSeries.get_WaterLines()).put_Visible(false);// 无支持线
		((CPen0)SurfaceSeries.get_Pen()).put_Color(RGB(255, 0, 0));// 曲线颜色
	}
	else
	{
		m_tchart.AddSeries(scLine);
		LineSeries = m_tchart.Series(st_ConfigData.testtimes - st_FlagData.remaintimes - 1);
	}

	// 下面使用AddArray函数绘图，比AddXY绘图速度快，不需要每次刷新
	COleSafeArray XValues, YValues;
	COleSafeArray ZValues;
	DWORD wLength = st_FlagData.datalen;
	DWORD zLength = 5;
	XValues.Create(VT_R8, 1, &wLength);//VT_R8就是指double 
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

	//LineSeries1.Clear();	// 清空数据
	//SurfaceSeries.Clear();

	//if (st_FlagData.is3D)	// 3d显示
	//{
	//	for (i = 0; i < st_FlagData.datalen; i++)
	//	{
	//		for (j = 0; j < 5; j++)	// Z不能为一个定值，必须有一定宽度
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


// 图表tchart控件初始化
void CPMSRTestDlg::TChartInit()
{
	// 禁止鼠标滚轮
	CEnvironment env = m_tchart.get_Environment();
	env.put_MouseWheelScroll(false);
	// 坐标轴初始化
	CAxes axes = m_tchart.get_Axis();
	CAxis leftAxis = (CAxis)axes.get_Left();
	CAxis bottomAxis = (CAxis)axes.get_Bottom();
	leftAxis.put_Visible(TRUE);
	leftAxis.put_Automatic(FALSE);
	leftAxis.put_Maximum(256);// 纵坐标最大值
	leftAxis.put_Minimum(0);
	leftAxis.put_Increment(10);
	bottomAxis.put_Automatic(FALSE);
	bottomAxis.put_Maximum(401);// 横坐标最大值
	bottomAxis.put_Minimum(0);
	bottomAxis.put_Increment(1);
	((CAxis)axes.get_Depth()).put_Visible(TRUE);// 显示z轴
	((CAxis)axes.get_Depth()).put_Automatic(false);
	((CAxis)axes.get_Depth()).put_Minimum(0);
	((CAxis)axes.get_Depth()).put_Maximum(20);

	((CAspect)m_tchart.get_Aspect()).put_View3D(st_FlagData.is3D);// 2D
	m_tchart.SetFocus();
}


// 初始化变量
void CPMSRTestDlg::InitVariable()
{
	// 全局变量初始化
	st_FlagData.warningmode = WARNING_NONE;
	st_FlagData.state = RUNSTATE_IDLE;
	st_FlagData.rotating_speed = 0;
	memset(st_FlagData.data, MAX_BUFFER_SIZE, 0);
	st_FlagData.datalen = 0;
	st_FlagData.steplen = 0;
	st_FlagData.countdowncnt = 0;
	st_FlagData.is3D = FALSE;// 默认2D显示

	st_CommPara.bIsCommOpen = FALSE;
	st_CommPara.strReceivedData = "";
	st_CommPara.uiBytesReceived = 0;
	st_CommPara.uiBytesSent = 0;
	st_CommPara.usCommNum = 0;

	// 通信协议类初始化
	CommandProcessInit();

	// 控件初始化
	m_edit_Flux.SetWindowTextA("");
	m_edit_Pole.SetWindowTextA("");
}


// 读取界面设置的参数，保存到全局变量中
// 成功返回0，失败返回1
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
	// TODO:如果超范围，则设定默认值
	// ....

	m_edit_length.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.motorlen = i;
	if (st_ConfigData.motorlen == 0)
		result |= 2;
	// TODO:如果超范围，则设定默认值
	// ....

	m_edit_Pole.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.pole = i;
	if (st_ConfigData.pole == 0)
		result |= 4;
	// TODO:如果超范围，则设定默认值
	// ....

	m_edit_TestTimes.GetWindowTextA(str);
	i = atoi(str);
	st_ConfigData.testtimes = i;
	if (st_ConfigData.testtimes == 0)
		result |= 8;
	// TODO:如果超范围，则设定默认值
	// ....

	return result;
}


// 流程控制线程
UINT CPMSRTestDlg::FlowProcessThread(LPVOID pParam)
{
	CPMSRTestDlg* dlg = (CPMSRTestDlg*)pParam;

	while (1)
	{
		switch (st_FlagData.state)
		{
			// 电机启动延迟时间倒计时
			case RUNSTATE_START0:
			{
				if (st_FlagData.countdowncnt == 0)	// 延迟时间到
				{
					//::SendMessage(DlgWaiting->GetSafeHwnd(), WM_CLOSE,0,0);
					st_FlagData.remaintimes = st_ConfigData.testtimes;// 设置测试次数计数

					// 发送步进电机移动步长
					if (st_FlagData.remaintimes > 0)
					{
						dlg->m_tchart.RemoveAllSeries();// 清除图表所有曲线

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

					
					//// 退出线程
					//UINT code;
					//GetExitCodeThread(FlowProcessThread, (LPDWORD)&code);
					//AfxEndThread(code, TRUE);//return 0;
				}
			}
			break;

			case RUNSTATE_START:
			{
				// 发送步进电机移动步长
				if (st_FlagData.remaintimes > 0)
				{
					dlg->SendStepLength();
					st_FlagData.remaintimes--;

					st_FlagData.state = RUNSTATE_ROTAING;
				}
			}
			break;

			// 获得转速
			case RUNSTATE_ROTAING:
			{

			}
			break;

			// 获得采样数据
			case RUNSTATE_SAMPLE:
			{
				if (st_FlagData.datalen > 0)	// 有数据时，显示波形
					dlg->ShowWave();
				if (st_FlagData.remaintimes == 0)
				{
					// 进行结果判定
					st_FlagData.ispass = FALSE;
					// .....

					// 停止

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


// 调试信息，发送数据
void CPMSRTestDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
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


// 使能或禁止设置参数控件的输入
//int CPMSRTestDlg::EnableSettings(bool bEnable)
//{
//	if (bEnable)	// 使能
//	{
//		m_combo_type.EnableWindow(TRUE);
//		m_edit_length.EnableWindow(TRUE);
//		m_edit_TestTimes.EnableWindow(TRUE);
//		m_edit_Flux.EnableWindow(TRUE);
//		m_edit_Pole.EnableWindow(TRUE);
//		m_edit_SN.EnableWindow(TRUE);
//	}
//	else// 禁用
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

// 使能或禁止设置参数控件的输入
afx_msg LRESULT CPMSRTestDlg::OnEnablesetcontrols(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnable = (BOOL)wParam;

	if (bEnable)	// 使能
	{
		m_combo_type.EnableWindow(TRUE);
		m_edit_length.EnableWindow(TRUE);
		m_edit_TestTimes.EnableWindow(TRUE);
		m_edit_Flux.EnableWindow(TRUE);
		m_edit_Pole.EnableWindow(TRUE);
		m_edit_SN.EnableWindow(TRUE);
	}
	else// 禁用
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


// 向下位机发送步进电机移动距离
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

// 删除对象
afx_msg LRESULT CPMSRTestDlg::OnDeletedlgcountdown(WPARAM wParam, LPARAM lParam)
{
	//delete DlgWaiting;
	bExtiCountDown = FALSE;
	return 0;
}

// 3D显示切换
void CPMSRTestDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	if (st_FlagData.is3D)
	{
		// 切换为2D显示
		m_button_2D3D.SetWindowTextA("3D");
		((CAspect)m_tchart.get_Aspect()).put_View3D(FALSE);
		st_FlagData.is3D = FALSE;
	}
	else
	{
		// 切换为3D显示
		m_button_2D3D.SetWindowTextA("2D");
		((CAspect)m_tchart.get_Aspect()).put_View3D(TRUE);
		st_FlagData.is3D = TRUE;
	}
}


// 向下位机发送判定结果并发送停止指令
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
