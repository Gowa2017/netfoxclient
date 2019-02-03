#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include "GameLogic.h"
#include <locale>
#include <float.h>
//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					300									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//下注时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					5									//空闲时间

//下注时间
#define IDI_PLACE_JETTON			2									//下注时间
#define TIME_PLACE_JETTON			15									//下注时间

//结束时间
#define IDI_GAME_END				3									//结束时间
#define TIME_GAME_END				14									//结束时间


//信息查询
#define KEY_STOCK					0
#define KEY_IMMORTAL_COUNT			1
#define KEY_ROBOT_COUNT				2
#define KEY_IMMORTAL_BET			3	
#define KEY_ROBOT_BET				4
#define KEY_MAX						5

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//构造函数
CTableFrameSink::CTableFrameSink()
{ 	
	//AllocConsole();
	//freopen("CON","wt",stdout);

	//起始分数
	ZeroMemory(m_lUserStartScore,sizeof(m_lUserStartScore));

	//下注数
	ZeroMemory(m_lAllBet,sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet,sizeof(m_lPlayBet));
	ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));

	//分数
	m_lBankerScore = 0l;
	ZeroMemory(m_lPlayScore,sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//状态变量
	m_dwBetTime=0L;
	m_bControl=false;
	m_lControlStorage=0;
	CopyMemory(m_szControlName,TEXT("无人"),sizeof(m_szControlName));
	//记录变量
	ZeroMemory(&m_OccUpYesAtconfig,sizeof(m_OccUpYesAtconfig));
	//庄家信息
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wBankerTime=0;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	for (int i =0 ; i< MAX_OCCUPY_SEAT_COUNT;i++)
	{
		m_wOccupySeatChairID[i] = -1;
	}
	//记录变量
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;

	//庄家设置
	m_nBankerTimeLimit = 0l;
	m_nBankerTimeAdd = 0l;							
	m_lExtraBankerScore = 0l;
	m_nExtraBankerTime = 0l;
	m_lPlayerBankerMAX = 0l;
	m_bExchangeBanker = true;
	m_nStorageCount=0;
	//m_lStorageMax=50000000;

	//时间控制
	m_cbFreeTime = TIME_FREE;

	m_cbBetTime = TIME_PLACE_JETTON;
	m_cbEndTime = TIME_GAME_END;

	//机器人控制
	m_nChipRobotCount = 0;
	m_nRobotListMaxCount =0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//服务控制
	m_hControlInst = NULL;
	m_pServerContro = NULL;
	m_hControlInst = LoadLibrary(TEXT("redblackServerControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}
	
	m_PeizhiUserArray.RemoveAll();

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//释放对象
VOID CTableFrameSink::Release()
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//接口查询
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
	if (m_pITableFrame == NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//读取配置
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\redblackConfig.ini"),szPath);


	  	//设置文件名
	TCHAR szPath1[MAX_PATH]=TEXT("");
	GetModuleFileName( NULL,szPath1,CountArray(szPath1));

	CString szTemp;
	szTemp = szPath1;
	int nPos = szTemp.ReverseFind(TEXT('\\'));
	_sntprintf(szPath, CountArray(szPath), TEXT("%s"), szTemp.Left(nPos));
	_sntprintf(m_szControlConfigFileName,sizeof(m_szControlConfigFileName),TEXT("%s\\GameControl\\bairen.ini"),szPath);

	_sntprintf(m_szControlRoomName, sizeof(m_szControlRoomName), _T("%d"),m_pGameServiceOption->wServerID);


	ReadConfigInformation();	

	return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
	//下注数
	ZeroMemory(m_lAllBet,sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet,sizeof(m_lPlayBet));
	ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
	//分数
	m_lBankerScore = 0l;
	ZeroMemory(m_lPlayScore,sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//机器人控制
	m_nChipRobotCount = 0;
	m_bControl=false;
	m_lControlStorage=0;
	m_nStorageCount=0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	return;
}


//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}
//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置随机种子
	srand(GetTickCount());

	//发送库存消息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%.2lf"), m_lStorageCurrent);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}
	
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//获取庄家
	IServerUserItem* pIBankerServerUserItem = NULL;
	if ( m_wCurrentBanker == INVALID_CHAIR )
	{
		m_lBankerScore = 1000000000;
	}
	else
	{
		IServerUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if ( pIBankerServerUserItem != NULL )
		{
			m_lBankerScore = pIBankerServerUserItem->GetUserScore();
		}
	}

	//设置变量
	GameStart.cbTimeLeave = m_cbBetTime;
	GameStart.wBankerUser = m_wCurrentBanker;
	GameStart.lBottomPourImpose	= m_lBottomPourImpose;
	GameStart.lBankerScore = m_lBankerScore;
	GameStart.nListUserCount = m_ApplyUserArray.GetCount();

	//下注机器人数量
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}
	GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	nChipRobotCount = 0;
	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++) 
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	if(nChipRobotCount > 0)
		GameStart.nAndriodCount=nChipRobotCount-1;

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

    //旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//游戏玩家
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//设置积分
		GameStart.lPlayBetScore=min(pIServerUserItem->GetUserScore() - m_nServiceCharge,m_lUserLimitScore);
		GameStart.lPlayFreeSocre = pIServerUserItem->GetUserScore();

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			//计算分数
			SCORE lBankerWinScore = GameOver();

			//递增次数
			m_wBankerTime++;

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//庄家信息
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;

			//扑克信息
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			GameEnd.cbCardType[0] = m_GameLogic.GetCardType(m_cbTableCardArray[0],MAX_COUNT);
			GameEnd.cbCardType[1] = m_GameLogic.GetCardType(m_cbTableCardArray[1],MAX_COUNT);
			CopyMemory(GameEnd.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
			CopyMemory(GameEnd.cbWinArea,m_cbWinArea,sizeof(GameEnd.cbWinArea));
			//所有玩家成绩
			CopyMemory(GameEnd.lAllPlayerScore,m_lUserWinScore,sizeof(GameEnd.lAllPlayerScore));
			//发送积分
			GameEnd.cbTimeLeave=m_cbEndTime;	
			for ( WORD i = 0; i < GAME_PLAYER; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if ( pIServerUserItem == NULL ) continue;
				
				//坐下玩家的成绩
				for (WORD j = 0; j<MAX_OCCUPY_SEAT_COUNT;j++)
				{
					if(m_wOccupySeatChairID[j] == pIServerUserItem->GetChairID())
					{
						GameEnd.lOccupySeatUserWinScore[j] = m_lUserWinScore[i];						
					}
				}
				//设置成绩
				GameEnd.lPlayAllScore = m_lUserWinScore[i];
	
				memcpy( GameEnd.lPlayScore, m_lPlayScore[i], sizeof(GameEnd.lPlayScore));
				
				//设置税收
				GameEnd.lRevenue = m_lUserRevenue[i];

				//发送消息					
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			return true;
		}
	case GER_NETWORK_ERROR:		//网络中断
	case GER_USER_LEAVE:		//用户离开
		{
			//闲家判断
			if (m_wCurrentBanker!=wChairID)
			{
				//变量定义
				SCORE lRevenue=0;

				//写入积分
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
				{
					for ( WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex )
					{
						if (m_lPlayBet[wChairID][wAreaIndex] != 0)
						{
							CMD_S_PlaceBetFail PlaceBetFail;
							ZeroMemory(&PlaceBetFail,sizeof(PlaceBetFail));
							PlaceBetFail.lBetArea = (BYTE)wAreaIndex;
							PlaceBetFail.lPlaceScore = m_lPlayBet[wChairID][wAreaIndex];
							PlaceBetFail.wPlaceUser = wChairID;

							//游戏玩家
							for (WORD i=0; i<GAME_PLAYER; ++i)
							{
								IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
								if (pIServerUserItem == NULL) 
									continue;

								m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceBetFail,sizeof(PlaceBetFail));
							}

							m_lAllBet[wAreaIndex] -= m_lPlayBet[wChairID][wAreaIndex];
							m_lPlayBet[wChairID][wAreaIndex] = 0;
						}
					}
				}
				else
				{

					//是否写分
					bool bWritePoints = false;
					for( WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex )
					{
						if( m_lPlayBet[wChairID][wAreaIndex] != 0 )
						{
							bWritePoints = true;
							break;
						}
					}

					//不写分
					if( !bWritePoints )
					{
						return true;
					}

					//写入积分
					if((pIServerUserItem->GetUserScore() - m_nServiceCharge) + m_lUserWinScore[wChairID] < 0L)
						m_lUserWinScore[wChairID] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

					tagScoreInfo ScoreInfoArray;
					ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
					ScoreInfoArray.lScore=m_lUserWinScore[wChairID];
					ScoreInfoArray.lRevenue = m_lUserRevenue[wChairID];

					if ( ScoreInfoArray.lScore > 0 )
						ScoreInfoArray.cbType=SCORE_TYPE_WIN;
					else if ( ScoreInfoArray.lScore < 0 )
						ScoreInfoArray.cbType=SCORE_TYPE_LOSE;
					else
						ScoreInfoArray.cbType=SCORE_TYPE_DRAW;

					m_pITableFrame->WriteUserScore(wChairID,ScoreInfoArray);
					m_lUserWinScore[wChairID] = 0;
					m_lUserRevenue[wChairID] = 0;


					//清除下注
					for (WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex )
					{
						m_lPlayBet[wChairID][wAreaIndex] = 0;
					}
				}

				return true;
			}

			//状态判断
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_END)
			{
				//提示消息
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于庄家[ %s ]强退，游戏提前结束！"),pIServerUserItem->GetNickName());

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);

				//设置时间
				m_dwBetTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);

				//计算分数
				GameOver();

				//结束消息
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//庄家信息
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				if (m_lBankerWinScore>0L) GameEnd.lBankerScore=0;

				//扑克信息
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				GameEnd.cbCardType[0] = m_GameLogic.GetCardType(m_cbTableCardArray[0],MAX_COUNT);
				GameEnd.cbCardType[1] = m_GameLogic.GetCardType(m_cbTableCardArray[1],MAX_COUNT);
				CopyMemory(GameEnd.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));

				//发送积分
				GameEnd.cbTimeLeave=m_cbEndTime;	
				for ( WORD i = 0; i < GAME_PLAYER; ++i )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if ( pIServerUserItem == NULL ) continue;

					//设置成绩
					GameEnd.lPlayAllScore = m_lUserWinScore[i];
					memcpy( GameEnd.lPlayScore, m_lPlayScore[i], sizeof(GameEnd.lPlayScore));

					//设置税收
					GameEnd.lRevenue = m_lUserRevenue[i];

					//发送消息					
					m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//扣除分数
			if(pIServerUserItem->GetUserScore() - m_nServiceCharge + m_lUserWinScore[m_wCurrentBanker] < 0L)
				m_lUserWinScore[m_wCurrentBanker] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

			tagScoreInfo ScoreInfoArray;
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			ScoreInfoArray.lScore = m_lUserWinScore[m_wCurrentBanker];
			ScoreInfoArray.lRevenue = m_lUserRevenue[m_wCurrentBanker];

			if ( ScoreInfoArray.lScore > 0 )
				ScoreInfoArray.cbType = SCORE_TYPE_WIN;
			else if ( ScoreInfoArray.lScore < 0 )
				ScoreInfoArray.cbType = SCORE_TYPE_LOSE;
			else
				ScoreInfoArray.cbType = SCORE_TYPE_DRAW;

			//m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			m_pITableFrame->WriteUserScore( m_wCurrentBanker, ScoreInfoArray );

			m_lUserWinScore[m_wCurrentBanker] = 0L;

			//切换庄家
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//游戏作弊
			BYTE cbArea[15];
			ZeroMemory(cbArea,sizeof(cbArea));
			BYTE cbControlTimes = 0;
			if(m_pServerContro != NULL && m_pServerContro->NeedControl())
			{
				m_pServerContro->GetControlArea(cbArea,cbControlTimes);
			}

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//全局信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			StatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));
			StatusFree.bGenreEducate =  m_pGameServiceOption->wServerType&GAME_GENRE_EDUCATE;
			//玩家信息
			StatusFree.lPlayFreeSocre = pIServerUserItem->GetUserScore();

			//庄家信息
			StatusFree.wBankerUser = m_wCurrentBanker;	
			StatusFree.wBankerTime = m_wBankerTime;
			StatusFree.lBankerWinScore = m_lBankerWinScore;
			StatusFree.lBankerScore = m_lBankerScore;

			//是否系统坐庄
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;

			//控制信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			CopyMemory(StatusFree.cbArea,cbArea,sizeof(StatusFree.cbArea));
			StatusFree.cbControlTimes = cbControlTimes;

			StatusFree.bIsGameCheatUser = false;
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				StatusFree.bIsGameCheatUser = true;
			}
			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_szGameRoomName, sizeof(StatusFree.szGameRoomName));
			StatusFree.lBottomPourImpose = m_lBottomPourImpose;
			//机器人配置
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(&StatusFree.occupyseatConfig, &m_OccUpYesAtconfig, sizeof(tagOccUpYesAtconfig));
			//全局下注
			CopyMemory(StatusFree.wOccupySeatChairID,m_wOccupySeatChairID,sizeof(StatusFree.wOccupySeatChairID));
			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//限制提示
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n本房间上庄条件为：%s\n区域限制为：%s\n玩家限制为：%s"), AddComma(m_lApplyBankerCondition), AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//发送申请者
			SendApplyUser(pIServerUserItem);
			//发送配置玩家
			SendPeizhiUser(pIServerUserItem);
			//更新库存信息
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				CMD_S_UpdateStorage updateStorage;
				ZeroMemory(&updateStorage, sizeof(updateStorage));
				
				updateStorage.cbReqType = RQ_REFRESH_STORAGE;
				updateStorage.lStorageStart = m_lStorageStart;
				updateStorage.lStorageDeduct = m_lStorageDeduct;
				updateStorage.lStorageCurrent = m_lStorageCurrent;
				updateStorage.lStorageMax1 = m_lStorageMax1;
				updateStorage.lStorageMul1 = m_lStorageMul1;
				updateStorage.lStorageMax2 = m_lStorageMax2;
				updateStorage.lStorageMul2 = m_lStorageMul2;
			
				m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
			}

			return bSuccess;
		}
	case GAME_SCENE_BET:		//游戏状态
	case GAME_SCENE_END:		//结束状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);		

			//游戏作弊
			BYTE cbArea[15];
			ZeroMemory(cbArea,sizeof(cbArea));
			BYTE cbControlTimes = 0;
			if(m_pServerContro != NULL && m_pServerContro->NeedControl())
			{
				m_pServerContro->GetControlArea(cbArea,cbControlTimes);
			}

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));
			int x = sizeof(StatusPlay);

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL) - m_dwBetTime;
			int	nTotalTime = ( (cbGameStatus == GAME_SCENE_BET) ? m_cbBetTime : m_cbEndTime);
			StatusPlay.cbTimeLeave = (BYTE)(nTotalTime - __min(dwPassTime, (DWORD)nTotalTime));
			StatusPlay.cbGameStatus = m_pITableFrame->GetGameStatus();		

			//全局下注
			memcpy(StatusPlay.lAllBet, m_lAllBet, sizeof(StatusPlay.lAllBet));
			StatusPlay.lPlayFreeSocre = pIServerUserItem->GetUserScore();
			StatusPlay.bGenreEducate =  m_pGameServiceOption->wServerType&GAME_GENRE_EDUCATE;


			//真人玩家总下注
			SCORE playerTotleBet[AREA_MAX];
			ZeroMemory(playerTotleBet,sizeof(playerTotleBet));
			//机器人总下注
			SCORE androidTotleBet[AREA_MAX];
			ZeroMemory(androidTotleBet,sizeof(androidTotleBet));
			//真人玩家每个区域的下注
			SCORE playerAreaBet[GAME_PLAYER][AREA_MAX];
			ZeroMemory(playerAreaBet,sizeof(playerAreaBet));
			//玩家id
			DWORD dwPlayerID[GAME_PLAYER];
			ZeroMemory(dwPlayerID,sizeof(dwPlayerID));
			//每个玩家总下注
			SCORE lplayerTotleBet[GAME_PLAYER];
			ZeroMemory(lplayerTotleBet,sizeof(lplayerTotleBet));

			for ( WORD i = 0; i < GAME_PLAYER; ++i )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;
				if (pIServerUserItem->IsAndroidUser() == FALSE)
				{
					for (int j=0;j<AREA_MAX;j++)
					{
						playerTotleBet[j] += m_lPlayBet[i][j];
						playerAreaBet[i][j] = m_lPlayBet[i][j];
						lplayerTotleBet[i] += m_lPlayBet[i][j];
					}
				}
				else
				{
					for (int m=0;m<AREA_MAX;m++)
					{
						androidTotleBet[m] += m_lPlayBet[i][m];
					}
				}
				dwPlayerID[i] = pIServerUserItem->GetGameID();

			}
			//memcpy(StatusPlay.lPlayerBet, playerTotleBet, sizeof(StatusPlay.lPlayerBet));
			//memcpy(StatusPlay.lAndroidBet, androidTotleBet, sizeof(StatusPlay.lAndroidBet));
			//memcpy(StatusPlay.lPlayerAreaBet, playerAreaBet, sizeof(StatusPlay.lPlayerAreaBet));
			//memcpy(StatusPlay.dwGameID, dwPlayerID, sizeof(StatusPlay.dwGameID));
			//memcpy(StatusPlay.lPlayerTotleBet, lplayerTotleBet, sizeof(StatusPlay.lPlayerTotleBet));


			//玩家下注
			if (pIServerUserItem->GetUserStatus() != US_LOOKON && bSendSecret)
			{
				memcpy(StatusPlay.lPlayBet, m_lPlayBet[wChiarID], sizeof(StatusPlay.lPlayBet));
				memcpy(StatusPlay.lPlayScore, m_lPlayScore[wChiarID], sizeof(StatusPlay.lPlayScore));

				//最大下注
				StatusPlay.lPlayBetScore = min(pIServerUserItem->GetUserScore() - m_nServiceCharge, m_lUserLimitScore);
			}

			//庄家信息
			StatusPlay.wBankerUser = m_wCurrentBanker;			
			StatusPlay.wBankerTime = m_wBankerTime;
			StatusPlay.lBankerWinScore = m_lBankerWinScore;	
			StatusPlay.lBankerScore = m_lBankerScore;

			//是否系统坐庄
			StatusPlay.bEnableSysBanker = m_bEnableSysBanker;

			//控制信息
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;	

			CopyMemory(StatusPlay.cbArea,cbArea,sizeof(StatusPlay.cbArea));
			StatusPlay.cbControlTimes = cbControlTimes;

			StatusPlay.bIsGameCheatUser = false;
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				StatusPlay.bIsGameCheatUser = true;
			}
			//结束判断
			if (cbGameStatus == GAME_SCENE_END)
			{
				//扑克信息
				CopyMemory(StatusPlay.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				StatusPlay.cbCardType[0] = m_GameLogic.GetCardType(m_cbTableCardArray[0],MAX_COUNT);
				StatusPlay.cbCardType[1] = m_GameLogic.GetCardType(m_cbTableCardArray[1],MAX_COUNT);
				//结束分数
				StatusPlay.lPlayAllScore = m_lUserWinScore[wChiarID];
				StatusPlay.lRevenue = m_lUserRevenue[wChiarID];
				CopyMemory(StatusPlay.cbWinArea,m_cbWinArea,sizeof(StatusPlay.cbWinArea));
				//所有玩家成绩
				CopyMemory(StatusPlay.lAllPlayerScore,m_lUserWinScore,sizeof(StatusPlay.lAllPlayerScore));	
				
			}

			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_szGameRoomName, sizeof(StatusPlay.szGameRoomName));
			StatusPlay.lBottomPourImpose = m_lBottomPourImpose;
			//机器人配置
			if(pIServerUserItem->IsAndroidUser())
			{
				tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
				ASSERT(pCustomConfig);

				CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
			}

			CopyMemory(&StatusPlay.occupyseatConfig, &m_OccUpYesAtconfig, sizeof(tagOccUpYesAtconfig));
			//全局下注
			CopyMemory(StatusPlay.wOccupySeatChairID,m_wOccupySeatChairID,sizeof(StatusPlay.wOccupySeatChairID));
			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

			//限制提示
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n本房间上庄条件为：%s\n区域限制为：%s\n玩家限制为：%s"), AddComma(m_lApplyBankerCondition), AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
			
			//发送申请者
			SendApplyUser( pIServerUserItem );
			//发送配置玩家
			SendPeizhiUser(pIServerUserItem);
			//更新库存信息
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				CMD_S_UpdateStorage updateStorage;
				ZeroMemory(&updateStorage, sizeof(updateStorage));
				
				updateStorage.cbReqType = RQ_REFRESH_STORAGE;
				updateStorage.lStorageStart = m_lStorageStart;
				updateStorage.lStorageDeduct = m_lStorageDeduct;
				updateStorage.lStorageCurrent = m_lStorageCurrent;
				updateStorage.lStorageMax1 = m_lStorageMax1;
				updateStorage.lStorageMul1 = m_lStorageMul1;
				updateStorage.lStorageMax2 = m_lStorageMax2;
				updateStorage.lStorageMul2 = m_lStorageMul2;
			
				m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
			}
			
			//发送玩家下注信息
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				SendUserBetInfo(pIServerUserItem);
			}

			return bSuccess;
		}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case IDI_FREE:		//空闲时间
		{
			//开始游戏
			m_pITableFrame->StartGame();

			//设置时间
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_cbBetTime*1000,1,0L);

			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);			

				//结束游戏
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//设置时间
				m_dwBetTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			SCORE TempStartScore=0;

			
			//系统坐庄
			bool bSystemBanker = false;
			if ( m_wCurrentBanker == INVALID_CHAIR )
			{
				bSystemBanker = true;
			}
			else
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				if ( pServerUserItem != NULL )	
					bSystemBanker = pServerUserItem->IsAndroidUser();
			}

			//写入积分
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//判断是否写分
				bool bXie = false;
				for ( WORD iA = 0; iA < AREA_MAX; ++iA )
				{
					if ( m_lPlayBet[wUserChairID][iA] != 0 )
					{
						bXie = true;
						break;
					}
				}

				if ( wUserChairID == m_wCurrentBanker )
				{
					bXie = true;
				}

				if ( !bXie )
				{
					continue;
				}

				//防止负分
				if(((pIServerUserItem->GetUserScore() - m_nServiceCharge) + m_lUserWinScore[wUserChairID]) < 0L)
					m_lUserWinScore[wUserChairID] = -(pIServerUserItem->GetUserScore() - m_nServiceCharge);

				//写入积分				
				ScoreInfoArray[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
				ScoreInfoArray[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];

				if ( ScoreInfoArray[wUserChairID].lScore > 0 )
					ScoreInfoArray[wUserChairID].cbType = SCORE_TYPE_WIN;
				else if ( ScoreInfoArray[wUserChairID].lScore < 0 )
					ScoreInfoArray[wUserChairID].cbType = SCORE_TYPE_LOSE;
				else
					ScoreInfoArray[wUserChairID].cbType = SCORE_TYPE_DRAW;

				if(bSystemBanker && !pIServerUserItem->IsAndroidUser())				//系统坐庄
					TempStartScore += m_lUserWinScore[wUserChairID];
				else if(bSystemBanker == false && pIServerUserItem->IsAndroidUser())			//玩家坐庄
					TempStartScore -= m_lUserWinScore[wUserChairID];


				//更新玩家单日和总输赢
				SCORE lTdTotalScore = pIServerUserItem->GetTdTotalScore();
				SCORE lTotalScore = pIServerUserItem->GetTotalScore();

				pIServerUserItem->SetTotalScore(lTotalScore +  m_lUserWinScore[wUserChairID]);
				pIServerUserItem->SetTdTotalScore(lTdTotalScore +  m_lUserWinScore[wUserChairID]);
			}



			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			
	  for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
      {
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
			if(pIServerUserItem == NULL)
			{
				continue;
			}
	  		for (int i =0;i<MAX_OCCUPY_SEAT_COUNT;i++)
			{
				if(m_wOccupySeatChairID[i] == pIServerUserItem->GetChairID() && pIServerUserItem->GetUserScore() < m_OccUpYesAtconfig.lOccupySeatFree)
				{
					m_wOccupySeatChairID[i] = -1;
					break;
				}
			}
	  }
		
		CMD_S_UpdateOccupySeat UpdateOccupySeat;
		ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
		//UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();
		//全局下注
		CopyMemory(&UpdateOccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(UpdateOccupySeat.tabWOccupySeatChairID));

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			//库存衰减
			INT lTempDeduct=0;
			lTempDeduct=m_lStorageDeduct;
			bool bDeduct=NeedDeductStorage();
			lTempDeduct=bDeduct?lTempDeduct:0;

			ReadConfigInformation();

			for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if(pIServerUserItem == NULL)
					continue;
				if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
					continue;
				for(WORD i = 0; i < GAME_PLAYER; ++i)
				{
					IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
					if(pUserItem == NULL)
						continue;
					if (pUserItem->IsAndroidUser())
						continue;
					CMD_S_UpAlllosewin UpAlllosewin;
					ZeroMemory(&UpAlllosewin, sizeof(UpAlllosewin));
					UpAlllosewin.dwGameID = pUserItem->GetGameID();
					UpAlllosewin.lTdTotalScore = pUserItem->GetTdTotalScore();
					UpAlllosewin.lTotalScore = pUserItem->GetTotalScore();
					UpAlllosewin.lScore = pUserItem->GetUserInfo()->lScore;
					m_pITableFrame->SendTableData(wUserChairID, SUB_S_UPALLLOSEWIN, &UpAlllosewin, sizeof(UpAlllosewin));
					m_pITableFrame->SendLookonData(wUserChairID, SUB_S_UPALLLOSEWIN, &UpAlllosewin, sizeof(UpAlllosewin));
				}
			}


			if(m_pServerContro!=NULL&&!m_bControl)
			{

				m_lStorageCurrent -= TempStartScore;

				//库存衰减
				if (TempStartScore < -0.001 || TempStartScore > 0.001)
				{
					m_lAttenuationScore +=  m_lStorageCurrent*m_lStorageDeduct/1000;
					m_lStorageCurrent -= m_lStorageCurrent*m_lStorageDeduct/1000;
				}

				TCHAR szStorage[1024]=TEXT("");
				_sntprintf(szStorage,sizeof(szStorage),TEXT("%.2lf"),m_lStorageCurrent);
				WritePrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),szStorage,m_szControlConfigFileName);

				_sntprintf(szStorage,sizeof(szStorage),TEXT("%.2lf"),m_lAttenuationScore);
				WritePrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),szStorage,m_szControlConfigFileName);
			}

			//if (m_lStorageCurrent > 0)
			//	m_lStorageCurrent = m_lStorageCurrent - m_lStorageCurrent*lTempDeduct/1000;

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//切换庄家
			ChangeBanker(false);

			//设置时间
			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,0L);

			//游戏作弊
			BYTE cbControl = 0;
			if(m_pServerContro != NULL && m_pServerContro->NeedControl())
			{
				cbControl = 1;
			}

			//发送消息
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_cbFreeTime;
			GameFree.lStorageStart=m_lStorageCurrent;
			GameFree.nListUserCount=m_ApplyUserArray.GetCount()-1;
			GameFree.cbControl = cbControl;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			
			//更新库存信息
			for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserID);
				if ( pIServerUserItem == NULL ) continue;
				if( !CUserRight::IsGameCheatUser( pIServerUserItem->GetUserRight() ) ) continue;

				CMD_S_UpdateStorage updateStorage;
				ZeroMemory(&updateStorage, sizeof(updateStorage));
				
				updateStorage.cbReqType = RQ_REFRESH_STORAGE;
				updateStorage.lStorageStart = m_lStorageStart;
				updateStorage.lStorageDeduct = m_lStorageDeduct;
				updateStorage.lStorageCurrent = m_lStorageCurrent;
				updateStorage.lStorageMax1 = m_lStorageMax1;
				updateStorage.lStorageMul1 = m_lStorageMul1;
				updateStorage.lStorageMax2 = m_lStorageMax2;
				updateStorage.lStorageMul2 = m_lStorageMul2;
			
				m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
			}

			return true;
		}
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
			if (wDataSize!=sizeof(CMD_C_PlaceBet)) return false;

			//用户效验
			//if ( pIServerUserItem->GetUserStatus() != US_PLAYING ) return true;

			//消息处理
			CMD_C_PlaceBet * pPlaceBet = (CMD_C_PlaceBet *)pData;
			return OnUserPlayBet(pIServerUserItem->GetChairID(), pPlaceBet->cbBetArea, pPlaceBet->lBetScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if ( m_pServerContro == NULL)
				return false;
			CopyMemory(m_szControlName,pIServerUserItem->GetNickName(),sizeof(m_szControlName));

			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}
	case SUB_C_OCCUPYSEAT:
		{
			ASSERT(wDataSize==sizeof(CMD_C_OccupySeat));
			if(wDataSize!=sizeof(CMD_C_OccupySeat))
				return false;

			//消息处理
			CMD_C_OccupySeat * pOccupySeat=(CMD_C_OccupySeat *)pData;

			if(m_OccUpYesAtconfig.occupyseatType == OCCUPYSEAT_VIPTYPE)
			{

			}
			else if(m_OccUpYesAtconfig.occupyseatType == OCCUPYSEAT_CONSUMETYPE)
			{
				//CMD_S_OccupySeat_Fail OccupySeat_Fail;
				//ZeroMemory(&OccupySeat_Fail,sizeof(OccupySeat_Fail));
				//OccupySeat_Fail.cbAlreadyOccupySeatIndex = pOccupySeat->cbOccupySeatIndex;
				//OccupySeat_Fail.wAlreadyOccupySeatChairID = pOccupySeat->wOccupySeatChairID;
				//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
				//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
				//return true;
			}
			else
			{

			}
			CMD_S_OccupySeat OccupySeat;
			ZeroMemory(&OccupySeat, sizeof(OccupySeat));
			OccupySeat.wOccupySeatChairID = pOccupySeat->wOccupySeatChairID;
			OccupySeat.cbOccupySeatIndex = pOccupySeat->cbOccupySeatIndex;
			m_wOccupySeatChairID[OccupySeat.cbOccupySeatIndex] = pOccupySeat->wOccupySeatChairID;
			//全局下注
			CopyMemory(&OccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(OccupySeat.tabWOccupySeatChairID));

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

			return true;
		}
	case SUB_C_QUIT_OCCUPYSEAT:
		{
			for (int i =0;i<MAX_OCCUPY_SEAT_COUNT;i++)
			{
				if(m_wOccupySeatChairID[i] == pIServerUserItem->GetChairID())
				{
					m_wOccupySeatChairID[i] = -1;
					break;
				}
			}

			CMD_S_UpdateOccupySeat UpdateOccupySeat;
			ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
			UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();
			//全局下注
			CopyMemory(&UpdateOccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(UpdateOccupySeat.tabWOccupySeatChairID));

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			return true;
		}
	case SUB_C_UPDATE_STORAGE:		//更新库存
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			//消息处理
			CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
			{
				m_lStorageDeduct = pUpdateStorage->lStorageDeduct;
				m_lStorageCurrent = pUpdateStorage->lStorageCurrent;
				m_lStorageMax1 = pUpdateStorage->lStorageMax1;
				m_lStorageMul1 = pUpdateStorage->lStorageMul1;
				m_lStorageMax2 = pUpdateStorage->lStorageMax2;
				m_lStorageMul2 = pUpdateStorage->lStorageMul2;

				//记录信息
				CString strControlInfo, str;
				str.Format(TEXT("%s"), TEXT("修改库存设置！"));
				CTime Time(CTime::GetCurrentTime());
				strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\r\n"),
					m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
					Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

				WriteInfo(TEXT("百人红九控制信息.log"),strControlInfo);
			}
			
			for(WORD wUserID = 0; wUserID < GAME_PLAYER; wUserID++)
			{
				IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(wUserID);
				if ( pIServerUserItemSend == NULL ) continue;
				if( !CUserRight::IsGameCheatUser( pIServerUserItemSend->GetUserRight() ) ) continue;

				if(RQ_REFRESH_STORAGE == pUpdateStorage->cbReqType && pIServerUserItem->GetChairID() != wUserID) continue;

				CMD_S_UpdateStorage updateStorage;
				ZeroMemory(&updateStorage, sizeof(updateStorage));
				
				if(RQ_SET_STORAGE == pUpdateStorage->cbReqType && pIServerUserItem->GetChairID() == wUserID)
				{
					updateStorage.cbReqType = RQ_SET_STORAGE;
				}
				else
				{
					updateStorage.cbReqType = RQ_REFRESH_STORAGE;
				}

				updateStorage.lStorageStart = m_lStorageStart;
				updateStorage.lStorageDeduct = m_lStorageDeduct;
				updateStorage.lStorageCurrent = m_lStorageCurrent;
				updateStorage.lStorageMax1 = m_lStorageMax1;
				updateStorage.lStorageMul1 = m_lStorageMul1;
				updateStorage.lStorageMax2 = m_lStorageMax2;
				updateStorage.lStorageMul2 = m_lStorageMul2;

				m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
			}

			return true;
		}
	case SUB_C_PEIZHI_USER:
		{
			ASSERT(wDataSize==sizeof(CMD_C_peizhiVec));
			if(wDataSize!=sizeof(CMD_C_peizhiVec)) return false;
			//消息处理
			CMD_C_peizhiVec * pPeizhiVec=(CMD_C_peizhiVec *)pData;
			if(pPeizhiVec == NULL)return false;
			//变量定义
			IServerUserItem * pUserItem=m_pITableFrame->SearchUserItemGameID(pPeizhiVec->dwGameID);
			if(pUserItem!= NULL)
			{
				bool bOK = false;
				for(int i =0;i < m_PeizhiUserArray.GetCount();i++)
				{
					if(m_PeizhiUserArray[i] ==  pPeizhiVec->dwGameID)
					{
						bOK = true;
						break;
					}
				}
				if(bOK == false)
				{
					m_PeizhiUserArray.Add(pPeizhiVec->dwGameID);
					CMD_S_peizhiVec peizhiVec;
					ZeroMemory(&peizhiVec,sizeof(peizhiVec));
					peizhiVec.dwGameID = pPeizhiVec->dwGameID;
					peizhiVec.lScore = pUserItem->GetUserInfo()->lScore;
					//CopyMemory(peizhiVec.szNickName,pUserItem->GetNickName(),sizeof(peizhiVec.szNickName));
	
					for(int i =0;i<GAME_PLAYER;i++)
					{
						IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(i);
						if(pControlUserItem == NULL)continue;
						//权限判断
						if(CUserRight::IsGameCheatUser(pControlUserItem->GetUserRight())==false)
							continue;
						m_pITableFrame->SendTableData(i, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
					}
				}
			}
			return true;
		}
	case SUB_C_DelPeizhi:
		{
			ASSERT(wDataSize==sizeof(CMD_C_DelPeizhi));
			if(wDataSize!=sizeof(CMD_C_DelPeizhi)) return false;

			CMD_C_DelPeizhi * pDelPeizhi=(CMD_C_DelPeizhi *)pData;
			if(pDelPeizhi == NULL)return false;
			bool bOK = false;
			//撤销玩家
			for (WORD i=0; i<m_PeizhiUserArray.GetCount(); ++i)
			{
				//获取玩家
				DWORD dwGameID=m_PeizhiUserArray[i];

				//条件过滤
				if (dwGameID != pDelPeizhi->dwGameID) continue;

				//删除玩家
				m_PeizhiUserArray.RemoveAt(i);
				bOK= true;
				break;
			}
			if(bOK)
			{
				CMD_S_DelPeizhi DelPeizhi;
				ZeroMemory(&DelPeizhi,sizeof(DelPeizhi));
				DelPeizhi.dwGameID = pDelPeizhi->dwGameID;
				for(int i =0;i<GAME_PLAYER;i++)
				{
					IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pControlUserItem == NULL)continue;
					//权限判断
					if(CUserRight::IsGameCheatUser(pControlUserItem->GetUserRight())==false)
						continue;
					m_pITableFrame->SendTableData(i, SUB_S_DelPeizhi, &DelPeizhi, sizeof(DelPeizhi));
				}
			}
			return true;
		}
	}

	return false;
}
void  CTableFrameSink::SendPeizhiUser( IServerUserItem *pRcvServerUserItem )
{
	//权限判断
	if(CUserRight::IsGameCheatUser(pRcvServerUserItem->GetUserRight())==false)
		return;
	for (INT_PTR nUserIdx=0; nUserIdx<m_PeizhiUserArray.GetCount(); ++nUserIdx)
	{
		DWORD dwGameID=m_PeizhiUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem=m_pITableFrame->SearchUserItemGameID(dwGameID);
		if (!pServerUserItem) continue;

		CMD_S_peizhiVec peizhiVec;
		ZeroMemory(&peizhiVec,sizeof(peizhiVec));
		peizhiVec.dwGameID = pServerUserItem->GetGameID();
		peizhiVec.lScore = pServerUserItem->GetUserInfo()->lScore;
		//CopyMemory(peizhiVec.szNickName,pServerUserItem->GetNickName(),sizeof(peizhiVec.szNickName));
		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
	}
}
//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// 聊天消息
	if (wSubCmdID == SUB_GF_USER_CHAT && CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
	{
		//变量定义
		CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

		//效验参数
		ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
		ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
		ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

		//效验参数
		if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
		if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
		if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

		bool bKeyProcess = false;
		CString strChatString(pUserChat->szChatString);
		CString strKey[KEY_MAX] = { TEXT("/stock"), TEXT("/immortal count"), TEXT("/robot count"), TEXT("/immortal bet"), TEXT("/robot bet") };
		CString strName[KEY_MAX] = { TEXT("库存"), TEXT("真人数量"), TEXT("机器人数量"), TEXT("真人下注"), TEXT("机器人下注") };
		if ( strChatString == TEXT("/help") )
		{
			bKeyProcess = true;
			CString strMsg;
			for ( int i = 0 ; i < KEY_MAX; ++i)
			{
				strMsg += TEXT("\n");
				strMsg += strKey[i];
				strMsg += TEXT(" 查看");
				strMsg += strName[i];
			}
			m_pITableFrame->SendGameMessage(pIServerUserItem, strMsg, SMT_CHAT);	
		}
		else 
		{
			CString strMsg;
			for ( int i = 0 ; i < KEY_MAX; ++i)
			{
				if ( strChatString == strKey[i] )
				{
					bKeyProcess = true;
					switch(i)
					{
					case KEY_STOCK:
						{
							strMsg.Format(TEXT("库存剩余量：%.2lf"), m_lStorageCurrent);
						}
						break;
					case KEY_IMMORTAL_COUNT:
						{
							int nImmortal = 0;
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//获取用户
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( !pIServerUserItem->IsAndroidUser() )
									nImmortal += 1;
							}
							strMsg.Format(TEXT("真人数量：%d"), nImmortal);
						}
						break;
					case KEY_ROBOT_COUNT:
						{
							int nRobot = 0;
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//获取用户
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( pIServerUserItem->IsAndroidUser() )
									nRobot += 1;
							}
							strMsg.Format(TEXT("机器人数量：%d"), nRobot);
						}
						break;
					case KEY_IMMORTAL_BET:
						{
							SCORE lBet[AREA_MAX] = {0};
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//获取用户
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( !pIServerUserItem->IsAndroidUser() )
								{	
									for ( int nArea = 0; nArea < AREA_MAX; ++nArea )
									{
										lBet[nArea] += m_lPlayBet[wChairID][nArea];
									}
								}
							}
						}
						break;
					case KEY_ROBOT_BET:
						{
							SCORE lBet[AREA_MAX] = {0};
							for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
							{
								//获取用户
								IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
								if (pIServerUserItem == NULL) continue;

								if ( pIServerUserItem->IsAndroidUser() )
								{	
									for ( int nArea = 0; nArea < AREA_MAX; ++nArea )
									{
										lBet[nArea] += m_lPlayBet[wChairID][nArea];
									}
								}
							}
						}
						break;
					}
					m_pITableFrame->SendGameMessage(pIServerUserItem, strMsg, SMT_CHAT);	
					break;
				}
			}
		}
		return bKeyProcess;
	}
	return false;
}
//用户下注
bool CTableFrameSink::OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(wChairID == m_wCurrentBanker)return true;
	if(wChairID == INVALID_CHAIR)return false;
	if(pIServerUserItem!= NULL)
	{
		for(int i =0;i < AREA_MAX;i++)
		{
			if(m_lPlayBet[wChairID][i] > 0.001f)return true;
		}
	}
	return false;
}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();

	//设置时间
	if ((bLookonUser == false)&&(m_dwBetTime == 0L))
	{
		m_dwBetTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
	}

	//限制提示
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n本房间上庄条件为：%.2lf\n区域限制为：%.2lf\n玩家限制为：%.2lf"),
		AddComma(m_lApplyBankerCondition),AddComma(m_lAreaLimitScore), AddComma(m_lUserLimitScore));
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);


	for (INT_PTR nUserIdx=0; nUserIdx<m_PeizhiUserArray.GetCount(); ++nUserIdx)
	{
		DWORD dwGameID=m_PeizhiUserArray[nUserIdx];
		if (pIServerUserItem->GetGameID() == dwGameID)
		{
			for(int j = 0;j<GAME_PLAYER;j++)
			{
				//获取玩家
				IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(j);
				if (pServerUserItem == NULL)continue;
				//权限判断
				if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==false)continue;
				CMD_S_peizhiVec peizhiVec;
				ZeroMemory(&peizhiVec,sizeof(peizhiVec));
				peizhiVec.dwGameID = pIServerUserItem->GetGameID();
				//CopyMemory(peizhiVec.szNickName,pIServerUserItem->GetNickName(),sizeof(peizhiVec.szNickName));
				//发送消息
				m_pITableFrame->SendTableData(j, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
			}

		}
	}

	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//起始分数
	m_lUserStartScore[wChairID] = 0;

	//记录成绩
	if (bLookonUser == false)
	{
		//切换庄家
		if (wChairID == m_wCurrentBanker) ChangeBanker(true);

		//取消申请
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserPlayBet(WORD wChairID, BYTE cbBetArea, SCORE lBetScore)
{
	//效验参数
	ASSERT((cbBetArea<AREA_MAX)&&(lBetScore>0L));
	if ((cbBetArea>=AREA_MAX)||(lBetScore<=0L)) return false;

	//效验状态
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_BET)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}

	//庄家判断
	if (m_wCurrentBanker == wChairID)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	if (m_bEnableSysBanker == false && m_wCurrentBanker == INVALID_CHAIR)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	
	//庄家积分
	SCORE lBankerScore = 0;
	if(INVALID_CHAIR != m_wCurrentBanker)
	{
		IServerUserItem * pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		lBankerScore = pIServerUserItemBanker->GetUserScore();
	}

	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	SCORE lUserScore = pIServerUserItem->GetUserScore() - m_nServiceCharge;
	SCORE lBetCount = 0;
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		lBetCount += m_lPlayBet[wChairID][i];
	}
	
	SCORE lAllBetCount=0L;
	for (int nAreaIndex=0; nAreaIndex < AREA_MAX; ++nAreaIndex)
	{
		lAllBetCount += m_lAllBet[nAreaIndex];
	}

	//成功标识
	bool bPlaceBetSuccess=true;

	//合法校验
	if (lUserScore < (lBetCount + lBetScore) || lUserScore < m_lBottomPourImpose)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}
	else
	{
		//机器人验证
		if(pIServerUserItem->IsAndroidUser())
		{
			//区域限制
			if (m_lRobotAreaScore[cbBetArea] + lBetScore > m_lRobotAreaLimit)
				return true;

			//数目限制
			bool bHaveChip = false;
			bHaveChip = (lBetCount>0);

			if (!bHaveChip)
			{
				if (m_nChipRobotCount+1 > m_nMaxChipRobot)
				{
					SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
					return true;
				}
				else
					m_nChipRobotCount++;
			}

			//统计分数
			if (bPlaceBetSuccess)
				m_lRobotAreaScore[cbBetArea] += lBetScore;
		}
	}

	if (m_lUserLimitScore < lBetCount + lBetScore)
	{
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
		return true;
	}


	////合法验证
	//if ( GetMaxPlayerScore(cbBetArea, wChairID,lBetScore))
	//{
	//	m_lAllBet[cbBetArea] += lBetScore;
	//	m_lPlayBet[wChairID][cbBetArea]  += lBetScore;
	//}
	//else
	//{
	//	bPlaceBetSuccess = false;
	//}

	m_lAllBet[cbBetArea] += lBetScore;
	m_lPlayBet[wChairID][cbBetArea]  += lBetScore;

	if (bPlaceBetSuccess)
	{
		//变量定义
		CMD_S_PlaceBet PlaceBet;
		ZeroMemory(&PlaceBet,sizeof(PlaceBet));

		//构造变量
		PlaceBet.wChairID=wChairID;
		PlaceBet.cbBetArea=cbBetArea;
		PlaceBet.lBetScore=lBetScore;	
		PlaceBet.cbAndroidUser=pIServerUserItem->IsAndroidUser()?true:false;
		PlaceBet.cbAndroidUserT=pIServerUserItem->IsAndroidUser()?true:false;
		PlaceBet.lPlayerAreaBet = m_lPlayBet[wChairID][cbBetArea];
		//已下注额
		SCORE lNowBet = 0l;
		for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
			lNowBet += m_lPlayBet[wChairID][nAreaIndex];

		PlaceBet.lAllJetton = lNowBet;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));

		//发送玩家下注信息
		if(!pIServerUserItem->IsAndroidUser())
		{
			for(WORD i=0; i<GAME_PLAYER; i++)
			{
				IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
				if(NULL == pIServerUserItemSend) continue;
				if(!CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight())) continue;

				SendUserBetInfo(pIServerUserItemSend);
			}
		}
	}
	else
	{
		//发送消息
		SendPlaceBetFail(wChairID,cbBetArea,lBetScore);
	}

	return true;
}

//发送消息
void CTableFrameSink::SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, SCORE lBetScore)
{
	CMD_S_PlaceBetFail PlaceBetFail;
	ZeroMemory(&PlaceBetFail,sizeof(PlaceBetFail));
	PlaceBetFail.lBetArea=cbBetArea;
	PlaceBetFail.lPlaceScore=lBetScore;
	PlaceBetFail.wPlaceUser=wChairID;

	//发送消息
	m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceBetFail,sizeof(PlaceBetFail));
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
	//随机扑克
	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));
	//m_cbTableCardArray[0][0] = 0x08;
	//m_cbTableCardArray[0][1] = 0x08;

	//m_cbTableCardArray[1][0] = 0x07;
	//m_cbTableCardArray[1][1] = 0x07;

	//首次发牌
	m_cbCardCount[INDEX_PLAYER] = 3;
	m_cbCardCount[INDEX_BANKER] = 3;
	return true;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//合法判断
	SCORE lUserScore = pIApplyServerUserItem->GetUserScore();
	if ( lUserScore < m_lApplyBankerCondition )
	{
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你的金币不足以申请庄家，申请失败！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//存在判断
	WORD wApplyUserChairID = pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID = m_ApplyUserArray[nUserIdx];
		if (wChairID == wApplyUserChairID)
		{
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你已经申请了庄家，不需要再次申请！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}
	
	if (pIApplyServerUserItem->IsAndroidUser()&&(m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
	{
		return true;
	}

	//保存信息 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser = wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//切换判断
	if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE && m_ApplyUserArray.GetCount() == 1)
	{
		ChangeBanker(false);
	}

	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID() == m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_SCENE_FREE)
	{
		//发送消息
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("游戏已经开始，不可以取消当庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//存在判断
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker!=wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker == wChairID)
		{
			//切换庄家 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return false;
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//切换标识
	bool bChangeBanker=false;

	//取消当前
	if (bCancelCurrentBanker)
	{
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			//获取玩家
			WORD wChairID=m_ApplyUserArray[i];

			//条件过滤
			if (wChairID!=m_wCurrentBanker) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			break;
		}

		//设置庄家
		m_wCurrentBanker=INVALID_CHAIR;

		//轮换判断
		TakeTurns();

		//设置变量
		bChangeBanker=true;
		m_bExchangeBanker = true;
	}
	//轮庄判断
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//获取庄家
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if(pIServerUserItem)
		{
			SCORE lBankerScore=pIServerUserItem->GetUserScore();

			//次数判断
			if (m_lPlayerBankerMAX<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//庄家增加判断 同一个庄家情况下只判断一次
				if(m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore>=m_lApplyBankerCondition)
				{
					//加庄局数设置：当庄家坐满设定的局数之后(m_nBankerTimeLimit)，
					//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
					//可以再加坐庄m_nBankerTimeAdd局，加庄局数可设置。

					//金币超过m_lExtraBankerScore之后，
					//就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_nExtraBankerTime局。
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
						WORD wChairID = m_ApplyUserArray[i];
						IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						SCORE lScore = pIServerUserItem->GetUserScore();

						if ( wChairID != m_wCurrentBanker && lBankerScore <= lScore )
						{
							bScoreMAX = false;
							break;
						}
					}

					if ( bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l) )
					{
						if ( bScoreMAX )
						{
							m_lPlayerBankerMAX += m_nBankerTimeAdd;
						}
						if ( lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l )
						{
							m_lPlayerBankerMAX += m_nExtraBankerTime;
						}
						return true;
					}
				}

				//撤销玩家
				for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//获取玩家
					WORD wChairID=m_ApplyUserArray[i];

					//条件过滤
					if (wChairID!=m_wCurrentBanker) continue;

					//删除玩家
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//设置庄家
				m_wCurrentBanker=INVALID_CHAIR;

				//轮换判断
				TakeTurns();

				bChangeBanker=true;
				m_bExchangeBanker = true;

				//提示消息
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]分数少于(%.2lf)，强行换庄!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
            		_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"),pIServerUserItem->GetNickName(),m_lPlayerBankerMAX);
				
				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
			}
		}
	}
	//系统做庄
	else if (m_wCurrentBanker == INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//轮换判断
		TakeTurns();

		bChangeBanker=true;
		m_bExchangeBanker = true;
	}

	//切换判断
	if (bChangeBanker)
	{
		//最大坐庄数
		m_lPlayerBankerMAX = m_nBankerTimeLimit;

		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//发送消息
		CMD_S_ChangeBanker stChangeBanker;
		ZeroMemory(&stChangeBanker,sizeof(stChangeBanker));
		stChangeBanker.wBankerUser = m_wCurrentBanker;
		if (m_wCurrentBanker != INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			stChangeBanker.lBankerScore = pIServerUserItem->GetUserScore();
		}
		else
		{
			stChangeBanker.lBankerScore = 100000000;
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&stChangeBanker,sizeof(CMD_S_ChangeBanker));

		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			//获取参数
			m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
			ASSERT(m_pGameServiceOption!=NULL);

			//读取消息
			int nMessageCount = 3;
			
			//读取配置
			INT nIndex=rand()%nMessageCount;
			TCHAR szMessage1[256],szMessage2[256];
			tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomConfig);
			if(0 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
			}
			else if(1 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
			}
			else if(2 == nIndex)
			{
				CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
			}

			//获取玩家
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

			//发送消息
			_sntprintf(szMessage2,CountArray(szMessage2),TEXT("【 %s 】上庄了，%s"),pIServerUserItem->GetNickName(), szMessage1);
			SendGameMessage(INVALID_CHAIR,szMessage2);
		}
	}

	return bChangeBanker;
}

//轮换判断
void CTableFrameSink::TakeTurns()
{
	//变量定义
	int nInvalidApply = 0;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
		{
			//获取分数
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker=m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//发送消息
					CMD_S_CancelBanker CancelBanker = {};

					//设置变量
					CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

					//发送消息
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//提示消息
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于你的金币数（%.2lf）少于坐庄必须金币数（%.2lf）,你无法上庄！"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//删除玩家
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
}

//发送庄家
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//庄家判断
		if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//切换庄家
	if (wChairID == m_wCurrentBanker) ChangeBanker(true);

	//取消申请
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		//设置变量
		CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//最大下注
bool CTableFrameSink::GetMaxPlayerScore( BYTE cbBetArea, WORD wChairID,SCORE lBetScore )
{
		//获取玩家
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;

	//已下注额
	SCORE lNowJetton = 0;
	ASSERT(AREA_MAX<=CountArray(m_lAllBet));
	if ((CountArray(m_lAllBet)>AREA_MAX)) return false;
	
	//庄家金币
	SCORE lBankerScore=2147483647;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}
	
	//for (int nAreaIndex=0; nAreaIndex<AREA_MAX; ++nAreaIndex) 
	//{
	//	lBankerScore-=m_lAllBet[nAreaIndex];
	//	lNowJetton += m_lPlayBet[wChairID][nAreaIndex];
	//}

	SCORE lBetCount = 0;
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		lBetCount += m_lPlayBet[wChairID][i];
	}
	
	SCORE lAllBetCount=0L;
	for (int nAreaIndex=0; nAreaIndex < AREA_MAX; ++nAreaIndex)
	{
		lAllBetCount += m_lAllBet[nAreaIndex];
	}

		//区域倍率
	BYTE cbMultiple[AREA_WIN_MAX] = {MULTIPLE_BLACK, MULTIPLE_ADD, MULTIPLE_RED, 
									MULTIPLE_BAOZI, MULTIPLE_TONGHUASHUN, MULTIPLE_JINHUA, MULTIPLE_SHUNZI, MULTIPLE_DUIZI};

	SCORE lScore = 0;
	for(int i = 0;i<3;i++)
	{
		if(i != 1)
		{
			for(int j = 3;j<AREA_MAX;j++)
			{
				if(j != cbBetArea)
				{
					//当开这个区域需要赔付金币
					SCORE lBKScore = m_lAllBet[i] * cbMultiple[i] +   (m_lAllBet[cbBetArea] + lBetScore) *  cbMultiple[cbBetArea] + m_lAllBet[j]*  cbMultiple[j];
					//这是庄家赚的金币
					SCORE lBankerRemain = lAllBetCount - m_lAllBet[i] - m_lAllBet[cbBetArea] - m_lAllBet[j];
					if(lBankerScore + lBankerRemain - lBKScore < 0)return false;
				}
			}
		}
		else
		{
			//当开这个区域需要赔付金币
			SCORE lBKScore = m_lAllBet[i] * cbMultiple[i] +   (m_lAllBet[cbBetArea] + lBetScore) *  cbMultiple[cbBetArea] ;
			//这是庄家赚的金币
			SCORE lBankerRemain = lAllBetCount - m_lAllBet[cbBetArea];
			if(lBankerScore + lBankerRemain - lBKScore < 0)return false;
		}
	}

	return true;

	////最大下注
	SCORE lMaxBet = 0L;

	////最大下注
	//lMaxBet = min(pIMeServerUserItem->GetUserScore() - lNowBet, m_lUserLimitScore - m_lPlayBet[wChairID][cbBetArea]);

	//lMaxBet = min( lMaxBet, m_lAreaLimitScore - m_lAllBet[cbBetArea]);

	//lMaxBet = min( lMaxBet, lBankerScore / (cbMultiple[cbBetArea] - 1));

	////非零限制
	//ASSERT(lMaxBet >= 0);
	//lMaxBet = max(lMaxBet, 0);

	////庄闲对等
	//if ( cbBetArea == AREA_XIAN && (m_lAllBet[AREA_ZHUANG] - m_lAllBet[AREA_XIAN]) && lMaxBet < (m_lAllBet[AREA_ZHUANG] - m_lAllBet[AREA_XIAN]) )
	//	lMaxBet = m_lAllBet[AREA_ZHUANG] - m_lAllBet[AREA_XIAN];
	//else if( cbBetArea == AREA_ZHUANG && (m_lAllBet[AREA_XIAN] - m_lAllBet[AREA_ZHUANG]) && lMaxBet < (m_lAllBet[AREA_XIAN] - m_lAllBet[AREA_ZHUANG]) )
	//	lMaxBet = m_lAllBet[AREA_XIAN] - m_lAllBet[AREA_ZHUANG];

	return lMaxBet;
}


//计算得分
bool CTableFrameSink::CalculateScore( OUT SCORE& lBankerWinScore, OUT tagServerGameRecord& GameRecord ,BYTE cbDispatchCount)
{
	m_bControl = false;
	if ( m_pServerContro && m_pServerContro->NeedControl() && m_pServerContro->ControlResult(m_cbTableCardArray[0], m_cbCardCount))
	{
		m_bControl = true;
	}

	//计算牌型
	//BYTE cbPlayerCount = m_GameLogic.GetCardType( m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER] );
	//BYTE cbBankerCount = m_GameLogic.GetCardType( m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER] );

	//系统输赢
	SCORE lSystemScore = 0l;
	ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
	//推断玩家
	BYTE cbWinArea[AREA_WIN_MAX] = {FALSE};
	BYTE cbWinType = m_GameLogic.CompareCard(m_cbTableCardArray[0],MAX_COUNT,m_cbTableCardArray[1],MAX_COUNT, cbWinArea);
	CopyMemory(m_cbWinArea,cbWinArea,sizeof(cbWinArea));
	//游戏记录
	GameRecord.cbWinType = cbWinType;
	if (cbWinType == CARD_DUIZI && cbWinArea[CARD_DUIZI])
		GameRecord.cbWinType = CARD_DUIZI * 10;
	GameRecord.cbGameResult = cbWinArea[0] == true ? 0 : 2;

	//玩家成绩
	SCORE lUserLostScore[GAME_PLAYER];
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));



	//区域倍率
	BYTE cbMultiple[AREA_WIN_MAX] = {MULTIPLE_BLACK, MULTIPLE_ADD, MULTIPLE_RED, 
		MULTIPLE_BAOZI, MULTIPLE_TONGHUASHUN, MULTIPLE_JINHUA, MULTIPLE_SHUNZI, MULTIPLE_DUIZI};

	//庄家是不是机器人
	bool bIsBankerAndroidUser = true;
	if ( m_wCurrentBanker != INVALID_CHAIR )
	{
		IServerUserItem * pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		if (pIBankerUserItem != NULL) 
		{
			bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();
		}
	}

	//计算积分
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if ( m_wCurrentBanker == wChairID ) continue;

		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();
		for ( WORD wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex )
		{
			if ( cbWinArea[wAreaIndex] == TRUE )
			{
				BYTE cbMultipleArea = wAreaIndex;
				if (wAreaIndex == AREA_SPECIAL_INDEX)
				{
					for (WORD smallwAreaIndex = 3; smallwAreaIndex< AREA_WIN_MAX; ++smallwAreaIndex)
					{
						if (cbWinArea[smallwAreaIndex]) {
							cbMultipleArea = smallwAreaIndex;
							break;
						}
					}
				}
				m_lUserWinScore[wChairID] += (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));
				m_lPlayScore[wChairID][wAreaIndex] += (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));
				lBankerWinScore -= (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));

				//系统得分
				if(bIsAndroidUser && bIsBankerAndroidUser == FALSE)
					lSystemScore += (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));
				if(!bIsAndroidUser && bIsBankerAndroidUser == TRUE)
					lSystemScore -= (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));
				//庄家得分
				if ( m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker != wChairID )
					m_lPlayScore[m_wCurrentBanker][wAreaIndex] -= (m_lPlayBet[wChairID][wAreaIndex] * (cbMultiple[cbMultipleArea] - 1));
			}
			else
			{
				lUserLostScore[wChairID] -= m_lPlayBet[wChairID][wAreaIndex];
				m_lPlayScore[wChairID][wAreaIndex] -= m_lPlayBet[wChairID][wAreaIndex];
				lBankerWinScore += m_lPlayBet[wChairID][wAreaIndex];

				//系统得分
				if(bIsAndroidUser && bIsBankerAndroidUser == FALSE)
					lSystemScore -= m_lPlayBet[wChairID][wAreaIndex];
				if(!bIsAndroidUser && bIsBankerAndroidUser == TRUE)
					lSystemScore += m_lPlayBet[wChairID][wAreaIndex];

				//庄家得分
				if ( m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker != wChairID )
					m_lPlayScore[m_wCurrentBanker][wAreaIndex] += m_lPlayBet[wChairID][wAreaIndex];
			}

		}
		
		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];

		//计算税收
		if ( m_lUserWinScore[wChairID] > 0 )
		{
			SCORE fRevenuePer=m_pGameServiceOption->wRevenueRatio;
			m_lUserRevenue[wChairID] = (SCORE) ((LONGLONG((( m_lUserWinScore[wChairID] ))*fRevenuePer/1000*100))/100.0);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}
	}

	//庄家成绩
	if (m_wCurrentBanker != INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			SCORE fRevenuePer=m_pGameServiceOption->wRevenueRatio;
			m_lUserRevenue[m_wCurrentBanker] = (SCORE) ((LONGLONG((( m_lUserWinScore[m_wCurrentBanker] ))*fRevenuePer/1000*100))/100.0);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}
	
	SCORE lTempDeduct=0;
	lTempDeduct=m_lStorageDeduct;

	//特殊控制
	if ( m_bControl )
	{
		return true; 
	}

	//系统分值计算	
	if ((lSystemScore + m_lStorageCurrent) < -0.001 && (cbDispatchCount < 10 || lSystemScore < -0.001))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//游戏结束计算
SCORE CTableFrameSink::GameOver()
{
	//定义变量
	SCORE lBankerWinScore = 0l;
	bool bSuccess = false;

	//游戏记录
	tagServerGameRecord& GameRecord = m_GameRecordArrary[m_nRecordLast];

	////计算分数
	BYTE cbDispatchCount = 0;
	do 
	{
		m_nStorageCount++;
		//派发扑克
		DispatchTableCard();

		//试探性判断
		lBankerWinScore = 0l;
		bSuccess = CalculateScore( lBankerWinScore, GameRecord ,cbDispatchCount);
		cbDispatchCount ++;

	} while (!bSuccess);

	////派发扑克
	//DispatchTableCard();
	//lBankerWinScore = 0l;
	//CalculateScore( lBankerWinScore, GameRecord ,cbDispatchCount);

	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore = lBankerWinScore;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	return lBankerWinScore;
}

//推断赢家
void CTableFrameSink::DeduceWinner(BYTE* pWinArea)
 {
}


//发送记录
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize=0;
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	int nIndex = m_nRecordFirst;
	while ( nIndex != m_nRecordLast )
	{
		if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
			wBufferSize=0;
		}
		CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		wBufferSize+=sizeof(tagServerGameRecord);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
}

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID == INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//旁观玩家
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem == NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
	//获取自定义配置
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//上庄
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE)?true:false;
	
	//时间
	m_cbFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_cbBetTime = pCustomConfig->CustomGeneral.lBetTime;
	m_cbEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if(m_cbFreeTime < TIME_FREE	|| m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
	if(m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
	if(m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

	//下注
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;

	//库存
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	m_lStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
	if(m_lStorageMul1 < 0 || m_lStorageMul1 > 100 ) m_lStorageMul1 = 50;
	if(m_lStorageMul2 < 0 || m_lStorageMul2 > 100 ) m_lStorageMul2 = 80;
	
	//机器人
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand()%(lRobotBetMaxCount-lRobotBetMinCount+1) + lRobotBetMinCount;
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 8;
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;

	TCHAR OutBuf[255];
	m_OccUpYesAtconfig.occupyseatType = GetPrivateProfileInt(m_szGameRoomName, TEXT("occupyseatType"), 0, m_szConfigFileName);
	if(m_OccUpYesAtconfig.occupyseatType == 0)AfxMessageBox(_T("244"));
	m_OccUpYesAtconfig.enVipIndex = GetPrivateProfileInt(m_szGameRoomName, TEXT("enVipIndex"), 0, m_szConfigFileName);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("lOccupySeatConsume"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lOccupySeatConsume);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("lOccupySeatFree"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lOccupySeatFree);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("lForceStandUpCondition"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lForceStandUpCondition);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("lBottomPourImpose"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	m_lBottomPourImpose = _tstof(OutBuf);

		//控制库存
  	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("IsOpen"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_IsOpen = _tstol(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lStorageCurrent = _tstof(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageDeduct"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lStorageDeduct = _tstof(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lAttenuationScore = _tstof(OutBuf);

	return;
}

// 添加逗号
CString CTableFrameSink::AddComma( SCORE lScore )
{
	CString strScore;
	CString strReturn;
	SCORE lNumber = lScore;
	if ( lScore < 0 )
		lNumber = -lNumber;

	strScore.Format(TEXT("%.2lf"), lNumber);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%3) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}

	if ( lScore < 0 )
		strReturn.Insert(0, '-');

	return strReturn;
}
//////////////////////////////////////////////////////////////////////////

bool CTableFrameSink::OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer)
{
	//如果不具有管理员权限 则返回错误
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
		return false;
	
	return true;
}

//咨询玩家
bool CTableFrameSink::OnEventQueryChargeable( IServerUserItem *pIServerUserItem, bool bLookonUser )
{
	if ( bLookonUser )
		return false;

	return true;
}

//咨询服务费
SCORE CTableFrameSink::OnEventQueryCharge( IServerUserItem *pIServerUserItem )
{
	if( m_lUserWinScore[pIServerUserItem->GetChairID()] != 0 )
		return m_nServiceCharge;

	return 0;
}	

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge( WORD wChairID )
{
	// 庄家判断
	if ( wChairID == m_wCurrentBanker )
	{
		for ( int i = 0; i < GAME_PLAYER; ++i )
		{
			for ( int j = 0; j < AREA_MAX; ++j )
			{
				if ( m_lPlayBet[i][j] != 0 )
					return true;
			}
		}
		return false;
	}

	// 一般玩家
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		if ( m_lPlayBet[wChairID][i] != 0 )
			return true;
	}

	return false;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//当庄家分数在空闲时间变动时(即庄家进行了存取款)校验庄家的上庄条件
	if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE)
	{
		ChangeBanker(false);
	}
	
	if(m_pITableFrame->GetGameStatus()==GAME_SCENE_BET)
	{
		CMD_S_UserScoreNotify UserScoreNotify;
		ZeroMemory(&UserScoreNotify, sizeof(UserScoreNotify));
		
		UserScoreNotify.wChairID = wChairID;
		UserScoreNotify.lPlayBetScore = min(pIServerUserItem->GetUserScore() - m_nServiceCharge,m_lUserLimitScore);

		m_pITableFrame->SendTableData(wChairID,SUB_S_USER_SCORE_NOTIFY,&UserScoreNotify,sizeof(UserScoreNotify));
	}

	return true;
}

//是否衰减
bool CTableFrameSink::NeedDeductStorage()
{

	if(m_lStorageCurrent <=0 ) return false;

	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL ) continue; 

		if(!pIServerUserItem->IsAndroidUser())
		{
			for (int nAreaIndex=0; nAreaIndex<=AREA_MAX; ++nAreaIndex) 
			{
				if (m_lPlayBet[i][nAreaIndex]!=0)
				{
					return true;
				}				
			}			
		}
	}

	return false;

}

//发送下注信息
void CTableFrameSink::SendUserBetInfo( IServerUserItem *pIServerUserItem )
{
	if(NULL == pIServerUserItem) return;

	//权限判断
	if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())) return;

	//构造数据
	CMD_S_SendUserBetInfo SendUserBetInfo;
	ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));
	
	CopyMemory(&SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
	CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lPlayBet, sizeof(m_lPlayBet));

	//发送消息	
	m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));
	
	return;
}

//测试写信息
void CTableFrameSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//设置语言区域
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );
}

////////////////////////////////////////////////////////////////////////////////////////////