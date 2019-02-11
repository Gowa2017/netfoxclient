#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

#define IDI_OUT_CARD			101								//出牌定时
#define IDI_OPERATE				102								//操作定时

#define TIME_LESS				5								//最少时间
#define TIME_OUT_CARD			5								//出牌时间
#define TIME_OPERATE			5								//操作时间
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
  
  //接口变量
  m_pIAndroidUserItem=NULL;
  srand((unsigned)time(NULL));



  return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
  return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //查询接口
  m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
  if(m_pIAndroidUserItem==NULL)
  {
    return false;
  }


  //检查银行
  //UINT nElapse=TIME_CHECK_BANKER+rand()%100;
  //m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

  return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
  

  return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  try
  {
    switch(nTimerID)
    {
		case IDI_OUT_CARD:
		{
			CMD_C_OutCard OutCard;
			OutCard.cbCardData = m_cbHandCardData[MAX_COUNT - 1];

			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCard, sizeof(OutCard));
			return true;
		}

    }

  }
  catch(...)
  {
    CString cs;
    cs.Format(TEXT("异常ID=%d"),nTimerID);
    CTraceService::TraceString(cs,TraceLevel_Debug);
  }

  return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:  //游戏开始
    {
		//消息处理
		return OnSubGameStart(pData,wDataSize);
    }
	case SUB_S_OUT_CARD:	//玩家出牌
	{
		//消息处理
		return OnSubOutCard(pData,wDataSize);
	}
    case SUB_S_SEND_CARD: //发牌消息
    {
		//消息处理
		return OnSubSendCard(pData,wDataSize);
    }
	case SUB_S_OPERATE_NOTIFY:
	{
		//消息处理
		return OnSubOperateNotify(pData,wDataSize);
	}
  }

  return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:    //空闲状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //消息处理
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;


      //ReadConfigInformation(&(pStatusFree->RobotConfig));
	  m_pIAndroidUserItem->SendUserReady(NULL,0);

      return true;
    }
  }

  ASSERT(FALSE);

  return false;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);

	CopyMemory(m_cbHandCardData, pGameStart->cbCardData, sizeof(m_cbHandCardData));
	m_cbHandCardCount = MAX_COUNT-1;

	if (m_pIAndroidUserItem->GetChairID() == pGameStart->wBankerUser)
	{
		m_cbHandCardCount = MAX_COUNT;
		int nElapse = (rand() % TIME_OUT_CARD) + TIME_LESS;

		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, nElapse);
	}

	return true;
}

//玩家出牌
bool CAndroidUserItemSink::OnSubOutCard(const VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_OutCard));
	if (wDataSize != sizeof(CMD_S_OutCard)) return false;

	//消息处理
	CMD_S_OutCard * pOutCard = (CMD_S_OutCard *)pBuffer;




	if (m_pIAndroidUserItem->GetChairID() == pOutCard->wOutCardUser)
	{
		BYTE cbTempCard[MAX_COUNT];
		CopyMemory(cbTempCard,m_cbHandCardData,sizeof(cbTempCard));

		for (int i = 0; i < MAX_COUNT; i++)
		{
			if (cbTempCard[i] == pOutCard->cbOutCardData)
			{
				cbTempCard[i] = 0;
				break;
			}
		}

		ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

		int restcount = 0;
		for (int i = 0; i < MAX_COUNT; i++)
		{
			if (cbTempCard[i] != 0)
			{
				m_cbHandCardData[restcount] = cbTempCard[i];
				restcount++;
			}
		}
		m_cbHandCardCount = restcount;
	}

	return true;
}

//发送牌
bool CAndroidUserItemSink::OnSubSendCard(const VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//消息处理
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	m_cbHandCardData[m_cbHandCardCount] = pSendCard->cbCardData;
	m_cbHandCardCount++;

	if (m_pIAndroidUserItem->GetChairID() == pSendCard->wCurrentUser)
	{
		int nElapse = (rand() % TIME_OUT_CARD) + TIME_LESS;

		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, nElapse);
	}

	return true;
}
//操作提示
bool CAndroidUserItemSink::OnSubOperateNotify(const VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_OperateNotify));
	if (wDataSize != sizeof(CMD_S_OperateNotify)) return false;

	//消息处理
	CMD_S_OperateNotify * pOperateNotify = (CMD_S_OperateNotify *)pBuffer;


	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCard[0] = 0;
	OperateCard.cbOperateCard[1] = 0;
	OperateCard.cbOperateCard[2] = 0;

	if ( (pOperateNotify->cbActionMask & WIK_CHI_HU) != 0)
		OperateCard.cbOperateCode = WIK_CHI_HU;
	else
		OperateCard.cbOperateCode = WIK_NULL;

	//发送消息
	m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD, &OperateCard, sizeof(OperateCard));


	return true;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
