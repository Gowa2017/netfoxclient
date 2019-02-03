#pragma once


//////////////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

//#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#define _AFX_ALL_WARNINGS						// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏

#include <AfxWin.h>				// MFC 核心组件和标准组件
#include <AfxExt.h>				// MFC 扩展
#include <AfxDisp.h>			// MFC 自动化类
#include <AfxDtctl.h>			// MFC 对 Internet Explorer 4 公共控件的支持
#include <AfxCmn.h>				// MFC 对 Windows 公共控件的支持

//////////////////////////////////////////////////////////////////////////////////

#ifdef SDK_CONDITIONo

//////////////////////////////////////////////////////////////////////////////////
//开发环境

//平台环境
#include "..\..\..\开发库\Include\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../../开发库/Lib/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../开发库/Lib/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../../开发库/Lib/Ansi/KernelEngineD.lib")
#else
	#pragma comment (lib,"../../../开发库/Lib/Unicode/KernelEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

//平台环境
#include "..\..\..\..\系统模块\服务器组件\游戏服务\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../../../系统模块/链接库/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../系统模块/链接库/unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../../../系统模块/链接库/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../系统模块/链接库/Debug_Unicode/KernelEngine.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif // SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////

//游戏文件
#include "..\消息定义\CMD_Game.h"

//////////////////////////////////////////////////////////////////////////////////
