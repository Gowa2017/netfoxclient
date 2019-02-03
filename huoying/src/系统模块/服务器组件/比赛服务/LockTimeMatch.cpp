#include "StdAfx.h"
#include "LockTimeMatch.h"
#include "..\游戏服务器\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//比赛状态
enum emMatchStatus
{
	MatchStatus_Free=0,
	MatchStatus_WaitPlay,
	MatchStatus_Playing,
	MatchStatus_WaitEnd,
	MatchStatus_End
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_VALUE				0xFFFF								//无效值

//时钟定义
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//切换状态
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//分配用户
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//开始报名
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//开始截止
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//结束时钟

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CLockTimeMatch::CLockTimeMatch()
{
	//状态变量
	m_MatchStatus=MatchStatus_Free;

	//比赛配置
	m_pMatchOption=NULL;
	m_pLockTimeMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//内核接口
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

CLockTimeMatch::~CLockTimeMatch(void)
{
	//释放资源
	SafeDeleteArray(m_ppITableFrame);

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);			

	//移除元素
	m_DistributeManage.RemoveAll();
}

//接口查询
VOID* CLockTimeMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CLockTimeMatch::OnStartService()
{
	//计算场次
	m_pMatchOption->lMatchNo=CalcMatchNo();

	//设置状态
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		if (m_pMatchOption->cbMatchStatus==MS_FREE) m_MatchStatus=MatchStatus_Free;
		if (m_pMatchOption->cbMatchStatus==MS_MATCHEND) m_MatchStatus=MatchStatus_End;
		if (m_pMatchOption->cbMatchStatus==MS_MATCHING) m_MatchStatus=MatchStatus_Playing;	
	}

	//设置状态
	m_TimeLastMatch=CTime::GetCurrentTime();	

	//更新状态
	UpdateMatchStatus();

	//切换时钟
	SwitchMatchTimer();		 

	//报名开始
	if (IsMatchSignupStart()==false)
	{
		//开始检测	
		m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);
	}	

	//报名截止
	if (IsMatchSignupEnd()==false && m_MatchStatus<MatchStatus_Playing)
	{
		m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	
	}
}

//绑定桌子
bool CLockTimeMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//创建钩子
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//初始化接口
bool CLockTimeMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//服务配置
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//内核组件
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//服务组件		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//分组设置
	m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//比赛规则
	m_pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//调整间隔
	if (m_pMatchOption->wDistributeTimeSpace==0)
	{
		m_pMatchOption->wDistributeTimeSpace=5;
	}

	//调整人数
	if (m_pMatchOption->wMaxPartakeGameUser<2) 
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMaxPartakeGameUser>m_pGameServiceAttrib->wChairCount)
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMinPartakeGameUser<2) 
	{
		m_pMatchOption->wMinPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//调整人数
	if (m_pMatchOption->wMinPartakeGameUser>m_pMatchOption->wMaxPartakeGameUser)
	{
		m_pMatchOption->wMinPartakeGameUser=m_pMatchOption->wMaxPartakeGameUser;
	}

	//分组人数
	if (m_pMatchOption->wMinDistributeUser<m_pMatchOption->wMinPartakeGameUser)
	{
		m_pMatchOption->wMinDistributeUser=m_pMatchOption->wMinPartakeGameUser;
	}

	//创建桌子
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}	

	return true;
}

//时间事件
bool CLockTimeMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	switch(dwTimerID)
	{
	case IDI_DISTRIBUTE_USER:				//分配用户
		{
			//执行分组
			PerformDistribute();

			return true;
		}
	case IDI_SWITCH_STATUS:					//切换状态
		{
			//判断日期
			if (m_MatchStatus==MatchStatus_End)
			{
				if (m_TimeLastMatch.GetDay()!=CTime::GetCurrentTime().GetDay())
				{
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{	
						//计算场次
						m_pMatchOption->lMatchNo=CalcMatchNo();

						//设置变量						
						m_MatchStatus=MatchStatus_Free;
						m_TimeLastMatch=CTime::GetCurrentTime();						

						//开始检测	
						m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);						

						//报名截止
						m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	

						//切换时钟
						SwitchMatchTimer();		

						//关闭定时器
						m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);

						//发送状态
						BYTE cbMatchStatus=MS_FREE;
						m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
					}					
					else
					{
						//变量定义
						CTime CurTime=CTime::GetCurrentTime();
						CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);

						//计算时间
						int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

						//比赛开始
						if (nCurrSeconds>=(MatchStartTime.GetHour()*3600+MatchStartTime.GetMinute()*60+MatchStartTime.GetSecond()))
						{
							//设置状态
							m_MatchStatus=MatchStatus_Playing;

							//切换时钟
							SwitchMatchTimer();

							//关闭定时器
							m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);							

							//发送状态
							BYTE cbMatchStatus=MS_MATCHING;
							m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
						}
					}
				}
			}
			
			return true;
		}
	case IDI_CHECK_START_SIGNUP:			//开始报名
		{
			//报名开始
			if (IsMatchSignupStart()==true)
			{			
				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);

				//变量定义
				DBR_GR_SignupStart SignupStart;

				//构造结构
				SignupStart.dwMatchID=m_pMatchOption->dwMatchID;
				SignupStart.lMatchNo=m_pMatchOption->lMatchNo;
				SignupStart.cbMatchType=m_pMatchOption->cbMatchType;
				SignupStart.wServerID=m_pGameServiceOption->wServerID;
				SignupStart.cbSignupMode=m_pMatchOption->cbSignupMode;

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(SignupStart.dwMatchID,DBR_GR_MATCH_SIGNUP_START,0,&SignupStart,sizeof(SignupStart));
			}

			return true;
		}
	case IDI_CHECK_END_SIGNUP:				//报名截止
		{
			if (IsMatchSignupEnd()==true)
			{			
				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_END_SIGNUP);	

				//取消比赛
				if (m_MatchStatus==MatchStatus_WaitPlay)
				{
					//比赛结束
					DBR_GR_MatchCancel MatchCancel;
					ZeroMemory(&MatchCancel,sizeof(MatchCancel));

					//构造数据
					MatchCancel.bMatchFinish=FALSE;
					MatchCancel.dwMatchID=m_pMatchOption->dwMatchID;
					MatchCancel.lMatchNo=m_pMatchOption->lMatchNo;
					MatchCancel.wServerID=m_pGameServiceOption->wServerID;
					MatchCancel.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

					//常规开赛
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchCancel.bMatchFinish=TRUE;
					}

					//循环开赛
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{
						//获取时间
						CTime CurTime=CTime::GetCurrentTime();
						CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
						CTimeSpan TimeSpan=MatchEndTime-CurTime;

						//判断时间
						if (TimeSpan.GetDays()==0) MatchCancel.bMatchFinish=TRUE;
					}					

					//投递请求
					m_pIDataBaseEngine->PostDataBaseRequest(MatchCancel.dwMatchID,DBR_GR_MATCH_CANCEL,0,&MatchCancel,sizeof(MatchCancel));
					
					return true;
				}
			}

			return true;
		}
	case IDI_CHECK_START_MATCH:				//开始比赛
		{
			//变量定义
			BYTE cbMatchStatus=m_MatchStatus;

			//更新状态
			UpdateMatchStatus();

			//等待比赛
			if (m_MatchStatus==MatchStatus_WaitPlay)
			{
				//切换时钟
				SwitchMatchTimer();	

				//判断状态
				if (cbMatchStatus!=MatchStatus_WaitPlay)
				{
					//发送状态
					BYTE cbMatchStatus=MS_WAITPLAY;
					m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

					//构造人数
					CMD_GR_Match_Num MatchNum;
					MatchNum.dwWaitting=m_DistributeManage.GetCount();
					MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
					
					//发送人数
					m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
				}
			}

			//比赛开始
			if (m_MatchStatus==MatchStatus_Playing)
			{
				//切换时钟
				SwitchMatchTimer();	

				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_START_MATCH);

				//变量定义
				DBR_GR_MatchStart MatchStart;

				//构造结构
				MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
				MatchStart.lMatchNo=m_pMatchOption->lMatchNo;
				MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
				MatchStart.wServerID=m_pGameServiceOption->wServerID;

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));

				//发送状态
				BYTE cbMatchStatus=MS_MATCHING;
				m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
			}

			return true;
		}
	case IDI_CHECK_END_MATCH:				//结束比赛
		{
			//变量定义
			BYTE cbMatchStatus=m_MatchStatus;

			//更新状态
			UpdateMatchStatus();

			//比赛结束
			if (m_MatchStatus==MatchStatus_WaitEnd)
			{
				if (cbMatchStatus!=MatchStatus_WaitEnd)
				{
					//发送状态
					BYTE cbMatchStatus=MS_WAITEND;
					m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));
					
					//获取时间
					CTime CurTime=CTime::GetCurrentTime();
					CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

					//比赛结束
					bool bMatchOver=CurTime>=MatchEndTime;

					//变量定义					
					WORD wUserIndex=0;
					TCHAR szMessage[128];
					IServerUserItem *pIServerUserItem = NULL;

					do 
					{
						pIServerUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
						if (pIServerUserItem==NULL) break;

						//比赛中用户
						if (pIServerUserItem->GetUserStatus()==US_PLAYING)
						{							
							if (bMatchOver==false)
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("今天的比赛结束时间已到，请尽快在 %d 分钟内结束本局游戏。"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
							else
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("本场比赛结束时间已到，请尽快在 %d 分钟内结束当前比赛。否则系统将会自动解散本回合并不计入成绩。"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
						}
						else
						{
							if (bMatchOver==false)
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("今天的比赛到此结束，请您明天再来参加！"));
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
							else
							{
								//发送结束消息
								_sntprintf(szMessage,CountArray(szMessage),TEXT("本场比赛结束时间已到，感谢您的参与；请在 %d 分钟后前往相关页面查询您的本场比赛成绩。"),m_pLockTimeMatch->wEndDelayTime/60);								
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
						}

					} while (true);
				}
			}

			//比赛结束
			if (m_MatchStatus==MatchStatus_End)
			{
				//关闭定时器
				m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
				m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

				//发送状态
				BYTE cbMatchStatus=MS_MATCHEND;
				m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

				//解散游戏
				for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
				{
					//获取对象
					ITableFrame * pITableFrame=m_ppITableFrame[i];					
					ASSERT(pITableFrame!=NULL);
					if (pITableFrame==NULL) continue;

					//解散游戏
					if (pITableFrame->IsGameStarted()) 
					{
						pITableFrame->DismissGame();
					}

					//执行起立
					PerformAllUserStandUp(pITableFrame);
				}

				//移除所有
				m_DistributeManage.RemoveAll();

				//获取时间
				CTime CurTime=CTime::GetCurrentTime();
				CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

				//比赛结束
				if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND || CurTime>=MatchEndTime)
				{
					//比赛结束
					DBR_GR_MatchOver MatchOver;
					ZeroMemory(&MatchOver,sizeof(MatchOver));

					//构造数据				
					MatchOver.cbMatchType=MATCH_TYPE_LOCKTIME;
					MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
					MatchOver.lMatchNo=m_pMatchOption->lMatchNo;					
					MatchOver.wPlayCount=m_pLockTimeMatch->wMinPlayCount;
					MatchOver.wServerID=m_pGameServiceOption->wServerID;
					MatchOver.bMatchFinish=(CurTime>=MatchEndTime)?TRUE:FALSE;

					//常规开赛
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchOver.MatchEndTime=m_pLockTimeMatch->MatchEndTime;
						MatchOver.MatchStartTime=m_pLockTimeMatch->MatchStartTime;
					}	

					//循环开赛
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{
						//变量定义
						SYSTEMTIME SystemTimeCurr;
						CurTime.GetAsSystemTime(SystemTimeCurr);

						//设置时间
						SystemTimeCurr.wHour=m_pLockTimeMatch->MatchStartTime.wHour;
						SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchStartTime.wMinute;
						SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchStartTime.wSecond;

						//开始时间
						MatchOver.MatchStartTime=SystemTimeCurr;

                        //设置时间
						SystemTimeCurr.wHour=m_pLockTimeMatch->MatchEndTime.wHour;
						SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchEndTime.wMinute;
						SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchEndTime.wSecond;

						//结束时间
						MatchOver.MatchEndTime=SystemTimeCurr;						
					}									

					//投递请求
					m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
					
					//切换时钟
					if (CurTime<MatchEndTime) SwitchMatchTimer();					
						
					return true;
				}

				//发送结束消息
				TCHAR szMessage[128]=TEXT("今天的比赛到此结束，请您明天再来参加！");
				m_pIGameServiceFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);

				//切换时钟
				SwitchMatchTimer();				
			}

			return true;
		}
	}
	return true;
}

//数据库事件
bool CLockTimeMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch(wRequestID)
	{
	case DBO_GR_MATCH_QUERY_RESULT:			//查询结果
		{
			//参数校验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize!=sizeof(DBO_GR_MatchQueryResult)) return false;

			//提取数据
			DBO_GR_MatchQueryResult * pMatchQueryResult = (DBO_GR_MatchQueryResult*)pData;	

			//具备资格
			if (pMatchQueryResult->bHoldQualify==true)
			{
				//报名方式
				BYTE cbSignupMode=SIGNUP_MODE_MATCH_USER;

				//报名费用
				return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwContextID);
			}

			//扣除费用
			if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
			{
				//扣费提示
				SendDeductFeeNotify(pIServerUserItem);
			}
			else
			{
				//消息通知
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("抱歉,您不具备本场比赛的报名资格,报名失败！"),SMT_EJECT);
			}

			return true;
		}
	case DBO_GR_MATCH_BUY_RESULT:			//购买结果
		{
			//参数校验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchBuyResult)) return false;

			//提取数据
			DBO_GR_MatchBuyResult * pMatchBuyResult = (DBO_GR_MatchBuyResult*)pData;

			//购买失败
			if (pMatchBuyResult->bSuccessed==false)
			{
				//发送消息
				if (pMatchBuyResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchBuyResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//消息通知
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，保险卡购买成功，请等待系统为您分配座位！"),SMT_CHAT);

			//加入分组
			InsertDistribute(pIServerUserItem);	

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchBuyResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			return true;
		}		
	case DBO_GR_MATCH_REVIVE_INFO:			//复活信息
		{
			//参数校验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize!=sizeof(DBO_GR_MatchReviveInfo)) return false;

			//提取数据
			DBO_GR_MatchReviveInfo * pMatchReviveInfo = (DBO_GR_MatchReviveInfo*)pData;

			//变量定义
			bool bEliminate=false;

			//淘汰判断
			if (!pMatchReviveInfo->bSuccessed || (!pMatchReviveInfo->bSafeCardRevive && pMatchReviveInfo->cbReviveTimesed >= m_pLockTimeMatch->cbReviveTimes))
			{
				bEliminate=true;
			}

			//淘汰提醒
			if (bEliminate==true)
			{
				//淘汰提醒
				SendEliminateNotify(pIServerUserItem);

				return true;
			}

			//复活提醒
			SendReviveNotify(pIServerUserItem,pMatchReviveInfo->cbReviveTimesed,pMatchReviveInfo->bSafeCardRevive);

			//保险卡复活
			if (pMatchReviveInfo->bSafeCardRevive==true)
			{
				//玩家复活
				OnEventSocketMatch(SUB_GR_MATCH_USER_REVIVE,NULL,0,pIServerUserItem,dwContextID);
			}

			return true;
		}
	case DBO_GR_MATCH_REVIVE_RESULT:		//复活结果
		{
			//参数校验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchReviveResult)) return false;

			//提取数据
			DBO_GR_MatchReviveResult * pMatchReviveResult = (DBO_GR_MatchReviveResult*)pData;

			//购买失败
			if (pMatchReviveResult->bSuccessed==false)
			{
				//发送消息
				if (pMatchReviveResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchReviveResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//发送分数
			SendMatchUserInitScore(pIServerUserItem);

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//报名状态		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//消息通知
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，复活成功，请等待系统为您分配座位！"),SMT_CHAT|SMT_EJECT);

			//加入分组
			InsertDistribute(pIServerUserItem);	

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchReviveResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			return true;
		}
	case DBO_GR_MATCH_SIGNUP_RESULT:		//报名结果
		{
			//参数效验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//报名失败
			if (pMatchSignupResult->bResultCode==false)
			{
				//提示消息不为空
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//发送分数
			SendMatchUserInitScore(pIServerUserItem);

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//报名状态		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

			//使用保险卡
			if (m_pLockTimeMatch->cbReviveEnabled && m_pLockTimeMatch->cbSafeCardEnabled && pIServerUserItem->IsAndroidUser()==false)
			{
				SendSafeCardNotify(pIServerUserItem);
			}
			else
			{
				SendSignupSuccessNotify(pIServerUserItem);				
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//退赛结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//系统取消
			if (pMatchSignupResult->dwReason==UNSIGNUP_REASON_SYSTEM) return true;

			//退费失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//获取成绩
			tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

			//设置成绩
			pUserScore->dwWinCount=0L;
			pUserScore->dwLostCount=0L;
			pUserScore->dwFleeCount=0L;
			pUserScore->dwDrawCount=0L;
			pUserScore->lIntegralCount=0L;
			pUserScore->lScore-=m_pLockTimeMatch->lMatchInitScore;

			//发送分数
			SendMatchUserScore(pIServerUserItem);

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//报名状态		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_RANK_LIST:			//比赛排行
		{
			//参数校验
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//提取数据
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//变量定义
			bool bRewardUser=true;
			tagMatchRankInfo * pMatchRankInfo=NULL;
			IServerUserItem * pIRewardUserItem=NULL;

			//发放奖励
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//查找玩家
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if (pIRewardUserItem!=NULL && pIRewardUserItem->IsAndroidUser()==true) continue;

				//写入奖励
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);

				//设置标识
				if(pIRewardUserItem) pIRewardUserItem->SetMatchData(&bRewardUser);
			}

			//在线用户
			WORD wUserIndex=0;			
			while (true)
			{
				pIRewardUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
				if (pIRewardUserItem==NULL) break;
				if (pIRewardUserItem->IsAndroidUser()==true) continue;
				if (pIRewardUserItem->GetMatchData()==&bRewardUser)
				{
					//设置参数
					pIRewardUserItem->SetMatchData(NULL);

					//构造消息
					//TCHAR szMessage[256]=TEXT"\0";
					//_sntprintf(szMessage,CountArray(szMessage),TEXT"点击查看比赛排行：http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
					//pIRewardUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

					////发送消息
					//m_pIGameServiceFrame->SendGameMessage(pIRewardUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	

					continue;
				}

				//发送结束消息
				TCHAR szMessage[128]=TEXT("本次比赛到此结束，请您前往网站查看系统排名或移驾其他比赛房间参与比赛！");
				m_pIGameServiceFrame->SendGameMessage(pIRewardUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);	
			}			

			//断开协调
			if (m_pIGameServiceFrame!=NULL && pMatchRankList->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond();
			}

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//奖励结果
		{
			//参数效验
			ASSERT(wDataSize==sizeof(DBO_GR_MatchRewardResult));
			if(wDataSize!=sizeof(DBO_GR_MatchRewardResult)) return false;

			//提取数据
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//查找玩家
			IServerUserItem *  pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRewardResult->dwUserID);
			if (pIRewardUserItem==NULL) return true;

			//奖励成功
			if(pMatchRewardResult->bResultCode==true)
			{
				//更新用户信息
				tagUserInfo * pUserInfo=pIRewardUserItem->GetUserInfo();

				//发送分数
				SendMatchUserScore(pIRewardUserItem);

				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//发送数据
				m_pIGameServiceFrame->SendData(pIRewardUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_CANCEL_RESULT:		//取消结果
		{
			//参数效验
			ASSERT(wDataSize==sizeof(DBO_GR_MatchCancelResult));
			if(wDataSize!=sizeof(DBO_GR_MatchCancelResult)) return false;

			//提取数据
			DBO_GR_MatchCancelResult * pMatchCancelResult = (DBO_GR_MatchCancelResult*)pData;

			//取消成功
			if (pMatchCancelResult->bSuccessed==true)
			{
				//发送结束消息
				TCHAR szMessage[128]=TEXT("因报名人数不足，取消本场比赛。已报名的玩家稍后将会收到系统退还的报名费用，请注意查收。");
				m_pIGameServiceFrame->SendRoomMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME|SMT_CLOSE_ROOM);	
			}

			//获取时间
			CTime CurTime=CTime::GetCurrentTime();
			CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

			//断开协调
			if (m_pIGameServiceFrame!=NULL && pMatchCancelResult->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond(); 
			}

			//移除所有
			m_DistributeManage.RemoveAll();

			break;
		}
	}
	return true;
}

//比赛事件
bool CLockTimeMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//比赛费用
		{
			//参数效验
			ASSERT(wDataSize==sizeof(BYTE));
			if(wDataSize!=sizeof(BYTE)) return false;

			//提取数据
			BYTE cbSignupMode=*(BYTE*)pData;
			if ((cbSignupMode&m_pMatchOption->cbSignupMode)==0)return false;

			//变量定义
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//构造结构
			MatchSignup.cbSignupMode=cbSignupMode;			
			MatchSignup.lMatchNo=m_pMatchOption->lMatchNo;
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();			
			MatchSignup.lInitScore=m_pLockTimeMatch->lMatchInitScore;			
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();			
			MatchSignup.wMaxSignupUser=m_pLockTimeMatch->wMaxSignupUser;
            
			//拷贝机器码
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//退出比赛
		{
			//参数效验
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	case SUB_GR_MATCH_BUY_SAFECARD:	//购买保险
		{
			//参数效验
			ASSERT(wDataSize==sizeof(CMD_GR_Match_BuySafeCard));
			if(wDataSize!=sizeof(CMD_GR_Match_BuySafeCard)) return false;

			//提取数据
			CMD_GR_Match_BuySafeCard * pMatchBuySafeCard =(CMD_GR_Match_BuySafeCard *)pData;

			//购买保险
			if (pMatchBuySafeCard->bBuySafeCard)
			{
				//构造结构
				DBR_GR_MatchBuySafeCard MatchBuySafeCard;

				//设置变量
				MatchBuySafeCard.dwUserID=pIServerUserItem->GetUserID();
				MatchBuySafeCard.dwMatchID=m_pMatchOption->dwMatchID;
				MatchBuySafeCard.lMatchNo=m_pMatchOption->lMatchNo;
				MatchBuySafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(MatchBuySafeCard.dwUserID,DBR_GR_MATCH_BUY_SAFECARD,dwSocketID,&MatchBuySafeCard,sizeof(MatchBuySafeCard));
			}
			else
			{
				//消息通知
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("系统正在努力为您分配座位，请等待！"),SMT_CHAT);

				//加入分组
				InsertDistribute(pIServerUserItem);
			}

			return true;
		}
	case SUB_GR_MATCH_USER_REVIVE:	//玩家复活
		{
			//参数效验
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			//分数判断
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()>m_pLockTimeMatch->lMatchCullScore)
			{
				//消息通知
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("您的分数未达到淘汰分数线，不需要复活！"),SMT_CHAT);

				return true;
			}			

			//构造结构
			DBR_GR_MatchUserRevive MatchUserRevive;

			//比赛信息			
			MatchUserRevive.dwMatchID=m_pMatchOption->dwMatchID;
			MatchUserRevive.lMatchNo=m_pMatchOption->lMatchNo;
			MatchUserRevive.dwUserID=pIServerUserItem->GetUserID();

			//复活配置						
			MatchUserRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
			MatchUserRevive.lInitScore=m_pLockTimeMatch->lMatchInitScore;
			MatchUserRevive.lCullScore=m_pLockTimeMatch->lMatchCullScore;						
			MatchUserRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;			

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(MatchUserRevive.dwUserID,DBR_GR_MATCH_USER_REVIVE,dwSocketID,&MatchUserRevive,sizeof(MatchUserRevive));

			return true;
		}
	}

	return true;
}

//用户积分
bool CLockTimeMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//变量定义
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//构造数据
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//发送数据
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));

	//即时写分
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//比赛信息
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.lMatchNo=m_pMatchOption->lMatchNo;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);
	}

	return true;
}

//用户数据
bool CLockTimeMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//用户状态
bool CLockTimeMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//清除数据
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//用户权限
bool CLockTimeMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//用户登录
bool CLockTimeMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//构造结构
	CMD_GR_Match_Rule MatchRule;

	//基本信息
	MatchRule.dwMatchID=m_pMatchOption->dwMatchID;
	MatchRule.cbMatchType=m_pMatchOption->cbMatchType;
	lstrcpyn(MatchRule.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchRule.szMatchName));	

	//报名信息
	MatchRule.cbFeeType=m_pMatchOption->cbFeeType;
	MatchRule.lSignupFee=m_pMatchOption->lSignupFee;	
	MatchRule.dwExperience=m_pMatchOption->dwExperience;	
	MatchRule.cbDeductArea=m_pMatchOption->cbDeductArea;
	MatchRule.cbSignupMode=m_pMatchOption->cbSignupMode;
	MatchRule.cbMemberOrder=m_pMatchOption->cbMemberOrder;
	MatchRule.cbJoinCondition=m_pMatchOption->cbJoinCondition;	

	//排名方式
	MatchRule.cbRankingMode=m_pMatchOption->cbRankingMode;
	MatchRule.wCountInnings=m_pMatchOption->wCountInnings;
	MatchRule.cbFilterGradesMode=m_pMatchOption->cbFilterGradesMode;

	//比赛规则
	CopyMemory(MatchRule.cbMatchRule,m_pMatchOption->cbMatchRule,sizeof(MatchRule.cbMatchRule));

	//比赛奖励
	MatchRule.wRewardCount=__min(m_pMatchOption->wRewardCount,CountArray(MatchRule.MatchRewardInfo));
	CopyMemory(MatchRule.MatchRewardInfo,m_pMatchOption->MatchRewardInfo,MatchRule.wRewardCount* sizeof(MatchRule.MatchRewardInfo[0]));

	//计算大小
	WORD wDataSize=sizeof(MatchRule)-sizeof(MatchRule.MatchRewardInfo);
	wDataSize+=MatchRule.wRewardCount*sizeof(MatchRule.MatchRewardInfo[0]);

	//发送属性
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_RULE,&MatchRule,wDataSize);

	return true;
}

//用户登出
bool CLockTimeMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	RemoveDistribute(pIServerUserItem);

	return true;
}

//登录完成
bool CLockTimeMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	//发送状态
	if (m_MatchStatus>=MatchStatus_Free && pIServerUserItem->IsAndroidUser()==false)
	{
		//比赛状态		
		BYTE cbMatchStatus=MS_FREE;		
		if (m_MatchStatus==MatchStatus_End) cbMatchStatus=MS_MATCHEND;
		if (m_MatchStatus==MatchStatus_WaitEnd) cbMatchStatus=MS_WAITEND;
		if (m_MatchStatus==MatchStatus_Playing) cbMatchStatus=MS_MATCHING;		
		if (m_MatchStatus==MatchStatus_WaitPlay) cbMatchStatus=MS_WAITPLAY;
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

		//淘汰判断
		if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
		{
			//淘汰判断
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
			{
				pIServerUserItem->SetUserMatchStatus(MUS_OUT);
			}
		}		

		//报名状态		
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

		//开赛倒计时
		if (m_MatchStatus==MatchStatus_WaitPlay)
		{
			//加入分组
			if (cbUserMatchStatus==MUS_SIGNUPED) InsertDistribute(pIServerUserItem);

			//构造人数
			CMD_GR_Match_Num MatchNum;
			MatchNum.dwWaitting=m_DistributeManage.GetCount();
			MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
			
			//发送人数
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		}
	}
 
	return true;
}

//进入事件
bool CLockTimeMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//判断时间
	TCHAR szDescribe[256];
	WORD wMessageType;
	if (VerifyMatchTime(szDescribe,CountArray(szDescribe),wMessageType)==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,wMessageType|SMT_GLOBAL,dwUserIP==0);
		return true;
	}

	//手机用户
	if (bIsMobile == true)
	{
		//处理消息
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//构造数据
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonMobile.lMatchNo=m_pMatchOption->lMatchNo;
		LogonMobile.cbMatchType=m_pMatchOption->cbMatchType;
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));
	}
	else
	{
		//处理消息
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//变量定义
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//构造数据
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		LogonUserID.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonUserID.lMatchNo=m_pMatchOption->lMatchNo;	
		LogonUserID.cbMatchType=m_pMatchOption->cbMatchType;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//用户参赛
bool CLockTimeMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//判断状态
	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		return false;
	}

	//报名未开始
	if (IsMatchSignupStart()==false)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("报名尚未开始，请稍后再来！"),SMT_EJECT);

		return true;
	}

	//报名已截止
	if (IsMatchSignupEnd()==true && pIServerUserItem->GetUserMatchStatus()==MUS_NULL)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("报名已经截止，请您移驾至其他比赛房间参与比赛！"),SMT_EJECT);

		return true;
	}

	//会员等级
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_MEMBER_ORDER)
	{
		if (m_pMatchOption->cbMemberOrder>pIServerUserItem->GetMemberOrder())
		{
			//发送消息
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("抱歉，您的会员等级达不到报名条件，报名失败！"),SMT_EJECT);

			return true;
		}
	}

	//经验等级
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_EXPERIENCE)
	{
		if (m_pMatchOption->dwExperience>pIServerUserItem->GetUserInfo()->dwExperience)
		{
			//发送消息
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("抱歉，您的经验值达不到报名条件，报名失败！"),SMT_EJECT);

			return true;
		}
	}

	//判断状态
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_SIGNUPED)
	{
		//比赛用户
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
		{
			if (pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				DBR_GR_QueryMatchQualify QueryMatchQualify;

				//设置变量
				QueryMatchQualify.lMatchNo=m_pMatchOption->lMatchNo;
				QueryMatchQualify.dwMatchID=m_pMatchOption->dwMatchID;				
				QueryMatchQualify.dwUserID=pIServerUserItem->GetUserID();

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(QueryMatchQualify.dwUserID,DBR_GR_MATCH_QUERY_QUALIFY,dwSocketID,&QueryMatchQualify,sizeof(QueryMatchQualify));

				return true;
			}
		}

		//扣除报名费
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{			
			//真人扣报名费
			if(m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
			{
				SendDeductFeeNotify(pIServerUserItem);

				return true;
			}			
		}		

		//自动报名
		if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
		{
			//报名方式
			BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

			return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwSocketID);
		}
	}

	//加入分组
	if (InsertDistribute(pIServerUserItem)==false) return false;
	
	//发送成绩
	SendMatchUserScore(pIServerUserItem);

	//发送消息
	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("系统正在努力为您分配座位，请稍后。。。"),SMT_CHAT);

	return true;
}

//用户退赛
bool CLockTimeMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//判断比赛状态
	if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	{
		return true;
	}

	//正在游戏状态
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//已进行过比赛
	if (pIServerUserItem->GetUserPlayCount()>0)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("抱歉,您已参加过本场比赛，不能退赛！"),SMT_CHAT);

		return true;
	}
	
	//退还报名费
	if(m_pMatchOption->lSignupFee>0 && !pIServerUserItem->IsAndroidUser())
	{
		//变量定义
		DBR_GR_MatchUnSignup MatchUnSignup;
		MatchUnSignup.dwReason=cbReason;
		MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

		//构造结构
		MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
		MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
		MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
		MatchUnSignup.lMatchNo=m_pMatchOption->lMatchNo;
		lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
	}

	//移除分组
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	//移除同桌信息
	m_DistributeManage.RemoveUserSameTableInfo(pIServerUserItem->GetUserID());

	return true;
}

//游戏开始
bool CLockTimeMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//发送比赛信息
	SendTableUserMatchInfo(pITableFrame, INVALID_CHAIR);

	//构造数据包
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//变量定义
	IServerUserItem *pIServerUserItem=NULL;
	IServerUserItem *pITableUserItem=NULL;	

	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		pIServerUserItem=pITableFrame->GetTableUserItem(i);
		//ASSERT(pIServerUserItem!=NULL);
		if (pIServerUserItem==NULL) continue;

		//构造提示消息
		TCHAR szMessage[64];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("本局比赛是您的第 %d 局。"),pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount+1);
		lstrcpyn(SystemMessage.szString,szMessage,CountArray(SystemMessage.szString));
		SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
		SystemMessage.wType=SMT_CHAT;

		//发送提示数据
		WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
		pITableFrame->SendTableData(pIServerUserItem->GetChairID(),SUB_GF_SYSTEM_MESSAGE, &SystemMessage,wSendSize,MDM_GF_FRAME);

		//清除同桌
		ClearSameTableUser(pIServerUserItem->GetUserID());

		for (INT j=0; j<pITableFrame->GetChairCount(); j++)
		{
			//获取用户
			pITableUserItem=pITableFrame->GetTableUserItem(j);
			if(pITableUserItem==NULL) continue;
			if(pITableUserItem==pIServerUserItem) continue;

			//插入同桌用户
			InsertSameTableUser(pIServerUserItem->GetUserID(),pITableUserItem->GetUserID());
		}
	}

	return true;
}

//游戏结束
bool CLockTimeMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	//结束处理
	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		IServerUserItem *pIUserItem=pITableFrame->GetTableUserItem(i);
		//ASSERT(pIUserItem!=NULL);
		if (pIUserItem==NULL) continue;

		//正常结束
		if (cbReason!=REMOVE_BY_ENDMATCH)
		{
			//达到淘汰线
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
			{
				//设置状态
				pIUserItem->SetUserMatchStatus(MUS_OUT);

				//报名状态		
				BYTE cbUserMatchStatus=pIUserItem->GetUserMatchStatus();
				m_pIGameServiceFrame->SendData(pIUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

				//淘汰提醒
				if (m_pLockTimeMatch->cbReviveEnabled==TRUE)
				{
					//构造结构
					DBR_GR_MatchQueryRevive MatchQueryRevive;

					//设置变量
					MatchQueryRevive.dwUserID=pIUserItem->GetUserID();
					MatchQueryRevive.dwMatchID=m_pMatchOption->dwMatchID;
					MatchQueryRevive.lMatchNo=m_pMatchOption->lMatchNo;


					//投递数据
					m_pIDataBaseEngine->PostDataBaseRequest(MatchQueryRevive.dwUserID,DBR_GR_MATCH_QUERY_REVIVE,0,&MatchQueryRevive,sizeof(MatchQueryRevive));
				}
				else
				{
					//淘汰提醒
					SendEliminateNotify(pIUserItem);
				}

				continue;
			}			

			//完成局数
			if (m_pMatchOption->cbRankingMode&RANKING_MODE_TOTAL_GRADES)
			{
				if (pIUserItem->GetUserPlayCount()-pIUserItem->GetUserInfo()->dwDrawCount==m_pLockTimeMatch->wMinPlayCount)
				{
					TCHAR szMessage[128]=TEXT("");
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您已完成%d局比赛，可以继续当前比赛或选择退出等待本次比赛活动结束后排名！"),m_pLockTimeMatch->wMinPlayCount);
					m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

					continue;
				}				
			}

			//特定局数
			if (m_pMatchOption->cbRankingMode&RANKING_MODE_SPECIAL_GRADES)
			{
				if (pIUserItem->GetUserPlayCount()-pIUserItem->GetUserInfo()->dwDrawCount==m_pMatchOption->wCountInnings)
				{
					TCHAR szMessage[128]=TEXT("");
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您已完成%d局比赛，系统将会记录您当前的成绩并重置比赛分，您可以继续当前比赛或选择退出等待本次比赛结束后查看排名！"),m_pMatchOption->wCountInnings);
					m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

					//发送分数
					SendMatchUserInitScore(pIUserItem);

					//构造结构
					DBR_GR_MatchRecordGrades MatchRecordGrades;

					//设置变量
					MatchRecordGrades.dwUserID=pIUserItem->GetUserID();
					MatchRecordGrades.dwMatchID=m_pMatchOption->dwMatchID;
					MatchRecordGrades.lMatchNo=m_pMatchOption->lMatchNo;
					MatchRecordGrades.wServerID=m_pGameServiceOption->wServerID;
					MatchRecordGrades.lInitScore=m_pLockTimeMatch->lMatchInitScore;

					//投递数据
					m_pIDataBaseEngine->PostDataBaseRequest(MatchRecordGrades.dwUserID,DBR_GR_MATCH_RECORD_GRADES,0,&MatchRecordGrades,sizeof(MatchRecordGrades));

					continue;
				}				
			}

			//插入分组
			if (InsertDistribute(pIUserItem)==true)
			{
				m_pIGameServiceFrame->SendGameMessage(pIUserItem,TEXT("本局比赛结束，系统正在努力为您分配座位，请稍后..."),SMT_CHAT|SMT_TABLE_ROLL);
			}
		}
	}

	return true;
}

//玩家返赛
bool CLockTimeMatch::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//发送比赛信息
	SendTableUserMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//用户坐下
bool CLockTimeMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool CLockTimeMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//旁观判断
	if (bLookonUser==false)
	{
		//移除分组
		RemoveDistribute(pIServerUserItem);

		//判断桌号
		if (wTableID<m_pGameServiceOption->wTableCount)
		{
			//获取桌子
			ITableFrame * pITableFrame=m_ppITableFrame[wTableID];
			if (pITableFrame==NULL ) return false;
			if (pITableFrame->GetGameStatus()>=GAME_STATUS_PLAY) return true;

			//插入分组
			for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
			{
				IServerUserItem * pITableUserItem=pITableFrame->GetTableUserItem(i);
				if (pITableUserItem==NULL) continue;
				if (pITableUserItem->GetUserStatus()!=US_READY) continue;			

				//插入分组
				if (InsertDistribute(pITableUserItem)==true)
				{
					//设置状态
					pITableUserItem->SetUserStatus(US_SIT,wTableID,i);
					m_pIGameServiceFrame->SendGameMessage(pITableUserItem,TEXT("由于玩家离开，系统正在为您重新分配座位，请稍后..."),SMT_CHAT|SMT_TABLE_ROLL);
				}
			}
		}
	}

	return true;
}

 //用户同意
bool CLockTimeMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return true; 
}

//参赛权限
bool CLockTimeMatch::VerifyUserEnterMatch(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//机器不受限制
	if (pIServerUserItem->IsAndroidUser()==true) return true;

	//变量定义
	WORD wMessageType;
	TCHAR szMessage[128]=TEXT("");

	//校验时间
	if (VerifyMatchTime(szMessage,CountArray(szMessage),wMessageType)==false)
	{
		//发送消息
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,wMessageType);

		return false;
	}

	//淘汰判断
	if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
	{
		_sntprintf(szMessage,CountArray(szMessage),TEXT("由于您的比赛币低于 %d，您将不能继续游戏！"),m_pLockTimeMatch->lMatchCullScore);

		//发送消息
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	//最大局数
	if (m_pLockTimeMatch->wMaxPlayCount>0 && (pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)==m_pLockTimeMatch->wMaxPlayCount)
	{
		_sntprintf(szMessage,CountArray(szMessage),TEXT("您已完成 %d 局比赛，您将不能继续游戏，请等待比赛结束来查看排名！"),m_pLockTimeMatch->wMaxPlayCount);

		//发送消息
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	return true;
}

//截止报名
bool CLockTimeMatch::IsMatchSignupEnd()
{
	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);	

	//常规开赛
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		//比较时间
		CTimeSpan TimeSpan=CurTime-SignupEndTime;
		
		return TimeSpan.GetTotalSeconds()>=0;	
	}
	else
	{
		//计算时间
		int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

		//判断时间
		return nCurrSeconds>=(SignupEndTime.GetHour()*3600+SignupEndTime.GetMinute()*60+SignupEndTime.GetSecond());
	}		
}

//开始报名
bool CLockTimeMatch::IsMatchSignupStart()
{
	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	

	//常规开赛
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		//比较时间
		CTimeSpan TimeSpan = CurTime-SignupStartTime;

		return TimeSpan.GetTotalSeconds()>=0;	
	}
	else
	{
		//计算时间
		int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

		//判断时间
		return nCurrSeconds>=(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond());
	}	
}

//开始间隔
DWORD CLockTimeMatch::GetMatchStartInterval()
{
	//获取时间
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

	//变量定义
	DWORD dwTimeInterval=1;
	DWORD dwCurrStamp,dwStartStamp,dwEndStamp;

	//比赛未开始
	if(CurTime<MatchStartTime)
	{
		CTimeSpan TimeSpan = MatchStartTime-CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
	}

	//比赛已结束
	if(CurTime>=MatchEndTime) dwTimeInterval=0;

	//计算时间
	dwCurrStamp = CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();
	dwStartStamp = m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond;
	dwEndStamp = m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond;

	//比赛未开始
	if(dwCurrStamp<dwStartStamp) 
	{
		dwTimeInterval = dwStartStamp-dwCurrStamp;
	}

	//比赛已结束
	if(dwCurrStamp>=dwEndStamp)
	{
		dwTimeInterval = 24*3600-(dwCurrStamp-dwStartStamp);		
	}

	return dwTimeInterval;
}

//執行起立
VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//参数校验
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//处理起立
	for (int i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
		if (pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
		{
			pITableFrame->PerformStandUpAction(pUserServerItem);
		}
	}	
}

//计算场次
LONGLONG CLockTimeMatch::CalcMatchNo()
{
	//循环开赛
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		//变量定义
		SYSTEMTIME SystemStartTime=m_pLockTimeMatch->MatchStartTime;

		//设置变量
		SystemStartTime.wHour=0;
		SystemStartTime.wMinute=0;
		SystemStartTime.wSecond=0;
		SystemStartTime.wMilliseconds=0;

		//获取时间
		CTime CurTime=CTime::GetCurrentTime();
		CTime MatchStartTime(SystemStartTime);

		//变量定义
		CTimeSpan TimeSpan=CurTime-MatchStartTime;

		return TimeSpan.GetDays()+1;
	}

	return 1;
}

//分配用户
bool CLockTimeMatch::PerformDistribute()
{
	//状态判断
	if (m_MatchStatus!=MatchStatus_Playing) return true;

	//人数校验
	if (m_DistributeManage.GetCount()<m_pMatchOption->wMinDistributeUser) return false;

	//分配用户
	while(true)
	{
		//变量定义
		CDistributeInfoArray DistributeInfoArray;

        //获取用户
		WORD wRandCount = __max(m_pMatchOption->wMaxPartakeGameUser-m_pMatchOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pMatchOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if (wDistributeCount < wChairCount) break;

		//寻找位置
		ITableFrame * pICurrTableFrame=NULL;
		for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//获取对象
			ASSERT(m_ppITableFrame[i]!=NULL);
			ITableFrame * pITableFrame=m_ppITableFrame[i];

			//状态判断
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame=pITableFrame;
				break;
			}
		}

		//桌子判断
		if (pICurrTableFrame==NULL) break;

		//玩家坐下
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//变量定义
			WORD wChairID=INVALID_CHAIR;

			//椅子搜索
			for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
			{
				if (pICurrTableFrame->GetTableUserItem(i)==NULL)
				{
					wChairID = i;
					break;
				}
			}

			//分配用户
			if (wChairID!=INVALID_CHAIR)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//用户起立
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=pIServerUserItem->GetTableID();
					ITableFrame * pITableFrame = m_ppITableFrame[wTableID];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//用户坐下
				if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//坐下结果
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			if (bSitSuccess==true || nSitFailedIndex==nIndex)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
			}
			else
			{
				//用户起立
				if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
					m_ppITableFrame[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
				}
			}
		}			
	}

	return true;
}

//更新状态
VOID CLockTimeMatch::UpdateMatchStatus()
{
	//变量定义
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);	

	//比赛未开始
	BYTE MatchStatus=m_MatchStatus;

	//等待开始比赛
	if (MatchStatus==MatchStatus_Free && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitPlay;
	}

	//比赛进行中
	if (MatchStatus==MatchStatus_WaitPlay && m_DistributeManage.GetCount()>=m_pLockTimeMatch->wStartUserCount)
	{
		MatchStatus=MatchStatus_Playing;
	}

	//等待结束
	if (MatchStatus==MatchStatus_Playing && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitEnd;
	}	

	//结束判断
	if (MatchStatus==MatchStatus_WaitEnd)  
	{
		//变量定义
		bool bContinueWait=false;

		//解散游戏
		for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//获取对象
			ITableFrame * pITableFrame=m_ppITableFrame[i];								
			if (pITableFrame==NULL) continue;

			//解散游戏
			if (pITableFrame->IsGameStarted())
			{
				bContinueWait=true;
				break;
			}
		}

		//无需等待
		if (bContinueWait==false)
		{
			MatchStatus=MatchStatus_End;
		}

		//强制结束
		if ((DWORD)(CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond()) >= 
			(DWORD)(m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond+m_pLockTimeMatch->wEndDelayTime))
		{
			MatchStatus=MatchStatus_End;
		}
	}

	//设置状态
	m_MatchStatus=MatchStatus; 	
}

//切换时钟
VOID CLockTimeMatch::SwitchMatchTimer()
{
	//比赛未开始
	if (m_MatchStatus==MatchStatus_Free)
	{
		//开始检测	
		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	}

	//等待开始
	if (m_MatchStatus==MatchStatus_WaitPlay)
	{
		//结束检测
		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	}

	//比赛进行中
	if (m_MatchStatus==MatchStatus_Playing)
	{
		//结束检测
		m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);

		//分组检测
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pMatchOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//等待结束
	if (m_MatchStatus==MatchStatus_WaitEnd)
	{
		//结束检测
		m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);
	}

	//比赛已结束
	if (m_MatchStatus==MatchStatus_End)
	{
		//切换状态
		m_pITimerEngine->SetTimer(IDI_SWITCH_STATUS,2000,TIMES_INFINITY,NULL);
	}
}

//插入用户
bool CLockTimeMatch::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//比赛状态
 	if(m_MatchStatus!=MatchStatus_WaitPlay && m_MatchStatus!=MatchStatus_Playing) return false;

	//状态判断
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//参赛校验
	if(VerifyUserEnterMatch(pIServerUserItem)==false) return false;

	//存在判断
	if (m_DistributeManage.SearchNode(pIServerUserItem)!=NULL) return true;

	//变量定义
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//设置变量
	DistributeInfo.pPertainNode=NULL;	
	DistributeInfo.dwInsertStamp=(DWORD)time(NULL);
	DistributeInfo.pIServerUserItem=pIServerUserItem;   

	//插入节点
	if (m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//等待开始
	if (m_MatchStatus==MatchStatus_WaitPlay)
	{
		//构造人数
		CMD_GR_Match_Num MatchNum;
		MatchNum.dwWaitting=m_DistributeManage.GetCount();
		MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;

		//发送人数
		m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	}

	return true;
}


//清除玩家
void CLockTimeMatch::ClearSameTableUser(DWORD dwUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		pSameTableInfo->wPlayerCount=0;
		ZeroMemory(pSameTableInfo->wPlayerIDSet,sizeof(pSameTableInfo->wPlayerIDSet));
	}
}

//插入用户
void CLockTimeMatch::InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID)
{
	//变量定义
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		//添加用户
		pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++] = dwTableUserID;
	}

	return;
}

//事件校验
bool CLockTimeMatch::VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType)
{
	//变量定义
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	
	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);

	//变量定义
	bool bMatchRound=m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND;
	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//设置类型
	wMessageType=SMT_EJECT|SMT_CHAT;

	//比赛结束
	if (CurTime>=MatchEndTime)
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("比赛已经结束,请您移驾至其他比赛房间参与比赛吧！"));
		return false;
	}

	//比赛已结束
	if (nCurrSeconds >= MatchEndTime.GetHour()*3600+MatchEndTime.GetMinute()*60+MatchEndTime.GetSecond())
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("今天的比赛已经结束,请您于明天%d时%d分%d秒前来参加比赛！"),MatchStartTime.GetHour(),MatchStartTime.GetMinute(),MatchStartTime.GetSecond());
		return false;
	}

	//报名未开始
	if (CurTime<SignupStartTime)
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf(pszMessage,wMaxCount,TEXT("比赛报名尚未开始,本场比赛报名时段为：%d-%d-%d %d:%d:%d至%d-%d-%d %d:%d:%d"),
			SignupStartTime.GetYear(),SignupStartTime.GetMonth(),SignupStartTime.GetDay(),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond(),
			SignupEndTime.GetYear(),SignupEndTime.GetMonth(),SignupEndTime.GetDay(),SignupEndTime.GetHour(),SignupEndTime.GetMinute(),SignupEndTime.GetSecond());
		return false;
	}


	//报名未开始
	if (bMatchRound && nCurrSeconds<(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond()))
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf(pszMessage,wMaxCount,TEXT("比赛报名尚未开始,请您于今天%d时%d分%d秒前来报名参加比赛！"),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond());
		return false;
	}	

	return true;
}

//发送用户分数
bool CLockTimeMatch::SendMatchUserScore(IServerUserItem * pIServerUserItem)
{
	//发送数据
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.dBeans=pUserInfo->dBeans;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.lScore=pUserInfo->lScore;

	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//发送分数
bool CLockTimeMatch::SendMatchUserInitScore(IServerUserItem * pIServerUserItem)
{
	//获取成绩
	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

	//设置成绩
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pLockTimeMatch->lMatchInitScore;

	//发送分数
	return SendMatchUserScore(pIServerUserItem);
}

//复活提醒
bool CLockTimeMatch::SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard)
{
	if (bUseSafeCard==false)
	{
		//构造结构 
		CMD_GR_Match_Revive MatchRevive;

		//比赛信息
		MatchRevive.dwMatchID=m_pMatchOption->dwMatchID;
		MatchRevive.lMatchNO=m_pMatchOption->lMatchNo;

		//复活信息
		MatchRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
		MatchRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;
		MatchRevive.cbReviveTimesed=cbReviveTimesed;

		//构造消息
		_sntprintf(MatchRevive.szNotifyContent,CountArray(MatchRevive.szNotifyContent),TEXT("您的比赛币低于 %d，将不能继续游戏。 您只需支付 %.2f 金币便可复活继续比赛，您确定要复活吗？"),m_pLockTimeMatch->lMatchCullScore,m_pLockTimeMatch->lReviveFee);

        //发送数据
		WORD wDataSize=sizeof(MatchRevive)-sizeof(MatchRevive.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchRevive.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_REVIVE,&MatchRevive,wDataSize);
	}
	else
	{
		//变量定义
		TCHAR szMessage[128];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("您的比赛币低于 %d，将不能继续游戏。由于您购买了保险卡，系统将免费为您复活一次！"),m_pLockTimeMatch->lMatchCullScore);

		//消息通知
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT);
	}

	return true;
}

//保险提醒
bool CLockTimeMatch::SendSafeCardNotify(IServerUserItem * pIServerUserItem)
{
	//构造结构
	CMD_GR_Match_SafeCard MatchSafeCard;

	//设置变量
	MatchSafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	MatchSafeCard.lMatchNO=m_pMatchOption->lMatchNo;
	MatchSafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	//构造消息
	_sntprintf(MatchSafeCard.szNotifyContent,CountArray(MatchSafeCard.szNotifyContent),TEXT("恭喜您报名成功！本场比赛出售保险卡, 被淘汰可免费复活一次，售价 %.2f 金币，您是否要购买？"),MatchSafeCard.lSafeCardFee);

	//发送数据
	WORD wDataSize=sizeof(MatchSafeCard)-sizeof(MatchSafeCard.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchSafeCard.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_SAFECARD,&MatchSafeCard,wDataSize);

	return true;
}

//报名提醒
bool CLockTimeMatch::SendDeductFeeNotify(IServerUserItem * pIServerUserItem)
{
	//构造结构
	CMD_GR_Match_Fee MatchFee;

	//设置变量
	MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
	MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
	MatchFee.lMatchNO=m_pMatchOption->lMatchNo;
	MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
	MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
	MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;	

	ZeroMemory(MatchFee.szNotifyContent,sizeof(MatchFee.szNotifyContent));

	//构造提示			
	if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER && MatchFee.lSignupFee>0)
	{
		LPCTSTR pszFeeType[]={TEXT("游戏币"),TEXT("元宝")};
		_sntprintf(MatchFee.szNotifyContent,CountArray(MatchFee.szNotifyContent),TEXT("参赛将扣除报名费 %.2f %s，确认要参赛吗？"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
	}

	//发送消息
	WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

	return true;
}

//淘汰提醒
bool CLockTimeMatch::SendEliminateNotify(IServerUserItem * pIServerUserItem)
{
	//变量定义
	TCHAR szMessage[128]=TEXT("");
	_sntprintf(szMessage,CountArray(szMessage),TEXT("由于您的比赛币低于 %d，您将不能继续游戏！"),m_pLockTimeMatch->lMatchCullScore);

	//发送消息
	m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);

	//构造结构
	DBR_GR_MatchEliminate MatchEliminate;
	MatchEliminate.cbMatchType=MATCH_TYPE_LOCKTIME;
	MatchEliminate.dwMatchID=m_pMatchOption->dwMatchID;
	MatchEliminate.lMatchNo=m_pMatchOption->lMatchNo;
	MatchEliminate.dwUserID=pIServerUserItem->GetUserID();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(MatchEliminate.dwUserID,DBR_GR_MATCH_ELIMINATE,0,&MatchEliminate,sizeof(MatchEliminate));

	//发送数据
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_ELIMINATE,NULL,0);

	return true;
}

//报名成功
bool CLockTimeMatch::SendSignupSuccessNotify(IServerUserItem * pIServerUserItem)
{
	//报名成功
	if (m_MatchStatus!=MatchStatus_Free)
	{
		//消息通知
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，报名成功，请等待系统为您分配座位！"),SMT_CHAT);

		//加入分组
		InsertDistribute(pIServerUserItem);					
	}
	else
	{
		//消息通知
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("恭喜您，报名成功，请届时前来参加比赛！"),SMT_CHAT);
	}

	return true;
}

//移除玩家
bool CLockTimeMatch::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//验证参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//移除用户
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}


//发送比赛信息
void CLockTimeMatch::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//验证参数
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//构造数据
	CMD_GR_Match_Info MatchInfo;
	ZeroMemory(&MatchInfo,sizeof(MatchInfo));

	//设置标题
	_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("积分方式：累计积分"));
	_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("游戏基数：%d"), m_pLockTimeMatch->lMatchInitScore);
	_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("局制名称：定时开赛"));

	for(WORD i=0; i<pITableFrame->GetChairCount(); i++)
	{
		//获取用户
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//设置局数
		MatchInfo.wGameCount=(WORD)(pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)+1;

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			pITableFrame->SendTableData(i,SUB_GR_MATCH_INFO,&MatchInfo,sizeof(MatchInfo),MDM_GF_FRAME);
		}
	}

	return;
}

//写入奖励
bool CLockTimeMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//参数效验
	ASSERT(pMatchRankInfo!=NULL);
	if(pMatchRankInfo==NULL) return false;

	//写入记录
	if (m_pIDataBaseEngine!=NULL)
	{
		//变量定义
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		
		//构造结构
		MatchReward.dwUserID=pMatchRankInfo->dwUserID;			
		MatchReward.lRewardGold=pMatchRankInfo->lRewardGold;
		MatchReward.lRewardIngot=pMatchRankInfo->lRewardIngot;
		MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
		MatchReward.dwClientAddr=pIServerUserItem?pIServerUserItem->GetClientAddr():0;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	}

	//在线玩家
	if (pIServerUserItem!=NULL)
	{
		//变量定义
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult,sizeof(MatchResult));

		//比赛奖励
		GetSystemTime(&MatchResult.MatchEndTime);
		MatchResult.wMatchRank=pMatchRankInfo->wRankID;
		MatchResult.lGold=pMatchRankInfo->lRewardGold;		
		MatchResult.lIngot=pMatchRankInfo->lRewardIngot;	
		MatchResult.dwExperience=pMatchRankInfo->dwRewardExperience;
		lstrcpyn(MatchResult.szNickName,pIServerUserItem->GetNickName(),CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchResult.szMatchName));

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
		}
	}		

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
