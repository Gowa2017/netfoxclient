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
//ϵͳ����

//ƽ̨����
#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"��֧��Ansi��ʽ")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"��֧��Ansi��ʽ")
#else
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/KernelEngine.lib")
#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/Unicode/ServiceCore.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�ļ�
#include "..\��Ϣ����\CMD_Fish.h"

//////////////////////////////////////////////////////////////////////////////////
