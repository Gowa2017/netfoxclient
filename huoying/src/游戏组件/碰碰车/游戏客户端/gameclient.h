#pragma once

#include "Stdafx.h"
#include "Resource.h"

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

  //��������
protected:
  //CGameFrameWnd *         m_pGameFrameWnd;
  //CGameFrameEngine *        m_pGameFrameEngine;         //�������


  //���غ���
public:
  ////���ú���
  //virtual BOOL InitInstance();
  ////�˳�����
  //virtual BOOL ExitInstance();


  //�̳к���
public:
  ////�������
  //virtual CGameFrameWindow * GetGameFrameWindow();
  //�������
  virtual CGameFrameWnd * GetGameFrameWnd();
  //��������
  virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion);

public:
  //��ȡ���
  //CGameFrameWnd * GetFrameWnd();
};

//////////////////////////////////////////////////////////////////////////////////

//�������
extern CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////
