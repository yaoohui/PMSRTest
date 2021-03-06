
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include "SerialPort.h"// 串口类






// 除要将 bMultiInstance 参数的 TRUE 
// 传递给 COleObjectFactory 构造函数之外，此宏与 IMPLEMENT_OLECREATE 相同。
// 对于自动化控制器所请求的每一个自动化代理对象,
// 需要分别启动一个该应用程序的实例。
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
                RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
        const AFX_DATADEF GUID class_name::guid = \
                { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("BOF","adoBOF") rename("EOF","adoEOF")

// 串口相关变量结构体
struct CommPara
{
	CSerialPort m_Comm;
	BOOL bIsCommOpen = false;	// 串口打开标志

	CString strCommName;		// 当前串口号
	UCHAR uiCommName;			// 串口号，数值
	UINT uiBaudRate;
	UCHAR uiByteSize;
	UCHAR uiStopBits;
	UCHAR uiParity;

	UINT uiBytesSent;			// 发送的总字节数
	UINT uiBytesReceived;		// 接收的总字节数
	CString strReceivedData;	// 接收的数据
	CString strSendData;		// 串口发送的数据
	USHORT usCommNum;			// 串口数量
};

struct ConfigVal
{
	UINT StartDelay;	// 启动延时时间，单位：s
	UINT motorlen;		// 电机长度，单位：mm
	UINT testtimes;		// 试验次数
	UINT flux;			// 磁通量
	UINT pole;			// 磁极数
};

#define MODEID	0x17		// 模块ID，串口通讯部分

extern struct ConfigVal st_ConfigData;
extern struct CommPara st_CommPara;
extern CString strIniFileName;

