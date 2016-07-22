
// PMSRTest.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// 自定义消息
#define WM_WAITCOUNTDOWN		(WM_USER + 1000)// 倒计时窗体消息
#define WM_ENABLESETCONTROLS	(WM_USER + 1001)// 主界面窗体消息
#define WM_DELETEDLGCOUNTDOWN	(WM_USER + 1002)// 主界面窗体消息--删除对象

// CPMSRTestApp: 
// 有关此类的实现，请参阅 PMSRTest.cpp
//

class CPMSRTestApp : public CWinApp
{
public:
	CPMSRTestApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPMSRTestApp theApp;