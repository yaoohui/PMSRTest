#pragma once


// CPMSRType �Ի���

class CPMSRType : public CDialogEx
{
	DECLARE_DYNAMIC(CPMSRType)

public:
	CPMSRType(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPMSRType();

// �Ի�������
	enum { IDD = IDD_PMSRTEST_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
