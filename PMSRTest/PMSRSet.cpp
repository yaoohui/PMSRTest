// PMSRSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRSet.h"
#include "afxdialogex.h"

extern UINT TestDelay;//开始测试延时

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


void CPMSRSet::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
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
				else if (strContent.Left(n - 1) == _T("TestDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //如果相同，取出该字符串所设置的值
					TestDelay = atoi(temp); //获取传感器使能设置
					if ((TestDelay < 0) || (TestDelay > 90000))
					{
						TestDelay = 10000;
						AfxMessageBox(_T("set.ini文件中，延时时间设置错误！"), MB_OK);
					}
					temp.Format("%d", TestDelay);
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
