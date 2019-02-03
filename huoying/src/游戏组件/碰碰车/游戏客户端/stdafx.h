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
#include <AtlBase.h>
#include <AtlDbcli.h>
#include <AfxDtctl.h>

//////////////////////////////////////////////////////////////////////////////////
//���뻷��
#include "..\..\..\SDKCondition.h"
#define __SPECIAL___
//////////////////////////////////////////////////////////////////////////////////
#define SDK_CONDITION
#ifdef SDK_CONDITION
//////////////////////////////////////////////////////////////////////////////////
//��������

#include "..\..\..\������\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../������/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../������/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ShareControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../������/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ShareControlD.lib")
#pragma comment (lib,"../../../������/Lib/Ansi/ShareControlD.lib")
#else
#pragma comment (lib,"../../../������/Lib/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ShareControlD.lib")
#pragma comment (lib,"../../../������/Lib/Unicode/ShareControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

#include "..\..\..\..\ϵͳģ��\�ͻ������\��Ϸ���\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ShareControl.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/WHImage.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ShareControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/ShareControlD.lib")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ShareControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif

#include "..\��Ϣ����\CMD_GAME.h"