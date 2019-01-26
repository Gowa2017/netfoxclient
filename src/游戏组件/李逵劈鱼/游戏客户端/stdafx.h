#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#define _AFX_ALL_WARNINGS						// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏

#include <AfxWin.h>				// MFC 核心组件和标准组件
#include <AfxExt.h>				// MFC 扩展
#include <AfxDisp.h>			// MFC 自动化类
#include <AfxDtctl.h>			// MFC 对 Internet Explorer 4 公共控件的支持
#include <AfxCmn.h>				// MFC 对 Windows 公共控件的支持

#include <iterator>
#include <assert.h>

#define timeGetTime	GetTickCount
//////////////////////////////////////////////////////////////////////////////////

// ArraySizeHelper是一个返回类型为char[N]的函数,其形参类型为 T[N].
// 函数没必要实现, 因为sizeof只需要类型.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)返回array数组的元素个数. 该表达式是编译时常量,
// 可以用于定义新的数组. 如果传递一个指针会报编译时错误.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define SAFE_DELETE(x) { if (NULL != (x)) { delete (x); (x) = NULL; } }

#include "..\消息定义\CMD_Fish.h"

//////////////////////////////////////////////////////////////////////////////////
#include "..\..\..\..\..\系统模块\全局定义\Macro.h"
#include "..\..\..\..\..\系统模块\全局定义\Define.h"
#include "..\..\..\..\..\系统模块\全局定义\Struct.h"
#include "..\..\..\..\..\系统模块\全局定义\Packet.h"
#include "..\..\..\..\..\系统模块\全局定义\Property.h"
#include "..\..\..\..\..\系统模块\全局定义\PacketAide.h"
#include "..\..\..\..\..\系统模块\全局定义\RightDefine.h"
#include "..\..\..\..\..\系统模块\消息定义\CMD_Commom.h"
#include "..\..\..\..\..\系统模块\消息定义\CMD_GameServer.h"
#include "..\..\..\..\..\系统模块\消息定义\IPC_GameFrame.h"
#include <afxcontrolbars.h>
//#define VERSION_FRAME   PROCESS_VERSION(11,0,3)   // 框架版本
#define VERSION_FRAME   101384195       // 这个数字为PROCESS_VERSION(11,0,3)的值,自己转换一下

