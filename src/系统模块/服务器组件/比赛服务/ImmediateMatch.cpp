#include "StdAfx.h"
#include "ImmediateMatch.h"
#include "..\游戏服务器\DataBasePacket.h"
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//无效值

//////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+1)			//分配用户
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+2)			//开始时钟
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+3)			//结束时钟

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CImmediateMatch::CImmediateMatch()
{
	//变量定义
	m_dwStartTime=0;

	//比赛配置
	m_pMatchOption=NULL;	
	m_pImmediateMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//设置指针
	m_ppITableFrame=NULL;
	m_pCurMatchGroup=NULL;

	//内核接口
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
	m_pIServerUserItemSink=NULL;
}

CImmediateMatch::~CImmediateMatch(void)
{
	SafeDeleteArray(m_ppITableFrame);
	
	m_LoopTimer.RemoveAll();
	SafeDelete(m_pCurMatchGroup);
	m_MatchGroup.Append(m_OverMatchGroup);
	for (int i=0; i<m_MatchGroup.GetCount(); i++)
	{
		SafeDelete(m_MatchGroup[i]);
	}

	m_MatchGroup.RemoveAll();
	m_OverMatchGroup.RemoveAll();
}

//接口查询
VOID* CImmediateMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);	
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CImmediateMatch::OnStartService()
{
	//创建分组
	if (m_pCurMatchGroup==NULL)
	{
		//设置时间
		m_dwStartTime=(DWORD)time(NULL);
		m_pCurMatchGroup=new CImmediateGroup(CalcMatchNo(),m_pMatchOption,this);
	}

	//最多同时9组编号
	for (int i=0;i<79;i++) m_LoopTimer.Add(i);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);

	//插入空桌
	InsertNullTable();
}

//绑定桌子
bool CImmediateMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
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
bool CImmediateMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
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

	//比赛规则
	m_pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

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

	//创建桌子数组
	CImmediateGroup::m_wChairCount=m_pMatchOption->wMinPartakeGameUser;
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	return true;
}

//时间事件
bool CImmediateMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	if(dwTimerID>=IDI_GROUP_TIMER_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//查看是哪场比赛的定时器
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_TIMER_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//定时检测一下 那些比赛人数不够被卡的问题
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//参赛用户小于椅子数量 就要结束
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CImmediateGroup * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	}

	return true;
}


//数据库事件
bool CImmediateMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//报名结果
		{
			//参数效验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//报名失败
			if(pMatchSignupResult->bResultCode==false)
			{
				//提示消息不为空
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//判断场次
			if (m_pCurMatchGroup->m_lMatchNo!=pMatchSignupResult->lMatchNo)
			{
				SendRoomMessage(pIServerUserItem, TEXT("比赛已经开始,报名失败,请重新报名下一场比赛！"),SMT_CHAT|SMT_EJECT);

				//退还费用
				if (m_pMatchOption->lSignupFee>0)
				{
					//变量定义
					DBR_GR_MatchUnSignup MatchUnSignup;
					MatchUnSignup.dwReason=UNSIGNUP_REASON_SYSTEM;
					MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

					//构造结构
					MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
					MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
					MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
					MatchUnSignup.lMatchNo=pMatchSignupResult->lMatchNo;
					lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

					//投递数据
					m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
				}

				return true;
			}

			//加入比赛
			if (m_pCurMatchGroup->OnUserSignUpMatch(pIServerUserItem)==false)
			{
				SendRoomMessage(pIServerUserItem, TEXT("报名失败"),SMT_CHAT);
				return false;
			}

			//报名成功
			m_OnMatchUserItem.Add(pIServerUserItem);
			SendMatchInfo(pIServerUserItem);
			if (m_pIGameServiceFrame!=NULL)
			{
				SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您分配座位！"),SMT_CHAT);
			}

			//金币更新
			if (pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//退赛结果
		{
			//参数效验
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//退费失败
			if (pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//设置状态
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//发送比赛状态
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			SendData (pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//扣除报名费
			if (pMatchSignupResult->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
			{
				//房间扣费
				if(m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER)
				{
					//退费通知
					TCHAR szDescribe[128]=TEXT("");
					if (m_pMatchOption->lSignupFee>0)
					{
						LPCTSTR pszFeeType[]={TEXT("游戏币"),TEXT("元宝")};
						_sntprintf(szDescribe,CountArray(szDescribe),TEXT("退赛成功，退还报名费%.2f%s！"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
					}
					else
					{
						_sntprintf(szDescribe,CountArray(szDescribe),TEXT("退赛成功！"));
					}
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
				}
				
				//网站扣费
				if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_WEBSITE)
				{
					TCHAR szDescribe[128]=TEXT("退赛成功！");
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
				}
			}

			//比赛晋级
			if (pMatchSignupResult->cbSignupMode==SIGNUP_MODE_MATCH_USER)
			{
				TCHAR szDescribe[128]=TEXT("退赛成功！");
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
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
			IServerUserItem * pIRewardUserItem=NULL;
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//发放奖励
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//查找玩家
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if(pIRewardUserItem==NULL || pIRewardUserItem->IsAndroidUser()==true) continue;

				//写入奖励
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);
			}

			//变量定义
			CImmediateGroup * pImmediateGroup=NULL;

			//查找分组
			for (int i=0;i<m_MatchGroup.GetCount();i++)
			{
				if(m_MatchGroup[i]->m_lMatchNo==pMatchRankList->lMatchNo)
				{
					pImmediateGroup=m_MatchGroup[i];

					if(pImmediateGroup!=NULL)
					{
						//全部起立
						pImmediateGroup->PerformAllUserStandUp();

						//发送状态
						BYTE cbMatchStatus=MS_FREE;
						SendGroupData (MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus),pImmediateGroup);

						//还原编号
						m_LoopTimer.Add(pImmediateGroup->m_LoopTimer);

						//参赛玩家退赛
						POSITION pos=pImmediateGroup->m_OnMatchUserMap.GetStartPosition();
						IServerUserItem *pITempUserItem=NULL;
						DWORD dwUserID=0;
						WORD wQuitUserCount=0;
						WORD wMatchUserCount=(WORD)pImmediateGroup->m_OnMatchUserMap.GetCount();
						while(pos!=NULL)
						{
							pImmediateGroup->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pITempUserItem);
							if (pITempUserItem!=NULL)
							{
								pITempUserItem->SetUserMatchStatus(MUS_NULL);
								if (true==OnEventUserQuitMatch(pITempUserItem,UNSIGNUP_REASON_PLAYER))
									wQuitUserCount++;

								//比赛结束后代打玩家未返回，则踢出房间
								bool bAndroidUser=pITempUserItem->IsAndroidUser();
								bool bTrusteeUser=pITempUserItem->IsTrusteeUser();
								if (bAndroidUser==false && bTrusteeUser==true)
								{
									//离开桌子
									WORD wTableID=pITempUserItem->GetTableID();
									if (wTableID!=INVALID_TABLE)
									{
										//状态判断
										ASSERT (pITempUserItem->GetUserStatus()!=US_PLAYING);
										SendGameMessage(pITempUserItem,TEXT(""),SMT_CLOSE_GAME);
										
										//查找桌子并弹起
										for(int m=0;m<pImmediateGroup->m_MatchTableArray.GetCount();m++)
										{
											if(pITempUserItem->GetTableID()==pImmediateGroup->m_MatchTableArray[m]->wTableID)
											{
												pImmediateGroup->m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pITempUserItem);
												break;
											}
										}
									}

									//离开房间
									if (pITempUserItem->GetBindIndex()==INVALID_WORD)
									{
										pITempUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
									}
								}
							}
						}
						
						//校验退赛结果
						ASSERT(wQuitUserCount==wMatchUserCount);
						if (wQuitUserCount!=wMatchUserCount)
						{
							CTraceService::TraceString(TEXT("比赛结束后，退赛人数异常"),TraceLevel_Exception);
						}

						//回收全部桌子
						for (int j=0;pImmediateGroup->m_MatchTableArray.GetCount();)
						{
							tagTableFrameInfo* pTempTable=pImmediateGroup->m_MatchTableArray[j];
							pImmediateGroup->m_MatchTableArray.RemoveAt(j);
							SafeDelete(pTempTable);
						}
					}
					//移除对象
					m_MatchGroup.RemoveAt(i);
					m_OverMatchGroup.Add(pImmediateGroup);

					break;
				}
			}


			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//奖励结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchRewardResult)) return false;

			//提取数据
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//奖励成功
			if(pMatchRewardResult->bResultCode==true)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//发送数据
				SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//命令消息
bool CImmediateMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
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
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.lMatchNo=m_pCurMatchGroup->m_lMatchNo;	
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();
			MatchSignup.lInitScore=m_pImmediateMatch->lInitalScore;			
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();					
			MatchSignup.wMaxSignupUser=m_pImmediateMatch->wStartUserCount;
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

			//退出比赛
			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	}
	return true;
}

//用户登录
bool CImmediateMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
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
bool CImmediateMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//退出比赛
	OnEventUserQuitMatch(pIServerUserItem, UNSIGNUP_REASON_SYSTEM);

	return true;
}

//登录完成
bool CImmediateMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{	
	//发送比赛信息
	return SendMatchInfo(pIServerUserItem);
}

//进入事件
bool CImmediateMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//手机用户
	if(bIsMobile == true)
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
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//用户参赛
bool CImmediateMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("用户指针为空!"),TraceLevel_Warning);
		return true;
	}

	//机器延迟报名
	if (pIServerUserItem->IsAndroidUser()==true)
	{
		//获取时间
		DWORD dwCurrTime=(DWORD)time(NULL);
		if (dwCurrTime-m_dwStartTime<m_pImmediateMatch->wAndroidDelaySignupTime)
		{
			return true;
		}
	}

	//桌子不够
	WORD wTableCount=GetNullTable();
	if (wTableCount<m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount)
	{
		//构造消息
		TCHAR szString[128];
		_sntprintf(szString,CountArray(szString),TEXT("抱歉，当前桌子不够请等待。还差 %d 张桌子可以报名"),m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount-wTableCount);
		SendRoomMessage(pIServerUserItem,szString,SMT_CHAT);
		return true;
	}

	//人数已满
	if (m_pCurMatchGroup==NULL || m_pCurMatchGroup->m_cbMatchStatus==MatchStatus_Wait)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，该房间参赛人数已满，请稍后再报名或者进入另一房间比赛。"),SMT_CHAT);
		return true;
	}

	//重复报名
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]->GetUserID()==pIServerUserItem->GetUserID())
		{
			SendRoomMessage(pIServerUserItem, TEXT("您已经成功报名，不能重复报名！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	//编号不够
	if (m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，比赛编号不够请稍等。"),SMT_CHAT);
		return true;
	}

	//状态不对
	if (pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("对不起，您当前的状态不允许参加比赛！"),SMT_CHAT);
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

	//缴纳报名费
	if (m_pMatchOption->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
	{
		//真人扣报名费
		if (m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
		{
			//构造结构
			CMD_GR_Match_Fee MatchFee;

			//设置变量
			MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
			MatchFee.lMatchNO=m_pCurMatchGroup->m_lMatchNo;
			MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
			MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
			MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;
			MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

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
	}

	//机器人报名
	if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
	{
		//机器数判断
		if(pIServerUserItem->IsAndroidUser()==true && (m_pCurMatchGroup->m_wAndroidUserCount >= 
		   m_pImmediateMatch->wAndroidUserCount))
		{
			return true;
		}

		//变量定义
		BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
		return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem ,dwSocketID);
	}

	//报名成功
	m_OnMatchUserItem.Add(pIServerUserItem);
	SendMatchInfo(NULL);
	if (m_pIGameServiceFrame!=NULL)
	{
		LONGLONG lScore=LONGLONG(m_pMatchOption->lSignupFee)*-1;
		/*if(m_pImmediateMatch->lInitalScore==0)
		{
			pIServerUserItem->GetUserInfo()->lScore+=lScore;
			m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_FEE);
		}*/
		SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
	}

	return true;
}

//用户退赛
bool CImmediateMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//正在游戏状态
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//正在比赛阶段
	BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
	if (cbUserMatchStatus==MUS_PLAYING)
	{
		return true;
	}

	//否则玩家退赛
	if(RemoveMatchUserItem(pIServerUserItem))
	{
		//从正在比赛的组中删除该玩家
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CImmediateGroup *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem))
			{
				if (pMatch->m_cbMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				}
				return true;
			}
		}

		//从正在报名的组中删除该玩家
		if(m_pCurMatchGroup->OnUserQuitMatch(pIServerUserItem))
		{
			//退还报名费
			if(m_pMatchOption->lSignupFee>=0 /*&& !pIServerUserItem->IsAndroidUser()*/)
			{
				//变量定义
				DBR_GR_MatchUnSignup MatchUnSignup;
				MatchUnSignup.dwReason=cbReason;
				MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

				//构造结构
				MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchUnSignup.lMatchNo=m_pCurMatchGroup->m_lMatchNo;
				lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
			}
			else
			{
				if (m_pCurMatchGroup->m_cbMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				}
			}

			//发送比赛
			SendMatchInfo(NULL);

			return true;
		}

		//发送比赛
		SendMatchInfo(NULL);
	}

	return false;
}

//用户积分
bool CImmediateMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//获取对象
	CImmediateGroup * pImmediateGroup = static_cast<CImmediateGroup *>(pIServerUserItem->GetMatchData());
	if(pImmediateGroup==NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem, cbReason);
	}

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
	UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

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
		WriteGameScore.lMatchNo=pImmediateGroup->m_lMatchNo;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);
	}

	return true;
}

//用户数据
bool CImmediateMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//用户状态
bool CImmediateMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//清除数据
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	//回调接口
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//用户权限
bool CImmediateMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//报名人满，开始比赛
bool CImmediateMatch::OnEventMatchStart(CImmediateGroup *pMatch)
{
	ASSERT(pMatch==m_pCurMatchGroup&&m_LoopTimer.GetCount()>0);

	if (pMatch!=NULL)
	{
		//变量定义
		DBR_GR_MatchStart MatchStart;

		//构造结构
		MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
		MatchStart.lMatchNo=pMatch->m_lMatchNo;
		MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
		MatchStart.wServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));
	
		//编号不够 不能报名的
		pMatch->m_LoopTimer=m_LoopTimer[0];
		m_LoopTimer.RemoveAt(0);
		m_MatchGroup.Add(pMatch);

		//准备新的一组比赛
		m_dwStartTime=(DWORD)time(NULL);
		m_pCurMatchGroup=new CImmediateGroup(CalcMatchNo(),m_pMatchOption,this);
		
		SendMatchInfo(NULL);
		BYTE cbMatchStatus=MS_MATCHING;
		SendGroupData( MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pMatch);
	}

	return true;
}


//比赛结束
bool CImmediateMatch::OnEventMatchOver(CImmediateGroup *pMatch)
{
	//比赛结束
	if(pMatch!=NULL)
	{
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver,sizeof(MatchOver));

		//构造数据				
		MatchOver.bMatchFinish=TRUE;
		MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
		MatchOver.lMatchNo=pMatch->m_lMatchNo;		
		MatchOver.cbMatchType=MATCH_TYPE_IMMEDIATE;
		MatchOver.wPlayCount=m_pImmediateMatch->wPlayCount;
		MatchOver.wServerID=m_pGameServiceOption->wServerID;
		
		//获取时间
		MatchOver.MatchStartTime=pMatch->m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
	}

	return true;
}

//关闭定时器
bool CImmediateMatch::KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)
{
	m_pITimerEngine->KillTimer(dwTimerID);

	return false;
}

//设置定时器
bool CImmediateMatch::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)
{
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}


//发送数据
bool CImmediateMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{

	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatchGroup->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_pImmediateMatch->wStartUserCount;
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	if(pIServerUserItem==NULL)
	{
		return true;
	}

	//发送比赛信息
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));

	//发送报名状态
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

			return true;
		}
	}

	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
	if((LONGLONG)m_pImmediateMatch->lInitalScore!=0)
	{
		pUserScore->lScore=(LONG)m_pImmediateMatch->lInitalScore;
	}

	return SendGroupUserScore(pIServerUserItem, NULL);
}


//发送信息
bool CImmediateMatch::SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		SendGameMessage(pUserItem,pStrMessage, SMT_CHAT|SMT_TABLE_ROLL);
	}
	return true;
}


//发送用户分数
bool CImmediateMatch::SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
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
	UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

	//构造积分
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	if(pMatch!=NULL)
	{
		POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pUserItem=NULL;
		DWORD dwUserID=0;
		while(pos!=NULL)
		{
			pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));	
		}
		return true;
	}

	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}


//发送状态
bool  CImmediateMatch::SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsClientReady()==true);
	if (pIServerUserItem->IsClientReady()==false) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

	//构造数据
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.UserStatus.wTableID=pUserData->wTableID;
	UserStatus.UserStatus.wChairID=pUserData->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserData->cbUserStatus;

	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		if(pUserItem->IsClientReady())
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));	
	}
	return true;
}


//发送游戏消息
bool CImmediateMatch::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true)&&(pIServerUserItem->IsAndroidUser()==false))
	{
		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;
		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}


//发送消息
bool CImmediateMatch::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD && false==pIServerUserItem->IsAndroidUser())
	{
		//构造数据包
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;

		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//发送数据到一组用户
bool CImmediateMatch::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		m_pIGameServiceFrame->SendData(pUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);	
	}
	return true;
}

//发送数据
bool CImmediateMatch::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_COMPUTER,wMainCmdID,wSubCmdID, pData, wDataSize);
    
	return true;
}

//为首轮插入空桌子
void CImmediateMatch::InsertNullTable()
{
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//超过就退出
		WORD nCurTableCount=(WORD)m_pCurMatchGroup->m_MatchTableArray.GetCount();
		if(nCurTableCount>=m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount)break;

		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			tagTableFrameInfo * pTableFrameInfo=new tagTableFrameInfo;
			ZeroMemory(pTableFrameInfo,sizeof(tagTableFrameInfo));			
			pTableFrameInfo->wTableID=i;
			pTableFrameInfo->pTableFrame=m_ppITableFrame[i];
			pTableFrameInfo->lBaseScore=m_pImmediateMatch->lInitalBase;
			ITableFrameHook * pFrameHook=QUERY_OBJECT_PTR_INTERFACE(m_ppITableFrame[i]->GetTableFrameHook(),ITableFrameHook);
			pFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(m_pCurMatchGroup,IUnknownEx));
			m_pCurMatchGroup->AddMatchTable(pTableFrameInfo);
		}
	}
}

//获取一个空闲的机器人
IAndroidUserItem * CImmediateMatch::GetFreeAndroidUserItem()
{
	return NULL;
}

//写入奖励
bool CImmediateMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//写入判断
	bool bWriteReward=(pMatchRankInfo->lRewardGold+pMatchRankInfo->lRewardIngot+pMatchRankInfo->dwRewardExperience)>0;

	//写入奖励
	if (bWriteReward==true)
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

			//构造消息
			//TCHAR szMessage[256]=TEXT("\0");
			//_sntprintf(szMessage,CountArray(szMessage),TEXT("点击查看比赛排行：http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
			//pIServerUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

			////发送消息
			//m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	
		}
	}
	else
	{
		//构造消息
		TCHAR szMessage[256];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("比赛已结束，恭喜您获得第%d名，欢迎您参加其他场次的比赛！"),pMatchRankInfo->wRankID);

		//发送消息
		SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		return true;
	}

	//写入记录
	if(m_pIDataBaseEngine!=NULL)
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

	return true;
}

//计算场次
LONGLONG CImmediateMatch::CalcMatchNo()
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//时间分量
	WORD wDay=SystemTime.wDay;	
	WORD wMonth=SystemTime.wMonth;
	WORD wYear=SystemTime.wYear-1980;
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//记录变量
	static DWORD wMatchCount=0;
	static DWORD wLastDay=SystemTime.wDay;	

	//更新记录
	if (SystemTime.wDay>wLastDay)
	{
		wMatchCount=1;
		wLastDay=SystemTime.wDay;
	}
	else
	{
		wMatchCount++;
	}

	return (LONGLONG(wYear)<<44|LONGLONG(wMonth)<<40|LONGLONG(wDay)<<34|LONGLONG(dwSecond)<<16|LONGLONG(wMatchCount));
}

//获取空桌子
WORD CImmediateMatch::GetNullTable()
{
	WORD wTableCount=0;
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//其他组已经分配了该桌
			}
		}
		//插入桌子
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			wTableCount++;
		}
	}
	return wTableCount;
}

//删除用户
bool CImmediateMatch::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//移除参赛用户
bool CImmediateMatch::RemoveMatchUserItem(IServerUserItem *pIServerUserItem)
{
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];
		
		//用户判断
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());

			m_OnMatchUserItem.RemoveAt(i);
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
