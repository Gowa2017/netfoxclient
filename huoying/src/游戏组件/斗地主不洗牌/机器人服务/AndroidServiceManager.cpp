#include "StdAfx.h"
#include "AndroidServiceManager.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////
CGameServiceManager *	CGameServiceManager::m_pGameServiceManager=NULL;

//���캯��
CGameServiceManager::CGameServiceManager()
{
	ASSERT(m_pGameServiceManager==NULL);
	if (m_pGameServiceManager==NULL) m_pGameServiceManager=this;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
	ASSERT(m_pGameServiceManager==this);
	if (m_pGameServiceManager==this) m_pGameServiceManager=NULL;
}

//�ӿڲ�ѯ
void *  CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CAndroidUserItemSink * pAndroidUserItemSink=NULL;

	try
	{
		//��������
		pAndroidUserItemSink=new CAndroidUserItemSink();
		if (pAndroidUserItemSink==NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject=pAndroidUserItemSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pAndroidUserItemSink);

	return NULL;
}

//������Ϸ��
VOID *  CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}
//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//��ȡ����
bool  CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	return true;
}

//�����޸�
bool  CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//��Ԫ����
	GameServiceOption.lCellScore -=__max(1.0,GameServiceOption.lCellScore);

	//��������
	GameServiceOption.lMinTableScore=__max(0L,GameServiceOption.lMinTableScore);

	//��������
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

	CopyMemory(&m_GameServiceOption,&GameServiceOption,sizeof(tagGameServiceOption));

	return true;
}
//��������
DECLARE_CREATE_MODULE(GameServiceManager)

//////////////////////////////////////////////////////////////////////////
