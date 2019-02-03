#include "StdAfx.h"
#include "AfxTempl.h"
#include "LockTimeMatch.h"
#include "ImmediateMatch.h"
#include "MatchServiceManager.h"

////////////////////////////////////////////////////////////////////////

//���캯��
CMatchServiceManager::CMatchServiceManager(void)
{
	//״̬����
	m_bIsService=false;

	//���ñ���
	m_pIGameMatchItem=NULL;

	return;
}

//��������
CMatchServiceManager::~CMatchServiceManager(void)
{	
	//�ͷ�ָ��
	if(m_pIGameMatchItem!=NULL) SafeDelete(m_pIGameMatchItem);
}

//ֹͣ����
bool CMatchServiceManager::StopService()
{
	//״̬�ж�
	ASSERT(m_bIsService==true);

	//����״̬
	m_bIsService=false;

	//�ͷ�ָ��
	if(m_pIGameMatchItem!=NULL) SafeRelease(m_pIGameMatchItem);

	return true;
}

//��������
bool CMatchServiceManager::StartService()
{
	//״̬�ж�
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//����״̬
	m_bIsService=true;

	//����֪ͨ
	if(m_pIGameMatchItem!=NULL) m_pIGameMatchItem->OnStartService();

	return true;
}
//�ӿڲ�ѯ
void *  CMatchServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IMatchServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CMatchServiceManager::CreateGameMatch(BYTE cbMatchType)
{
	//�ӿ��ж�
	ASSERT(m_pIGameMatchItem==NULL);
	if(m_pIGameMatchItem!=NULL) return false;

	try
	{
		//��ʱ��
		if(cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			m_pIGameMatchItem = new CLockTimeMatch();
			if(m_pIGameMatchItem==NULL) throw TEXT("��ʱ������ʧ�ܣ�");
		}

		//��ʱ��
		if(cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			m_pIGameMatchItem = new CImmediateMatch();
			if(m_pIGameMatchItem==NULL) throw TEXT("��ʱ������ʧ�ܣ�");
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		return false;
	}

	return m_pIGameMatchItem!=NULL;
}

//��ʼ�����ӿ��
bool CMatchServiceManager::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->BindTableFrame(pTableFrame,wTableID);	
	}

	return true;
}


//��ʼ���ӿ�
bool CMatchServiceManager::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->InitMatchInterface(MatchManagerParameter);
	}

	return true;
}

//ʱ���¼�
bool CMatchServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventTimer(dwTimerID,dwBindParameter);	
	}

	return true;
}

//���ݿ��¼�
bool CMatchServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize,dwContextID);	
	}

	return true;
}


//������Ϣ
bool CMatchServiceManager::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);	
	}

	return true;
}

//�û���¼
bool CMatchServiceManager::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserLogon(pIServerUserItem);	
	}

	return true;
}

//�û��ǳ�
bool CMatchServiceManager::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserLogout(pIServerUserItem);	
	}

	return true;
}

//��¼���
bool CMatchServiceManager::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserLogonFinish(pIServerUserItem);	
	}

	return true;
}

//�������
bool CMatchServiceManager::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventEnterMatch(dwSocketID, pData, dwUserIP, bIsMobile);	
	}

	return true;
	
}

//�û�����
bool CMatchServiceManager::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserJoinMatch(pIServerUserItem,cbReason,dwSocketID);	
	}

	return true;
}

//�û�����
bool  CMatchServiceManager::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return m_pIGameMatchItem->OnEventUserQuitMatch(pIServerUserItem,cbReason,pBestRank,dwContextID);	
	}

	return true;
}

//�û��ӿ�
IUnknownEx * CMatchServiceManager::GetServerUserItemSink()
{
	ASSERT(m_pIGameMatchItem!=NULL);
	if(m_pIGameMatchItem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchItem,IUnknownEx);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(MatchServiceManager);

////////////////////////////////////////////////////////////////////////
