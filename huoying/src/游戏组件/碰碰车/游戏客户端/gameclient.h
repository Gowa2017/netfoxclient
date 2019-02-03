#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////////////

//应用程序
class CGameClientApp : public CGameFrameApp
{
  //函数定义
public:
  //构造函数
  CGameClientApp();
  //析构函数
  virtual ~CGameClientApp();

  //变量定义
protected:
  //CGameFrameWnd *         m_pGameFrameWnd;
  //CGameFrameEngine *        m_pGameFrameEngine;         //框架引擎


  //重载函数
public:
  ////配置函数
  //virtual BOOL InitInstance();
  ////退出函数
  //virtual BOOL ExitInstance();


  //继承函数
public:
  ////创建框架
  //virtual CGameFrameWindow * GetGameFrameWindow();
  //创建框架
  virtual CGameFrameWnd * GetGameFrameWnd();
  //创建引擎
  virtual CGameFrameEngine * GetGameFrameEngine(DWORD dwSDKVersion);

public:
  //获取框架
  //CGameFrameWnd * GetFrameWnd();
};

//////////////////////////////////////////////////////////////////////////////////

//程序对象
extern CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////
