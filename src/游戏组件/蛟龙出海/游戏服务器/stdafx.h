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
//系统环境

//平台环境
#include "..\..\..\..\系统模块\服务器组件\游戏服务\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"不支持Ansi格式")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/KernelEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"不支持Ansi格式")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/KernelEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCore.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

//游戏文件
#include "..\消息定义\CMD_Fish.h"

//////////////////////////////////////////////////////////////////////////////////
