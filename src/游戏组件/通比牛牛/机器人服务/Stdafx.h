#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
//
////���뻷��
//#include "..\..\..\SDKCondition.h"
//
////////////////////////////////////////////////////////////////////////////////////
//
//#define SDK_CONDITION
//
//#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//��������

//ƽ̨����
//#include "..\..\..\������\Include\GameServiceHead.h"
//
//#ifndef _DEBUG
//#ifndef _UNICODE
//#pragma comment (lib,"../../../������/Lib/Ansi/KernelEngine.lib")
//#else
//#pragma comment (lib,"../../../������/Lib/Unicode/KernelEngine.lib")
//#endif
//#else
//#ifndef _UNICODE
//#pragma comment (lib,"../../../������/Lib/Ansi/KernelEngineD.lib")
//#else
//#pragma comment (lib,"../../../������/Lib/Unicode/KernelEngineD.lib")
//#endif
//#endif
//
////////////////////////////////////////////////////////////////////////////////////
//
//#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

//ƽ̨����
#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngine.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngineD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

//#endif // SDK_CONDITION

//���ͷ�ļ�
#include "..\��Ϣ����\CMD_Ox.h"

//////////////////////////////////////////////////////////////////////////