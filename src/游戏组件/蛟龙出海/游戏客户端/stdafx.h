#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC �ļ�

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#define _AFX_ALL_WARNINGS						// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������

#include <AfxWin.h>				// MFC ��������ͱ�׼���
#include <AfxExt.h>				// MFC ��չ
#include <AfxDisp.h>			// MFC �Զ�����
#include <AfxDtctl.h>			// MFC �� Internet Explorer 4 �����ؼ���֧��
#include <AfxCmn.h>				// MFC �� Windows �����ؼ���֧��

#include <iterator>
#include <assert.h>

#define timeGetTime	GetTickCount
//////////////////////////////////////////////////////////////////////////////////

// ArraySizeHelper��һ����������Ϊchar[N]�ĺ���,���β�����Ϊ T[N].
// ����û��Ҫʵ��, ��Ϊsizeofֻ��Ҫ����.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)����array�����Ԫ�ظ���. �ñ��ʽ�Ǳ���ʱ����,
// �������ڶ����µ�����. �������һ��ָ��ᱨ����ʱ����.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define SAFE_DELETE(x) { if (NULL != (x)) { delete (x); (x) = NULL; } }

#include "..\��Ϣ����\CMD_Fish.h"

//////////////////////////////////////////////////////////////////////////////////
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\Macro.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\Define.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\Struct.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\Packet.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\Property.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\PacketAide.h"
#include "..\..\..\..\..\ϵͳģ��\ȫ�ֶ���\RightDefine.h"
#include "..\..\..\..\..\ϵͳģ��\��Ϣ����\CMD_Commom.h"
#include "..\..\..\..\..\ϵͳģ��\��Ϣ����\CMD_GameServer.h"
#include "..\..\..\..\..\ϵͳģ��\��Ϣ����\IPC_GameFrame.h"
#include <afxcontrolbars.h>
//#define VERSION_FRAME   PROCESS_VERSION(11,0,3)   // ��ܰ汾
#define VERSION_FRAME   101384195       // �������ΪPROCESS_VERSION(11,0,3)��ֵ,�Լ�ת��һ��

