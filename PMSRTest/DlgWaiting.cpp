// DlgWaiting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include "PMSRTestDlg.h"

// CDlgWaiting �Ի���

IMPLEMENT_DYNAMIC(CDlgWaiting, CDialogEx)

CDlgWaiting::CDlgWaiting(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_COUNTDOWN, pParent)
{

}

CDlgWaiting::~CDlgWaiting()
{
}

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COUNTDOWN, m_static_countdown);
}


BEGIN_MESSAGE_MAP(CDlgWaiting, CDialogEx)
	ON_MESSAGE(WM_WAITCOUNTDOWN, &CDlgWaiting::OnWaitcountdown)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgWaiting::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgWaiting ��Ϣ�������


BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRect rect1;
	int iWidth = GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);
	this->GetWindowRect(&rect1);
	SetWindowPos(NULL, (iWidth - rect1.Width()) / 2, (iHeight - rect1.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_static_countdown.SetWindowTextA("");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

// ���յ������巢�͵���Ϣ
afx_msg LRESULT CDlgWaiting::OnWaitcountdown(WPARAM wParam, LPARAM lParam)
{
	UINT countdown = wParam;
	CString str;

	str.Format("%d", countdown);
	m_static_countdown.SetWindowTextA(str);
	UpdateData(FALSE);

	if (countdown == 0)	// ����ʱ��ɣ��رմ���
	{
		this->GetParent()->EnableWindow(TRUE);

		// ��ø����ڵľ��
		HWND hWnd = this->GetParent()->GetSafeHwnd();
		::SendMessage(hWnd, WM_ENABLESETCONTROLS, FALSE, 0);// �����ڿؼ���ɫ
		SendMessage(WM_CLOSE, 0, 0);
	}
	return 0;
}


// �˳�����ʱ
void CDlgWaiting::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->GetParent()->EnableWindow(TRUE);
	// ��ø����ڵľ��
	HWND hWnd = this->GetParent()->GetSafeHwnd();
	::SendMessage(hWnd, WM_ENABLESETCONTROLS, TRUE, 0);// �����ڿؼ�ʹ��
	::SendMessage(hWnd, WM_DELETEDLGCOUNTDOWN, 0, 0);// ɾ������ָ��
	SendMessage(WM_CLOSE, 0, 0);
}
