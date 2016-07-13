#pragma once
#include "afxwin.h"


// CPMSRSet �Ի���

class CPMSRSet : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRSet)

public:
	CPMSRSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPMSRSet();

// �Ի�������
	enum { IDD = IDD_PMSRTEST_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
