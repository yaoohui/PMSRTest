
// DlgProxy.cpp : ʵ���ļ�
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
	
	// ΪʹӦ�ó������Զ��������ڻ״̬ʱһֱ���� 
	//	���У����캯������ AfxOleLockApp��
	AfxOleLockApp();

	// ͨ��Ӧ�ó����������ָ��
	//  �����ʶԻ���  ���ô�����ڲ�ָ��
	//  ָ��Ի��򣬲����öԻ���ĺ���ָ��ָ��
	//  �ô���
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
	// Ϊ������ OLE �Զ����������ж������ֹӦ�ó���
	//	������������ AfxOleUnlockApp��
	//  ���������������⣬�⻹���������Ի���
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CPMSRTestDlgAutoProxy::OnFinalRelease()
{
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease��  ���ཫ�Զ�
	// ɾ���ö���  �ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ע��: ��������˶� IID_IPMSRTest ��֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡�  �� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {A41ADA07-9238-4D40-948F-6B62ACB076AD}
static const IID IID_IPMSRTest =
{ 0xA41ADA07, 0x9238, 0x4D40, { 0x94, 0x8F, 0x6B, 0x62, 0xAC, 0xB0, 0x76, 0xAD } };

BEGIN_INTERFACE_MAP(CPMSRTestDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CPMSRTestDlgAutoProxy, IID_IPMSRTest, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 ���ڴ���Ŀ�� StdAfx.h �ж���
// {08E80809-3712-48C0-BFE7-8FB611FB45EA}
IMPLEMENT_OLECREATE2(CPMSRTestDlgAutoProxy, "PMSRTest.Application", 0x8e80809, 0x3712, 0x48c0, 0xbf, 0xe7, 0x8f, 0xb6, 0x11, 0xfb, 0x45, 0xea)


// CPMSRTestDlgAutoProxy ��Ϣ�������
