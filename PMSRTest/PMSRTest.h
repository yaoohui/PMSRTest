
// PMSRTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// �Զ�����Ϣ
#define WM_WAITCOUNTDOWN		(WM_USER + 1000)// ����ʱ������Ϣ
#define WM_ENABLESETCONTROLS	(WM_USER + 1001)// �����洰����Ϣ
#define WM_DELETEDLGCOUNTDOWN	(WM_USER + 1002)// �����洰����Ϣ--ɾ������

// CPMSRTestApp: 
// �йش����ʵ�֣������ PMSRTest.cpp
//

class CPMSRTestApp : public CWinApp
{
public:
	CPMSRTestApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPMSRTestApp theApp;