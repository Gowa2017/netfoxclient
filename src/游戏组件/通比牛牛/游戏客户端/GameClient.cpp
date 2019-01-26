#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//�������
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientApp::CGameClientApp()
{
}

//��������
CGameClientApp::~CGameClientApp()
{
}

//��������
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
  //�汾���
  if(InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
  {
    return NULL;
  }

  //��������
  return new CGameClientEngine;
}

//////////////////////////////////////////////////////////////////////////
