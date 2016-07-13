
// PMSRTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

class CPMSRTestDlgAutoProxy;


// CPMSRTestDlg 对话框
class CPMSRTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRTestDlg);
	friend class CPMSRTestDlgAutoProxy;

// 构造
public:
	CPMSRTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CPMSRTestDlg();

// 对话框数据
	enum { IDD = IDD_PMSRTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CPMSRTestDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void CPMSRTestDlg::OpenComm();
	void CPMSRTestDlg::ReadSettings();
	void CPMSRTestDlg::InitStatusBar();
	void CPMSRTestDlg::ConnectSQLServer();
	void CPMSRTestDlg::Initinterface();
	void CPMSRTestDlg::InitFont();

	CComboBox m_combo_type;
	CEdit m_edit_length;
	CEdit m_edit_TestTimes;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton8();
	CEdit m_edit_Weight;
	CEdit m_edit_Flux;
	CEdit m_edit_Pole;
	CEdit m_edit_SN;
};
