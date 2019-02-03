#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME  TEXT("OxBattleAndroid.dll") //�������
#else
#define ANDROID_SERVICE_DLL_NAME  TEXT("OxBattleAndroid.dll") //�������
#endif
//���캯��
CGameServiceManager::CGameServiceManager()
{
  //�ں�����
  m_GameServiceAttrib.wKindID=KIND_ID;
  m_GameServiceAttrib.wChairCount=GAME_PLAYER;
  m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_EDUCATE);

  //���ܱ�־
  m_GameServiceAttrib.cbDynamicJoin=TRUE;
  m_GameServiceAttrib.cbAndroidUser=TRUE;
  m_GameServiceAttrib.cbOffLineTrustee=FALSE;

  //��������
  m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
  m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
  lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
  lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPOxBattleDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
  lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("OxBattle.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
  lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("OxBattleServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

  return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
  QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
  return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
  //��������
  CTableFrameSink * pTableFrameSink=NULL;

  try
  {
    //��������
    pTableFrameSink=new CTableFrameSink();
    if(pTableFrameSink==NULL)
    {
      throw TEXT("����ʧ��");
    }

    //��ѯ�ӿ�
    VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
    if(pObject==NULL)
    {
      throw TEXT("�ӿڲ�ѯʧ��");
    }

    return pObject;
  }
  catch(...) {}

  //ɾ������
  SafeDelete(pTableFrameSink);

  return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
  try
  {
    //����ģ��
    if(m_hDllInstance==NULL)
    {
      m_hDllInstance=AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
      if(m_hDllInstance==NULL)
      {
        throw TEXT("�����˷���ģ�鲻����");
      }
    }

    //Ѱ�Һ���
    ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,"CreateAndroidUserItemSink");
    if(CreateProc==NULL)
    {
      throw TEXT("�����˷���ģ��������Ϸ�");
    }

    //�������
    return CreateProc(Guid,dwQueryVer);
  }
  catch(...) {}

  return NULL;
}
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
  return NULL;
}
//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
  //���ñ���
  GameServiceAttrib=m_GameServiceAttrib;

  return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
  //Ч�����
  ASSERT(&GameServiceOption!=NULL);
  if(&GameServiceOption==NULL)
  {
    return false;
  }

  //��Ԫ����
  GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

  //��������
  //�����Ϸ
  /*if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
  {
    GameServiceOption.lMinEnterScore=__max(GameServiceOption.lCellScore,GameServiceOption.lMinEnterScore);
  }*/


  ////��������
  //if(GameServiceOption.lRestrictScore!=0L)
  //{
  //  GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinEnterScore);
  //}

  return true;

}

//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
  return m_DlgCustomRule.SaveCustomRule(pcbCustomRule,wCustonSize);
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
  return m_DlgCustomRule.DefaultCustomRule(pcbCustomRule,wCustonSize);
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
  //��������
  if(m_DlgCustomRule.m_hWnd == NULL)
  {
    //������Դ
    AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

    //��������
    m_DlgCustomRule.Create(IDD_CUSTOM_RULE,pParentWnd);

    //��ԭ��Դ
    AfxSetResourceHandle(GetModuleHandle(NULL));
  }

  //��������ģ��
  HINSTANCE hDllInstance = NULL;
  hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
  if(hDllInstance==NULL)
  {
    m_DlgCustomRule.HideAndroidModule(true);
  }

  //���ñ���
  m_DlgCustomRule.SetCustomRule(pcbCustomRule,wCustonSize);

  //��ʾ����
  m_DlgCustomRule.SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

  return m_DlgCustomRule.GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
  static CGameServiceManager GameServiceManager;
  return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
