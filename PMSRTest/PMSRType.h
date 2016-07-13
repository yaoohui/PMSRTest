#pragma once


// CPMSRType 对话框

class CPMSRType : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRType)

public:
	CPMSRType(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPMSRType();

// 对话框数据
	enum { IDD = IDD_PMSRTEST_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
