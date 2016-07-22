// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include "PMSRTestDlg.h"

// CDlgWaiting 对话框

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


// CDlgWaiting 消息处理程序


BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect1;
	int iWidth = GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);
	this->GetWindowRect(&rect1);
	SetWindowPos(NULL, (iWidth - rect1.Width()) / 2, (iHeight - rect1.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_static_countdown.SetWindowTextA("");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 接收到主窗体发送的消息
afx_msg LRESULT CDlgWaiting::OnWaitcountdown(WPARAM wParam, LPARAM lParam)
{
	UINT countdown = wParam;
	CString str;

	str.Format("%d", countdown);
	m_static_countdown.SetWindowTextA(str);
	UpdateData(FALSE);

	if (countdown == 0)	// 倒计时完成，关闭窗口
	{
		this->GetParent()->EnableWindow(TRUE);

		// 获得父窗口的句柄
		HWND hWnd = this->GetParent()->GetSafeHwnd();
		::SendMessage(hWnd, WM_ENABLESETCONTROLS, FALSE, 0);// 主窗口控件灰色
		SendMessage(WM_CLOSE, 0, 0);
	}
	return 0;
}


// 退出倒计时
void CDlgWaiting::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetParent()->EnableWindow(TRUE);
	// 获得父窗口的句柄
	HWND hWnd = this->GetParent()->GetSafeHwnd();
	::SendMessage(hWnd, WM_ENABLESETCONTROLS, TRUE, 0);// 主窗口控件使能
	::SendMessage(hWnd, WM_DELETEDLGCOUNTDOWN, 0, 0);// 删除对象指针
	SendMessage(WM_CLOSE, 0, 0);
}
