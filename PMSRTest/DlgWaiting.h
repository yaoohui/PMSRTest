#pragma once
#include "afxwin.h"


// CDlgWaiting �Ի���

class CDlgWaiting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	CDlgWaiting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWaiting();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_COUNTDOWN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_static_countdown;
	virtual BOOL OnInitDialog();
};
