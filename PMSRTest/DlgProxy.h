
// DlgProxy.h: ͷ�ļ�
//

#pragma once

class CPMSRTestDlg;


// CPMSRTestDlgAutoProxy ����Ŀ��

class CPMSRTestDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CPMSRTestDlgAutoProxy)

	CPMSRTestDlgAutoProxy();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��

// ����
public:
	CPMSRTestDlg* m_pDialog;

// ����
public:

// ��д
	public:
	virtual void OnFinalRelease();

// ʵ��
protected:
	virtual ~CPMSRTestDlgAutoProxy();

	// ���ɵ���Ϣӳ�亯��

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CPMSRTestDlgAutoProxy)

	// ���ɵ� OLE ����ӳ�亯��

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

