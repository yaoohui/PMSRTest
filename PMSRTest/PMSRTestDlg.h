
// PMSRTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "tchart1.h"
#include "SerialPort.h"
#include "EditLog.h"
#include "DlgWaiting.h"



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
	// 主窗体布局
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
	// 通信协议类初始化
	void CommandProcessInit();
	// 串口接收数据处理
	void CommandPress();
protected:
	afx_msg LRESULT OnCommRxchar(WPARAM ch, LPARAM port);
public:
	CEdit m_edit_ReceiveData;
	//CEditLog  m_EditLogger;// 属性
	CButton m_check_ReceiveHex;
	CButton m_button_Clear;
	CEdit m_edit_SendData;
	CButton m_check_SendHex;
	CButton m_button_Send;
	afx_msg void OnBnClickedButton9();
	// 显示或隐藏调试用控件，根据是否有宏定义SHOW_DEBUG确定调试用控件是否显示
	void ShowDebugControls();
protected:
	afx_msg LRESULT OnCommBreakDetected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommRxflagDetected(WPARAM wParam, LPARAM lParam);
public:
	// 在图表tchart上显示波形
	void ShowWave();
	// 图表tchart控件初始化
	void TChartInit();
	// 初始化变量
	void InitVariable();
private:
	// 读取界面设置的参数，保存到全局变量中
	// 成功返回0，失败返回1
	int ReadParameters();

//	CDlgWaiting *DlgWaiting;
// 等待被测电机转速稳定倒计时界面
public:
	CStatic m_static_Result;
	CEdit m_edit_Result;
	// 流程控制线程
	static UINT FlowProcessThread(LPVOID pParam);// 注意线程需要定义成全局函数或者类的静态成员函数
protected:
//	afx_msg LRESULT OnWaitcountdown(WPARAM wParam, LPARAM lParam);
private:
	CDlgWaiting* DlgWaiting;
public:
	afx_msg void OnBnClickedButton10();
	// 使能或禁止设置参数控件的输入
//	int EnableSettings(bool bEnable);
protected:
	afx_msg LRESULT OnEnablesetcontrols(WPARAM wParam, LPARAM lParam);
public:
	// 向下位机发送步进电机移动距离
	void SendStepLength();
protected:
	afx_msg LRESULT OnDeletedlgcountdown(WPARAM wParam, LPARAM lParam);
};
