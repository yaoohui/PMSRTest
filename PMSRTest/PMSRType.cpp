// PMSRType.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "PMSRType.h"
#include "afxdialogex.h"


// CPMSRType 对话框

IMPLEMENT_DYNAMIC(CPMSRType, CDialogEx)

CPMSRType::CPMSRType(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPMSRType::IDD, pParent)
{

}

CPMSRType::~CPMSRType()
{
}

void CPMSRType::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPMSRType, CDialogEx)
END_MESSAGE_MAP()


// CPMSRType 消息处理程序
