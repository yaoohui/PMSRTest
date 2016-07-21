// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"


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
END_MESSAGE_MAP()


// CDlgWaiting 消息处理程序


BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_static_countdown.SetWindowTextA("");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
