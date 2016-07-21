#pragma once
#include "afxwin.h"


// CDlgWaiting 对话框

class CDlgWaiting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	CDlgWaiting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWaiting();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_COUNTDOWN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_static_countdown;
	virtual BOOL OnInitDialog();
};
