
// DlgProxy.cpp : 实现文件
//

#include "stdafx.h"
#include "PMSRTest.h"
#include "DlgProxy.h"
#include "PMSRTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPMSRTestDlgAutoProxy

IMPLEMENT_DYNCREATE(CPMSRTestDlgAutoProxy, CCmdTarget)


CPMSRTestDlgAutoProxy::CPMSRTestDlgAutoProxy()
{
	EnableAutomation();
	
	// 为使应用程序在自动化对象处于活动状态时一直保持 
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CPMSRTestDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CPMSRTestDlg)))
		{
			m_pDialog = reinterpret_cast<CPMSRTestDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CPMSRTestDlgAutoProxy::~CPMSRTestDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CPMSRTestDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IPMSRTest 的支持
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {A41ADA07-9238-4D40-948F-6B62ACB076AD}
static const IID IID_IPMSRTest =
{ 0xA41ADA07, 0x9238, 0x4D40, { 0x94, 0x8F, 0x6B, 0x62, 0xAC, 0xB0, 0x76, 0xAD } };

BEGIN_INTERFACE_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CPMSRTestDlgAutoProxy, IID_IPMSRTest, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {08E80809-3712-48C0-BFE7-8FB611FB45EA}
IMPLEMENT_OLECREATE2(CPMSRTestDlgAutoProxy, "PMSRTest.Application", 0x8e80809, 0x3712, 0x48c0, 0xbf, 0xe7, 0x8f, 0xb6, 0x11, 0xfb, 0x45, 0xea)


// CPMSRTestDlgAutoProxy 消息处理程序
