#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////

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
    ASSERT(FALSE);
    return NULL;
  }

  //��������
  return new CGameClientEngine;
}

CGameFrameWnd * CGameClientApp::GetGameFrameWnd()
{
  return (CGameFrameWnd *)new CGameFrameWindow;
}
//////////////////////////////////////////////////////////////////////////////////
