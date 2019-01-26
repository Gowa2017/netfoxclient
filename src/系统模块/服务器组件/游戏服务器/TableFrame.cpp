#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//断线定义
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//断线标识
#define IDI_ANDROID_THEJOINT		(TIME_TABLE_SINK_RANGE+2)			//机器人串房起立
#define MAX_OFF_LINE				3									//断线次数
#define TIME_OFF_LINE				60000L								//断线时间
#define TIME_OFF_LINE_PERSONAL		300000L								//断线时间
#define TIME_ANDROID_THEJOINT		30000L								//串场离开时间
//开始超时
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+3)			    //开始超时
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+22)			    //开始超时
#ifndef _DEBUG
#define TIME_OVERTIME				30000L								//超时时间
#else
#define TIME_OVERTIME               30000L                               //超时时间
#endif

//私人房间
#define IDI_PERSONAL_BEFORE_BEGAIN_TIME		TIME_TABLE_SINK_RANGE+23		//私人房间超时
#define IDI_PERSONAL_AFTER_BEGIN_TIME			TIME_TABLE_SINK_RANGE+24		//最大超时
#define IDI_PERSONAL_AFTER_CREATE_ROOM_TIME			TIME_TABLE_SINK_RANGE+25		//房间创建后多长时间无人坐桌解散房间


//////////////////////////////////////////////////////////////////////////////////

//游戏记录
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrame::CTableFrame()
{
	//固有属性
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;

	//标志变量
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));
	//ZeroMemory(m_lWriteuser,sizeof(m_lWriteuser));
	//游戏变量
	m_lCellScore=0L;
	m_wDrawCount=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//时间变量
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//动态属性
	m_dwTableOwnerID=0L;
	m_dwRecordTableID = 0;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//断线变量
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//配置信息
	m_pGameParameter=NULL;
	m_pGameMatchOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件接口
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//扩张接口
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;

	//数据接口
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//比赛接口
	m_pITableFrameHook=NULL;
	m_pIMatchTableAction=NULL;

	//用户数组
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//私人房间
	m_bPersonalLock = false;
	m_pTableOwnerUser = NULL;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_lIniScore = 0;
	m_dwPersonalPlayCount = 0;
	m_dwTimeBegin = 0;
	m_PlayerWaitTime = 0;
	m_TableOwnerWaitTime = 0;
	m_GameStartOverTime = 0;
	m_bWaitConclude = false;
	m_dwReplyCount = 0;
	ZeroMemory(m_bRequestReply, sizeof(m_bRequestReply));
	ZeroMemory(m_szPersonalTableID,sizeof(m_szPersonalTableID));
	m_MapPersonalTableScoreInfo.InitHashTable(MAX_CHAIR);

	//是否强制解散游戏
	m_bIsDissumGame = false;
	memset(m_cbSpecialInfo, 0, sizeof(m_cbSpecialInfo));
	m_nSpecialInfoLen = 0;
	m_bIsAllRequstCancel = false;
	memset(m_bBeOut, 0, sizeof(m_bBeOut));
	m_bFirstRegister = false;
	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);

	return;
}

//接口查询
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ICompilationSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//开始游戏
bool CTableFrame::StartGame()
{
	//游戏状态
	ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//保存变量
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//设置状态
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;
	//ZeroMemory(m_lWriteuser,sizeof(m_lWriteuser));
	//开始时间
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//记录时间
	if(m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		m_dwPersonalPlayCount++;

	//记录房主
	m_dwRecordTableID = m_dwTableOwnerID;

	//开始设置
	if (bGameStarted==false)
	{
		//状态变量
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
		memset(m_bBeOut, 0, sizeof(m_bBeOut));
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);

		//设置用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem!=NULL)
			{
				//锁定游戏币
				if (m_pGameServiceOption->lServiceScore>0L)
				{
					m_lFrozenedScore[i]=m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
				}

				//设置状态
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus!=US_OFFLINE)&&(cbUserStatus!=US_PLAYING))
				{
					pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);
					printf("游戏开始设置玩家状态_%d\n",pIServerUserItem->GetUserStatus());
				}

				////最大时间
				if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && i == 0)
					SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount*60*1000,1,i);
				if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_szPersonalTableID != NULL )		
				{
					m_pIMainServiceFrame->PersonalRoomWriteJoinInfo(pIServerUserItem->GetUserID(),  m_wTableID, m_szPersonalTableID);
				}
				

			}
		}

		//发送状态
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//分数记录
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if(pUserItem == NULL) continue;

		m_MapPersonalTableScoreInfo[pUserItem->GetUserID()] = pUserItem->GetUserScore();
	}

	//比赛通知
	bool bStart=true;
	if(m_pITableFrameHook!=NULL) bStart=m_pITableFrameHook->OnEventGameStart(m_wChairCount);

	//通知事件
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	return true;
}

//解散游戏
bool CTableFrame::DismissGame()
{
	//状态判断
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//结束游戏
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		//写入分数
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//积分变量
			tagScoreInfo ScoreInfoArray;
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			ScoreInfoArray.cbType = SCORE_TYPE_FLEE;								//积分类型
			ScoreInfoArray.lScore = 0;								//用户分数
			ScoreInfoArray.lGrade = 0;								//用户成绩
			ScoreInfoArray.lRevenue = 0;							//游戏税收
			WriteUserScore(i,ScoreInfoArray);
		}
	}

	//设置状态
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		SendTableStatus();
	}


	return false;
}

//结束游戏
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//效验状态
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//保存变量
	bool bDrawStarted=m_bDrawStarted;

	//设置状态
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	//关闭开始超时定时器
	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);

	//游戏记录
	RecordGameScore(bDrawStarted);
	
	//结束设置
	if (m_bGameStarted==false)
	{
		//变量定义
		bool bOffLineWait=false;

		//设置用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//用户处理
			if (pIServerUserItem!=NULL)
			{
				//解锁游戏币
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//设置状态
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//断线处理
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
					{						
						pIServerUserItem->SetClientReady(true);
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
					}
					else
					{
						PerformStandUpAction(pIServerUserItem);
					}
				}
				else
				{
					if (pIServerUserItem->GetUserStatus() == US_NULL)
					{
						pIServerUserItem->SetUserStatus(US_NULL,m_wTableID,i);
						PerformStandUpAction(pIServerUserItem);
					}
					else
					{
						//设置状态
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);	
					}
				}
			}
		}

		//删除时间
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//通知比赛
	if(m_pITableFrameHook!=NULL) m_pITableFrameHook->OnEventGameEnd(0, NULL, cbGameStatus);

	//重置桌子
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	//踢出检测
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			//if (m_pGameServiceOption->lCellScore < 1)
			//{
			//	m_pGameServiceOption->lCellScore  = 1;
			//}

			//积分限制
			if ((m_pGameServiceOption->lMinTableScore!=0L)&&
				(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore)&& 
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) && (pIServerUserItem->IsAndroidUser() || m_pGameServiceAttrib->wChairCount>= MAX_CHAIR)				
				)
			{
				//构造提示
				TCHAR szDescribe[128]=TEXT("");
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				{
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore);
				}
				else
				{
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的游戏积分少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore);
				}

				//发送消息
				if (pIServerUserItem->IsAndroidUser()==true)
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_CLOSE_ROOM|SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

				//用户起立
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//构造提示
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{
					_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					m_bBeOut[i]  = true;
					continue;
				}
			}

			//关闭判断
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("由于系统维护，当前游戏桌子禁止用户继续游戏！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

				//用户起立
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			//机器起立
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//机器处理
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//绑定信息
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//查找机器
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//离开判断
					//if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
					{
						//起立处理
						//PerformStandUpAction(pIServerUserItem);

						continue;
					}

					//起立判断
					//if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						//起立处理
						//PerformStandUpAction(pIServerUserItem);

						continue;
					}												
				}
			}

			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
			{
				//启动定时
				if(IsGameStarted()==false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR)						SetGameTimer(IDI_START_OVERTIME+i,TIME_OVERTIME,1,i);
			}
		}		
	}

	//结束桌子
	ConcludeTable();

	//发送状态
	SendTableStatus();

	//踢出用户
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		DWORD dwTimeNow = (DWORD)time(NULL);
		if((m_dwDrawCountLimit!=0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount) ||
			(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit <= dwTimeNow - m_dwTimeBegin))
		{

			m_dwPersonalPlayCount = 0;

			CMD_GR_PersonalTableEnd PersonalTableEnd;
			ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

			lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("约战结束。"), CountArray(PersonalTableEnd.szDescribeString));
			TCHAR szInfo[260] = {0};
			for(int i = 0; i < m_wChairCount; ++i)
			{
				PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
			}
			//添加特殊信息
			memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
			PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;


			WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;

			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pUserItem = GetTableUserItem(i);
				if(pUserItem == NULL) continue;

				PersonalTableEnd.lScore[i] =m_PersonalUserScoreInfo[i].lScore;
				m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd) );
			}


			//防止客户端因为状态为free获取不到用户指针
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				PerformStandUpAction(pTableUserItem);
			}

			//LogPrintf(("解散桌子"));
			//解散桌子
			m_pIMainServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, m_wTableID);

			//桌子设置
			m_dwTableOwnerID = 0;
			m_dwDrawCountLimit = 0;
			m_dwDrawTimeLimit = 0;
			m_dwTimeBegin = 0;

			m_dwReplyCount = 0;
			ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
			//ZeroMemory(m_szPersonalTableID,sizeof(m_szPersonalTableID));
			m_MapPersonalTableScoreInfo.RemoveAll();
		}

	}

	return true;
}

//结束桌子
bool CTableFrame::ConcludeTable()
{
	//结束桌子
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//人数判断
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//模式判断
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}


//写入积分
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	//效验参数
	ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)&&(ScoreInfo.cbType==SCORE_TYPE_NULL)) 
		return false;

	//获取用户
	//ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");
	//if(m_lWriteuser[wChairID] == true)
	//	return true;
	//else m_lWriteuser[wChairID] = true;
	//写入积分
	if (pIServerUserItem!=NULL)
	{
		if (ScoreInfo.lScore - m_pGameParameter->lHallNoticeThreshold >= DBL_EPSILON)
		{
			TCHAR szHallMessage[128] = TEXT("");
			_sntprintf(szHallMessage, sizeof(szHallMessage), TEXT("鸿运当头!ID:%d在【%s】中一把赢了%0.2f金!"), pIServerUserItem->GetGameID(), m_pGameServiceOption->szServerName, ScoreInfo.lScore);
			m_pIMainServiceFrame->SendHallMessage(szHallMessage, 0);
		}

		//变量定义
		DWORD dwUserMemal=0L;
		SCORE lRevenueScore=__min(m_lFrozenedScore[wChairID],m_pGameServiceOption->lServiceScore);

		//扣服务费
		if (m_pGameServiceOption->lServiceScore>0L 
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//扣服务费
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//解锁游戏币
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//奖牌计算
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			DWORD dwMedalRate=m_pGameParameter->dwMedalRate;
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*dwMedalRate/1000L);
		}

		//游戏时间
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if(dwPlayGameTime!=INVALID_DWORD) dwPlayTimeCount=dwPlayGameTime;

		//变量定义
		tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();

		//道具判断
		if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		{
			if (ScoreInfo.lScore>0L)
			{
				//积分双卡
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DOOUBLE);
				if ( pPropertyBuff != NULL )
				{
					//积分翻倍 2倍 4倍
					ScoreInfo.lScore *= pPropertyBuff->dwScoreMultiple;
					if( pPropertyBuff->dwScoreMultiple == 4 )
					{
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 四倍积分卡 ]，得分翻四倍！)"),pIServerUserItem->GetNickName());
					}
					else if( pPropertyBuff->dwScoreMultiple == 2 )
					{
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 双倍积分卡 ]，得分翻倍！"), pIServerUserItem->GetNickName());
					}
				}
			}
			else
			{
				//附身符
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE);
				if ( pPropertyBuff != NULL )
				{
					//积分不减
					ScoreInfo.lScore = 0;
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 护身符卡 ]，积分不变！"),pIServerUserItem->GetNickName());
				}
			}
		}

		//写入积分
		DWORD dwWinExperience=ScoreInfo.cbType==SCORE_TYPE_WIN?m_pGameParameter->dwWinExperience:0;
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore,ScoreInfo.lGrade,ScoreInfo.lRevenue,dwUserMemal,ScoreInfo.cbType,dwPlayTimeCount,dwWinExperience);

		//提取积分
		tagVariationInfo VariationInfo;
		pIServerUserItem->DistillVariation(VariationInfo);

		m_PersonalUserScoreInfo[wChairID].dwUserID = pIServerUserItem->GetUserID();		//用户ID
		//用户昵称
		lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, pIServerUserItem->GetNickName(), sizeof(m_PersonalUserScoreInfo[wChairID].szUserNicname));
		m_PersonalUserScoreInfo[wChairID].lScore += ScoreInfo.lScore;							//用户分数
		m_PersonalUserScoreInfo[wChairID].lGrade += ScoreInfo.lGrade;							//用户成绩
		m_PersonalUserScoreInfo[wChairID].lTaxCount += ScoreInfo.lRevenue;					//用户税收

		//游戏记录
		if (pIServerUserItem->IsAndroidUser()==false && CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//查询库存
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//获取对象
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//删除对象
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//创建对象
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//创建对象
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("游戏记录对象创建失败");
				}
				catch (...)
				{
					ASSERT(FALSE);
				}
			}

			//记录数据
			if (pGameScoreRecord!=NULL)
			{
				//用户信息
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//用户信息
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//成绩信息
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;

				//附加信息
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//机器人免税
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//插入数据
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//游戏记录
		if(dwGameMemal != INVALID_DWORD || dwPlayGameTime!=INVALID_DWORD)
		{
			DWORD dwStartGameTime=INVALID_DWORD;
			if(dwPlayGameTime!=INVALID_DWORD)
			{
				dwStartGameTime=(dwCurrentTime-dwPlayGameTime);
			}
			RecordGameScore(true, dwStartGameTime);
		}
	}
	else
	{
		////离开用户
		//CTraceService::TraceString(TEXT("系统暂时未支持离开用户的补分操作处理！"),TraceLevel_Exception);

		//return false;
	}

	//广播消息
	if (szMessage[0]!=0)
	{
		//变量定义
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//游戏玩家
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//发送消息
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//旁观用户
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//发送消息
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//写入积分
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//效验参数
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//写入分数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	return true;
}

//计算税收
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;

	//计算税收
	if ((m_pGameServiceOption->wRevenueRatio>0 || m_PersonalRoomOption.lPersonalRoomTax > 0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//获取用户
		ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//计算税收
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			lRevenue=lScore*(m_pGameServiceOption->wRevenueRatio + m_PersonalRoomOption.lPersonalRoomTax)/REVENUE_DENOMINATOR ;
		}

		return lRevenue;
	}

	return 0L;
}

//消费限额
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//用户效验
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//查询额度
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//效验额度
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//寻找用户
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}
//寻找用户
IServerUserItem * CTableFrame::SearchUserItemGameID(DWORD dwGameID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetGameID()==dwGameID)) return pIServerUserItem;
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetGameID()==dwGameID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}
//游戏用户
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_TableUserItemArray[wChairID];
}

//旁观用户
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//设置时间
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//效验状态
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//发送消息
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendRoomMessage(lpszMessage,wType);
	}

	//用户效验
	/*ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;*/

	//发送消息
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//游戏消息
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送消息
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//发送场景
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//发送场景
	ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//设置接口
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	if (pIUnknownEx==NULL) return false;

	//类型判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//查询接口
	m_pITableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameHook);
	m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableUserAction);

	return true;
}

//伪造配置
bool CTableFrame::ImitateGameOption(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//状态判断
	if (m_bGameStarted==true) return true;
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_PLAYING) return false;

	//类型判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;	
	
	//设置状态
	pIServerUserItem->SetClientReady(true);

	//开始判断
	if ((pIServerUserItem->GetUserStatus()==US_READY)&&(EfficacyStartGame(pIServerUserItem->GetChairID())==true))
	{
		StartGame(); 
	}

	return true;
}

bool CTableFrame::SendChatMessage(IServerUserItem * pIServerUserItem,IServerUserItem * pITargetServerUserItem,DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//获取目标
	DWORD dwTargetUserID = 0;
	if (pITargetServerUserItem!=NULL)
	{
		dwTargetUserID = pITargetServerUserItem->GetUserID();
	}

	DWORD wDescribeLength = lstrlen(lpszDescribeString) + 1;
	if (wDescribeLength>LEN_USER_CHAT) wDescribeLength=LEN_USER_CHAT;
	WORD wChatLength = lstrlen(lpszChatString) + 1;
	if (wChatLength>LEN_USER_CHAT) wChatLength=LEN_USER_CHAT;

	//状态判断
	if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		lstrcpyn(lpszDescribeString,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),wDescribeLength);
		return false;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
	{
		lstrcpyn(lpszDescribeString,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),wDescribeLength);
		return true;
	}

	//构造消息
	CMD_GF_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//字符过滤
	m_pIMainServiceFrame->SensitiveWordFilter(lpszChatString,UserChat.szChatString,wChatLength);

	//构造数据
	UserChat.dwChatColor=dwChatColor;
	UserChat.wChatLength=wChatLength;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=pIServerUserItem->GetUserID();

	//发送数据
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

	//游戏用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
	}

	//旁观用户
	WORD wEnumIndex=0;
	IServerUserItem * pIWatchServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIWatchServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIWatchServerUserItem==NULL) break;

		//发送数据
		if (pIServerUserItem->IsClientReady()==true)
		{
			m_pIMainServiceFrame->SendData(pIWatchServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
		}
	} while (true);

	return true;
}

//断线事件
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户变量
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//游戏用户
	if (cbUserStatus!=US_LOOKON)
	{
		//效验用户
		ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//断线处理
		if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE) || 
			m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&
			(m_PersonalTableParameter.cbIsJoinGame && m_dwTableOwnerID == pUserInfo->dwUserID || (m_dwPersonalPlayCount != 0 && m_dwDrawCountLimit > m_dwPersonalPlayCount))
			)
		{
			//用户设置
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//比赛类型
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(true);

					//掉线通知
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
				}

				return true;
			}
			else
			{
				//断线处理		
				if (cbUserStatus==US_PLAYING)
				{
					if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
					{
						//用户设置
						if(m_pITableUserAction!=NULL && m_pGameServiceOption->wKindID == 306) 
						{
							m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
						}
						pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
						pIServerUserItem->SetClientReady(true);
					}
					else
					{
						PerformStandUpAction(pIServerUserItem);
						pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);		
					}
		
				}else if(cbUserStatus==US_READY || cbUserStatus==US_SIT)  //处理用户准备后/前断线问题
				{
					PerformStandUpAction(pIServerUserItem);	
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);			
				}
				return true;
			}
			//掉线通知
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			//断线处理
			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				dwElapse = m_PersonalTableParameter.dwTimeOffLineCount;
			}
			
			//断线处理
			if (m_dwOffLineTime[wChairID]==0L)
			{
				//设置变量
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//时间设置
				WORD wOffLineCount=GetOffLineUserCount();

				if (wOffLineCount==1)
				{
					if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse * 60 * 1000 ,1,wChairID);
					}
					else
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse ,1,wChairID);
					}
				}
			}

			return true;
		}
		else
		{
			//断线处理		
			if (cbUserStatus==US_OFFLINE)
			{
				if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
				{
					//用户设置
					pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
					pIServerUserItem->SetClientReady(true);
				}
				else
				{
					PerformStandUpAction(pIServerUserItem);
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);		
				}
			}else if(cbUserStatus==US_READY || cbUserStatus==US_SIT)  //处理用户准备后/前断线问题
			{
				PerformStandUpAction(pIServerUserItem);	
				pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);			
			}
			return true;

		}

	}

	//用户起立
	PerformStandUpAction(pIServerUserItem,true);
	
	//比赛处理
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//删除用户
	ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//积分事件
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//通知游戏
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//时间事件
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//断线事件
		{
			//效验状态
			ASSERT(m_bGameStarted==true || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL);
			if (m_bGameStarted==false && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL) return false;

			//变量定义
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//寻找用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//位置判断
			ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//用户判断
			ASSERT(dwBindParameter<m_wChairCount);

			//私人房间断线超时跳过，在下面处理
			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				break;
				//return true;
			}

			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)	dwElapse = TIME_OFF_LINE_PERSONAL;

			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//时间计算
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//设置时间
				dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
				SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//获取用户
			ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//结束游戏
			if (pIServerUserItem!=NULL)
			{
				//设置变量
				m_dwOffLineTime[wOffLineChair]=0L;

				//用户起立
				PerformStandUpAction(pIServerUserItem,true);
			}

			//继续时间
			if (m_bGameStarted==true)
			{
				//变量定义
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//寻找用户
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//设置时间
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//时间计算
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//设置时间
					dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
					SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
	}

	//未开始超时
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//变量定义
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//超时处理
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			//用户起立
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//约战未开始游戏超时
	if(dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME  || dwTimerID ==  IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
	{
		if (dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME)
		{
			OutputDebugStringA("ptdtcs **** IDI_PERSONAL_BEFORE_BEGAIN_TIME");
		}

		if (dwTimerID == IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
		{
			OutputDebugStringA("ptdtcs **** IDI_PERSONAL_BEFORE_BEGAIN_TIME");
		}
		//桌子状态
		if(m_bGameStarted == true && m_bPersonalLock == true) return true;

		m_bIsEnfoceDissum = true;
		m_bIsDissumGame = true;
		//退还费用
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_NOT_START ,  m_wTableID,  m_szPersonalTableID);//CANCELTABLE_REASON_ENFOCE
		return true;
	}

	//游戏开始之后超时 游戏断线超时
	if(dwTimerID == IDI_PERSONAL_AFTER_BEGIN_TIME  || dwTimerID == IDI_OFF_LINE  &&  m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL )
	{
		HostDissumeGame();

		return true;
	}
	if (IDI_ANDROID_THEJOINT == dwTimerID)
	{
		KillGameTimer(IDI_ANDROID_THEJOINT);
		SetGameTimer(IDI_ANDROID_THEJOINT, TIME_ANDROID_THEJOINT,1,NULL);
		DWORD dwTheJoint = rand()%4;
		if (dwTheJoint == 0)return true;
		DWORD dwIndex = 0;
		//游戏用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsAndroidUser()==false)) continue;
			//绑定信息
			CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

			//查找机器
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
			if(pIAndroidUserItem==NULL) continue;
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();
			if (pAndroidParameter == NULL)continue;
			if((pAndroidParameter->dwServiceMode&ANDROID_THEJOINT)!=0)
			{
				PerformStandUpAction(pIServerUserItem);
				dwIndex++;
			}
			if (dwIndex >= dwTheJoint)
			{
				break;
			}
		}
	}
	//错误断言
	ASSERT(FALSE);

	return false;
}

//游戏事件
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架事件
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//游戏处理
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//游戏配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//if (m_pGameServiceOption->lCellScore < 1)
			//{
			//	m_pGameServiceOption->lCellScore  = 1;
			//}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//构造提示
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{

					_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					return false;
				}

			}

			//变量定义
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//断线清理
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//设置变量
				m_dwOffLineTime[wChairID]=0L;

				//删除时间
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0)
				{
					//OutputDebugStringA("ptdt *** OnEventSocketFrame 杀死 断线定时器 开始超时定时器");
					//LogPrintf(("OnEventSocketFrame 杀死 断线定时器 开始超时定时器"));
					KillGameTimer(IDI_OFF_LINE);
					KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
					OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 1");
				}
			}

			//设置状态
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//发送状态
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//提示信息
			CMD_GR_PersonalTableTip PersonalTableTip;
			ZeroMemory(&PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			PersonalTableTip.dwTableOwnerUserID = m_dwTableOwnerID;
			PersonalTableTip.dwDrawCountLimit = m_dwDrawCountLimit;
			PersonalTableTip.dwDrawTimeLimit = m_dwDrawTimeLimit;
			PersonalTableTip.dwPlayCount = m_dwPersonalPlayCount;
			PersonalTableTip.dwPlayTime = (DWORD)time(NULL) - m_dwTimeBegin;
			PersonalTableTip.lCellScore = m_lCellScore;
			PersonalTableTip.lIniScore = m_lIniScore;
			lstrcpyn(PersonalTableTip.szServerID, m_szPersonalTableID, CountArray(PersonalTableTip.szServerID));
			PersonalTableTip.cbIsJoinGame = m_PersonalTableParameter.cbIsJoinGame;

			//如果使用的是金币数据库
			if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
			{
				PersonalTableTip.cbIsGoldOrGameScore = 0;
			}
			else
			{
				PersonalTableTip.cbIsGoldOrGameScore = 1;
			}


			m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_TIP, &PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			//发送消息
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("欢迎您进入“%s”游戏，祝您游戏愉快！"),m_pGameServiceAttrib->szGameName);
			m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);


			//发送场景
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);
			

				//准备人数

			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pITableUserItem=GetTableUserItem(i);
				if (pITableUserItem != NULL) continue;
							//发送状态
				CMD_GF_GameUserData GameUserData;
				GameUserData.cbUserCharID=i;
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_DATA,&GameUserData,sizeof(GameUserData));
			}

			//取消托管
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(false);
				}

				//回归通知
				if (cbUserStatus==US_PLAYING || cbUserStatus==US_OFFLINE)
				{
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserConnect(wChairID,pIServerUserItem);
					if(m_pITableFrameHook) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);
				}
			}

			//开始判断
			if ((cbUserStatus==US_READY)&&(EfficacyStartGame(wChairID)==true))
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_READY:		//用户准备
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD  && pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore);
				//发送消息
				if (pIServerUserItem->IsAndroidUser()==true)
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_CLOSE_ROOM|SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

				//用户起立
				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					PerformStandUpAction(pIServerUserItem);
				}
				return true;
			}

			//效验状态
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//防作弊分组判断
			if((CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("系统重新分配桌子，请稍后！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//发送消息
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////用户起立
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//事件通知
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//事件通知
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			//删除定时
			if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

			//开始判断
			if (EfficacyStartGame(wChairID)==false)
			{
				//设置状态
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_ANDEOID_QUIT:
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			PerformStandUpAction(pIServerUserItem);
			return true;
		}
	case SUB_GF_USER_CHAT:		//用户聊天
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

			//目标用户
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//字符过滤
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//构造数据
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//发送数据
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//用户表情
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//变量定义
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//目标用户
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GR_S_UserExpression UserExpression;
			ZeroMemory(&UserExpression,sizeof(UserExpression));

			//构造数据
			UserExpression.wItemIndex=pUserExpression->wItemIndex;
			UserExpression.dwSendUserID=pIServerUserItem->GetUserID();
			UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//发送数据
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_VOICE:	//用户语音
		{
			//变量定义
			CMD_GF_C_UserVoice * pUserVoice=(CMD_GF_C_UserVoice *)pData;

			//效验参数
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserVoice));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)));
			ASSERT(wDataSize==(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0])));

			//效验参数
			if (wDataSize>sizeof(CMD_GF_C_UserVoice)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0]))) return false;

			//目标用户
			if ((pUserVoice->dwTargetUserID!=0)&&(SearchUserItem(pUserVoice->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GF_S_UserVoice UserVoice;
			ZeroMemory(&UserVoice,sizeof(UserVoice));

			//构造数据
			UserVoice.dwSendUserID=pIServerUserItem->GetUserID();
			UserVoice.dwTargetUserID=pUserVoice->dwTargetUserID;
			UserVoice.dwVoiceLength = pUserVoice->dwVoiceLength>MAXT_VOICE_LENGTH?MAXT_VOICE_LENGTH:pUserVoice->dwVoiceLength;
			CopyMemory(UserVoice.byVoiceData,pUserVoice->byVoiceData,UserVoice.dwVoiceLength);

			//发送数据
			WORD wHeadSize=sizeof(UserVoice)-sizeof(UserVoice.byVoiceData);
			WORD wSendSize=(WORD)(wHeadSize+UserVoice.dwVoiceLength*sizeof(UserVoice.byVoiceData[0]));

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//发送数据
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
			}

			//旁观用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//发送数据
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//旁观配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//发送消息
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//设置判断
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//设置变量
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//发送消息
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//发送消息
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	}

	return false;
}

//游戏局数
WORD CTableFrame::GetDrawCount()
{
	return m_wDrawCount;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	//椅子搜索
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//随机空位
WORD CTableFrame::GetRandNullChairID()
{
	//椅子搜索
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}
//桌子机器人数量
WORD CTableFrame::GetTableAndroidConut()
{
	WORD wAnddroidCount = 0;
	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->IsAndroidUser())
			wAnddroidCount++;
	}
	if (wAnddroidCount == m_pGameServiceAttrib->wChairCount)
	{
		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("机器人有满桌"));

		//提示消息
		CTraceService::TraceString(szString,TraceLevel_Normal);
	}
	return wAnddroidCount;
}
//用户数目
WORD CTableFrame::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//旁观数目
WORD CTableFrame::GetLookonUserCount()
{
	//获取数目
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//断线数目
WORD CTableFrame::GetOffLineUserCount()
{
	//变量定义
	WORD wOffLineCount=0;

	//断线人数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//桌子状况
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//设置变量
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//准备判断
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//最少数目
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//所有准备
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//配对开始
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//时间控制
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//默认模式
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//配置桌子
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameMatchOption=TableFrameParameter.pGameMatchOption;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//组件接口
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	
	//设置变量
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//设置桌子
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//设置标识
	m_bTableInitFinish=true;

	//扩展接口
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	//读取配置
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\PersonalTable.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	m_PlayerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("PlayerWaitTime"),120);
	m_TableOwnerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("TableOwnerWaitTime"),1800);
	m_GameStartOverTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("GameStartOverTime"),900);

	return true;
}

//起立动作
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	DWORD dwTimeNow = (DWORD)time(NULL);

	//判断是否是金币场
	bool bTreasureDB = false;
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
	{
		bTreasureDB = true;
	}

	//游戏用户
	if (
		(
		(m_bGameStarted==true)&&
		((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)) &&
		m_pGameServiceOption->wServerType!=GAME_GENRE_PERSONAL
		) ||
		(
		((m_pGameServiceOption->lMinTableScore!=0L) && (pIServerUserItem->GetUserScore()>=m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore && bTreasureDB || !bTreasureDB)) &&
		(m_PersonalTableParameter.cbIsJoinGame || m_dwPersonalPlayCount != 0 ) && !m_bIsDissumGame &&//如果房主参与游戏,且不是强制解散游戏
		(m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL && (m_dwPersonalPlayCount != 0  || (m_dwTableOwnerID == pIServerUserItem->GetUserID() && ((m_dwDrawCountLimit!=0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount) ||
		(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit <= dwTimeNow - m_dwTimeBegin)) )))
		)
		)
	{	
		//比赛类型，私人房间类型
		if((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)||
			m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL && (m_PersonalTableParameter.cbIsJoinGame ||   
			(m_dwPersonalPlayCount != 0  && (
			( 
			(m_dwDrawCountLimit!=0 && m_dwDrawCountLimit  >= m_dwPersonalPlayCount) ||
			(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit >= dwTimeNow - m_dwTimeBegin)
			) 
			)
			)))
		{

			//设置托管
			pIServerUserItem->SetTrusteeUser(true);

			//掉线通知
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			return true;
		}
		if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
		{
			//用户设置
			if(m_pITableUserAction!=NULL && m_pGameServiceOption->wKindID == 306) //
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
			//设置托管
			pIServerUserItem->SetTrusteeUser(true);

			//-------------------------------//
			pIServerUserItem->SetClientReady(true);
			return true;
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID); 
			//-------------------------------//
			pIServerUserItem->SetClientReady(true);
		}

		//结束游戏
		BYTE cbConcludeReason=(cbUserStatus==US_OFFLINE)?GER_NETWORK_ERROR:GER_USER_LEAVE;
		m_pITableFrameSink->OnEventGameConclude(wChairID,pIServerUserItem,cbConcludeReason);

		//离开判断
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	//比赛场托管处理
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) && (m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE))
	{
		//获取状态
		BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

		//判断状态
		if (bInitiative==true && cbUserMatchStatus==MUS_PLAYING)
		{
			if (pIServerUserItem->IsTrusteeUser()==false)
			{
				pIServerUserItem->SetTrusteeUser(true);

				//掉线通知
				if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}	
			return true;
		}
	}

	//设置变量
	if (pIServerUserItem==pITableUserItem)
	{
		//删除定时
		if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

		//设置变量
		m_TableUserItemArray[wChairID]=NULL;

		//解锁游戏币
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//事件通知
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//事件通知
		if(m_pIMatchTableAction!=NULL && bInitiative==true)
		{
			m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//比赛类型
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE )
		{
			//获取状态
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

			if (cbUserStatus==US_OFFLINE && cbUserMatchStatus==MUS_PLAYING)
			{
				CString strMessage;
				strMessage.Format(TEXT("即时赛中出现玩家: %d起立时状态为US_OFFLINE的情况"),pIServerUserItem->GetUserID());

				//提示信息
				CTraceService::TraceString(strMessage,TraceLevel_Exception);
			}
		}

		//用户状态
		pIServerUserItem->SetClientReady(false);
		if (cbUserStatus == US_NULL)
		{
			if (pIServerUserItem->GetUserInfo()->dwUserID == 6346   || pIServerUserItem->GetUserInfo()->dwUserID == 6354)
			{
				printf(("游戏结束玩家托管退出________________________________________%d\n"),pIServerUserItem->GetUserInfo()->dwUserID);
			}
			pIServerUserItem->SetUserStatus((cbUserStatus==US_NULL)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		}
		else
		{
			pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);
		}


		//退还费用
		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{	
			pIServerUserItem->SetClientReady(false);
			if((((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()>=m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) ) &&
				m_dwPersonalPlayCount != 0 && m_dwTableOwnerID == pIServerUserItem->GetUserID() && m_bPersonalLock == true )
			{
				//退还费用
				KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
				OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 2");
				m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_PLAYER, m_wTableID, m_szPersonalTableID);
			}

			if (!m_bBeOut[wChairID] )
			{
				if(cbUserStatus == US_OFFLINE)
				{
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
				}
				else
				{
					//LogPrintf(("PerformStandUpAction 非 断线 设置桌子为空"));
					pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
				}
			}
		}

		//变量定义
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//设置变量
		m_wUserCount=wTableUserCount;

		//桌子信息
		if (wTableUserCount==0 && m_pGameServiceOption->wServerType!=GAME_GENRE_PERSONAL)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		if (wTableUserCount==0 && m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL &&  m_PersonalTableParameter.cbIsJoinGame)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		//踢走旁观
		if (wTableUserCount==0)
		{
			for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
			{
				SendGameMessage(m_LookonUserItemArray[i],TEXT("此游戏桌的所有玩家已经离开了！"),SMT_CLOSE_GAME|SMT_EJECT);
			}
		}

		//结束桌子
		ConcludeTable();

		//开始判断
		if (m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH && EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//发送状态
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//防作弊分组判断
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			&&m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
		{
			//防作弊分组判断
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//发送消息
						LPCTSTR pszMessage=TEXT("由玩家离开，系统重新分配桌子，请稍后！");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//发送消息
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);

						////用户起立
						//PerformStandUpAction(m_TableUserItemArray[i]);
					}
				}
			}
		}

		return true;
	}
	else
	{
		//起立处理
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//事件通知
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}

				//事件通知
				if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		//错误断言
		ASSERT(FALSE);
	}

	return true;
}

//旁观动作
bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//游戏状态
	if ((m_bGameStarted==false)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("游戏还没有开始，不能旁观此游戏桌！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//模拟处理
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//定义变量
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//查找机器
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//获取参数
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//模拟判断
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}


	//旁观判断
	if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule)==true
		&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==false))
	{
		if ((pITableUserItem!=NULL)&&(pITableUserItem->IsAndroidUser()==true))
		{
			SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
			return false;
		}
	}

	//状态判断
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止用户旁观！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//椅子判断
	if ((pITableUserItem==NULL)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("您所请求的位置没有游戏玩家，无法旁观此游戏桌"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//密码效验
	if ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L)&&(lstrcmp(pUserRule->szPassword,m_szEnterPassword)!=0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("游戏桌进入密码不正确，不能旁观游戏！"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//扩展效验
	if (m_pITableUserRequest!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableUserRequest->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//发送信息
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//事件通知
	if (m_pITableUserAction!=NULL)m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);

	//事件通知
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);

	return true;
}

//坐下动作
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword)
{
	
	//// 如果私人房已经结束
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL  &&( m_dwTableOwnerID == 0 && m_dwPersonalPlayCount > 0   || m_bIsEnfoceDissum))
	{
		//发送信息
		SendRequestFailure(pIServerUserItem,TEXT("私人房间已经解散！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//积分变量
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	//if (m_pGameServiceOption->lCellScore < 1)
	//{
	//	m_pGameServiceOption->lCellScore  = 1;
	//}
	if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		//构造提示
		Personal_Room_Message personalRoomMessage;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("您的游戏币少于 ") SCORE_STRING TEXT("，不能继续游戏！"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
			return false;

		}

	}

	//如果是私人房，且已经坐桌的人数大于或等于配置的人数 则不允许坐桌
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && GetSitUserCount() >= m_wChairCount)
	{
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子人数已满！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//状态判断
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//模拟处理
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//定义变量
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//查找机器
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//获取参数
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//模拟判断
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}

	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//游戏状态
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		SendRequestFailure(pIServerUserItem,TEXT("游戏已经开始了，现在不能进入游戏桌！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//椅子判断
	if (pITableUserItem!=NULL)
	{
		//防作弊
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return false;

		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"),pITableUserItem->GetNickName());

		//发送信息
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

		return false;
	}

	//密码效验
	if (m_wChairCount < MAX_CHAIR)
	{
		//防作弊
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
			{
				SendRequestFailure(pIServerUserItem,TEXT("游戏桌进入密码不正确，不能加入游戏！"),REQUEST_FAILURE_PASSWORD);
				return false;
			}
		}
	}

	//规则效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//扩展效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//发送信息
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//设置变量
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	//校正分数
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		if(m_bPersonalLock == false)
		{
			//发送信息
			SendRequestFailure(pIServerUserItem,TEXT("房间还未创建，无法进入！"),REQUEST_FAILURE_NORMAL);
			return false;
		}

		pIServerUserItem->ModifyUserInsure(m_lIniScore,0,0);

	}

	//用户状态
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
		}
		else
		{
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
		}
	}
	else
	{
		//设置变量
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//锁定游戏币
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID]=m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
		}

		//设置状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//设置变量
	m_wUserCount=GetSitUserCount();

	//桌子信息
	if (GetSitUserCount()==1)
	{
		//状态变量
		bool bTableLocked=IsTableLocked();

		//设置变量
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
		}
		lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));

		//私人桌子
		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			m_bWaitConclude = false;
			m_dwPersonalPlayCount = 0;
			m_MapPersonalTableScoreInfo.RemoveAll();

			//开始超时
			SetGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME, m_PersonalTableParameter.dwTimeNotBeginGame * 60 * 1000,1,NULL);
		}

		//发送状态
		if (bTableLocked!=IsTableLocked()) SendTableStatus();
	}

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
	{
		//启动定时
		if(IsGameStarted()==false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
		{
			SetGameTimer(IDI_START_OVERTIME+wChairID,TIME_OVERTIME,1,wChairID);
		}
	}


	//关闭创建开始后无人坐桌
	KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

	m_bBeOut[wChairID]  = false;

	//事件通知
	if (m_pITableUserAction!=NULL)
	{
		if (m_wChairCount >= MAX_CHAIR)
		{
			if (m_bFirstRegister == false)
			{
				SetGameTimer(IDI_ANDROID_THEJOINT, TIME_ANDROID_THEJOINT,1,NULL);
			}
			m_bFirstRegister = true;
			m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
		}
		else
		{
			m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
		}

	}

	//事件通知
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);

	return true;
}

//单元积分
VOID CTableFrame::SetCellScore(SCORE lCellScore)
{
	m_lCellScore=lCellScore;
	if (m_bTableInitFinish==true)
	{
		SendTableStatus();
		m_pITableFrameSink->SetGameBaseScore(lCellScore);
	}
}

//桌子状态
bool CTableFrame::SendTableStatus()
{
	//变量定义
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//构造数据
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//电脑数据
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//手机数据
	m_pIMainServiceFrame->SendData(BG_MOBILE,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//请求失败
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//构造数据
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//开始效验
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	//状态判断
	if (m_bGameStarted==true) return false;

	//模式过滤
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//准备人数
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			//状态判断
			if (pITableUserItem->IsClientReady()==false) return false;
			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY)) return false;

			//用户计数
			wReadyUserCount++;
		}
	}

	//开始处理
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//所有准备
		{
			//数目判断
			if (wReadyUserCount>=2L) return true;

			return false;
		}
	case START_MODE_FULL_READY:			//满人开始
		{
			//人数判断
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//配对开始
		{
			//数目判断
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//位置判断
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//获取用户
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//位置过滤
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//地址效验
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount >= MAX_CHAIR) return true;

	//地址效验
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//地址效验
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//发送信息
					LPCTSTR pszDescribe=TEXT("此游戏桌玩家设置了不跟相同 IP 地址的玩家游戏，您 IP 地址与此玩家的 IP 地址相同，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					//发送信息
					LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//获取用户
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//积分效验
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount >= MAX_CHAIR) return true;

	//变量定义
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			//获取规则
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true)
			{
				//最高积分
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//最低积分
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

//积分效验
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//积分变量
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);
	if (pIServerUserItem->IsAndroidUser()) return true;   //机器人直接跳过。
	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏币，您的游戏币不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));
		else if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的比赛币，您的比赛币不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏积分，您的积分不够，不能加入！"),__max(lMinTableScore,lLessEnterScore));

			}
			else
			{
				return true;
			}
		}

		//发送信息
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);

		return false;
	}

	return true;
}


//检查分配
bool CTableFrame::CheckDistribute()
{
	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
	{
		//桌子状况
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//用户起立
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//游戏记录
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//写入记录
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//设置变量
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//游戏时间
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//用户积分
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//获取对象
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//用户数目
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//奖牌统计
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//统计信息
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//成绩信息
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//投递数据
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//清理记录
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

//请求解散
bool CTableFrame::CancelTableRequest(DWORD dwUserID, WORD dwChairID)
{
	//校验用户
	IServerUserItem* pUserItem = GetTableUserItem(dwChairID);
	if(pUserItem == NULL) return false;
	if(pUserItem->GetUserID() != dwUserID) return false;

	if(m_wUserCount == 1 && pUserItem->GetUserID() == m_dwTableOwnerID)//m_PersonalTableParameter.cbIsJoinGame)
	{
		//退还费用
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 3");
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE,  m_wTableID, m_szPersonalTableID);
		m_dwPersonalPlayCount = 0;

		return true;
	}
	m_dwReplyCount = 1;
	ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
	m_bRequestReply[dwChairID] = true;

	return true;
}

//解散答复
bool CTableFrame::CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree)
{
	//查找用户
	IServerUserItem* pUserItem = SearchUserItem(dwUserID);
	if(pUserItem == NULL) return false;

	//答复设置
	WORD wChairID = pUserItem->GetChairID();
	if(cbAgree == 0)
	{
		m_bRequestReply[wChairID] = false;			//不同意
	}
	else
	{
		m_bRequestReply[wChairID] = true;			//同意
	}
	m_dwReplyCount++;

	//结果判断
	bool bCancel = false;
	WORD wOnlineCount = 0;
	WORD wAgreeCount = 0;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if(pIServerUserItem == NULL) continue;
		if(pIServerUserItem->GetUserStatus() != US_OFFLINE) wOnlineCount++;
		//if(m_dwOffLineTime[i] == 0 && GetTableUserItem(i) != NULL) wOnlineCount++;
	}

	if(m_dwReplyCount == wOnlineCount)
	{
		for(int i = 0; i <m_wChairCount; ++i)
		{
			if(m_bRequestReply[i])
			{
				wAgreeCount++;
			}
		}
	}

	if(m_dwReplyCount < wOnlineCount) return true;
	if(wAgreeCount >= wOnlineCount || wAgreeCount >= (m_wChairCount/2 + 1)) bCancel = true;

	//请求结果
	CMD_GR_RequestResult RequestResult;
	ZeroMemory(&RequestResult, sizeof(CMD_GR_RequestResult));
	RequestResult.dwTableID = m_wTableID;
	RequestResult.cbResult = bCancel?1:0;	//1 ： 解散桌子 0 ： 继续游戏
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pTableUserItem = GetTableUserItem(i);
		if(pTableUserItem == NULL) continue;
		m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));

	}

	//解散桌子
	if(bCancel == true)
	{
		m_bIsAllRequstCancel = true;
		m_bIsDissumGame = true;
		if(m_bGameStarted == true)
		{
			//解散游戏
			DismissGame();
		}

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4");

		TCHAR szInfo[260] = {0};	
		//用户成绩
		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("玩家申请解散，游戏提前结束！"), CountArray(PersonalTableEnd.szDescribeString));
		for(int i = 0; i < m_wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
		}

		//添加特殊信息
		memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;


		if(m_dwPersonalPlayCount == 0)		//游戏未开始
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_szPersonalTableID);
		}
		else	//游戏开始
		{
			m_dwPersonalPlayCount = 0;
			WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

				//	PerformStandUpAction(pTableUserItem);
			}

			//防止客户端因为状态为free获取不到用户指针
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				PerformStandUpAction(pTableUserItem);
			}
			m_pIMainServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, m_wTableID);
		}

		//桌子设置
		m_pTableOwnerUser = NULL;
		//m_bPersonalLock = false;
		m_dwTimeBegin = 0;
		m_dwReplyCount = 0;
		ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
		m_MapPersonalTableScoreInfo.RemoveAll();

	}

	return true;
}

//私人编号
void CTableFrame::SetPersonalTableID(LPCTSTR szTableID)
{
	if(szTableID == NULL)
	{
		ZeroMemory(m_szPersonalTableID, sizeof(m_szPersonalTableID));
	}
	else
	{
		lstrcpyn(m_szPersonalTableID, szTableID, CountArray(m_szPersonalTableID));
		m_bIsAllRequstCancel = false;
		memset(m_bBeOut, 0, sizeof(m_bBeOut));
		m_bIsDissumGame = false;
	}

	return;
}

//私人编号
TCHAR * CTableFrame::GetPersonalTableID()
{
	return m_szPersonalTableID;
}

//设置桌子上椅子的个数
VOID CTableFrame::SetTableChairCount(WORD wChairCount)
{
	m_wChairCount = wChairCount;
	m_pGameServiceAttrib->wChairCount = wChairCount;
	m_PersonalTableParameter.wJoinGamePeopleCount = wChairCount;
}

//设置桌子
VOID CTableFrame::SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore)
{
	m_dwDrawCountLimit = dwDrawCountLimit;
	m_dwDrawTimeLimit = dwDrawTimeLimit;
	m_lIniScore = lIniScore;

	return;
}

//设置桌子参数
VOID CTableFrame::SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter,  tagPersonalRoomOption PersonalRoomOption )
{
	m_PersonalTableParameter = PersonalTableParameter;
	m_dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
	m_dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
	m_PersonalRoomOption = PersonalRoomOption;

	m_dwPersonalPlayCount = 0; 
	m_bIsEnfoceDissum = false;
}

//获取分数
bool CTableFrame::GetPersonalScore(DWORD dwUserID, LONGLONG& lScore)
{
	LONGLONG lScoretmp = 0;
	lScore = 0;
	if(m_MapPersonalTableScoreInfo.Lookup(dwUserID, lScoretmp) == TRUE)
	{
		lScore = lScoretmp;
		return true;
	}
	return false;
}

//桌子信息
tagPersonalTableParameter CTableFrame::GetPersonalTableParameter()
{
	tagPersonalTableParameter PersonalTableParameter;
	ZeroMemory(&PersonalTableParameter, sizeof(tagPersonalTableParameter));

	PersonalTableParameter.dwPlayTimeLimit = m_dwDrawTimeLimit;
	PersonalTableParameter.dwPlayTurnCount = m_dwDrawCountLimit;
	PersonalTableParameter.lIniScore = m_lIniScore;

	return PersonalTableParameter;
}

//桌子创建后多长时间未开始游戏 解散桌子
VOID CTableFrame::SetTimerNotBeginAfterCreate()
{
	if (m_PersonalTableParameter.cbIsJoinGame)
	{
		return;
	}

	SetGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME, m_PersonalTableParameter.dwTimeAfterCreateRoom * 60 * 1000,1,NULL);
}

//房主强制解散房间
void CTableFrame::HostDissumeGame(BOOL bIsEnforce )
{
	m_bIsDissumGame = true;
	if(m_bGameStarted == true)
	{
		//解散游戏
		DismissGame();
	}

	//结束消息
	CMD_GR_PersonalTableEnd PersonalTableEnd;
	ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
	lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("约战结束。"), CountArray(PersonalTableEnd.szDescribeString));
	for(int i = 0; i < m_wChairCount; ++i)
	{
		//IServerUserItem* pUserItem = m_TableUserItemArray[i];
		//if(pUserItem == NULL) continue;
		//PersonalTableEnd.lScore[i] = pUserItem->GetUserScore();
		PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;

	}

	//添加特殊信息
	memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
	PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;
	TCHAR szInfo[260] = {0};


	WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[i];
		if(pUserItem == NULL ) 
		{
			continue;
		}

		//约战结果
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

		//构造数据
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		if (bIsEnforce)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_ENFOCE;
			lstrcpyn(CancelTable.szDescribeString, TEXT("房主强制解散游戏！"), CountArray(CancelTable.szDescribeString));
		}
		else
		{
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏自动解散！"), CountArray(CancelTable.szDescribeString));
		}


		//解散消息
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//用户状态
		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		//PerformStandUpAction(pUserItem);
	}

	if(!bIsEnforce)
	{
		if (m_dwPersonalPlayCount == 0 )
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_ENFOCE ,  m_wTableID,  m_szPersonalTableID);//CANCELTABLE_REASON_ENFOCE
		}
		else
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_OVER_TIME ,  m_wTableID,  m_szPersonalTableID);
		}
	}

	m_pTableOwnerUser = NULL;
	m_dwTableOwnerID = 0;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_dwTimeBegin = 0;
	m_dwReplyCount = 0;
	ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
	m_MapPersonalTableScoreInfo.RemoveAll();
	m_dwPersonalPlayCount = 0;
	m_bIsEnfoceDissum = true;
}

//设置游戏规则
VOID CTableFrame::SetGameRule(byte * cbRule,  int nLen)
{
	memcpy(m_cbGanmeRule,  cbRule, sizeof(m_cbGanmeRule));
}

//获取游戏规则
byte * CTableFrame::GetGameRule()
{
	return m_cbGanmeRule;
}

//获取结算时的特殊信息
void CTableFrame::GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen)
{
	memcpy(m_cbSpecialInfo, cbSpecialInfo, nSpecialLen);
	m_nSpecialInfoLen = nSpecialLen;
}

DWORD CTableFrame::GetRecordTableOwner()
{
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** GetRecordTableOwner = %d "),  m_dwRecordTableID);
	OutputDebugString(szInfo);
		return  m_dwRecordTableID; 
}

//私人房间是否解散
bool CTableFrame::IsPersonalRoomDisumme()
{
	return m_bIsDissumGame;
}

//设置是金币数据库还是积分数据库,  0 为金币库 1 为 积分库
void CTableFrame::SetDataBaseMode(byte cbDataBaseMode)
{
		m_cbDataBaseMode = cbDataBaseMode;
}

//获取数据库模式,  0 为金币库 1 为 积分库
byte CTableFrame::GetDataBaseMode()
{
	return m_cbDataBaseMode;
}
//////////////////////////////////////////////////////////////////////////////////
