
// PMSRTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "tchart1.h"
#include "SerialPort.h"
#include "EditLog.h"
#include "DlgWaiting.h"



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
	// �����岼��
	void LayoutFrame();
	CStatic m_group_Parameter;
	CStatic m_group_Result;
	CStatic m_static_Type;
	CStatic m_static_Length;
	CStatic m_static_TestTimes;
	CStatic m_static_Flux;
	CStatic m_static_Pole;
	CStatic m_static_SN;
	CStatic m_static_Weight;
	CTchart1 m_tchart;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CButton m_button_Keyboard;
	CButton m_button_ScreenShot;
	CButton m_button_Save;
	CButton m_button_Print;
	CButton m_button_2D3D;
	CButton m_button_Type;
	CButton m_button_Settings;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	CButton m_button_Exit;
	// ͨ��Э�����ʼ��
	void CommandProcessInit();
	// ���ڽ������ݴ���
	void CommandPress();
protected:
	afx_msg LRESULT OnCommRxchar(WPARAM ch, LPARAM port);
public:
	CEdit m_edit_ReceiveData;
	//CEditLog  m_EditLogger;// ����
	CButton m_check_ReceiveHex;
	CButton m_button_Clear;
	CEdit m_edit_SendData;
	CButton m_check_SendHex;
	CButton m_button_Send;
	afx_msg void OnBnClickedButton9();
	// ��ʾ�����ص����ÿؼ��������Ƿ��к궨��SHOW_DEBUGȷ�������ÿؼ��Ƿ���ʾ
	void ShowDebugControls();
protected:
	afx_msg LRESULT OnCommBreakDetected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommRxflagDetected(WPARAM wParam, LPARAM lParam);
public:
	// ��ͼ��tchart����ʾ����
	void ShowWave();
	// ͼ��tchart�ؼ���ʼ��
	void TChartInit();
	// ��ʼ������
	void InitVariable();
private:
	// ��ȡ�������õĲ��������浽ȫ�ֱ�����
	// �ɹ�����0��ʧ�ܷ���1
	int ReadParameters();

//	CDlgWaiting *DlgWaiting;
// �ȴ�������ת���ȶ�����ʱ����
public:
	CStatic m_static_Result;
	CEdit m_edit_Result;
	// ���̿����߳�
	static UINT FlowProcessThread(LPVOID pParam);// ע���߳���Ҫ�����ȫ�ֺ���������ľ�̬��Ա����
protected:
//	afx_msg LRESULT OnWaitcountdown(WPARAM wParam, LPARAM lParam);
private:
	CDlgWaiting* DlgWaiting;
public:
	afx_msg void OnBnClickedButton10();
	// ʹ�ܻ��ֹ���ò����ؼ�������
//	int EnableSettings(bool bEnable);
protected:
	afx_msg LRESULT OnEnablesetcontrols(WPARAM wParam, LPARAM lParam);
public:
	// ����λ�����Ͳ�������ƶ�����
	void SendStepLength();
protected:
	afx_msg LRESULT OnDeletedlgcountdown(WPARAM wParam, LPARAM lParam);
};
