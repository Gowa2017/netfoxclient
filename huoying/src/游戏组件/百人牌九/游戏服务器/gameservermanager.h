#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//��Ϸ������������
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
{
  //�ؼ�����
protected:
  CDlgCustomRule            m_DlgCustomRule;        //�Զ�����

  //��������
protected:
  tagGameServiceAttrib        m_GameServiceAttrib;      //��������
  HINSTANCE             m_hDllInstance;         //DLL ���

  //�������
  //CGameServiceManagerHelper     m_AndroidServiceHelper;     //�����˷���

  //��������
public:
  //���캯��
  CGameServiceManager(void);
  //��������
  virtual ~CGameServiceManager(void);

  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID Release()
  {
    return;
  }
  //�ӿڲ�ѯ
  virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

  //�����ӿ�
public:
  //��������
  virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer);
  //��������
  virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
  //��������
  virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);

  //�����ӿ�
public:
  //�������
  virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
  //��������
  virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

  //���ýӿ�
public:
  //Ĭ������
  virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
  //��ȡ����
  virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
  //��������
  virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

};

//////////////////////////////////////////////////////////////////////////

#endif
