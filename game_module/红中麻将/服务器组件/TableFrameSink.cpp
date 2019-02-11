#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

#define IDI_CHECK_TABLE				1					//检查桌子
#define IDI_START_GAME				2					//开始定时器
#define IDI_OUT_CARD				3					//出牌定时器
#define IDI_OPERATE_CARD			4					//操作定时器
#define IDI_OVER_TIME				5					//超时带打

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_wPlayerCount = GAME_PLAYER;

	//组件变量
	m_pITableFrame = NULL;
	m_pGameCustomRule = NULL;
	m_pGameServiceOption = NULL;
	m_pGameServiceAttrib = NULL;	

	diFenUnit = 1;

	//游戏变量
	m_wSiceCount = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wLianZhuang = 1;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbMagicIndex = MAX_INDEX;
	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

	//运行变量
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard = 0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));

	//状态变量
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	//出牌信息
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//出牌信息
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = MAX_REPERTORY;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;

	//扑克信息
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_cbUserCatchCardCount,sizeof(m_cbUserCatchCardCount));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));
	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	m_wLastCatchCardUser=INVALID_CHAIR;

	//规则变量
	m_cbMaCount=0;
	m_cbPlayerCount=0;

	m_cbJiePao = FALSE;

#ifdef  CARD_DISPATCHER_CONTROL
	m_cbControlGameCount = 0;
#endif

	m_pC = NULL;
	m_hC = LoadLibrary(TEXT("CMj.dll"));
	if (m_hC)
	{
		typedef void * (*CREATE)();
		CREATE pFunc = (CREATE)GetProcAddress(m_hC, "CreateCGame");
		if (pFunc)
		{
			m_pC = static_cast<ICGame*>(pFunc());
		}
		else
		{
			//CTraceService::TraceString(_TEXT("cerr"));
		}
	}


	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
	if (m_pC)
	{
		delete m_pC;
		m_pC = NULL;
	}

	if (m_hC)
	{
		FreeLibrary(m_hC);
		m_hC = NULL;
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
	m_wSiceCount = 0;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbMagicIndex = MAX_INDEX;
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = MAX_REPERTORY;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;

	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

	//运行变量
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//状态变量
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));	
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		m_vecEnjoinChiHu[i].clear();
		m_vecEnjoinChiPeng[i].clear();	
	}

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));
	
	//出牌信息
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//扑克信息
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbUserCatchCardCount,sizeof(m_cbUserCatchCardCount));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));


	m_wLastCatchCardUser=INVALID_CHAIR;

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));
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

#ifdef CARD_DISPATCHER_CONTROL
	if(!m_pITableFrame->GetTableID())
		CTraceService::TraceString(TraceLevel_Exception,L"红中麻将配牌器功能开启,数据文件为SparrowData.dat");
#endif

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_cbMaCount = m_pGameCustomRule->cbMaCount;
	m_cbPlayerCount = m_pGameCustomRule->cbPlayerCount;

	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	//开始模式
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
		m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
	else
		m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

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
	//金币游戏
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		return 64 * m_pITableFrame->GetCellScore();
	}
	return 0;
}

void CTableFrameSink::OnPrivateTableEnded(int flag)
{
	m_TableEnded = true;
}
void CTableFrameSink::OnPrivateTableCreated()
{
	m_PlayCount = 0;
	m_TableEnded = false;

	m_HistoryScore.ResetData();
	m_wBankerUser = INVALID_CHAIR;
	m_wLianZhuang = 1;

	BYTE* rule = m_pITableFrame->GetGameRule();
	ASSERT(rule[0] == 1);
	m_cbPlayerCount = rule[1];
	m_cbMaCount = rule[2];
	m_cbJiePao = rule[3];
	m_cbQiangGang = rule[4];
	m_cbQiXiaoDui = rule[5];

	diFenUnit = rule[98]*10;

	m_GameLogic.m_cbQiXiaoDui = m_cbQiXiaoDui;
}

void CTableFrameSink::PrintCards()
{
	return;
	strInfo.Format(TEXT("发牌库存:"));
	//GameLog(strInfo);

	for (int n = 0; n < MAX_REPERTORY; n++)
	{
		strInfo.Format(TEXT("card[%d]:0x%02x"),n, m_cbRepertoryCard[n]);
		//GameLog(strInfo);
	}



	strInfo.Format(TEXT("发牌库存统计:"));
	//GameLog(strInfo);

	BYTE cards[3 * 9 + 1];

	int pos = 0;
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			cards[pos++] = (i << 4) + j;
		}
	}
	cards[27] = 0x35;

	for (int i = 0; i < CountArray(cards); i++)
	{
		BYTE value = cards[i];
		int count = 0;
		for (int n = 0; n < MAX_REPERTORY; n++)
		{
			if (m_cbRepertoryCard[n] == value)
			{
				count++;
			}
		}

		strInfo.Format(TEXT("card:0x%02x, %d个"), value, count);
		//GameLog(strInfo);
	}


}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	m_PlayCount++;

	m_TableEnded = false;

	//mGameRecordStream.resetValues();
	//GetLocalTime(&mGameRecordStream.timeStart); 
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//随机扑克
	BYTE Sice1 = rand()%6 + 1;
	BYTE Sice2 = rand()%6 + 1;
	BYTE minSice = min(Sice1,Sice2);
	m_wSiceCount = MAKEWORD(Sice1,Sice2);
	m_cbLeftCardCount=MAX_REPERTORY;
	m_cbSendCardCount = 0;
	m_bUserActionDone=false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;


	bool ok = false;
	/*if (m_pC)
	{
		ok = m_pC->CCards(m_pITableFrame, this);
	}*/

	if (!ok)
	{
		//备份发牌库存
		BYTE repoLen = 0;
		BYTE repoCards[CountArray(m_GameLogic.m_cbCardDataArray)];
		int MAX_REPO = repoLen = CountArray(repoCards);
		CopyMemory(repoCards, m_GameLogic.m_cbCardDataArray, repoLen);
		m_GameLogic.GetCards(&m_cbRepertoryCard[0], repoLen, repoCards, repoLen);
	}

	//m_GameLogic.RandCardList(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

#ifdef  CARD_DISPATCHER_CONTROL

	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\SparrowData.dat"), szPath);
	//非控制状态
	if(m_cbControlGameCount==0)
	{
		CFile file;
		if (file.Open(strSaveFileName, CFile::modeRead))
		{
			file.Read(&m_wBankerUser,sizeof(m_wBankerUser));
			file.Read(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));		
			file.Close();
		}		
	}
	else
	{
		m_wBankerUser = m_wControBankerUser;
		CopyMemory(m_cbRepertoryCard, m_cbControlRepertoryCard, sizeof(m_cbRepertoryCard));
		m_cbControlGameCount--;
	}

#endif

	//const BYTE cbCardDataArray[MAX_REPERTORY]=
	//{
	//	0x35, 0x15, 0x35, 0x01, 0x24, 0x02, 0x04, 0x08, 0x09,					
	//	0x01, 0x14, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x28,					
	//	0x01, 0x02, 0x03, 0x04, 0x05, 0x09, 0x26, 0x27, 0x29,					
	//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x11, 0x12, 0x13, 0x17, 0x18, 0x19, 0x16, 0x06, 0x07,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x21, 0x22, 0x23, 0x02, 0x25, 0x06, 0x07, 0x08, 0x05,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x03, 0x35, 0x05, 0x35													
	//};

	//for (BYTE i = 0; i < MAX_REPERTORY; i++)
	//{
	//	m_cbRepertoryCard[i] = cbCardDataArray[i];
	//}

	//红中可以当财神
 	m_cbMagicIndex = m_GameLogic.SwitchToCardIndex(0x35);
 	m_GameLogic.SetMagicIndex(m_cbMagicIndex);

	//分发扑克
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		if(m_pITableFrame->GetTableUserItem(i) == NULL)
			continue;

		m_bPlayStatus[i]=true;
		m_cbLeftCardCount -= (MAX_COUNT - 1);
		m_cbMinusHeadCount += (MAX_COUNT - 1);
		m_cbHandCardCount[i] = (MAX_COUNT - 1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount], MAX_COUNT - 1, m_cbCardIndex[i]);
	}

	if(m_wBankerUser == INVALID_CHAIR && (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0 )//房卡模式下先把庄家给房主
	{
		DWORD OwnerId =  m_pITableFrame->GetTableOwner();
		IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
		if (pOwnerItem != NULL && pOwnerItem->GetChairID() != INVALID_CHAIR)
			m_wBankerUser = pOwnerItem->GetChairID();
	}
	//确定庄家
	if(m_wBankerUser == INVALID_CHAIR || m_bPlayStatus[m_wBankerUser]==false)
	{
		m_wBankerUser = rand() % m_cbPlayerCount;
		while(m_pITableFrame->GetTableUserItem(m_wBankerUser)==NULL)
		{
			m_wBankerUser = (m_wBankerUser+1)%m_cbPlayerCount;
		}
	}

	//发送扑克
	m_cbMinusHeadCount++;
	m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	m_cbUserCatchCardCount[m_wBankerUser]++;
	//设置变量
	m_cbProvideCard = m_cbSendCardData;
	m_wProvideUser = m_wBankerUser;
	m_wCurrentUser = m_wBankerUser;

	//堆立信息
	WORD wSiceCount = LOBYTE(m_wSiceCount) + HIBYTE(m_wSiceCount);
	WORD wTakeChairID = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
	BYTE cbTakeCount = MAX_REPERTORY - m_cbLeftCardCount;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//计算数目
		BYTE cbValidCount = HEAP_FULL_COUNT - m_cbHeapCardInfo[wTakeChairID][1] - ((i == 0) ? (minSice) * 2 : 0);
		BYTE cbRemoveCount = __min(cbValidCount, cbTakeCount);

		//提取扑克
		cbTakeCount -= cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i == 0) ? 1 : 0] += cbRemoveCount;

		//完成判断
		if (cbTakeCount == 0)
		{
			m_wHeapHead = wTakeChairID;
			m_wHeapTail = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
			break;
		}
		//切换索引
		wTakeChairID = (wTakeChairID + GAME_PLAYER - 1) % GAME_PLAYER;
	}

	//操作分析
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));

	//杠牌判断
	tagGangCardResult GangCardResult;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[m_wBankerUser],NULL,0,0,GangCardResult);

	//胡牌判断
	CChiHuRight chr;
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wBankerUser],NULL,0,m_cbSendCardData,chr,true);
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	m_cbHandCardCount[m_wBankerUser]++;


	//用户椅子座位信息
	CMD_S_GameUserInfo initUserInfo;
	ZeroMemory(&initUserInfo, sizeof(initUserInfo));
	for (int i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL)
		{
			continue;
		}

		initUserInfo.userId[i] = pServerUserItem->GetUserID();
	}
	addGameRecord(INVALID_CHAIR, SUB_S_INIT_USERINFO, &initUserInfo, sizeof(initUserInfo));


	//听牌判断
	BYTE cbCount = 0;
	BYTE cbOutCard[MAX_COUNT][28] = {0};
	CMD_S_Hu_Data HuData;
	ZeroMemory(&HuData,sizeof(HuData));
	if(m_bTing[m_wBankerUser] == false)
	{
		cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wBankerUser],0,0,HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
		if(cbCount >0)
		{
			m_cbUserAction[m_wBankerUser] |= WIK_LISTEN;
			for(int i=0;i<MAX_COUNT;i++)
			{
				if(HuData.cbHuCardCount[i]>0)
				{
					for(int j=0;j<HuData.cbHuCardCount[i];j++)
					{
						HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(m_wBankerUser,HuData.cbHuCardData[i][j]);
					}
				}
				else
					break;
			}
			//addGameRecord(m_wBankerUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
			m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
		}
	}
	
	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wBankerUser = m_wBankerUser;
	GameStart.wSiceCount = m_wSiceCount;
	GameStart.wHeapHead = m_wHeapHead;
	GameStart.wHeapTail = m_wHeapTail;
	GameStart.cbMagicIndex = m_cbMagicIndex;
	CopyMemory(GameStart.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(GameStart.cbHeapCardInfo));

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		GameStart.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
		GameStart.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
	}

	//发送数据
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i) == NULL)
			continue;
		GameStart.cbUserAction = m_cbUserAction[i];
		ZeroMemory(GameStart.cbCardData, sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameStart.cbCardData);

		GameStart.cbOutCardCount=0;
		ZeroMemory(GameStart.cbOutCardData, sizeof(GameStart.cbOutCardData));
		if(i == m_wBankerUser && cbCount>0)
		{
			GameStart.cbOutCardCount=cbCount;
			CopyMemory(GameStart.cbOutCardData,cbOutCard,sizeof(cbOutCard));
		}

		addGameRecord(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));		
	}

	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) != 0)
	{
		m_pITableFrame->SetGameTimer(IDI_CHECK_TABLE, 30000, -1, NULL);
		m_cbWaitTime = 0;
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	//没有动态加入，就是创建房间指定的人数
	//mGameRecordStream.userCount = m_cbPlayerCount;

	return true;
}

void CTableFrameSink::SetLeftCards(CMD_S_GameConclude &conclude)
{

	strInfo.Format(TEXT("结局发送底牌:%d张"), m_cbLeftCardCount);
	//GameLog(strInfo);


	for (int i = 0; i<m_cbLeftCardCount; i++)
	{
		conclude.cbLeftCardData[i] = m_cbRepertoryCard[m_cbMinusLastCount + i];

		//strInfo.Format(TEXT("card[%d]:0x%02x"), (m_cbMinusLastCount + i), m_cbRepertoryCard[m_cbMinusLastCount + i]);
		////GameLog(strInfo);
	}
	conclude.cbLeftCardCount = m_cbLeftCardCount;

}
//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		m_pITableFrame->KillGameTimer(IDI_CHECK_TABLE);
	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);

	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	{

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.cbSendCardData = m_cbSendCardData;
			GameConclude.wLeftUser = INVALID_CHAIR;			

			//结束信息
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				GameConclude.dwChiHuKind[i] = m_dwChiHuKind[i];
				//权位过滤
				if(m_dwChiHuKind[i] == WIK_CHI_HU)
				{
					FiltrateRight(i, m_ChiHuRight[i]);
					m_ChiHuRight[i].GetRightData(GameConclude.dwChiHuRight[i], MAX_RIGHT_COUNT);
				}
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
			}

			int nCount = m_cbMaCount;
			//if(nCount>1)
			//	nCount++;


			//（需放在库存，算分也会用到）
			bool reSetMa = false;
			WORD cChair = INVALID_CHAIR;
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (WIK_CHI_HU == m_dwChiHuKind[(m_wBankerUser + i) % GAME_PLAYER])
				{
					IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem((m_wBankerUser + i) % GAME_PLAYER);
					if (pUserItem != NULL)
					{
						cChair = (m_wBankerUser + i) % GAME_PLAYER;
						reSetMa = true;
					}
				}
			}
			
			//计算胡牌输赢分
			SCORE lUserGameScore[GAME_PLAYER] = { 0 };
			CalHuPaiScore(lUserGameScore);

			//拷贝码数据
			CopyMemory(GameConclude.cbMaCount, m_cbUserMaCount, sizeof(m_cbUserMaCount));

			for(int i=0;i<nCount;i++)
			{
				GameConclude.cbMaData[i] = m_cbRepertoryCard[m_cbMinusLastCount+i];
			}

			//积分变量
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			GameConclude.wProvideUser = m_wProvideUser;
			GameConclude.cbProvideCard = m_cbProvideCard;

			//统计积分
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				if(!m_bPlayStatus[i])
					continue;

				GameConclude.lGameScore[i] = lUserGameScore[i];
				//胡牌分算完后再加上杠的输赢分就是玩家本轮最终输赢分
				GameConclude.lGameScore[i] += m_lUserGangScore[i];
				GameConclude.lGangScore[i] = m_lUserGangScore[i];

				//收税
				if (GameConclude.lGameScore[i]>0 && (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
				{
					GameConclude.lRevenue[i] = m_pITableFrame->CalculateRevenue(i,GameConclude.lGameScore[i]);
					GameConclude.lGameScore[i] -= GameConclude.lRevenue[i];
				}

				ScoreInfoArray[i].lRevenue = GameConclude.lRevenue[i];
				ScoreInfoArray[i].lScore = GameConclude.lGameScore[i];
				ScoreInfoArray[i].cbType = ScoreInfoArray[i].lScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

				//历史积分
				m_HistoryScore.OnEventUserScore(i, GameConclude.lGameScore[i]);
				if(m_stRecord.nCount<32)
				{
					m_stRecord.lDetailScore[i][m_stRecord.nCount]=GameConclude.lGameScore[i];
					m_stRecord.lAllScore[i] += GameConclude.lGameScore[i];
				}


				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);

			}
			m_stRecord.nCount++;

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
			{
				addGameRecord(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));
			}


			for (int i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
				GameConclude.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
			}

			SetLeftCards(GameConclude);

			addGameRecord(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray, m_cbPlayerCount);

			//结束游戏
			//TODO:写入本局记录
			//GetLocalTime(&mGameRecordStream.timeEnd);
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			//mGameRecordStream.resetValues();

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
			{
				//解散房间、正常约局结束，都要清除胡牌次数
				if (m_pITableFrame->IsPersonalRoomDisumme() || m_TableEnded )
				{
					ZeroMemory(&m_stRecord,sizeof(m_stRecord));
				}
			}
			
			return true;
		}
	case GER_NETWORK_ERROR:	//网络中断
		{
			return true;
		}
	case GER_USER_LEAVE:	//用户强退
		{
			
			//自动托管
			OnUserTrustee(wChairID,true);

			return true;
		}
	case GER_DISMISS:		//游戏解散
	{

			//变量定义
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));

			m_wBankerUser = INVALID_CHAIR;

			GameConclude.cbSendCardData = m_cbSendCardData;

			//用户扑克
			BYTE cbCardIndex=0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
			}

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
				GameConclude.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
			}

			SetLeftCards(GameConclude);
			//发送信息
			addGameRecord(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));


			//结束游戏
			//TODO:写入本局记录??
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
			{
				if(m_pITableFrame->IsPersonalRoomDisumme() )//当前朋友局解散清理记录
				{
					ZeroMemory(&m_stRecord,sizeof(m_stRecord));
				}
			}
			
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
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			StatusFree.cbMaCount = m_cbMaCount;
			StatusFree.cbPlayerCount = m_cbPlayerCount;
			//自定规则
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;

			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			//自定规则
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusPlay.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			
			OnUserTrustee(wChairID,false);//重入取消托管

			//规则
			StatusPlay.cbMaCount = m_cbMaCount;
			StatusPlay.cbPlayerCount = m_cbPlayerCount;
			//游戏变量
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.lCellScore = m_pGameServiceOption->lCellScore;
			StatusPlay.cbMagicIndex = m_cbMagicIndex;
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(m_bTrustee));

			//状态变量
			StatusPlay.cbActionCard = m_cbProvideCard;
			StatusPlay.cbLeftCardCount = m_cbLeftCardCount;
			StatusPlay.cbActionMask = !m_bResponse[wChairID] ? m_cbUserAction[wChairID] : WIK_NULL;
			
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(StatusPlay.bTrustee));
			CopyMemory(StatusPlay.bTing, m_bTing, sizeof(StatusPlay.bTing));
			//当前能胡的牌
			StatusPlay.cbOutCardCount = m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],StatusPlay.cbOutCardCount,StatusPlay.cbOutCardDataEx,StatusPlay.cbHuCardCount,StatusPlay.cbHuCardData); 

			//历史记录
			StatusPlay.wOutCardUser = m_wOutCardUser;
			StatusPlay.cbOutCardData = m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard, m_cbDiscardCard, sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount, m_cbDiscardCount, sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray, m_WeaveItemArray, sizeof(StatusPlay.WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveItemCount, m_cbWeaveItemCount, sizeof(StatusPlay.cbWeaveItemCount));

			//堆立信息
			StatusPlay.wHeapHead = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

			//扑克数据
			for(int i = 0; i < GAME_PLAYER; i++)
			{
				StatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}
			m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID], StatusPlay.cbCardData);
			StatusPlay.cbSendCardData = (m_wCurrentUser == wChairID) ? m_cbSendCardData : 0x00;

			//历史积分
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_CHECK_TABLE)
		{
			m_cbWaitTime++;
			if(m_cbWaitTime>=3)
			{
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			else if(m_cbWaitTime>=2)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pServerUserItem!=NULL && pServerUserItem->GetUserStatus()==US_OFFLINE)return true;				
				}
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			return true;
		}
	}
	//托管出牌代打
	if(wTimerID == IDI_OUT_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		BYTE card = m_cbSendCardData;
		if (!m_GameLogic.IsValidCard(card) || m_GameLogic.IsMagicCard(card))
		{
			for(int i=0;i<MAX_INDEX;i++)
			{
				if(m_cbCardIndex[m_wCurrentUser][i]>0)
				{
					card = m_GameLogic.SwitchToCardData(i);

					if (!m_GameLogic.IsMagicCard(card))
						break;
				}
			}
		}

		OnUserOutCard(m_wCurrentUser,card,true);
		return true;
	}
	//超时代打
	else if (wTimerID == IDI_OVER_TIME)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		if (m_wCurrentUser == INVALID_CHAIR)
		{
			//发送提示
			for (WORD i = 0; i<m_cbPlayerCount; i++)
			{
				if (m_cbUserAction[i] != WIK_NULL)
				{
					BYTE cbOperateCard[3] = { 0 };
					if (m_cbUserAction[i] != WIK_LISTEN)
						OnUserOperateCard(i, WIK_NULL, cbOperateCard);
					else
						OnUserListenCard(i, false);
				}
			}
		}
		else
		{
			BYTE card = m_cbSendCardData;
			if (!m_GameLogic.IsValidCard(card) || m_GameLogic.IsMagicCard(card))
			{
				for (int i = 0; i<MAX_INDEX; i++)
				{
					if (m_cbCardIndex[m_wCurrentUser][i]>0)
					{
						card = m_GameLogic.SwitchToCardData(i);

						if (!m_GameLogic.IsMagicCard(card))
							break;
					}
				}
			}

			OnUserOutCard(m_wCurrentUser, card, true);
		}
	}
	//托管操作代打
	else if(wTimerID == IDI_OPERATE_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		//发送提示
		for (WORD i = 0; i<m_cbPlayerCount; i++)
		{
			if (m_cbUserAction[i] != WIK_NULL && m_bTrustee[i])
			{
				BYTE cbOperateCard[3] = { 0 };
				if (m_cbUserAction[i] != WIK_LISTEN)
					OnUserOperateCard(i, WIK_NULL, cbOperateCard);
				else
					OnUserListenCard(i, false);
			}
		}
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
	m_cbWaitTime = 0;
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_OutCard));
			if (wDataSize != sizeof(CMD_C_OutCard)) return false;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_OutCard * pOutCard = (CMD_C_OutCard *)pData;
			return OnUserOutCard(pIServerUserItem->GetChairID(), pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_OperateCard));
			if (wDataSize != sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard = (CMD_C_OperateCard *)pData;
			return OnUserOperateCard(pIServerUserItem->GetChairID(), pOperateCard->cbOperateCode, pOperateCard->cbOperateCard);
		}
	case SUB_C_LISTEN_CARD:
		{
			//效验消息
			ASSERT(wDataSize == sizeof(CMD_C_ListenCard));
			if (wDataSize != sizeof(CMD_C_ListenCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//消息处理
			CMD_C_ListenCard * pOperateCard = (CMD_C_ListenCard *)pData;
			return OnUserListenCard(pIServerUserItem->GetChairID(), pOperateCard->bListenCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_REPLACE_CARD:	//用户补牌
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_ReplaceCard));
			if (wDataSize != sizeof(CMD_C_ReplaceCard)) return false;
			//用户效验
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//变量定义
			CMD_C_ReplaceCard * pReplaceCard = (CMD_C_ReplaceCard *)pData;

			//消息处理
			return OnUserReplaceCard(pIServerUserItem->GetChairID(), pReplaceCard->cbCardData);
		}

#ifdef  CARD_DISPATCHER_CONTROL
	case SUB_C_SEND_CARD:
		{
			ASSERT(wDataSize==sizeof(CMD_C_SendCard));
			if(wDataSize!=sizeof(CMD_C_SendCard)) return false;

			CMD_C_SendCard * pSendCard = (CMD_C_SendCard *)pData;

			return OnUserSendCard(pSendCard->cbCardCount, pSendCard->wBankerUser, pSendCard->cbCardData, pSendCard->cbControlGameCount);
		}
#endif

	default:
		break;
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//重入加入房间，会再次执行坐下
	return true;

	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		if (wChairID == INVALID_CHAIR)
		{
			return false;
		}
		m_HistoryScore.OnEventUserEnter(wChairID);
		m_wBankerUser = INVALID_CHAIR;
		m_wLianZhuang = 1;
	}

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//非结局不会执行

	//历史积分
	if (bLookonUser==false)
	{
		ASSERT(wChairID != INVALID_CHAIR);
		if (wChairID == INVALID_CHAIR)
		{
			return false;
		}
		m_HistoryScore.OnEventUserLeave(wChairID);
		m_wBankerUser = INVALID_CHAIR;
		m_wLianZhuang = 1;
	}

	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//房卡模式
	{
		if(m_pITableFrame->IsPersonalRoomDisumme() )//当前朋友局解散清理记录
		{
			ZeroMemory(&m_stRecord,sizeof(m_stRecord));
		}
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut/*=false*/)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) return true;

	//错误断言
	ASSERT(wChairID == m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData));

	//效验参数
	if (wChairID != m_wCurrentUser) return true;
	if (!m_GameLogic.IsValidCard(cbCardData)) return true;

	//不能出红中
	if (m_GameLogic.IsMagicCard(cbCardData))
	{
		BYTE cbHandCardCount = 0;
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			cbHandCardCount += m_cbCardIndex[wChairID][i];
		}
		//除非手上只有红中了
		if (!(m_cbMagicIndex != MAX_INDEX && m_cbCardIndex[wChairID][m_cbMagicIndex] == cbHandCardCount))
			return true;
	}

	//删除扑克
 	if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData))
	{
 		ASSERT(FALSE);
 		return false;
 	}

	//清除禁止
	m_bEnjoinChiHu[wChairID] = false;
	m_bEnjoinChiPeng[wChairID] = false;
	m_vecEnjoinChiPeng[wChairID].clear();
	m_vecEnjoinChiHu[wChairID].clear();

	//设置变量
	m_enSendStatus = OutCard_Send;
	m_cbSendCardData = 0;
	m_cbUserAction[wChairID] = WIK_NULL;
	m_cbPerformAction[wChairID] = WIK_NULL;

	//出牌记录
	m_wOutCardUser = wChairID;
	m_cbOutCardData = cbCardData;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser = wChairID;
	OutCard.cbOutCardData = cbCardData;
	OutCard.bSysOut=bSysOut;
	//发送消息
	addGameRecord(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));

	m_wProvideUser = wChairID;
	m_cbProvideCard = cbCardData;

	//用户切换
	m_wCurrentUser = (wChairID + 1) % m_cbPlayerCount;
	for(int i=0;i<m_cbPlayerCount;i++)
	{
		if(m_bPlayStatus[m_wCurrentUser])
			break;
		m_wCurrentUser = (m_wCurrentUser+1)%m_cbPlayerCount;
	}

	//响应判断
 	bool bAroseAction = EstimateUserRespond(wChairID, cbCardData, EstimatKind_OutCard);
 
	if(m_cbGangStatus != WIK_GANERAL)
	{
		m_bGangOutCard = true;
		m_cbGangStatus = WIK_GANERAL;
		m_wProvideGangUser = INVALID_CHAIR;
	}	
	else
	{
		m_bGangOutCard = false;
	}

 	//派发扑克
 	if (!bAroseAction) 
	{
		DispatchCardData(m_wCurrentUser);
	}
	else
	{
		m_bUserActionDone = true;
		m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD, 1000, 1, 0);
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3])
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) return true;

	//效验用户 注意：机器人有可能发生此断言
	//ASSERT((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR));
	if ((wChairID != m_wCurrentUser) && (m_wCurrentUser != INVALID_CHAIR))  return true;

 	//被动动作
 	if (m_wCurrentUser == INVALID_CHAIR)
 	{
 		//效验状态
 		ASSERT(!m_bResponse[wChairID]);
 		ASSERT(m_cbUserAction[wChairID] != WIK_NULL);
 		ASSERT((cbOperateCode == WIK_NULL) || ((m_cbUserAction[wChairID]&cbOperateCode) != 0));
 
 		//效验状态
 		if (m_bResponse[wChairID]) return true;
 		if(m_cbUserAction[wChairID] == WIK_NULL) return true;
 		if ((cbOperateCode!=WIK_NULL) && ((m_cbUserAction[wChairID]&cbOperateCode) == 0)) return true;
 
 		//变量定义
 		WORD wTargetUser = wChairID;
 		BYTE cbTargetAction = cbOperateCode;
 
 		//设置变量
		m_bEnjoinGang[wTargetUser] = false;
 		m_bResponse[wTargetUser] = true;
 		m_cbPerformAction[wTargetUser] = cbOperateCode;
 		if(cbOperateCard[0] == 0)
		{
			m_cbOperateCard[wTargetUser][0] = m_cbProvideCard;
		}
 		else 
		{
			CopyMemory(m_cbOperateCard[wTargetUser], cbOperateCard, sizeof(m_cbOperateCard[wTargetUser]));
		}
 
		//放弃操作
		if (cbTargetAction == WIK_NULL)
		{
			////禁止这轮吃胡
			if((m_cbUserAction[wTargetUser] & WIK_CHI_HU) != 0)
			{
				m_bEnjoinChiHu[wTargetUser]=true;
				//m_vecEnjoinChiHu[wTargetUser].push_back(m_cbProvideCard);
			}
			////禁止吃碰杠这张牌
			//if((m_cbUserAction[wTargetUser] & WIK_PENG) != 0)
			//{
			//	m_vecEnjoinChiPeng[wTargetUser].push_back(m_cbProvideCard);
			//}
		}

 		//执行判断
 		for (WORD i = 0; i < m_cbPlayerCount; i++)
 		{
 			//获取动作
 			BYTE cbUserAction = (!m_bResponse[i]) ? m_cbUserAction[i] : m_cbPerformAction[i];
 
 			//优先级别
 			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(cbUserAction);
 			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(cbTargetAction);
 
 			//动作判断
 			if (cbUserActionRank > cbTargetActionRank)
 			{
 				wTargetUser = i;
 				cbTargetAction = cbUserAction;
 			}
 		}
 		if (!m_bResponse[wTargetUser]) 
 			return true;
 
 		//吃胡等待
 		if (cbTargetAction == WIK_CHI_HU)
 		{
			for (WORD i = 0; i < m_cbPlayerCount; i++)
 			{
 				if (!m_bResponse[i] && (m_cbUserAction[i]&WIK_CHI_HU))
 					return true;
 			}
 		}
 
 		//放弃操作
 		if (cbTargetAction == WIK_NULL)
 		{
			//用户状态
 			ZeroMemory(m_bResponse, sizeof(m_bResponse));
 			ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 			ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
			DispatchCardData(m_wResumeUser,m_cbGangStatus != WIK_GANERAL);
 			return true;
 		}
 
 		//变量定义
 		BYTE cbTargetCard = m_cbOperateCard[wTargetUser][0];
 
 		//出牌变量
 		m_enSendStatus = Gang_Send;
		m_cbSendCardData = 0;
 		m_wOutCardUser = INVALID_CHAIR;
		m_cbOutCardData = 0;
 
 		//胡牌操作
 		if (cbTargetAction == WIK_CHI_HU)
 		{
 			//结束信息
 			m_cbChiHuCard = cbTargetCard;
 
			WORD wChiHuUser = m_wBankerUser;
			for(int i = 0; i < m_cbPlayerCount; i++)
 			{
 				wChiHuUser = (m_wBankerUser + i) % m_cbPlayerCount;
				//过虑判断
 				if ((m_cbPerformAction[wChiHuUser]&WIK_CHI_HU) == 0)
 					continue;
 
 				//胡牌判断
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChiHuUser];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChiHuUser];
 				m_dwChiHuKind[wChiHuUser] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChiHuUser], pWeaveItem, cbWeaveItemCount, m_cbChiHuCard, m_ChiHuRight[wChiHuUser]);

 				//插入扑克
 				if (m_dwChiHuKind[wChiHuUser] != WIK_NULL) 
 				{
 					wTargetUser = wChiHuUser;
					//break;
 				}				
 			}
 
 			//结束游戏
 			ASSERT(m_dwChiHuKind[wTargetUser] != WIK_NULL);
 			OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 			return true;
 		}
 
 		//组合扑克
 		ASSERT(m_cbWeaveItemCount[wTargetUser] < MAX_WEAVE);
 		WORD wIndex = m_cbWeaveItemCount[wTargetUser]++;
 		m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_GANERAL;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard = cbTargetCard;
 		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind = cbTargetAction;
 		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCardData[0] = cbTargetCard;
 		if(cbTargetAction&(WIK_LEFT | WIK_CENTER | WIK_RIGHT))
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = m_cbOperateCard[wTargetUser][1];
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = m_cbOperateCard[wTargetUser][2];
 		}
 		else
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = cbTargetCard;
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = cbTargetCard;
 			if(cbTargetAction & WIK_GANG)
			{
				m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_FANG_GANG;
				m_WeaveItemArray[wTargetUser][wIndex].cbCardData[3] = cbTargetCard;
			}
 		}
 
 		//删除扑克
 		switch (cbTargetAction)
 		{
 		case WIK_LEFT:		//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_RIGHT:		//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_CENTER:	//上牌操作
 			{
 				//删除扑克
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_PENG:		//碰牌操作
 			{
 				//删除扑克
 				BYTE cbRemoveCard[] = {cbTargetCard,cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;

 				break;
 			}
 		case WIK_GANG:		//杠牌操作
 			{
 				//删除扑克,被动动作只存在放杠
 				BYTE cbRemoveCard[] = {cbTargetCard, cbTargetCard, cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, CountArray(cbRemoveCard)))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
 
 				break;
 			}
 		default:
 			ASSERT(FALSE);
 			return false;
 		}
 
 		//构造结果
 		CMD_S_OperateResult OperateResult;
 		ZeroMemory(&OperateResult, sizeof(OperateResult));
 		OperateResult.wOperateUser = wTargetUser;
 		OperateResult.cbOperateCode = cbTargetAction;
 		OperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		OperateResult.cbOperateCard[0] = cbTargetCard;
 		if(cbTargetAction & (WIK_LEFT | WIK_CENTER | WIK_RIGHT))
		{
			CopyMemory(&OperateResult.cbOperateCard[1], &m_cbOperateCard[wTargetUser][1], 2 * sizeof(BYTE));
		}
 		else if(cbTargetAction&WIK_PENG)
 		{
 			OperateResult.cbOperateCard[1] = cbTargetCard;
 			OperateResult.cbOperateCard[2] = cbTargetCard;
 		}
 
 		//用户状态
 		ZeroMemory(m_bResponse, sizeof(m_bResponse));
 		ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 		ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 		ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
 		//如果非杠牌
 		if(cbTargetAction != WIK_GANG)
 		{
			m_wProvideUser = INVALID_CHAIR;
			m_cbProvideCard = 0;

 			tagGangCardResult gcr;
 			m_cbUserAction[wTargetUser] |= m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser],0, gcr);

			if(m_bTing[wTargetUser] == false)
			{
				BYTE cbCount = 0;
				CMD_S_Hu_Data HuData;
				ZeroMemory(&HuData,sizeof(HuData));
				cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[wTargetUser],m_WeaveItemArray[wTargetUser],m_cbWeaveItemCount[wTargetUser],HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
				if(cbCount >0)
				{
					m_cbUserAction[wTargetUser] |= WIK_LISTEN; 

					for(int i=0;i<MAX_COUNT;i++)
					{
						if(HuData.cbHuCardCount[i]>0)
						{
							for(int j=0;j<HuData.cbHuCardCount[i];j++)
							{
								HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wTargetUser,HuData.cbHuCardData[i][j]);
							}
						}
						else
							break;
					}
					//addGameRecord(wTargetUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
					m_pITableFrame->SendTableData(wTargetUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
				}

			}
 			OperateResult.cbActionMask |= m_cbUserAction[wTargetUser];
 		}
 
 		//发送消息
		addGameRecord(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 
		m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
		m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

 		//设置用户
 		m_wCurrentUser = wTargetUser;
 
 		//杠牌处理
 		if (cbTargetAction == WIK_GANG)
 		{
 			m_cbGangStatus = WIK_FANG_GANG;
			m_wProvideGangUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
			m_bGangCard[wTargetUser] = true;
			m_cbGangCount[wTargetUser]++;

			/*放杠不抢杠胡
			//效验动作
			bool bAroseAction = false;
			bAroseAction = EstimateUserRespond(wTargetUser, cbOperateCard[0], EstimatKind_GangCard);

			//发送扑克
			if (!bAroseAction)
			{
				DispatchCardData(wTargetUser, true);
			}
			*/
			DispatchCardData(wTargetUser, true);
 		}
 
 		return true;
 	}
 
 	//主动动作
 	if (m_wCurrentUser == wChairID)
 	{
 		//效验操作
 		//ASSERT((cbOperateCode != WIK_NULL) && ((m_cbUserAction[wChairID]&cbOperateCode) != 0));
 		//if ((cbOperateCode == WIK_NULL) || ((m_cbUserAction[wChairID]&cbOperateCode) == 0)) return false;
 
 		//扑克效验
 		ASSERT((cbOperateCode == WIK_NULL) || (cbOperateCode == WIK_CHI_HU) || (m_GameLogic.IsValidCard(cbOperateCard[0])));
 		if ((cbOperateCode != WIK_NULL) && (cbOperateCode != WIK_CHI_HU) && (!m_GameLogic.IsValidCard(cbOperateCard[0]))) return false;
 
 		//设置变量
 		m_cbUserAction[m_wCurrentUser] = WIK_NULL;
 		m_cbPerformAction[m_wCurrentUser] = WIK_NULL;
 
 		//执行动作
 		switch (cbOperateCode)
 		{
 		case WIK_GANG:			//杠牌操作
 			{
				m_enSendStatus = Gang_Send;

 				//变量定义
 				BYTE cbWeaveIndex = 0xFF;
 				BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard[0]);
				WORD wProvideUser = wChairID;
				BYTE cbGangKind = WIK_MING_GANG;
 				//杠牌处理
 				if (m_cbCardIndex[wChairID][cbCardIndex] == 1)
 				{
 					//寻找组合
 					for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
 					{
 						BYTE cbWeaveKind = m_WeaveItemArray[wChairID][i].cbWeaveKind;
 						BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
 						if ((cbCenterCard == cbOperateCard[0]) && (cbWeaveKind == WIK_PENG))
 						{
 							cbWeaveIndex = i;
 							break;
 						}
 					}
 
 					//效验动作
 					ASSERT(cbWeaveIndex != 0xFF);
 					if (cbWeaveIndex == 0xFF) return false;
 
					cbGangKind = WIK_MING_GANG;

 					//组合扑克
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_MING_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[3] = cbOperateCard[0];
 
 					//杠牌得分
 					wProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
 				}
 				else
 				{
 					//扑克效验
 					ASSERT(m_cbCardIndex[wChairID][cbCardIndex] == 4);
 					if (m_cbCardIndex[wChairID][cbCardIndex] != 4) 
 						return false;
 
 					//设置变量
 					cbGangKind = WIK_AN_GANG;
 					cbWeaveIndex = m_cbWeaveItemCount[wChairID]++;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_AN_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser = wChairID;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					for(BYTE j = 0; j < 4; j++) 
					{
						m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[j] = cbOperateCard[0];
					}
 				}
 
 				//删除扑克
 				m_cbCardIndex[wChairID][cbCardIndex] = 0;
				m_cbGangStatus = cbGangKind;
				m_wProvideGangUser = wProvideUser;
				m_bGangCard[wChairID] = true;
				m_cbGangCount[wChairID]++;
 
 				//构造结果
 				CMD_S_OperateResult OperateResult;
 				ZeroMemory(&OperateResult, sizeof(OperateResult));
 				OperateResult.wOperateUser = wChairID;
 				OperateResult.wProvideUser = wProvideUser;
 				OperateResult.cbOperateCode = cbOperateCode;
 				OperateResult.cbOperateCard[0] = cbOperateCard[0];
 
 				//发送消息
				addGameRecord(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));

 				//效验动作
 				bool bAroseAction = false;
 				if (cbGangKind == WIK_MING_GANG && m_cbQiangGang) //检查抢杠胡
				{
					bAroseAction=EstimateUserRespond(wChairID, cbOperateCard[0], EstimatKind_GangCard);
				}
 
 				//发送扑克
 				if (!bAroseAction)
 				{
 					DispatchCardData(wChairID, true);
 				}
 
 				return true;
 			}
 		case WIK_CHI_HU:		//自摸
 			{
 				//普通胡牌
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChairID];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChairID];
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], &m_cbSendCardData, 1))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID], pWeaveItem, cbWeaveItemCount, m_cbSendCardData, m_ChiHuRight[wChairID], m_cbOutCardData == 0/*未出牌第一把，可能是4个红中胡牌*/);
 
 				//结束信息
				m_cbChiHuCard = m_cbSendCardData;
				m_cbProvideCard = m_cbSendCardData;
				
 				//结束游戏
 				OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 				return true;
 			}
 		}
 
 		return true;
 	}

	return false;
}


//用户听牌
bool CTableFrameSink::OnUserListenCard(WORD wChairID, bool bListenCard)
{
	if(bListenCard)
	{
		ASSERT(!m_bTing[wChairID]);
		if(WIK_LISTEN == m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID]))
		{
			m_bTing[wChairID] = true;
			CMD_S_ListenCard ListenCard;
			ZeroMemory(&ListenCard,sizeof(ListenCard));

			ListenCard.wListenUser = wChairID;
			ListenCard.bListen = true;
			//发给消息
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));

			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if( i != wChairID)
					m_pITableFrame->SendTableData(i,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));
			}

			//计算胡几张字
			m_cbHuCardCount[wChairID] = m_GameLogic.GetHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],m_cbHuCardData[wChairID]);
			ASSERT(m_cbHuCardCount[wChairID]>0);
			ListenCard.cbHuCardCount = m_cbHuCardCount[wChairID];
			CopyMemory(ListenCard.cbHuCardData,m_cbHuCardData[wChairID],sizeof(ListenCard.cbHuCardData));

			m_pITableFrame->SendTableData(wChairID,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));
			

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{

		ASSERT(m_bTing[wChairID]);
		m_bTing[wChairID] = false;

		CMD_S_ListenCard ListenCard;
		ListenCard.wListenUser = wChairID;
		ListenCard.bListen = false;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		return true;
	}
}

//用户托管
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//效验状态
	ASSERT((wChairID < m_cbPlayerCount));
	if ((wChairID>=m_cbPlayerCount)) return false;

	m_bTrustee[wChairID]=bTrustee;

	CMD_S_Trustee Trustee;
	Trustee.bTrustee=bTrustee;
	Trustee.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

	if(bTrustee)
	{
		if(wChairID == m_wCurrentUser && m_bUserActionDone==false)
		{
			
			BYTE cardindex=INVALID_BYTE;
			if(m_cbSendCardData != 0)
				cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
			else
			{
				for(int i=0;i<MAX_INDEX;i++)
				{
					if(m_cbCardIndex[wChairID][i]>0)
					{
						cardindex = i;
						break;
					}
				}
			}

			ASSERT(cardindex != INVALID_BYTE);
			OnUserOutCard(wChairID,m_GameLogic.SwitchToCardData(cardindex));
		}
		else if(m_wCurrentUser == INVALID_CHAIR && m_bUserActionDone==false)
		{
			BYTE operatecard[3]={0};
			OnUserOperateCard(wChairID,WIK_NULL,operatecard);
		}
	}

	return true;
}

//用户补牌
bool CTableFrameSink::OnUserReplaceCard(WORD wChairID, BYTE cbCardData)
{
	//错误断言
	ASSERT(m_GameLogic.IsValidCard(cbCardData));
	//效验参数
	if(!m_GameLogic.IsValidCard(cbCardData))  return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//状态变量
	m_enSendStatus = BuHua_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;

	//构造数据
	CMD_S_ReplaceCard ReplaceCard;
	ReplaceCard.wReplaceUser = wChairID;
	ReplaceCard.cbReplaceCard = cbCardData;

	//发送消息
	addGameRecord(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));

	//派发扑克
	DispatchCardData(wChairID, true);

	return true;
}

//发送扑克
bool CTableFrameSink::OnUserSendCard(BYTE cbCardCount, WORD wBankerUser, BYTE cbCardData[], BYTE cbControlGameCount)
{
#ifdef  CARD_DISPATCHER_CONTROL
	ASSERT(cbCardCount == MAX_REPERTORY && wBankerUser < GAME_PLAYER);
	if(cbCardCount != MAX_REPERTORY || wBankerUser >= GAME_PLAYER) return false;

	//设置控制局数
	m_cbControlGameCount = cbControlGameCount;

	m_wControBankerUser = wBankerUser;

	//拷贝控制扑克
	CopyMemory(m_cbControlRepertoryCard, cbCardData, sizeof(m_cbControlRepertoryCard));
#endif
	return true;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			addGameRecord(i,SUB_S_OPERATE_NOTIFY, &OperateNotify, sizeof(OperateNotify));
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			
// 			if(m_bTrustee[i])
// 			{
// 				m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD+i,1000,1,0);
// 			}
		}
	}

	return true;
}

//取得扑克
BYTE CTableFrameSink::GetSendCard(bool bTail)
{
	//发送扑克
	m_cbSendCardCount++;
	m_cbLeftCardCount--;

	BYTE cbSendCardData = 0;
	
	BYTE cbIndexCard;
	if(bTail)
	{	
		cbSendCardData = m_cbRepertoryCard[m_cbMinusLastCount];
		m_cbMinusLastCount++;
	}
	else
	{
		m_cbMinusHeadCount++;
		cbIndexCard = MAX_REPERTORY - m_cbMinusHeadCount;
		cbSendCardData=m_cbRepertoryCard[cbIndexCard];
	}	

	//堆立信息
	ASSERT(m_wHeapHead != INVALID_CHAIR && m_wHeapTail != INVALID_CHAIR);
	if(!bTail)
	{
		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapHead][0] + m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount == HEAP_FULL_COUNT) 
		{
			m_wHeapHead = (m_wHeapHead + GAME_PLAYER - 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapHead][0]++;
	}
	else
	{
		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapTail][0] + m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount == HEAP_FULL_COUNT) 
		{
			m_wHeapTail = (m_wHeapTail + 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapTail][1]++;
	}

	return cbSendCardData;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wSendCardUser, bool bTail)
{
	//状态效验
	ASSERT(wSendCardUser != INVALID_CHAIR);
	if (wSendCardUser == INVALID_CHAIR) return false;

	ASSERT(m_enSendStatus != Not_Send);
	if(m_enSendStatus == Not_Send) return false;

	WORD wCurrentUser = wSendCardUser;

	//丢弃扑克
	if ((m_wOutCardUser != INVALID_CHAIR) && (m_cbOutCardData != 0))
	{
		m_cbOutCardCount++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]] = m_cbOutCardData;
		m_cbDiscardCount[m_wOutCardUser]++;
	}

	//荒庄结束
	if (m_cbLeftCardCount <= (m_cbMaCount>1?m_cbMaCount:m_cbMaCount) )//2-6码要多留一颗，一码全中不用
	{
		m_cbChiHuCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);

		return true;
	}

	//发送扑克
	m_cbProvideCard = GetSendCard(bTail);	
	m_cbSendCardData = m_cbProvideCard;
	m_wLastCatchCardUser = wSendCardUser;
	//清除禁止胡牌的牌
	m_bEnjoinChiHu[wCurrentUser] = false;
	m_vecEnjoinChiHu[wCurrentUser].clear();
	m_bEnjoinChiPeng[wCurrentUser] = false;		
	m_vecEnjoinChiPeng[wCurrentUser].clear();
	m_bEnjoinGang[wCurrentUser] = false;
	
	//设置变量	
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;	
	m_wCurrentUser = wCurrentUser;
	m_wProvideUser = wCurrentUser;		
	m_bGangOutCard = false;
	
	if(bTail)//从尾部取牌，说明玩家杠牌了,计算分数
	{
		CalGangScore();
	}

	//加牌
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;
	m_cbUserCatchCardCount[wCurrentUser]++;

	if(!m_bTrustee[wCurrentUser])
	{
		//胡牌判断
		if(!m_bEnjoinChiHu[wCurrentUser])
		{		
			CChiHuRight chr;
			m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser],
				m_cbWeaveItemCount[wCurrentUser], m_cbSendCardData, chr);
			m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
		}

		//杠牌判断
		if ((!m_bEnjoinGang[wCurrentUser]) && (m_cbLeftCardCount > m_cbEndLeftCount) && !m_bTing[wCurrentUser])
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser],m_cbProvideCard ,GangCardResult);
		}
	}

		//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));

	//听牌判断
	CMD_S_Hu_Data HuData;
	ZeroMemory(&HuData,sizeof(HuData));

	BYTE cbCount = 0;
	if(m_bTing[wCurrentUser] == false)
	{
		cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
		if(cbCount >0)
		{
			m_cbUserAction[wCurrentUser] |= WIK_LISTEN; 
			
			for(int i=0;i<MAX_COUNT;i++)
			{
				if(HuData.cbHuCardCount[i]>0)
				{
					for(int j=0;j<HuData.cbHuCardCount[i];j++)
					{
						HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wCurrentUser,HuData.cbHuCardData[i][j]);
					}
				}
				else
					break;
			}
			//addGameRecord(wCurrentUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
			m_pITableFrame->SendTableData(wCurrentUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
		}
	}

	SendCard.wSendCardUser = wSendCardUser;
	SendCard.wCurrentUser = wCurrentUser;
	SendCard.bTail = bTail;
	SendCard.cbActionMask = m_cbUserAction[wCurrentUser];
	SendCard.cbCardData = m_cbProvideCard;
	
	//发送数据
	addGameRecord(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

	m_bUserActionDone=false;
	if(m_bTrustee[wCurrentUser])
	{
		m_bUserActionDone=true;
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD,1000,1,0);
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction = false;

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		//用户过滤
		if (wCenterUser == i || !m_bPlayStatus[i] || m_bTrustee[i]) continue;

		//出牌类型
		if (EstimatKind == EstimatKind_OutCard )
		{
			//吃碰判断
			if(!m_bEnjoinChiPeng[i])
			{
				bool bPeng = true;
				for(UINT j = 0; j < m_vecEnjoinChiPeng[i].size(); j++)
				{
					if(m_vecEnjoinChiPeng[i][j] == cbCenterCard)
					{
						bPeng = false;
					}
				}
				if(bPeng)
				{
					//碰牌判断
					m_cbUserAction[i] |= m_GameLogic.EstimatePengCard(m_cbCardIndex[i], cbCenterCard);
				}

				//杠牌判断
				if(m_cbLeftCardCount > m_cbEndLeftCount && !m_bEnjoinGang[i]) 
				{
					m_cbUserAction[i] |= m_GameLogic.EstimateGangCard(m_cbCardIndex[i], cbCenterCard);
				}
			}
		}

		//检查炮胡(允许接炮或者抢杠胡)
   		if ((EstimatKind == EstimatKind_GangCard) || (m_cbJiePao && !m_bEnjoinChiHu[i]))
   		{
			if(m_cbMagicIndex == MAX_INDEX || (m_cbMagicIndex != MAX_INDEX && cbCenterCard != m_GameLogic.SwitchToCardData(m_cbMagicIndex)))
			{
				bool bChiHu = true;
				for(UINT j = 0; j < m_vecEnjoinChiHu[i].size(); j++)
				{
					if(m_vecEnjoinChiHu[i][j] == cbCenterCard)
					{
						bChiHu = false;
						break;
					}
				}
				if(bChiHu)
				{
					//吃胡判断
					CChiHuRight chr;
					BYTE cbWeaveCount = m_cbWeaveItemCount[i];
					BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i], m_WeaveItemArray[i], cbWeaveCount, cbCenterCard, chr);
					m_cbUserAction[i] |= cbAction;
				}
			}
   		}

		//结果判断
		if (m_cbUserAction[i] != WIK_NULL) 
			bAroseAction = true;
	}

	//结果处理
	if (bAroseAction) 
	{
		//设置变量
		m_wProvideUser = wCenterUser;
		m_cbProvideCard = cbCenterCard;
		m_wResumeUser = m_wCurrentUser;
		m_wCurrentUser = INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		return true;
	}

	return false;
}

//算分
void CTableFrameSink::CalHuPaiScore(LONGLONG lEndScore[GAME_PLAYER])
{
	//初始化
	ZeroMemory(lEndScore,sizeof(LONGLONG)*GAME_PLAYER);
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));

	bool bNoNegative = (m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 || (m_pGameServiceOption->wServerType & SCORE_GENRE_POSITIVE) != 0;

	SCORE lCellScore = m_pITableFrame->GetCellScore();

	SCORE lUserScore[GAME_PLAYER] = {0};//玩家手上分
	SCORE lTempScore = 0;
	SCORE lTempAllScore = 0;
	for(int i = 0; i < GAME_PLAYER; i++)
	{
		if(!m_bPlayStatus[i])
			continue;
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			lUserScore[i] = pIServerUserItem->GetUserScore();
		}		
	}

	WORD wWinUser[GAME_PLAYER] = {0};
	BYTE cbWinCount = 0;

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		if(WIK_CHI_HU == m_dwChiHuKind[(m_wBankerUser + i) % GAME_PLAYER])
		{
			wWinUser[cbWinCount++] = (m_wBankerUser + i) % GAME_PLAYER;

			//统计胡牌次数
			m_stRecord.cbHuCount[(m_wBankerUser + i) % GAME_PLAYER]++;
		}
	}

	if(cbWinCount > 0)//有人胡牌
	{
		bool bZiMo= (m_wProvideUser == wWinUser[0]);
		if(bZiMo)//自摸
		{
			BYTE cbTimes = GetTimes(wWinUser[0]);
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(!m_bPlayStatus[i])
					continue;
				if(i != wWinUser[0])
				{
					lEndScore[i] -= cbTimes * lCellScore * diFenUnit;
					lEndScore[wWinUser[0]] += cbTimes * lCellScore * diFenUnit;
				}
			}
		}
		else if(m_cbGangStatus==WIK_MING_GANG)//抢杠
		{
			for(int i=0;i<cbWinCount;i++)
			{
				BYTE cbTimes=GetTimes(wWinUser[i]);

				for(int j=0;j<GAME_PLAYER;j++)
				{
					if(!m_bPlayStatus[j])
						continue;
					if(j != wWinUser[i])
					{
						lEndScore[wWinUser[i]] += cbTimes * lCellScore * diFenUnit;
					}
				}
				lEndScore[m_wProvideUser] -= lEndScore[wWinUser[i]];
			}
		}
		else//放炮
		{
			for(int i=0;i<cbWinCount;i++)
			{
				BYTE cbTimes=GetTimes(wWinUser[i]);
				lEndScore[wWinUser[i]] += cbTimes * lCellScore * diFenUnit;
				lEndScore[m_wProvideUser] -= cbTimes * lCellScore * diFenUnit;
			}
		}

		//谁胡谁当庄
		m_wBankerUser = wWinUser[0];
		if(cbWinCount > 1)//多个玩家胡牌，放炮者当庄
			m_wBankerUser = m_wProvideUser;
	}
	else//荒庄
	{
		m_wBankerUser = m_wLastCatchCardUser;//最后一个摸牌的人当庄
	}

}

void CTableFrameSink::CalGangScore()
{
	SCORE lcell = m_pITableFrame->GetCellScore();
	if(m_cbGangStatus == WIK_FANG_GANG)//放杠一家扣分
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[m_wProvideGangUser] -= lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += lcell * diFenUnit;
			}
		}
		//记录明杠次数
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_MING_GANG)//明杠每家出1倍
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += lcell * diFenUnit;
			}
		}
		//记录明杠次数
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_AN_GANG)//暗杠每家出2倍
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= 2 * lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += 2 * lcell * diFenUnit;
			}
		}
		//记录暗杠次数
		m_stRecord.cbAnGang[m_wCurrentUser]++;
	}
}
//权位过滤
void CTableFrameSink::FiltrateRight(WORD wWinner, CChiHuRight &chr)
{
	//自摸
	if(wWinner == m_wProvideUser)
	{
		chr |= CHR_ZI_MO;

	}
	else if(m_cbGangStatus == WIK_MING_GANG)
	{
		chr |= CHR_QIANG_GANG_HU;
	}
	else
	{
		//chr |= CHR_JIE_PAO;
	}

}

//设置基数
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

BYTE CTableFrameSink::GetTimes(WORD wChairId)
{
	BYTE cbScore = 0;
	ASSERT(m_cbMaCount >= 0 && m_cbMaCount <= 6);
	m_cbUserMaCount[wChairId]=m_cbMaCount;

	if(m_cbMaCount == 1)//一码全中
	{
		BYTE carddata = m_cbRepertoryCard[m_cbMinusLastCount];
		if(m_GameLogic.GetCardColor(carddata) < 0x30)
		{
			cbScore = m_GameLogic.GetCardValue(carddata);
		}
		else//红中10分
		{
			cbScore = 10;
		}
		//统计中码个数,一码全中码数应该为分数
		m_stRecord.cbMaCount[wChairId]+=cbScore;
	}
	else//2-6码
	{
		//没选无码的话,胡牌手中没红中，加一个码
		//if(m_cbUserMaCount[wChairId] && m_cbCardIndex[wChairId][m_cbMagicIndex] == 0 && m_GameLogic.SwitchToCardIndex(m_cbProvideCard) != m_cbMagicIndex)
		//	m_cbUserMaCount[wChairId]++;

		for(int i=0;i<m_cbUserMaCount[wChairId];i++)
		{
			BYTE carddata = m_cbRepertoryCard[m_cbMinusLastCount+i];

			if(m_GameLogic.GetCardValue(carddata)%4 == 1)//1,5,9,红中 算中码
			{
				cbScore += 2;
				//统计中码个数
				m_stRecord.cbMaCount[wChairId]++;
			}
		}
	}

	return cbScore+2;//基础倍数+2
}

BYTE CTableFrameSink::GetRemainingCount(WORD wChairID,BYTE cbCardData)
{
	BYTE cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE nCount=0;
	for(int i=m_cbMinusLastCount;i<MAX_REPERTORY-m_cbMinusHeadCount;i++)
	{
		if(m_cbRepertoryCard[i] == cbCardData)
			nCount++;
	}
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if( i == wChairID)
			continue;

		nCount += m_cbCardIndex[i][cbIndex];
	}
	return nCount;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	
	//自动托管
	OnUserTrustee(wChairID,true);


	return true;
}



void CTableFrameSink::addGameRecord(WORD wChairId, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
}


//////////////////////////////////////////////////////////////////////////////////
