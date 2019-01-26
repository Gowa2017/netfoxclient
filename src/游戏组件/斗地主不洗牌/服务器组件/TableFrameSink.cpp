#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
#define IDI_CHECK_TABLE				1					//检查桌子
#define IDI_USER_OUT_TIME			6					//玩家超时
#define BTRUSTEE_BUFFER_TIME		0					//托管缓冲时间
#define BTRUSTEE_OUTCARD_TIME		3					//托管时间
#define ROBOT_GRAB_LORD_RATE		60					//机器人抢地主的概率
#define MULTIPLE_TIME				5					//加倍时间

//动作标识
#define IDI_DELAY_ENDGAME     10        //动作标识
#define IDI_DELAY_TIME        3000      //延时时间

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_bOffLineTrustee = false;

	//炸弹变量
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//加倍
	ZeroMemory(m_multipleUser,sizeof(m_multipleUser));

	//游戏变量
	m_wTimerControl=0;
	m_cbBombCount=0;
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));

	//火箭变量
	m_cbRocket = 0;

	// 倍数
	m_lMultiple = 1;

	//叫分信息
	m_cbBankerScore=0;
	m_bUserGrab = false;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	m_wLastOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	// 时间变量
	m_dwStartTime = 0;

	//扑克信息
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//服务控制
	m_hControlInst = NULL;
	m_pServerControl = NULL;
	m_hControlInst = LoadLibrary(TEXT("LandServerControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{

	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_cbBombCount=0;
	m_cbRocket = 0;
	m_lMultiple = 1;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
	ZeroMemory(m_cbEachBombCount,sizeof(m_cbEachBombCount));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//加倍信息
	ZeroMemory(m_multipleUser,sizeof(m_multipleUser));

	//叫分信息
	m_cbBankerScore=0;
	m_bUserGrab = false;
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	m_cbCallScoreCount=0;

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	m_wLastOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//时间信息
	m_dwStartTime = 0;

	//扑克信息
	ZeroMemory(m_cbBankerCard,sizeof(m_cbBankerCard));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_bOffLineTrustee = CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);

	return true;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//最少积分
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbScoreInfo,sizeof(m_cbScoreInfo));
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_CALL);

	//混乱扑克
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard,sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));
// 
 #ifdef _DEBUG

	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D, 方块
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, 梅花
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, 红桃
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, 黑桃

 	//BYTE cbTempRandCard[FULL_COUNT]=
 	//{
 	//	0x11,0x11,0x1D,0x0C,0x0B,0x0A,0x0A,0x09,0x07,0x07,0x07,0x07,0x06,0x06,0x06,0x05,0x05,
		//0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
		//0x29,0x2A,0x2B,0x2C,0x2D,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,
		//0x05,0x05,0x04,
 	//};
 
 	//CopyMemory(cbRandCard, cbTempRandCard, sizeof(cbRandCard));
 #endif
	
	//比赛房间随机第一个随机叫分
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 )
	{
		m_wFirstUser = INVALID_CHAIR;
	}
	//抽取明牌
	BYTE cbValidCardData=0;
	BYTE cbValidCardIndex=0;
	WORD wStartUser=m_wFirstUser;
	WORD wCurrentUser=m_wFirstUser;

	//抽取玩家
	if (wStartUser==INVALID_CHAIR)
	{
		//抽取扑克
		cbValidCardIndex=rand()%DISPATCH_COUNT;
		cbValidCardData=cbRandCard[cbValidCardIndex];

		//设置用户
		wStartUser=m_GameLogic.GetCardValue(cbValidCardData)%GAME_PLAYER;
		wCurrentUser=(wStartUser+cbValidCardIndex/NORMAL_COUNT)%GAME_PLAYER;
	}

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserIndex=(wStartUser+i)%GAME_PLAYER;
		m_cbHandCardCount[wUserIndex]=NORMAL_COUNT;
		CopyMemory(&m_cbHandCardData[wUserIndex],&cbRandCard[i*m_cbHandCardCount[wUserIndex]],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
	}

	//设置底牌
	CopyMemory(m_cbBankerCard,&cbRandCard[DISPATCH_COUNT],sizeof(m_cbBankerCard));
	//机器人数据
	CMD_S_AndroidCard AndroidCard ;
	ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserIndex = (m_wFirstUser+i)%GAME_PLAYER;
		m_cbHandCardCount[wUserIndex]=NORMAL_COUNT;
		CopyMemory(&m_cbHandCardData[wUserIndex],&cbRandCard[i*m_cbHandCardCount[wUserIndex]],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
		CopyMemory(&AndroidCard.cbHandCard[wUserIndex], &cbRandCard[i*m_cbHandCardCount[wUserIndex]], sizeof(BYTE)*m_cbHandCardCount[wUserIndex]) ;
	}

	//设置用户
	m_wFirstUser=wCurrentUser;
	m_wCurrentUser=wCurrentUser;

	//构造变量
	CMD_S_GameStart GameStart;
	GameStart.wStartUser=wStartUser;
	GameStart.wCurrentUser=wCurrentUser;
	GameStart.cbValidCardData=cbValidCardData;
	GameStart.cbValidCardIndex=cbValidCardIndex;
	GameStart.cbTimeCallScore = m_pGameCustomRule->cbTimeCallScore;

	AndroidCard.wCurrentUser = m_wCurrentUser;
	CopyMemory(AndroidCard.cbBankerCard,m_cbBankerCard,3*sizeof(BYTE));

	//构造其他玩家的数据

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送数据
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(pServerUserItem != NULL && !pServerUserItem->IsAndroidUser())
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) {
				CMD_S_Other_CardData OtherCardData;
				ZeroMemory(&OtherCardData, sizeof(OtherCardData));
				for (WORD j=0; j<GAME_PLAYER; j++) {
					if (i != j) {
						CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
					}
				}
				m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
				m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
			}
		}
		else 
		{
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&AndroidCard,sizeof(AndroidCard));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
	}
	//WB改
	//SendCheatCard();

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}
	
	//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	m_dwStartTime=GetTickCount();
	// 第一个叫分的多3S钟缓冲时间
	DWORD dwTimerTime=m_pGameCustomRule->cbTimeCallScore + 3;
	m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			//设置变量
			GameConclude.cbBankerScore=m_cbBankerScore;
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			//炸弹信息
			GameConclude.cbBombCount=m_cbBombCount;
			GameConclude.lBombMultiple=1L;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//用户扑克
			BYTE cbCardIndex=0;
			BYTE landLoseMultiple = 2;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_multipleUser[i] == 2) landLoseMultiple = landLoseMultiple + 1;
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//炸弹统计
			LONG lScoreTimes=1L * m_cbBankerScore;
			for (BYTE i=0;i<m_cbBombCount;i++) {
				lScoreTimes*=2L;
				GameConclude.lBombMultiple *=2L;
			}

			//火箭
			if (m_cbRocket == 1)
			{
				GameConclude.lRocketMultiple=2L;
				lScoreTimes*=2L;
			}

			//春天判断
			if (wChairID==m_wBankerUser)
			{
				//用户定义
				WORD wUser1=(m_wBankerUser+1)%GAME_PLAYER;
				WORD wUser2=(m_wBankerUser+2)%GAME_PLAYER;

				//用户判断
				if ((m_cbOutCardCount[wUser1]==0)&&(m_cbOutCardCount[wUser2]==0)) 
				{
					lScoreTimes*=2L;
					GameConclude.bChunTian=TRUE;
					GameConclude.lChunTianMultiple=2L;
				}
			}

			//春天判断
			if (wChairID!=m_wBankerUser)
			{
				if (m_cbOutCardCount[m_wBankerUser]==1)
				{
					lScoreTimes*=2L;
					GameConclude.bFanChunTian=TRUE;
					GameConclude.lFanChunTianMultiple=2L;
				}
			}

			//总倍数
			GameConclude.lAllMultiple=lScoreTimes;
			GameConclude.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			//调整倍数
			//lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			//农民托管数量
			WORD wTrusteeCount=0;
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				for(WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (i!=m_wBankerUser)
					{
						if (m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser()==true) wTrusteeCount++;
					}
				}
			}

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			SCORE lCellScore=m_pITableFrame->GetCellScore();

			SCORE ShowScoreArray[GAME_PLAYER];
			ZeroMemory(ShowScoreArray,sizeof(ShowScoreArray));
			

			SCORE lMaxScoreLand=0L;
			// 地主输赢
			SCORE lUserScore = landLoseMultiple*lCellScore*lScoreTimes;
			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
			if (pServerUserItem != NULL)
			{
				SCORE lScore = pServerUserItem->GetUserInfo()->lScore;
				lMaxScoreLand = __min(lScore, lUserScore);
			}
			SCORE lMaxScoreFarm=0L;
			SCORE FarmScore[3]= {0};
			//BYTE isNeedFarm = true;
			// 农民输赢
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (i==m_wBankerUser)
					continue;
				SCORE lUserScore = lCellScore*lScoreTimes;
				BYTE tempMultiple = 1L;
				if (m_multipleUser[i] == 2) 
				{
					tempMultiple = 2;
					lUserScore *= 2L;
				}
				IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem != NULL)
				{
					SCORE lScore = pServerUserItem->GetUserInfo()->lScore;
					lScore = __min(lScore, lUserScore);
					lScore = __min(lScore, lMaxScoreLand/landLoseMultiple*tempMultiple);
					FarmScore[i] = lScore;
					lMaxScoreFarm += lScore;
				}
			}
			//if (lMaxScoreLand <= lMaxScoreFarm) isNeedFarm = false;
			lMaxScoreLand = __min(lMaxScoreLand, lMaxScoreFarm);
			//SCORE lMaxScore = __min(lMaxScoreLand, lMaxScoreFarm);
			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				SCORE lUserScore=0L;
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				bool bTrusteeUser= pIServerUserItem->IsTrusteeUser();
				BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
				if (i==m_wBankerUser)
				{
					ScoreInfoArray[i].lScore = lMaxScoreLand;
					if (m_cbHandCardCount[m_wBankerUser] != 0)
						ScoreInfoArray[i].lScore = -1L * lMaxScoreLand;
				}
				else
				{
					ScoreInfoArray[i].lScore = FarmScore[i];
					if (m_cbHandCardCount[m_wBankerUser] == 0)
						ScoreInfoArray[i].lScore = -1L * FarmScore[i];
				}
				// 保护机制 赢局金币不大于游戏携带金币值
				//if (ScoreInfoArray[i].lScore > 0)
				//{
				//	if (pIServerUserItem->GetUserInfo()->lScore < ScoreInfoArray[i].lScore)
				//	{
				//		ShowScoreArray[i] = ScoreInfoArray[i].lScore;
				//		ScoreInfoArray[i].lScore = pIServerUserItem->GetUserInfo()->lScore;
				//	}
				//} else {
				//	ScoreInfoArray[i].lScore = __max(ScoreInfoArray[i].lScore, -1L*pIServerUserItem->GetUserInfo()->lScore);
				//}
				ScoreInfoArray[i].cbType=(ScoreInfoArray[i].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				//计算税收
				if (ScoreInfoArray[i].lScore > 0)
				{
					ScoreInfoArray[i].lRevenue=m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) 
						ScoreInfoArray[i].lScore-=ScoreInfoArray[i].lRevenue;
				}
				//if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore-=ScoreInfoArray[i].lRevenue;
				//if (ShowScoreArray[i] > 0)
				//{
				//	SCORE lRevnueTemp = m_pITableFrame->CalculateRevenue(i,ShowScoreArray[i]);
				//	if (lRevnueTemp>0L) ShowScoreArray[i]-=lRevnueTemp;
				//	//设置积分
				//	GameConclude.lGameScore[i]=ShowScoreArray[i];
				//}
				//else
				//{
				//	GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;
				//}
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;


				//历史积分
				//m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);

				// 离线玩家起立离开
				if (cbUserStatus == US_OFFLINE) {
					//WORD wTableID=pIServerUserItem->GetTableID();
					//CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
					//if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
				}
			}
			CopyMemory(GameConclude.bMultiple,m_multipleUser,sizeof(GameConclude.bMultiple));
			for (BYTE j=0; j<GAME_PLAYER; j++)
			{
				//for (BYTE z=0;z<GAME_PLAYER; z++)
				//{
				//	if (ShowScoreArray[z] > 0)
				//	{
				//		if (z==j)
				//		{
				//			GameConclude.lGameScore[z]=ScoreInfoArray[z].lScore;
				//			GameConclude.bSpecialTip = true;
				//		}
				//		else
				//		{
				//			GameConclude.lGameScore[z] = ShowScoreArray[z];
				//		}
				//	}
				//}
				if (i==m_wBankerUser)
				{
					if (m_cbHandCardCount[m_wBankerUser] == 0 && lMaxScoreLand > lMaxScoreFarm)
						GameConclude.bSpecialTip = true;
				}
				else
					if (m_cbHandCardCount[m_wBankerUser] != 0 && lMaxScoreLand < lMaxScoreFarm)
						GameConclude.bSpecialTip = true;
				//发送数据
				m_pITableFrame->SendTableData(j,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
				m_pITableFrame->SendLookonData(j,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			}


			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//切换用户
			m_wFirstUser=wChairID;
			m_pITableFrame->SetGameTimer(IDI_DELAY_ENDGAME,IDI_DELAY_TIME,1,0L);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//用户强退

		//自动托管
		OnUserTrustee(wChairID,true);

		return true;
	case GER_NETWORK_ERROR:	//网络中断
		{
			//if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			//{
			//	if(m_bOffLineTrustee)
			//	{
			//		IsOfflineTrustee();

			//		return true;
			//	}
			//}
			if (m_bOffLineTrustee) {
				OnUserTrustee(wChairID,true);
			}
			//if (m_bTrustee[m_wCurrentUser] == true) {
			//	IsOfflineTrustee();
			//}

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude,sizeof(GameConclude));

			//设置变量
			if (m_wBankerUser == wChairID)
			{
				BYTE multipleNum = 1L;
				for (WORD k=0; k<GAME_PLAYER; k++)
				{
					if (m_multipleUser[k] == 2) multipleNum *= 2L;
				}
				GameConclude.cbBankerScore=m_cbBankerScore * multipleNum;

			}
			else
			{
				if (m_multipleUser[wChairID] == 2) GameConclude.cbBankerScore = m_cbBankerScore * 2;
				else GameConclude.cbBankerScore = m_cbBankerScore;
			}
			GameConclude.lCellScore=m_pITableFrame->GetCellScore();

			//炸弹信息
			GameConclude.cbBombCount=m_cbBombCount;
			CopyMemory(GameConclude.cbEachBombCount,m_cbEachBombCount,sizeof(GameConclude.cbEachBombCount));

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//拷贝扑克
				GameConclude.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameConclude.cbHandCardData[cbCardIndex],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));

				//设置索引
				cbCardIndex+=m_cbHandCardCount[i];
			}

			//炸弹统计
			WORD lScoreTimes=1;
			for (WORD i=0;i<m_cbBombCount;i++) lScoreTimes*=2L;

			//调整倍数
			lScoreTimes=__min(m_pGameCustomRule->wMaxScoreTimes,lScoreTimes);

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//变量定义
			SCORE lCellScore=m_pITableFrame->GetCellScore();
			SCORE lUserScore=lCellScore*__max(lScoreTimes,m_pGameCustomRule->wFleeScoreTimes);

			//金币平衡
			if ((m_pGameServiceOption->wServerType&SCORE_GENRE_POSITIVE)!=0 || (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE))!=0)
			{
				lUserScore=__min(pIServerUserItem->GetUserScore(),lUserScore);
			}

			//逃跑罚分
			ScoreInfoArray[wChairID].lScore=-lUserScore;
			ScoreInfoArray[wChairID].cbType=SCORE_TYPE_FLEE;

			//分享罚分
			if (m_pGameCustomRule->cbFleeScorePatch==TRUE)
			{
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					//过滤
					if ( i == wChairID )
						continue;

					//设置积分
					ScoreInfoArray[i].lScore=lUserScore/2L;
					ScoreInfoArray[i].cbType=SCORE_TYPE_WIN;

					//计算税收
					ScoreInfoArray[i].lRevenue=m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore-=ScoreInfoArray[i].lRevenue;
				}
			}

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置成绩
				GameConclude.lGameScore[i]=ScoreInfoArray[i].lScore;

				//历史成绩
				m_HistoryScore.OnEventUserScore(i,GameConclude.lGameScore[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE,&GameConclude,sizeof(GameConclude));

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=m_pITableFrame->GetCellScore();

			//自定规则
			StatusFree.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusFree.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_CALL:	//叫分状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//单元积分
			StatusCall.lCellScore=m_pITableFrame->GetCellScore();
			DWORD curTime =GetTickCount();
			//自定规则
			StatusCall.cbTimeOutCard=m_pGameCustomRule->cbTimeOutCard;
			StatusCall.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore - (BYTE)((curTime - m_dwStartTime)/1000L);
			if ((BYTE)((curTime - m_dwStartTime)/1000L) >= m_pGameCustomRule->cbTimeCallScore) {
				StatusCall.cbTimeCallScore = 2;
			}
			StatusCall.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusCall.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;


			//游戏信息
			StatusCall.wCurrentUser=m_wCurrentUser;
			if (m_wBankerUser == wChairID)
			{
				BYTE multipleNum = 1L;
				for (WORD k=0; k<GAME_PLAYER; k++)
				{
					if (m_multipleUser[k] == 2) multipleNum *= 2L;
				}
				StatusCall.cbBankerScore=m_cbBankerScore * multipleNum;
			}
			else
			{
				if (m_multipleUser[wChairID] == 2) StatusCall.cbBankerScore = m_cbBankerScore * 2;
				else StatusCall.cbBankerScore = m_cbBankerScore;
			}
			CopyMemory(StatusCall.cbScoreInfo,m_cbScoreInfo,sizeof(m_cbScoreInfo));
			CopyMemory(StatusCall.cbHandCardData,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID]*sizeof(BYTE));
			CopyMemory(StatusCall.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
				IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem == NULL) continue;
				if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) {
					CMD_S_Other_CardData OtherCardData;
					ZeroMemory(&OtherCardData, sizeof(OtherCardData));
					for (WORD j=0; j<GAME_PLAYER; j++) {
						if (i != j) {
							CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
						}
					}
					m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
					m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
				}
			}

			//发送场景
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));

			if(pIServerUserItem)
			{
				//作弊用户
				if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
				{
					//OnCheatCard(pIServerUserItem);
				}
			}
			return bSendResult;
		}
	case GAME_SCENE_MULTIPLE:	//加倍状态
		{
			//构造数据
			CMD_S_StatusMultiple StatusMultiple;
			ZeroMemory(&StatusMultiple,sizeof(StatusMultiple));

			//单元积分
			StatusMultiple.lCellScore=m_pITableFrame->GetCellScore();

			//游戏变量
			StatusMultiple.wBankerUser=m_wBankerUser;
			if (m_wBankerUser == wChairID)
			{
				BYTE multipleNum = 1L;
				for (WORD k=0; k<GAME_PLAYER; k++)
				{
					if (m_multipleUser[k] == 2) multipleNum *= 2L;
				}
				StatusMultiple.cbBankerScore=m_cbBankerScore * multipleNum;

			}
			else
			{
				if (m_multipleUser[wChairID] == 2) StatusMultiple.cbBankerScore = m_cbBankerScore * 2;
				else StatusMultiple.cbBankerScore = m_cbBankerScore;
			}



			DWORD curTime =GetTickCount();

			//自定规则
			DWORD dwTimerTime=MULTIPLE_TIME;
			StatusMultiple.cbMultiple=dwTimerTime - (BYTE)((curTime - m_dwStartTime)/1000L);

			StatusMultiple.lEnterScore = m_pGameServiceOption->lMinEnterScore;

			//扑克信息
			CopyMemory(StatusMultiple.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
			CopyMemory(StatusMultiple.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusMultiple.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusMultiple.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusMultiple.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusMultiple.lCollectScore[i]=pHistoryScore->lCollectScore;
				IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem == NULL) continue;
				if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) {
					CMD_S_Other_CardData OtherCardData;
					ZeroMemory(&OtherCardData, sizeof(OtherCardData));
					for (WORD j=0; j<GAME_PLAYER; j++) {
						if (i != j) {
							CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
						}
					}
					m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
					m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
				}
			}

			//发送场景
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusMultiple,sizeof(StatusMultiple));

			if(pIServerUserItem)
			{
				//作弊用户
				if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
				{
					//OnCheatCard(pIServerUserItem);
				}
			}
			return bSendResult;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//单元积分
			StatusPlay.lCellScore=m_pITableFrame->GetCellScore();

			//游戏变量
			StatusPlay.cbBombCount=m_cbBombCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			if (m_wBankerUser == wChairID)
			{
				BYTE multipleNum = 1L;
				for (WORD k=0; k<GAME_PLAYER; k++)
				{
					if (m_multipleUser[k] == 2) multipleNum *= 2L;
				}
				StatusPlay.cbBankerScore=m_cbBankerScore * multipleNum;

			}
			else
			{
				if (m_multipleUser[wChairID] == 2) StatusPlay.cbBankerScore = m_cbBankerScore * 2;
				else StatusPlay.cbBankerScore = m_cbBankerScore;
			}
			
			DWORD curTime =GetTickCount();

			//自定规则
			DWORD dwTimerTime=BTRUSTEE_OUTCARD_TIME;
			if (m_bTrustee[wChairID] != true) {
				dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
				if (m_wLastOutCardUser == INVALID_CHAIR)
					dwTimerTime = m_pGameCustomRule->cbTimeHeadOutCard;
			}
			StatusPlay.cbTimeOutCard=dwTimerTime - (BYTE)((curTime - m_dwStartTime)/1000L);
			if ((BYTE)((curTime - m_dwStartTime)/1000L) >= dwTimerTime) {
				StatusPlay.cbTimeOutCard = 2;
			}
			StatusPlay.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;
			StatusPlay.cbTimeStartGame=m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeHeadOutCard=m_pGameCustomRule->cbTimeHeadOutCard;

			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克信息
			CopyMemory(StatusPlay.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
				IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem == NULL) continue;
				if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) {
					CMD_S_Other_CardData OtherCardData;
					ZeroMemory(&OtherCardData, sizeof(OtherCardData));
					for (WORD j=0; j<GAME_PLAYER; j++) {
						if (i != j) {
							CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
						}
					}
					m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
					m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
				}
			}

			//发送场景
			bool bSendResult = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			if(pIServerUserItem)
			{
				//作弊用户
				if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
				{
					//OnCheatCard(pIServerUserItem);
				}
			}
			return bSendResult;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

////时间事件
//bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
//
//{
//	//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
//	//{
//		if(wTimerID==IDI_USER_OUT_TIME)
//		{
//			m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
//			if (m_wCurrentUser==INVALID_CHAIR) return true;
//			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
//			ASSERT(pServerUserItem!=NULL);
//			if (pServerUserItem==NULL) return false;
//			bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
//			BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
//			switch(cbGameStatus)
//			{
//			case GAME_SCENE_CALL:
//				{
//					if(m_cbBankerScore==0){
//						OnUserCallScore(m_wCurrentUser,0x01);
//					}
//					else{
//						OnUserCallScore(m_wCurrentUser,0xFF);
//					}
//					break;
//				}
//			case GAME_SCENE_PLAY:
//				{
//					if(m_cbTurnCardCount == 0)
//					{
//						tagSearchCardResult SearchCardResult;
//						m_GameLogic.SearchOutCard( m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],NULL,0,&SearchCardResult);
//						if(SearchCardResult.cbCardCount > 0)
//						{
//							OnUserOutCard(m_wCurrentUser, SearchCardResult.cbResultCard[0], SearchCardResult.cbCardCount[0]);
//						}
//						else
//						{
//							ASSERT(FALSE);
//						}
//
//					}
//					else
//					{
//						OnUserPassCard(m_wCurrentUser);
//					}					
//					break;
//				}
//			}
//			return true;
//		}
//	// }	
//
//	return false;
//}
//

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)

{
	if(wTimerID==IDI_USER_OUT_TIME)
	{
		TCHAR szMessage[128];

		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);


		m_dwStartTime=0;
		if (m_wCurrentUser==INVALID_CHAIR) return true;


		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		ASSERT(pServerUserItem!=NULL);


		if (pServerUserItem==NULL) return false;
		//bool bTrusteeUser=pServerUserItem->IsTrusteeUser();

		OnUserTrustee(m_wCurrentUser, true);

		return true;
	}
	if(wTimerID==IDI_DELAY_ENDGAME)
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

		//return true;
	}
	return false;
}


//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//m_cbWaitTime=0;
	switch (wSubCmdID)
	{
	case SUB_S_ANDROIDUP:
		{
			m_pITableFrame->PerformStandUpAction(pIServerUserItem);
			return true;
		}
	case SUB_C_CALL_SCORE:	//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallScore));
			if (wDataSize!=sizeof(CMD_C_CallScore)) return false;

			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_CALL);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_CALL) return true;

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_C_CallScore * pCallScore=(CMD_C_CallScore *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserCallScore(wChairID,pCallScore->cbCallScore);
		}
	case SUB_C_OUT_CARD:	//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:	//用户放弃
		{
			//状态效验
			//ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;
			//用户效验
			//ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Trustee));
			if (wDataSize!=sizeof(CMD_C_Trustee)) return false;
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_MULTIPLE:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Multiple));
			if (wDataSize!=sizeof(CMD_C_Multiple)) return false;
			CMD_C_Multiple *pMultiple =(CMD_C_Multiple *)pData;
			if (pMultiple == NULL) 
			{
				return false;
			}
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_MULTIPLE)
			{
				return true;
			}
			return OnUserMultiple(pIServerUserItem->GetChairID(), pMultiple->bIsMultiple);
		}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(m_bOffLineTrustee && wChairID == m_wCurrentUser)
	{
		OnUserTrustee(m_wCurrentUser, true);
	}
	return true;
}

//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//if(m_bOffLineTrustee)
	//{
	//	if (((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)&&(pIServerUserItem->GetChairID()==m_wCurrentUser))
	//	{
	//		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
	//	}
	//}
	//return true; 
	OnUserTrustee(wChairID,false);//重入取消托管
	return true;
}

// 托管逻辑

bool CTableFrameSink::TrusteeOperate() 
{
	switch(m_pITableFrame->GetGameStatus())
	{
	case GAME_SCENE_CALL:
		{
			OnUserCallScore(m_wCurrentUser,0xFF);
			break;
		}
	case GAME_SCENE_PLAY:
		{
			// 最后一个是自己出的话，要继续出牌，否则就过
			if(m_cbTurnCardCount==0 || m_wLastOutCardUser == m_wCurrentUser)
			{
				tagSearchCardResult SearchCardResult;
				m_GameLogic.SearchOutCard( m_cbHandCardData[m_wCurrentUser],m_cbHandCardCount[m_wCurrentUser],NULL,0,&SearchCardResult);
				if(SearchCardResult.cbSearchCount > 0)
				{
					BYTE cbCardData[MAX_COUNT];
					BYTE cbCardCount;
					if (m_cbOutCardCount[m_wCurrentUser]==0) 
					{
						BYTE index = 0;
						while (index<SearchCardResult.cbSearchCount)
						{
							CopyMemory(&cbCardData,SearchCardResult.cbResultCard[index],sizeof(SearchCardResult.cbResultCard[index]));
							cbCardCount = SearchCardResult.cbCardCount[index];
							BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
							if (cbCardType == CT_BOMB_CARD)
							{
								index++;
							} else {
								break;
							}
						}
					} else {
						CopyMemory(&cbCardData,SearchCardResult.cbResultCard[0],sizeof(SearchCardResult.cbResultCard[0]));
						cbCardCount = SearchCardResult.cbCardCount[0];
					}
					OnUserOutCard(m_wCurrentUser, cbCardData, cbCardCount);
				}
				else
				{
					ASSERT(FALSE);
				}

			}
			else
			{
				OnUserPassCard(m_wCurrentUser);
			}					
			break;
		}
	}
	return true;
}

//用户加倍
bool CTableFrameSink::OnUserMultiple(WORD wChairID, bool bIsMultiple)
{
		//效验状态
	ASSERT((wChairID < GAME_PLAYER));
	if ((wChairID>=GAME_PLAYER)) return false;
	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pServerUserItem == NULL)
		return false;
	SCORE lScore = pServerUserItem->GetUserInfo()->lScore;
	if (bIsMultiple && lScore < m_pGameServiceOption->lMinEnterScore * 50)
	{
		return false;
	}
	if (m_wBankerUser == wChairID)
		return true;
	BYTE isMultiple = 1;
	if (bIsMultiple) isMultiple = 2;
	m_multipleUser[wChairID] = isMultiple;
	BYTE cbBankerScore = m_cbBankerScore;
	CMD_S_Multiple Multiple;
	ZeroMemory(&Multiple, sizeof(Multiple));
	Multiple.bIsMultiple=bIsMultiple;
	Multiple.wChairID = wChairID;
	BYTE multipleNum = 1;
	for (WORD k=0; k<GAME_PLAYER; k++)
	{
		if (m_multipleUser[k] == 2) multipleNum *= 2L;
	}
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_wBankerUser == i) Multiple.cbBankerScore = multipleNum * cbBankerScore;
		else if (m_multipleUser[i] == 2) Multiple.cbBankerScore = cbBankerScore * 2;
		else Multiple.cbBankerScore = cbBankerScore;
		m_pITableFrame->SendTableData(i,SUB_S_MULTIPLE,&Multiple,sizeof(Multiple));
		m_pITableFrame->SendLookonData(i,SUB_S_MULTIPLE,&Multiple,sizeof(Multiple));
	}
	for (BYTE i=0; i< 3; i++)
	{
		if (m_multipleUser[i] == 0 && i != m_wBankerUser)
			return true;
	}
	// 开始出牌
	//删除超时定时器
	m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
	m_dwStartTime=0;
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);
	CMD_S_START_OUTCARD StartOutCard;
	ZeroMemory(&StartOutCard, sizeof(StartOutCard));
	StartOutCard.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
	StartOutCard.wBankerUser=m_wBankerUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_START_OUTCARD,&StartOutCard, sizeof(StartOutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_START_OUTCARD,&StartOutCard, sizeof(StartOutCard));
	m_dwStartTime=GetTickCount();
	DWORD dwTimerTime=m_pGameCustomRule->cbTimeHeadOutCard;
	m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
	return true;
}


//用户托管

bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//效验状态
	ASSERT((wChairID < GAME_PLAYER));
	if ((wChairID>=GAME_PLAYER)) return false;
	// 加倍时间到
	if (m_pITableFrame->GetGameStatus() == GAME_SCENE_MULTIPLE)
	{
		BYTE multipleNum = 1L;
		for (WORD k=0; k<GAME_PLAYER; k++)
		{
			if (m_multipleUser[k] == 2) multipleNum *= 2L;
		}
		for (BYTE i = 0; i< 3; i++)
		{
			if (m_multipleUser[i] != 0 || i == m_wBankerUser)
				continue;
			CMD_S_Multiple Multiple;
			ZeroMemory(&Multiple, sizeof(Multiple));
			Multiple.bIsMultiple=false;
			Multiple.wChairID = i;
			if (i == m_wBankerUser) Multiple.cbBankerScore = multipleNum *m_cbBankerScore;
			else
			{
				if (m_multipleUser[i] == 2) Multiple.cbBankerScore = 2 *m_cbBankerScore;
				else Multiple.cbBankerScore = m_cbBankerScore;
			}
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MULTIPLE,&Multiple,sizeof(Multiple));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MULTIPLE,&Multiple,sizeof(Multiple));
			m_multipleUser[i]=1;
		}
		// 开始出牌
		//设置状态
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
		m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);
		CMD_S_START_OUTCARD StartOutCard;
		ZeroMemory(&StartOutCard, sizeof(StartOutCard));
		StartOutCard.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
		StartOutCard.wBankerUser=m_wBankerUser;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_START_OUTCARD,&StartOutCard, sizeof(StartOutCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_START_OUTCARD,&StartOutCard, sizeof(StartOutCard));
		m_dwStartTime=GetTickCount();
		DWORD dwTimerTime=m_pGameCustomRule->cbTimeHeadOutCard;
		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
		return true;
	}
	if (m_bTrustee[wChairID] != bTrustee) {
		m_bTrustee[wChairID]=bTrustee;

		CMD_S_Trustee Trustee;
		Trustee.bTrustee=bTrustee;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	}
	
	if(bTrustee)
	{
		if(wChairID == m_wCurrentUser)
		{
			TrusteeOperate();
		}
	}
	return true;
}


//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);
	}

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);
	}
	m_bTrustee[wChairID] = false;
	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return true;

	//删除超时定时器
	m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
	m_dwStartTime=0;

	//设置变量
	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	if (m_wCurrentUser==m_wTurnWiner) m_cbTurnCardCount=0;

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bSysOut = m_bTrustee[m_wCurrentUser];
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;
	DWORD dwTimerTime=BTRUSTEE_OUTCARD_TIME;
	if (m_bTrustee[m_wCurrentUser] != true) {
		dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
	}
	PassCard.cbTimeOutCard = dwTimerTime;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	//	IsOfflineTrustee();
	m_dwStartTime=GetTickCount();
	m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
	return true;
}

//用户叫分
bool CTableFrameSink::OnUserCallScore(WORD wChairID, BYTE cbCallScore)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	//删除超时定时器
	m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
	m_dwStartTime=0;

	//效验参数
	//ASSERT(((cbCallScore>=1)&&(cbCallScore<=3)&&(cbCallScore>m_cbBankerScore))||(cbCallScore==255));
	//if (((cbCallScore<1)||(cbCallScore>3)||(cbCallScore<=m_cbBankerScore))&&(cbCallScore!=255))
	//	cbCallScore = 255;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if(pServerUserItem != NULL && pServerUserItem->IsAndroidUser())
	{
		// 机器人
		if (m_cbBankerScore > 0)
		{
			// 如果真实玩家抢过地主，则机器人放弃
			if (m_bUserGrab)
			{
				cbCallScore = 0xFF;
			} else {
				BYTE randNum = rand()%100;
				if (randNum > ROBOT_GRAB_LORD_RATE)
				{
					cbCallScore = 0xFF;
				}
			}
		}
	}
	//设置状态
	if (cbCallScore!=0xFF)
	{
		if (m_cbScoreInfo[wChairID] == 0xFF)
		{
			return true;
		}
		if (m_cbBankerScore == 0) 
		{
			m_cbBankerScore = 3;
			cbCallScore = 3;
			m_wFirstUser = m_wCurrentUser;
		} else 
		{
			if (m_cbBankerScore == 3)
			{
				m_wFirstUser = (m_wFirstUser+1)%GAME_PLAYER;
			}
			m_cbBankerScore = m_cbBankerScore * 2;
			cbCallScore = 2;
		}
		m_wBankerUser=m_wCurrentUser;
	}
	// 真实玩家抢地主更新状态
	if (pServerUserItem != NULL && !pServerUserItem->IsAndroidUser() && cbCallScore == 2)
	{
		m_bUserGrab = true;
	}

	if (m_cbScoreInfo[wChairID] == 0)
	{
		//设置叫分
		m_cbScoreInfo[wChairID]=cbCallScore;
	}else {
		m_cbScoreInfo[wChairID] = m_cbScoreInfo[wChairID] * 2;
	}
	//设置用户
	if (m_wFirstUser==(wChairID+1)%GAME_PLAYER && m_cbScoreInfo[(wChairID+1)%GAME_PLAYER] != 0)
	{
		m_wCurrentUser=INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;
		BYTE tmp_cbChairID = wChairID+1;
		while (m_wCurrentUser != m_wFirstUser && m_cbScoreInfo[m_wCurrentUser] == 0xFF)
		{
			m_wCurrentUser=(tmp_cbChairID+1)%GAME_PLAYER;
			tmp_cbChairID++;
		}
		if (m_wCurrentUser == m_wFirstUser && m_cbScoreInfo[m_wCurrentUser] != 0)
		{
			m_wCurrentUser=INVALID_CHAIR;
		}
	}

	//构造变量
	CMD_S_CallScore CallScore;
	CallScore.wCallScoreUser=wChairID;
	CallScore.wCurrentUser=m_wCurrentUser;
	CallScore.cbUserCallScore=cbCallScore;
	CallScore.cbCurrentScore=m_cbBankerScore;
	CallScore.cbTimeCallScore=m_pGameCustomRule->cbTimeCallScore;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_SCORE,&CallScore,sizeof(CallScore));

	m_dwStartTime=GetTickCount();
	//开始判断
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		//无人叫分
		if ( m_cbBankerScore == 0 )
		{
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				if(m_cbCallScoreCount<99)
				{
					m_cbCallScoreCount++;
					return OnEventGameStart();
				}
			}
			m_wBankerUser=m_wFirstUser;
			m_cbBankerScore=1;
		}
		//设置状态
		m_pITableFrame->SetGameStatus(GAME_SCENE_MULTIPLE);

		//设置变量
		if (m_cbBankerScore==0) m_cbBankerScore=1;
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;
		m_cbCallScoreCount=0;

		//发送底牌
		m_cbHandCardCount[m_wBankerUser]+=CountArray(m_cbBankerCard);
		CopyMemory(&m_cbHandCardData[m_wBankerUser][NORMAL_COUNT],m_cbBankerCard,sizeof(m_cbBankerCard));

		//排列扑克
		m_GameLogic.SortCardList(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser],ST_ORDER);


		//设置用户
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//发送消息
		CMD_S_BankerInfo BankerInfo;
		BankerInfo.wBankerUser=m_wBankerUser;
		BankerInfo.wCurrentUser=m_wCurrentUser;
		BankerInfo.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
		BankerInfo.cbBankerScore=m_cbBankerScore;
		BankerInfo.cbMultiple = MULTIPLE_TIME;
		BankerInfo.lEnterScore = m_pGameServiceOption->lMinEnterScore;
		CopyMemory(BankerInfo.cbBankerCard,m_cbBankerCard,sizeof(m_cbBankerCard));

		m_dwStartTime = GetTickCount();
		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BANKER_INFO,&BankerInfo,sizeof(BankerInfo));

		//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		DWORD dwTimerTime=MULTIPLE_TIME;
		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
		// 重置托管
		ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL) continue;
			if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) {
				CMD_S_Other_CardData OtherCardData;
				ZeroMemory(&OtherCardData, sizeof(OtherCardData));
				for (WORD j=0; j<GAME_PLAYER; j++) {
					if (i != j) {
						CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
					}
				}
				m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
				m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
			}
		}
		return true;

	}
	//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	//if (m_bTrustee[m_wCurrentUser] == true) {
	//	IsOfflineTrustee();
	//}
	DWORD dwTimerTime=m_pGameCustomRule->cbTimeCallScore;
	m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return true;

	//获取类型
	BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);

	//类型判断
	if (cbCardType==CT_ERROR) 
	{
		//ASSERT(FALSE);
		//return false;
	}
	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);
			//return false;
			OnUserPassCard(wChairID);
			return true;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCardList(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}
	//删除超时定时器
	m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
	m_dwStartTime=0;
	m_wLastOutCardUser = wChairID;

	//出牌变量
	m_cbOutCardCount[wChairID]++;

	//设置变量
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//炸弹判断
	if (cbCardType==CT_BOMB_CARD) 
	{
		m_cbBombCount++;
		m_cbEachBombCount[wChairID]++;
		m_lMultiple *= 2L;
	}
	if (cbCardType==CT_MISSILE_CARD)
	{
		m_cbRocket = true;
		m_lMultiple *= 2L;
	}
	
	//切换用户
	m_wTurnWiner=wChairID;
	if (m_cbHandCardCount[wChairID]!=0)
	{
		//if (cbCardType!=CT_MISSILE_CARD)
		//{
		//	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		//}
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	}
	else m_wCurrentUser=INVALID_CHAIR;
	//构造数据
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(OutCard));
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	DWORD dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
	if (m_wCurrentUser==INVALID_CHAIR) {
		OutCard.bSysOut = false;
	} else {
		OutCard.bSysOut = m_bTrustee[m_wCurrentUser];
		if (m_bTrustee[m_wCurrentUser] == true) {
			dwTimerTime=BTRUSTEE_OUTCARD_TIME;
		}
	}
	OutCard.cbTimeOutCard = dwTimerTime;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	BYTE multipleNum = 1L;
	for (WORD k=0; k<GAME_PLAYER; k++)
	{
		if (m_multipleUser[k] == 2) multipleNum *= 2L;
	}
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_wBankerUser == i)
		{
			OutCard.lMultiple = m_lMultiple * multipleNum * m_cbBankerScore;

		}
		else
		{
			if (m_multipleUser[i] == 2) OutCard.lMultiple = 2 * m_cbBankerScore * m_lMultiple;
			else OutCard.lMultiple = m_cbBankerScore * m_lMultiple;
		}
		m_pITableFrame->SendTableData(i,SUB_S_OUT_CARD,&OutCard,wSendSize);
		m_pITableFrame->SendLookonData(i,SUB_S_OUT_CARD,&OutCard,wSendSize);
	}
	
	//出牌最大
	//if (cbCardType==CT_MISSILE_CARD) m_cbTurnCardCount=0;

	//结束判断

	if (m_wCurrentUser==INVALID_CHAIR) 
	{
		OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	}
	else
	{
		//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		//if (m_bTrustee[m_wCurrentUser] == true) {
		//	IsOfflineTrustee();
		//}
		m_dwStartTime=GetTickCount();
		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,(dwTimerTime+BTRUSTEE_BUFFER_TIME)*1000,-1,NULL);
	}
	//作弊
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pServerUserItem == NULL) continue;

		if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==true) 
		{
			CMD_S_Other_CardData OtherCardData;
			ZeroMemory(&OtherCardData, sizeof(OtherCardData));
			for (WORD j=0; j<GAME_PLAYER; j++) 
			{
				if (i != j) 
				{
					CopyMemory(OtherCardData.cbCardDataList[j],m_cbHandCardData[j],sizeof(BYTE)*m_cbHandCardCount[j]);
				}
			}
			m_pITableFrame->SendTableData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
			m_pITableFrame->SendLookonData(i,SUB_S_OTHER_CARDS,&OtherCardData,sizeof(OtherCardData));
		}
	}
	return true;
}

//作弊用户
bool CTableFrameSink::OnCheatCard(IServerUserItem * pIServerUserItem)
{
	if(m_pServerControl)
	{
		CMD_S_CheatCard CheatCard ;
		ZeroMemory(&CheatCard, sizeof(CheatCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			CheatCard.wCardUser[CheatCard.cbUserCount] = i;
			CheatCard.cbCardCount[CheatCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(CheatCard.cbCardData[CheatCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		//发送作弊
		//WB改 这里这样发包存在BUG
		//m_pITableFrame->SendTableData(wChairID,SUB_S_CHEAT_CARD,&CheatCard,sizeof(CheatCard));
		//m_pITableFrame->SendLookonData(wChairID,SUB_S_CHEAT_CARD,&CheatCard,sizeof(CheatCard));
		m_pServerControl->ServerControl(&CheatCard, m_pITableFrame, pIServerUserItem);
		
	}

	return true;
}

//发送作弊信息
void CTableFrameSink::SendCheatCard()
{
	if(m_pServerControl)
	{
		CMD_S_CheatCard CheatCard ;
		ZeroMemory(&CheatCard, sizeof(CheatCard));

		for(WORD i = 0; i < GAME_PLAYER; i++)
		{
			CheatCard.wCardUser[CheatCard.cbUserCount] = i;
			CheatCard.cbCardCount[CheatCard.cbUserCount] = m_cbHandCardCount[i];
			CopyMemory(CheatCard.cbCardData[CheatCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
		m_pServerControl->ServerControl(&CheatCard, m_pITableFrame);
	}

	return;
}
//设置基数
void CTableFrameSink::SetGameBaseScore(SCORE lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//是否托管
bool CTableFrameSink::IsOfflineTrustee()
{
	//非比赛模式
	// if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;
	//非常规座位
	if(m_wCurrentUser==INVALID_CHAIR) return false;

	//允许代打
	if (m_bOffLineTrustee)
	{
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==NULL) return false;

		//删除超时定时器
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_TIME);
		m_dwStartTime=0;

		//变量定义
		bool bTrusteeUser=pServerUserItem->IsTrusteeUser();
		DWORD dwTimerTime=BTRUSTEE_OUTCARD_TIME;
		
		if (bTrusteeUser==true)
		{
			dwTimerTime+=(rand()%3);
		}
		else
		{
			BYTE cbGameStatus=m_pITableFrame->GetGameStatus();
			if (cbGameStatus==GAME_SCENE_CALL) 
			{
				dwTimerTime=m_pGameCustomRule->cbTimeCallScore;
			}
			if (cbGameStatus==GAME_SCENE_PLAY)
			{
				if (m_cbTurnCardCount==0)
					dwTimerTime=m_pGameCustomRule->cbTimeHeadOutCard;
				else
					dwTimerTime=m_pGameCustomRule->cbTimeOutCard;
			}

			dwTimerTime+=(rand()%3+5);
		}
		m_pITableFrame->SetGameTimer(IDI_USER_OUT_TIME,dwTimerTime*1000,-1,NULL);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
