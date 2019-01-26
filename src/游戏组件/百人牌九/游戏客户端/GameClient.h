#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameFrameWindow.h"
//////////////////////////////////////////////////////////////////////////////////

//Ӧ�ó���
class CGameClientApp : public CGameFrameApp
{
  //��������
public:
  //���캯��
  CGameClientApp();
  //��������
  virtual ~CGameClientApp();

  //�̳к���
public:
  //�������
  virtual CGameFrameWnd * GetGameFrameWnd();
  //��������
  virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion);
};

//////////////////////////////////////////////////////////////////////////////////

//�������
extern CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////
