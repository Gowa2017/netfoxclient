#include "StdAfx.h"
#include "TableFrameHook.h"


//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameHook::m_wPlayerCount=MAX_CHAIR;			//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameHook::CTableFrameHook()
{
	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pMatchEventSink=NULL;

	return;
}

//��������
CTableFrameHook::~CTableFrameHook(void)
{
}

//�ӿڲ�ѯ
void *  CTableFrameHook::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameHook,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameHook,Guid,dwQueryVer);
	return NULL;
}

//
bool CTableFrameHook::SetMatchEventSink(IUnknownEx * pIUnknownEx)
{
	m_pMatchEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchEventSink);

	return true;
}

//��ʼ��
bool  CTableFrameHook::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//��Ϸ��ʼ
bool CTableFrameHook::OnEventGameStart(WORD wChairCount)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameStart(m_pITableFrame, wChairCount);
	}

	return false;
}

//��Ϸ����
bool  CTableFrameHook::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameEnd(m_pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}

//��ҷ���
bool CTableFrameHook::OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventUserReturnMatch(m_pITableFrame,pIServerUserItem);
	}

	return false;
}

//�û�����
bool CTableFrameHook::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL)  
	{
		return m_pMatchEventSink->OnActionUserSitDown(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�����
bool CTableFrameHook::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL) 
	{
		return m_pMatchEventSink->OnActionUserStandUp(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�ͬ��
bool CTableFrameHook::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnActionUserOnReady(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
