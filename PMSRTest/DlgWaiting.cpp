// DlgWaiting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"


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
END_MESSAGE_MAP()


// CDlgWaiting ��Ϣ�������


BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_static_countdown.SetWindowTextA("");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
