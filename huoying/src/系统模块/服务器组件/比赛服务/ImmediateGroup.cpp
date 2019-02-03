#include "StdAfx.h"
#include "ImmediateGroup.h"

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_VALUE				0xFFFF								//无效值

//静态变量
WORD CImmediateGroup::m_wChairCount;									//椅子数目

//////////////////////////////////////////////////////////////////////////
//构造函数
CImmediateGroup::CImmediateGroup(LONGLONG lMatchNo,tagGameMatchOption * pGameMatchOption,IImmediateGroupSink *pIImmediateGroupSink)
{
	//指针变量
	m_pMatchOption=pGameMatchOption;
	m_pMatchSink=pIImmediateGroupSink;
	m_pImmediateMatch=(tagImmediateMatch *)pGameMatchOption->cbMatchRule;

	//状态变量
	m_lMatchNo=lMatchNo;
	m_cbMatchStatus=MatchStatus_Signup;
	m_lCurBase=m_pImmediateMatch->lInitalBase;

	//设置变量
	ZeroMemory(&m_MatchStartTime,sizeof(m_MatchStartTime));

	//移除元素
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
	m_MatchTableArray.RemoveAll();
    
	//设置变量
	m_LoopTimer=0;
	m_wAndroidUserCount=0;	
}

//析构函数
CImmediateGroup::~CImmediateGroup(void)
{
	//杀死定时器
	AllKillMatchTimer();

	//设置状态
	m_wAndroidUserCount=0;
	m_cbMatchStatus=MatchStatus_Null;

	//释放桌子
	for (int i=0; i<m_MatchTableArray.GetCount(); i++)
	{
		SafeDelete(m_MatchTableArray[i]);
	}

	//移除元素
	m_MatchTableArray.RemoveAll();
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
}

//接口查询
VOID * CImmediateGroup::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMatchEventSink,Guid,dwQueryVer);
	return NULL;
}

//用户报名
bool CImmediateGroup::OnUserSignUpMatch(IServerUserItem * pUserItem)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//查找用户
	IServerUserItem * pIServerUserExist=NULL;
	m_OnMatchUserMap.Lookup(pUserItem->GetUserID(),pIServerUserExist);

	//用户判断
	ASSERT(pIServerUserExist==NULL);
	if (pIServerUserExist!=NULL) return false;

	//人数效验
	ASSERT(m_OnMatchUserMap.GetCount()<m_pImmediateMatch->wStartUserCount);
	if (m_OnMatchUserMap.GetCount()>=m_pImmediateMatch->wStartUserCount) return false;

	//机器判断
	if (pUserItem->IsAndroidUser()==true)
	{
		if (m_wAndroidUserCount>=m_pImmediateMatch->wAndroidUserCount)
		{
			return false;
		}
		m_wAndroidUserCount++;
	}

	//设置成绩
	tagUserInfo *pUserScore=pUserItem->GetUserInfo();
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pImmediateMatch->lInitalScore;

	//发送成绩
	m_pMatchSink->SendGroupUserScore(pUserItem,NULL);

	//设置用户
	m_OnMatchUserMap[pUserItem->GetUserID()]=pUserItem;

	//设置报名状态
	pUserItem->SetUserMatchStatus(MUS_SIGNUPED);

	//设置报名时间
	pUserItem->SetSignUpTime((DWORD)time(NULL));

	//发送报名状态
	BYTE cbUserMatchStatus=pUserItem->GetUserMatchStatus();
	m_pMatchSink->SendData(pUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

	//报名人满
	if (m_OnMatchUserMap.GetCount()>=m_pImmediateMatch->wStartUserCount)
	{
		//事件通知
		ASSERT(m_pMatchSink!=NULL);
		if (m_pMatchSink!=NULL) m_pMatchSink->SendMatchInfo(NULL);
		EfficacyStartMatch();
	}

	return true;
}
//比赛开始
VOID CImmediateGroup::EfficacyStartMatch()
{
	//插入桌子
	m_pMatchSink->InsertNullTable();

	//没有桌子的情况
	if( m_MatchTableArray.GetCount()<m_pImmediateMatch->wStartUserCount/m_wChairCount )
	{
		if(m_OnMatchUserMap.GetCount()<m_wChairCount)
		{
			//构造数据包
			CMD_CM_SystemMessage Message;
			Message.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME;
			lstrcpyn(Message.szString,TEXT("抱歉，由于某用户强行退出比赛，该场比赛提前结束。你可以继续等待比赛！"),CountArray(Message.szString));

			Message.wLength=CountStringBuffer(Message.szString);
			//发送数据
			WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
			m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
			m_cbMatchStatus=MatchStatus_Signup;
			return ;
		}

		m_cbMatchStatus=MatchStatus_Wait;
		//3秒后继续调用该函数直到有桌子
		SetMatchGameTimer(IDI_WAIT_TABLE,3000,1,NULL);

		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=SMT_CHAT;
		_sntprintf(Message.szString,CountArray(Message.szString),
			TEXT("抱歉，由于过多用户桌子数量不够！还差 %d 张桌子开赛,请等待。。。"),m_pImmediateMatch->wStartUserCount/m_wChairCount-m_MatchTableArray.GetCount());
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
		return ;
	}

	//预赛安排座位
	DWORD dwUserID=0L;
	IServerUserItem *pIServerUserItem=NULL;
	POSITION pos=m_OnMatchUserMap.GetStartPosition();

	WORD wChairID=0;
	while (pos!=NULL)
	{
		//获取用户
		m_OnMatchUserMap.GetNextAssoc(pos,dwUserID,pIServerUserItem);
		ASSERT(pIServerUserItem!=NULL);

		InsertFreeGroup(pIServerUserItem);

		//设置用户比赛中
		pIServerUserItem->SetUserMatchStatus(MUS_PLAYING);
		pIServerUserItem->SetMatchData(this);

		//发送状态
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
		m_pMatchSink->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));
	}

	//桌子自动拉人
	PerformDistributeTable();

	//设置状态	
	m_cbMatchStatus=MatchStatus_Playing;

	//记录时间
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);
	
	//开始通知
	if (m_pMatchSink!=NULL) m_pMatchSink->OnEventMatchStart(this);
		
	//设置定时器
	SetMatchGameTimer(IDI_CHECK_MATCH,1000,TIMES_INFINITY,0);
}

//用户退赛
bool CImmediateGroup::OnUserQuitMatch(IServerUserItem *pUserItem)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//删除用户
	bool bSuccess=(m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID()) == TRUE);
	if(bSuccess==false)return false;

	if (pUserItem->IsAndroidUser())
	{
		ASSERT(m_wAndroidUserCount>0);
		if (m_wAndroidUserCount>0) m_wAndroidUserCount--;
	}

	//设置状态
	pUserItem->SetUserMatchStatus(MUS_NULL);
	BYTE cbUserMatchStatus=pUserItem->GetUserMatchStatus();
	m_pMatchSink->SendData(pUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

	return bSuccess;
}

//定时器
bool CImmediateGroup::OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//状态校验
	if(m_cbMatchStatus==MatchStatus_Null) return true;

	switch(dwTimerID)
	{
	case IDI_CHECK_MATCH:
		{
			CaptureMatchTimer();

			return true;
		}
	case IDI_WAIT_TABLE:
		{
			if(m_cbMatchStatus==MatchStatus_Wait)
			{
				EfficacyStartMatch();
			}

			return true;
		}
	case IDI_SWITCH_TABLE:
		{	
			//关闭定时器
			KillMatchGameTimer(IDI_SWITCH_TABLE);

			//全部起立
			PerformAllUserStandUp();

			//分配桌子
			PerformDistributeTable();

			return true;
		}
	case IDI_MATCH_FINISH:
		{
			//最后结束
			MatchOver();

			return true;
		}
	}

	return true;
}

//用户排序
WORD CImmediateGroup::SortMapUser(tagMatchScore score[])
{
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	POSITION pos=m_OnMatchUserMap.GetStartPosition();
	nCount=0;
	while(pos!=NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos,score[nCount].dwUserID, score[nCount].pUserItem);
		score[nCount].dwUserID=score[nCount].pUserItem->GetUserID();
		score[nCount].lScore=score[nCount].pUserItem->GetUserScore();
		nCount++;
	}
	for(INT_PTR i = 1;i < nCount;i++)
	{
		INT_PTR left = 0,right = i - 1;
		tagMatchScore  Temp = score[i];
		while(left <= right)
		{
			INT_PTR mid = (left + right)/2;
			if(score[i].lScore > score[mid].lScore) right = mid - 1;
			else left = mid + 1;
		}
		for(INT_PTR j = i - 1;j >= left;j--)
		{
			score[j+1] = score[j];
		}
		score[left] = Temp;
	}

	return (WORD)nCount;
}

//游戏开始
bool CImmediateGroup::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//设置底分
	pITableFrame->SetCellScore((SCORE)m_lCurBase);
	KillMatchTimer(IDI_CHECK_TABLE_START,(WPARAM)pITableFrame);

	//获取接口
	tagTableFrameInfo *pTableFrameInfo=GetTableInterface(pITableFrame);

	//设置状态
	pTableFrameInfo->bMatchTaskFinish=false;
	pTableFrameInfo->bSwtichTaskFinish=false;
	pTableFrameInfo->bWaitForNextInnings=false;

	//已完成局数加1
	pTableFrameInfo->wPlayCount++;

	//发送比赛信息
	SendTableMatchInfo(pITableFrame, INVALID_CHAIR);

	//构造数据包
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	TCHAR szMsg[256];
	_sntprintf(szMsg, CountArray(szMsg),TEXT("定局排名赛第%d局开始（共%d局）。"),pTableFrameInfo->wPlayCount,m_pImmediateMatch->wPlayCount);
	lstrcpyn(SystemMessage.szString,szMsg,CountArray(SystemMessage.szString));
	SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
	SystemMessage.wType=SMT_CHAT;

	//发送数据
	WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
	pITableFrame->SendTableData(INVALID_CHAIR,SUB_GF_SYSTEM_MESSAGE, (VOID*)&SystemMessage,wSendSize,MDM_GF_FRAME);

	return true;
}

//游戏结束
bool CImmediateGroup::OnEventGameEnd(ITableFrame * pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//获取桌子信息
	tagTableFrameInfo * pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return false;

	//设置标识
	pTableFrameInfo->bWaitForNextInnings=true;

	//变量定义
	TCHAR szMessage[128]=TEXT("");

	//完成局数
	if (pTableFrameInfo->wPlayCount>=m_pImmediateMatch->wPlayCount)
	{
		//等待提醒
		SendWaitMessage(pITableFrame);	

		//设置变量
		pTableFrameInfo->wPlayCount=0;
		pTableFrameInfo->bMatchTaskFinish=true;	

		//比赛完成
		if (CheckMatchTaskFinish()==true)
		{
			SetMatchGameTimer(IDI_MATCH_FINISH,5000,1,0);		
		}

		//构造消息		
		_sntprintf(szMessage,CountArray(szMessage),TEXT("恭喜您,您已玩满%d局游戏,等其他桌完成后系统将进行排名,请您耐心等待！"),m_pImmediateMatch->wPlayCount);
	}
	else
	{
		//换桌判断
		if (m_pImmediateMatch->cbSwitchTableCount>0&&(pTableFrameInfo->wPlayCount%m_pImmediateMatch->cbSwitchTableCount)==0)
		{
			//设置状态
			pTableFrameInfo->bSwtichTaskFinish=true;

			//插入空闲列表
			for (WORD i=0;i<m_wChairCount;i++)
			{
				IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem!=NULL) InsertFreeGroup(pIServerUserItem);
			}

			//检查换桌
			if (CheckSwitchTaskFinish()==true)
			{
				SetMatchGameTimer(IDI_SWITCH_TABLE,5000,1,0);
			}

			//等待提醒
			SendWaitMessage(pITableFrame);

			//构造消息
			lstrcpyn(szMessage,TEXT("本局已结束，请耐心等待其他桌完成，稍后为您配桌..."),CountArray(szMessage));
		}
		else
		{
			//设置定时器
			PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);

			//构造消息
			lstrcpyn(szMessage,TEXT("本局已结束，请稍候，系统配桌中..."),CountArray(szMessage));
		}			
	}

	//发送消息
	pITableFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_TABLE_ROLL);

	//分数通知
	if (m_OnMatchUserMap.GetCount()>0)
	{
		//分数通知
		tagMatchScore *pScore = new tagMatchScore[m_OnMatchUserMap.GetCount()];
		ZeroMemory(pScore,sizeof(tagMatchScore)*m_OnMatchUserMap.GetCount());
		WORD wCount=SortMapUser(pScore);
		for (WORD i=0;i<wCount;i++)
		{
			m_pMatchSink->SendGroupUserScore(pScore[i].pUserItem,this);
		}

		SafeDeleteArray(pScore);
	}

	return true;
}

//玩家返赛
bool CImmediateGroup::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//发送比赛信息
	SendTableMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//用户坐下
bool CImmediateGroup::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool CImmediateGroup::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

 //用户同意
bool CImmediateGroup::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{ 
	return false; 
}

//获取名次
WORD CImmediateGroup::GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame)
{
	//参数效验
	ASSERT(pUserServerItem!=NULL);
	if(pUserServerItem==NULL) return INVALID_WORD;

	//定义变量
	LONGLONG lUserScore=pUserServerItem->GetUserScore();
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	if(pITableFrame==NULL)
	{
		//遍历容器
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pLoopUserItem);
			if(pLoopUserItem!=pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}
	else
	{
		ASSERT(pUserServerItem->GetTableID()==pITableFrame->GetTableID());
		if(pUserServerItem->GetTableID()!=pITableFrame->GetTableID()) return INVALID_WORD;

		for(WORD i=0; i<m_wChairCount; i++)
		{
			pLoopUserItem=pITableFrame->GetTableUserItem(i);
			if(pLoopUserItem==NULL) continue;

			if(pLoopUserItem!=pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}

	return wRank;
}

//获取接口
tagTableFrameInfo* CImmediateGroup::GetTableInterface(ITableFrame* pITableFrame)
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame==pITableFrame) return m_MatchTableArray[i];
	}

	return NULL;
}

//比赛结束
VOID CImmediateGroup::MatchOver()
{
	//删除定时器
	AllKillMatchTimer();
	KillMatchGameTimer(IDI_CHECK_MATCH);
	KillMatchGameTimer(IDI_WAIT_TABLE);
	KillMatchGameTimer(IDI_SWITCH_TABLE);
	KillMatchGameTimer(IDI_MATCH_FINISH);

	//设置比赛状态
	if (m_cbMatchStatus==MatchStatus_Playing)
	{
		m_cbMatchStatus=MatchStatus_Null;
		m_pMatchSink->OnEventMatchOver(this);
	}
}

//发送提示
VOID CImmediateGroup::SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID/*=INVALID_CHAIR*/)
{
	//状态校验
	if (m_cbMatchStatus==MatchStatus_Null || m_cbMatchStatus==MatchStatus_Signup)
	{
		return;
	}

	//获取信息
	tagTableFrameInfo * pITableFameInfo=GetTableInterface(pTableFrame);
	if (pITableFameInfo==NULL) return;

	//变量定义
	WORD wPlayingTable=0;
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY)
		{
			wPlayingTable++;
		}
	}

	//变量定义
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip,sizeof(WaitTip));
	WaitTip.wPlayingTable=wPlayingTable;
	WaitTip.wUserCount=(WORD)m_OnMatchUserMap.GetCount();
	WaitTip.wCurGameCount=pITableFameInfo->wPlayCount;
	WaitTip.wGameCount=m_pImmediateMatch->wPlayCount;

	lstrcpyn(WaitTip.szMatchName, m_pMatchOption->szMatchName,LEN_SERVER);

	//发送比赛信息给本桌的玩家
	for(WORD i=0;i<m_wChairCount;i++)
	{
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pTableUserItem=pTableFrame->GetTableUserItem(i);
		if(pTableUserItem==NULL) continue;
		WaitTip.wCurTableRank=GetUserRank(pTableUserItem,pTableFrame);
		if(WaitTip.wCurTableRank==INVALID_WORD) continue;


		WaitTip.wRank=GetUserRank(pTableUserItem);
		WaitTip.lScore=pTableUserItem->GetUserScore();

		m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
	}

	//发送比赛信息给其他桌的玩家
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(pTableFrame->GetTableID()==m_MatchTableArray[i]->pTableFrame->GetTableID())continue;
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pTableUserItem=m_MatchTableArray[i]->pTableFrame->GetTableUserItem(j);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRank(pTableUserItem,m_MatchTableArray[i]->pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			WaitTip.wRank=GetUserRank(pTableUserItem);
			WaitTip.lScore=pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}
}

//分配桌子
VOID CImmediateGroup::PerformDistributeTable()
{
	//检查人数
	if (CheckMatchUser()) return;

	//变量定义
	CTableFrameMananerArray	FreeTableArray;	

	//获取空桌
	for (INT_PTR i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame->IsGameStarted()==false&&m_MatchTableArray[i]->bMatchTaskFinish==false)
		{
			//获取数量
			INT_PTR cbTimer=m_MatchTimerArray.GetCount();
			bool cbIsInsert=true;
			for (int j=0;j<cbTimer;j++)
			{
				//判断一下有没有卡赛的桌子在等待
				if ((m_MatchTimerArray[j]->dwTimerID==IDI_CHECK_TABLE_START||m_MatchTimerArray[j]->dwTimerID==IDI_CONTINUE_GAME)&&(ITableFrame*)(m_MatchTimerArray[j]->wParam)==m_MatchTableArray[i]->pTableFrame)
				{
					cbIsInsert=false;
					break;
				}
			}

			//插入判断
			if (cbIsInsert==true)
			{
				FreeTableArray.Add(m_MatchTableArray[i]);
			}
		}
	}

	for (INT_PTR i=0;i<FreeTableArray.GetCount();i++)
	{
		DWORD nCurUserCount=(DWORD)m_FreeUserArray.GetCount();		
		if(nCurUserCount/m_wChairCount>0)
		{
			//全部起立
			PerformAllUserStandUp(FreeTableArray[i]->pTableFrame);

			//处理坐下
			for (WORD wChairID=0; wChairID<m_wChairCount; wChairID++)
			{
				//用户插入该桌
				WORD wRandID=rand()%(WORD)m_FreeUserArray.GetCount();

				//获取用户
				IServerUserItem *pIServerUserItem=m_FreeUserArray[wRandID];				
				if (pIServerUserItem==NULL) continue;
				
				//先起立在坐下 让该桌的人全部起立
				if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					for(int m=0;m<m_MatchTableArray.GetCount();m++)
					{
						if(pIServerUserItem->GetTableID()==m_MatchTableArray[m]->wTableID)
						{
							m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pIServerUserItem);
							break;
						}
					}
				}

				if (FreeTableArray[i]->pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem))
				{
					m_FreeUserArray.RemoveAt(wRandID);
				}
				else
				{
					CTraceService::TraceString(TEXT("坐下失败？"),TraceLevel_Exception);
				}
			}

			//10秒后做一个检查 发现该桌还没有开始游戏 说明已经卡掉了
			PostMatchTimer(IDI_CHECK_TABLE_START,10,(WPARAM)FreeTableArray[i]->pTableFrame);
		}
	}

	return;
}

//发送一组信息
VOID CImmediateGroup::SendGroupMessage(LPCTSTR pStrMessage)
{
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType=SMT_EJECT|SMT_CHAT;
	lstrcpyn(Message.szString,pStrMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);

    //发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
}

//检查结束
bool CImmediateGroup::CheckMatchTaskFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->bMatchTaskFinish==false) return false;
	}

	return true;
}

//检查换桌
bool CImmediateGroup::CheckSwitchTaskFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->bSwtichTaskFinish==false) return false;
	}

	return true;
}


//執行起立
VOID CImmediateGroup::PerformAllUserStandUp()
{	
	//遍历桌子
	for (WORD i=0; i<m_MatchTableArray.GetCount();i++)
	{
		//桌子信息
		tagTableFrameInfo * pTableFrameInfo=m_MatchTableArray[i];
		if (pTableFrameInfo==NULL) continue;

		//桌子指针
		ITableFrame* pTableFrame=pTableFrameInfo->pTableFrame;		
		if (pTableFrame==NULL) continue;

		//强制解散游戏
		if (pTableFrame->IsGameStarted()==true)
		{
			pTableFrame->DismissGame();			
		}

		//執行起立
		PerformAllUserStandUp(pTableFrame);
	}
	return;
}

//執行起立
VOID CImmediateGroup::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//参数校验
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//处理起立
	for (int i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
		if(pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
		{
			pITableFrame->PerformStandUpAction(pUserServerItem);
		}
	}	
}
//本桌继续游戏
VOID CImmediateGroup::ContinueGame(ITableFrame * pITableFrame)
{
	//参数校验
	ASSERT (pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//获取接口
	tagTableFrameInfo * pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return;

	//查找用户
	for (int i=0;i<m_wChairCount;i++)
	{
		//查找用户
		IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//处理起立
		if (pITableFrame->PerformStandUpAction(pIServerUserItem)==false)
		{
			ASSERT (FALSE);
			CTraceService::TraceString(TEXT("继续游戏_起立失败"),TraceLevel_Exception);

			continue;
		}

		//处理坐下
		if (pITableFrame->PerformSitDownAction(i,pIServerUserItem)==false)
		{			
			ASSERT (FALSE);
			CTraceService::TraceString(TEXT("继续游戏_坐下失败"),TraceLevel_Exception);

			continue;
		}		
	}

	//设置定时器
	PostMatchTimer(IDI_CHECK_TABLE_START,30,(WPARAM)pITableFrame);
}

VOID CImmediateGroup::SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	if (m_pMatchSink!=NULL)
	{
		m_pMatchSink->SetGameTimer(dwTimerID+10*m_LoopTimer,dwElapse,dwRepeat,dwBindParameter,this);
	}
}

//杀死比赛专用定时器
VOID CImmediateGroup::KillMatchGameTimer(DWORD dwTimerID)
{
	if (m_pMatchSink!=NULL)
	{
		m_pMatchSink->KillGameTimer(dwTimerID+10*m_LoopTimer,this);
	}
}

//检测参赛人数
bool CImmediateGroup::CheckMatchUser()
{
	//参赛用户不够 椅子数量就要终止比赛
	DWORD nCurUserCount=(DWORD)m_OnMatchUserMap.GetCount();
	if (nCurUserCount<m_wChairCount)
	{
		ASSERT(FALSE);
		CTraceService::TraceString(TEXT("检查人数..."),TraceLevel_Exception);
		return false;

		//构造数据包
		CString szString;
		szString.Format(TEXT("抱歉，由于比赛用户强行退出比赛，人数不足 %d 人，提前结算。欢迎您继续报名参加比赛！"),m_wChairCount);
		SendGroupMessage(szString);

		MatchOver();

		return true;
	}
	return false;
}

//检查桌子
VOID CImmediateGroup::CheckTableStart(ITableFrame *pITableFrame)
{
	//校验参数
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	if(pITableFrame&&pITableFrame->IsGameStarted()==false)
	{
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem && pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				if (pIServerUserItem->IsClientReady()==false || pIServerUserItem->IsTrusteeUser()==true)
				{
					pITableFrame->ImitateGameOption(pIServerUserItem);
				}
			}
		}
	}
}

//插入分组
VOID CImmediateGroup::InsertFreeGroup(IServerUserItem * pUserServerItem)
{
	//参数校验
	ASSERT (pUserServerItem!=NULL);
	if (pUserServerItem==NULL) return;
	
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		if( m_FreeUserArray[i] != NULL && m_FreeUserArray[i]->GetUserID()==pUserServerItem->GetUserID()) return;
	}

	m_FreeUserArray.Add(pUserServerItem);
}

//发送比赛信息
VOID CImmediateGroup::SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//验证参数
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	tagTableFrameInfo *pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return;

	//变量定义
	WORD wChairCount=pITableFrame->GetChairCount();
	TCHAR szMsg[256]=TEXT("");

	if(m_cbMatchStatus==MatchStatus_Playing)
	{
		//构造数据
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo,sizeof(MatchInfo));
		_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：累计积分"));
		_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_lCurBase);
		_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：定局排名"));
		_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("需要完成：%d局比赛"), m_pImmediateMatch->wPlayCount);
		MatchInfo.wGameCount=pTableFrameInfo->wPlayCount;
		
		//wChairID为INVALID_CHAIR发给本桌所有玩家，否则只发给指定玩家
		for(WORD i=0; i<wChairCount; i++)
		{
			//获取用户
			if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if(pIServerUserItem==NULL) continue;

			//发送数据
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);
		}

		//发送提示
		if ((wChairID!=INVALID_CHAIR) && (pTableFrameInfo->bWaitForNextInnings==true))
		{
			SendWaitMessage(pITableFrame,wChairID);
		}
	}

	return;
}

//捕获比赛专用定时器
VOID CImmediateGroup::CaptureMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(--m_MatchTimerArray[i]->iElapse<=0)
		{
			if (m_MatchTimerArray[i]->dwTimerID==IDI_CONTINUE_GAME)
			{
				//继续游戏
				ContinueGame((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			else if(m_MatchTimerArray[i]->dwTimerID==IDI_LEAVE_TABLE)
			{
				//离开本桌
				PerformAllUserStandUp((ITableFrame*)m_MatchTimerArray[i]->wParam);

			}else if(m_MatchTimerArray[i]->dwTimerID==IDI_CHECK_TABLE_START)
			{
				//检查桌子
				CheckTableStart((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			//删除定时器
			if (KillMatchTimer(i)) i--;
			else ASSERT(false);
		}
	}
}
//投递比赛专用定时器
VOID CImmediateGroup::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagMatchTimer* pMatchTimer=new tagMatchTimer;
	pMatchTimer->dwTimerID=dwTimerID;
	pMatchTimer->iElapse=iElapse;
	//pMatchTimer->dwRepeat=dwRepeat;
	pMatchTimer->wParam=wParam;
	pMatchTimer->lParam=lParam;
	m_MatchTimerArray.Add(pMatchTimer);
}

//杀死比赛专用定时器
bool CImmediateGroup::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(m_MatchTimerArray[i]->dwTimerID==dwTimerID&&m_MatchTimerArray[i]->wParam==wParam)
		{
			tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
			m_MatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}
	return false;
}
//杀死比赛专用定时器
bool CImmediateGroup::KillMatchTimer(INT_PTR dwIndexID)
{
	ASSERT(dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount());
	if( dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount() )
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[dwIndexID];
		m_MatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}
	return false;
}
//全部杀死比赛专用定时器
VOID CImmediateGroup::AllKillMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();/*i++*/)
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
		m_MatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}