#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
	//�ں�����
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = GAME_GENRE_GOLD | GAME_GENRE_EDUCATE;

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbAndroidUser = TRUE;
	m_GameServiceAttrib.cbOffLineTrustee = FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("LKPY.EXE"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("lkpy_server.DLL"), CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink = NULL;

	try
	{
		//��������
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CAndroidUserItemSink * pAndroidUserItemSink = NULL;

	try
	{
		//��������
		pAndroidUserItemSink = new CAndroidUserItemSink();
		if (pAndroidUserItemSink == NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject = pAndroidUserItemSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pAndroidUserItemSink);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib = m_GameServiceAttrib;

	return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
