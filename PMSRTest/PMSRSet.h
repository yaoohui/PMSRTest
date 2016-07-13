#pragma once
#include "afxwin.h"


// CPMSRSet 对话框

class CPMSRSet : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRSet)

public:
	CPMSRSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPMSRSet();

// 对话框数据
	enum { IDD = IDD_PMSRTEST_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void CPMSRSet::ReadSettings();

	CEdit m_edit_SetTimeDelay;
	CComboBox m_combo_ComNumSet;
	void CPMSRSet::FindComPort();
	void CPMSRSet::InitBaudRate();

	CComboBox m_combo_BaudRate;
};
