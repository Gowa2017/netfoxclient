#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS           2                 //最少时间

//游戏时间
#define TIME_USER_CALL_BANKER     3                 //叫庄时间
#define TIME_USER_START_GAME      5                 //开始时间
#define TIME_USER_ADD_SCORE       3                 //下注时间
#define TIME_USER_OPEN_CARD       3                 //摊牌时间

#define TIME_CHECK_BANKER       30                  //摊牌时间

//游戏时间
#define IDI_START_GAME          (100)                 //开始定时器
#define IDI_CALL_BANKER         (101)                 //叫庄定时器
#define IDI_USER_ADD_SCORE        (102)                 //下注定时器
#define IDI_OPEN_CARD         (103)                 //开牌定时器
#define IDI_DELAY_TIME          (105)                 //延时定时器



//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  m_nRobotBankStorageMul=0;
  m_lRobotBankGetScore=0;
  m_lRobotBankGetScoreBanker=0;
  ZeroMemory(m_lRobotScoreRange,sizeof(m_lRobotScoreRange));

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
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  //检查银行
  //UINT nElapse=TIME_CHECK_BANKER+rand()%100;
  //m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

  return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  try
  {
    switch(nTimerID)
    {
      case IDI_DELAY_TIME:
      {
        //开始时间
        UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME);
        m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

        return true;
      }
      case IDI_START_GAME:    //开始定时器
      {
		  IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
		  if (pUserItem)
		  {
			  //变量定义
			  SCORE lRobotScore = pUserItem->GetUserScore();
			  //判断存取
			  if ((lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)||(lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore))
			  {		
				  //发送机器人退出
				  m_pIAndroidUserItem->SendAndroidQuit(NULL, 0);
				  return true;
			  }
		  }
		  		  		//离开判断
		if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
		{
			//起立处理
			//PerformStandUpAction(pIServerUserItem);
        //发送信息
        m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
			//continue;
			return true;
		}

		//起立判断
		if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
		{
			//起立处理
			//PerformStandUpAction(pIServerUserItem);
        m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
			//continue;
			return true;
		}	


        //发送准备
        m_pIAndroidUserItem->SendUserReady(NULL,0);

        return true;
      }
      case IDI_CALL_BANKER:   //叫庄定时
      {
        //设置变量
        CMD_C_CallBanker CallBanker;
        CallBanker.bBanker = rand()%2;

        //发送信息
        m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER,&CallBanker,sizeof(CallBanker));

        return true;
      }
      case IDI_USER_ADD_SCORE:  //加注定时
      {
        //可下注筹码
        SCORE lUserMaxScore[MAX_JETTON_AREA];
        ZeroMemory(lUserMaxScore,sizeof(lUserMaxScore));
        SCORE lTemp=m_lTurnMaxScore;
        for(WORD i=0; i<MAX_JETTON_AREA; i++)
        {
          if(lTemp > 0)
            //lUserMaxScore[i] = __max(lTemp/((i+1)),1L);
          {
            lUserMaxScore[i]=__max(lTemp/(pow(2,i)),1L);
          }
          else
          {
            lUserMaxScore[i] = 1;
          }
        }

        //下注区域 60%概率选择中大注下
        BYTE cbAddScoreIndex = 0;
        BYTE cbRand1 = rand() % 100;
        BYTE cbRand2 = cbRand1 % 2;

        cbAddScoreIndex = (cbRand1 < 60) ? cbRand2 : (2 + cbRand2);

        //发送消息
        CMD_C_AddScore AddScore;
        AddScore.lScore=lUserMaxScore[cbAddScoreIndex%MAX_JETTON_AREA];
        m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

        return true;
      }
      case IDI_OPEN_CARD:     //开牌定时
      {
        //发送消息
        CMD_C_OxCard OxCard;
        OxCard.bOX=(m_GameLogic.GetCardType(m_HandCardData,MAX_COUNT)>0)?TRUE:FALSE;
        m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD,&OxCard,sizeof(OxCard));

        //BankOperate();

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
    case SUB_S_CALL_BANKER: //用户叫庄
    {
      //消息处理
      return OnSubCallBanker(pData,wDataSize);
    }
    case SUB_S_GAME_START:  //游戏开始
    {
      //消息处理
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_ADD_SCORE: //用户下注
    {
      //消息处理
      return OnSubAddScore(pData,wDataSize);
    }
    case SUB_S_SEND_CARD: //发牌消息
    {
      //消息处理
      return OnSubSendCard(pData,wDataSize);
    }
    case SUB_S_OPEN_CARD: //用户摊牌
    {
      //消息处理
      return OnSubOpenCard(pData,wDataSize);
    }
    case SUB_S_PLAYER_EXIT: //用户强退
    {
      //消息处理
      return OnSubPlayerExit(pData,wDataSize);
    }
    case SUB_S_GAME_END:  //游戏结束
    {
      //消息处理
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_ALL_CARD:
    {
      return true;
    }
    case SUB_S_ANDROID_BANKOPERATOR:
    {
      //BankOperate(2);
      return true;
    }
    case SUB_S_ADMIN_STORAGE_INFO:
    {
      return true;
    }
  }

  //错误断言
  ASSERT(FALSE);

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

      memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusFree->RobotConfig));

      BankOperate(2);

      //开始时间
      UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME);
      m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

      return true;
    }
    case GS_TK_CALL:  // 叫庄状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusCall))
      {
        return false;
      }
      CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;

      memcpy(m_szRoomName, pStatusCall->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusCall->RobotConfig));

      BankOperate(2);

      //始叫用户
      if(pStatusCall->wCallBanker==m_pIAndroidUserItem->GetChairID())
      {
        //叫庄时间
        UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
        m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER,nElapse);
      }

      return true;
    }
    case GS_TK_SCORE: //下注状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusScore))
      {
        return false;
      }
      CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

      memcpy(m_szRoomName, pStatusScore->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusScore->RobotConfig));

      BankOperate(2);

      //设置变量
      m_lTurnMaxScore=pStatusScore->lTurnMaxScore;
      WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

      //设置筹码
      if(pStatusScore->lTurnMaxScore>0L && pStatusScore->lTableScore[wMeChairId]==0L)
      {
        //下注时间
        UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
        m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
      }

      return true;
    }
    case GS_TK_PLAYING: //游戏状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusPlay->RobotConfig));

      BankOperate(2);

      //设置变量
      m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
      WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();
      CopyMemory(m_HandCardData,pStatusPlay->cbHandCardData[wMeChiarID],MAX_COUNT);

      //控件处理
      if(pStatusPlay->bOxCard[wMeChiarID]==0xff)
      {
        //开牌时间
        UINT nElapse = TIME_LESS + 2 + (rand() % TIME_USER_OPEN_CARD);
        m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD,nElapse);
      }

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

//用户叫庄
bool CAndroidUserItemSink::OnSubCallBanker(const void * pBuffer, WORD wDataSize)
{

  //效验数据
  if(wDataSize!=sizeof(CMD_S_CallBanker))
  {
    return false;
  }
  CMD_S_CallBanker * pCallBanker=(CMD_S_CallBanker *)pBuffer;

  //始叫用户
  if(pCallBanker->wCallBanker==m_pIAndroidUserItem->GetChairID())
  {
    //叫庄时间
    UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
    m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER,nElapse);
  }

  return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //设置变量
  m_lTurnMaxScore=pGameStart->lTurnMaxScore;

  //设置筹码
  if(pGameStart->lTurnMaxScore>0)
  {
    //下注时间
    UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
    m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
  }

  return true;
}

//用户下注
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_AddScore))
  {
    return false;
  }
  CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

  return true;
}

//发牌消息
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{

  //效验数据
  if(wDataSize!=sizeof(CMD_S_SendCard))
  {
    return false;
  }
  CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

  //设置数据
  WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();
  CopyMemory(m_HandCardData,pSendCard->cbCardData[wMeChiarID],sizeof(m_HandCardData));

  //开牌时间
  UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
  m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD,nElapse);

  return true;
}

//用户摊牌
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_Open_Card))
  {
    return false;
  }
  CMD_S_Open_Card * pOpenCard=(CMD_S_Open_Card *)pBuffer;

  return true;
}

//用户强退
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_PlayerExit))
  {
    return false;
  }
  CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

  return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //效验参数
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  //删除定时器
  m_pIAndroidUserItem->KillGameTimer(IDI_CALL_BANKER);
  m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
  m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);

  //开始时间
  if(pGameEnd->cbDelayOverGame==0)
  {
    UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME) + 3;
    m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

    //BankOperate(3);
  }
  else
  {
    UINT nElapse=/*TIME_LESS +*/ pGameEnd->cbDelayOverGame;
    m_pIAndroidUserItem->SetGameTimer(IDI_DELAY_TIME,nElapse);
  }


  //清理变量
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));


  return true;
}

////银行操作
//void CAndroidUserItemSink::BankOperate(BYTE cbType)
//{
//  if(cbType == 3)
//  {
//    if(rand() % 100 > 33)
//    {
//      return;
//    }
//  }
//  IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
//  if(pUserItem->GetUserStatus()>=US_SIT)
//  {
//    if(cbType==1)
//    {
//      CString strInfo;
//      strInfo.Format(TEXT("大厅：状态不对，不执行存取款"));
//      NcaTextOut(strInfo);
//
//      return;
//
//    }
//  }
//
//  //变量定义
//  SCORE lRobotScore = pUserItem->GetUserScore();
//
//  {
//    CString strInfo;
//    strInfo.Format(TEXT("[%s] 分数(%I64d)"),pUserItem->GetNickName(),lRobotScore);
//
//    if(lRobotScore > m_lRobotScoreRange[1])
//    {
//      CString strInfo1;
//      strInfo1.Format(TEXT("满足存款条件(%I64d)"),m_lRobotScoreRange[1]);
//      strInfo+=strInfo1;
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//    }
//    else if(lRobotScore < m_lRobotScoreRange[0])
//    {
//      CString strInfo1;
//      strInfo1.Format(TEXT("满足取款条件(%I64d)"),m_lRobotScoreRange[0]);
//      strInfo+=strInfo1;
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//    }
//
//    //判断存取
//    if(lRobotScore > m_lRobotScoreRange[1])
//    {
//      SCORE lSaveScore=0L;
//
//      lSaveScore = SCORE(lRobotScore*m_nRobotBankStorageMul/100);
//      if(lSaveScore > lRobotScore)
//      {
//        lSaveScore = lRobotScore;
//      }
//
//      if(lSaveScore > 0)
//      {
//        m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
//      }
//
//      SCORE lRobotNewScore = pUserItem->GetUserScore();
//
//      CString strInfo;
//      strInfo.Format(TEXT("[%s] 执行存款：存款前金币(%I64d)，存款后金币(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);
//
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//
//    }
//    else if(lRobotScore < m_lRobotScoreRange[0])
//    {
//
//      CString strInfo;
//      strInfo.Format(TEXT("配置信息：取款最小值(%I64d)，取款最大值(%I64d)"),m_lRobotBankGetScore,m_lRobotBankGetScoreBanker);
//
//      if(cbType==1)
//        NcaTextOut(strInfo);
//
//      SCORE lScore=rand()%m_lRobotBankGetScoreBanker+m_lRobotBankGetScore;
//      if(lScore > 0)
//      {
//        m_pIAndroidUserItem->PerformTakeScore(lScore);
//      }
//
//      SCORE lRobotNewScore = pUserItem->GetUserScore();
//
//      //CString strInfo;
//      strInfo.Format(TEXT("[%s] 执行取款：取款前金币(%I64d)，取款后金币(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);
//
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//
//    }
//  }
//}
//银行操作
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("大厅：状态不对，不执行存取款"));
			NcaTextOut(strInfo);
			return;
		}
	}

	//变量定义
	SCORE lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] 分数(%.2lf)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("满足存款条件(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("满足取款条件(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
		}

		//判断存取
		if (lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{			
			SCORE lSaveScore=0L;
			lSaveScore = lRobotScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore;

			if (lSaveScore > 0) 
			{
				lSaveScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			}
			SCORE lRobotNewScore = pUserItem->GetUserScore();

			CString strInfo;
			strInfo.Format(TEXT("[%s] 执行存款：存款前金币(%.2lf)，存款后金币(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo;
			SCORE lScore=m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore-lRobotScore;
			if (lScore > 0)
			{
				lScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformTakeScore(lScore);
			}

			SCORE lRobotNewScore = pUserItem->GetUserScore();
			strInfo.Format(TEXT("[%s] 执行取款：取款前金币(%.2lf)，取款后金币(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
		}
	}
}
//读取配置
void CAndroidUserItemSink::ReadConfigInformation(tagRobotConfig *pConfig)
{
  m_lRobotScoreRange[0] = pConfig->lRobotScoreMin;
  m_lRobotScoreRange[1] = pConfig->lRobotScoreMax;


  if(m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
  {
    m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
  }

  m_lRobotBankGetScore = pConfig->lRobotBankGet;

  m_lRobotBankGetScoreBanker = pConfig->lRobotBankGetBanker;

  m_nRobotBankStorageMul = pConfig->lRobotBankStoMul;


  if(m_nRobotBankStorageMul < 0 || m_nRobotBankStorageMul > 100)
  {
    m_nRobotBankStorageMul = 20;
  }
}


//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
