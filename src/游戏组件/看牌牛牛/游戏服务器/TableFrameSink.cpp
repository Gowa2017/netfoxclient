#include "StdAfx.h"
#include "TableFrameSink.h"
#include <conio.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD      CTableFrameSink::m_wPlayerCount=GAME_PLAYER;      //游戏人数
bool        CTableFrameSink::m_bFirstInit=true;


//全局变量
SCORE          g_lStorageStart = 0;            //库存起始值
SCORE          g_lStorageCurrent = 0;            //当前库存
SCORE          g_lStorageDeduct = 0;           //回扣变量
SCORE          g_lStorageMax = 50000000;         //库存封顶
WORD            g_wStorageMul = 0;              //系统输钱比例

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo; //玩家USERID映射玩家信

//////////////////////////////////////////////////////////////////////////

#define IDI_SO_OPERATE              2             //代打定时器
#define TIME_SO_OPERATE             35000           //代打定时器

//动作标识
#define IDI_DELAY_ENDGAME     10        //动作标识
#define IDI_DELAY_TIME        3000      //延时时间
#define IDI_USERUP              3             //代打定时器
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
  //游戏变量
  m_lExitScore=0;
  m_lDynamicScore=0;
  m_wBankerUser=INVALID_CHAIR;
  m_wFisrtCallUser=INVALID_CHAIR;
  m_wCurrentUser=INVALID_CHAIR;
m_WinnerScore = 0;
  //用户状态
  ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
  ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
  ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
  for(BYTE i=0; i<m_wPlayerCount; i++)
  {
    m_cbOxCard[i]=0xff;
  }
	m_bIsGameRate = false;
  //扑克变量
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

  //下注信息
  ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

  ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));
	ZeroMemory(m_cbExchangeCard,sizeof(m_cbExchangeCard));
  //组件变量
  m_pITableFrame=NULL;
  m_pGameServiceOption=NULL;

  //服务控制
  m_hInst = NULL;
  m_pServerContro = NULL;
  m_hInst = LoadLibrary(TEXT("OxKPNewServerControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)();
    CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl");
    if(ServerControl)
    {
      m_pServerContro = static_cast<IServerControl*>(ServerControl());
    }
  }


  if(m_bFirstInit)
  {
    CString strName = GetFileDialogPath()+  "\\OxNewBattle.log";
    CFileFind findLogFile;
    if(findLogFile.FindFile(strName))
    {
      ::DeleteFile(strName);
    }

    m_bFirstInit=false;
  }

  //AllocConsole();
  //freopen("CON", "wt", stdout);

  return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
  if(m_pServerContro)
  {
    delete m_pServerContro;
    m_pServerContro = NULL;
  }

  if(m_hInst)
  {
    FreeLibrary(m_hInst);
    m_hInst = NULL;
  }
}

//接口查询--检测相关信息版本
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
  QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
  return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //查询接口
  ASSERT(pIUnknownEx!=NULL);
  m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
  if(m_pITableFrame==NULL)
  {
    return false;
  }

  m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

  //游戏配置
  m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
  m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

  //读取库存
  tagCustomRule *pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
  ASSERT(pGameCustomRule != NULL);

  //初始化库存
  g_lStorageCurrent = pGameCustomRule->lStorageStart;
  g_lStorageStart = pGameCustomRule->lStorageStart;
  g_lStorageDeduct = pGameCustomRule->lStorageDeduct;
  g_lStorageMax = pGameCustomRule->lStorageMax;
  g_wStorageMul = (WORD)(pGameCustomRule->lStorageMul);
  m_lBonus = pGameCustomRule->lBonus;

  //校验数据
  if(g_lStorageDeduct < 0 || g_lStorageDeduct > 1000)
  {
    g_lStorageDeduct = 0;
  }

  if(g_lStorageDeduct > 1000)
  {
    g_lStorageDeduct = 1000;
  }

  if(g_wStorageMul < 0 || g_wStorageMul > 100)
  {
    g_wStorageMul = 50;
  }

  return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
  //游戏变量
  m_lExitScore=0;
  m_wCurrentUser=INVALID_CHAIR;
  m_lDynamicScore=0;
  //用户状态
  ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
  ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
  ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
  for(BYTE i=0; i<m_wPlayerCount; i++)
  {
    m_cbOxCard[i]=0xff;
  }
m_WinnerScore = 0;
	m_bIsGameRate = false;
  //扑克变量
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

  //下注信息
  ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));
	ZeroMemory(m_cbExchangeCard,sizeof(m_cbExchangeCard));
  return;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
  ASSERT(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE);
  if(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE)
  {
    return true;
  }
  return false;
}


//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  ////历史积分
  //if(bLookonUser==false)
  //{
  //  m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID());
  //}

  if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
  {
	  if (pIServerUserItem->GetUserInfo()->cbUserStatus != US_LOOKON)
	  {
		  m_cbDynamicJoin[pIServerUserItem->GetChairID()]=true;
	  }
  }

  //int nCount = 0;
  ////用户状态
  //for(WORD i=0; i<m_wPlayerCount; i++)
  //{
	 // //获取用户
	 // IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
	 // if(pIServerUser==NULL)continue;
	 // if(!pIServerUser->IsAndroidUser())nCount++;
  //}

  if(m_pITableFrame->GetGameStatus()==GS_TK_FREE)
  {
	  for (int i = 0;i<GAME_PLAYER;i++ )
	  {
		//获取用户
		  IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		  if(pIServerUser != NULL&&pIServerUser->IsAndroidUser())
		  {
			if(pIServerUser->GetUserInfo()->cbUserStatus >= US_PLAYING)
			{
				pIServerUser->SetUserStatus(US_READY,pIServerUser->GetTableID(),pIServerUser->GetChairID());
			}
		  }
	  }
	 //m_pITableFrame->SetGameTimer(IDI_USERUP,20000,1,0);
  }
  ////过滤机器人
  //if(pIServerUserItem->IsAndroidUser() == false)
  //{
  //  //查找已存在的用户
  //  ROOMUSERINFO lastroomUserInfo;
  //  ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
  //  if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo) == false)
  //  {
  //    UpdateUserRosterInfo(pIServerUserItem);
  //  }
  //  else //存在用户
  //  {
  //    RemoveUserRoster();
  //  }
  //}

  return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  //历史积分
  if(bLookonUser==false)
  {
   // m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
    m_cbDynamicJoin[pIServerUserItem->GetChairID()]=false;
    m_bSpecialClient[pIServerUserItem->GetChairID()]=false;
  }

 // RemoveUserRoster();

  return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
  ////库存
  // if(g_lStorageCurrent>0 && NeedDeductStorage())
  //{
  //  g_lStorageCurrent=g_lStorageCurrent-g_lStorageCurrent*g_lStorageDeduct/1000;
  //}

  //CString strInfo;
  //strInfo.Format(TEXT("当前库存：%I64d"), g_lStorageCurrent);
  //NcaTextOut(strInfo);

  ////设置状态
  //m_pITableFrame->SetGameStatus(GS_TK_CALL);

  //用户状态
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    //获取用户
    IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);

    if(pIServerUser==NULL)
    {
      m_cbPlayStatus[i]=FALSE;
    }
    else
    {
      m_cbPlayStatus[i]=TRUE;
    }
  }

  //ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));

  ////首局随机始叫
  //if(m_wFisrtCallUser==INVALID_CHAIR)
  //{
  //  m_wFisrtCallUser=rand()%m_wPlayerCount;
  //}
  //else
  //{
  //  m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
  //}
  //if(m_cbPlayStatus[m_wFisrtCallUser]==FALSE)
  //{
	 //do
	 //{
		// m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	 //} while (m_cbPlayStatus[m_wFisrtCallUser]==FALSE);
  //}



  WORD wFisrtChooseCallUser =rand()%m_wPlayerCount;

  if(m_cbPlayStatus[wFisrtChooseCallUser]==FALSE)
  {
	  do
	  {
		  wFisrtChooseCallUser=(wFisrtChooseCallUser+1)%m_wPlayerCount;
	  } while (m_cbPlayStatus[wFisrtChooseCallUser]==FALSE);
  }


  //随机扑克
  BYTE bTempArray[52];
  m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray));

  	WORD wIndex = 0;
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
	  if(m_cbPlayStatus[i] == FALSE)
	  {
		  continue;
	  }
	  //派发扑克
	  CopyMemory(m_cbHandCardData[i], &bTempArray[wIndex*MAX_COUNT], MAX_COUNT);
	  wIndex++;
  }
	ZeroMemory(m_cbExchangeCard,sizeof(m_cbExchangeCard));
	m_cbCardIndex = 0;
	for(int i = wIndex*MAX_COUNT ;i < 52;i++ )
	{
		m_cbExchangeCard[m_cbCardIndex] = bTempArray[i];
		m_cbCardIndex++;
	}

	for (int j = 0;j < GAME_PLAYER;j++)
	{
		IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
		if (pServerUser == NULL) continue;
		if(pServerUser->GetGameRate() <101)
		{
			//m_bUserControl = true;
			if(rand()%100 < pServerUser->GetGameRate())
			{
				ProbeJudge(j,CHEAT_TYPE_WIN);  //指定玩家胜利
				m_bIsGameRate = true;
			}
			else
			{
				ProbeJudge(j,CHEAT_TYPE_LOST);  //指定玩家失败
				m_bIsGameRate = true;
			}
		}
	}


  m_wBankerUser = INVALID_CHAIR;

  m_wCurrentUser=INVALID_CHAIR;


  //m_bBuckleServiceCharge[m_wBankerUser]=true;
  //设置状态
  m_pITableFrame->SetGameStatus(GS_TK_CALL);

  //庄家积分
  //IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
  //SCORE lBankerScore=pIServerUserItem->GetUserScore();

  ////最大下注
  //for(WORD i=0; i<m_wPlayerCount; i++)
  //{
	 // if(m_cbPlayStatus[i]!=TRUE || i==m_wBankerUser)
	 // {
		//  continue;
	 // }

	 // //下注变量 客户要求
	 // m_lTurnMaxScore[i]=GetUserMaxTurnScore(i);
  //}

  //当前用户
  //m_wCurrentUser=m_wFisrtCallUser;

  //设置变量
  CMD_S_CallBanker CallBanker;
  CallBanker.wCallBanker=m_wCurrentUser;
  CallBanker.bFirstTimes=true;
  CopyMemory(CallBanker.cbPlayerStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
  CallBanker.wFisrtChooseCallUser = wFisrtChooseCallUser;



  //发送数据
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_cbPlayStatus[i]!=TRUE)
    {
      continue;
    }
	for(int j =0;j<MAX_COUNT - 1;j++)
	{
		CallBanker.cbHandCardData[i][j] = m_cbHandCardData[i][j];
	}
    m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
  }
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));

  m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
  m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
  m_dwGameTime=GetTickCount();

  ////随机扑克
  //CMD_S_AllCard AllCard= {0};
  //BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
  //m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
  //for(WORD i=0; i<m_wPlayerCount; i++)
  //{
  //  IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
  //  if(pIServerUser==NULL)
  //  {
  //    continue;
  //  }
  //  if(pIServerUser->IsAndroidUser())
  //  {
  //    AllCard.bAICount[i] =true;
  //  }

  //  //派发扑克
  //  CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
  //  CopyMemory(AllCard.cbCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
  //}

//  //发送数据
//  for (WORD i=0;i<m_wPlayerCount;i++)
//  {
//    IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
//    if(pIServerUser==NULL)continue;
//#ifndef _DEBUG
//    if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false || m_bSpecialClient[i]==false)continue;
//#endif
//    m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
//  }
  return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  switch(cbReason)
  {
    case GER_DISMISS:   //游戏解散
    {
      ////效验参数
      //ASSERT(pIServerUserItem!=NULL);
      //ASSERT(wChairID<GAME_PLAYER);

      //构造数据
      CMD_S_GameEnd GameEnd = {0};

      //发送信息
      //m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
     // m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	  //删除时间
	  m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      return true;
    }
    case GER_NORMAL:    //常规结束
    {
      //删除时间
      m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

      //定义变量
      CMD_S_GameEnd GameEnd;
      ZeroMemory(&GameEnd,sizeof(GameEnd));
      WORD wWinTimes[GAME_PLAYER],wWinCount[GAME_PLAYER];
      ZeroMemory(wWinCount,sizeof(wWinCount));
      ZeroMemory(wWinTimes,sizeof(wWinTimes));

      //保存扑克
      BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
      CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

      //庄家倍数
      ASSERT(m_cbOxCard[m_wBankerUser]<2);
      if(m_cbOxCard[m_wBankerUser]==TRUE)
      {
        wWinTimes[m_wBankerUser]=m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser],MAX_COUNT) * m_cbCallStatus[m_wBankerUser];
      }
      else
      {
        wWinTimes[m_wBankerUser]=1* m_cbCallStatus[m_wBankerUser];
      }

      //对比玩家
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)
        {
          continue;
        }

        ASSERT(m_cbOxCard[i]<2);

        //对比扑克
        if(m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,m_cbOxCard[i],m_cbOxCard[m_wBankerUser]))
        {
          wWinCount[i]++;
          //获取倍数
          if(m_cbOxCard[i]==TRUE)
          {
            wWinTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT)* m_cbCallStatus[m_wBankerUser];
          }
          else
          {
            wWinTimes[i]=1* m_cbCallStatus[m_wBankerUser];
          }
        }
        else
        {
          wWinCount[m_wBankerUser]++;
        }
      }

      //统计得分
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)
        {
          continue;
        }

        if(wWinCount[i]>0)  //闲家胜利
        {
          GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
          GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
          m_lTableScore[i]=0;

          //if((m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT) == OX_FIVEKING ? true : false))
          //{
          //  GameEnd.lGameScore[i] += m_lBonus;
          //}
        }
        else          //庄家胜利
        {
          GameEnd.lGameScore[i]=(-1)*m_lTableScore[i]*wWinTimes[m_wBankerUser];
          GameEnd.lGameScore[m_wBankerUser]+=(-1)*GameEnd.lGameScore[i];
          m_lTableScore[i]=0;

          //if((m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser], MAX_COUNT) == OX_FIVEKING ? true : false))
          //{
          //  GameEnd.lGameScore[m_wBankerUser] += m_lBonus;
          //}
        }
      }

      //闲家强退分数
      GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

	  //统计得分
	  for(WORD i=0; i<m_wPlayerCount; i++)
	  {
		  if(m_cbPlayStatus[i]==FALSE)
			  continue;
		  IServerUserItem * pIUserItem = m_pITableFrame->GetTableUserItem(i);
		  if (pIUserItem)
		  {
			  if (GameEnd.lGameScore[i] >0 )
			  {
				 if (GameEnd.lGameScore[i] > pIUserItem->GetUserInfo()->lScore)
				 {
					 GameEnd.lGameScore[i] = pIUserItem->GetUserInfo()->lScore;
				 }
			  }
			  else
			  {
				 if ((GameEnd.lGameScore[i]*(-1)) > pIUserItem->GetUserInfo()->lScore)
				 {
					 GameEnd.lGameScore[i] = (-1)*pIUserItem->GetUserInfo()->lScore;
				 }
			  }
		  }
	  }
      //五花牛
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        //获取用户
        IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
          continue;
        }

        GameEnd.bfiveKing[i] = (m_GameLogic.GetCardType(m_cbHandCardData[i], MAX_COUNT) == OX_FIVEKING ? true : false);

        //if(GameEnd.bfiveKing[i] && !pIServerUserItem->IsAndroidUser())
        //{
        //  CString strInfo;
        //  strInfo.Format(TEXT("真人玩家[%s]开出五花牛，获得彩金[%I64d]\n"), pIServerUserItem->GetNickName(), m_lBonus);
        //  NcaTextOut(strInfo);

        //  g_lStorageCurrent -= m_lBonus;
        //}
      }

      //离开用户
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(m_lTableScore[i]>0)
        {
          GameEnd.lGameScore[i]=-m_lTableScore[i];
        }
      }

      //修改积分
      tagScoreInfo ScoreInfoArray[GAME_PLAYER];
      ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

      bool bDelayOverGame=false;

      //积分税收
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(m_cbPlayStatus[i]==FALSE)
        {
          continue;
        }

        if(GameEnd.lGameScore[i]>0L)
        {
          GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
          if(GameEnd.lGameTax[i]>0)
          {
            GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
          }
        }

        //历史积分
        m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);

        //保存积分
        ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
        ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
        ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

        //if(ScoreInfoArray[i].cbType ==SCORE_TYPE_LOSE && bDelayOverGame==false)
        //{
        //  IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
        //  if(pUserItem!=NULL && (pUserItem->GetUserScore()+GameEnd.lGameScore[i])<m_pGameServiceOption->lMinTableScore)
        //  {
        //    bDelayOverGame=true;
        //  }
        //}
      }

#ifdef _DEBUG

      bDelayOverGame=true;

#endif // _DEBUG

      if(bDelayOverGame)
      {
        GameEnd.cbDelayOverGame = 3;
      }

      //发送信息
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
        {
          continue;
        }
        m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      }

      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      TryWriteTableScore(ScoreInfoArray);

      //库存统计
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        //获取用户
        IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserIte==NULL)
        {
          continue;
        }

        //库存累计
        if(!pIServerUserIte->IsAndroidUser())
        {
          g_lStorageCurrent-=GameEnd.lGameScore[i];
        }

      }

      ////更新用户名单
      //for(WORD i=0; i<GAME_PLAYER; i++)
      //{
      //  //获取用户
      //  IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

      //  if(!pIServerUserItem)
      //  {
      //    continue;
      //  }

      //  SCORE lGameScore = 0;

      //  //查找已存在的用户
      //  ROOMUSERINFO lastroomUserInfo;
      //  ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
      //  if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo)
      //     && lastroomUserInfo.userroster != INVALID_ROSTER)
      //  {
      //    lGameScore = lastroomUserInfo.lGameScore + GameEnd.lGameScore[i];
      //    UpdateUserRosterInfo(pIServerUserItem, lGameScore, lastroomUserInfo.userroster, lastroomUserInfo.lScoreLimit);
      //    RemoveUserRoster();
      //  }
      //}

      ////游戏玩家
      //for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
      //{
      //  IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
      //  if(pIServerUserItem==NULL)
      //  {
      //    continue;
      //  }
      //  //权限判断
      //  if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      //  {
      //    CMD_S_ADMIN_STORAGE_INFO StorageInfo;
      //    ZeroMemory(&StorageInfo,sizeof(StorageInfo));
      //    StorageInfo.lCurrentStorage = g_lStorageCurrent;
      //    StorageInfo.lCurrentDeduct = g_lStorageDeduct;
      //    StorageInfo.lMaxStorage = g_lStorageMax;
      //    StorageInfo.wStorageMul = g_wStorageMul;
      //    m_pITableFrame->SendTableData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      //    m_pITableFrame->SendLookonData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      //  }
      //}

      ////结束游戏
      //if(bDelayOverGame)
      //{
      //  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
      //  m_pITableFrame->SetGameTimer(IDI_DELAY_ENDGAME,IDI_DELAY_TIME,1,0L);
      //}
      //else
      //{
        m_pITableFrame->ConcludeGame(GS_TK_FREE);
      //}
      return true;
    }
    case GER_USER_LEAVE:    //用户强退
    case GER_NETWORK_ERROR:
    {
      //效验参数
      ASSERT(pIServerUserItem!=NULL);
      ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]==TRUE||m_cbDynamicJoin[wChairID]==FALSE));

      if(m_cbPlayStatus[wChairID]==FALSE)
      {
        return true;
      }
      //设置状态
      m_cbPlayStatus[wChairID]=FALSE;
      m_cbDynamicJoin[wChairID]=FALSE;

      //定义变量
      CMD_S_PlayerExit PlayerExit;
      PlayerExit.wPlayerID=wChairID;

      //发送信息
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(i==wChairID || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))
        {
          continue;
        }
        m_pITableFrame->SendTableData(i,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));
      }
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));


      WORD wWinTimes[GAME_PLAYER];

      ZeroMemory(wWinTimes,sizeof(wWinTimes));

      if(m_pITableFrame->GetGameStatus()>GS_TK_CALL)
      {
        if(wChairID==m_wBankerUser)   //庄家强退
        {
          //定义变量
          CMD_S_GameEnd GameEnd;
          ZeroMemory(&GameEnd,sizeof(GameEnd));
          ZeroMemory(wWinTimes,sizeof(wWinTimes));

          BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
          CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

          //得分倍数
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)
            {
              continue;
            }
            wWinTimes[i]=(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)?(1):(m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT))* m_cbCallStatus[m_wBankerUser];
          }

          //统计得分 已下或没下
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)
            {
              continue;
            }
            GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
            GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
            m_lTableScore[i]=0;
          }

          //闲家强退分数
          GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

          //离开用户
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_lTableScore[i]>0)
            {
              GameEnd.lGameScore[i]=-m_lTableScore[i];
            }
          }

          //修改积分
          tagScoreInfo ScoreInfoArray[GAME_PLAYER];
          ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

          //积分税收
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)
            {
              continue;
            }

            if(GameEnd.lGameScore[i]>0L)
            {
              GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
              if(GameEnd.lGameTax[i]>0)
              {
                GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
              }
            }

            //保存积分
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];

            if(i==m_wBankerUser)
            {
              ScoreInfoArray[i].cbType =SCORE_TYPE_FLEE;
            }
            else if(m_cbPlayStatus[i]==TRUE)
            {
              ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
            }
          }

          TryWriteTableScore(ScoreInfoArray);

          //发送信息
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(i==m_wBankerUser || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))
            {
              continue;
            }
            m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
          }
          m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		  m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
          //写入库存
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)
            {
              continue;
            }

            //获取用户
            IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);

            //库存累计
            if((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser()))
            {
              g_lStorageCurrent-=GameEnd.lGameScore[i];
            }

          }
          //结束游戏
          m_pITableFrame->ConcludeGame(GS_TK_FREE);

          //更新用户名单
          for(WORD i=0; i<GAME_PLAYER; i++)
          {
            //获取用户
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            if(!pIServerUserItem)
            {
              continue;
            }

            SCORE lGameScore = 0;

            //查找已存在的用户
            ROOMUSERINFO lastroomUserInfo;
            ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
            if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo)
               && lastroomUserInfo.userroster != INVALID_ROSTER)
            {
              lGameScore = lastroomUserInfo.lGameScore + GameEnd.lGameScore[i];
              UpdateUserRosterInfo(pIServerUserItem, lGameScore, lastroomUserInfo.userroster, lastroomUserInfo.lScoreLimit);
              RemoveUserRoster();
            }
          }
        }
        else            //闲家强退
        {
          //已经下注
          if(m_lTableScore[wChairID]>0L)
          {
            ZeroMemory(wWinTimes,sizeof(wWinTimes));

            //用户扑克
            BYTE cbUserCardData[MAX_COUNT];
            CopyMemory(cbUserCardData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);

            //用户倍数
            wWinTimes[m_wBankerUser]=(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)?(1):(m_GameLogic.GetTimes(cbUserCardData,MAX_COUNT));

            //修改积分
            SCORE lScore=-m_lTableScore[wChairID]*wWinTimes[m_wBankerUser];
            m_lExitScore+=(-1*lScore);
            m_lTableScore[wChairID]=(-1*lScore);

            tagScoreInfo ScoreInfoArray[GAME_PLAYER];
            ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
            ScoreInfoArray[wChairID].lScore = lScore;
            ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;

            TryWriteTableScore(ScoreInfoArray);

            //获取用户
            IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);

            //库存累计
            if((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser()))
            {
              g_lStorageCurrent-=lScore;
            }

            //更新用户名单
            for(WORD i=0; i<GAME_PLAYER; i++)
            {
              //获取用户
              IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

              if(!pIServerUserItem || i != wChairID)
              {
                continue;
              }

              SCORE lGameScore = 0;

              //查找已存在的用户
              ROOMUSERINFO lastroomUserInfo;
              ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
              if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo)
                 && lastroomUserInfo.userroster != INVALID_ROSTER)
              {
                lGameScore = lastroomUserInfo.lGameScore + lScore;
                UpdateUserRosterInfo(pIServerUserItem, lGameScore, lastroomUserInfo.userroster, lastroomUserInfo.lScoreLimit);
                RemoveUserRoster();
              }
            }
          }

          //玩家人数
          WORD wUserCount=0;
          for(WORD i=0; i<m_wPlayerCount; i++)if(m_cbPlayStatus[i]==TRUE)
            {
              wUserCount++;
            }

          //结束游戏
          if(wUserCount==1)
          {
            //定义变量
            CMD_S_GameEnd GameEnd;
            ZeroMemory(&GameEnd,sizeof(GameEnd));
            ASSERT(m_lExitScore>=0L);

            //统计得分
            GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;
            GameEnd.lGameTax[m_wBankerUser]=m_pITableFrame->CalculateRevenue(m_wBankerUser,GameEnd.lGameScore[m_wBankerUser]);
            GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameTax[m_wBankerUser];

            //离开用户
            for(WORD i=0; i<m_wPlayerCount; i++)
            {
              if(m_lTableScore[i]>0)
              {
                GameEnd.lGameScore[i]=-m_lTableScore[i];
              }
            }

            //发送信息
            m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
            m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
            for(WORD Zero=0; Zero<m_wPlayerCount; Zero++)if(m_lTableScore[Zero]!=0)
              {
                break;
              }
            if(Zero!=m_wPlayerCount)
            {
              //修改积分
              tagScoreInfo ScoreInfoArray[GAME_PLAYER];
              ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
              ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
              ScoreInfoArray[m_wBankerUser].lRevenue = GameEnd.lGameTax[m_wBankerUser];
              ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_WIN;

              TryWriteTableScore(ScoreInfoArray);

              //获取用户
              IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);

              //库存累计
              if((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser()))
              {
                g_lStorageCurrent-=GameEnd.lGameScore[m_wBankerUser];
              }

            }

            //结束游戏
            m_pITableFrame->ConcludeGame(GS_TK_FREE);

            //更新用户名单
            for(WORD i=0; i<GAME_PLAYER; i++)
            {
              //获取用户
              IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

              if(!pIServerUserItem)
              {
                continue;
              }

              SCORE lGameScore = 0;

              //查找已存在的用户
              ROOMUSERINFO lastroomUserInfo;
              ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
              if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo)
                 && lastroomUserInfo.userroster != INVALID_ROSTER)
              {
                lGameScore = lastroomUserInfo.lGameScore + GameEnd.lGameScore[i];
                UpdateUserRosterInfo(pIServerUserItem, lGameScore, lastroomUserInfo.userroster, lastroomUserInfo.lScoreLimit);
                RemoveUserRoster();
              }
            }
          }
          else if(m_pITableFrame->GetGameStatus()==GS_TK_SCORE && m_lTableScore[wChairID]==0L)
          {
            OnUserAddScore(wChairID,0);
          }
          else if(m_pITableFrame->GetGameStatus()==GS_TK_PLAYING && m_cbOxCard[wChairID]==0xff)
          {
            OnUserOpenCard(wChairID,(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0)?TRUE:FALSE);
          }
        }
      }
      else
      {
        //玩家人数
        WORD wUserCount=0;
        for(WORD i=0; i<m_wPlayerCount; i++)if(m_cbPlayStatus[i]==TRUE)
          {
            wUserCount++;
          }

        //结束游戏
        if(wUserCount==1)
        {
          //定义变量
          CMD_S_GameEnd GameEnd;
          ZeroMemory(&GameEnd,sizeof(GameEnd));

          //发送信息
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
            {
              continue;
            }
            m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
          }
          m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		  m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
          //结束游戏
          m_pITableFrame->ConcludeGame(GS_TK_FREE);
        }
        else if(m_wCurrentUser==wChairID)
        {
          OnUserCallBanker(wChairID,0);
        }
      }
      return true;
    }
  }

  return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:    //空闲状态
    {
      //构造数据
      CMD_S_StatusFree StatusFree;
      ZeroMemory(&StatusFree,sizeof(StatusFree));

      //设置变量
      StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

      //历史积分
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
        StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
        StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
      }
      //房间名称
      CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

      //机器人配置
      tagRobotConfig RobotConfig;
      ZeroMemory(&RobotConfig, sizeof(RobotConfig));
      tagCustomRule* pCustomRule = (tagCustomRule*)(m_pGameServiceOption->cbCustomRule);
      RobotConfig.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      RobotConfig.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      RobotConfig.lRobotBankGet = pCustomRule->lRobotBankGet;
      RobotConfig.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      RobotConfig.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CopyMemory(&(StatusFree.RobotConfig), &RobotConfig, sizeof(RobotConfig));
      StatusFree.lStartStorage = g_lStorageStart;
      StatusFree.lBonus = m_lBonus;

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo,sizeof(StorageInfo));
        StorageInfo.lCurrentStorage = g_lStorageCurrent;
        StorageInfo.lCurrentDeduct = g_lStorageDeduct;
        StorageInfo.lMaxStorage = g_lStorageMax;
        StorageInfo.wStorageMul = g_wStorageMul;

        m_pITableFrame->SendTableData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      }
      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
    }
    case GS_TK_CALL:  //叫庄状态
    {
      //构造数据
      CMD_S_StatusCall StatusCall;
      ZeroMemory(&StatusCall,sizeof(StatusCall));

      //历史积分
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
        StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
        StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
      }
		StatusCall.lCellScore = m_pGameServiceOption->lCellScore;
      //设置变量
      StatusCall.wCallBanker=m_wCurrentUser;
      StatusCall.cbDynamicJoin=m_cbDynamicJoin[wChairID];
      CopyMemory(StatusCall.cbPlayStatus,m_cbPlayStatus,sizeof(StatusCall.cbPlayStatus));
	  DWORD dwPassTime= (GetTickCount()-m_dwGameTime)/1000;
	  StatusCall.cbTimeLeave =30 - dwPassTime;
	  if(30 - dwPassTime <= 0)StatusCall.cbTimeLeave = 2;
      //房间名称
      CopyMemory(StatusCall.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusCall.szGameRoomName));

      //机器人配置
      tagRobotConfig RobotConfig;
      ZeroMemory(&RobotConfig, sizeof(RobotConfig));
      tagCustomRule* pCustomRule = (tagCustomRule*)(m_pGameServiceOption->cbCustomRule);
      RobotConfig.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      RobotConfig.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      RobotConfig.lRobotBankGet = pCustomRule->lRobotBankGet;
      RobotConfig.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      RobotConfig.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CopyMemory(&(StatusCall.RobotConfig), &RobotConfig, sizeof(RobotConfig));
      StatusCall.lStartStorage = g_lStorageStart;
      StatusCall.lBonus = m_lBonus;

	  //设置扑克
	  for(WORD i=0; i< m_wPlayerCount; i++)
	  {
		  if(m_cbPlayStatus[i]==FALSE)
			  continue;
			for(int j =0;j<MAX_COUNT - 1;j++)
			{
				StatusCall.cbHandCardData[i][j] = m_cbHandCardData[i][j];
			}
			StatusCall.cbCallStatus[i] = m_cbCallStatus[i];
	  }
      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo,sizeof(StorageInfo));
        StorageInfo.lCurrentStorage = g_lStorageCurrent;
        StorageInfo.lCurrentDeduct = g_lStorageDeduct;
        StorageInfo.lMaxStorage = g_lStorageMax;
        StorageInfo.wStorageMul = g_wStorageMul;

        m_pITableFrame->SendTableData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
    }
    case GS_TK_SCORE: //下注状态
    {
      //构造数据
      CMD_S_StatusScore StatusScore;
      memset(&StatusScore,0,sizeof(StatusScore));

      //历史积分
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
        StatusScore.lTurnScore[i]=pHistoryScore->lTurnScore;
        StatusScore.lCollectScore[i]=pHistoryScore->lCollectScore;
      }
		StatusScore.lCellScore = m_pGameServiceOption->lCellScore;
      //加注信息
      StatusScore.lTurnMaxScore=GetUserMaxTurnScore(wChairID);
      StatusScore.wBankerUser=m_wBankerUser;
      StatusScore.cbDynamicJoin=m_cbDynamicJoin[wChairID];
      CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(StatusScore.cbPlayStatus));
	  DWORD dwPassTime= (GetTickCount()-m_dwGameTime)/1000;
	  StatusScore.cbTimeLeave = 30-dwPassTime;
	   if(30 - dwPassTime <= 0)StatusScore.cbTimeLeave = 2;
      //设置积分
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(m_cbPlayStatus[i]==FALSE)
        {
          continue;
        }
		for(int j =0;j<MAX_COUNT - 1;j++)
		{
			StatusScore.cbHandCardData[i][j] = m_cbHandCardData[i][j];
		}
        StatusScore.lTableScore[i]=m_lTableScore[i];
		StatusScore.cbCallStatus[i] = m_cbCallStatus[i];
      }

      //房间名称
      CopyMemory(StatusScore.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusScore.szGameRoomName));

      //机器人配置
      tagRobotConfig RobotConfig;
      ZeroMemory(&RobotConfig, sizeof(RobotConfig));
      tagCustomRule* pCustomRule = (tagCustomRule*)(m_pGameServiceOption->cbCustomRule);
      RobotConfig.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      RobotConfig.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      RobotConfig.lRobotBankGet = pCustomRule->lRobotBankGet;
      RobotConfig.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      RobotConfig.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CopyMemory(&(StatusScore.RobotConfig), &RobotConfig, sizeof(RobotConfig));
      StatusScore.lStartStorage = g_lStorageStart;
      StatusScore.lBonus = m_lBonus;

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo,sizeof(StorageInfo));
        StorageInfo.lCurrentStorage = g_lStorageCurrent;
        StorageInfo.lCurrentDeduct = g_lStorageDeduct;
        StorageInfo.lMaxStorage = g_lStorageMax;
        StorageInfo.wStorageMul = g_wStorageMul;

        m_pITableFrame->SendTableData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
    }
    case GS_TK_PLAYING: //游戏状态
    {
      //构造数据
      CMD_S_StatusPlay StatusPlay;
      memset(&StatusPlay,0,sizeof(StatusPlay));

      //历史积分
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
        StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
        StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
      }
		StatusPlay.lCellScore = m_pGameServiceOption->lCellScore;
      //设置信息
      StatusPlay.lTurnMaxScore=GetUserMaxTurnScore(wChairID);
      StatusPlay.wBankerUser=m_wBankerUser;
      StatusPlay.cbDynamicJoin=m_cbDynamicJoin[wChairID];
      CopyMemory(StatusPlay.bOxCard,m_cbOxCard,sizeof(StatusPlay.bOxCard));
      CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));

      //房间名称
      CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));
	  DWORD dwPassTime= (GetTickCount()-m_dwGameTime)/1000;
	  StatusPlay.cbTimeLeave = 30-dwPassTime;
	  if(30 - dwPassTime <= 0)StatusPlay.cbTimeLeave = 2;
      //机器人配置
      tagRobotConfig RobotConfig;
      ZeroMemory(&RobotConfig, sizeof(RobotConfig));
      tagCustomRule* pCustomRule = (tagCustomRule*)(m_pGameServiceOption->cbCustomRule);
      RobotConfig.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      RobotConfig.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      RobotConfig.lRobotBankGet = pCustomRule->lRobotBankGet;
      RobotConfig.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      RobotConfig.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CopyMemory(&(StatusPlay.RobotConfig), &RobotConfig, sizeof(RobotConfig));
      StatusPlay.lStartStorage = g_lStorageStart;
      StatusPlay.lBonus = m_lBonus;

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo,sizeof(StorageInfo));
        StorageInfo.lCurrentStorage = g_lStorageCurrent;
        StorageInfo.lCurrentDeduct = g_lStorageDeduct;
        StorageInfo.lMaxStorage = g_lStorageMax;
        StorageInfo.wStorageMul = g_wStorageMul;

        m_pITableFrame->SendTableData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID,SUB_S_ADMIN_STORAGE_INFO,&StorageInfo,sizeof(StorageInfo));
      }

      //设置扑克
      for(WORD i=0; i< m_wPlayerCount; i++)
      {
        if(m_cbPlayStatus[i]==FALSE)
        {
          continue;
        }
        WORD j= i;
        StatusPlay.lTableScore[j]=m_lTableScore[j];
        CopyMemory(StatusPlay.cbHandCardData[j],m_cbHandCardData[j],MAX_COUNT);
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
    }
  }
  //效验错误
  ASSERT(FALSE);

  return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{

  switch(dwTimerID)
  {
  case IDI_USERUP:
	  {
		  for(WORD i=0; i<GAME_PLAYER; i++)
		  {

			  m_pITableFrame->KillGameTimer(IDI_USERUP);
			  //获取用户
			  IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			  if(!pIServerUserItem)continue;
			  if(!pIServerUserItem->IsAndroidUser())continue;
			  	printf("机器人退出状态——%d\n",pIServerUserItem->GetUserInfo()->cbUserStatus);
				pIServerUserItem->SetUserStatus(US_SIT,pIServerUserItem->GetTableID(),pIServerUserItem->GetChairID());
			  if(pIServerUserItem->GetUserInfo()->cbUserStatus < US_READY)
			  {
				  
				  bool bbb = m_pITableFrame->PerformStandUpAction(pIServerUserItem);
				  if (bbb)
				  {
					  printf("机器人退出成功\n");
				  }
			  }

		  }

		  return true;
	  }
    case IDI_DELAY_ENDGAME:
    {
      //检查机器人存储款
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        //获取用户
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
          continue;
        }
        if(!pIServerUserItem->IsAndroidUser())
        {
          continue;
        }
        m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
      }

      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
      m_pITableFrame->KillGameTimer(IDI_DELAY_ENDGAME);

      return true;
    }
    case IDI_SO_OPERATE:
    {
      //删除时间
      m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

      //游戏状态
      switch(m_pITableFrame->GetGameStatus())
      {
        case GS_TK_CALL:      //用户叫庄
        {
          //OnUserCallBanker(m_wCurrentUser, 0);
			for(WORD i=0; i<m_wPlayerCount; i++)
			{
				if(m_cbPlayStatus[i]==TRUE && m_cbCallStatus[i] == 0)
				{
					OnUserCallBanker(i, 5);
				}
			}
          break;
        }
        case GS_TK_SCORE:     //下注操作
        {
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_lTableScore[i]>0L || m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)
            {
              continue;
            }
			OnUserAddScore(i,m_pGameServiceOption->lCellScore);
            //if(m_lTurnMaxScore[i] > 0)
            //{
            //  OnUserAddScore(i,m_lTurnMaxScore[i]/8);
            //}
            //else
            //{
            //  OnUserAddScore(i,1);
            //}
          }

          break;
        }
        case GS_TK_PLAYING:     //用户开牌
        {
          for(WORD i=0; i<m_wPlayerCount; i++)
          {
            if(m_cbOxCard[i]<2 || m_cbPlayStatus[i]==FALSE)
            {
              continue;
            }
            OnUserOpenCard(i, (m_GameLogic.GetCardType(m_cbHandCardData[i],MAX_COUNT)>0)?TRUE:FALSE);
          }

          break;
        }
        default:
        {
          break;
        }
      }

      if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
      {
        m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
      }
      return true;
    }
  }
  return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  bool bResult=false;
  switch(wSubCmdID)
  {
  case SUB_S_ANDROIDQIANGZHIUP:
	  {
		  pIServerUserItem->SetUserStatus(US_SIT,pIServerUserItem->GetTableID(),pIServerUserItem->GetChairID());
		  m_pITableFrame->PerformStandUpAction(pIServerUserItem);
		  return true;
	  }
	case SUB_S_ANDROIDUP:
	{
		m_pITableFrame->PerformStandUpAction(pIServerUserItem);
		return true;
	}
	 case SUB_C_AMDIN_CHANGE_CARD:
	{
		CMD_C_Admin_ChangeCard* pChangeCard = (CMD_C_Admin_ChangeCard*)pDataBuffer;

		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
		{
			WORD wChairID = pIServerUserItem->GetChairID();
			for (WORD i=0;i<5;i++)
			{
				if (m_cbHandCardData[wChairID][i] == pChangeCard->cbStart)
				{
// 						printf("pChangeCard->cbStart=%d,pChangeCard->cbEnd=%d\n",m_GameLogic.GetCardValue(pChangeCard->cbStart),
// 							m_GameLogic.GetCardValue(pChangeCard->cbEnd));
					m_cbHandCardData[wChairID][i] = pChangeCard->cbEnd;

					CMD_S_Admin_ChangeCard Admin_ChangeCard;
					ZeroMemory(&Admin_ChangeCard,sizeof(Admin_ChangeCard));
					Admin_ChangeCard.dwCharID = wChairID;
					CopyMemory(Admin_ChangeCard.cbCardData,m_cbHandCardData[wChairID],sizeof(Admin_ChangeCard.cbCardData));
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADMIN_COLTERCARD,&Admin_ChangeCard,sizeof(Admin_ChangeCard));
					break;
				}
			}
		}
		return true;
	}
    case SUB_C_CALL_BANKER:     //用户叫庄
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_C_CallBanker));
      if(wDataSize!=sizeof(CMD_C_CallBanker))
      {
        return false;
      }

      //变量定义
      CMD_C_CallBanker * pCallBanker=(CMD_C_CallBanker *)pDataBuffer;

      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(IsUserPlaying(pUserData->wChairID));
      if(!IsUserPlaying(pUserData->wChairID))
      {
        return false;
      }

      //消息处理
      bResult=OnUserCallBanker(pUserData->wChairID,pCallBanker->bBanker);
      break;
    }
    case SUB_C_ADD_SCORE:     //用户加注
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_C_AddScore));
      if(wDataSize!=sizeof(CMD_C_AddScore))
      {
        return false;
      }

      //变量定义
      CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(IsUserPlaying(pUserData->wChairID));
      if(!IsUserPlaying(pUserData->wChairID))
      {
        return false;
      }

      //消息处理
      bResult=OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
      break;
    }
    case SUB_C_OPEN_CARD:     //用户摊牌
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_C_OxCard));
      if(wDataSize!=sizeof(CMD_C_OxCard))
      {
        return false;
      }

      //变量定义
      CMD_C_OxCard * pOxCard=(CMD_C_OxCard *)pDataBuffer;

      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]!=FALSE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      //消息处理
      bResult=OnUserOpenCard(pUserData->wChairID,pOxCard->bOX);
      break;
    }
    case SUB_C_SPECIAL_CLIENT_REPORT:  //特殊客户端
    {
      ////效验数据
      //ASSERT(wDataSize==sizeof(CMD_C_SPECIAL_CLIENT_REPORT));
      //if (wDataSize!=sizeof(CMD_C_SPECIAL_CLIENT_REPORT)) return false;

      ////变量定义
      //CMD_C_SPECIAL_CLIENT_REPORT * pSpecailClient=(CMD_C_SPECIAL_CLIENT_REPORT *)pDataBuffer;
      //if(pSpecailClient==NULL) return false;

      ////设置变量
      //m_bSpecialClient[pSpecailClient->wUserChairID]=true;

      return true;
    }
    case SUB_C_MODIFY_STORAGE:
    {
      ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
      if(wDataSize != sizeof(CMD_C_ModifyStorage))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
      {
        return false;
      }

      CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pDataBuffer;

      //全局变量
      g_lStorageCurrent = pModifyStorage->lStorageCurrent;
      g_lStorageDeduct = pModifyStorage->lStorageDeduct;
      g_lStorageMax = pModifyStorage->lMaxStorage;
      g_wStorageMul = pModifyStorage->wStorageMul;

      return true;
    }
    case SUB_C_REQUEST_ADD_USERROSTER:
    {
      //效验数据
      ASSERT(wDataSize == sizeof(CMD_C_Request_Add_Userroster));
      if(wDataSize != sizeof(CMD_C_Request_Add_Userroster))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
      {
        return false;
      }

      //变量定义
      CMD_C_Request_Add_Userroster *pUserroster = (CMD_C_Request_Add_Userroster*)pDataBuffer;

      //遍历映射
      POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO userinfo;

      while(ptHead)
      {
        ZeroMemory(&userinfo, sizeof(userinfo));
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        //查询用户
        if(userinfo.dwGameID == pUserroster->dwUserGameID)
        {
          //过滤重复名单
          USERROSTER roster = (pUserroster->bWin == true ? WHITE_ROSTER : BLACK_ROSTER);
          if(userinfo.userroster == roster)
          {
            CMD_S_DuplicateUserRoster DuplicateUserRoster;
            ZeroMemory(&DuplicateUserRoster, sizeof(DuplicateUserRoster));

            DuplicateUserRoster.dwGameID = userinfo.dwGameID;
            DuplicateUserRoster.userroster = roster;
            DuplicateUserRoster.lScoreLimit = userinfo.lScoreLimit;

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_DUPLICATE_USERROSTER, &DuplicateUserRoster, sizeof(DuplicateUserRoster));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_DUPLICATE_USERROSTER, &DuplicateUserRoster, sizeof(DuplicateUserRoster));

            return true;
          }

          ROOMUSERINFO keyInfo;
          ZeroMemory(&keyInfo, sizeof(keyInfo));
          keyInfo.dwGameID = userinfo.dwGameID;
          keyInfo.lGameScore = 0;
          keyInfo.lScoreLimit = pUserroster->lScoreLimit;
          CopyMemory(keyInfo.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
          keyInfo.userroster = (pUserroster->bWin == true ? WHITE_ROSTER : BLACK_ROSTER);
          g_MapRoomUserInfo.SetAt(dwUserID, keyInfo);

          CMD_S_AddUserRoster_Result Roster_Result;
          ZeroMemory(&Roster_Result, sizeof(Roster_Result));
          Roster_Result.dwGameID = keyInfo.dwGameID;
          Roster_Result.dwUserID = dwUserID;
          Roster_Result.bSucceed = true;
          Roster_Result.lGameScore = 0;
          Roster_Result.lScoreLimit = pUserroster->lScoreLimit;
          Roster_Result.userroster = keyInfo.userroster;
          CopyMemory(Roster_Result.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));

          //发送数据
          m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RESULT_ADD_USERROSTER, &Roster_Result, sizeof(Roster_Result));
          m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RESULT_ADD_USERROSTER, &Roster_Result, sizeof(Roster_Result));

          return true;
        }
      }

      CMD_S_AddUserRoster_Result Roster_Result;
      ZeroMemory(&Roster_Result, sizeof(Roster_Result));
      Roster_Result.dwGameID = pUserroster->dwUserGameID;
      Roster_Result.bSucceed = false;

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RESULT_ADD_USERROSTER, &Roster_Result, sizeof(Roster_Result));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RESULT_ADD_USERROSTER, &Roster_Result, sizeof(Roster_Result));

      return true;
    }
    case SUB_C_REQUEST_DELETE_USERROSTER:
    {
      //效验数据
      ASSERT(wDataSize == sizeof(CMD_C_Request_Delete_Userroster));
      if(wDataSize != sizeof(CMD_C_Request_Delete_Userroster))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
      {
        return false;
      }

      //变量定义
      CMD_C_Request_Delete_Userroster *pUserroster = (CMD_C_Request_Delete_Userroster*)pDataBuffer;

      //遍历映射
      POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO userinfo;

      while(ptHead)
      {
        ZeroMemory(&userinfo, sizeof(userinfo));
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        //查询用户
        if(userinfo.dwGameID == pUserroster->dwUserGameID)
        {
          //之前存在黑白名单标识重设无效名单
          if(userinfo.userroster == WHITE_ROSTER || userinfo.userroster == BLACK_ROSTER)
          {
            ROOMUSERINFO keyInfo;
            ZeroMemory(&keyInfo, sizeof(keyInfo));
            keyInfo.dwGameID = userinfo.dwGameID;
            keyInfo.lGameScore = 0;
            keyInfo.lScoreLimit = 0;
            CopyMemory(keyInfo.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
            keyInfo.userroster = INVALID_ROSTER;
            g_MapRoomUserInfo.SetAt(dwUserID, keyInfo);

            CMD_S_DeleteUserRoster_Result Roster_Result;
            ZeroMemory(&Roster_Result, sizeof(Roster_Result));
            Roster_Result.dwGameID = userinfo.dwGameID;
            Roster_Result.dwUserID = dwUserID;
            Roster_Result.bFind = true;
            Roster_Result.bExistRoster = true;
            Roster_Result.beforeuserroster = userinfo.userroster;

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));

            return true;
          }

          CMD_S_DeleteUserRoster_Result Roster_Result;
          ZeroMemory(&Roster_Result, sizeof(Roster_Result));
          Roster_Result.dwGameID = userinfo.dwGameID;
          Roster_Result.bFind = true;
          Roster_Result.bExistRoster = false;
          Roster_Result.beforeuserroster = userinfo.userroster;

          //发送数据
          m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));
          m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));

          return true;

        }
      }

      CMD_S_DeleteUserRoster_Result Roster_Result;
      ZeroMemory(&Roster_Result, sizeof(Roster_Result));
      Roster_Result.dwGameID = pUserroster->dwUserGameID;
      Roster_Result.bFind = false;
      Roster_Result.bExistRoster = false;
      Roster_Result.beforeuserroster = INVALID_ROSTER;

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_RESULT_DELETE_USERROSTER, &Roster_Result, sizeof(Roster_Result));

      return true;
    }
    case SUB_C_REQUEST_UPDATE_USERROSTER:
    {
      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
      {
        return false;
      }

      //遍历映射
      POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO KeyUserInfo;
      ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));

      CMD_S_UpdateUserRoster UpdateUserRoster;

      while(ptHead)
      {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

        //过滤无效名单用户
        if(KeyUserInfo.userroster != INVALID_ROSTER)
        {
          ZeroMemory(&UpdateUserRoster, sizeof(UpdateUserRoster));
          CopyMemory(&(UpdateUserRoster.roomUserInfo), &KeyUserInfo, sizeof(KeyUserInfo));
          UpdateUserRoster.dwUserID = dwUserID;

          //发送数据
          m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_USERROSTER, &UpdateUserRoster, sizeof(UpdateUserRoster));
          m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_USERROSTER, &UpdateUserRoster, sizeof(UpdateUserRoster));
        }
      }

      return true;
    }
  }

  //操作定时器
  if(bResult)
  {
    //m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
    return true;
  }

  return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  return false;
}


//叫庄事件
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, BYTE bBanker)
{
  //状态效验
  ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_CALL);
  if(m_pITableFrame->GetGameStatus()!=GS_TK_CALL)
  {
    return true;
  }
  //ASSERT(m_wCurrentUser==wChairID);
  //if(m_wCurrentUser!=wChairID)
  //{
  //  return true;
  //}
  if (bBanker > ADD_MAX_TIMES || bBanker < 0)
  {
	  //庄家积分
	  IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	  //输出信息
	  TCHAR szBuffer[128]=TEXT("");
	  _sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserCallBanker 玩家ID [%d]"),pUserItem->GetGameID());
	  //输出信息
	  CTraceService::TraceString(szBuffer,TraceLevel_Normal);
	  return false;
  }
	if(m_cbCallStatus[wChairID] != 0)return true;
  //设置变量
  m_cbCallStatus[wChairID]=bBanker;
  //叫庄人数
  WORD wCallUserCount=0;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_cbPlayStatus[i]==TRUE && m_cbCallStatus[i] != 0)
    {
      wCallUserCount++;
    }
    else if(m_cbPlayStatus[i]!=TRUE)
    {
      wCallUserCount++;
    }
  }
	CMD_S_RopBanker RopBanker;
	ZeroMemory(&RopBanker,sizeof(RopBanker));
	RopBanker.wCallBanker = wChairID;
	RopBanker.cbmultiple = bBanker;
	//发送数据
	for(WORD i=0; i<m_wPlayerCount; i++)
	{
		if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
			continue;

		m_pITableFrame->SendTableData(i,SUB_S_ROPBANKER,&RopBanker,sizeof(RopBanker));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ROPBANKER,&RopBanker,sizeof(RopBanker));
  //下注开始
  if(wCallUserCount==m_wPlayerCount)
  {
		//取出最大的倍数
	    DWORD deCharID = INVALID_DWORD;
		//最大的倍数
		int iTimes = 0;
		//最大的人数
		int nIndex = 0;
		//最多倍数的玩家
		int iMaxTimes[GAME_PLAYER];			//存储抢庄倍数最大的玩家
		int iMaxTimesbak[GAME_PLAYER];			//存储抢庄倍数最大的玩家
		for(int i =0;i<GAME_PLAYER;i++)
			iMaxTimes[i] = -1;
		for(int i =0;i<GAME_PLAYER;i++)
			iMaxTimesbak[i] = -1;
		for(int i =0;i <m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=TRUE)continue;
			if(m_cbCallStatus[i] > iTimes && m_cbCallStatus[i] < 5)    //状态五为不叫
			{
				iTimes = m_cbCallStatus[i];
				deCharID = i;
			}
		}
		WORD     wFisrtChooseCallUser;            //庄家用户
		//说明有人加倍
		if(iTimes != 0)
		{
			for(int i =0;i <m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=TRUE)continue;
				if(m_cbCallStatus[i] == iTimes && m_cbCallStatus[i] < 5)    //状态五为不叫
				{
					iMaxTimes[nIndex] = i;
					iMaxTimesbak[i] = i;
					nIndex++;
				}
			}
			if(nIndex == 1)
			{
				m_wBankerUser = deCharID;
				wFisrtChooseCallUser = deCharID;
			}
			else
			{
				m_wBankerUser = iMaxTimes[rand()%nIndex];
				wFisrtChooseCallUser = iMaxTimes[rand()%nIndex];
			}
		}
		else     //说明没有人叫庄，就随机
		{
			//最多倍数的玩家
			WORD iUserCharid[GAME_PLAYER];			//存储抢庄倍数最大的玩家
			ZeroMemory(iUserCharid,sizeof(iUserCharid));
			int  nUsernum = 0;
			for(int i =0;i <m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=TRUE)continue;
				iUserCharid[nUsernum] = i;
				m_cbCallStatus[i]=1;
				nUsernum++;
			}
			m_wBankerUser = iUserCharid[rand()%nUsernum];
			wFisrtChooseCallUser = iUserCharid[rand()%nUsernum];
		}

		m_wCurrentUser=INVALID_CHAIR;


		//m_bBuckleServiceCharge[m_wBankerUser]=true;
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_SCORE);

		//庄家积分
	    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		SCORE lBankerScore=pIServerUserItem->GetUserScore();

		////最大下注
		//for(WORD i=0; i<m_wPlayerCount; i++)
		//{
		//  if(m_cbPlayStatus[i]!=TRUE || i==m_wBankerUser)
		//  {
		//    continue;
		//  }

		//  //下注变量 客户要求
		//  m_lTurnMaxScore[i]=GetUserMaxTurnScore(i);
		//}

		//设置变量
		CMD_S_GameStart GameStart;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.lTurnMaxScore=lBankerScore;
		GameStart.wFisrtChooseCallUser = wFisrtChooseCallUser;
		GameStart.lCellScore = m_pGameServiceOption->lCellScore;
		GameStart.wRandNumber = rand()%2 + 2;
		for(int i =0;i<GAME_PLAYER;i++)
			GameStart.iMaxTimes[i] = iMaxTimesbak[i];
		CopyMemory(GameStart.cbPlayerStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
		//发送数据
		for(WORD i=0; i<m_wPlayerCount; i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
			{
				continue;
			}
			GameStart.lTurnMaxScore=m_lTurnMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));


		m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
		m_dwGameTime=GetTickCount();
  }
  //else     //用户叫庄
  //{
  //  //设置变量
  //  CMD_S_CallBanker CallBanker;
  //  CallBanker.wCallBanker=m_wCurrentUser;
  //  CallBanker.bFirstTimes=false;

  //  //发送数据
  //  for(WORD i=0; i<m_wPlayerCount; i++)
  //  {
  //    if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
  //    {
  //      continue;
  //    }
  //    m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
  //  }
  //  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
  //}
  return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, SCORE lScore)
{
  //状态效验
  ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
  if(m_pITableFrame->GetGameStatus()!=GS_TK_SCORE)
  {
    return true;
  }
	if(m_lTableScore[wChairID] > 0)return true;

	if (lScore >  (m_pGameServiceOption->lCellScore * 4) || lScore < 0.001)
	{
		//庄家积分
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		//输出信息
		TCHAR szBuffer[128]=TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserAddScore 玩家ID [%d]"),pUserItem->GetGameID());
		//输出信息
		CTraceService::TraceString(szBuffer,TraceLevel_Normal);
		return false;
	}
  //金币效验
  if(m_cbPlayStatus[wChairID]==TRUE)
  {
    //ASSERT(lScore>0 && lScore<=m_lTurnMaxScore[wChairID]);
    //if(lScore<=0 || lScore>m_lTurnMaxScore[wChairID])
    //{
    //  return false;
    //}
  }
  else //没下注玩家强退
  {
    ASSERT(lScore==0);
    if(lScore!=0)
    {
      return false;
    }
  }

  if(!UserCanAddScore(wChairID,lScore))
  {
	  //庄家积分
	  IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	  //输出信息
	  TCHAR szBuffer[128]=TEXT("");
	  _sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserAddScore_2 玩家ID [%d]"),pUserItem->GetGameID());
	  //输出信息
	  CTraceService::TraceString(szBuffer,TraceLevel_Normal);
	  if(pUserItem->IsAndroidUser())
	  {
		  _sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserAddScore_2 玩家ID [%d]"),pUserItem->GetGameID());
		  //输出信息
		  CTraceService::TraceString(szBuffer,TraceLevel_Normal);

		  lScore = m_pGameServiceOption->lCellScore;
	  }
	  else
	  {
		return false;
	  }
  }

  if(lScore>0L)
  {
    //下注金币
    m_lTableScore[wChairID]=lScore;
   // m_bBuckleServiceCharge[wChairID]=true;
    //构造数据
    CMD_S_AddScore AddScore;
    AddScore.wAddScoreUser=wChairID;
    AddScore.lAddScoreCount=m_lTableScore[wChairID];

    //发送数据
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
      {
        continue;
      }
      m_pITableFrame->SendTableData(i,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
  }

  //下注人数
  BYTE bUserCount=0;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_lTableScore[i]>0L && m_cbPlayStatus[i]==TRUE)
    {
      bUserCount++;
    }
    else if(m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)
    {
      bUserCount++;
    }
  }

  //闲家全到
  if(bUserCount==m_wPlayerCount)
  {
    //设置状态
    m_pITableFrame->SetGameStatus(GS_TK_PLAYING);

    //构造数据
    CMD_S_SendCard SendCard;
    ZeroMemory(SendCard.cbCardData,sizeof(SendCard.cbCardData));

    BYTE cbWhiteRosterCount = 0;
    BYTE cbBlackRosterCount = 0;
    BYTE cbInvalidRosterCount = 0;
    BYTE cbAndroidCount = 0;

    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

      if(!pIServerUserItem)
      {
        continue;
      }

      if(pIServerUserItem->IsAndroidUser())
      {
        cbAndroidCount++;
      }

      //查找已存在的用户
      ROOMUSERINFO lastroomUserInfo;
      ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
      if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo) == TRUE)
      {
        if(lastroomUserInfo.userroster == WHITE_ROSTER)
        {
          cbWhiteRosterCount++;
        }
        else if(lastroomUserInfo.userroster == BLACK_ROSTER)
        {
          cbBlackRosterCount++;
        }
        else if(lastroomUserInfo.userroster == INVALID_ROSTER)
        {
          cbInvalidRosterCount++;
        }
      }


    }

    ////都为机器人或者无效名单
    //if(cbAndroidCount == GAME_PLAYER || cbInvalidRosterCount == GAME_PLAYER
    //   || (cbInvalidRosterCount != 0 && cbWhiteRosterCount == 0 && cbBlackRosterCount == 0) || cbWhiteRosterCount == GAME_PLAYER || cbBlackRosterCount == GAME_PLAYER)
    //{
    //  //分析扑克
    // // AnalyseCard();
    //}
    //else if(cbWhiteRosterCount != 0 || cbBlackRosterCount != 0)
    //{
    //  USERROSTER userroster[GAME_PLAYER] = { INVALID_ROSTER, INVALID_ROSTER, INVALID_ROSTER, INVALID_ROSTER};

    //  //更新用户名单
    //  for(WORD i=0; i<GAME_PLAYER; i++)
    //  {
    //    //获取用户
    //    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

    //    if(!pIServerUserItem)
    //    {
    //      continue;

    //    }

    //    //查找已存在的用户
    //    ROOMUSERINFO lastroomUserInfo;
    //    ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
    //    if(g_MapRoomUserInfo.Lookup(pIServerUserItem->GetUserID(), lastroomUserInfo))
    //    {
    //      userroster[i] = lastroomUserInfo.userroster;
    //    }
    //  }

    //  m_pServerContro->ControlResult(m_cbHandCardData, userroster);
    //}
	printf("玩家进入控制\n");
		if(m_bIsGameRate == false)
		{
			AndroidHuanPai();
		}
    //发送扑克
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      if(m_cbPlayStatus[i]==FALSE)
      {
        continue;
      }

      //派发扑克
      CopyMemory(SendCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
    }

    //发送数据
    for(WORD i=0; i< m_wPlayerCount; i++)
    {
      if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
      {
        continue;
      }

      m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
	m_dwGameTime=GetTickCount();

	//随机扑克
	CMD_S_AllCard AllCard;
	ZeroMemory(&AllCard,sizeof(AllCard));

	WORD wIndex = 0;
	for(WORD i=0; i<m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser==NULL)continue;
		if(m_cbPlayStatus[i]==FALSE)continue;


		CopyMemory(AllCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
		wIndex++;
	}

	//随机扑克
	BYTE bTempArray[52];
	m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray));
	for (int i = 0 ;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser==NULL)continue;
		if(m_cbPlayStatus[i]==FALSE)continue;

		for (int j = 0;j < MAX_COUNT;j++)
		{
			for (int m = 0;m < 52;m++)
			{
				if (m_cbHandCardData[i][j] == bTempArray[m])
				{
					bTempArray[m] = 0x00;
					break;
				}
			}
		}
	}
	BYTE bTempArray2[52] = {0};
	int nIndexcount = 0;
	for (int m = 0;m < 52;m++)
	{
		if (bTempArray[m] != 0x00)
		{
			bTempArray2[nIndexcount] = bTempArray[m];
			nIndexcount++;
		}
	}
  //发送数据
  for (WORD i=0;i<m_wPlayerCount;i++)
  {
	 if(m_cbPlayStatus[i]!=true)continue;
    IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUser==NULL)continue;

	CopyMemory(AllCard.cbSurplusCardData,bTempArray2,sizeof(AllCard.cbSurplusCardData));
	AllCard.cbSurplusCardCount = 52-wIndex*MAX_COUNT;
//#ifndef _DEBUG
    if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false /*|| m_bSpecialClient[i]==false*/)continue;
//#endif
    m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
  }

  }

  return true;
}

//摊牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE bOx)
{
  //状态效验
  ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_PLAYING);
  if(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)
  {
    return true;
  }
  if(m_cbOxCard[wChairID]!=0xff)
  {
    return true;
  }

  //效验数据
  ASSERT(bOx<=TRUE);
  if(bOx>TRUE)
  {
    return false;
  }

  //效验数据
  if(bOx)
  {
    ASSERT(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0);
    if(!(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0))
    {
      return false;
    }
  }
  else if(m_cbPlayStatus[wChairID]==TRUE)
  {
	  if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_FOURKING)
	  {
		  bOx=TRUE;
	  }
  }
  //else if(m_cbPlayStatus[wChairID]==TRUE)
  //{
  //  if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_THREE_SAME)
  //  {
  //    bOx=TRUE;
  //  }
  //}

  //牛牛数据
  m_cbOxCard[wChairID] = bOx;

  //摊牌人数
  BYTE bUserCount=0;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_cbOxCard[i]<2 && m_cbPlayStatus[i]==TRUE)
    {
      bUserCount++;
    }
    else if(m_cbPlayStatus[i]==FALSE)
    {
      bUserCount++;
    }
  }

  //构造变量
  CMD_S_Open_Card OpenCard;
  ZeroMemory(&OpenCard,sizeof(OpenCard));

  //设置变量
  OpenCard.bOpen=bOx;
  OpenCard.wPlayerID=wChairID;

  //发送数据
  for(WORD i=0; i< m_wPlayerCount; i++)
  {
    if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
    {
      continue;
    }
    m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
  }
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));

  //结束游戏
  if(bUserCount == m_wPlayerCount)
  {
    return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
  }

  return true;
}

////扑克分析
//void CTableFrameSink::AnalyseCard()
//{
//  //机器人数
//  bool bIsAiBanker = false;
//  WORD wAiCount = 0;
//  WORD wPlayerCount = 0;
//  for(WORD i=0; i<m_wPlayerCount; i++)
//  {
//    //获取用户
//    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
//    if(pIServerUserItem!=NULL)
//    {
//      if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
//      {
//        continue;
//      }
//      if(pIServerUserItem->IsAndroidUser())
//      {
//        wAiCount++ ;
//        if(!bIsAiBanker && i==m_wBankerUser)
//        {
//          bIsAiBanker = true;
//        }
//      }
//      wPlayerCount++;
//    }
//  }
//
//  //全部机器
//  if(wPlayerCount == wAiCount || wAiCount==0)
//  {
//    return;
//  }
//
//  //变量定义
//  WORD wWinUser = INVALID_CHAIR;
//
//  //计算机器人分数
//  SCORE lAndroidScore = CalculateAndroidScore(bIsAiBanker, wWinUser);
//
//  //赔付比例
//  double dclaimraido = 0.3;
//
//  // 送分判断
//  if(g_lStorageCurrent > 0 && g_lStorageCurrent > g_lStorageMax && rand()%100 < g_wStorageMul)
//  {
//    //获取用户
//    IServerUserItem * pIWinUserUserItem = m_pITableFrame->GetTableUserItem(wWinUser);
//
//    //非机器人赢
//    if(pIWinUserUserItem != NULL && pIWinUserUserItem->IsAndroidUser())
//    {
//      //随机赢家
//      WORD wWinPlay = INVALID_CHAIR;
//
//      INT nCirculateCount = 0;
//
//      while(true)
//      {
//        do
//        {
//          wWinPlay = rand()%GAME_PLAYER;
//        }
//        while(m_pITableFrame->GetTableUserItem(wWinPlay) == NULL || m_pITableFrame->GetTableUserItem(wWinPlay)->IsAndroidUser());
//
//        //交换数据
//        BYTE cbTempData[MAX_COUNT];
//        CopyMemory(cbTempData, m_cbHandCardData[wWinPlay], MAX_COUNT*sizeof(BYTE));
//        CopyMemory(m_cbHandCardData[wWinPlay], m_cbHandCardData[wWinUser], MAX_COUNT*sizeof(BYTE));
//        CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_COUNT*sizeof(BYTE));
//
//        lAndroidScore = CalculateAndroidScore(bIsAiBanker, wWinUser);
//        nCirculateCount++;
//
//        if(g_lStorageCurrent + lAndroidScore > g_lStorageCurrent * dclaimraido)
//        {
//          break;
//        }
//
//        if(nCirculateCount >= GAME_PLAYER)
//        {
//          //不满足条件把最大的牌赋值给任意一个机器人
//          WORD wRandAndroid = INVALID_CHAIR;
//          do
//          {
//            wRandAndroid = rand()%GAME_PLAYER;
//          }
//          while(m_pITableFrame->GetTableUserItem(wRandAndroid) == NULL || !(m_pITableFrame->GetTableUserItem(wRandAndroid)->IsAndroidUser()));
//
//          //交换数据
//          BYTE cbTempData[MAX_COUNT];
//          CopyMemory(cbTempData, m_cbHandCardData[wRandAndroid], MAX_COUNT*sizeof(BYTE));
//          CopyMemory(m_cbHandCardData[wRandAndroid], m_cbHandCardData[wWinUser], MAX_COUNT*sizeof(BYTE));
//          CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_COUNT*sizeof(BYTE));
//
//          break;
//        }
//      }
//    }
//    else
//    {
//      goto LABEL;
//    }
//
//    return;
//  }
//
//LABEL:
//  //判断库存
//  if(g_lStorageCurrent + lAndroidScore <= 0)
//  {
//    //获取用户
//    IServerUserItem * pIWinUserUserItem = m_pITableFrame->GetTableUserItem(wWinUser);
//
//    //非机器人赢
//    if(pIWinUserUserItem != NULL && !pIWinUserUserItem->IsAndroidUser())
//    {
//      //随机赢家
//      WORD wWinAndroid = INVALID_CHAIR;
//
//      do
//      {
//        wWinAndroid = rand()%GAME_PLAYER;
//      }
//      while(m_pITableFrame->GetTableUserItem(wWinAndroid) == NULL || !m_pITableFrame->GetTableUserItem(wWinAndroid)->IsAndroidUser());
//
//      //交换数据
//      BYTE cbTempData[MAX_COUNT];
//      CopyMemory(cbTempData, m_cbHandCardData[wWinAndroid], MAX_COUNT*sizeof(BYTE));
//      CopyMemory(m_cbHandCardData[wWinAndroid], m_cbHandCardData[wWinUser], MAX_COUNT*sizeof(BYTE));
//      CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_COUNT*sizeof(BYTE));
//    }
//  }
//
//  return;
//}



//判断库存
bool CTableFrameSink::JudgeStock()
{

  //机器人数
  bool bIsAiBanker = false;
  WORD wAiCount = 0;
  WORD wPlayerCount = 0;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    //获取用户
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem!=NULL)
    {
      if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)
      {
        continue;
      }
      if(pIServerUserItem->IsAndroidUser())
      {
        wAiCount++ ;
        if(!bIsAiBanker && i==m_wBankerUser)
        {
          bIsAiBanker = true;
        }
      }
      wPlayerCount++;
    }
  }

  //扑克变量
  BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
  CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

  //牛牛数据
  BOOL bUserOxData[GAME_PLAYER];
  ZeroMemory(bUserOxData,sizeof(bUserOxData));
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(m_cbPlayStatus[i]==FALSE)
    {
      continue;
    }
    bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
  }

  //排列扑克
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
  }

  //变量定义
  SCORE lAndroidScore=0;

  //倍数变量
  BYTE cbCardTimes[GAME_PLAYER];
  ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

  //查找倍数
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_cbPlayStatus[i]==TRUE)
    {
      cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
    }
  }

  //机器庄家
  if(bIsAiBanker)
  {
    //对比扑克
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      //用户过滤
      if((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE))
      {
        continue;
      }

      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //机器过滤
      if((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //对比扑克
      if(m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
      {
        lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
      }
      else
      {
        lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
      }
    }
  }
  else//用户庄家
  {
    //对比扑克
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //用户过滤
      if((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //对比扑克
      if(m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
      {
        lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
      }
      else
      {
        lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
      }
    }
  }

  //变量定义
  WORD wMaxUser=INVALID_CHAIR;
  WORD wMinAndroid=INVALID_CHAIR;
  WORD wMaxAndroid=INVALID_CHAIR;

  //查找特殊玩家
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    //真人玩家
    if(pIServerUserItem->IsAndroidUser()==false)
    {
      //初始设置
      if(wMaxUser==INVALID_CHAIR)
      {
        wMaxUser=i;
      }

      //获取较大者
      if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxUser],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxUser])==true)
      {
        wMaxUser=i;
      }
    }

    //机器玩家
    if(pIServerUserItem->IsAndroidUser()==true)
    {
      //初始设置
      if(wMinAndroid==INVALID_CHAIR)
      {
        wMinAndroid=i;
      }
      if(wMaxAndroid==INVALID_CHAIR)
      {
        wMaxAndroid=i;
      }

      //获取较小者
      if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMinAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMinAndroid])==false)
      {
        wMinAndroid=i;
      }

      //获取较大者
      if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxAndroid])==true)
      {
        wMaxAndroid=i;
      }
    }
  }

  return g_lStorageCurrent+lAndroidScore>0;

}

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pUserItem==NULL)
    {
      continue;
    }

    if(m_bBuckleServiceCharge[i]&&i==wChairID)
    {
      return true;
    }

  }
  return false;
}


bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
  //修改积分
  tagScoreInfo ScoreInfo[GAME_PLAYER];
  ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
  memcpy(&ScoreInfo,ScoreInfoArray,sizeof(ScoreInfo));
  //记录异常
  SCORE beforeScore[GAME_PLAYER];
  ZeroMemory(beforeScore,sizeof(beforeScore));
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
    if(pItem!=NULL)
    {
      beforeScore[i]=pItem->GetUserScore();

    }
  }
  m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
  SCORE afterScore[GAME_PLAYER];
  ZeroMemory(afterScore,sizeof(afterScore));
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
    if(pItem!=NULL)
    {
      afterScore[i]=pItem->GetUserScore();

      if(afterScore[i]<0)
      {
        //异常写入日志

        CString strInfo;
        strInfo.Format(TEXT("[%s] 出现负分"),pItem->GetNickName());
        NcaTextOut(strInfo);

        strInfo.Format(TEXT("写分前分数：%.2lf"),beforeScore[i]);
        NcaTextOut(strInfo);

        strInfo.Format(TEXT("写分信息：写入积分 %.2lf，税收 %.2lf"),ScoreInfoArray[i].lScore,ScoreInfoArray[i].lRevenue);
        NcaTextOut(strInfo);

        strInfo.Format(TEXT("写分后分数：%.2lf"),afterScore[i]);
        NcaTextOut(strInfo);

      }

    }
  }
  return true;
}

//最大下分
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{

  SCORE lMaxTurnScore=0L;
  if(wChairID==m_wBankerUser)
  {
    return 0;
  }
  //庄家积分
  IServerUserItem *pIBankerItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
  SCORE lBankerScore=0L;
  if(pIBankerItem!=NULL)
  {
    lBankerScore=pIBankerItem->GetUserScore();
  }

  //玩家人数
  WORD wUserCount=0;
  for(WORD i=0; i<m_wPlayerCount; i++)
    if(m_cbPlayStatus[i]==TRUE)
    {
      wUserCount++;
    }

  //获取用户
  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

  if(pIServerUserItem!=NULL)
  {
    //获取积分
    SCORE lScore=pIServerUserItem->GetUserScore();

    lMaxTurnScore=__min(lBankerScore/(wUserCount-1)/MAX_TIMES/4,lScore/MAX_TIMES/4);
  }

  return lMaxTurnScore;


}

//是否可加
bool CTableFrameSink::UserCanAddScore(WORD wChairID, SCORE lAddScore)
{

  //获取用户
  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

  if(pIServerUserItem!=NULL)
  {
    //获取积分
    SCORE lScore=pIServerUserItem->GetUserScore();

    if(lAddScore > lScore/MAX_TIMES/m_cbCallStatus[m_wBankerUser])
    {
      return false;
    }
  }

  WORD wPlayerCount = 0;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
	  //获取用户
	  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
	  if(pIServerUserItem!=NULL)
	  {
		  if(m_cbPlayStatus[i]==FALSE)
			  continue;
		  wPlayerCount++;
	  }
  }

  //获取用户
  IServerUserItem * pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);

  if(pIBankerServerUserItem!=NULL)
  {
	  //获取积分
	  SCORE lScore=pIBankerServerUserItem->GetUserScore();

	  if(lAddScore > lScore/MAX_TIMES/m_cbCallStatus[m_wBankerUser]/(wPlayerCount-1))
	  {
		  return false;
	  }

  }
  return true;

}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
  SCORE consumeQuota=0L;
  /*SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
  if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE)
  {
    consumeQuota=pIServerUserItem->GetUserScore()-lMinTableScore;

  }*/
  return consumeQuota;
}

//是否衰减
bool CTableFrameSink::NeedDeductStorage()
{
  for(int i = 0; i < GAME_PLAYER; ++i)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem == NULL)
    {
      continue;
    }

    if(!pIServerUserItem->IsAndroidUser())
    {
      return true;
    }
  }

  return false;

}

//更新房间用户信息
void CTableFrameSink::UpdateUserRosterInfo(IServerUserItem *pIServerUserItem, SCORE lGameScore, USERROSTER userRoster, SCORE lScoreLimit)
{
  //变量定义
  ROOMUSERINFO roomUserInfo;
  ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

  roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
  CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
  roomUserInfo.userroster = userRoster;
  roomUserInfo.lGameScore = lGameScore;
  roomUserInfo.lScoreLimit = lScoreLimit;

  g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);
}

//解除用户名单
void CTableFrameSink::RemoveUserRoster()
{
  //遍历映射
  POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
  DWORD dwUserID = 0;
  ROOMUSERINFO userinfo;

  while(ptHead)
  {
    ZeroMemory(&userinfo, sizeof(userinfo));
    g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

    //白名单用户
    if((userinfo.userroster == WHITE_ROSTER && userinfo.lGameScore > userinfo.lScoreLimit)
       || (userinfo.userroster == BLACK_ROSTER && -userinfo.lGameScore > userinfo.lScoreLimit))
    {
      ROOMUSERINFO keyInfo;
      ZeroMemory(&keyInfo, sizeof(keyInfo));
      keyInfo.dwGameID = userinfo.dwGameID;
      keyInfo.lGameScore = 0;
      keyInfo.lScoreLimit = 0;
      CopyMemory(keyInfo.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
      keyInfo.userroster = INVALID_ROSTER;
      g_MapRoomUserInfo.SetAt(dwUserID, keyInfo);

      CMD_S_RemoveKeyUserRoster RemoveKeyUserRoster;
      ZeroMemory(&RemoveKeyUserRoster, sizeof(RemoveKeyUserRoster));
      RemoveKeyUserRoster.dwUserID = dwUserID;
      RemoveKeyUserRoster.dwGameID = userinfo.dwGameID;
      RemoveKeyUserRoster.userroster = userinfo.userroster;
      RemoveKeyUserRoster.lScoreLimit = userinfo.lScoreLimit;

      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
          continue;
        }

        if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false
           || pIServerUserItem->IsAndroidUser() == true)
        {
          continue;
        }

        m_pITableFrame->SendTableData(i, SUB_S_REMOVEKEY_USERROSTER, &RemoveKeyUserRoster, sizeof(RemoveKeyUserRoster));
        m_pITableFrame->SendLookonData(i, SUB_S_REMOVEKEY_USERROSTER, &RemoveKeyUserRoster, sizeof(RemoveKeyUserRoster));
      }
    }

  }
}

//计算机器人分数
SCORE CTableFrameSink::CalculateAndroidScore(bool bIsAiBanker, WORD &wWinUser)
{
  //扑克变量
  BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
  CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

  //牛牛数据
  BOOL bUserOxData[GAME_PLAYER];
  ZeroMemory(bUserOxData,sizeof(bUserOxData));
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(m_cbPlayStatus[i]==FALSE)
    {
      continue;
    }
    bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
  }

  //排列扑克
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
  }

  //变量定义
  SCORE lAndroidScore=0;

  //倍数变量
  BYTE cbCardTimes[GAME_PLAYER];
  ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

  //查找倍数
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    if(m_cbPlayStatus[i]==TRUE)
    {
      cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
    }
  }

  //机器庄家
  if(bIsAiBanker)
  {
    //对比扑克
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      //用户过滤
      if((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE))
      {
        continue;
      }

      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //机器过滤
      if((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //对比扑克
      if(m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
      {
        lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
      }
      else
      {
        lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
      }
    }
  }
  else//用户庄家
  {
    //对比扑克
    for(WORD i=0; i<m_wPlayerCount; i++)
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //用户过滤
      if((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //对比扑克
      if(m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
      {
        lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
      }
      else
      {
        lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
      }
    }
  }

  //查找最大玩家
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    //用户过滤
    if(m_cbPlayStatus[i]==FALSE)
    {
      continue;
    }

    //设置用户
    if(wWinUser==INVALID_CHAIR)
    {
      wWinUser=i;
      continue;
    }

    //对比扑克
    if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[i],bUserOxData[wWinUser])==true)
    {
      wWinUser=i;
    }
  }

  return lAndroidScore;
}

//试探性判断, 确保Chair的用户可以赢
void CTableFrameSink::ProbeJudge(WORD wChairIDCtrl,BYTE cbCheatType)
{	
	//获取用户

	IServerUserItem * pIServerCtrlUserItem=m_pITableFrame->GetTableUserItem(wChairIDCtrl);
	//待用户不存在, 不需要控制

	if(pIServerCtrlUserItem==NULL) return;

	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));
	//排列扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//牛牛数据
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i] ==false)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}


	IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(m_wBankerUser);

	//WORD wChairID=0;
	int iMaxIndex=0;
	int iLostIndex=0;
	//iMaxIndex = EstimateWinner(0,4); //获取最大用户
	//iLostIndex = EstimateLose(0,4);	//获取最小用户
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==false)continue;
		iMaxIndex=i;
		iLostIndex=i;
		break;
	}
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=true ||   iMaxIndex ==i )continue;

		if (m_GameLogic.CompareCard(cbUserCardData[iMaxIndex],cbUserCardData[i],MAX_COUNT,bUserOxData[iMaxIndex],bUserOxData[i])==false)
		{
			iMaxIndex = i;
		}
	}
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=true ||   iLostIndex ==i )continue;

		if (m_GameLogic.CompareCard(cbUserCardData[iLostIndex],cbUserCardData[i],MAX_COUNT,bUserOxData[iLostIndex],bUserOxData[i])==true)
		{
			iLostIndex = i;
		}
	}
	//iMaxIndex = EstimateWinner(0,4); //获取最大用户
	//iLostIndex = EstimateLose(0,4);	//获取最小用户

	//临时变量
	BYTE cbTemp[MAX_COUNT]={0};
	//交换牌
	WORD wWinerUser=iMaxIndex;     //赢

	if(cbCheatType==CHEAT_TYPE_LOST) wWinerUser=iLostIndex;   //输

	if (wWinerUser!=wChairIDCtrl) //如果当前用户的牌不是要换的牌的用户就开始换牌
	{

		//交牌扑克
		CopyMemory(cbTemp,m_cbHandCardData[wWinerUser],MAX_COUNT);   //把指定用户的牌放到变量
		CopyMemory(m_cbHandCardData[wWinerUser],m_cbHandCardData[wChairIDCtrl],MAX_COUNT); //把控制用户的牌与指定用户的牌交换
		CopyMemory(m_cbHandCardData[wChairIDCtrl],cbTemp,MAX_COUNT);  //最后交换

	}
	return;
}
void CTableFrameSink::AndroidHuanPai()
{
	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetModuleFileName( NULL,szPath,CountArray(szPath));

	CString szTemp;
	szTemp = szPath;
	int nPos = szTemp.ReverseFind(TEXT('\\'));
	_sntprintf(szPath, CountArray(szPath), TEXT("%s"), szTemp.Left(nPos));

	TCHAR szFileNameG[MAX_PATH]=TEXT("");
	_sntprintf(szFileNameG,sizeof(szFileNameG),TEXT("%s\\GameControl\\global.ini"),szPath);

	TCHAR szGameRoomName[32]=TEXT("");
	_sntprintf(szGameRoomName, sizeof(szGameRoomName), _T("%d"),m_pGameServiceOption->wServerID);

	int iJiLv = 0;
	int nIsOpen = false;
	iJiLv = GetPrivateProfileInt( szGameRoomName, TEXT("WinRate"),0, szFileNameG);

	printf("机器人控制参数为%d\n",iJiLv);
	nIsOpen = GetPrivateProfileInt( szGameRoomName, TEXT("IsOpen"),0, szFileNameG);
	if(nIsOpen == false)return;
	bool bIsWin = false;
	if (iJiLv >= rand()%100) bIsWin = true;
	else bIsWin = false;

	for(int i =0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(pServerUserItem == NULL)continue;
		if(pServerUserItem->IsAndroidUser())
		{
			if(bIsWin)
			{
				BeginChange(true);

			}
			else
			{
				BeginChange(false);
			}
			break; 
		}

	}
}
void CTableFrameSink::BeginChange(bool IsLoss )
{
	IServerUserItem * pIServerCtrlUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
	//待用户不存在, 不需要控制

	if(pIServerCtrlUserItem!=NULL && pIServerCtrlUserItem->IsAndroidUser())
	{
		for(int i =0 ;i< m_cbCardIndex;i++)
		{
			BYTE  cbIndex[GAME_PLAYER] = {0xff};
			if(m_cbExchangeCard[i] != 0)
			{
				BYTE cbItem = m_cbHandCardData[m_wBankerUser][4];
				m_cbHandCardData[m_wBankerUser][4] = m_cbExchangeCard[i];
				m_cbExchangeCard[i] = cbItem;

				cbIndex[m_wBankerUser] = i;
				for(int j =0;j<GAME_PLAYER;j++)
				{
					if(j == m_wBankerUser )continue;
					IServerUserItem * pIServerItem=m_pITableFrame->GetTableUserItem(j);
					if(pIServerItem == NULL)continue;
					if(pIServerItem->IsAndroidUser())continue;
					for(int k = 0;k < m_cbCardIndex;k++)
					{
						bool bOK = false;
						for(int m =0;m<GAME_PLAYER;m++)
						{
							if(k == cbIndex[m])
							{
								bOK = true;
								break;
							}
						}
						if(bOK)continue;
						m_WinnerScore = 0;
						cbIndex[j] = k; 

						BYTE cbItem = m_cbHandCardData[j][4];

						m_cbHandCardData[j][4] = m_cbExchangeCard[k];
						m_cbExchangeCard[k] = cbItem;

						//分析输
						AnalyseCard();
						if(IsLoss ==false)
						{
							if(m_WinnerScore<0)
							{
								return;
							}
						}
						else
						{
							if(m_WinnerScore>0)
							{
								bOK = true;
								return;
							}
						}

						for(int z =0;z<GAME_PLAYER;z++)
						{
							if(z == m_wBankerUser )continue;
							if(z == j)continue;
							IServerUserItem * pIServerItem=m_pITableFrame->GetTableUserItem(z);
							if(pIServerItem == NULL)continue;
							if(pIServerItem->IsAndroidUser())continue;
							for(int k = 0;k < m_cbCardIndex;k++)
							{
								bool bOK = false;
								for(int m =0;m<GAME_PLAYER;m++)
								{
									if(k == cbIndex[m])
									{
										bOK = true;
										break;
									}
								}
								if(bOK)continue;
								m_WinnerScore = 0;
								cbIndex[z] = k;

								BYTE cbItem = m_cbHandCardData[z][4];

								m_cbHandCardData[z][4] = m_cbExchangeCard[k];
								m_cbExchangeCard[k] = cbItem;
								//分析输赢
								AnalyseCard();
								if(IsLoss ==false)
								{
									if(m_WinnerScore<0)
									{
										return;
									}
								}
								else
								{
									if(m_WinnerScore>0)
									{
										bOK = true;
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if(pIServerCtrlUserItem!=NULL && !pIServerCtrlUserItem->IsAndroidUser())
	{
		for(int i = 0;i< m_cbCardIndex;i++)
		{
			BYTE  cbIndex[GAME_PLAYER] = {0xff};
			if(m_cbExchangeCard[i] != 0)
			{
				BYTE cbItem = m_cbHandCardData[m_wBankerUser][4];
				m_cbHandCardData[m_wBankerUser][4] = m_cbExchangeCard[i];
				 m_cbExchangeCard[i] = cbItem;
				cbIndex[m_wBankerUser] = i;
				for(int j =0;j<GAME_PLAYER;j++)
				{
					if(j == m_wBankerUser )continue;
					IServerUserItem * pIServerItem=m_pITableFrame->GetTableUserItem(j);
					if(pIServerItem == NULL)continue;
					if(!pIServerItem->IsAndroidUser())continue;
					for(int k = 0;k < m_cbCardIndex;k++)
					{
						bool bOK = false;
						for(int m =0;m<GAME_PLAYER;m++)
						{
							if(k == cbIndex[m])
							{
								bOK = true;
								break;
							}
						}
						if(bOK)continue;
						m_WinnerScore = 0;
						cbIndex[j] = k;
						BYTE cbItem = m_cbHandCardData[j][4];
						m_cbHandCardData[j][4] = m_cbExchangeCard[k];
						m_cbExchangeCard[k] = cbItem;
						//分析输赢
						AnalyseCard();
						if(IsLoss ==false)
						{
							if(m_WinnerScore<0)
							{
								return;
							}
						}
						else
						{
							if(m_WinnerScore>0)
							{
								bOK = true;
								return;
							}
						}

						for(int z =0;z<GAME_PLAYER;z++)
						{
							if(z == m_wBankerUser )continue;
							if(z == j)continue;
							IServerUserItem * pIServerItem=m_pITableFrame->GetTableUserItem(z);
							if(pIServerItem == NULL)continue;
							if(!pIServerItem->IsAndroidUser())continue;
							for(int k = 0;k < m_cbCardIndex;k++)
							{
								bool bOK = false;
								for(int m =0;m<GAME_PLAYER;m++)
								{
									if(k == cbIndex[m])
									{
										bOK = true;
										break;
									}
								}
								if(bOK)continue;
								m_WinnerScore = 0;
								cbIndex[z] = k;
								BYTE cbItem = m_cbHandCardData[z][4];
								m_cbHandCardData[z][4] = m_cbExchangeCard[k];
								m_cbExchangeCard[k] = cbItem;
								//分析输赢
								AnalyseCard();
								if(IsLoss ==false)
								{
									if(m_WinnerScore<0)
									{
										return;
									}
								}
								else
								{
									if(m_WinnerScore>0)
									{
										bOK = true;
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
//扑克分析
void CTableFrameSink::AnalyseCard()
{
	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//牛牛数据
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=true)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//排列扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//变量定义
	m_WinnerScore=0;

	//倍数变量
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//查找倍数
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==true)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	IServerUserItem *pServerBankerUser = m_pITableFrame->GetTableUserItem(m_wBankerUser);
	//机器庄家
	if(pServerBankerUser->IsAndroidUser())
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//用户过滤
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]!=true)) continue;

			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//机器过滤
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				m_WinnerScore-=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				m_WinnerScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	else//用户庄家
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//用户过滤
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				m_WinnerScore+=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				m_WinnerScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
