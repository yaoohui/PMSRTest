// PMSRSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRSet.h"
#include "afxdialogex.h"

CStdioFile settingFile;
CFileException fileException;

// CPMSRSet 对话框

IMPLEMENT_DYNAMIC(CPMSRSet, CDialogEx)

CPMSRSet::CPMSRSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPMSRSet::IDD, pParent)
{

}

CPMSRSet::~CPMSRSet()
{
}

void CPMSRSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit_SetTimeDelay);
	DDX_Control(pDX, IDC_COMBO1, m_combo_ComNumSet);
	DDX_Control(pDX, IDC_COMBO2, m_combo_BaudRate);
}


BEGIN_MESSAGE_MAP(CPMSRSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPMSRSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CPMSRSet 消息处理程序

// 保存参数
void CPMSRSet::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str,str1;

	// 如果有栏为空，则填入默认值

	// 启动延时
	m_edit_SetTimeDelay.GetWindowTextA(str);
	if (str == "")
		st_ConfigData.StartDelay = 30;
	else
		st_ConfigData.StartDelay = atoi(str);
	

	//写文件
	if (settingFile.Open(strIniFileName, CFile::typeText | CFile::modeReadWrite | CFile::modeCreate), &fileException)
	{
		UpdateData(TRUE);

		settingFile.WriteString("//设置参数 Setting Parameter\r\n\r\n");
		settingFile.WriteString("//端口号\r\n");		
		m_combo_ComNumSet.GetWindowTextA(str);
		st_CommPara.strCommName = str;
		str1 = str.Mid(3);
		st_CommPara.uiCommName = atoi(str1);
		str1.Format("ComNum = %s;\r\n\r\n", str);
		settingFile.WriteString(str1);
		
		settingFile.WriteString("//波特率\r\n");
		m_combo_BaudRate.GetWindowTextA(str);
		if (str == "")
		{
			str1.Format("BaudRate = 9600;\r\n\r\n");
			st_CommPara.uiBaudRate = 9600;
		}
		else
		{
			str1.Format("BaudRate = %s;\r\n\r\n", str);
			st_CommPara.uiBaudRate = atoi(str);
		}
		settingFile.WriteString(str1);

		settingFile.WriteString("//数据位\r\n");
		settingFile.WriteString("ByteSize = 8;\r\n\r\n");
		st_CommPara.uiByteSize = 8;

		settingFile.WriteString("//停止位\r\n");
		settingFile.WriteString("StopBits = 1;\r\n\r\n");
		st_CommPara.uiStopBits = 0;// 注意：停止位为1位时，取值为0

		settingFile.WriteString("//校验\r\n");
		settingFile.WriteString("Parity = No;\r\n\r\n");
		st_CommPara.uiParity = NOPARITY;

		settingFile.WriteString("//启动延时，单位：秒\r\n");
		str1.Format("StartDelay = %d;\r\n\r\n", st_ConfigData.StartDelay);
		settingFile.WriteString(str1);
	}
	else
	{
		TRACE("Can't open file %s,error=%u\n", strIniFileName, fileException.m_cause);
	}
	settingFile.Close();


	CDialogEx::OnOK();
}


BOOL CPMSRSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	FindComPort();
	ReadSettings();
	InitBaudRate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


//
// 读取软件设置
//
void CPMSRSet::ReadSettings()
{
	//读文件
	if (settingFile.Open(strIniFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
	{
		CString strContent;
		CString temp = 0;

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
					m_combo_ComNumSet.SetWindowTextA(temp);
				}
				else if (strContent.Left(n - 1) == _T("BaudRate")) //判断是否与要求的字符串相同
				{
					//strcpy(temp, strContent.Mid(n+1,l-n-2));
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					m_combo_BaudRate.SetWindowTextA(temp);
				}
				else if (strContent.Left(n - 1) == _T("StartDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					st_ConfigData.StartDelay = atoi(temp); //获取传感器使能设置
					if ((st_ConfigData.StartDelay < 0) || (st_ConfigData.StartDelay > 900))
					{
						st_ConfigData.StartDelay = 100;
						AfxMessageBox(_T("set.ini文件中，延时时间设置错误！"), MB_OK);
					}
					temp.Format("%d", st_ConfigData.StartDelay);
					m_edit_SetTimeDelay.SetWindowTextA(temp);
				}
			}
		}
	}
	else
	{
		TRACE("Can't open file");
	}

	settingFile.Close();
}


// 查找串口
void CPMSRSet::FindComPort()
{
	HKEY   hKey;

	st_CommPara.usCommNum = 0;	// 串口数量

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		TCHAR       szPortName[256], szComName[256];
		DWORD       dwLong, dwSize;
		int         nCount = 0;
		CComboBox*  pCombo = (CComboBox*)GetDlgItem(IDC_COMBO1);

		pCombo->ResetContent();
		while (true)
		{
			dwLong = dwSize = 256;
			if (RegEnumValue(hKey, nCount, szPortName, &dwLong, NULL, NULL, (PUCHAR)szComName, &dwSize) == ERROR_NO_MORE_ITEMS)
				break;

			pCombo->InsertString(nCount, szComName);
			nCount++;
			st_CommPara.usCommNum++;
		}
		RegCloseKey(hKey);
		pCombo->SetCurSel(0);
	}

}


//
//初始化串口波特率
//
void CPMSRSet::InitBaudRate()
{
	m_combo_BaudRate.InsertString(0, "9600");
	m_combo_BaudRate.InsertString(1, "19200");
	m_combo_BaudRate.InsertString(2, "115200");
}
