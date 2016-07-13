
// PMSRTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

class CPMSRTestDlgAutoProxy;


// CPMSRTestDlg �Ի���
class CPMSRTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRTestDlg);
	friend class CPMSRTestDlgAutoProxy;

// ����
public:
	CPMSRTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CPMSRTestDlg();

// �Ի�������
	enum { IDD = IDD_PMSRTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	CPMSRTestDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// ���ɵ���Ϣӳ�亯��
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
