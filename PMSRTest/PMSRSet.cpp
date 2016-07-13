// PMSRSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRSet.h"
#include "afxdialogex.h"

extern UINT TestDelay;//��ʼ������ʱ

// CPMSRSet �Ի���

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


// CPMSRSet ��Ϣ�������


void CPMSRSet::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


BOOL CPMSRSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	FindComPort();
	ReadSettings();
	InitBaudRate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


//
// ��ȡ�������
//
void CPMSRSet::ReadSettings()
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
		CString temp = 0;

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
					m_combo_ComNumSet.SetWindowTextA(temp);
				}
				else if (strContent.Left(n - 1) == _T("BaudRate")) //�ж��Ƿ���Ҫ����ַ�����ͬ
				{
					//strcpy(temp, strContent.Mid(n+1,l-n-2));
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					m_combo_BaudRate.SetWindowTextA(temp);
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


// ���Ҵ���
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
//��ʼ�����ڲ�����
//
void CPMSRSet::InitBaudRate()
{
	m_combo_BaudRate.InsertString(0, "9600");
	m_combo_BaudRate.InsertString(1, "19200");
	m_combo_BaudRate.InsertString(2, "115200");
}
