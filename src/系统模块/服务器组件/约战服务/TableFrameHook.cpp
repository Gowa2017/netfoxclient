#include "StdAfx.h"
#include "TableFrameHook.h"


//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameHook::m_wPlayerCount=MAX_CHAIR;			//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameHook::CTableFrameHook()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomEventSink=NULL;

	return;
}

//析构函数
CTableFrameHook::~CTableFrameHook(void)
{
}

//接口查询
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
	m_pPersonalRoomEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IPersonalRoomEventSink);

	return true;
}

//初始化
bool  CTableFrameHook::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//游戏开始
bool CTableFrameHook::OnEventGameStart(WORD wChairCount)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventGameStart(m_pITableFrame, wChairCount);
	}

	return false;
}

//游戏结束
bool  CTableFrameHook::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventGameEnd(m_pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}

//玩家返赛
bool CTableFrameHook::OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnEventUserReturnMatch(m_pITableFrame,pIServerUserItem);
	}

	return false;
}

//用户坐下
bool CTableFrameHook::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPersonalRoomEventSink!=NULL)  
	{
		return m_pPersonalRoomEventSink->OnActionUserSitDown(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户起来
bool CTableFrameHook::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPersonalRoomEventSink!=NULL) 
	{
		return m_pPersonalRoomEventSink->OnActionUserStandUp(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户同意
bool CTableFrameHook::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pPersonalRoomEventSink!=NULL)
	{
		return m_pPersonalRoomEventSink->OnActionUserOnReady(pIServerUserItem->GetTableID(), wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
