
// PMSRTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


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