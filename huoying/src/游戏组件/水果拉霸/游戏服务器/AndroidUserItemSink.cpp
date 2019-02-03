#include "Stdafx.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////

//游戏时间
#define IDI_START_GAME				200									//出牌定时器
#define IDI_USER_OPERATE			201									//开始定时器
#define IDI_USER_BANK				202									//开始定时器

#define TIME_USER_START_GAME		5									//开始时间
#define TIME_USER_OPERATE			30									//操作时间
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);

	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem)!=NULL);
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);

	//错误判断
	if (m_pIAndroidUserItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{			
			//发送数据
			CMD_C_OneStart OneStart;
			OneStart.lBetCount = 1;
			OneStart.lBet = 1;	
			//发送消息
			m_pIAndroidUserItem->SendUserReady(NULL, NULL);
			m_pIAndroidUserItem->SendSocketData(SUB_C_ONE_START, &OneStart, sizeof(OneStart));

			return true;
		}
	case IDI_USER_OPERATE:		//开始定时器
		{			
			if (m_cbGameMode == GM_SMALL)
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_THREE_END);
			}
			else
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_ONE_END);
			}
			return true;
		}
	case IDI_USER_BANK:		//存取款定时器
		{			
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			//变量定义
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			LONGLONG lRobotScore = pUserItem->GetUserScore();	
			tagAndroidParameter *pAndroidParameter = m_pIAndroidUserItem->GetAndroidParameter();

			if (lRobotScore < pAndroidParameter->lTakeMinScore / 2)
			{
				LONGLONG lScore=pAndroidParameter->lTakeMinScore + (rand()%(LONGLONG)__max(pAndroidParameter->lTakeMaxScore - pAndroidParameter->lTakeMinScore, 1));
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformTakeScore(lScore);
				}
			}
			else if (lRobotScore > pAndroidParameter->lTakeMaxScore * 2)
			{
				LONGLONG lScore=rand()%(LONGLONG)__max(lRobotScore - pAndroidParameter->lTakeMinScore, 1);
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformSaveScore(lScore);
				}
			}

			return true;
		}	
	}
	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SMALL_START:
		{
			return true;
		}
	case SUB_S_GAME_CONCLUDE:	//游戏结束
		{
			return OnSubGameConclude(pData,wDataSize);
		}	
	}
	return true;

}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:		//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			//开始时间
			UINT nElapse= 2 + (rand() % TIME_USER_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			return true;
		}
	case GAME_SMALL_GAME:		//小游戏状态
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_S_SmallStatus));
			if (wDataSize != sizeof(CMD_S_SmallStatus)) return false;
			//变量定义
			CMD_S_SmallStatus * pStatusTwo = (CMD_S_SmallStatus *)pData;
			m_cbGameMode = GM_SMALL;
			//开始时间
			UINT nElapse= 5 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
			return true;
		}
	}

	return true;
}

//用户进入
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	m_cbGameMode = pGameStart->cbGameMode;
	if (pGameStart->cbGameMode == GM_SMALL)
	{
		//开始时间
		UINT nElapse= 10 + (rand() % TIME_USER_OPERATE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
	}
	else
	{
		//开始时间
		UINT nElapse= 5 + (rand() % 2);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubTwoStart(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameTwoStart));
	if (wDataSize!=sizeof(CMD_S_GameTwoStart)) return false;

	//变量定义
	CMD_S_GameTwoStart * pTwoStart=(CMD_S_GameTwoStart *)pData;

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubThreeStart(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeStart));
	if (wDataSize!=sizeof(CMD_S_GameThreeStart)) return false;

	//变量定义
	CMD_S_GameThreeStart * pGameThreeStart=(CMD_S_GameThreeStart *)pData;


	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubThreeKaiJian(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeKaiJiang));
	if (wDataSize!=sizeof(CMD_S_GameThreeKaiJiang)) return false;

	//变量定义
	CMD_S_GameThreeKaiJiang * pGameThreeKaiJiang=(CMD_S_GameThreeKaiJiang *)pData;

	

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubThreeEnd(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==0);
	if (wDataSize!=0) return false;


	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//开始时间
	UINT nElapse= 2 + (rand() % TIME_USER_START_GAME);
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	if((rand() % 10) < 3)
	{
		UINT nElapseTemp = 2 + (rand() % 2);
		if(nElapse > nElapseTemp)
		{
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_BANK,nElapse);
		}
	}
	return true;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
