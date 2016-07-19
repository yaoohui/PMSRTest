// PMSRSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRSet.h"
#include "afxdialogex.h"

CStdioFile settingFile;
CFileException fileException;

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

// �������
void CPMSRSet::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str,str1;

	// �������Ϊ�գ�������Ĭ��ֵ

	// ������ʱ
	m_edit_SetTimeDelay.GetWindowTextA(str);
	if (str == "")
		st_ConfigData.StartDelay = 30;
	else
		st_ConfigData.StartDelay = atoi(str);
	

	//д�ļ�
	if (settingFile.Open(strIniFileName, CFile::typeText | CFile::modeReadWrite | CFile::modeCreate), &fileException)
	{
		UpdateData(TRUE);

		settingFile.WriteString("//���ò��� Setting Parameter\r\n\r\n");
		settingFile.WriteString("//�˿ں�\r\n");		
		m_combo_ComNumSet.GetWindowTextA(str);
		st_CommPara.strCommName = str;
		str1 = str.Mid(3);
		st_CommPara.uiCommName = atoi(str1);
		str1.Format("ComNum = %s;\r\n\r\n", str);
		settingFile.WriteString(str1);
		
		settingFile.WriteString("//������\r\n");
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

		settingFile.WriteString("//����λ\r\n");
		settingFile.WriteString("ByteSize = 8;\r\n\r\n");
		st_CommPara.uiByteSize = 8;

		settingFile.WriteString("//ֹͣλ\r\n");
		settingFile.WriteString("StopBits = 1;\r\n\r\n");
		st_CommPara.uiStopBits = 0;// ע�⣺ֹͣλΪ1λʱ��ȡֵΪ0

		settingFile.WriteString("//У��\r\n");
		settingFile.WriteString("Parity = No;\r\n\r\n");
		st_CommPara.uiParity = NOPARITY;

		settingFile.WriteString("//������ʱ����λ����\r\n");
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
	//���ļ�
	if (settingFile.Open(strIniFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite), &fileException)
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
				else if (strContent.Left(n - 1) == _T("StartDelay"))
				{
					temp = strContent.Mid(n + 2, m - n - 2); //�����ͬ��ȡ�����ַ��������õ�ֵ
					st_ConfigData.StartDelay = atoi(temp); //��ȡ������ʹ������
					if ((st_ConfigData.StartDelay < 0) || (st_ConfigData.StartDelay > 900))
					{
						st_ConfigData.StartDelay = 100;
						AfxMessageBox(_T("set.ini�ļ��У���ʱʱ�����ô���"), MB_OK);
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


// ���Ҵ���
void CPMSRSet::FindComPort()
{
	HKEY   hKey;

	st_CommPara.usCommNum = 0;	// ��������

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
//��ʼ�����ڲ�����
//
void CPMSRSet::InitBaudRate()
{
	m_combo_BaudRate.InsertString(0, "9600");
	m_combo_BaudRate.InsertString(1, "19200");
	m_combo_BaudRate.InsertString(2, "115200");
}
