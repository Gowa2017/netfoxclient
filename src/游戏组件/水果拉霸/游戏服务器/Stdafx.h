#pragma once


//////////////////////////////////////////////////////////////////////////////////
//MFC �ļ�

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

//#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#define _AFX_ALL_WARNINGS						// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������

#include <AfxWin.h>				// MFC ��������ͱ�׼���
#include <AfxExt.h>				// MFC ��չ
#include <AfxDisp.h>			// MFC �Զ�����
#include <AfxDtctl.h>			// MFC �� Internet Explorer 4 �����ؼ���֧��
#include <AfxCmn.h>				// MFC �� Windows �����ؼ���֧��

//////////////////////////////////////////////////////////////////////////////////

#ifdef SDK_CONDITIONo

//////////////////////////////////////////////////////////////////////////////////
//��������

//ƽ̨����
#include "..\..\..\������\Include\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../../������/Lib/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../������/Lib/Unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../../������/Lib/Ansi/KernelEngineD.lib")
#else
	#pragma comment (lib,"../../../������/Lib/Unicode/KernelEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

//ƽ̨����
#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/unicode/KernelEngine.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Ansi/KernelEngine.lib")
#else
	#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Debug_Unicode/KernelEngine.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif // SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�ļ�
#include "..\��Ϣ����\CMD_Game.h"

//////////////////////////////////////////////////////////////////////////////////
