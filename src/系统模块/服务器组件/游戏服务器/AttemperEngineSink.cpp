#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//时间标识

#define IDI_LOAD_ANDROID_USER		(IDI_MAIN_MODULE_START+1)			//机器信息
#define IDI_REPORT_SERVER_INFO		(IDI_MAIN_MODULE_START+2)			//房间信息
#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//连接时间
#define IDI_GAME_SERVICE_PULSE		(IDI_MAIN_MODULE_START+4)			//服务脉冲
#define IDI_DISTRIBUTE_ANDROID		(IDI_MAIN_MODULE_START+5)			//分配机器
#define IDI_DBCORRESPOND_NOTIFY		(IDI_MAIN_MODULE_START+6)			//缓存通知
#define IDI_LOAD_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+7)			//系统消息
#define IDI_SEND_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+8)			//系统消息
#define IDI_LOAD_SENSITIVE_WORD		(IDI_MAIN_MODULE_START+9)			//加载敏感词
#define IDI_DISTRIBUTE_USER		    (IDI_MAIN_MODULE_START+10)			//分配用户
#define IDI_ANDROID_JION_MATCH		(IDI_MAIN_MODULE_START+11)			//分配用户
#define IDI_LOAD_ANDROID_CONFIG		(IDI_MAIN_MODULE_START+12)			//机器人配置
//////////////////////////////////////////////////////////////////////////////////
//时间定义 秒

#define TIME_LOAD_ANDROID_USER		15L									//加载机器
#define TIME_DISTRIBUTE_ANDROID		15L									//分配用户
#define TIME_REPORT_SERVER_INFO		25L									//上报时间
#define TIME_DBCORRESPOND_NOTIFY	3L									//缓存通知时间
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//系统消息时间
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //系统消息时间
#define TIME_LOAD_SENSITIVE_WORD	5L									//加载敏感词时间
#define TIME_LOAD_ANDROID_CONFIG	60L									//加载机器人配置
//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define TEMP_MESSAGE_ID				INVALID_DWORD			


//间隔时间
#define MIN_INTERVAL_TIME			10									//间隔时间
#define MAX_INTERVAL_TIME			1*60								//临时标识

#define ZZMJ_KIND_ID  386
#define HZMJ_KIND_ID  389
#define SET_RULE			1

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//控制变量
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//绑定数据
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//会员参数
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//任务参数
	m_wTaskCount=0;
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//等级参数
	m_wLevelCount=0;
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));

	//状态变量
	m_pInitParameter=NULL;
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameMatchOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;
	m_bHasPersonalRoomService = FALSE;

	//配置数据
	ZeroMemory(&m_DataConfigColumn,sizeof(m_DataConfigColumn));
	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));
	//比赛变量
	m_pIMatchServiceManager=NULL;

	HINSTANCE hInstLibrary = NULL;
#ifdef _DEBUG
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomServiceD.dll"));
#else
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomService.dll"));
#endif
	if (hInstLibrary == NULL)
	{
		FreeLibrary(hInstLibrary); 
	}
	else
	{
		m_bHasPersonalRoomService = true;
	}
	InitializeCriticalSection(&m_cs_test);
	// AllocConsole();
	//freopen("CON","wt",stdout);
	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//删除消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i=0;i<m_SystemMessageBuffer.GetCount();i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//清空数组
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	//清理数据	
	m_KickUserItemMap.RemoveAll();
	m_DistributeManage.RemoveAll();
	DeleteCriticalSection(&m_cs_test);
	return;
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//绑定信息
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//配置比赛
	InitMatchServiceManager();

	//配置机器
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置桌子
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	if(m_pIMatchServiceManager!=NULL)
	{
		if (m_ServerUserManager.SetServerUserItemSink(m_pIMatchServiceManager->GetServerUserItemSink())==false)
		{
			ASSERT(FALSE);
			return false;
		}

		/*if (m_ServerUserManager.SetMatchUserItemSink(m_pIMatchServiceManager->GetMatchUserItemSink())==false)
		{
			ASSERT(FALSE);
			return false;
		}*/
	}
	else
	{
		if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//启动机器
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	m_AndroidUserManager.SetMainServiceFrame(QUERY_ME_INTERFACE(IMainServiceFrame));

	//设置规则
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//设置时间
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//延时加载敏感词
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	//防作弊模式
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//机器人参赛
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		m_pITimerEngine->SetTimer(IDI_ANDROID_JION_MATCH,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//加载机器
	if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) || 
		CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule) )
	{
#ifdef _DEBUG			
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,10000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);
#endif
	}
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_CONFIG,TIME_LOAD_ANDROID_CONFIG*1000L,TIMES_INFINITY,NULL);
	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//配置信息
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameMatchOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//绑定数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//任务变量
	m_wTaskCount=0;
	ZeroMemory(&m_TaskParameter,sizeof(m_TaskParameter));

	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));

	//会员参数
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//等级参数
	m_wLevelCount=0;
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));


	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//删除用户
	m_TableFrameArray.RemoveAll();
	m_DistributeManage.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_ServerListManager.ResetServerList();
	
	//停止服务
	m_AndroidUserManager.ConcludeService();

	//停止比赛
	if(m_pIMatchServiceManager!=NULL)
		m_pIMatchServiceManager->StopService();

	//移除消息
	RemoveSystemMessage();

	//移除任务
	m_UserTaskManager.ResetTaskManager();

	//复位关键字
	m_WordsFilter.ResetSensitiveWordArray();

	//私人配置
	if(m_PersonalTableParameterArray.GetCount() > 0)
	{
		
		INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
		for(INT_PTR i = 0; i < nSize; ++i)
		{
			tagPersonalTableParameter* pPersonalTableParameter = m_PersonalTableParameterArray.GetAt(i);
			SafeDelete(pPersonalTableParameter);
		}
		m_PersonalTableParameterArray.RemoveAll();
	}

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//连接协调
		{
			//发起连接
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("正在连接协调服务器 [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//加载配置
		{
			//加载配置
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//加载列表
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_COLUMN,0L,NULL,0L);

			//加载机器
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);			

			//加载消息
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

			//加载任务
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//会员参数
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_MEMBER_PARAMETER,0,NULL,0);	


			//成长配置
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GROWLEVEL_CONFIG,0,NULL,0);	

			//加载签到
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_CHECKIN_REWARD,0L,NULL,0L);

			//加载低保
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_BASEENSURE,0L,NULL,0L);

			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//私人配置
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PERSONAL_PARAMETER,0,&m_pGameServiceOption->wKindID,sizeof(WORD),0);
			}

			return true;
		}
	}

	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	try
	{

		//调度时间
		if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
		{
			//时间处理
			switch (dwTimerID)
			{
			case IDI_LOAD_ANDROID_USER:		//加载机器
				{
					//变量定义
					DWORD dwBatchID=0;
					DWORD dwAndroidCount=0;

					//加载判断
					if(m_AndroidUserManager.GetAndroidLoadInfo(dwBatchID,dwAndroidCount))
					{
						//变量定义
						tagAndroidParameterEx * pAndroidParameter=NULL;
						pAndroidParameter = m_AndroidUserManager.GetAndroidParameter(dwBatchID);

						//构造结构
						DBR_GR_LoadAndroidUser LoadAndroidUser;
						LoadAndroidUser.dwBatchID=dwBatchID;
						LoadAndroidUser.dwAndroidCount=dwAndroidCount;
						LoadAndroidUser.AndroidCountMember0=pAndroidParameter->AndroidParameter.AndroidCountMember0;
						LoadAndroidUser.AndroidCountMember1=pAndroidParameter->AndroidParameter.AndroidCountMember1;
						LoadAndroidUser.AndroidCountMember2=pAndroidParameter->AndroidParameter.AndroidCountMember2;
						LoadAndroidUser.AndroidCountMember3=pAndroidParameter->AndroidParameter.AndroidCountMember3;
						LoadAndroidUser.AndroidCountMember4=pAndroidParameter->AndroidParameter.AndroidCountMember4;
						LoadAndroidUser.AndroidCountMember5=pAndroidParameter->AndroidParameter.AndroidCountMember5;

						//加载机器
						m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,&LoadAndroidUser,sizeof(LoadAndroidUser));
					}

					return true;
				}
			case IDI_REPORT_SERVER_INFO:	//房间信息
				{
					//变量定义
					CMD_CS_C_ServerOnLine ServerOnLine;
					ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

					//设置变量				
					ServerOnLine.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
					ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerOnLine.dwAndroidCount;

					//发送数据
					m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine));

					return true;
				}
			case IDI_CONNECT_CORRESPOND:	//连接协调
				{
					//发起连接
					tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
					m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//构造提示
					TCHAR szString[512]=TEXT("");
					_sntprintf(szString,CountArray(szString),TEXT("正在连接协调服务器 [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//提示消息
					CTraceService::TraceString(szString,TraceLevel_Normal);

					return true;
				}
			case IDI_GAME_SERVICE_PULSE:	//服务维护
				{
					return true;
				}
			case IDI_LOAD_ANDROID_CONFIG:
				{
					//加载机器
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);		
					return true;
				}
			case IDI_DISTRIBUTE_ANDROID:	//分配机器
				{
					//约战房不分配机器人
					if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
					{
						return true;
					}

					//动作处理
					if (m_AndroidUserManager.GetAndroidCount()>0 && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
					{
						//变量定义
						bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
						bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));

						//模拟处理
						if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
						{
							//机器状态
							tagAndroidUserInfo AndroidSimulate; 
							m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);

							//机器处理
							if (AndroidSimulate.wFreeUserCount>0)
							{
								for (WORD i=0;i<8;i++)
								{
									//随机桌子
									WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));

									//获取桌子
									CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;


									if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
									{
										if (pTableFrame->GetTableAndroidConut()>=3)continue;
									}
									else if(m_pGameServiceOption->wKindID ==200)
									{
										if (pTableFrame->GetTableAndroidConut()>=2)continue;
									}
									else if(m_pGameServiceOption->wKindID ==6)
									{
										if (pTableFrame->GetTableAndroidConut()>=3)continue;
									}

									//桌子状况
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//分配判断
									if (TableUserInfo.wTableUserCount>0) continue;
									if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

									//坐下判断
									if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
									{
										//变量定义
										WORD wHandleCount=0;
										WORD wWantAndroidCount=TableUserInfo.wMinUserCount;

										//数据调整
										if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
										{
											WORD wChairCount=m_pGameServiceAttrib->wChairCount;
											WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
											WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
											wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
										}

										WORD wAndroid = pTableFrame->GetTableAndroidConut();
										//坐下处理
										for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
										{
											//变量定义
											WORD wChairID=pTableFrame->GetRandNullChairID();

											if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
											{
												if (wAndroid + wHandleCount >=3)break;
											}
											else if(m_pGameServiceOption->wKindID ==200)
											{
												if (wAndroid + wHandleCount>=2)break;
											}
											else if(m_pGameServiceOption->wKindID ==6)
											{
												if (wAndroid + wHandleCount>=3)break;
											}
											//无效过滤
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											//用户坐下
											IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
											if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
											{
												//设置变量
												wHandleCount++;

												//完成判断
												if (wHandleCount>=wWantAndroidCount) 
												{
													break;
												}
											}
										}

										if(wHandleCount > 0) break;
									}
								}
							}
						}

						//陪打处理
						if (bAllowAndroidAttend==true)
						{
							//被动状态
							tagAndroidUserInfo AndroidPassivity;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

							if(bAllowAvertCheatMode)
							{
								//坐下处理
								for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
									if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
								//被动处理
								if (AndroidPassivity.wFreeUserCount>0)
								{
									//百人游戏
									if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
									{
										for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
										{
											//获取桌子
											CTableFrame * pTableFrame=m_TableFrameArray[i];
											if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
											if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
											{
												if (pTableFrame->GetTableAndroidConut()>=3)continue;
											}
											else if(m_pGameServiceOption->wKindID ==200)
											{
												if (pTableFrame->GetTableAndroidConut()>=2)continue;
											}
											else if(m_pGameServiceOption->wKindID ==6)
											{
												if (pTableFrame->GetTableAndroidConut()>=3)continue;
											}
											//桌子状况
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//分配判断
											if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

											//变量定义
											IServerUserItem * pIServerUserItem=NULL;
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//无效过滤
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											WORD wHandleCount=0;
											WORD wAndroid = pTableFrame->GetTableAndroidConut();
											//坐下处理
											for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
											{
												if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
												{
													if (wAndroid + wHandleCount >=3)break;
												}
												else if(m_pGameServiceOption->wKindID ==200)
												{
													if (wAndroid + wHandleCount>=2)break;;	
												}
												else if(m_pGameServiceOption->wKindID ==6)
												{
													if (wAndroid + wHandleCount>=3)break;;
												}

												IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];

												if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) 
												{
													wHandleCount++;
													return true;
												}
											}
										}
									}
									else
									{
										for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
										{
											//获取桌子
											CTableFrame * pTableFrame=m_TableFrameArray[i];
											if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
											if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
											{
												if (pTableFrame->GetTableAndroidConut()>=3)continue;
											}
											else if(m_pGameServiceOption->wKindID ==200)
											{
												if (pTableFrame->GetTableAndroidConut()>=2)continue;
											}
											else if(m_pGameServiceOption->wKindID ==6)
											{
												if (pTableFrame->GetTableAndroidConut()>=3)continue;
											}
											//桌子状况
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//分配判断
											if (wUserSitCount==0) continue;
											if (TableUserInfo.wTableUserCount==0) continue;
											if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%100>50)) continue;

											//变量定义
											IServerUserItem * pIServerUserItem=NULL;
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//无效过滤
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											WORD wHandleCount=0;
											WORD wAndroid = pTableFrame->GetTableAndroidConut();
											//坐下处理
											for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
											{
												if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
												{
													if (wAndroid + wHandleCount >=3)break;;
												}
												else if(m_pGameServiceOption->wKindID ==200)
												{
													if (wAndroid + wHandleCount>=2)break;;
												}
												else if(m_pGameServiceOption->wKindID ==6)
												{
													if (wAndroid + wHandleCount>=3)break;;
												}

												IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];

												if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) 
												{
													wHandleCount++;
													return true;
												}
											}
										}
									}
								}
							}
						}

						//陪打处理
						if (bAllowAndroidAttend==true)
						{
							//主动状态
							tagAndroidUserInfo AndroidInitiative;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
							WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;

							if(bAllowAvertCheatMode)
							{
								//坐下处理
								for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
									if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
								//主动处理
								if (AndroidInitiative.wFreeUserCount>0)
								{
									for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
									{
										//获取桌子
										CTableFrame * pTableFrame=m_TableFrameArray[i];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
										if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
										{
											if (pTableFrame->GetTableAndroidConut()>=3)continue;
										}
										else if(m_pGameServiceOption->wKindID ==200)
										{
											if (pTableFrame->GetTableAndroidConut()>=2)continue;
										}
										else if(m_pGameServiceOption->wKindID ==6)
										{
											if (pTableFrame->GetTableAndroidConut()>=3)continue;
										}
										//桌子状况
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//分配判断
										if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1) && (rand()%100>50)) continue;

										//变量定义
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//无效过滤
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										WORD wHandleCount=0;
										WORD wAndroid = pTableFrame->GetTableAndroidConut();
										//坐下处理
										for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
										{
											if (m_pGameServiceOption->wKindID == 27||m_pGameServiceOption->wKindID == 28||m_pGameServiceOption->wKindID == 36)
											{
												if (wAndroid + wHandleCount >=3)break;;

											}
											else if(m_pGameServiceOption->wKindID ==200)
											{
												if (wAndroid + wHandleCount>=2)break;;
											}
											else if(m_pGameServiceOption->wKindID ==6)
											{
												if (wAndroid + wHandleCount>=3)break;;
											}

											IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];

											if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) 
											{
												wHandleCount++;
												return true;
											}
										}
									}
								}
							}
						}

						bool bSinglePlayerGame=m_pGameServiceAttrib->wChairCount==1;
						if (bSinglePlayerGame)
						{
							//获取时间
							DWORD dwCurrTime = (DWORD)time(NULL);
							if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

							//设置变量
							m_dwLastDisposeTime=dwCurrTime;
							m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
						}

						//变量定义
						bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

						//类型判断
						if(bMaxPlayerGame)
						{
							//获取时间
							DWORD dwCurrTime = (DWORD)time(NULL);
							if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

							//设置变量
							m_dwLastDisposeTime=dwCurrTime;
							m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
						}

						//起立处理
						WORD wStandUpCount=0;
						WORD wRandCount=((rand()%3)+1);
						INT_PTR nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
						for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
						{
							//获取桌子
							INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
							CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
							if (pTableFrame->IsGameStarted()==true) continue;

							//桌子状况
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//用户过虑
							bool bRand = ((rand()%100)>50);
							if (TableUserInfo.wTableAndroidCount==0) continue;
							if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
							if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;

							//起立处理
							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
							{
								//获取用户
								IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
								if (pIServerUserItem==NULL) continue;

								//用户起立
								if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
								{
									wStandUpCount++;
									if(wStandUpCount>=wRandCount)
										return true;
									else
										break;
								}
							}
						}

						//起立处理
						nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
						for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
						{
							//获取桌子
							INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
							CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
							if (pTableFrame->IsGameStarted()==true) continue;

							//桌子状况
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//用户过虑
							bool bRand = ((rand()%100)>50);
							if (TableUserInfo.wTableAndroidCount==0) continue;
							if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;

							//起立处理
							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
							{
								//获取用户
								IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
								if (pIServerUserItem==NULL) continue;

								//用户起立
								if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
								{
									wStandUpCount++;
									if(wStandUpCount>=wRandCount)
										return true;
									else
										break;
								}
							}
						}
					}	

					return true;
				}
			case IDI_DBCORRESPOND_NOTIFY: //缓存定时处理
				{
					if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
					return true;
				}
			case IDI_LOAD_SYSTEM_MESSAGE: //系统消息
				{
					//清除消息数据
					RemoveSystemMessage();

					//加载消息
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

					return true;
				}
			case IDI_LOAD_SENSITIVE_WORD:	//加载敏感词
				{
					//投递请求
					m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
					return true;
				}
			case IDI_SEND_SYSTEM_MESSAGE: //系统消息
				{
					//数量判断
					if(m_SystemMessageActive.GetCount()==0) return true;

					//时效判断
					DWORD dwCurrTime = (DWORD)time(NULL);
					for(INT_PTR nIndex=m_SystemMessageActive.GetCount()-1;nIndex>=0;nIndex--)
					{
						tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

						//时效判断
						if(pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
						{
							m_SystemMessageActive.RemoveAt(nIndex);
							SafeDelete(pTagSystemMessage);

							continue;
						}

						//间隔判断
						if(pTagSystemMessage->dwLastTime+pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
						{
							//更新数据
							pTagSystemMessage->dwLastTime=dwCurrTime;

							//构造消息
							CMD_GR_SendMessage SendMessage = {};
							SendMessage.cbAllRoom = (pTagSystemMessage->SystemMessage.dwMessageID==TEMP_MESSAGE_ID)?TRUE:FALSE;
							SendMessage.cbGame = (pTagSystemMessage->SystemMessage.cbMessageType==1)?TRUE:FALSE;
							SendMessage.cbRoom = (pTagSystemMessage->SystemMessage.cbMessageType==2)?TRUE:FALSE;
							if(pTagSystemMessage->SystemMessage.cbMessageType==3)
							{
								SendMessage.cbGame = TRUE;
								SendMessage.cbRoom = TRUE;
							}
							lstrcpyn(SendMessage.szSystemMessage,pTagSystemMessage->SystemMessage.szSystemMessage,CountArray(SendMessage.szSystemMessage));
							SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage)+1;

							//发送消息
							WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
							SendSystemMessage(&SendMessage,wSendSize);
						}
					}

					return true;
				}
			case IDI_DISTRIBUTE_USER: //分配用户
				{
					//执行分组
					PerformDistribute();

					return true;
				}
			case IDI_ANDROID_JION_MATCH:	//机器人参赛
				{
					//比赛类型
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
					{
						//陪玩标识
						bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);

						//陪玩处理
						if (bAllowAndroidAttend==true)
						{
							//被动状态
							tagAndroidUserInfo AndroidPassivity;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

							//坐下处理
							WORD wIndex = rand()%(AndroidPassivity.wFreeUserCount+1);
							for (WORD j=wIndex;j<AndroidPassivity.wFreeUserCount+wIndex;j++)
							{
								WORD wAndroidIndex = j%AndroidPassivity.wFreeUserCount;
								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[wAndroidIndex];
								if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
							}
						}
					}

					return true;
				}
			}
		}

		//机器时器
		if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
		{
			//时间处理
			m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

			return true;
		}

		//比赛定时器
		if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
		{
			if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
			return true;
		}

		//桌子时间
		if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
		{
			//桌子号码
			DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
			WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

			//时间效验
			if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
			{
				ASSERT(FALSE);
				return false;
			}

			//时间通知
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
			return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
		}
	}catch(...)
	{
		TCHAR szMessage[128];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("定时器异常：TimerID=%d"),dwTimerID);
		CTraceService::TraceString(szMessage,TraceLevel_Warning);
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_LOGON_SUCCESS:			//登录成功
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOGON_FAILURE:			//登录失败
		{
			return OnDBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PARAMETER:			//游戏参数
		{
			return OnDBGameParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_COLUMN_INFO:		//列表信息
		{
			return OnDBGameColumnInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_PARAMETER:	//机器参数
		{
			return OnDBGameAndroidParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_INFO:		//机器信息
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}

	case DBO_GR_GAME_PROPERTY_BUFF:		//道具Buff
		{
			return OnDBGamePropertyBuff(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOAD_TRUMPET:			//喇叭信息
		{
			return OnDBGameLoadTrumpet(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SEND_TRUMPET:			//使用喇叭
		{
			return	OnDBGameSendTrumpet(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_MEMBER_PAREMETER:  //会员参数
		{
			return OnDBGameMemberParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_QUERY_INFO_RESULT://会员查询
		{
			return OnDBPCMemberDayQueryInfoResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_DAY_PRESENT_RESULT://会员送金
		{
			return OnDBPCMemberDayPresentResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_DAY_GIFT_RESULT://会员礼包
		{
			return OnDBPCMemberDayGiftResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_INFO:		//银行信息
		{
			return OnDBUserInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_SUCCESS:	//银行成功
		{
			return OnDBUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_FAILURE:	//银行失败
		{
			return OnDBUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_USER_INFO:  //用户信息
		{
			return OnDBUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_ENABLE_RESULT: //开通结果
		{
			return OnDBUserInsureEnableResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_SUCCESS:		//道具成功
		{
			return OnDBPropertySuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_RESULT:  //系统消息
		{
			return OnDBSystemMessage(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_FINISH:	//加载完成
		{
			return OnDBSystemMessageFinish(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SENSITIVE_WORDS:		//加载敏感词
		{
			return OnDBSensitiveWords(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_INFO:			//签到信息
		{
			return OnDBPCUserCheckInInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_RESULT:			//签到结果
		{
			return OnDBPCUserCheckInResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_REWARD:			//签到奖励
		{
			return OnDBPCCheckInReward(dwContextID,pData,wDataSize);
		}
	case DBO_GR_BASEENSURE_PARAMETER:	//低保参数
		{
			return OnDBPCBaseEnsureParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_BASEENSURE_RESULT:		//低保结果
		{
			return OnDBPCBaseEnsureResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST:				//任务列表
		{
			return OnDBUserTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST_END:			//任务结束
		{
			return OnDBUserTaskListEnd(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_INFO:				//任务信息
		{
			return OnDBUserTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_RESULT:			//任务结果
		{
			return OnDBUserTaskResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PURCHASE_RESULT:		//购买结果
		{
			return OnDBPurchaseResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_EXCHANGE_RESULT:		//兑换结果
		{
			return OnDBExChangeResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_CONFIG:		//等级配置
		{
			return OnDBPCGrowLevelConfig(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_PARAMETER:	//等级参数
		{
			return OnDBPCGrowLevelParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_UPGRADE:		//等级升级
		{
			return OnDBPCGrowLevelUpgrade(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_BUY:		//购买结果
		{
			return OnDBGamePropertyBuy(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_FAILURE:		//道具失败
		{
			return OnDBPropertyFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_QUERY_BACKPACK:			//背包结果
		{
			return OnDBQueryPropertyBackpack(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_USE:			//使用结果
		{
			return OnDBQueryPropertyUse(dwContextID,pData,wDataSize);
		}
	case DBO_GR_QUERY_SEND_PRESENT:		//查询赠送
		{
			return OnDBQuerySendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_PRESENT:		//道具赠送
		{
			return OnDBQueryPropertyPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GET_SEND_PRESENT:		//获取赠送
		{
			return OnDBQueryGetSendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CREATE_SUCCESS:			//创建成功
		{
			return OnDBCreateSucess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CREATE_FAILURE:			//创建失败
		{
			return OnDBCreateFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CANCEL_CREATE_RESULT:		//取消创建
		{
			return OnDBCancelCreateTable(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOAD_PERSONAL_ROOM_OPTION:
		{
			ASSERT(wDataSize == sizeof(tagPersonalRoomOption));
			if (sizeof(tagPersonalRoomOption)!=wDataSize) return false;
			tagPersonalRoomOption * pPersonalRoomOption = (tagPersonalRoomOption *)pData;
			memcpy(&m_PersonalRoomOption, pPersonalRoomOption, sizeof(tagPersonalRoomOption) );
			return true;
		}
	case DBO_GR_LOAD_PERSONAL_PARAMETER:
		{
			return OnDBLoadPersonalParameter(dwContextID, pData, wDataSize);
		}
	case DBO_GR_DISSUME_TABLE_RESULTE:
		{
			return OnDBDissumeTableResult(dwContextID, pData, wDataSize);
		}
	case DBO_GR_CURRENCE_ROOMCARD_AND_BEAN:
		{
			return OnDBCurrenceRoomCardAndBeant(dwContextID, pData, wDataSize);
		}
	case DBO_GR_UPDATE_CONTROLINFO:		//更新控制
		{
			return OnDBUpdateControlInfo(dwContextID,pData,wDataSize);
		}
	}

	//比赛事件
	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//参数效验
		if(m_pIMatchServiceManager==NULL) return false;

		//查找用户
		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem * pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		return m_pIMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize,dwContextID);
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置变量
		m_bCollectUser=false;

		//删除时间
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//重连判断
		if (m_bNeekCorrespond==true)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("与协调服务器的连接关闭了，%ld 秒后将重新连接"),m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在发送游戏房间注册信息..."),TraceLevel_Normal);

		//变量定义
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//服务端口
		CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
		RegisterServer.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//构造数据
		RegisterServer.wKindID=m_pGameServiceOption->wKindID;
		RegisterServer.wNodeID=m_pGameServiceOption->wNodeID;
		RegisterServer.wSortID=m_pGameServiceOption->wSortID;
		RegisterServer.wServerID=m_pGameServiceOption->wServerID;
		RegisterServer.wServerKind=m_pGameServiceOption->wServerKind;
		RegisterServer.wServerType=m_pGameServiceOption->wServerType;	
		RegisterServer.wServerLevel=m_pGameServiceOption->wServerLevel;
		RegisterServer.lCellScore=m_pGameServiceOption->lCellScore;
		RegisterServer.cbEnterMember=m_pGameServiceOption->cbMinEnterMember;
		RegisterServer.lEnterScore=m_pGameServiceOption->lMinEnterScore;
		RegisterServer.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
		RegisterServer.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		RegisterServer.dwServerRule=m_pGameServiceOption->dwServerRule&(SR_ALLOW_AVERT_CHEAT_MODE | SR_SUPORT_TYPE_PC | SR_SUPORT_TYPE_MOBILE);
		RegisterServer.wTableCount = m_pGameServiceOption->wTableCount;

		lstrcpyn(RegisterServer.szServerName,m_pGameServiceOption->szServerName,CountArray(RegisterServer.szServerName));
		lstrcpyn(RegisterServer.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(RegisterServer.szServerAddr));

		//发送数据
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVER,&RegisterServer,sizeof(RegisterServer));

		//如果是约战房发送创建房间限制
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			CMD_CS_S_RegisterPersonal  RegisterPersonal;
			RegisterPersonal.dwKindID = m_pGameServiceOption->wKindID;
			RegisterPersonal.dwMaxCreate = m_PersonalRoomOption.wCanCreateCount;
			m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_PERSONAL, &RegisterPersonal, sizeof(RegisterPersonal));
		}
		
		//是否使用金币库
		bool bIsTreasureDB = false;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
		{
			bIsTreasureDB = true;
		}

		//桌子状态
		for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[i];
			//如果使用的是金币数据库
			if (bIsTreasureDB)
			{
				pTableFrame->SetDataBaseMode(0);
			}
			else
			{
				pTableFrame->SetDataBaseMode(1);
			}

		}
		//设置时间
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//服务信息
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//用户汇总
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_ANDROID_SERVICE: //机器服务
			{
				return OnTCPSocketMainAndroidService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//获取用户
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;
	WORD wTableID=INVALID_WORD;

	try
	{
		//用户处理
		if (pIServerUserItem!=NULL)
		{
			//变量定义
			wTableID=pIServerUserItem->GetTableID();

			WORD wDrawCount = 0;
			if(wTableID!=INVALID_TABLE)
			{
				CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
				wDrawCount = pTableFrame->GetDrawCount();
			}

			//断线处理
			BYTE cbMatchStatus = pIServerUserItem->GetUserMatchStatus();
			bool bImmediteMatch = (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE);
			if (wTableID!=INVALID_TABLE || (bImmediteMatch==true && cbMatchStatus == MUS_PLAYING)|| (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wDrawCount!=0 ))
			{
				//解除绑定
				pIServerUserItem->DetachBindStatus();
				if(wTableID < m_pGameServiceOption->wTableCount)
				{
					//断线通知
					ASSERT(wTableID<m_pGameServiceOption->wTableCount);
					m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
				}
				else //先不处理看有什么问题
				{

				}
			}
			else
			{
				pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
			}
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("关闭连接异常: wTableID=%d"),wTableID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize == 866)
	{
		TCHAR szMessage[260]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("会发生异常的消息: Command.wMainCmdID =%d   Command.wSubCmdID = %d"), Command.wMainCmdID, Command.wSubCmdID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//游戏命令
		{
			bool bReturnVal = OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				//输出信息
				TCHAR szBuffer[128]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("MDM_GF_GAME 游戏命令返回 [ %ld ]"),Command.wSubCmdID);
				//输出信息
				CTraceService::TraceString(szBuffer,TraceLevel_Normal);

			}
			return bReturnVal;
		}
	case MDM_GF_FRAME:		//框架命令
		{
			bool bReturnVal =  OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				CTraceService::TraceString(TEXT("MDM_GF_FRAME 框架命令返回 false"),TraceLevel_Normal);
			}
			return bReturnVal;
		}
	case MDM_GR_INSURE:		//银行命令
		{
			return OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_TASK:		//任务命令
		{
			return OnTCPNetworkMainTask(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_EXCHANGE:	//兑换命令
		{
			return OnTCPNetworkMainExchange(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGE:		//管理命令
		{
			return OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//比赛命令
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MEMBER:	//会员命令
		{
			return OnTCPNetworkMainMember(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PROPERTY:	//道具命令
		{
			return OnTCPNetworkMainProperty(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_BASEENSURE:	//低保命令
		{
			return OnTCPNetworkMainBaseEnsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_CHECKIN: //签到命令
		{
			return OnTCPNetworkMainCheckIn(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PERSONAL_TABLE:
		{
			return OnTCPNetworkMainPersonalTable(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_ALL_CLIENT);

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_ALL_CLIENT);

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendHallMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_HALL_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true))
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	if (bAndroid)
	{
		//机器用户
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//常规用户
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//机器数据
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//用户数据
	if (cbSendMask==BG_ALL_CLIENT)
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_MOBILE);
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_COMPUTER);
	}
	else
	{
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//机器用户
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//网络用户
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}

	return false;
}


//群发数据
bool CAttemperEngineSink::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//枚举用户
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//过滤用户
		IServerUserItem *pITargetUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pITargetUserItem==NULL) continue;
		//if(!pITargetUserItem->IsAndroidUser())
			if(pITargetUserItem->IsMobileUser() == false) continue;

		//定义变量
		WORD wMobileUserRule = pITargetUserItem->GetMobileUserRule();
		WORD wTagerTableID = pITargetUserItem->GetTableID();
		bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
		bool bRecviceGameChat = ((wMobileUserRule&RECVICE_GAME_CHAT)!=0);
		bool bRecviceRoomChat = ((wMobileUserRule&RECVICE_ROOM_CHAT)!=0);
		bool bRecviceRoomWhisper = ((wMobileUserRule&RECVICE_ROOM_WHISPER)!=0);

		//状态过滤
		if((pITargetUserItem->GetUserStatus() >= US_SIT) && pITargetUserItem->IsClientReady() &&(m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH))
		{
			if(wCmdTable!=INVALID_WORD && wTagerTableID != wCmdTable)continue;
		}

		//聊天过滤
		if(wSubCmdID==SUB_GR_USER_CHAT || wSubCmdID==SUB_GR_USER_EXPRESSION)
		{
			if(!bRecviceGameChat || !bRecviceRoomChat) continue;
		}
		if(wSubCmdID==SUB_GR_WISPER_CHAT || wSubCmdID==SUB_GR_WISPER_EXPRESSION)
		{
			if(!bRecviceRoomWhisper) continue;
		}

		//部分可视
		if(!bViewModeAll)
		{
			//消息过滤
			if(wMainCmdID==MDM_GR_USER && wSubCmdID==SUB_GR_USER_ENTER && wCmdTable==INVALID_TABLE) continue;
			
			//定义变量
			WORD wTagerDeskPos = pITargetUserItem->GetMobileUserDeskPos();
			WORD wTagerDeskCount = m_pInitParameter->m_wVisibleTableCount;
			WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;

			//状态消息过滤
			if (wCmdTable!=INVALID_TABLE)
			{
				WORD wLastPagePos=(m_pGameServiceOption->wTableCount/wTagerDeskCount)*wTagerDeskCount;

				if (wCmdTable==0)
				{
					if ((wTagerDeskPos < 0)||(wTagerDeskPos > 2*wTagerDeskCount))continue;
				}
				else if (wCmdTable==wLastPagePos)
				{
					if ((wTagerDeskPos < wLastPagePos-2*wTagerDeskCount)||(wTagerDeskPos > wLastPagePos+wTagerDeskCount))continue;
				}
				else
				{
					if ((wTagerDeskPos < wCmdTable-wEverySendPage*wTagerDeskCount)||(wTagerDeskPos > wCmdTable+wEverySendPage*wTagerDeskCount+wTagerDeskCount-1))continue;
				}
			}
		}

		//发送消息
		SendData(pITargetUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//用户积分
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
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
	UserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

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
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

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
	MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//构造积分
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//发送数据
	SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));


	//即时写分
	if (
		(CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
		&&(pIServerUserItem->IsVariation()==true) || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
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

		//调整分数
		if(pIServerUserItem->IsAndroidUser()==true)
		{
			WriteGameScore.VariationInfo.lScore=0;
			WriteGameScore.VariationInfo.lGrade=0;
			WriteGameScore.VariationInfo.lInsure=0;
			WriteGameScore.VariationInfo.lRevenue=0;
		}

		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			if ((lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) != 0))
			{
				WriteGameScore.VariationInfo.lScore=0;
				WriteGameScore.VariationInfo.lGrade=0;
				WriteGameScore.VariationInfo.lInsure=0;
				WriteGameScore.VariationInfo.lRevenue=0;
			}
		}

		//查找任务
		tagUserTaskEntry * pUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(),TASK_STATUS_UNFINISH); 
		if(pUserTaskEntry!=NULL)
		{
			ShowLog(TEXT("查找任务进来么？"));
			WriteGameScore.bTaskForward = true;
			ShowLog(TEXT("%d--等于-----"),WriteGameScore.bTaskForward);
		}

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);


		//私人房间ID
		DWORD dwRoomHostID = 0;
		TCHAR szPersonalRoomID[ROOM_ID_LEN] = {0};
		INT_PTR nSize = m_TableFrameArray.GetCount();
		for(INT_PTR i = 0; i < nSize; ++i)
		{
			CTableFrame* pTableFrame = m_TableFrameArray.GetAt(i);
			if (pTableFrame->GetTableID() == pIServerUserItem->GetTableID())
			{
				lstrcpyn(szPersonalRoomID, pTableFrame->GetPersonalTableID(), sizeof(szPersonalRoomID));
				dwRoomHostID = pTableFrame->GetRecordTableOwner();
			}
		}

		//私人房间分数数据结构
		DBR_GR_WritePersonalGameScore  WritePersonalScore;
		WritePersonalScore.dwRoomHostID = dwRoomHostID;
		WritePersonalScore.bTaskForward = WriteGameScore.bTaskForward;
		WritePersonalScore.dwClientAddr = WriteGameScore.dwClientAddr;
		WritePersonalScore.dwDBQuestID  = WriteGameScore.dwDBQuestID;
		WritePersonalScore.dwInoutIndex = WriteGameScore.dwInoutIndex;
		//WritePersonalScore.dwMatchID    = WriteGameScore.dwMatchID;
		//WritePersonalScore.dwMatchNO	= WriteGameScore.dwMatchNO;
		WritePersonalScore.dwUserID		= WriteGameScore.dwUserID;
		WritePersonalScore.dwPersonalRoomTax = m_PersonalRoomOption.lPersonalRoomTax;
		lstrcpyn(WritePersonalScore.szRoomID, szPersonalRoomID, sizeof(WritePersonalScore.szRoomID));
		memcpy(&WritePersonalScore.VariationInfo, &WriteGameScore.VariationInfo, sizeof(WritePersonalScore.VariationInfo));
		WritePersonalScore.VariationInfo.lScore += WritePersonalScore.VariationInfo.lRevenue;
		WritePersonalScore.VariationInfo.lRevenue = 0;

		//请求数据库写入积分
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			m_pIDBCorrespondManager->PostDataBaseRequest(WritePersonalScore.dwUserID,DBR_GR_WRITE_PERSONAL_GAME_SCORE,0L,&WritePersonalScore,sizeof(WritePersonalScore), TRUE);
		}

		//推进任务
		if(pUserTaskEntry!=NULL)
		{
			PerformTaskProgress(pIServerUserItem,pUserTaskEntry,WriteGameScore.VariationInfo.dwWinCount,WriteGameScore.VariationInfo.dwLostCount,WriteGameScore.VariationInfo.dwDrawCount);
		}
	}

	//通知桌子
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		m_TableFrameArray[pIServerUserItem->GetTableID()]->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	}

	return true;
}

//用户状态
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if (pIServerUserItem->GetUserInfo()->dwUserID == 6346   || pIServerUserItem->GetUserInfo()->dwUserID == 6354)
	{
		printf(("OnEventUserItemStatus游戏结束玩家托管退出________________________________________%d\n"),pIServerUserItem->GetUserInfo()->dwUserID);
	}
	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//构造数据
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus=pIServerUserItem->GetUserStatus();

	//用户信息
	WORD wTableID=pIServerUserItem->GetTableID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//修改信息
	if(pIServerUserItem->GetUserStatus()>=US_SIT)
	{
		//定义变量
		WORD wNewDeskPos = (pIServerUserItem->GetTableID()/m_pInitParameter->m_wVisibleTableCount)*m_pInitParameter->m_wVisibleTableCount;
		WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-m_pInitParameter->m_wVisibleTableCount;
		if(wNewDeskPos > wMaxDeskPos) wNewDeskPos = wMaxDeskPos;
		pIServerUserItem->SetMobileUserDeskPos(wNewDeskPos);
	}

	//发送数据
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	if ((m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) || (CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)==true))
	{
		if (pIServerUserItem->IsMobileUser()==true)
		{
			if (cbUserStatus==US_SIT || cbUserStatus==US_READY)
			{
				SendUserInfoPacketBatchToPage(pIServerUserItem);
			}
		}

		//发送消息
		if(cbUserStatus>=US_SIT)
		{
			/*if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
				SendUserInfoPacketBatchToPage(pIServerUserItem);*/
			SendDataBatchToMobileUser(wTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
		}
		else
		{
			SendDataBatchToMobileUser(wOldTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
		}
	}
	else
	{
		if(cbUserStatus>=US_SIT)
		{
			if (wOldTableID==INVALID_TABLE && cbUserStatus==US_SIT)
			{
				SendUserInfoPacketBatchToPage(pIServerUserItem);
			}
			else if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
			{
				SendDataBatchToMobileUser(wOldTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
			}
			
			SendDataBatchToMobileUser(wTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
		}
		else
		{
			SendDataBatchToMobileUser(wOldTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
		}
	}

	//发送玩家状态
	CMD_CS_C_UserStatus  Status;
	Status.dwUserID = UserStatus.dwUserID;
	Status.cbUserStatus = UserStatus.UserStatus.cbUserStatus;
	Status.wKindID  = m_pGameServiceOption->wKindID;
	Status.wServerID =m_pGameServiceOption->wServerID;
	Status.wTableID = UserStatus.UserStatus.wTableID;
	Status.wChairID = UserStatus.UserStatus.wChairID;
	m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_STATUS,&Status,sizeof(Status));


	//离开判断
	if (pIServerUserItem->GetUserStatus()==US_NULL)
	{
		//获取绑定
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//绑带处理
		if (pBindParameter!=NULL)
		{
			//绑定处理
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//中断网络
			if (pBindParameter->dwSocketID!=0L)
			{
				if (LOWORD(pBindParameter->dwSocketID)>=INDEX_ANDROID)
				{
					m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,false);
				}
				else
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}
		}

		//离开处理
		OnEventUserLogout(pIServerUserItem,0L);
	}
	return true;
}

//用户权限
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//比赛权限
	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;
	ManageUserRight.bGameRight=cbRightKind==UR_KIND_GAME;
	
	//发送请求
	m_pIDBCorrespondManager->PostDataBaseRequest(ManageUserRight.dwUserID,DBR_GR_MANAGE_USER_RIGHT,0,&ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//用户数据
bool CAttemperEngineSink::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	LPCTSTR pszUserGameData=pIServerUserItem->GetUserGameData();
	INT nLenght=lstrlen(pszUserGameData);
	
	ASSERT(nLenght>0);
	if (nLenght<=0) return false;

	CMD_GR_UserGameData UserGameData;
	ZeroMemory(&UserGameData,sizeof(UserGameData));

	UserGameData.dwUserID=pIServerUserItem->GetUserID();
	lstrcpyn(UserGameData.szUserGameData,pszUserGameData,CountArray(UserGameData.szUserGameData));

	//发送数据
	WORD wDataSize=CountStringBuffer(UserGameData.szUserGameData);
	WORD wHeadSize=sizeof(UserGameData)-sizeof(UserGameData.szUserGameData);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_GAME_DATA,&UserGameData,wHeadSize+wDataSize);

	//写数据库
	DBR_GR_WriteUserGameData WriteUserGameData;
	ZeroMemory(&WriteUserGameData,sizeof(WriteUserGameData));

	WriteUserGameData.wKindID=m_pGameServiceOption->wKindID;
	WriteUserGameData.dwUserID=pIServerUserItem->GetUserID();
	WriteUserGameData.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(WriteUserGameData.szUserGameData,pszUserGameData,CountArray(WriteUserGameData.szUserGameData));

	//发送请求
	DWORD dwUserID=pIServerUserItem->GetUserID();
	WORD wDataSize1=CountStringBuffer(WriteUserGameData.szUserGameData);
	WORD wHeadSize1=sizeof(WriteUserGameData)-sizeof(WriteUserGameData.szUserGameData);
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_WRITE_USER_GAME_DATA,0,&WriteUserGameData,wHeadSize1+wDataSize1);

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//废弃判断
	if ((pBindParameter->pIServerUserItem!=NULL)||(pBindParameter->dwSocketID!=dwContextID))
	{
		//错误断言
		ASSERT(FALSE);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_NORMAL);

		return true;
	}

	//变量定义
	bool bAndroidUser=(wBindIndex>=INDEX_ANDROID);
	bool bMobileUser=(pBindParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//被踢判断
	DWORD dwKickTime;
	if(m_KickUserItemMap.Lookup(pDBOLogonSuccess->dwUserID,dwKickTime)==TRUE)
	{
		//时效判断
		DWORD dwCurrTime = (DWORD)time(NULL);
		if(dwKickTime+VALID_TIME_KICK_BY_MANAGER > dwCurrTime)
		{
			//发送失败
			SendLogonFailure(TEXT("您已被管理员请出房间,1小时之内不能进入！"),FAILURE_TYPE_ADMIN_KICK_OUT,pBindParameter->dwSocketID);

			//解除锁定
			PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
			//解锁玩家
			DBR_GR_Deblocking Deblocking;
			Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
			Deblocking.dwServerID=m_pGameServiceOption->wServerID;

			//投递请求
			m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

			return true;
		}
		else
		{
			//移除玩家
			m_KickUserItemMap.RemoveKey(pDBOLogonSuccess->dwUserID);
		}
	}

	//切换判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		//切换用户
		SwitchUserItemConnect(pIServerUserItem,pDBOLogonSuccess->szMachineID,wBindIndex,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_USER_IMPACT);

		return true;
	}

	//维护判断
	if ((CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule)==true)&&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//发送失败
		SendLogonFailure(TEXT("抱歉，由于系统维护的原因，当前游戏房间禁止用户进入！"),FAILURE_TYPE_SYSTEM_MAINTENANCE,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SYSTEM);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//查找机器
	if (bAndroidUser==true && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
	{
		//查找机器
		DWORD dwUserID=pDBOLogonSuccess->dwUserID;
		IAndroidUserItem * pIAndroidUserItem=m_AndroidUserManager.SearchAndroidUserItem(dwUserID,dwContextID);

		//修改积分
		if (pIAndroidUserItem!=NULL)
		{
			//获取配置
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//调整积分
			if ((pAndroidParameter->lTakeMinScore!=0L)&&(pAndroidParameter->lTakeMaxScore!=0L))
			{
				//变量定义
				SCORE lTakeMinScore=(SCORE)pAndroidParameter->lTakeMinScore;
				SCORE lTakeMaxScore=(SCORE)__max(pAndroidParameter->lTakeMaxScore,pAndroidParameter->lTakeMinScore);

				//调整积分
				if ((lTakeMaxScore-lTakeMinScore)>0.0f)
				{
					SCORE lTakeScore = __max((lTakeMaxScore-lTakeMinScore)/10,1);
					//pDBOLogonSuccess->lScore=(SCORE)(lTakeMinScore+(rand()%10)*lTakeScore+rand()%lTakeScore);
					pDBOLogonSuccess->lScore=(SCORE)(lTakeMinScore+(rand()%10)*lTakeScore+(rand()%1000)*lTakeScore/1000);//MXM
				}
				else
				{
					pDBOLogonSuccess->lScore=(SCORE)lTakeMaxScore;
				}
			}
		}
	}

	//最低分数
	if ((m_pGameServiceOption->lMinEnterScore!=0L)&&(pDBOLogonSuccess->lScore<m_pGameServiceOption->lMinEnterScore))
	{
		//发送失败
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("进入该房间需要%.2f金币！"), m_pGameServiceOption->lMinEnterScore);
		SendLogonFailure(szMsg,FAILURE_TYPE_ROOM_COIN_NOTENOUGH,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//最高分数
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(pDBOLogonSuccess->lScore>m_pGameServiceOption->lMaxEnterScore))
	{
		//发送失败
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("进入该房间需要%.2f金币！"), m_pGameServiceOption->lMaxEnterScore);
		SendLogonFailure(szMsg,FAILURE_TYPE_ROOM_COIN_NOTENOUGH,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//会员判断
	if(m_pGameServiceOption->cbMinEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder < m_pGameServiceOption->cbMinEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//发送失败
		SendLogonFailure(TEXT("抱歉，您的会员级别低于当前游戏房间的最低进入会员条件，不能进入当前游戏房间！"),FAILURE_TYPE_VIP_LIMIT_MIN,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//会员判断
	if(m_pGameServiceOption->cbMaxEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder > m_pGameServiceOption->cbMaxEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//发送失败
		SendLogonFailure(TEXT("抱歉，您的会员级别高于当前游戏房间的最高进入会员条件，不能进入当前游戏房间！"),FAILURE_TYPE_VIP_LIMIT_MAX,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//满人判断
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	if ((pDBOLogonSuccess->cbMemberOrder==0)&&(pDBOLogonSuccess->cbMasterOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-RESERVE_USER_COUNT)))
	{
		//发送失败
		SendLogonFailure(TEXT("抱歉，由于此房间已经人满，普通玩家不能继续进入了！"),FAILURE_TYPE_ROOM_FULL,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);
		//解锁玩家
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//用户变量
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//属性资料
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID=pDBOLogonSuccess->dwGameID;
	UserInfo.dwGroupID=pDBOLogonSuccess->dwGroupID;
	UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));

	//IP归属地
	lstrcpyn(UserInfo.szChangeLogonIP, pDBOLogonSuccess->szChangeLogonIP, CountArray(UserInfo.szChangeLogonIP));

	//用户资料
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogonSuccess->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogonSuccess->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//附属资料
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));

	//状态设置
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//积分信息
	UserInfo.lScore=pDBOLogonSuccess->lScore;
	UserInfo.lGrade=pDBOLogonSuccess->lGrade;
	UserInfo.lIngot=pDBOLogonSuccess->lIngot;
	UserInfo.lInsure=pDBOLogonSuccess->lInsure;
	UserInfo.dBeans=pDBOLogonSuccess->dBeans;
	UserInfo.dwWinCount=pDBOLogonSuccess->dwWinCount;
	UserInfo.dwLostCount=pDBOLogonSuccess->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogonSuccess->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogonSuccess->dwFleeCount;	
	UserInfo.dwExperience=pDBOLogonSuccess->dwExperience;
	UserInfo.lLoveLiness=pDBOLogonSuccess->lLoveLiness;
	UserInfo.lIntegralCount=pDBOLogonSuccess->lIntegralCount;

	//代理信息
	UserInfo.dwAgentID=pDBOLogonSuccess->dwAgentID;

	//登录信息
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

	//用户权限
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogonSuccess->dwMasterRight;

	//辅助变量
	UserInfoPlus.bMobileUser=bMobileUser;
	UserInfoPlus.bAndroidUser=bAndroidUser;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfoPlus.szPassword));
	lstrcpyn(UserInfoPlus.szUserGameData,pDBOLogonSuccess->szUserGameData,CountArray(UserInfoPlus.szUserGameData));

	//连接信息
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//激活用户
	m_ServerUserManager.InsertUserItem(&pIServerUserItem,UserInfo,UserInfoPlus);

	//用户任务
	if(pDBOLogonSuccess->wTaskCount>0)
	{
		m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),pDBOLogonSuccess->UserTaskInfo,pDBOLogonSuccess->wTaskCount);
	}

	//错误判断
	if (pIServerUserItem==NULL)
	{
		//错误断言
		ASSERT(FALSE);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		//断开用户
		if (bAndroidUser==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(dwContextID,true);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
		}

		return true;
	}

	//设置用户
	pBindParameter->pIServerUserItem=pIServerUserItem;

	//修改参数
	if(pIServerUserItem->IsMobileUser())
	{
		SetMobileUserParameter(pIServerUserItem,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);
	}

	//报名状态
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		pIServerUserItem->SetUserMatchStatus(CUserRight::IsGameMatchUser(pIServerUserItem->GetUserRight())?MUS_SIGNUPED:MUS_NULL);
	}

	//登录事件
	OnEventUserLogon(pIServerUserItem,false);

	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_UserEnter UserEnter;
		ZeroMemory(&UserEnter,sizeof(UserEnter));

		//设置变量
		UserEnter.dwUserID=pIServerUserItem->GetUserID();
		UserEnter.dwGameID=pIServerUserItem->GetGameID();
		lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

		//辅助信息
		UserEnter.cbGender=pIServerUserItem->GetGender();
		UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
		UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();
		UserEnter.cbAndroidUser = bAndroidUser;

		//用户详细信息
		tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
		if(pUserInfo != NULL) memcpy(&UserEnter.userInfo, pUserInfo, sizeof(tagUserInfo));

		//发送消息
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
	}
	
	//检测道具Buff
	DWORD UserID = pDBOLogonSuccess->dwUserID;
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GAME_BUFF,dwContextID,(void*)&UserID,sizeof(UserID));

	//背包喇叭
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_TRUMPET,dwContextID,(void*)&UserID,sizeof(UserID));

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem!=NULL)) return true;

	//发送错误
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	//断开连接
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID,true);
	}
	else
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//配置信息
bool CAttemperEngineSink::OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBO_GR_GameParameter));
	if (wDataSize!=sizeof(DBO_GR_GameParameter)) return false;

	//变量定义
	DBO_GR_GameParameter * pGameParameter=(DBO_GR_GameParameter *)pData;

	//汇率信息
	m_pGameParameter->dwMedalRate=pGameParameter->dwMedalRate;
	m_pGameParameter->dwRevenueRate=pGameParameter->dwRevenueRate;
	m_pGameParameter->dwExchangeRate=pGameParameter->dwExchangeRate;	
	m_pGameParameter->dwPresentExchangeRate=pGameParameter->dwPresentExchangeRate;
	m_pGameParameter->dwRateGold=pGameParameter->dwRateGold;
	m_pGameParameter->lEducateGrantScore=pGameParameter->lEducateGrantScore;
	m_pGameParameter->lHallNoticeThreshold=pGameParameter->lHallNoticeThreshold;
	//经验奖励
	m_pGameParameter->dwWinExperience=pGameParameter->dwWinExperience;	

	//版本信息
	m_pGameParameter->dwClientVersion=pGameParameter->dwClientVersion;
	m_pGameParameter->dwServerVersion=pGameParameter->dwServerVersion;

	//版本效验
	if (VERSION_EFFICACY==TRUE)
	{
		//版本判断
		bool bVersionInvalid=false;
		if (m_pGameParameter->dwClientVersion!=m_pGameServiceAttrib->dwClientVersion) bVersionInvalid=true;
		if (m_pGameParameter->dwServerVersion!=m_pGameServiceAttrib->dwServerVersion) bVersionInvalid=true;

		//提示信息
		if (bVersionInvalid==true)
		{
			CTraceService::TraceString(TEXT("平台数据库服务组件版本注册信息与当前组件版本信息不一致"),TraceLevel_Warning);
		}
	}

	return true;
}

//列表信息
bool CAttemperEngineSink::OnDBGameColumnInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameColumnInfo * pGameColumnInfo=(DBO_GR_GameColumnInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameColumnInfo)-sizeof(pGameColumnInfo->ColumnItemInfo);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0])))) return false;

	//数据处理
	if (pGameColumnInfo->cbColumnCount==0)
	{
		//默认列表
	}
	else
	{
		//拷贝数据
		m_DataConfigColumn.cbColumnCount=pGameColumnInfo->cbColumnCount;
		CopyMemory(m_DataConfigColumn.ColumnItem,pGameColumnInfo->ColumnItemInfo,pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]));
	}

	return true;
}

//机器信息
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidAccountsInfo);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0])))) return false;

	//设置机器
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.InsertAndroidInfo(pGameAndroidInfo->AndroidAccountsInfo,pGameAndroidInfo->wAndroidCount,pGameAndroidInfo->dwBatchID);
	}

	return true;
}

//机器参数
bool CAttemperEngineSink::OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameAndroidParameter * pGameAndroidParameter=(DBO_GR_GameAndroidParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidParameter)-sizeof(pGameAndroidParameter->AndroidParameter);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0])))) return false;

	//设置机器
	if (pGameAndroidParameter->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.AddAndroidParameter(pGameAndroidParameter->AndroidParameter,pGameAndroidParameter->wParameterCount);
	}

	return true;
}


bool CAttemperEngineSink::OnDBGamePropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_UserGamePropertyBuff * pGamePropertyBuff=(DBO_GR_UserGamePropertyBuff *)pData;
	WORD wHeadSize=sizeof(DBO_GR_UserGamePropertyBuff)-sizeof(pGamePropertyBuff->PropertyBuff);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGamePropertyBuff->cbBuffCount*sizeof(pGamePropertyBuff->PropertyBuff[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGamePropertyBuff->cbBuffCount*sizeof(pGamePropertyBuff->PropertyBuff[0])))) return false;
	
	//设置管理
	m_GamePropertyManager.ClearUserBuff(pGamePropertyBuff->dwUserID);
	m_GamePropertyManager.SetGamePropertyBuff(pGamePropertyBuff->dwUserID, pGamePropertyBuff->PropertyBuff,pGamePropertyBuff->cbBuffCount);

	//拷贝数据
	CMD_GR_S_PropertyBuff PropertyBuffResult;
	PropertyBuffResult.dwUserID = pGamePropertyBuff->dwUserID;
	PropertyBuffResult.cbBuffCount = pGamePropertyBuff->cbBuffCount;
	memcpy(PropertyBuffResult.PropertyBuff, pGamePropertyBuff->PropertyBuff, sizeof(tagPropertyBuff)*pGamePropertyBuff->cbBuffCount);

	WORD dwHeadSize=sizeof(PropertyBuffResult)-sizeof(PropertyBuffResult.PropertyBuff);
	WORD dwDataSize=PropertyBuffResult.cbBuffCount*sizeof(PropertyBuffResult.PropertyBuff[0]);

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_USER_PROP_BUFF,&PropertyBuffResult,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBGameLoadTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_UserGameTrumpet * pUserGameTrumpet=(DBO_GR_UserGameTrumpet *)pData;
	
	CMD_GR_S_UserTrumpet UserTrumpet;
	UserTrumpet.dwTrumpetCount = pUserGameTrumpet->dwTrumpetCount;
	UserTrumpet.dwTyphonCount = pUserGameTrumpet->dwTyphonCount;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_USER_TRUMPET,&UserTrumpet,sizeof(UserTrumpet));
	return true;
}

bool CAttemperEngineSink::OnDBGameSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_Send_Trumpet* pSendTrumpetResult = (DBO_GR_Send_Trumpet*)pData;
	if(pSendTrumpetResult->bSuccful)
	{
		//道具索引
		BYTE cbIndex=(pSendTrumpetResult->dwPropID==PROPERTY_ID_TRUMPET)?2:3;

		//构造结构
		CMD_GR_S_SendTrumpet  SendTrumpet;
		SendTrumpet.dwSendUserID=pSendTrumpetResult->dwUserID;
		SendTrumpet.wPropertyIndex=(WORD)pSendTrumpetResult->dwPropID;
		SendTrumpet.TrumpetColor=pSendTrumpetResult->TrumpetColor;
		ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
		CopyMemory(SendTrumpet.szSendNickName,pSendTrumpetResult->szSendNickName,sizeof(SendTrumpet.szSendNickName));

		//字符过滤
		SensitiveWordFilter(pSendTrumpetResult->szTrumpetContent,SendTrumpet.szTrumpetContent,CountArray(SendTrumpet.szTrumpetContent));

		//房间转发
		if(cbIndex==3)
		{
			//广播房间
			if(m_pITCPSocketService)
			{
				m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet));
			}
		}

		//游戏转发
		if(cbIndex==2)
		{
			//发送数据
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_PROPERTY,SUB_GR_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet),BG_COMPUTER);
		}
	}
	return true;
}

//会员参数
bool CAttemperEngineSink::OnDBGameMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_MemberParameter * pMemberParameter=(DBO_GR_MemberParameter *)pData;

	//拷贝数据	
	m_wMemberCount = pMemberParameter->wMemberCount;
	CopyMemory(m_MemberParameter,pMemberParameter->MemberParameter,sizeof(tagMemberParameterNew)*m_wMemberCount);

	return true;
}

//会员查询
bool CAttemperEngineSink::OnDBPCMemberDayQueryInfoResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_MemberQueryInfoResult * pMemberResult=(DBO_GR_MemberQueryInfoResult *)pData;

	//构造结构
	CMD_GR_MemberQueryInfoResult MemberResult;
	MemberResult.bPresent=pMemberResult->bPresent;
	MemberResult.bGift=pMemberResult->bGift;
	//拷贝数据	
	MemberResult.GiftSubCount = pMemberResult->GiftSubCount;
	CopyMemory(MemberResult.GiftSub,pMemberResult->GiftSub,sizeof(tagGiftPropertyInfo)*MemberResult.GiftSubCount);

	//发送数据
	WORD wSendDataSize=sizeof(MemberResult);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_QUERY_INFO_RESULT,&MemberResult,wSendDataSize);

	return true;
}

//会员送金
bool CAttemperEngineSink::OnDBPCMemberDayPresentResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_MemberDayPresentResult * pMemberResult=(DBO_GR_MemberDayPresentResult *)pData;

	//更新游戏币
	if( pMemberResult->bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(pMemberResult->lGameScore-pIServerUserItem->GetUserScore(),0,0);
	}

	//构造结构
	CMD_GR_MemberDayPresentResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	MemberResult.lGameScore=pMemberResult->lGameScore;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_DAY_PRESENT_RESULT,&MemberResult,wSendDataSize);

	return true;

}

//会员礼包
bool CAttemperEngineSink::OnDBPCMemberDayGiftResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_MemberDayGiftResult * pMemberResult=(DBO_GR_MemberDayGiftResult *)pData;

	//构造结构
	CMD_GR_MemberDayGiftResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_DAY_GIFT_RESULT,&MemberResult,wSendDataSize);
	return true;
}

//银行信息
bool CAttemperEngineSink::OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserInsureInfo * pUserInsureInfo=(DBO_GR_UserInsureInfo *)pData;

	//变量定义
	CMD_GR_S_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//构造数据
	UserInsureInfo.cbActivityGame=pUserInsureInfo->cbActivityGame;
	UserInsureInfo.cbEnjoinTransfer=pUserInsureInfo->cbEnjoinTransfer;
	UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
	UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
	UserInsureInfo.wRevenueTransferMember=pUserInsureInfo->wRevenueTransferMember;
	UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
	UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
	UserInsureInfo.lUserScore+=pIServerUserItem->GetUserScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetTrusteeScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetFrozenedScore();
	UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	return true;
}

//银行成功
bool CAttemperEngineSink::OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserInsureSuccess * pUserInsureSuccess=(DBO_GR_UserInsureSuccess *)pData;

	//变量定义
	SCORE lFrozenedScore=pUserInsureSuccess->lFrozenedScore;
	SCORE lInsureRevenue=pUserInsureSuccess->lInsureRevenue;
	SCORE lVariationScore=pUserInsureSuccess->lVariationScore;
	SCORE lVariationInsure=pUserInsureSuccess->lVariationInsure;

	//解冻积分
	if ((lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//银行操作
	if (pIServerUserItem->ModifyUserInsure(lVariationScore,lVariationInsure,lInsureRevenue)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	CMD_GR_S_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//构造变量
	UserInsureSuccess.cbActivityGame=pUserInsureSuccess->cbActivityGame;
	UserInsureSuccess.lUserScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//发送数据
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送错误
	DBO_GR_UserInsureFailure * pUserInsureFailure=(DBO_GR_UserInsureFailure *)pData;
	SendInsureFailure(pIServerUserItem,pUserInsureFailure->szDescribeString,pUserInsureFailure->lResultCode,pUserInsureFailure->cbActivityGame);

	//解冻积分
	if ((pUserInsureFailure->lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(pUserInsureFailure->lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//用户信息
bool CAttemperEngineSink::OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserTransferUserInfo * pTransferUserInfo=(DBO_GR_UserTransferUserInfo *)pData;

	//变量定义
	CMD_GR_S_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//构造变量
	UserTransferUserInfo.cbActivityGame=pTransferUserInfo->cbActivityGame;
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szAccounts,pTransferUserInfo->szAccounts,CountArray(UserTransferUserInfo.szAccounts));

	//发送数据
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_TRANSFER_USER_INFO,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

//开通结果
bool CAttemperEngineSink::OnDBUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserInsureEnableResult * pUserInsureEnableResult=(DBO_GR_UserInsureEnableResult *)pData;

	//变量定义
	CMD_GR_S_UserInsureEnableResult UserInsureEnableResult;
	ZeroMemory(&UserInsureEnableResult,sizeof(UserInsureEnableResult));

	//构造变量
	UserInsureEnableResult.cbInsureEnabled=pUserInsureEnableResult->cbInsureEnabled;
	lstrcpyn(UserInsureEnableResult.szDescribeString,pUserInsureEnableResult->szDescribeString,CountArray(UserInsureEnableResult.szDescribeString));

	//计算大小
	WORD wHeadSize=CountStringBuffer(UserInsureEnableResult.szDescribeString);
	wHeadSize+= sizeof(UserInsureEnableResult)-sizeof(UserInsureEnableResult.szDescribeString);

	//发送数据
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_ENABLE_RESULT,&UserInsureEnableResult,wHeadSize);

	return true;
}

//道具成功
bool CAttemperEngineSink::OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_S_PropertySuccess * pPropertySuccess=(DBO_GR_S_PropertySuccess *)pData;

	//获取用户
	IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwSourceUserID);
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwTargetUserID);

	//赠送用户
	if (pISourceUserItem!=NULL)
	{
		//变量定义
		SCORE lFrozenedScore=pPropertySuccess->lFrozenedScore;

		//解冻积分
		if ((lFrozenedScore>0L)&&(pISourceUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//更新魅力
	if (pISourceUserItem!=NULL && pPropertySuccess->lSendLoveLiness != 0)
	{
		pISourceUserItem->ModifyUserProperty(0,(LONG)pPropertySuccess->lSendLoveLiness);
	}

	//更新魅力
	if(pITargetUserItem!=NULL && pPropertySuccess->lRecvLoveLiness != 0)
	{
		pITargetUserItem->ModifyUserProperty(0,(LONG)pPropertySuccess->lRecvLoveLiness);
	}

	//变量定义
	DWORD dwCurrentTime=(DWORD)time(NULL);


	//负分清零
	if( pPropertySuccess->wPropKind == PROP_KIND_NEGATIVE_SCORE_CLEAR )
	{
		if( m_pGameServiceOption->wServerType&GAME_GENRE_SCORE )
		{
			//变量定义
			SCORE lCurrScore = pITargetUserItem->GetUserScore();
			if ( lCurrScore < 0)
			{
				//用户信息
				tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
				if(pUserInfo==NULL) return true;
				//修改积分
				pUserInfo->lScore=0;
				SendPropertyEffect(pITargetUserItem, PROP_KIND_NEGATIVE_SCORE_CLEAR);
			}
		}
		
	}
	//逃跑清零
	else if( pPropertySuccess->wPropKind == PROP_KIND_ESCAPE_CLEAR )
	{
		//用户信息
		tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
		if(pUserInfo==NULL) return true;

		//修改逃跑率
		if(pUserInfo->dwFleeCount > 0)
		{
			pUserInfo->dwFleeCount=0;
			SendPropertyEffect(pITargetUserItem, PROP_KIND_ESCAPE_CLEAR);
		}
	}
	//VIP卡
	else if( pPropertySuccess->wPropKind == PROP_KIND_VIP )
	{
		//用户信息
		tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
		if(pUserInfo==NULL) return true;

		//更新会员
		pUserInfo->cbMemberOrder=pPropertySuccess->cbMemberOrder;

		//修改权限
		pITargetUserItem->ModifyUserRight(pPropertySuccess->dwUserRight,0);

		//发送消息
		SendPropertyEffect(pITargetUserItem, PROP_KIND_VIP);
	}



	//广播礼物
	if( pPropertySuccess->wPropKind==PROP_KIND_GIFT )
	{
		//变量定义
		CMD_GR_S_PropertySuccess PropertySuccess;
		ZeroMemory(&PropertySuccess,sizeof(PropertySuccess));

		//设置变量
		PropertySuccess.wKind = pPropertySuccess->wPropKind;
		PropertySuccess.cbRequestArea=pPropertySuccess->cbRequestArea;
		PropertySuccess.wItemCount=pPropertySuccess->wItemCount;
		PropertySuccess.wPropertyIndex=pPropertySuccess->wPropertyIndex;
		PropertySuccess.dwSourceUserID=pPropertySuccess->dwSourceUserID;
		PropertySuccess.dwTargetUserID=pPropertySuccess->dwTargetUserID;

		//发送消息
		if (pISourceUserItem!=NULL)
		{
			SendData(pISourceUserItem,MDM_GR_PROPERTY,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
		}

		//发送消息
		if (pITargetUserItem!=NULL && pITargetUserItem!=pISourceUserItem)
		{
			SendData(pITargetUserItem,MDM_GR_PROPERTY,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
		}

		SendPropertyMessage(PropertySuccess.dwSourceUserID,PropertySuccess.dwTargetUserID,PropertySuccess.wPropertyIndex,
		PropertySuccess.wItemCount);
	}

	return true;
}

//道具失败
bool CAttemperEngineSink::OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_PropertyFailure));
	if(wDataSize!=sizeof(DBO_GR_PropertyFailure)) return false;

	//提取数据
	DBO_GR_PropertyFailure * pPropertyFailure = (DBO_GR_PropertyFailure *)pData;
	if(pPropertyFailure==NULL) return false;

	//构造结构
	CMD_GR_GamePropertyFailure PropertyFailure;
	PropertyFailure.lErrorCode = pPropertyFailure->lErrorCode;
	lstrcpyn(PropertyFailure.szDescribeString,pPropertyFailure->szDescribeString,CountArray(PropertyFailure.szDescribeString));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_GAME_PROPERTY_FAILURE,&PropertyFailure,sizeof(PropertyFailure));

	return true;
}

//道具购买结果
bool CAttemperEngineSink::OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_PropertyBuyResult * pPropertyBuyResult=(DBO_GR_PropertyBuyResult *)pData;

	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();

	//构造结构
	CMD_GR_PropertyBuyResult PropertyBuyResult;
	PropertyBuyResult.dwUserID = pPropertyBuyResult->dwUserID;
	PropertyBuyResult.dwPropertyID = pPropertyBuyResult->dwPropertyID;
	PropertyBuyResult.dwItemCount = pPropertyBuyResult->dwItemCount;
	PropertyBuyResult.lInsureScore = pPropertyBuyResult->lInsureScore;
	PropertyBuyResult.lUserMedal = pPropertyBuyResult->lUserMedal;
	PropertyBuyResult.lLoveLiness = pPropertyBuyResult->lLoveLiness;
	PropertyBuyResult.dCash = pPropertyBuyResult->dCash;
	PropertyBuyResult.cbCurrMemberOrder = pPropertyBuyResult->cbCurrMemberOrder;
	lstrcpyn(PropertyBuyResult.szNotifyContent,pPropertyBuyResult->szNotifyContent,CountArray(PropertyBuyResult.szNotifyContent));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY, SUB_GR_GAME_PROPERTY_BUY_RESULT,&PropertyBuyResult,sizeof(PropertyBuyResult));

	pIServerUserItem->ModifyUserInsure(0,pPropertyBuyResult->lInsureScore-pIServerUserItem->GetUserInsure(),0);

	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_QueryBackpack* pQueryBackpackResult = (DBO_GR_QueryBackpack*)pData;

	//构造返回
	BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
	CMD_GR_S_BackpackProperty* pBackpackPropertyResult = (CMD_GR_S_BackpackProperty*)cbDataBuffer;

	//初始化参数
	WORD dwDataBufferSize = sizeof(CMD_GR_S_BackpackProperty);
	pBackpackPropertyResult->dwUserID = pQueryBackpackResult->dwUserID;
	pBackpackPropertyResult->dwCount = pQueryBackpackResult->dwCount;
	pBackpackPropertyResult->dwStatus = pQueryBackpackResult->dwStatus;
	if(pQueryBackpackResult->dwCount > 0)
	{
		memcpy(pBackpackPropertyResult->PropertyInfo, pQueryBackpackResult->PropertyInfo, sizeof(tagBackpackProperty)*(pQueryBackpackResult->dwCount));
		dwDataBufferSize += sizeof(tagBackpackProperty)*(WORD)(pQueryBackpackResult->dwCount-1);
	}
	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_PROPERTY_BACKPACK_RESULT,pBackpackPropertyResult,dwDataBufferSize);
	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_PropertyUse* pPropertyUseResult = (DBO_GR_PropertyUse*)pData;

	//构造结构
	CMD_GR_S_PropertyUse PropertyUseResult;

	PropertyUseResult.dwUserID = pPropertyUseResult->dwUserID;
	PropertyUseResult.dwRecvUserID = pPropertyUseResult->dwRecvUserID;
	PropertyUseResult.wUseArea = pPropertyUseResult->wUseArea;
	PropertyUseResult.wServiceArea = pPropertyUseResult->wServiceArea;
	PropertyUseResult.dwPropID = pPropertyUseResult->dwPropID;
	PropertyUseResult.dwPropKind = pPropertyUseResult->dwPropKind;
	PropertyUseResult.wPropCount = pPropertyUseResult->wPropCount;
	PropertyUseResult.Score = pPropertyUseResult->Score;
	PropertyUseResult.dwRemainderPropCount = pPropertyUseResult->dwRemainderPropCount;
	PropertyUseResult.lRecvLoveLiness = pPropertyUseResult->lRecvLoveLiness;
	PropertyUseResult.lSendLoveLiness = pPropertyUseResult->lSendLoveLiness;
	PropertyUseResult.lUseResultsGold = pPropertyUseResult->lUseResultsGold;
	PropertyUseResult.UseResultsValidTime = pPropertyUseResult->UseResultsValidTime;
	PropertyUseResult.tUseTime = pPropertyUseResult->tUseTime;
	PropertyUseResult.dwHandleCode = pPropertyUseResult->dwHandleCode;
	PropertyUseResult.cbMemberOrder = pPropertyUseResult->cbMemberOrder;
	lstrcpyn(PropertyUseResult.szName, pPropertyUseResult->szName,CountArray(PropertyUseResult.szName));
	lstrcpyn(PropertyUseResult.szNotifyContent, pPropertyUseResult->szNotifyContent,CountArray(PropertyUseResult.szNotifyContent));

	WORD wSendSize=sizeof(PropertyUseResult)-sizeof(PropertyUseResult.szNotifyContent)+CountStringBuffer(PropertyUseResult.szNotifyContent);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_PROPERTY_USE_RESULT,&PropertyUseResult,wSendSize);
	if( m_pGameServiceOption->wServerType&GAME_GENRE_GOLD )
	{
		//更新金币
		IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertyUseResult->dwUserID);
		if(pISourceUserItem) pISourceUserItem->ModifyUserInsure(pPropertyUseResult->lUseResultsGold, 0, 0);
	}

	//判断时长
	if(pPropertyUseResult->UseResultsValidTime > 0)
	{
		//加载Buff
		DWORD UserID = pPropertyUseResult->dwUserID;
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GAME_BUFF,dwContextID,(void*)&UserID,sizeof(UserID));
	}

	//调用结果
	DBO_GR_S_PropertySuccess PropertySuccess={0};
	PropertySuccess.wItemCount = (WORD)pPropertyUseResult->wPropCount;
	PropertySuccess.wPropertyIndex = (WORD)pPropertyUseResult->dwPropID;
	PropertySuccess.dwSourceUserID = pPropertyUseResult->dwUserID;
	PropertySuccess.dwTargetUserID = pPropertyUseResult->dwRecvUserID;
	PropertySuccess.wPropKind = (WORD)pPropertyUseResult->dwPropKind;
	PropertySuccess.cbRequestArea = (BYTE)pPropertyUseResult->wServiceArea; //消费方式
	PropertySuccess.wServiceArea = pPropertyUseResult->wServiceArea;
	PropertySuccess.lSendLoveLiness = pPropertyUseResult->lSendLoveLiness;
	PropertySuccess.lRecvLoveLiness = pPropertyUseResult->lRecvLoveLiness;
	OnDBPropertySuccess(dwContextID, &PropertySuccess, sizeof(PropertySuccess));
	
	return true;
}

bool CAttemperEngineSink::OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//构造结构
	DBO_GR_QuerySendPresent* pQuerySendPresent = (DBO_GR_QuerySendPresent*)pData;
	CMD_GR_S_QuerySendPresent QuerySendPresentResult={0};
	QuerySendPresentResult.wPresentCount = pQuerySendPresent->wPresentCount;
	memcpy(QuerySendPresentResult.Present, pQuerySendPresent->Present, sizeof(pQuerySendPresent->wPresentCount*sizeof(pQuerySendPresent->Present[0])));

	WORD dwHeadSize=sizeof(QuerySendPresentResult)-sizeof(QuerySendPresentResult.Present);
	WORD dwDataSize=QuerySendPresentResult.wPresentCount*sizeof(QuerySendPresentResult.Present[0]);;
	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_QUERY_SEND_PRESENT_RESULT,&QuerySendPresentResult,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_PropertyPresent* pPropertyPresent = (DBO_GR_PropertyPresent*)pData;

	//构造结构
	CMD_GR_S_PropertyPresent PropertyPresentResult={0};
	PropertyPresentResult.dwUserID = pPropertyPresent->dwUserID;
	PropertyPresentResult.dwRecvGameID = pPropertyPresent->dwRecvGameID;
	PropertyPresentResult.dwPropID = pPropertyPresent->dwPropID;
	PropertyPresentResult.wPropCount = pPropertyPresent->wPropCount;
	PropertyPresentResult.wType = pPropertyPresent->wType;
	PropertyPresentResult.nHandleCode = pPropertyPresent->nHandleCode;

	lstrcpyn(PropertyPresentResult.szRecvNickName, pPropertyPresent->szRecvNickName,CountArray(PropertyPresentResult.szRecvNickName));
	lstrcpyn(PropertyPresentResult.szNotifyContent, pPropertyPresent->szNotifyContent,CountArray(PropertyPresentResult.szNotifyContent));
	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_PROPERTY_PRESENT_RESULT,&PropertyPresentResult,sizeof(PropertyPresentResult));
	
	//查找用户
	IServerUserItem* pServerUserItem =  m_ServerUserManager.SearchUserItem(pPropertyPresent->dwRecvUserID);
	if(pServerUserItem != NULL)
	{
		WORD wBindIndex=pServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
		//构造结构
		DBR_GR_QuerySendPresent QuerySendPresentRequest={0};
		QuerySendPresentRequest.dwUserID = pPropertyPresent->dwRecvUserID;
		QuerySendPresentRequest.dwClientAddr=pServerUserItem->GetClientAddr();
		//发送数据
		m_pIDBCorrespondManager->PostDataBaseRequest(pServerUserItem->GetUserID(),DBR_GR_QUERY_SEND_PRESENT,pBindParameter->dwSocketID,&QuerySendPresentRequest,sizeof(QuerySendPresentRequest));
	}
	
	return true;
}

bool CAttemperEngineSink::OnDBQueryGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//构造结构
	DBO_GR_GetSendPresent* pGetSendPresent = (DBO_GR_GetSendPresent*)pData;
	CMD_GR_S_GetSendPresent GetSendPresentResult={0};
	GetSendPresentResult.wPresentCount = pGetSendPresent->wPresentCount;
	memcpy(GetSendPresentResult.Present, pGetSendPresent->Present, pGetSendPresent->wPresentCount*sizeof(SendPresent));

	WORD dwHeadSize=sizeof(GetSendPresentResult)-sizeof(GetSendPresentResult.Present);
	WORD dwDataSize=GetSendPresentResult.wPresentCount*sizeof(GetSendPresentResult.Present[0]);
	
	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_GET_SEND_PRESENT_RESULT,&GetSendPresentResult,dwHeadSize+dwDataSize);

	return true;
}

//购买结果
bool CAttemperEngineSink::OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//提取数据
	DBO_GR_PurchaseResult * pPurchaseResult=(DBO_GR_PurchaseResult *)pData;

	//构造结构
	CMD_GR_PurchaseResult PurchaseResult;
	ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

	//设置变量
	PurchaseResult.bSuccessed=pPurchaseResult->bSuccessed;
	PurchaseResult.lCurrScore=pPurchaseResult->lCurrScore;
	PurchaseResult.dCurrBeans=pPurchaseResult->dCurrBeans;
	PurchaseResult.cbMemberOrder=pPurchaseResult->cbMemberOrder;
	lstrcpyn(PurchaseResult.szNotifyContent,pPurchaseResult->szNotifyContent,CountArray(PurchaseResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_PURCHASE_RESULT,&PurchaseResult,wSendDataSize);

	//结果判断
	if(PurchaseResult.bSuccessed==true)
	{
		//修改会员等级
		tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->cbMemberOrder=pPurchaseResult->cbMemberOrder;
		pUserInfo->dBeans=pPurchaseResult->dCurrBeans;
		SendUserInfoPacket(pIServerUserItem,dwContextID);

		//设置权限
		pIServerUserItem->ModifyUserRight(pPurchaseResult->dwUserRight,0,UR_KIND_SERVER);		

		//更新游戏币
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
		{
			SCORE lOldScore=pIServerUserItem->GetUserScore();
			lOldScore+=pIServerUserItem->GetFrozenedScore();
			lOldScore+=pIServerUserItem->GetTrusteeScore();
			pIServerUserItem->ModifyUserInsure(PurchaseResult.lCurrScore-lOldScore,0,0);
		}
	}

	return true;
}

//兑换结果
bool CAttemperEngineSink::OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//提取数据
	DBO_GR_ExchangeResult * pExchangeResult=(DBO_GR_ExchangeResult *)pData;

	//构造结构
	CMD_GR_ExchangeResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//设置变量
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lCurrIngot=pExchangeResult->lCurrIngot;
	ExchangeResult.dCurrBeans=pExchangeResult->dCurrBeans;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_RESULT,&ExchangeResult,wSendDataSize);

	//更新元宝
	if (ExchangeResult.bSuccessed==true)
	{
		//修改信息
		tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->lIngot=pExchangeResult->lCurrIngot;
		pUserInfo->dBeans=pExchangeResult->dCurrBeans;
	}

	//更新游戏币
	if(/*m_pGameServiceOption->wServerType==GAME_GENRE_GOLD && */ExchangeResult.bSuccessed==true)
	{
		SCORE lCurrScore=pIServerUserItem->GetUserScore();
		SCORE lVarInsureScore=pExchangeResult->lCurrScore-lCurrScore;
		pIServerUserItem->ModifyUserInsure(lVarInsureScore,0,0);
	}

	return true;
}

//签到信息
bool CAttemperEngineSink::OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//变量定义
	DBO_GR_CheckInInfo * pCheckInInfo=(DBO_GR_CheckInInfo *)pData;

	//变量定义
	CMD_GR_CheckInInfo CheckInInfo;
	ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

	//构造变量
	CheckInInfo.bTodayChecked=pCheckInInfo->bTodayChecked;
	CheckInInfo.wSeriesDate=pCheckInInfo->wSeriesDate;
	CopyMemory(CheckInInfo.lRewardGold,m_lCheckInReward,sizeof(CheckInInfo.lRewardGold));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_CHECKIN,SUB_GR_CHECKIN_INFO,&CheckInInfo,sizeof(CheckInInfo));

	return true;
}

//签到结果
bool CAttemperEngineSink::OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_CheckInResult * pCheckInResult=(DBO_GR_CheckInResult *)pData;

	//变量定义
	CMD_GR_CheckInResult CheckInResult;
	ZeroMemory(&CheckInResult,sizeof(CheckInResult));

	//构造变量
	CheckInResult.bSuccessed=pCheckInResult->bSuccessed;
	CheckInResult.lScore=pCheckInResult->lScore;
	lstrcpyn(CheckInResult.szNotifyContent,pCheckInResult->szNotifyContent,CountArray(CheckInResult.szNotifyContent));

	//更新游戏币
	if( CheckInResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(CheckInResult.lScore-pIServerUserItem->GetUserScore(),0,0);
	}


	//发送数据
	WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_CHECKIN,SUB_GR_CHECKIN_RESULT,&CheckInResult,wSendSize);

	return true;
}
//加载签到
bool CAttemperEngineSink::OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_CheckInReward));
	if(wDataSize!=sizeof(DBO_GR_CheckInReward)) return false;

	//提取数据
	DBO_GR_CheckInReward * pCheckInReward=(DBO_GR_CheckInReward *)pData;

	//拷贝数据
	CopyMemory(m_lCheckInReward,pCheckInReward->lRewardGold,sizeof(m_lCheckInReward));

	return true;
}

//低保参数
bool CAttemperEngineSink::OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_BaseEnsureParameter * pEnsureParameter=(DBO_GR_BaseEnsureParameter *)pData;

	//设置变量
	m_BaseEnsureParameter.cbTakeTimes = pEnsureParameter->cbTakeTimes;
	m_BaseEnsureParameter.lScoreAmount = pEnsureParameter->lScoreAmount;
	m_BaseEnsureParameter.lScoreCondition = pEnsureParameter->lScoreCondition;

	//获取状态
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	enServiceStatus ServiceStatus=pServiceUnits->GetServiceStatus();
	//事件通知
	if (ServiceStatus!=ServiceStatus_Service)
	{
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}

	return true;
}

//低保结果
bool CAttemperEngineSink::OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_BaseEnsureResult * pBaseEnsureResult=(DBO_GR_BaseEnsureResult *)pData;

	//构造结构
	CMD_GR_BaseEnsureResult BaseEnsureResult;
	BaseEnsureResult.bSuccessed=pBaseEnsureResult->bSuccessed;
	BaseEnsureResult.lGameScore=pBaseEnsureResult->lGameScore;
	lstrcpyn(BaseEnsureResult.szNotifyContent,pBaseEnsureResult->szNotifyContent,CountArray(BaseEnsureResult.szNotifyContent));

	//更新游戏币
	if( BaseEnsureResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(BaseEnsureResult.lGameScore-pIServerUserItem->GetUserScore(),0,0);
	}

	//发送数据
	WORD wSendDataSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
	wSendDataSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_BASEENSURE,SUB_GR_BASEENSURE_RESULT,&BaseEnsureResult,wSendDataSize);

	return true;
}

//任务列表
bool CAttemperEngineSink::OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_TaskListInfo * pTaskListInfo=(DBO_GR_TaskListInfo *)pData;

	//拷贝数据	
	WORD wTaskCount = __min(pTaskListInfo->wTaskCount, CountArray(m_TaskParameter)-m_wTaskCount);
	CopyMemory(m_TaskParameter+m_wTaskCount, ++pTaskListInfo, sizeof(tagTaskParameter)*wTaskCount);

	//设置变量
	m_wTaskCount += wTaskCount;

	return true;
}

//任务结束
bool CAttemperEngineSink::OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//移除任务
	m_UserTaskManager.RemoveTaskParameter();

	//添加任务
	m_UserTaskManager.AddTaskParameter(m_TaskParameter, m_wTaskCount);

	return true;
}

//任务信息
bool CAttemperEngineSink::OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskInfo));
	if (wDataSize>sizeof(DBO_GR_TaskInfo)) return false;

	//变量定义
	DBO_GR_TaskInfo * pTaskInfo=(DBO_GR_TaskInfo *)pData;

	//网络数据
	WORD wTaskCount=m_wTaskCount;
	WORD wSendSize=0,wParameterSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	LPBYTE pDataBuffer = cbDataBuffer;

	//设置数量
	pDataBuffer += sizeof(wTaskCount);
	wSendSize = sizeof(wTaskCount);

	//分包发送
	for(WORD i=0;i<wTaskCount;i++)
	{
		//越界判断
		if(wSendSize+sizeof(tagTaskParameter)>SOCKET_TCP_PACKET)
		{
			//重置变量
			pDataBuffer = cbDataBuffer;
			wTaskCount -= i;
			*(WORD*)pDataBuffer = i; 

			//发送数据
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,&cbDataBuffer,wSendSize);

			//重置变量
			pDataBuffer += sizeof(wSendSize);
			wSendSize = wSendSize;
		}

		//计算大小
		wParameterSize = sizeof(tagTaskParameter)-sizeof(m_TaskParameter[i].szTaskDescribe)+CountStringBuffer(m_TaskParameter[i].szTaskDescribe);
		*(WORD *)pDataBuffer = wParameterSize;
		pDataBuffer += sizeof(wParameterSize);
		wSendSize += sizeof(wParameterSize);

		//拷贝数据
		CopyMemory(pDataBuffer,&m_TaskParameter[i],wParameterSize);

		//推进指针
		pDataBuffer += wParameterSize;
		wSendSize += wParameterSize;
	}

	//剩余发送
	if(wTaskCount>0 && wSendSize)
	{
		//重置变量
		pDataBuffer = cbDataBuffer;
		*(WORD *)pDataBuffer = wTaskCount; 

		//发送数据
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);
	}

	//构造结构
	CMD_GR_S_TaskInfo TaskInfo;
	TaskInfo.wTaskCount = pTaskInfo->wTaskCount;
	CopyMemory(TaskInfo.TaskStatus,pTaskInfo->TaskStatus,sizeof(TaskInfo.TaskStatus[0])*pTaskInfo->wTaskCount);

	//发送数据
	WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
	wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_INFO,&TaskInfo,wSendDataSize);

	return true;
}

//任务结果
bool CAttemperEngineSink::OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskResult));
	if(wDataSize>sizeof(DBO_GR_TaskResult)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_TaskResult * pTaskResult=(DBO_GR_TaskResult *)pData;

	//变量定义
	CMD_GR_S_TaskResult TaskResult;
	ZeroMemory(&TaskResult,sizeof(TaskResult));

	//构造变量
	TaskResult.wCommandID=pTaskResult->wCommandID;
	TaskResult.bSuccessed=pTaskResult->bSuccessed;
	TaskResult.lCurrScore=pTaskResult->lCurrScore;
	TaskResult.lCurrIngot=pTaskResult->lCurrIngot;
	lstrcpyn(TaskResult.szNotifyContent,pTaskResult->szNotifyContent,CountArray(TaskResult.szNotifyContent));

	//发送数据
	WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_RESULT,&TaskResult,wSendSize);

	//奖励信息
	if (TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_REWARD)
	{
		//变量定义
		tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->lIngot=pTaskResult->lCurrIngot;

		SCORE lOldScore=pIServerUserItem->GetUserScore();
		lOldScore+=pIServerUserItem->GetFrozenedScore();
		lOldScore+=pIServerUserItem->GetTrusteeScore();
		pIServerUserItem->ModifyUserInsure(pTaskResult->lCurrScore-lOldScore,0,0);
	}

	//设置任务
	if(TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_TAKE)
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.SearchTaskParameter(pTaskResult->wCurrTaskID);
		if(pTaskParameter!=NULL && pTaskParameter->wKindID==m_pGameServiceOption->wKindID)
		{
			//变量定义
			tagUserTaskInfo UserTaskInfo;

			//设置任务		
			UserTaskInfo.wTaskProgress=0;
			UserTaskInfo.cbTaskStatus=TASK_STATUS_UNFINISH;
			UserTaskInfo.wTaskID=pTaskResult->wCurrTaskID;
			UserTaskInfo.dwResidueTime=pTaskParameter->dwTimeLimit;
			UserTaskInfo.dwLastUpdateTime=(DWORD)time(NULL);

			//设置任务
			m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),&UserTaskInfo,1);
		}
	}

	return true;
}

//系统消息
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBR_GR_SystemMessage));
	if(wDataSize!=sizeof(DBR_GR_SystemMessage)) return false;

	//提取数据
	DBR_GR_SystemMessage * pSystemMessage = (DBR_GR_SystemMessage *)pData;
	if(pSystemMessage==NULL) return false;

	for(INT_PTR nIndex=m_SystemMessageBuffer.GetCount()-1;nIndex>=0;nIndex--)
	{
		tagSystemMessage *pTagSystemMessage = m_SystemMessageBuffer[nIndex];
		if(pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID==pSystemMessage->dwMessageID)
		{
			CopyMemory(&pTagSystemMessage->SystemMessage, pSystemMessage, sizeof(DBR_GR_SystemMessage));
			m_SystemMessageActive.Add(pTagSystemMessage);
			m_SystemMessageBuffer.RemoveAt(nIndex);

			return true;
		}
	}

	//定义变量
	tagSystemMessage  *pSendMessage=new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//设置变量
	pSendMessage->dwLastTime=(DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage,pSystemMessage, sizeof(DBR_GR_SystemMessage));

	//记录消息
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//加载完成
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//处理临时消息
	if(m_SystemMessageBuffer.GetCount()>0)
	{
		//变量定义
		tagSystemMessage *pTagSystemMessage=NULL;

		for(INT_PTR nIndex=m_SystemMessageBuffer.GetCount()-1;nIndex>=0;nIndex--)
		{
			pTagSystemMessage = m_SystemMessageBuffer[nIndex];
			if(pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID==TEMP_MESSAGE_ID)
			{
				m_SystemMessageActive.Add(pTagSystemMessage);
				m_SystemMessageBuffer.RemoveAt(nIndex);
			}
		}
	}

	return true;
}

//加载敏感词
bool CAttemperEngineSink::OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//开始加载
	if(dwContextID==0xfffe)
	{
		m_WordsFilter.ResetSensitiveWordArray();
		m_pITimerEngine->KillTimer(IDI_LOAD_SENSITIVE_WORD);
		return true;			
	}

	//加载完成
	if(dwContextID==0xffff)
	{
		m_WordsFilter.FinishAdd();
		return true;
	}

	//加载敏感词
	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.AddSensitiveWords(pWords);
	return true;
}

//等级配置
bool CAttemperEngineSink::OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GrowLevelConfig * pGrowLevelConfig=(DBO_GR_GrowLevelConfig *)pData;

	//拷贝数据	
	m_wLevelCount = pGrowLevelConfig->wLevelCount;
	CopyMemory(m_GrowLevelConfig,&pGrowLevelConfig->GrowLevelConfig,sizeof(tagGrowLevelConfig)*m_wLevelCount);

	return true;
}

//等级参数
bool CAttemperEngineSink::OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_GrowLevelParameter * pGrowLevelParameter=(DBO_GR_GrowLevelParameter *)pData;

	//构造结构
	CMD_GR_GrowLevelParameter GrowLevelParameter;
	GrowLevelParameter.wCurrLevelID=pGrowLevelParameter->wCurrLevelID;	
	GrowLevelParameter.dwExperience=pGrowLevelParameter->dwExperience;
	GrowLevelParameter.dwUpgradeExperience=pGrowLevelParameter->dwUpgradeExperience;
	GrowLevelParameter.lUpgradeRewardGold=pGrowLevelParameter->lUpgradeRewardGold;
	GrowLevelParameter.lUpgradeRewardIngot=pGrowLevelParameter->lUpgradeRewardIngot;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_USER,SUB_GR_GROWLEVEL_PARAMETER,&GrowLevelParameter,sizeof(GrowLevelParameter));

	return true;
}

//等级升级
bool CAttemperEngineSink::OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_GrowLevelUpgrade * pGrowLevelUpgrade=(DBO_GR_GrowLevelUpgrade *)pData;

	//构造结构
	CMD_GR_GrowLevelUpgrade GrowLevelUpgrade;
	GrowLevelUpgrade.lCurrScore=pGrowLevelUpgrade->lCurrScore;
	GrowLevelUpgrade.lCurrIngot=pGrowLevelUpgrade->lCurrIngot;
	lstrcpyn(GrowLevelUpgrade.szNotifyContent,pGrowLevelUpgrade->szNotifyContent,CountArray(GrowLevelUpgrade.szNotifyContent));

	if (GrowLevelUpgrade.szNotifyContent[0]!=0)
	{
		tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->lScore=pGrowLevelUpgrade->lCurrScore;
		pUserInfo->lIngot=pGrowLevelUpgrade->lCurrIngot;

		SendUserInfoPacket(pIServerUserItem,dwContextID);
	}

	//发送数据
	WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
	wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_USER,SUB_GR_GROWLEVEL_UPGRADE,&GrowLevelUpgrade,wSendDataSize);

	return true;
}

//创建成功
bool CAttemperEngineSink::OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据校验
	ASSERT(wDataSize == sizeof(DBO_GR_CreateSuccess));
	if(wDataSize != sizeof(DBO_GR_CreateSuccess)) return false;

	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateSuccess* pCreateSuccess = (DBO_GR_CreateSuccess*)pData;

	//获取信息
	CTableFrame* pTableFrame = m_TableFrameArray[pCreateSuccess->dwTableID];
	WORD wChairID = pTableFrame->GetNullChairID();

	//初始分数
	LONGLONG lIniScore = 0;
	byte cbIsJoinGame = 0;
	INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagPersonalTableParameter* pTableParameter = m_PersonalTableParameterArray.GetAt(i);
		if(pTableParameter->dwPlayTurnCount == pCreateSuccess->dwDrawCountLimit && pTableParameter->dwPlayTimeLimit == pCreateSuccess->dwDrawTimeLimit)
		{
			lIniScore = pTableParameter->lIniScore;

			//将配置信息改为制定的配置信息
			pTableParameter->wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;	//参加游戏的最大人数
			pTableParameter->lCellScore = pCreateSuccess->lCellScore;													//私人房的最大底分

			pTableParameter->dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//私人放进行游戏的最大局数
			pTableParameter->dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//私人房进行游戏的最大时间 单位秒

			//cbIsJoinGame = pTableParameter->cbIsJoinGame ;

			break;
		}
	}
	cbIsJoinGame = m_PersonalRoomOption.cbIsJoinGame;
	pTableFrame->SetCellScore(pCreateSuccess->lCellScore);
	pTableFrame->SetGameRule(pCreateSuccess->cbGameRule, RULE_LEN);


	//如果使用的是金币数据库
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, 0);
		pTableFrame->SetDataBaseMode(0);
	}
	else
	{
		pTableFrame->SetPersonalTable(pCreateSuccess->dwDrawCountLimit, pCreateSuccess->dwDrawTimeLimit, lIniScore);
		pTableFrame->SetDataBaseMode(1);
	}


	//设置桌子配置信息
	tagPersonalTableParameter PersonalTableParameter;
	PersonalTableParameter.lIniScore = lIniScore;
	PersonalTableParameter.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;		//参加游戏的最大人数
	PersonalTableParameter.lCellScore = pCreateSuccess->lCellScore;													//私人房的最大底分
	PersonalTableParameter.dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//私人放进行游戏的最大局数
	PersonalTableParameter.dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;								//私人房进行游戏的最大时间 单位秒
	PersonalTableParameter.cbIsJoinGame = cbIsJoinGame;

	PersonalTableParameter.dwTimeAfterBeginCount = m_PersonalRoomOption.dwTimeAfterBeginCount;
	PersonalTableParameter.dwTimeAfterCreateRoom = m_PersonalRoomOption.dwTimeAfterCreateRoom;
	PersonalTableParameter.dwTimeNotBeginGame = m_PersonalRoomOption.dwTimeNotBeginGame;
	PersonalTableParameter.dwTimeOffLineCount = m_PersonalRoomOption.dwTimeOffLineCount;

	pTableFrame->SetPersonalTableParameter(PersonalTableParameter, m_PersonalRoomOption);

	//转转麻将设置一张桌子上椅子的个数
	if (m_pGameServiceAttrib->wKindID == ZZMJ_KIND_ID || m_pGameServiceAttrib->wKindID  == HZMJ_KIND_ID)
	{
		if (pCreateSuccess->cbGameRule[0] == SET_RULE) 
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
		}
	}

	//设置桌子	
	tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
	pUserInfo->dBeans = pCreateSuccess->dCurBeans;
	pUserInfo->lRoomCard = pCreateSuccess->lRoomCard;
	pTableFrame->SetTableOwner(pUserInfo->dwUserID);
	pTableFrame->SetTimerNotBeginAfterCreate();

	tagUserRule* pUserRule = pIServerUserItem->GetUserRule();
	lstrcpyn(pUserRule->szPassword, pCreateSuccess->szPassword, CountArray(pUserRule->szPassword));

	//如果房主参与游戏
	if (pCreateSuccess->cbIsJoinGame)
	{

		//执行坐下
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, pCreateSuccess->szPassword)== FALSE)
		{
			CTraceService::TraceString(TEXT("创建桌子坐下失败"), TraceLevel_Info);
			//解锁桌子
			pTableFrame->SetPersonalTableLlocked(false);
			pTableFrame->SetPersonalTable(0, 0, 0);
			pTableFrame->SetCellScore(0);
			pTableFrame->SetTableOwner(0);

			//退还费用
			DBR_GR_CancelCreateTable CancelCreateTable;
			ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			CancelCreateTable.dwUserID = pCreateSuccess->dwUserID;
			CancelCreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CancelCreateTable.dwTableID = pCreateSuccess->dwTableID;
			CancelCreateTable.dwReason = CANCELTABLE_REASON_SYSTEM;
			CancelCreateTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
			CancelCreateTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
			CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;

			//投递数据
			m_pIDBCorrespondManager->PostDataBaseRequest(pCreateSuccess->dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwContextID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			return true;
		}
	}




	//数据汇总
	CMD_CS_C_CreateTable CS_CreateTable;
	ZeroMemory(&CS_CreateTable, sizeof(CMD_CS_C_CreateTable));

	CS_CreateTable.dwSocketID = dwContextID;
	CS_CreateTable.dwClientAddr = pBindParameter->dwClientAddr;
	CS_CreateTable.PersonalTable.dwServerID	= m_pGameServiceOption->wServerID;
	CS_CreateTable.PersonalTable.dwKindID = m_pGameServiceOption->wKindID;
	CS_CreateTable.PersonalTable.dwTableID = pCreateSuccess->dwTableID;
	CS_CreateTable.PersonalTable.dwUserID = pIServerUserItem->GetUserID();
	CS_CreateTable.PersonalTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
	CS_CreateTable.PersonalTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
	CS_CreateTable.PersonalTable.lCellScore = pCreateSuccess->lCellScore;
	CS_CreateTable.PersonalTable.dwRoomTax = pCreateSuccess->dwRoomTax;
	CS_CreateTable.PersonalTable.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;
	lstrcpyn(CS_CreateTable.PersonalTable.szPassword, pCreateSuccess->szPassword, CountArray(CS_CreateTable.PersonalTable.szPassword));
	lstrcpyn(CS_CreateTable.szClientAddr, pCreateSuccess->szClientAddr, CountArray(CS_CreateTable.szClientAddr));

	//发送数据
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CREATE_TABLE, &CS_CreateTable, sizeof(CMD_CS_C_CreateTable));


	return true;
}

//创建失败
bool CAttemperEngineSink::OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据校验
	ASSERT(wDataSize == sizeof(DBO_GR_CreateFailure));
	if(wDataSize != sizeof(DBO_GR_CreateFailure)) return false;

	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateFailure* pCreateFailure = (DBO_GR_CreateFailure*)pData;

	//构造数据
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = pCreateFailure->lErrorCode;
	lstrcpyn(CreateFailure.szDescribeString, pCreateFailure->szDescribeString, CountArray(CreateFailure.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}

//取消创建
bool CAttemperEngineSink::OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据校验
	ASSERT(wDataSize == sizeof(DBO_GR_CancelCreateResult));
	if(wDataSize != sizeof(DBO_GR_CancelCreateResult)) return false;

	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if(pBindParameter == NULL) return true;
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	//获取数据
	DBO_GR_CancelCreateResult* pCancelCreateResult = (DBO_GR_CancelCreateResult*)pData;
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelCreateResult->dwTableID];
	ASSERT(pTableFrame != NULL);

	//设置桌子
	//ASSERT(pTableFrame->IsPersonalTableLocked() == true);
	pTableFrame->SetPersonalTableLlocked(false);
	pTableFrame->SetPersonalTable(0, 0, 0);
	if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_SYSTEM || pCancelCreateResult->dwReason == CANCELTABLE_REASON_PLAYER)
	{
		pTableFrame->SetTableOwner(0);
	}

	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		IServerUserItem* pUserItem =pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL) continue;

		//绑定参数
		WORD wBindIndex = pUserItem->GetBindIndex();
		tagBindParameter* pBind = GetBindParameter(wBindIndex);

		//构造数据
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = pCancelCreateResult->dwReason;
		if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_SYSTEM)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏自动解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_PLAYER)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏未开始，游戏自动解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_ENFOCE)
			lstrcpyn(CancelTable.szDescribeString, TEXT("房主退出游戏或游戏超时，游戏解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_OVER_TIME)
			lstrcpyn(CancelTable.szDescribeString, TEXT("游戏超时，游戏解散。"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_NOT_START)
			lstrcpyn(CancelTable.szDescribeString, TEXT("约战规定开始时间到未开始游戏，游戏解散。"), CountArray(CancelTable.szDescribeString));
		
		if (CANCELTABLE_REASON_NOT_START == pCancelCreateResult->dwReason || CANCELTABLE_REASON_OVER_TIME == pCancelCreateResult->dwReason)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		}
		//解散消息
		m_pITCPNetworkEngine->SendData(pBind->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//用户状态
		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		pTableFrame->PerformStandUpAction(pUserItem);
	}

	////构造数据
	//CMD_GR_CancelTable CancelTable;
	//ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
	//CancelTable.dwReason = pCancelCreateResult->dwReason;
	//lstrcpyn(CancelTable.szDescribeString, pCancelCreateResult->szDescribeString, CountArray(CancelTable.szDescribeString));

	////发送数据
	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

	////退出用户
	//pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);

	return true;
}

//私人配置
bool CAttemperEngineSink::OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagPersonalTableParameter)==0);
	if (wDataSize%sizeof(tagPersonalTableParameter)!=0) return false;

	//清楚缓存
	ASSERT(m_PersonalTableParameterArray.GetCount() == 0);
	if(m_PersonalTableParameterArray.GetCount() != 0)
	{
		INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
		for(INT_PTR i = 0; i < nSize; ++i)
		{
			tagPersonalTableParameter* pPersonalTableParameter = m_PersonalTableParameterArray.GetAt(i);
			SafeDelete(pPersonalTableParameter);
		}
		m_PersonalTableParameterArray.RemoveAll();
	}

	//数据转换
	WORD wCount = wDataSize/sizeof(tagPersonalTableParameter);
	tagPersonalTableParameter* pParameterData = (tagPersonalTableParameter*)pData;

	//保存配置
	for(int i = 0; i < wCount; ++i)
	{
		tagPersonalTableParameter* pPersonalTableParameter = new tagPersonalTableParameter;
		CopyMemory(pPersonalTableParameter,&pParameterData[i],sizeof(tagPersonalTableParameter));
		m_PersonalTableParameterArray.Add(pPersonalTableParameter);
	}

	return true;
}

//私人配置
bool CAttemperEngineSink::OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBO_GR_DissumeResult));
	if (sizeof(DBO_GR_DissumeResult)!=wDataSize) return false;
	DBO_GR_DissumeResult * pDissumeResult = (DBO_GR_DissumeResult *)pData;
	CMD_GR_DissumeTable  DissumeTable;
	DissumeTable.cbIsDissumSuccess = pDissumeResult->cbIsDissumSuccess;
	lstrcpyn(DissumeTable.szRoomID , pDissumeResult->szRoomID, sizeof(DissumeTable.szRoomID));
	DissumeTable.sysDissumeTime = pDissumeResult->sysDissumeTime;

	//获取房间玩家的信息
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		memcpy(&DissumeTable.PersonalUserScoreInfo[i],  &pDissumeResult->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
	}

	m_pITCPNetworkEngine->SendData(pDissumeResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_HOST_DISSUME_TABLE_RESULT, &DissumeTable, sizeof(CMD_GR_DissumeTable));

	return true;

}

//私人房间玩家请求房间信息
bool CAttemperEngineSink::OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	//ASSERT(wDataSize%sizeof(tagQueryPersonalRoomUserScore)==0);
	//if (wDataSize%sizeof(tagQueryPersonalRoomUserScore)!=0) return false;

	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_USER_QUERY_ROOM_SCORE_RESULT, pData, wDataSize);

	return true;

}

//私人房间玩家请求房间信息
bool CAttemperEngineSink::OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBO_GR_CurrenceRoomCardAndBeans));
	if (sizeof(DBO_GR_CurrenceRoomCardAndBeans)!=wDataSize) return false;
	DBO_GR_CurrenceRoomCardAndBeans * pCardAndBeans = (DBO_GR_CurrenceRoomCardAndBeans *)pData;
	CMD_GR_CurrenceRoomCardAndBeans  CurrenceRoomCardAndBeans;
	CurrenceRoomCardAndBeans.dbBeans = pCardAndBeans->dbBeans;
	CurrenceRoomCardAndBeans.lRoomCard = pCardAndBeans->lRoomCard;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CURRECE_ROOMCARD_AND_BEAN, &CurrenceRoomCardAndBeans, sizeof(CMD_GR_CurrenceRoomCardAndBeans));

	return true;

}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//变量定义
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//效验参数
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//房间信息
		{
			//废弃列表
			m_ServerListManager.DisuseServerItem();		

			//注册比赛
			if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				//构造结构
				CMD_CS_C_RegisterMatch RegitsterMatch;
				ZeroMemory(&RegitsterMatch,sizeof(RegitsterMatch));

				RegitsterMatch.wServerID = m_pGameServiceOption->wServerID;
				//基础信息
				RegitsterMatch.dwMatchID=m_pGameMatchOption->dwMatchID;
				RegitsterMatch.lMatchNo=m_pGameMatchOption->lMatchNo;	
				RegitsterMatch.cbMatchStatus=m_pGameMatchOption->cbMatchStatus;
				RegitsterMatch.cbMatchType=m_pGameMatchOption->cbMatchType;	
				lstrcpyn(RegitsterMatch.szMatchName,m_pGameMatchOption->szMatchName,CountArray(RegitsterMatch.szMatchName));
				//报名信息
				RegitsterMatch.cbFeeType=m_pGameMatchOption->cbFeeType;	
				RegitsterMatch.lSignupFee=m_pGameMatchOption->lSignupFee;
				RegitsterMatch.cbDeductArea=m_pGameMatchOption->cbDeductArea;
				RegitsterMatch.cbSignupMode=m_pGameMatchOption->cbSignupMode;	
				RegitsterMatch.cbJoinCondition=m_pGameMatchOption->cbJoinCondition;	
				RegitsterMatch.cbMemberOrder=m_pGameMatchOption->cbMemberOrder;	
				RegitsterMatch.dwExperience=m_pGameMatchOption->dwExperience;	
				RegitsterMatch.dwFromMatchID=m_pGameMatchOption->dwFromMatchID;	
				RegitsterMatch.cbFilterType=m_pGameMatchOption->cbFilterType;
				RegitsterMatch.wMaxRankID=m_pGameMatchOption->wMaxRankID;	
				RegitsterMatch.MatchEndDate=m_pGameMatchOption->MatchEndDate;
				RegitsterMatch.MatchStartDate=m_pGameMatchOption->MatchStartDate;
				//排名方式
				RegitsterMatch.cbRankingMode=m_pGameMatchOption->cbRankingMode;
				RegitsterMatch.wCountInnings=m_pGameMatchOption->wCountInnings;
				RegitsterMatch.cbFilterGradesMode=m_pGameMatchOption->cbFilterGradesMode;
				//分组配置
				RegitsterMatch.cbDistributeRule=m_pGameMatchOption->cbDistributeRule;
				RegitsterMatch.wMinDistributeUser=m_pGameMatchOption->wMinDistributeUser;
				RegitsterMatch.wDistributeTimeSpace=m_pGameMatchOption->wDistributeTimeSpace;
				RegitsterMatch.wMinPartakeGameUser=m_pGameMatchOption->wMinPartakeGameUser;
				RegitsterMatch.wMaxPartakeGameUser=m_pGameMatchOption->wMaxPartakeGameUser;

				CopyMemory(&RegitsterMatch.cbMatchRule,&m_pGameMatchOption->cbMatchRule,sizeof(RegitsterMatch.cbMatchRule));
				RegitsterMatch.wRewardCount=m_pGameMatchOption->wRewardCount;
				CopyMemory(&RegitsterMatch.MatchRewardInfo,m_pGameMatchOption->MatchRewardInfo,sizeof(RegitsterMatch.MatchRewardInfo));			

				//发送数据
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_MATCH,&RegitsterMatch,sizeof(RegitsterMatch));
			}

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//房间人数
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//变量定义
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);

			//设置人数
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//房间插入
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//变量定义
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//更新数据
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//房间修改
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//变量定义
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//设置房间
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGameServerItem->m_GameServer.szServerAddr));
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//房间删除
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//变量定义
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//变量定义
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//房间完成
		{
			//清理列表
			m_ServerListManager.CleanServerItem();

			//事件处理
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_CREATE_TABLE_RESULT:		//创建结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_CreateTableResult));
			if (wDataSize!=sizeof(CMD_CS_S_CreateTableResult)) return false;

			//变量定义
			CMD_CS_S_CreateTableResult * pCreateTableResult=(CMD_CS_S_CreateTableResult *)pData;

			//获取用户
			WORD wBindIndex=LOWORD(pCreateTableResult->dwSocketID);
			IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
			if (pIServerUserItem==NULL) return false;

			if(pCreateTableResult->PersonalTable.dwDrawCountLimit == 0 && pCreateTableResult->PersonalTable.dwDrawTimeLimit == 0)
			{
				//构造数据
				CMD_GR_CreateFailure CreateFailure;
				ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

				CreateFailure.lErrorCode = 1;
				lstrcpyn(CreateFailure.szDescribeString, TEXT("创建房间参数不正确！"), CountArray(CreateFailure.szDescribeString));

				//发送数据
				m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

				return false;
			}

			//获取桌子
			CTableFrame* pTableFrame = m_TableFrameArray[pCreateTableResult->PersonalTable.dwTableID];
			ASSERT(pTableFrame != NULL);

			tagPersonalTableParameter PersonalTableParameter = pTableFrame->GetPersonalTableParameter();
			pTableFrame->SetPersonalTableID(pCreateTableResult->PersonalTable.szRoomID);

			//构造数据
			CMD_GR_CreateSuccess CreateSuccess;
			ZeroMemory(&CreateSuccess, sizeof(CMD_GR_CreateSuccess));

			CreateSuccess.dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
			CreateSuccess.dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
			CreateSuccess.dBeans = pIServerUserItem->GetUserInfo()->dBeans;
			CreateSuccess.lRoomCard = pIServerUserItem->GetUserInfo()->lRoomCard;
			lstrcpyn(CreateSuccess.szServerID, pCreateTableResult->PersonalTable.szRoomID, CountArray(CreateSuccess.szServerID));
			
			m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_SUCCESS, &CreateSuccess, sizeof(CMD_GR_CreateSuccess));


			//插入桌子创建记录
			DBR_GR_InsertCreateRecord  CreateRecord;
			ZeroMemory(&CreateRecord, sizeof(DBR_GR_InsertCreateRecord));

			//桌子数据
			CreateRecord.dwServerID	= pCreateTableResult->PersonalTable.dwServerID;
			CreateRecord.dwUserID = pCreateTableResult->PersonalTable.dwUserID;
			CreateRecord.lCellScore = pCreateTableResult->PersonalTable.lCellScore;
			CreateRecord.dwDrawCountLimit = pCreateTableResult->PersonalTable.dwDrawCountLimit;
			CreateRecord.dwDrawTimeLimit = pCreateTableResult->PersonalTable.dwDrawTimeLimit;
			lstrcpyn(CreateRecord.szPassword, pCreateTableResult->PersonalTable.szPassword, CountArray(CreateRecord.szPassword));
			lstrcpyn(CreateRecord.szRoomID, pCreateTableResult->PersonalTable.szRoomID, CountArray(CreateRecord.szRoomID));
			CreateRecord.wJoinGamePeopleCount = pCreateTableResult->PersonalTable.wJoinGamePeopleCount;
			CreateRecord.dwRoomTax =  pCreateTableResult->PersonalTable.dwRoomTax;
			lstrcpyn(CreateRecord.szClientAddr, pCreateTableResult->szClientAddr, CountArray(CreateRecord.szClientAddr));

			m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_INSERT_CREATE_RECORD, 0, &CreateRecord, sizeof(CreateRecord));

			return true;
		}
	case SUB_CS_C_DISMISS_TABLE_RESULT:		//解散结果
		{
			ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
			if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;

			CMD_CS_C_DismissTableResult* pDismissTable = (CMD_CS_C_DismissTableResult*)pData;
			//获取桌子
			CTableFrame* pTableFrame = m_TableFrameArray[pDismissTable->PersonalTableInfo.dwTableID];
			ASSERT(pTableFrame != NULL);

			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				memcpy(&(pDismissTable->PersonalUserScoreInfo[i]), &(pTableFrame->m_PersonalUserScoreInfo[i]),  sizeof(tagPersonalUserScoreInfo));
			}
			for (int i = 0; i < pTableFrame->GetChairCount(); i++)
			{
				ZeroMemory(&(pTableFrame->m_PersonalUserScoreInfo[i]), sizeof(pTableFrame->m_PersonalUserScoreInfo[i]));
			}

			m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_DISSUME_ROOM, 0, pDismissTable, sizeof(CMD_CS_C_DismissTableResult));
			return true;

		}
		case SUB_CS_S_CLEARUER:
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ClearUser));
			if (wDataSize!=sizeof(CMD_CS_S_ClearUser)) return false;

			//变量定义
			CMD_CS_S_ClearUser * pClearUser=(CMD_CS_S_ClearUser *)pData;

			CString str;
			bool bOK = false;
			for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;++i)
			{

				CTableFrame * pTableFrame=m_TableFrameArray[i];
				if(pTableFrame == NULL)continue;
				IServerUserItem * pIServerUserItem=pTableFrame->SearchUserItem(pClearUser->dwUserID);
				if (pIServerUserItem!=NULL)
				{
					
					for(int j = 0;j < pTableFrame->GetChairCount();j++)
					{
						IServerUserItem * pIUserItem=pTableFrame->GetTableUserItem(j);
						if(pIUserItem!=NULL)
						{
							bOK = true;
							pTableFrame->DismissGame();
							pTableFrame->SendRoomMessage(pIUserItem,TEXT("您的账号为异常账号，已经被系统踢下线!"),SMT_EJECT|SMT_CHAT|SMT_GLOBAL);
							pTableFrame->PerformStandUpAction(pIUserItem);
							//pIUserItem->SetUserStatus(US_NULL,pIUserItem->GetTableID(),pIUserItem->GetChairID());
						}
					}
					break;
				}
			}
			if(bOK == true)
			{
				str.Format(_T("清理成功ＵＳＥＲＩＤ_%d"),pClearUser->dwUserID);
			}
			else
			{
				str.Format(_T("清理未成功ＵＳＥＲＩＤ_%d"),pClearUser->dwUserID);
			}
			CTraceService::TraceString(str,TraceLevel_Warning);
			return true;
		}
		//case SUB_CS_S_AMEND_GAME_CONTROL:
		//{
		//	
		//	return true;
		//}
	}

	return true;
}

//汇总事件
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//用户汇总
		{
			//变量定义
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//发送用户
			WORD wIndex=0;
			do
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//设置变量
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				UserEnter.dwGameID=pIServerUserItem->GetGameID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//辅助信息
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();
				UserEnter.cbAndroidUser=pIServerUserItem->IsAndroidUser();

				//发送数据
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));

			} while (true);

			//汇报完成
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:	//系统消息
		{
			//消息处理
			SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);

			return true;
		}
	case SUB_CS_S_PROPERTY_TRUMPET:  //喇叭消息
		{
			//发送数据
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_PROPERTY,SUB_GR_PROPERTY_TRUMPET,pData,wDataSize,BG_COMPUTER);

			return true;
		}
	case SUB_CS_S_PLATFORM_PARAMETER: //平台参数
		{
			//清除任务			
			m_wTaskCount=0;
			ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

			//加载配置
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//加载任务
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//会员参数
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_MEMBER_PARAMETER,0,NULL,0);	

			//成长配置
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GROWLEVEL_CONFIG,0,NULL,0);	

			//加载签到
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_CHECKIN_REWARD,0,NULL,0);	

			//加载低保
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_BASEENSURE,0L,NULL,0L);
			return true;
		}
	}

	return true;
}

//机器服务
bool CAttemperEngineSink::OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_ADDPARAMETER:		//添加参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_S_AddParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_AddParameter * pAddParameter = (CMD_CS_S_AddParameter *)pData;

			//添加参数
			m_AndroidUserManager.AddAndroidParameter(&pAddParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_MODIFYPARAMETER:  //修改参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_S_ModifyParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_ModifyParameter * pModifyParameter = (CMD_CS_S_ModifyParameter *)pData;

			//修改参数
			m_AndroidUserManager.AddAndroidParameter(&pModifyParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_DELETEPARAMETER:  //删除参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_S_DeleteParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_DeleteParameter * pDeleteParameter = (CMD_CS_S_DeleteParameter *)pData;

			//删除参数
			m_AndroidUserManager.RemoveAndroidParameter(pDeleteParameter->dwBatchID);

			return true;
		}
	}

	return true;
}

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//I D 登录
		{
			return OnTCPNetworkSubLogonUserID(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_MOBILE:		//手机登录
		{
			return OnTCPNetworkSubLogonMobile(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_ACCOUNTS:		//帐号登录
		{
			return OnTCPNetworkSubLogonAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//用户规则
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_LOOKON:		//用户旁观
		{
			return OnTCPNetworkSubUserLookon(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_SITDOWN:		//用户坐下
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_STANDUP:		//用户起立
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAT:			//用户聊天
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_EXPRESSION:	//用户表情
		{
			return OnTCPNetworkSubUserExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_CHAT:		//用户私聊
		{
			return OnTCPNetworkSubWisperChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_EXPRESSION:	//私聊表情
		{
			return OnTCPNetworkSubWisperExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INVITE_REQ:    //邀请用户
		{
			return OnTCPNetworkSubUserInviteReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_REPULSE_SIT:   //拒绝厌友
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_KICK_USER:    //踢出用户
		{
			return OnTCPNetworkSubMemberKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INFO_REQ:     //请求用户信息
		{
			return OnTCPNetworkSubUserInfoReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //请求更换位置
		{
			//进入开始锁定数据：保护共享资源只被一个线程调用
			EnterCriticalSection(&m_cs_test); 
			printf("SUB_GR_USER_CHAIR_REQ_____1\n");
			bool bOK = false;
			bOK =  OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
			if(bOK == true)
			{
				//变量定义		
				WORD wUserIndex=0;
				bool bSendAndroidFlag=false;
				IServerUserItem * pIServerUserItemSend=NULL;
				//获取用户
				WORD wBindIndex=LOWORD(dwSocketID);
				IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
				if(pIServerUserItem != NULL)
				{
					//权限判断		
					if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
					CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
					{
						bSendAndroidFlag=true;
					}

					//发送其他已在线用户给自己（除自己以外）		
					while (true)
					{
						pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
						if (pIServerUserItemSend==NULL) break;
						if (pIServerUserItemSend==pIServerUserItem) continue;
						SendUserInfoPacket(pIServerUserItemSend,dwSocketID,bSendAndroidFlag);
					}
				}				
			}

			//离开锁定的数据：允许共享资源被其他的线程调用
						printf("SUB_GR_USER_CHAIR_REQ_____2\n");
			LeaveCriticalSection(&m_cs_test);

			return bOK;
		}
	case SUB_GR_USER_CHAIR_INFO_REQ: //请求椅子用户信息
		{
			return OnTCPNetworkSubChairUserInfoReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GROWLEVEL_QUERY:		  //查询等级
		{
			return OnTCPNetworkSubGrowLevelQuery(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//银行处理
bool CAttemperEngineSink::OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_GR_ENABLE_INSURE_REQUEST:	//开通银行
		{
			return OnTCPNetworkSubEnableInsureRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_INSURE_INFO:		//银行查询
		{
			return OnTCPNetworkSubQueryInsureInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SAVE_SCORE_REQUEST:		//存款请求
		{
			return OnTCPNetworkSubSaveScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TAKE_SCORE_REQUEST:		//取款请求
		{
			return OnTCPNetworkSubTakeScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TRANSFER_SCORE_REQUEST:	//转帐请求
		{
			return OnTCPNetworkSubTransferScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_USER_INFO_REQUEST://查询用户
		{
			return OnTCPNetworkSubQueryUserInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//任务处理
bool CAttemperEngineSink::OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_TASK_TAKE:				//领取任务
		{
			return OnTCPNetworkSubTakeTaskRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_REWARD:			//领取奖励
		{
			return OnTCPNetworkSubTaskRewardRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_LOAD_INFO:			//加载任务
		{
			return OnTCPNetworkSubLoadTaskInfoRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_GIVEUP:			//放弃任务
		{
			return OnTCPNetworkSubGiveUpTaskRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//兑换处理
bool CAttemperEngineSink::OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_EXCHANGE_LOAD_INFO:		//查询信息
		{
			return OnTCPNetworkSubQueryExchangeInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PURCHASE_MEMBER:		//购买会员
		{
			return OnTCPNetworkSubPurchaseMember(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE_BYINGOT:	//兑换游戏币
		{
			return OnTCPNetworkSubExchangeScoreByIngot(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE_BYBEANS:	//兑换游戏币
		{
			return OnTCPNetworkSubExchangeScoreByBeans(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//低保处理
bool CAttemperEngineSink::OnTCPNetworkMainBaseEnsure( WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	switch (wSubCmdID)
	{
	case SUB_GR_BASEENSURE_LOAD:				//加载低保
		{
			return OnTCPNetworkSubBaseEnsureQueryRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_BASEENSURE_TAKE:			//领取低保
		{
			return OnTCPNetworkSubBaseEnsureTakeRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//会员处理
bool CAttemperEngineSink::OnTCPNetworkMainMember( WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	switch (wSubCmdID)
	{
	case SUB_GR_MEMBER_QUERY_INFO:		//会员查询
		{
			return OnTCPNetworkSubMemberQueryInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_MEMBER_DAY_PRESENT:		//会员送金
		{
			return OnTCPNetworkSubMemberDayPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_MEMBER_DAY_GIFT:		//会员礼包
		{
			return OnTCPNetworkSubMemberDayGift(pData,wDataSize,dwSocketID);
		}

	}

	return false;
}


//道具处理
bool CAttemperEngineSink::OnTCPNetworkMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_GAME_PROPERTY_BUY:	//购买道具
		{
			return OnTCPNetworkSubGamePropertyBuy(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_USE:		//道具使用
		{
			return OnTCPNetworkSubPropertyUse(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_SEND_PRESENT:	//查询赠送
		{
			return	OnTCPNetworkSubQuerySendPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_PRESENT:	//赠送道具
		{
			return OnTCPNetworkSubPropertyPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GET_SEND_PRESENT:	//获取赠送
		{
			return OnTCPNetworkSubGetSendPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_BACKPACK:	//背包道具
		{
			return OnTCPNetworkSubPropertyBackpack(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_TRUMPET:   //喇叭道具
		{
			return OnTCPNetwordSubSendTrumpet(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//签到处理
bool CAttemperEngineSink::OnTCPNetworkMainCheckIn(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_CHECKIN_QUERY:
		{
			return OnTCPNetworkSubCheckInQueryRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_CHECKIN_DONE:
		{
			return OnTCPNetworkSubCheckInDoneRequest(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//管理处理
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_OPTION:		//查询设置
		{
			return OnTCPNetworkSubQueryOption(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_OPTION_SERVER:		//房间设置
		{
			return OnTCPNetworkSubOptionServer(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KILL_USER:          //踢出用户
		{
			return OnTCPNetworkSubManagerKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LIMIT_USER_CHAT:	//限制聊天
		{
			return OnTCPNetworkSubLimitUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KICK_ALL_USER:		//踢出用户
		{
			return OnTCPNetworkSubKickAllUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_MESSAGE:		//发布消息
		{
			return OnTCPNetworkSubSendMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_DISMISSGAME:        //解散游戏
		{
			return OnTCPNetworkSubDismissGame(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_WARNING:		//警告用户
		{
			return OnTCPNetworkSubWarningUser(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//比赛命令
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//效验接口
	ASSERT(m_pIMatchServiceManager!=NULL);
	if (m_pIMatchServiceManager==NULL) return false;

	//消息处理
	return m_pIMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//私人房间
bool CAttemperEngineSink::OnTCPNetworkMainPersonalTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_GR_CREATE_TABLE:
		{
			return OnTCPNetworkSubCreateTable(pData, wDataSize, dwSocketID);
		}
	case SUB_GR_CANCEL_REQUEST:
		{
			return OnTCPNetworkSubCancelRequest(pData, wDataSize, dwSocketID);
		}
	case SUB_GR_REQUEST_REPLY:
		{
			return OnTCPNetworkSubRequestReply(pData, wDataSize, dwSocketID);
		}
	case SUB_GR_HOSTL_DISSUME_TABLE://房主强制解散桌子
		{
			return OnTCPNetworkSubHostDissumeTable(pData, wDataSize, dwSocketID);
		}
		//case SUB_GR_USER_QUERY_ROOM_SCORE: //请求房间成绩
		//	{
		//		return OnTCPNetworkSubQueryUserRoomScore(pData, wDataSize, dwSocketID);
		//	}
	}
	return false;
}

//游戏处理
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架处理
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//进入开始锁定数据：保护共享资源只被一个线程调用
	EnterCriticalSection(&m_cs_test);    
	printf("OnTCPNetworkMainFrame_____1\n");
	bool bOK = false;
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	bOK =  pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
	//离开锁定的数据：允许共享资源被其他的线程调用
	printf("OnTCPNetworkMainFrame_____2\n");
	LeaveCriticalSection(&m_cs_test);

	return bOK;
}

//I D 登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonUserID));
	if (wDataSize<sizeof(CMD_GR_LogonUserID)) return false;

	//处理消息
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//绑定信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//重复判断
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//密码校验
	if(((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0) && (LOWORD(dwSocketID)<INDEX_ANDROID))
	{
		//密码比较
		if(lstrcmp(pLogonUserID->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
		{
			//发送失败
			SendLogonFailure(TEXT("抱歉，您输入的房间密码不正确，请重新输入！"),FAILURE_TYPE_ROOM_PASSWORD_INCORRECT,dwSocketID);

			return true;
		}
	}

	//房间判断
	if(pLogonUserID->wKindID != m_pGameServiceOption->wKindID)
	{
		//发送失败
		SendLogonFailure(TEXT("很抱歉，此游戏房间已经关闭了，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//机器人和真人不许互踢
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("该帐号已在此房间游戏，且不允许踢出，请咨询管理员！"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return true;
		}
	}

	//密码判断
	CString strPassword = pLogonUserID->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//发送失败
		SendLogonFailure(TEXT("很抱歉，您的登录密码错误，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//版本信息
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonUserID->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonUserID->dwProcessVersion;

	//机器判断
	if(LOWORD(dwSocketID) < INDEX_ANDROID)
	{
		//大厅版本
		DWORD dwPlazaVersion=pLogonUserID->dwPlazaVersion;
		DWORD dwFrameVersion=pLogonUserID->dwFrameVersion;
		DWORD dwClientVersion=pLogonUserID->dwProcessVersion;
		if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;
	}

	//切换判断

	if((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonUserID->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonUserID->szMachineID,wBindIndex);
		return true;
	}

	//比赛效验
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,false))
	{
		return true;
	}

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwMatchID=m_pGameMatchOption->dwMatchID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));

	return true;
}

//手机登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonMobile));
	if (wDataSize<sizeof(CMD_GR_LogonMobile)) return false;

	//处理消息
	CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
	pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
	pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

	//绑定信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//重复判断
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//密码校验
	if(((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0) && (LOWORD(dwSocketID)<INDEX_ANDROID))
	{
		//密码比较
		if(lstrcmp(pLogonMobile->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
		{
			//发送失败
			SendLogonFailure(TEXT("抱歉，您输入的房间密码不正确，请重新输入！"),FAILURE_TYPE_ROOM_PASSWORD_INCORRECT,dwSocketID);

			return true;
		}
	}

	//机器人和真人不许互踢
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonMobile->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("该帐号已在此房间游戏，且不允许踢出，请咨询管理员！"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//密码判断
	CString strPassword = pLogonMobile->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//发送失败
		SendLogonFailure(TEXT("很抱歉，您的登录密码错误，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//版本信息
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->dwProcessVersion=pLogonMobile->dwProcessVersion;

	//大厅版本
	DWORD dwClientVersion=pLogonMobile->dwProcessVersion;
	if (PerformCheckVersion(0L,0L,dwClientVersion,dwSocketID)==false) return true;

	//切换判断
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonMobile->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonMobile->szMachineID,wBindIndex,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);
		return true;
	}

	//比赛效验
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,true))
	{
		return true;
	}

	//变量定义
	DBR_GR_LogonMobile LogonMobile;
	ZeroMemory(&LogonMobile,sizeof(LogonMobile));

	//构造数据
	LogonMobile.dwUserID=pLogonMobile->dwUserID;
	LogonMobile.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
	lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));
	LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
	LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
	LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;

	//私人房间
	if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		LogonMobile.cbPersonalServer = 1;
	}
	else
	{
		LogonMobile.cbPersonalServer = 0;
	}

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));

	return true;
}

//帐号登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonAccounts));
	if (wDataSize<=sizeof(CMD_GR_LogonAccounts)) return false;

	//处理消息
	CMD_GR_LogonAccounts * pLogonAccounts=(CMD_GR_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;

	//绑定信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//重复判断
	if (pIBindUserItem!=NULL)
	{ 
		ASSERT(FALSE);
		return false;
	}

	//机器人和真人不许互踢
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("该帐号已在此房间游戏，且不允许踢出，请咨询管理员！"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//密码判断
	CString strPassword = pLogonAccounts->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//发送失败
		SendLogonFailure(TEXT("很抱歉，您的登录密码错误，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//版本信息
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonAccounts->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonAccounts->dwProcessVersion;

	//大厅版本
	DWORD dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	DWORD dwFrameVersion=pLogonAccounts->dwFrameVersion;
	DWORD dwClientVersion=pLogonAccounts->dwProcessVersion;
	if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;

	//切换判断
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonAccounts->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonAccounts->szMachineID,wBindIndex);
		return true;
	}

	//变量定义
	DBR_GR_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//构造数据
	LogonAccounts.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//用户规则
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true||m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) return true;

	//消息处理
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//规则标志
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//规则属性
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//桌子密码
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//变量定义
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//提取处理
		while (true)
		{
			//提取数据
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//数据分析
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//桌子密码
				{
					//效验数据
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//规则判断
					ASSERT(CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==false);
					if (CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==true) break;

					//设置数据
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}

					break;
				}
			}
		}
	}

	return true;
}

//用户旁观
bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserLookon));
	if (wDataSize!=sizeof(CMD_GR_UserLookon)) return false;

	//效验数据
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//坐下处理
	CTableFrame * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//用户坐下
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//效验数据
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//重复判断
	if ((pUserSitDown->wTableID<m_pGameServiceOption->wTableCount)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}

	//插入分组
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//加入比赛
		InsertDistribute(pIServerUserItem);	
		
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		if(m_TableFrameArray[0]->EfficacyEnterTableScoreRule(0, pIServerUserItem))
		{
			InsertDistribute(pIServerUserItem);
		}

		return true;
	}

	//请求调整
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	DWORD wTailChairID = INVALID_CHAIR;
	//桌子调整
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//起始桌子
		WORD wStartTableID=0;
		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
		if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//动态加入
		bool bDynamicJoin=true;
		if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
		if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true && pIServerUserItem->GetTailGameID() != 0)
		{
			//寻找位置
			for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
			{
				CTableFrame * pTableFrame=m_TableFrameArray[i];
				if (pTableFrame != NULL)
				{
					for (int j =0;j<pTableFrame->GetChairCount();j++)
					{
						IServerUserItem *pIServerUser=pTableFrame->GetTableUserItem(j);
						if(pIServerUser != NULL)
						{
							if(pIServerUser->GetGameID() == pIServerUserItem->GetTailGameID())
							{	
								//设置变量
								wRequestTableID=i;
								wTailChairID = pIServerUser->GetChairID();
								break;
							}
						}
					}
				}
			}
			if (wRequestTableID < m_TableFrameArray.GetCount())
			{
				//获取空位
				WORD wNullChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();
				//wNullChairID = INVALID_CHAIR;
				if (wNullChairID == INVALID_CHAIR)
				{
					//游戏状态
					if ((m_TableFrameArray[wRequestTableID]->IsGameStarted()==true))
					{
						wRequestChairID = INVALID_CHAIR;
					}
					else
					{
						CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
						if (pTableFrame != NULL)
						{
							for (int j =0;j<pTableFrame->GetChairCount();j++)
							{
								IServerUserItem *pIServerUser=pTableFrame->GetTableUserItem(j);
								if(pIServerUser != NULL)
								{
									if(pIServerUser->IsAndroidUser())
									{	
										//设置变量
										pTableFrame->PerformStandUpAction(pIServerUser);
										wRequestChairID=j;
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					wRequestChairID = wNullChairID;
				}
				if(wRequestChairID == INVALID_CHAIR)
				{
					//坐下处理
					CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
					pTableFrame->PerformLookonAction(wTailChairID,pIServerUserItem);
					return true;
				}
			}
			else
			{
				//寻找位置
				for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
				{
					//游戏状态
					if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

					//获取空位
					WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

					//调整结果
					if (wNullChairID!=INVALID_CHAIR)
					{
						//设置变量
						wRequestTableID=i;
						wRequestChairID=wNullChairID;

						break;
					}
				}
			}

		}
		else
		{
			//寻找位置
			for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
			{
				//游戏状态
				if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

				//获取空位
				WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

				//调整结果
				if (wNullChairID!=INVALID_CHAIR)
				{
					//设置变量
					wRequestTableID=i;
					wRequestChairID=wNullChairID;

					break;
				}
			}
		}


		//结果判断
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("当前游戏房间已经人满为患了，暂时没有可以让您加入的位置，请稍后再试！"),0);
			return true;
		}
	}

	//椅子调整
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//效验参数
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//查找空位
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//结果判断
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("由于此游戏桌暂时没有可以让您加入的位置了，请选择另外的游戏桌！"),0);
			return true;
		}
	}

	//坐下处理
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword);

	return true;
}

//用户起立
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//定义变量
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;

	//取消分组
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		//取消分组
		DeleteDistribute(pIServerUserItem);

		if(pUserStandUp->wTableID==INVALID_TABLE) return true;
	}

	//效验数据
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//用户判断
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}

	//约战判断
	CTableFrame* pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
	ASSERT(pTableFrame != NULL);
	if(pTableFrame->GetDrawCount() != 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		SendRequestFailure(pIServerUserItem,TEXT("约战未结束，不能离开游戏，请先完成约战再离开！"),0);
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	return true;
}

//用户聊天
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_C_UserChat * pUserChat=(CMD_GR_C_UserChat *)pData;

	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GR_C_UserChat));
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

	//效验参数
	if (wDataSize>sizeof(CMD_GR_C_UserChat)) return false;
	if (wDataSize<(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//状态判断
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，当前此游戏房间禁止用户大厅聊天！"),SMT_CHAT);
		return true;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有大厅发言的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pUserChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//构造消息
	CMD_GR_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//字符过滤
	SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));
	
	//构造数据
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pUserChat->dwSendUserID;
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}

//用户表情
bool CAttemperEngineSink::OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_UserExpression));
	if (wDataSize!=sizeof(CMD_GR_C_UserExpression)) return false;

	//变量定义
	CMD_GR_C_UserExpression * pUserExpression=(CMD_GR_C_UserExpression *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserExpression->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserExpression->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//状态判断
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，当前此游戏房间禁止用户大厅聊天！"),SMT_CHAT);
		return true;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有大厅发言的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pUserExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//构造消息
	CMD_GR_S_UserExpression UserExpression;
	ZeroMemory(&UserExpression,sizeof(UserExpression));

	//构造数据
	UserExpression.wItemIndex=pUserExpression->wItemIndex;
	UserExpression.dwSendUserID=pUserExpression->dwSendUserID;
	UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

	//转发消息
	SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));

	return true;
}

//用户私聊
bool CAttemperEngineSink::OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_C_WisperChat * pWisperChat=(CMD_GR_C_WisperChat *)pData;

	//效验参数
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0])));

	//效验参数
	if (wDataSize<(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0]))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperChat->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//状态判断
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，当前此游戏房间禁止用户私聊！"),SMT_CHAT);
		return true;
	}

	//同桌判断
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//变量定义
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//规则判断
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//提示消息
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//权限判断
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pWisperChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//变量定义
	CMD_GR_S_WisperChat WisperChat;
	ZeroMemory(&WisperChat,sizeof(WisperChat));

	//字符过滤
	SensitiveWordFilter(pWisperChat->szChatString,WisperChat.szChatString,CountArray(WisperChat.szChatString));

	//构造数据
	WisperChat.dwChatColor=pWisperChat->dwChatColor;
	WisperChat.wChatLength=pWisperChat->wChatLength;
	WisperChat.dwSendUserID=pISendUserItem->GetUserID();
	WisperChat.dwTargetUserID=pIRecvUserItem->GetUserID();
	WisperChat.wChatLength=CountStringBuffer(WisperChat.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(WisperChat)-sizeof(WisperChat.szChatString);
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//转发消息
	WisperChat.dwSendUserID=pWisperChat->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//机器判断
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//枚举用户
		WORD wEnumIndex=0;
		IServerUserItem * pTempServerUserItem=NULL;
		do
		{
			pTempServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if(pTempServerUserItem!=NULL)
			{
				if(pTempServerUserItem==pISendUserItem) continue;
				if(pTempServerUserItem==pIRecvUserItem) continue;
				if(pTempServerUserItem->GetMasterOrder()>0 && 
				   CMasterRight::CanManagerAndroid(pTempServerUserItem->GetMasterRight())==true)
				{
					SendData(pTempServerUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));
				}
			}
		}while(pTempServerUserItem!=NULL);
	}

	return true;
}

//用户表情
bool CAttemperEngineSink::OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_WisperExpression));
	if (wDataSize!=sizeof(CMD_GR_C_WisperExpression)) return false;

	//变量定义
	CMD_GR_C_WisperExpression * pWisperExpression=(CMD_GR_C_WisperExpression *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperExpression->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//状态判断
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，当前此游戏房间禁止用户私聊！"),SMT_CHAT);
		return true;
	}

	//同桌判断
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//变量定义
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//规则判断
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//提示消息
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//权限判断
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pWisperExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//变量定义
	CMD_GR_S_WisperExpression WisperExpression;
	ZeroMemory(&WisperExpression,sizeof(WisperExpression));

	//构造数据
	WisperExpression.wItemIndex=pWisperExpression->wItemIndex;
	WisperExpression.dwSendUserID=pISendUserItem->GetUserID();
	WisperExpression.dwTargetUserID=pWisperExpression->dwTargetUserID;

	//转发消息
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//转发消息
	WisperExpression.dwSendUserID=pWisperExpression->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//机器判断
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//枚举用户
		WORD wEnumIndex=0;
		IServerUserItem * pTempServerUserItem=NULL;
		do
		{
			pTempServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if(pTempServerUserItem!=NULL)
			{
				if(pTempServerUserItem==pISendUserItem) continue;
				if(pTempServerUserItem==pIRecvUserItem) continue;
				if(pTempServerUserItem->GetMasterOrder()>0)
				{
					SendData(pTempServerUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));
				}
			}
		}while(pTempServerUserItem!=NULL);
	}

	return true;
}




//购买道具
bool CAttemperEngineSink::OnTCPNetworkSubGamePropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_PropertyBuy));
	if (wDataSize!=sizeof(CMD_GR_PropertyBuy)) return false;

	//变量定义
	CMD_GR_PropertyBuy * pPropertyBuy=(CMD_GR_PropertyBuy *)pData;

	//数据效验
	ASSERT(pPropertyBuy->dwItemCount>0);
	if (pPropertyBuy->dwItemCount==0) return false;

	//变量定义
	DBR_GR_PropertyBuy PropertyRequest;
	ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//购买信息
	PropertyRequest.dwUserID=pPropertyBuy->dwUserID;
	PropertyRequest.dwPropertyID=pPropertyBuy->dwPropertyID;
	PropertyRequest.dwItemCount=pPropertyBuy->dwItemCount;
	PropertyRequest.cbConsumeType = pPropertyBuy->cbConsumeType;
	PropertyRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(PropertyRequest.szPassword,pPropertyBuy->szPassword,CountArray(PropertyRequest.szPassword));
	lstrcpyn(PropertyRequest.szMachineID,pPropertyBuy->szMachineID,CountArray(PropertyRequest.szMachineID));

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_GAME_PROPERTY_BUY,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));
	
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyBackpack(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_BackpackProperty));
	if (wDataSize!=sizeof(CMD_GR_C_BackpackProperty)) return false;

	CMD_GR_C_BackpackProperty* pBackpackProperty = (CMD_GR_C_BackpackProperty*)pData;

	DBR_GR_QueryBackpack QueryBackpack;
	QueryBackpack.dwUserID = pBackpackProperty->dwUserID;
	QueryBackpack.dwKindID = pBackpackProperty->dwKindID;
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//系统信息
	QueryBackpack.dwClientAddr=pIServerUserItem->GetClientAddr();

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_BACKPACK,dwSocketID,&QueryBackpack,sizeof(QueryBackpack));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyUse(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyUse));
	if (wDataSize!=sizeof(CMD_GR_C_PropertyUse)) return false;

	//变量定义
	CMD_GR_C_PropertyUse * pPropertyUse = (CMD_GR_C_PropertyUse *)pData;

	//数据效验
	ASSERT(pPropertyUse->wPropCount>0);
	if (pPropertyUse->wPropCount==0) return false;

	//变量定义
	DBR_GR_PropertyUse PropertyUseRequest;
	ZeroMemory(&PropertyUseRequest,sizeof(PropertyUseRequest));
	PropertyUseRequest.dwUserID = pPropertyUse->dwUserID;
	PropertyUseRequest.dwRecvUserID = pPropertyUse->dwRecvUserID;
	PropertyUseRequest.wPropCount = pPropertyUse->wPropCount;
	PropertyUseRequest.dwPropID = pPropertyUse->dwPropID;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//系统信息
	PropertyUseRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
	
	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_USE,dwSocketID,&PropertyUseRequest,sizeof(PropertyUseRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubQuerySendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_QuerySendPresent));
	if (wDataSize!=sizeof(CMD_GR_C_QuerySendPresent)) return false;

	//变量定义
	CMD_GR_C_QuerySendPresent *pQuerySendPresent = (CMD_GR_C_QuerySendPresent *)pData;
	DBR_GR_QuerySendPresent QuerySendPresentRequest={0};
	QuerySendPresentRequest.dwUserID = pQuerySendPresent->dwUserID;
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//系统信息
	QuerySendPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_SEND_PRESENT,dwSocketID,&QuerySendPresentRequest,sizeof(QuerySendPresentRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyPresent));
	if (wDataSize!=sizeof(CMD_GR_C_PropertyPresent)) return false;

	//变量定义
	CMD_GR_C_PropertyPresent *PropertyPresent = (CMD_GR_C_PropertyPresent *)pData;
	DBR_GR_PropertyPresent PropertyPresentRequest={0};
	PropertyPresentRequest.dwUserID = PropertyPresent->dwUserID;
	PropertyPresentRequest.dwRecvGameID = PropertyPresent->dwRecvGameID;
	PropertyPresentRequest.dwPropID = PropertyPresent->dwPropID;
	PropertyPresentRequest.wPropCount = PropertyPresent->wPropCount;
	PropertyPresentRequest.wType = PropertyPresent->wType;
	lstrcpyn(PropertyPresentRequest.szRecvNickName,PropertyPresent->szRecvNickName,CountArray(PropertyPresentRequest.szRecvNickName));
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//系统信息
	PropertyPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_PRESENT,dwSocketID,&PropertyPresentRequest,sizeof(PropertyPresentRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubGetSendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_GetSendPresent));
	if (wDataSize!=sizeof(CMD_GR_C_GetSendPresent)) return false;

	//变量定义
	CMD_GR_C_GetSendPresent *pGetSendPresent = (CMD_GR_C_GetSendPresent *)pData;
	DBR_GR_GetSendPresent GetSendPresentRequest={0};
	GetSendPresentRequest.dwUserID = pGetSendPresent->dwUserID;
	lstrcpyn(GetSendPresentRequest.szPassword,pGetSendPresent->szPassword,CountArray(GetSendPresentRequest.szPassword));
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//系统信息
	GetSendPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_GET_SEND_PRESENT,dwSocketID,&GetSendPresentRequest,sizeof(GetSendPresentRequest));

	return true;
}


//使用道具
bool CAttemperEngineSink::OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_SendTrumpet));
	if (wDataSize!=sizeof(CMD_GR_C_SendTrumpet)) return false;

	//变量定义
	CMD_GR_C_SendTrumpet * pSendTrumpet=(CMD_GR_C_SendTrumpet *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if(pIServerUserItem==NULL) return false;

	//比赛房间
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		//发送消息
		SendPropertyFailure(pIServerUserItem,TEXT("比赛房间不可以使用此功能！"),0L,pSendTrumpet->cbRequestArea);
		return RESULT_FAIL;
	}

	//练习房间
	if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
	{
		SendPropertyFailure(pIServerUserItem,TEXT("练习房间不可以使用此功能！"),0L,pSendTrumpet->cbRequestArea);
		return RESULT_FAIL;
	}

	//构造结构
	DBR_GR_Send_Trumpet SendTrumpetRequest;
	ZeroMemory(&SendTrumpetRequest,sizeof(SendTrumpetRequest));

	SendTrumpetRequest.dwUserID = pIServerUserItem->GetUserID();
	SendTrumpetRequest.dwRecvUserID = pIServerUserItem->GetUserID();
	SendTrumpetRequest.wPropCount =1;
	SendTrumpetRequest.dwPropID = pSendTrumpet->wPropertyIndex;
	SendTrumpetRequest.TrumpetColor = pSendTrumpet->TrumpetColor;//喇叭颜色
	lstrcpyn(SendTrumpetRequest.szTrumpetContent,pSendTrumpet->szTrumpetContent,CountArray(SendTrumpetRequest.szTrumpetContent));
	lstrcpyn(SendTrumpetRequest.szSendNickName,pIServerUserItem->GetNickName(),CountArray(SendTrumpetRequest.szSendNickName));
	SendTrumpetRequest.dwClientAddr=pIServerUserItem->GetClientAddr();//系统信息

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_SEND_TRUMPET,dwSocketID,&SendTrumpetRequest,sizeof(SendTrumpetRequest));

	return true;
}

//邀请用户
bool CAttemperEngineSink::OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//消息处理
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//效验状态
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//目标用户
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//发送消息
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

	return true;
}

//拒绝厌友
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//消息处理
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//获取用户
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//发送消息
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("此桌有玩家设置了不与您同桌游戏！"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//弹起玩家
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//踢出命令
bool CAttemperEngineSink::OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//目标用户
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//权限判断
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//禁踢管理员
	if(pITargetUserItem->GetMasterOrder() > 0)
	{
		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("很抱歉，踢出管理员是不被允许的！"),SMT_EJECT);
		return true;
	}

	//百人游戏
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("很抱歉，百人游戏不许踢人！"),SMT_EJECT);
		return true;
	}

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//变量定义
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在游戏中,您不能将它踢出游戏！"),pITargetUserItem->GetNickName());

		//发送消息
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	//查找防踢卡
	tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE_TICK);
	if(pPropertyBuff != NULL)
	{
		//变量定义
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在使用防踢卡,您无法将它踢出游戏！"),pITargetUserItem->GetNickName());
		//发送消息
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true; 
	}

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//定义变量
		TCHAR szMessage[64]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("你已被%s请离桌子！"),pIServerUserItem->GetNickName());

		//发送消息
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//请求用户信息
bool CAttemperEngineSink::OnTCPNetworkSubUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_UserInfoReq)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL)
	{
		CTraceService::TraceString(TEXT("OnTCPNetworkSubUserInfoReq"),TraceLevel_Exception);

		return false;
	}

	//变量定义
	CMD_GR_UserInfoReq * pUserInfoReq = (CMD_GR_UserInfoReq *)pData;
	WORD wNewDeskPos = pUserInfoReq->wTablePos;
	WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-m_pInitParameter->m_wVisibleTableCount;

	//数量效验
	if(wNewDeskPos > wMaxDeskPos) wNewDeskPos = wMaxDeskPos;

	//更新信息
	pIServerUserItem->SetMobileUserDeskPos(wNewDeskPos);
	
	//发送信息
	SendVisibleTableUserInfoToMobileUser(pIServerUserItem,wNewDeskPos);

	return true;
}

//请求更换位置
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL)
	{
		CTraceService::TraceString(TEXT("OnTCPNetworkSubUserChairReq"),TraceLevel_Exception);
		return false;
	}

	//用户状态
	printf("玩家状态_%d\n",pIServerUserItem->GetUserStatus());
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//失败
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),REQUEST_FAILURE_NORMAL);
		return true;
	}
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
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}
		return true;
	}
	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;
	DWORD wRequestTableID = INVALID_CHAIR;
	DWORD wRequestChairID = INVALID_CHAIR;
	DWORD wTailChairID = INVALID_CHAIR;
	if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true && pIServerUserItem->GetTailGameID() != 0)
	{
		//寻找位置
		for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[i];
			if (pTableFrame != NULL)
			{
				for (int j =0;j<pTableFrame->GetChairCount();j++)
				{
					IServerUserItem *pIServerUser=pTableFrame->GetTableUserItem(j);
					if(pIServerUser != NULL)
					{
						if(pIServerUser->GetGameID() == pIServerUserItem->GetTailGameID())
						{	
							//设置变量
							wRequestTableID=i;
							wTailChairID = pIServerUser->GetChairID();
							break;
						}
					}
				}
			}
		}
		if (wRequestTableID < m_TableFrameArray.GetCount())
		{
			//获取空位
			WORD wNullChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();
			if (wNullChairID == INVALID_CHAIR)
			{
				//游戏状态
				if ((m_TableFrameArray[wRequestTableID]->IsGameStarted()==true))
				{
					wRequestChairID = INVALID_CHAIR;
				}
				else
				{
					CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
					if (pTableFrame != NULL)
					{
						for (int j =0;j<pTableFrame->GetChairCount();j++)
						{
							IServerUserItem *pIServerUser=pTableFrame->GetTableUserItem(j);
							if(pIServerUser != NULL)
							{
								if(pIServerUser->IsAndroidUser())
								{	
									//设置变量
									pTableFrame->PerformStandUpAction(pIServerUser);
									wRequestChairID=j;

									//离开处理
									if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
									{
										CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
										if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
									}

									//用户坐下
									return pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem);;
								}
							}
						}
					}
				}
			}
			else
			{
				wRequestChairID = wNullChairID;

				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//用户坐下
				return m_TableFrameArray[wRequestTableID]->PerformSitDownAction(wRequestChairID,pIServerUserItem);;
			}
			if(wRequestChairID == INVALID_CHAIR)
			{
				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//坐下处理
				CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
				return pTableFrame->PerformLookonAction(wTailChairID,pIServerUserItem);
			}
		}
	}
	//查找桌子
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=((INT)i+nStartIndex)%(INT)m_pGameServiceOption->wTableCount;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//用户坐下
		return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
	}

	//查找桌子
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=((INT)i+nStartIndex)%(INT)m_pGameServiceOption->wTableCount;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//用户坐下
		return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
	}

	//查找同桌
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//无效过滤
			WORD wTableID=pIServerUserItem->GetTableID();
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//用户坐下
				return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
			}
		}
	}

	//失败
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("没找到可进入的游戏桌！"),REQUEST_FAILURE_NORMAL);
	return true;
}

//请求椅子用户信息
bool CAttemperEngineSink::OnTCPNetworkSubChairUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_ChairUserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_ChairUserInfoReq)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_ChairUserInfoReq * pUserInfoReq = (CMD_GR_ChairUserInfoReq *)pData;
	if(pUserInfoReq->wTableID == INVALID_TABLE) return true;
	if(pUserInfoReq->wTableID >= m_pGameServiceOption->wTableCount)return true;

	//发送消息
	WORD wChairCout = m_TableFrameArray[pUserInfoReq->wTableID]->GetChairCount();
	for(WORD wIndex = 0; wIndex < wChairCout; wIndex++)
	{
		//获取用户
		if(pUserInfoReq->wChairID != INVALID_CHAIR && wIndex != pUserInfoReq->wChairID)continue;
		IServerUserItem * pTagerIServerUserItem=m_TableFrameArray[pUserInfoReq->wTableID]->GetTableUserItem(wIndex);
		if(pTagerIServerUserItem==NULL)continue;

		//变量定义
		BYTE cbBuffer[SOCKET_TCP_PACKET]={0};
		tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));
		tagUserInfo *pUserInfo = pTagerIServerUserItem->GetUserInfo();

		//用户属性
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
		pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

		//用户属性
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//用户状态
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//用户局数
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;
		pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

		//用户成绩
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pTagerIServerUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pTagerIServerUserItem->GetFrozenedScore();
		pUserInfoHead->dBeans=pUserInfo->dBeans;
		pUserInfoHead->lIngot=pUserInfo->lIngot;
		pUserInfoHead->bAndroid = pTagerIServerUserItem->IsAndroidUser();
		lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

		//叠加信息
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//发送消息
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//等级查询
bool CAttemperEngineSink::OnTCPNetworkSubGrowLevelQuery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//绑定信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	ASSERT(pBindParameter!=NULL);
	if (pBindParameter==NULL) return true;


	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_GrowLevelQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_GrowLevelQueryInfo)) return false;

	//提取数据
	CMD_GR_GrowLevelQueryInfo * pGrowLevelQueryInfo= (CMD_GR_GrowLevelQueryInfo *)pData;
	pGrowLevelQueryInfo->szPassword[CountArray(pGrowLevelQueryInfo->szPassword)-1]=0;
	pGrowLevelQueryInfo->szMachineID[CountArray(pGrowLevelQueryInfo->szMachineID)-1]=0;


	//构造结构
	DBR_GR_GrowLevelQueryInfo GrowLevelQueryInfo;
	GrowLevelQueryInfo.dwUserID = pGrowLevelQueryInfo->dwUserID;
	GrowLevelQueryInfo.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(GrowLevelQueryInfo.szPassword,pGrowLevelQueryInfo->szPassword,CountArray(GrowLevelQueryInfo.szPassword));
	lstrcpyn(GrowLevelQueryInfo.szMachineID,pGrowLevelQueryInfo->szMachineID,CountArray(GrowLevelQueryInfo.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pGrowLevelQueryInfo->dwUserID,DBR_GR_GROWLEVEL_QUERY_IFNO,dwSocketID,&GrowLevelQueryInfo,sizeof(GrowLevelQueryInfo));

	return true;
}

//查询参数
bool CAttemperEngineSink::OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//构造结构
	CMD_GR_ExchangeParameter ExchangeParameter;
	ZeroMemory(&ExchangeParameter,sizeof(ExchangeParameter));

	//设置变量
	ExchangeParameter.wMemberCount=m_wMemberCount;
	ExchangeParameter.dwExchangeRate=m_pGameParameter->dwExchangeRate;
	ExchangeParameter.dwPresentExchangeRate=m_pGameParameter->dwPresentExchangeRate;	
	ExchangeParameter.dwRateGold=m_pGameParameter->dwRateGold;	
	CopyMemory(ExchangeParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameter)*m_wMemberCount);

	//计算大小
	WORD wSendDataSize = sizeof(ExchangeParameter)-sizeof(ExchangeParameter.MemberParameter);
	wSendDataSize += sizeof(tagMemberParameter)*ExchangeParameter.wMemberCount;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_PARAM_INFO,&ExchangeParameter,wSendDataSize);

	return true;
}

//购买会员
bool CAttemperEngineSink::OnTCPNetworkSubPurchaseMember(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_PurchaseMember));
	if(wDataSize!=sizeof(CMD_GR_PurchaseMember)) return false;

	//构造结构
	CMD_GR_PurchaseMember * pPurchaseMember = (CMD_GR_PurchaseMember*)pData;
	pPurchaseMember->szMachineID[CountArray(pPurchaseMember->szMachineID)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_PurchaseMember PurchaseMember;
	ZeroMemory(&PurchaseMember,sizeof(PurchaseMember));

	//设置变量
	PurchaseMember.dwUserID=pPurchaseMember->dwUserID;
	PurchaseMember.cbMemberOrder=pPurchaseMember->cbMemberOrder;
	PurchaseMember.wPurchaseTime=pPurchaseMember->wPurchaseTime;
	PurchaseMember.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(PurchaseMember.szMachineID,pPurchaseMember->szMachineID,CountArray(PurchaseMember.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(PurchaseMember.dwUserID,DBR_GR_PURCHASE_MEMBER,dwSocketID,&PurchaseMember,sizeof(PurchaseMember));

	return true;
}

//兑换游戏币
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScoreByIngot(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScoreByIngot));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScoreByIngot)) return false;

	//构造结构
	CMD_GR_ExchangeScoreByIngot * pExchangeScore = (CMD_GR_ExchangeScoreByIngot*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//数据校验
	ASSERT(pExchangeScore->lExchangeIngot>0);
	if (pExchangeScore->lExchangeIngot<=0) return false;

	//构造结构
	DBR_GR_ExchangeScoreByIngot ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//设置变量
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.lExchangeIngot=pExchangeScore->lExchangeIngot;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE_INGOT,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//兑换游戏币
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScoreByBeans(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScoreByBeans));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScoreByBeans)) return false;

	//构造结构
	CMD_GR_ExchangeScoreByBeans * pExchangeScore = (CMD_GR_ExchangeScoreByBeans*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_ExchangeScoreByBeans ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//设置变量
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.dExchangeBeans=pExchangeScore->dExchangeBeans;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE_BEANS,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//查询银行
bool CAttemperEngineSink::OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//变量定义
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//构造数据
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(QueryInsureInfo.szPassword,pQueryInsureInfoRequest->szInsurePass,CountArray(QueryInsureInfo.szPassword));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return true;
}

//开通银行
bool CAttemperEngineSink::OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_EnableInsureRequest));
	if (wDataSize!=sizeof(CMD_GR_C_EnableInsureRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_C_EnableInsureRequest * pEnableInsureRequest = (CMD_GR_C_EnableInsureRequest *)pData;

	//变量定义
	DBR_GR_UserEnableInsure UserEnableInsure;
	ZeroMemory(&UserEnableInsure,sizeof(UserEnableInsure));

	//构造数据	
	UserEnableInsure.dwUserID=pIServerUserItem->GetUserID();
	UserEnableInsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserEnableInsure.cbActivityGame=pEnableInsureRequest->cbActivityGame;
	lstrcpyn(UserEnableInsure.szLogonPass,pEnableInsureRequest->szLogonPass,CountArray(UserEnableInsure.szLogonPass));
	lstrcpyn(UserEnableInsure.szInsurePass,pEnableInsureRequest->szInsurePass,CountArray(UserEnableInsure.szInsurePass));
	lstrcpyn(UserEnableInsure.szMachineID,pEnableInsureRequest->szMachineID,CountArray(UserEnableInsure.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(UserEnableInsure.dwUserID,DBR_GR_USER_ENABLE_INSURE,dwSocketID,&UserEnableInsure,sizeof(UserEnableInsure));

	return true;
}

//存款请求
bool CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_SaveScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_SaveScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_SaveScoreRequest * pSaveScoreRequest=(CMD_GR_C_SaveScoreRequest *)pData;

	//效验参数
	ASSERT(pSaveScoreRequest->lSaveScore>0L);
	if (pSaveScoreRequest->lSaveScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	if(pSaveScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendSaveInRoom(m_pGameServiceOption->dwServerRule))
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止房间存款，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//规则判断
	if(pSaveScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule))
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止游戏存款，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//变量定义
	SCORE lConsumeQuota=0L;
	SCORE lUserWholeScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();

	//获取限额
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		lConsumeQuota=m_TableFrameArray[wTableID]->QueryConsumeQuota(pIServerUserItem);
	}
	else
	{
		lConsumeQuota=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	}

	//限额判断
	if (pSaveScoreRequest->lSaveScore>lConsumeQuota)
	{
		if (lConsumeQuota<lUserWholeScore)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("由于您正在游戏中，游戏币可存入额度为 %.2f，存入操作失败！"),lConsumeQuota);

			//发送数据
			SendInsureFailure(pIServerUserItem,szDescribe,0L,pSaveScoreRequest->cbActivityGame);
		}
		else
		{
			//发送数据
			SendInsureFailure(pIServerUserItem,TEXT("您的游戏币余额不足，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		}

		return true;
	}

	//锁定积分
	if (pIServerUserItem->FrozenedUserScore(pSaveScoreRequest->lSaveScore)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	DBR_GR_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

	//构造数据
	UserSaveScore.cbActivityGame=pSaveScoreRequest->cbActivityGame;
	UserSaveScore.dwUserID=pIServerUserItem->GetUserID();
	UserSaveScore.lSaveScore=pSaveScoreRequest->lSaveScore;
	UserSaveScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserSaveScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserSaveScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

	return true;
}

//取款请求
bool CAttemperEngineSink::OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_TakeScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_TakeScoreRequest * pTakeScoreRequest=(CMD_GR_C_TakeScoreRequest *)pData;
	pTakeScoreRequest->szInsurePass[CountArray(pTakeScoreRequest->szInsurePass)-1]=0;

	//效验参数
	ASSERT(pTakeScoreRequest->lTakeScore>0L);
	if (pTakeScoreRequest->lTakeScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	if(pTakeScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendTakeInRoom(m_pGameServiceOption->dwServerRule))
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止房间取款，取出操作失败！"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//规则判断
	if(pTakeScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendTakeInGame(m_pGameServiceOption->dwServerRule))
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止游戏取款，取出操作失败！"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//变量定义
	DBR_GR_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

	//构造数据
	UserTakeScore.cbActivityGame=pTakeScoreRequest->cbActivityGame;
	UserTakeScore.dwUserID=pIServerUserItem->GetUserID();
	UserTakeScore.lTakeScore=pTakeScoreRequest->lTakeScore;
	UserTakeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserTakeScore.szPassword,pTakeScoreRequest->szInsurePass,CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTakeScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

	return true;
}

//转帐请求
bool CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GP_C_TransferScoreRequest));
	if (wDataSize!=sizeof(CMD_GP_C_TransferScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GP_C_TransferScoreRequest * pTransferScoreRequest=(CMD_GP_C_TransferScoreRequest *)pData;
	pTransferScoreRequest->szAccounts[CountArray(pTransferScoreRequest->szAccounts)-1]=0;
	pTransferScoreRequest->szInsurePass[CountArray(pTransferScoreRequest->szInsurePass)-1]=0;

	//效验参数
	ASSERT(pTransferScoreRequest->lTransferScore>0L);
	if (pTransferScoreRequest->lTransferScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBR_GR_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

	//构造数据
	UserTransferScore.cbActivityGame=pTransferScoreRequest->cbActivityGame;
	UserTransferScore.dwUserID=pIServerUserItem->GetUserID();
	UserTransferScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserTransferScore.lTransferScore=pTransferScoreRequest->lTransferScore;
	lstrcpyn(UserTransferScore.szAccounts,pTransferScoreRequest->szAccounts,CountArray(UserTransferScore.szAccounts));
	lstrcpyn(UserTransferScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTransferScore.szMachineID));
	lstrcpyn(UserTransferScore.szPassword,pTransferScoreRequest->szInsurePass,CountArray(UserTransferScore.szPassword));
	lstrcpyn(UserTransferScore.szTransRemark,pTransferScoreRequest->szTransRemark,CountArray(UserTransferScore.szTransRemark));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

	return true;
}

//查询用户请求
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryUserInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryUserInfoRequest)) return false;

	//房间判断
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GR_C_QueryUserInfoRequest *)pData;
	pQueryUserInfoRequest->szAccounts[CountArray(pQueryUserInfoRequest->szAccounts)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//ID判断
	if(pQueryUserInfoRequest->cbByNickName==FALSE)
	{
		//长度判断
		int nLen=lstrlen(pQueryUserInfoRequest->szAccounts);
		if(nLen>=8)
		{
			SendInsureFailure(pIServerUserItem,TEXT("请输入合法的玩家ID！"), 0, pQueryUserInfoRequest->cbActivityGame);
			return true;
		}

		//合法判断
		for(int i=0; i<nLen; i++)
		{
			if(pQueryUserInfoRequest->szAccounts[i] < TEXT('0') || pQueryUserInfoRequest->szAccounts[i] > TEXT('9'))
			{
				SendInsureFailure(pIServerUserItem,TEXT("请输入合法的玩家ID！"), 0, pQueryUserInfoRequest->cbActivityGame);
				return true;
			}
		}
	}

	//变量定义
	DBR_GR_QueryTransferUserInfo QueryTransferUserInfo;
	ZeroMemory(&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	//构造数据	
	QueryTransferUserInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryTransferUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
	QueryTransferUserInfo.cbActivityGame=pQueryUserInfoRequest->cbActivityGame;	
	lstrcpyn(QueryTransferUserInfo.szAccounts,pQueryUserInfoRequest->szAccounts,CountArray(QueryTransferUserInfo.szAccounts));	

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_TRANSFER_USER_INFO,dwSocketID,&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubCheckInQueryRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_CheckInQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_CheckInQueryInfo)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//提取数据
	CMD_GR_CheckInQueryInfo *pCheckInQueryInfo = (CMD_GR_CheckInQueryInfo *)pData;
	pCheckInQueryInfo->szPassword[CountArray(pCheckInQueryInfo->szPassword)-1]=0;


	//构造结构
	DBR_GR_CheckInQueryInfo CheckInQueryInfo;
	CheckInQueryInfo.dwUserID = pCheckInQueryInfo->dwUserID;
	lstrcpyn(CheckInQueryInfo.szPassword,pCheckInQueryInfo->szPassword,CountArray(CheckInQueryInfo.szPassword));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_CHECKIN_QUERY_INFO,dwSocketID,&CheckInQueryInfo,sizeof(CheckInQueryInfo));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubCheckInDoneRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_CheckInDone));
	if(wDataSize!=sizeof(CMD_GR_CheckInDone)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//提取数据
	CMD_GR_CheckInDone *pCheckInDone = (CMD_GR_CheckInDone *)pData;
	pCheckInDone->szPassword[CountArray(pCheckInDone->szPassword)-1]=0;
	pCheckInDone->szMachineID[CountArray(pCheckInDone->szMachineID)-1]=0;

	//构造结构
	DBR_GR_CheckInDone CheckInDone;
	CheckInDone.dwUserID = pCheckInDone->dwUserID;
	CheckInDone.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(CheckInDone.szPassword,pCheckInDone->szPassword,CountArray(CheckInDone.szPassword));
	lstrcpyn(CheckInDone.szMachineID,pCheckInDone->szMachineID,CountArray(CheckInDone.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_CHECKIN_DONE,dwSocketID,&CheckInDone,sizeof(CheckInDone));

	return true;
}

//领取任务
bool CAttemperEngineSink::OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeTask));
	if(wDataSize!=sizeof(CMD_GR_C_TakeTask)) return false;

	//提取数据
	CMD_GR_C_TakeTask * pTaskTake = (CMD_GR_C_TakeTask *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskTake TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_TAKE,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//领取奖励
bool CAttemperEngineSink::OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_TaskReward));
	if(wDataSize!=sizeof(CMD_GR_C_TaskReward)) return false;

	//提取数据
	CMD_GR_C_TaskReward * pTaskReward = (CMD_GR_C_TaskReward *)pData;
	pTaskReward->szPassword[CountArray(pTaskReward->szPassword)-1]=0;
	pTaskReward->szMachineID[CountArray(pTaskReward->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskReward->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskReward TaskReward;
	TaskReward.dwUserID = pTaskReward->dwUserID;
	TaskReward.wTaskID = pTaskReward->wTaskID;
	TaskReward.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskReward.szPassword,pTaskReward->szPassword,CountArray(TaskReward.szPassword));
	lstrcpyn(TaskReward.szMachineID,pTaskReward->szMachineID,CountArray(TaskReward.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_REWARD,dwSocketID,&TaskReward,sizeof(TaskReward));

	return true;
}

//加载任务
bool CAttemperEngineSink::OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_LoadTaskInfo));
	if(wDataSize!=sizeof(CMD_GR_C_LoadTaskInfo)) return false;

	//提取数据
	CMD_GR_C_LoadTaskInfo * pLoadTaskInfo = (CMD_GR_C_LoadTaskInfo *)pData;
	pLoadTaskInfo->szPassword[CountArray(pLoadTaskInfo->szPassword)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLoadTaskInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskQueryInfo TaskQueryInfo;
	TaskQueryInfo.dwUserID = pLoadTaskInfo->dwUserID;
	lstrcpyn(TaskQueryInfo.szPassword,pLoadTaskInfo->szPassword,CountArray(TaskQueryInfo.szPassword));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_QUERY_INFO,dwSocketID,&TaskQueryInfo,sizeof(TaskQueryInfo));

	return true;
}
//放弃任务
bool CAttemperEngineSink::OnTCPNetworkSubGiveUpTaskRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeGiveUp));
	if(wDataSize!=sizeof(CMD_GR_C_TakeGiveUp)) return false;

	//提取数据
	CMD_GR_C_TakeGiveUp * pTaskTake = (CMD_GR_C_TakeGiveUp *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskGiveUp TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_GIVEUP,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//任务查询
bool CAttemperEngineSink::OnTCPNetworkSubMemberQueryInfo( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_MemberQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_MemberQueryInfo)) return false;

	//提取数据
	CMD_GR_MemberQueryInfo * pMemberInfo= (CMD_GR_MemberQueryInfo *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_MemberQueryInfo MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_QUERY_INFO,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//任务送金
bool CAttemperEngineSink::OnTCPNetworkSubMemberDayPresent( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_MemberDayPresent));
	if(wDataSize!=sizeof(CMD_GR_MemberDayPresent)) return false;

	//提取数据
	CMD_GR_MemberDayPresent * pMemberInfo= (CMD_GR_MemberDayPresent *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_MemberDayPresent MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_DAY_PRESENT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//任务礼包
bool CAttemperEngineSink::OnTCPNetworkSubMemberDayGift( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_MemberDayGift));
	if(wDataSize!=sizeof(CMD_GR_MemberDayGift)) return false;

	//提取数据
	CMD_GR_MemberDayGift * pMemberInfo= (CMD_GR_MemberDayGift *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_MemberDayGift MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_DAY_GIFT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//查询低保
bool CAttemperEngineSink::OnTCPNetworkSubBaseEnsureQueryRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//构造结构
	CMD_GR_BaseEnsureParamter BaseEnsureParameter;
	BaseEnsureParameter.cbTakeTimes=m_BaseEnsureParameter.cbTakeTimes;
	BaseEnsureParameter.lScoreAmount=m_BaseEnsureParameter.lScoreAmount;
	BaseEnsureParameter.lScoreCondition=m_BaseEnsureParameter.lScoreCondition;

	//投递请求
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_BASEENSURE,SUB_GR_BASEENSURE_PARAMETER,&BaseEnsureParameter,sizeof(BaseEnsureParameter));

	return true;
}

//领取低保
bool CAttemperEngineSink::OnTCPNetworkSubBaseEnsureTakeRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_BaseEnsureTake));
	if(wDataSize!=sizeof(CMD_GR_BaseEnsureTake)) return false;

	//提取数据
	CMD_GR_BaseEnsureTake * pBaseEnsureTake = (CMD_GR_BaseEnsureTake *)pData;
	pBaseEnsureTake->szPassword[CountArray(pBaseEnsureTake->szPassword)-1]=0;
	pBaseEnsureTake->szMachineID[CountArray(pBaseEnsureTake->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pBaseEnsureTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TakeBaseEnsure TakeBaseEnsure;
	TakeBaseEnsure.dwUserID = pBaseEnsureTake->dwUserID;
	TakeBaseEnsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TakeBaseEnsure.szPassword,pBaseEnsureTake->szPassword,CountArray(TakeBaseEnsure.szPassword));
	lstrcpyn(TakeBaseEnsure.szMachineID,pBaseEnsureTake->szMachineID,CountArray(TakeBaseEnsure.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_BASEENSURE_TAKE,dwSocketID,&TakeBaseEnsure,sizeof(TakeBaseEnsure));

	return true;
}

//查询设置
bool CAttemperEngineSink::OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//变量定义
	CMD_GR_OptionCurrent OptionCurrent;
	ZeroMemory(&OptionCurrent,sizeof(OptionCurrent));

	//挂接属性
	OptionCurrent.ServerOptionInfo.wKindID=m_pGameServiceOption->wKindID;
	OptionCurrent.ServerOptionInfo.wNodeID=m_pGameServiceOption->wNodeID;
	OptionCurrent.ServerOptionInfo.wSortID=m_pGameServiceOption->wSortID;

	//房间配置
	OptionCurrent.ServerOptionInfo.wRevenueRatio=m_pGameServiceOption->wRevenueRatio;
	OptionCurrent.ServerOptionInfo.lServiceScore=m_pGameServiceOption->lServiceScore;
	OptionCurrent.ServerOptionInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	OptionCurrent.ServerOptionInfo.lMinTableScore=m_pGameServiceOption->lMinTableScore;
	OptionCurrent.ServerOptionInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	OptionCurrent.ServerOptionInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

	//会员限制
	OptionCurrent.ServerOptionInfo.cbMinEnterMember=m_pGameServiceOption->cbMinEnterMember;
	OptionCurrent.ServerOptionInfo.cbMaxEnterMember=m_pGameServiceOption->cbMaxEnterMember;

	//房间属性
	OptionCurrent.ServerOptionInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	lstrcpyn(OptionCurrent.ServerOptionInfo.szServerName,m_pGameServiceOption->szServerName,CountArray(OptionCurrent.ServerOptionInfo.szServerName));

	//聊天规则
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_ON_GAME;

	//房间规则
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_LOOKON;

	//银行规则
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_GAME;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_GAME;

	//其他规则
	//OptionCurrent.dwRuleMask|=SR_RECORD_GAME_TRACK;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_RULE;
	OptionCurrent.dwRuleMask|=SR_FORFEND_LOCK_TABLE;
	OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_SIMULATE;

	//组件规则
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_DYNAMIC_JOIN;
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_ATTEND;
	//if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_OFFLINE_TRUSTEE;

	//模式规则
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_RECORD_GAME_SCORE;
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_IMMEDIATE_WRITE_SCORE;

	//发送数据
	SendData(pIServerUserItem,MDM_GR_MANAGE,SUB_GR_OPTION_CURRENT,&OptionCurrent,sizeof(OptionCurrent));

	return true;
}

//房间设置
bool CAttemperEngineSink::OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_ServerOption));
	if (wDataSize!=sizeof(CMD_GR_ServerOption)) return false;

	//变量定义
	CMD_GR_ServerOption * pServerOption=(CMD_GR_ServerOption *)pData;
	tagServerOptionInfo * pServerOptionInfo=&pServerOption->ServerOptionInfo;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//变量定义
	bool bModifyServer=false;

	//挂接节点
	if (m_pGameServiceOption->wNodeID!=pServerOptionInfo->wNodeID)
	{
		bModifyServer=true;
		m_pGameServiceOption->wNodeID=pServerOptionInfo->wNodeID;
	}

	//挂接类型
	if ((pServerOptionInfo->wKindID!=0)&&(m_pGameServiceOption->wKindID!=pServerOptionInfo->wKindID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wKindID=pServerOptionInfo->wKindID;
	}
	
	//挂接排序
	if ((pServerOptionInfo->wSortID!=0)&&(m_pGameServiceOption->wSortID!=pServerOptionInfo->wSortID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wSortID=pServerOptionInfo->wSortID;
	}

	//房间名字
	if ((pServerOptionInfo->szServerName[0]!=0)&&(lstrcmp(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName)!=0))
	{
		bModifyServer=true;
		lstrcpyn(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName,CountArray(m_pGameServiceOption->szServerName));
	}

	//税收配置
	m_pGameServiceOption->wRevenueRatio=pServerOptionInfo->wRevenueRatio;
	m_pGameServiceOption->lServiceScore=pServerOptionInfo->lServiceScore;

	//房间配置
	m_pGameServiceOption->lRestrictScore=pServerOptionInfo->lRestrictScore;
	m_pGameServiceOption->lMinTableScore=pServerOptionInfo->lMinTableScore;
	m_pGameServiceOption->lMinEnterScore=pServerOptionInfo->lMinEnterScore;
	m_pGameServiceOption->lMaxEnterScore=pServerOptionInfo->lMaxEnterScore;

	//会员限制
	m_pGameServiceOption->cbMinEnterMember=pServerOptionInfo->cbMinEnterMember;
	m_pGameServiceOption->cbMaxEnterMember=pServerOptionInfo->cbMaxEnterMember;

	//聊天规则
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendRoomChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperOnGame(pServerOptionInfo->dwServerRule));

	//房间规则
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameLookon(pServerOptionInfo->dwServerRule));

	//银行规则
	CServerRule::SetForfendTakeInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendTakeInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInGame(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInGame(pServerOptionInfo->dwServerRule));

	//其他规则
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameTrack(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameRule(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendLockTable(pServerOptionInfo->dwServerRule));

	//动态加入
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowDynamicJoin(pServerOptionInfo->dwServerRule));
	}

	//机器管理
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowAndroidAttend(pServerOptionInfo->dwServerRule));
	}

	//断线托管
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowOffLineTrustee(pServerOptionInfo->dwServerRule));
	}

	//记录成绩
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameScore(pServerOptionInfo->dwServerRule));
	}

	//立即写分
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,CServerRule::IsImmediateWriteScore(pServerOptionInfo->dwServerRule));
	}

	//调整参数
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->RectifyServiceParameter();

	//发送修改
	if (bModifyServer==true)
	{
		//变量定义
		CMD_CS_C_ServerModify ServerModify;
		ZeroMemory(&ServerModify,sizeof(ServerModify));

		//服务端口
		ServerModify.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//房间信息
		ServerModify.wKindID=m_pGameServiceOption->wKindID;
		ServerModify.wNodeID=m_pGameServiceOption->wNodeID;
		ServerModify.wSortID=m_pGameServiceOption->wSortID;
		ServerModify.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
		ServerModify.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerModify.dwAndroidCount;
		ServerModify.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		lstrcpyn(ServerModify.szServerName,m_pGameServiceOption->szServerName,CountArray(ServerModify.szServerName));
		lstrcpyn(ServerModify.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(ServerModify.szServerAddr));

		//发送数据
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_MODIFY,&ServerModify,sizeof(ServerModify));
	}

	//发送信息
	SendRoomMessage(pIServerUserItem,TEXT("当前游戏服务器房间的“运行值”状态配置数据修改成功"),SMT_CHAT|SMT_EJECT);

	//输出信息
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("远程修改房间配置通知 管理员 %s [ %ld ]"),pIServerUserItem->GetNickName(),pIServerUserItem->GetUserID());

	//输出信息
	CTraceService::TraceString(szBuffer,TraceLevel_Info);

	return true;
}

//踢出用户
bool CAttemperEngineSink::OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//权限判断
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//目标用户
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//踢出记录
	m_KickUserItemMap[pITargetUserItem->GetUserID()]=(DWORD)time(NULL);

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//发送消息
		SendGameMessage(pITargetUserItem,TEXT("你已被管理员请离桌子！"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	//发送通知
	LPCTSTR pszMessage=TEXT("你已被管理员请离此游戏房间！");
	SendRoomMessage(pITargetUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

	pITargetUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//限制聊天
bool CAttemperEngineSink::OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
	if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

	//消息处理
	CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//目标用户
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
	if (pITargerUserItem==NULL) return true;

	//变量定义
	DWORD dwAddRight = 0, dwRemoveRight = 0;

	//大厅聊天
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
	{
		if (CMasterRight::CanLimitRoomChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_ROOM_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
	}

	//游戏聊天
	if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
	{
		if (CMasterRight::CanLimitGameChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_GAME_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_GAME_CHAT;
	}

	//大厅私聊
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
	{
		if (CMasterRight::CanLimitWisper(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_WISPER;
		else
			dwRemoveRight |= UR_CANNOT_WISPER;
	}

	//发送喇叭
	if(pLimitUserChat->cbLimitFlags==OSF_SEND_BUGLE)
	{
		if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

		if(pLimitUserChat->cbLimitValue == TRUE)
			dwAddRight |= UR_CANNOT_BUGLE;
		else
			dwRemoveRight |= UR_CANNOT_BUGLE;
	}

	if( dwAddRight != 0 || dwRemoveRight != 0 )
	{
		pITargerUserItem->ModifyUserRight(dwAddRight,dwRemoveRight);

		//发送通知
		CMD_GR_ConfigUserRight cur = {0};
		cur.dwUserRight = pITargerUserItem->GetUserRight();

		SendData( pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("用户聊天权限配置成功！"),SMT_CHAT);
	}
	else return false;

	return true;
}

//踢出所有用户
bool CAttemperEngineSink::OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//消息处理
	CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;

	//效验数据
	ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
	if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
	ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
	if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//解散所有游戏
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//tagBindParameter *pBindParameter = m_pNormalParameter;
	//for( INT i = 0; i < m_pGameServiceOption->wMaxPlayer; i++ )
	//{
	//	//目录用户
	//	IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
	//	if (pITargerUserItem==NULL || pITargerUserItem==pIServerUserItem ) 
	//	{
	//		pBindParameter++;
	//		continue;
	//	}

	//	//发送消息
	//	SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

	//	pBindParameter++;
	//} 

	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	while(pITargerUserItem!=NULL)
	{
		if(pITargerUserItem!=pIServerUserItem)
		{
			//发送消息
			SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);
		}

		//枚举用户
		pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	}

	return true;
}

//发布消息
bool CAttemperEngineSink::OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

	//变量定义
	CMD_GR_SendMessage *pSysMessage = (CMD_GR_SendMessage *)pData;

	if(pSysMessage->cbLoop==TRUE)
	{
		//定于变量
		tagSystemMessage  *pSendMessage=new tagSystemMessage;
		ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

		//构造消息
		pSendMessage->dwLastTime=(DWORD)time(NULL);
		pSendMessage->SystemMessage.dwMessageID=TEMP_MESSAGE_ID;
		pSendMessage->SystemMessage.cbAllRoom=pSysMessage->cbAllRoom;
		if(pSysMessage->cbRoom==TRUE && pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=3;
		else if(pSysMessage->cbRoom==TRUE)
			pSendMessage->SystemMessage.cbMessageType=2;
		else if(pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=1;
		pSendMessage->SystemMessage.dwTimeRate=pSysMessage->dwTimeRate;
		pSendMessage->SystemMessage.tStartTime=(DWORD)time(NULL);
		pSendMessage->SystemMessage.tConcludeTime=pSysMessage->tConcludeTime;
		lstrcpyn(pSendMessage->SystemMessage.szSystemMessage, pSysMessage->szSystemMessage, CountArray(pSendMessage->SystemMessage.szSystemMessage));

		//记录消息
		m_SystemMessageActive.Add(pSendMessage);
	}


	//消息处理
	return SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);
}

//解散游戏
bool CAttemperEngineSink::OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
	if (wDataSize!=sizeof(CMD_GR_DismissGame)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

	//消息处理
	CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;

	//效验数据
	if(pDismissGame->wDismissTableNum >= m_TableFrameArray.GetCount()) return true;

	//解散游戏
	CTableFrame *pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
	if(pTableFrame)
	{
		if(pTableFrame->IsGameStarted()) pTableFrame->DismissGame();
		pTableFrame->SendGameMessage(TEXT("抱歉，此桌已被管理员解散游戏"),SMT_EJECT);
	}

	return true;
}

//警告命令
bool CAttemperEngineSink::OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_SendWarning));
	if (wDataSize!=sizeof(CMD_GR_SendWarning)) return false;

	//提取数据
	CMD_GR_SendWarning * pSendWarning = (CMD_GR_SendWarning*)pData;
	ASSERT(pSendWarning!=NULL);

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pSendWarning->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//权限判断
	ASSERT(CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==false) return false;

	//权限判断
	ASSERT(pIServerUserItem->GetMasterOrder()>pITargetUserItem->GetMasterOrder());
	if(pIServerUserItem->GetMasterOrder()<pITargetUserItem->GetMasterOrder()) return false;

	//发送警告
	TCHAR szMessage[128]=TEXT("");
	_sntprintf(szMessage,CountArray(szMessage),TEXT("警告：%s"),pSendWarning->szWarningMessage);
	SendRoomMessage(pITargetUserItem,szMessage,SMT_EJECT);

	return true;
}

//创建桌子
bool CAttemperEngineSink::OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_CreateTable));
	if (wDataSize!=sizeof(CMD_GR_CreateTable)) return false;

	//提取数据
	CMD_GR_CreateTable * pCreateTable = (CMD_GR_CreateTable*)pData;
	ASSERT(pCreateTable!=NULL);

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	tagBindParameter* pBindParameter = GetBindParameter(wBindIndex);


	//寻找空闲桌子
	INT_PTR nSize = m_TableFrameArray.GetCount();

	for(INT_PTR i = 0; i < nSize; ++i)
	{
		CTableFrame* pTableFrame = m_TableFrameArray.GetAt(i);
		if(pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalTableLocked() == false)
		{
			//锁定桌子
			pTableFrame->SetPersonalTableLlocked(true);

			//桌子号
			DWORD dwTableID = pTableFrame->GetTableID();

			//构造数据
			DBR_GR_CreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(DBR_GR_CreateTable));

			CreateTable.dwUserID = pIServerUserItem->GetUserID();
			CreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CreateTable.dwServerID = m_pGameServiceOption->wServerID;
			CreateTable.dwTableID = dwTableID;
			CreateTable.dwDrawCountLimit = pCreateTable->dwDrawCountLimit;
			CreateTable.dwDrawTimeLimit = pCreateTable->dwDrawTimeLimit;
			CreateTable.lCellScore = pCreateTable->lCellScore;
			CreateTable.dwRoomTax = pCreateTable->dwRoomTax;
			CreateTable.wJoinGamePeopleCount = pCreateTable->wJoinGamePeopleCount;
			lstrcpyn(CreateTable.szPassword, pCreateTable->szPassword, CountArray(CreateTable.szPassword));
			memcpy(CreateTable.cbGameRule, pCreateTable->cbGameRule,  CountArray(CreateTable.cbGameRule));

			//投递数据
			m_pIDBCorrespondManager->PostDataBaseRequest(CreateTable.dwUserID, DBR_GR_CREATE_TABLE, dwSocketID, &CreateTable, sizeof(DBR_GR_CreateTable));

			return true;
		}
	}

	//构造数据
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = 1;
	lstrcpyn(CreateFailure.szDescribeString, TEXT("目前该游戏约战房间已满，请稍后再试！"), CountArray(CreateFailure.szDescribeString));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}
//取消请求
bool CAttemperEngineSink::OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_GR_CancelRequest));
	if(wDataSize != sizeof(CMD_GR_CancelRequest)) return false;

	//提取数据
	CMD_GR_CancelRequest * pCancelRequest = (CMD_GR_CancelRequest*)pData;
	ASSERT(pCancelRequest!=NULL);
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelRequest->dwTableID];
	ASSERT(pTableFrame != NULL);
	//if(pCancelRequest->dwUserID != pTableFrame->GetTableOwner()->GetUserID()) return false;

	if(pTableFrame->CancelTableRequest(pCancelRequest->dwUserID, pCancelRequest->dwChairID) == false) return false;

	//转发数据
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//过滤用户

		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_REQUEST, pData, wDataSize);
	}

	return true;
}
//取消答复
bool CAttemperEngineSink::OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_GR_RequestReply));
	if(wDataSize != sizeof(CMD_GR_RequestReply)) return false;

	//提取数据
	CMD_GR_RequestReply * pRequestReply = (CMD_GR_RequestReply*)pData;
	ASSERT(pRequestReply!=NULL);

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame* pTableFrame = m_TableFrameArray[pRequestReply->dwTableID];
	ASSERT(pTableFrame != NULL);
	//if(pRequestReply->dwUserID == pTableFrame->GetTableOwner()->GetUserID()) return false;

	//转发数据
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//过滤用户
		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, pData, wDataSize);
	}

	if(pTableFrame->CancelTableRequestReply(pRequestReply->dwUserID, pRequestReply->cbAgree) == false) return false;

	return true;
}

//取消请求
bool CAttemperEngineSink::OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_GR_HostDissumeGame));
	if(wDataSize != sizeof(CMD_GR_HostDissumeGame)) return false;

	//提取数据
	CMD_GR_HostDissumeGame * pCancelRequest = (CMD_GR_HostDissumeGame*)pData;
	ASSERT(pCancelRequest!=NULL);
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelRequest->dwTableID];
	ASSERT(pTableFrame != NULL);

	//如果强制解散的玩家不是房主，不允许解散
	if (pTableFrame->GetTableOwner() != pCancelRequest->dwUserID)
	{
		return true;
	}

	//投递数据
	DBR_GR_CancelCreateTable CancelCreateTable;
	ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

	CancelCreateTable.dwUserID = pTableFrame->GetTableOwner();
	CancelCreateTable.dwReason = 3;
	CancelCreateTable.dwDrawCountLimit = pTableFrame->GetPersonalTableParameter().dwPlayTurnCount;
	CancelCreateTable.dwDrawTimeLimit = pTableFrame->GetPersonalTableParameter().dwPlayTimeLimit;
	CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;
	lstrcpyn(CancelCreateTable.szRoomID,  pTableFrame->GetPersonalTableID(), sizeof(CancelCreateTable.szRoomID) );



	m_pIDBCorrespondManager->PostDataBaseRequest(CancelCreateTable.dwUserID, DBR_GR_HOST_CANCEL_CREATE_TABLE, dwSocketID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));


	pTableFrame->HostDissumeGame(true);

	if(pTableFrame)
	{
		pTableFrame->SetPersonalTableLlocked(false);
	}


	//发送数据
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));
	DismissTable.dwSocketID = dwSocketID;
	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID = pTableFrame->GetTableID();

	//发送消息
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));


	return true;
}

//玩家请求房间成绩
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	////OutputDebugString(TEXT("ptdt *** OnTCPNetworkSubHostDissumeTable"));
	////校验数据
	//ASSERT(wDataSize == sizeof(CMD_GR_QUERY_USER_ROOM_SCORE));
	//if(wDataSize != sizeof(CMD_GR_QUERY_USER_ROOM_SCORE)) return false;

	////提取数据
	//CMD_GR_QUERY_USER_ROOM_SCORE * pQueryUserRoomScore = (CMD_GR_QUERY_USER_ROOM_SCORE*)pData;
	//ASSERT(pQueryUserRoomScore!=NULL);
	////获取用户
	//WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//if (pIServerUserItem==NULL) return false;

	////发送数据
	//DBR_GR_QUERY_USER_ROOM_INFO QUERY_USER_ROOM_INFO;
	//ZeroMemory(&QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));
	//QUERY_USER_ROOM_INFO.dwUserID = pQueryUserRoomScore->dwUserID;//请求房间成绩的玩家

	////投递数据
	//m_pIDBCorrespondManager->PostDataBaseRequest(QUERY_USER_ROOM_INFO.dwUserID, DBR_GR_QUERY_USER_ROOM_SCORE, dwSocketID, &QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));

	return true;
}


//用户登录
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//获取参数
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bAndroidUser=pIServerUserItem->IsAndroidUser();
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//登录处理
	if (pIServerUserItem->IsMobileUser()==false)
	{
		//变量定义
		CMD_GR_LogonSuccess LogonSuccess;
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));

		//登录成功
		LogonSuccess.dwUserRight=pIServerUserItem->GetUserRight();
		LogonSuccess.dwMasterRight=pIServerUserItem->GetMasterRight();
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

		//登录通知
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

		//房间配置
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

		//列表配置
		WORD wConfigColumnHead=sizeof(m_DataConfigColumn)-sizeof(m_DataConfigColumn.ColumnItem);
		WORD wConfigColumnInfo=m_DataConfigColumn.cbColumnCount*sizeof(m_DataConfigColumn.ColumnItem[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_COLUMN,&m_DataConfigColumn,wConfigColumnHead+wConfigColumnInfo);


		//配置完成
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//发送自己信息给自己
		SendUserInfoPacket(pIServerUserItem,pBindParameter->dwSocketID);

		//变量定义		
		WORD wUserIndex=0;
		bool bSendAndroidFlag=false;
		IServerUserItem * pIServerUserItemSend=NULL;

		//权限判断		
		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
		   CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
		{
			bSendAndroidFlag=true;
		}

		//发送其他已在线用户给自己（除自己以外）		
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend==pIServerUserItem) continue;
			SendUserInfoPacket(pIServerUserItemSend,pBindParameter->dwSocketID,bSendAndroidFlag);
		}

		//桌子状态
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].lCellScore=pTableFrame->GetCellScore();
		}

		//桌子状态
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//发送自己的信息给其他在线用户
		if (bAlreadyOnLine==false)
		{
			//自己是机器人
			if (pIServerUserItem->IsAndroidUser()==true)
			{
				//设置变量
				wUserIndex=0;
				while (true)
				{
					pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
					if (pIServerUserItemSend==NULL) break;
					if (pIServerUserItemSend==pIServerUserItem) continue;

					//设置权限
					bSendAndroidFlag=false;
					if(CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight())==true ||
					   CMasterRight::CanManagerAndroid(pIServerUserItemSend->GetMasterRight())==true )
					{
						bSendAndroidFlag=true;
					}

					//获取参数
					WORD wBindIndexSend=pIServerUserItemSend->GetBindIndex();
					tagBindParameter * pBindParameterSend=GetBindParameter(wBindIndexSend);
					if (pBindParameterSend!=NULL)
					{
						//发送信息
						SendUserInfoPacket(pIServerUserItem,pBindParameterSend->dwSocketID,bSendAndroidFlag);
					}
				}
			}
			else
			{
				SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);
			}
		}

		//构造结构
		CMD_GR_LogonFinish LogonFinish;
		bool bHasTaskParameter=m_UserTaskManager.GetTaskParameterCount()>0;
		bool bHasUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID())!=NULL;
		LogonFinish.bGuideTask = bHasTaskParameter&&!bHasUserTaskEntry;

		//登录完成
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish));

		//登录完成
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogonFinish(pIServerUserItem);

		//欢迎消息
		if (bAndroidUser==false)
		{
			//构造提示
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("欢迎您进入“%s”游戏房间，祝您游戏愉快！"),m_pGameServiceAttrib->szGameName);

			//发送消息
			SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT);
		}
	}
	else
	{
		//登录通知
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

		//变量定义
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));		

		//房间配置
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));		

		//配置完成
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//发送自己信息给自己
		SendUserInfoPacket(pIServerUserItem,pBindParameter->dwSocketID);

		//变量定义		
		WORD wUserIndex=0;
		bool bSendAndroidFlag=false;
		IServerUserItem * pIServerUserItemSend=NULL;

		//权限判断		
		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
			CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
		{
			bSendAndroidFlag=true;
		}

		//发送其他已在线用户给自己（除自己以外）	
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend==pIServerUserItem) continue;
			SendUserInfoPacket(pIServerUserItemSend,pBindParameter->dwSocketID,bSendAndroidFlag);
		}

		//发送自己的信息给其他在线用户
		SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);

		//登录完成
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);

		//旁观用户
		if (pIServerUserItem->GetUserStatus()==US_LOOKON)
		{
			m_TableFrameArray[pIServerUserItem->GetTableID()]->PerformStandUpAction(pIServerUserItem);
		}

		//桌子状态
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].lCellScore=pTableFrame->GetCellScore();
		}

		//桌子状态
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//立即登录
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			WORD wMobileUserRule =  pIServerUserItem->GetMobileUserRule();
			if((wMobileUserRule&BEHAVIOR_LOGON_IMMEDIATELY)!=0)
			{
				MobileUserImmediately(pIServerUserItem);
			}
			//else
			//{
			//	SendViewTableUserInfoPacketToMobileUser(pIServerUserItem,INVALID_WORD);
			//}
		}

		//登录完成
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogonFinish(pIServerUserItem);
	}

	//网络设置
	if (bAndroidUser==false)
	{
		if (pBindParameter->cbClientKind==CLIENT_KIND_MOBILE)
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_MOBILE);
		}
		else
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_COMPUTER);
		}
	}

	//登录通知
	if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

	return;
}

//用户离开
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//提取成绩
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

    //用户信息
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//调整分数
	if(pIServerUserItem->IsAndroidUser()==true)
	{
		LeaveGameServer.VariationInfo.lScore=0;
		LeaveGameServer.VariationInfo.lGrade=0;
		LeaveGameServer.VariationInfo.lInsure=0;
		LeaveGameServer.VariationInfo.lRevenue=0;
	}

	//连接信息
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//是否为约战类型
	if(m_bHasPersonalRoomService)
	{
		LeaveGameServer.cbIsPersonalRoom = 1;
	}
	else
	{
		LeaveGameServer.cbIsPersonalRoom = 0;
	}

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//查找对象
	tagUserTaskEntry * pUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(),TASK_STATUS_UNFINISH|TASK_STATUS_FAILED); 

	//推进任务
	if(pUserTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem,pUserTaskEntry,LeaveGameServer.VariationInfo.dwWinCount,LeaveGameServer.VariationInfo.dwLostCount,LeaveGameServer.VariationInfo.dwDrawCount);
	}
	
	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//设置变量
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//发送消息
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//玩家退赛
	if (m_pIMatchServiceManager!=NULL) 
	{
		m_pIMatchServiceManager->OnEventUserQuitMatch(pIServerUserItem, 0);
	}

	//删除分组
	DeleteDistribute(pIServerUserItem);

	//移除任务
	m_UserTaskManager.RemoveUserTask(pIServerUserItem->GetUserID());

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	//登出通知
	if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogout(pIServerUserItem);

	return;
}

//解锁游戏币
bool CAttemperEngineSink::PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//设置变量
	LeaveGameServer.dwUserID=dwUserID;
	LeaveGameServer.dwInoutIndex=dwInoutIndex;
	LeaveGameServer.dwLeaveReason=dwLeaveReason;

	//是否为约战类型
	if(m_bHasPersonalRoomService)
	{
		LeaveGameServer.cbIsPersonalRoom = 1;
	}
	else
	{
		LeaveGameServer.cbIsPersonalRoom = 0;
	}
	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return true;
}

//推进任务
bool CAttemperEngineSink::PerformTaskProgress(IServerUserItem * pIServerUserItem,tagUserTaskEntry * pUserTaskEntry,DWORD dwWinCount,DWORD dwLostCount,DWORD dwDrawCount)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL && pUserTaskEntry!=NULL);
	if(pIServerUserItem==NULL || pUserTaskEntry==NULL) return false;

	//状态校验
	if(pUserTaskEntry->pTaskParameter==NULL) return true;
	if(pUserTaskEntry->cbTaskStatus!=TASK_STATUS_UNFINISH) return true;
	if(pUserTaskEntry->pTaskParameter->wKindID!=m_pGameServiceOption->wKindID) return true;

	//任务参数
	tagTaskParameter * pTaskParameter = pUserTaskEntry->pTaskParameter;

	//获取时间
	DWORD dwSpanSecond=(DWORD)time(NULL)-pUserTaskEntry->dwLastUpdateTime;
	dwSpanSecond=__min(dwSpanSecond,pUserTaskEntry->dwResidueTime);

	//更新时间	
	pUserTaskEntry->dwResidueTime-=dwSpanSecond;
	pUserTaskEntry->dwLastUpdateTime=(DWORD)time(NULL);

	//任务超时
	if(pUserTaskEntry->dwResidueTime == 0)
	{		
		pUserTaskEntry->wTaskProgress=0;
		pUserTaskEntry->pTaskParameter=NULL;		
		pUserTaskEntry->cbTaskStatus=TASK_STATUS_FAILED;		
	}

	//任务未完成
	if(pUserTaskEntry->cbTaskStatus==TASK_STATUS_UNFINISH)
	{
		//任务类型
		switch(pTaskParameter->wTaskType)
		{
		case TASK_TYPE_WIN_INNINGS:			//赢局任务
			{
				//更新状态
				pUserTaskEntry->wTaskProgress += (WORD)dwWinCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}

				break;
			};
		case TASK_TYPE_SUM_INNINGS:			//总局任务
			{
				//更新状态
				pUserTaskEntry->wTaskProgress += (WORD)dwWinCount+(WORD)dwLostCount+(WORD)dwDrawCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}
				break;
			};
		case TASK_TYPE_FIRST_WIN:			//首胜任务
			{
				//更新状态
				pUserTaskEntry->wTaskProgress = (WORD)dwWinCount;
				if(pUserTaskEntry->wTaskProgress==pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}
				else
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_FAILED;
				}

				break;
			};
		case TASK_TYPE_KEEP_WIN:			//连赢任务
			{

				if (dwLostCount>0 || dwDrawCount>0)
				{
					pUserTaskEntry->wTaskProgress=0;
				}
				else
				{
					//更新状态
					pUserTaskEntry->wTaskProgress += (WORD)dwWinCount;
					if(pUserTaskEntry->wTaskProgress==pTaskParameter->wTaskObject)
					{
						pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
					}
				}

				break;
			};
		default:
			{
				ASSERT(FALSE);
				break;
			};
		}
	}

	//任务完成
	if(pUserTaskEntry->cbTaskStatus==TASK_STATUS_SUCCESS)
	{
		//构造结构
		CMD_GR_S_TaskFinish TaskFinish;
		TaskFinish.wFinishTaskID=pTaskParameter->wTaskID;
		lstrcpyn(TaskFinish.szTaskName,pTaskParameter->szTaskName,CountArray(TaskFinish.szTaskName));

		//发送消息
		WORD wHeadSize = sizeof(TaskFinish)-sizeof(TaskFinish.szTaskName);
		WORD wDataSize = wHeadSize + CountStringBuffer(TaskFinish.szTaskName);
		SendData(pIServerUserItem,MDM_GR_TASK,SUB_GR_TASK_FINISH,&TaskFinish,wDataSize);
	}

	//递归调用
	if(pUserTaskEntry->pNextTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem,pUserTaskEntry->pNextStatusEntry,dwWinCount,dwLostCount,dwDrawCount);
	}

	return true;
}

//版本检查
bool CAttemperEngineSink::PerformCheckVersion(DWORD dwPlazaVersion, DWORD dwFrameVersion, DWORD dwClientVersion, DWORD dwSocketID)
{
	//变量定义
	bool bMustUpdateClient=false;
	bool bAdviceUpdateClient=false;

	//游戏版本
	if (VERSION_EFFICACY==TRUE)
	{
		if (GetSubVer(dwClientVersion)<GetSubVer(m_pGameServiceAttrib->dwClientVersion)) bAdviceUpdateClient=true;
		if (GetMainVer(dwClientVersion)!=GetMainVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;
		if (GetProductVer(dwClientVersion)!=GetProductVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;
	}
	else
	{
		if (GetSubVer(dwClientVersion)<GetSubVer(m_pGameParameter->dwClientVersion)) bAdviceUpdateClient=true;
		if (GetMainVer(dwClientVersion)!=GetMainVer(m_pGameParameter->dwClientVersion)) bMustUpdateClient=true;
		if (GetProductVer(dwClientVersion)!=GetProductVer(m_pGameParameter->dwClientVersion)) bMustUpdateClient=true;
	}

	//更新通知
	if ((bMustUpdateClient==true)||(bAdviceUpdateClient==true))
	{
		//变量定义
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//变量定义
		UpdateNotify.cbMustUpdatePlaza=false;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;
		UpdateNotify.cbAdviceUpdateClient=bAdviceUpdateClient;

		//当前版本
		UpdateNotify.dwCurrentPlazaVersion=VERSION_PLAZA;
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

		//发送消息
		SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//中止判断
		if (bMustUpdateClient==true)
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return false;
		}
	}

	return true;
}

//切换连接
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD)) return false;

	//断开用户
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//发送通知
		LPCTSTR pszMessage=TEXT("请注意，您的帐号在另一地方进入了此游戏房间，您被迫离开！");
		SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

		//绑定参数
		WORD wSourceIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pSourceParameter=GetBindParameter(wSourceIndex);

		//解除绑定
		ASSERT((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem));
		if ((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem)) pSourceParameter->pIServerUserItem=NULL;

		//断开用户
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID,false);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//机器判断
	LPCTSTR pszMachineID=pIServerUserItem->GetMachineID();
	bool bSameMachineID=(lstrcmp(pszMachineID,szMachineID)==0);

	//变量定义
	bool bAndroidUser=(wTargetIndex>=INDEX_ANDROID);
	tagBindParameter * pTargetParameter=GetBindParameter(wTargetIndex);

	//激活用户
	pTargetParameter->pIServerUserItem=pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr,wTargetIndex,szMachineID,bAndroidUser,false);

	//手机标识
	if(pTargetParameter->cbClientKind==CLIENT_KIND_MOBILE)
	{
		pIServerUserItem->SetMobileUser(true);
		SetMobileUserParameter(pIServerUserItem,cbDeviceType,wBehaviorFlags,wPageTableCount);
	}

	//状态切换
	bool bIsOffLine=false;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		//变量定义
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();

		//设置状态
		bIsOffLine=true;
		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
			if(pTableFrame->IsGameStarted())
				pIServerUserItem->SetUserStatus(US_PLAYING,wTableID,wChairID);
			else
				pIServerUserItem->SetUserStatus(US_SIT,wTableID,wChairID);
		}
		else
			pIServerUserItem->SetUserStatus(US_PLAYING,wTableID,wChairID);
	}

	//取消托管
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		if (pIServerUserItem->IsTrusteeUser()==true && pIServerUserItem->GetUserStatus()!=US_PLAYING)
		{
			pIServerUserItem->SetTrusteeUser(false);
		}
	}

	//登录事件
	OnEventUserLogon(pIServerUserItem,true);

	//安全提示
	if ((bAndroidUser==false)&&(bIsOffLine==false)&&(bSameMachineID==false))
	{
		SendRoomMessage(pIServerUserItem,TEXT("请注意，您的帐号在另一地方进入了此游戏房间，对方被迫离开！"),SMT_EJECT|SMT_CHAT|SMT_GLOBAL);
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//构造数据
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//发送数据
	SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//银行失败
bool CAttemperEngineSink::SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszString, LONG lErrorCode,BYTE cbActivityGame)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_S_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//构造数据
	UserInsureFailure.cbActivityGame=cbActivityGame;
	UserInsureFailure.lErrorCode=lErrorCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszString,CountArray(UserInsureFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);

	//发送数据
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDataSize);

	return true;
}

//请求失败
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//设置变量
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	//WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	//WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	//SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,sizeof(RequestFailure));
	return true;
}

//道具失败
bool CAttemperEngineSink::SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea)
{
	//变量定义
	CMD_GR_PropertyFailure PropertyFailure;
	ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

	//设置变量
	PropertyFailure.lErrorCode=lErrorCode;
	PropertyFailure.wRequestArea=wRequestArea;
	lstrcpyn(PropertyFailure.szDescribeString,pszDescribe,CountArray(PropertyFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
	WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_PROPERTY_FAILURE,&PropertyFailure,wHeadSize+wDataSize);

	return true;
}

//发送用户
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID,bool bSendAndroidFalg)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));

	//用户属性
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwGroupID=pUserInfo->dwGroupID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//用户属性	
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserInfo->cbMasterOrder;
	pUserInfoHead->bIsAndroid=pIServerUserItem->IsAndroidUser()&&bSendAndroidFalg;

	//用户状态
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//用户局数
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lLoveLiness=pUserInfo->lLoveLiness;
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

	//用户积分
	pUserInfoHead->lGrade=pUserInfo->lGrade;
	pUserInfoHead->lInsure=pUserInfo->lInsure;
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	//用户成绩
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();

	//lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//叠加信息
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	SendPacket.AddPacket(pUserInfo->szGroupName,DTP_GR_GROUP_NAME);
	SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);
	SendPacket.AddPacket(pUserInfo->szQQ,DTP_GR_QQ);
	SendPacket.AddPacket(pUserInfo->szMobilePhone,DTP_GR_MOBILE_PHONE);

	//发送数据
	if (dwSocketID==INVALID_DWORD)
	{
		WORD wHeadSize=sizeof(tagUserInfoHead);
		SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
		SendUserInfoPacketBatchToMobileUser(pIServerUserItem,INVALID_DWORD);
	}
	else
	{
		//获取用户
		WORD wBindIndex=LOWORD(dwSocketID);
		IServerUserItem * pITargetUserItem=GetBindUserItem(wBindIndex);
		ASSERT(pITargetUserItem!=NULL);

		if (pITargetUserItem->IsMobileUser()==false)
		{
			WORD wHeadSize=sizeof(tagUserInfoHead);
			SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
		}
		else
		{
			SendUserInfoPacketBatchToMobileUser(pIServerUserItem,dwSocketID);
		}
	}

	return true;
}


//广播道具
bool CAttemperEngineSink::SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount)
{
	//构造结构
	CMD_GR_S_PropertyMessage  PropertyMessage;
	PropertyMessage.wPropertyIndex=wPropertyIndex;
	PropertyMessage.dwSourceUserID=dwSourceID;
	PropertyMessage.dwTargerUserID=dwTargerID;
	PropertyMessage.wPropertyCount=wPropertyCount;

	//在线用户
	WORD wUserIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendData(pIServerUserItemSend,MDM_GR_PROPERTY,SUB_GR_PROPERTY_MESSAGE,&PropertyMessage,sizeof(PropertyMessage));
	}

	return true;
}

//道具效应
bool CAttemperEngineSink::SendPropertyEffect(IServerUserItem * pIServerUserItem, WORD wPropKind)
{
	//参数校验
	if(pIServerUserItem==NULL) return false;

	//构造结构
	CMD_GR_S_PropertyEffect  PropertyEffect={0};
	PropertyEffect.wPropKind = (BYTE)wPropKind;
	PropertyEffect.wUserID = pIServerUserItem->GetUserID();
	PropertyEffect.cbMemberOrder=pIServerUserItem->GetMemberOrder();
	PropertyEffect.dwFleeCount = pIServerUserItem->GetUserInfo()->dwFleeCount;
	PropertyEffect.lScore = pIServerUserItem->GetUserInfo()->lScore;

	//在线用户
	WORD wUserIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendData(pIServerUserItemSend,MDM_GR_PROPERTY,SUB_GR_PROPERTY_EFFECT,&PropertyEffect,sizeof(PropertyEffect));
	}

	return true;
}

//道具事件
bool CAttemperEngineSink::OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem)
{
	//目标玩家
	if ( pTargetUserItem == NULL )
	{
		//发送消息
		SendPropertyFailure(pISourceUserItem,TEXT("赠送失败，您要赠送的玩家已经离开！"), 0L,cbRequestArea);

		return false;
	}

	//房间判断
	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH ) && 
		(wPropertyIndex== PROPERTY_ID_SCORE_CLEAR||wPropertyIndex==PROPERTY_ID_TWO_CARD||wPropertyIndex == PROPERTY_ID_FOUR_CARD||wPropertyIndex == PROPERTY_ID_POSSESS) )
	{
		//发送消息
		SendPropertyFailure(pISourceUserItem,TEXT("此房间不可以使用此道具,购买失败"), 0L,cbRequestArea);

		return false;
	}

	//查找道具
	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(wPropertyIndex);

	//有效效验
	if(pPropertyInfo==NULL)
	{
		//发送消息
		SendPropertyFailure(pISourceUserItem,TEXT("此道具还未启用,购买失败！"), 0L,cbRequestArea);

		return false;
	}

	//自己使用
    if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_MESELF)==0 && pISourceUserItem==pTargetUserItem) 
	{
		//发送消息
		SendPropertyFailure(pISourceUserItem,TEXT("此道具不可自己使用,购买失败！"), 0L,cbRequestArea);

		return false;
	}

	//玩家使用
	if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_PLAYER)==0 && pISourceUserItem!=pTargetUserItem) 
	{
		//发送消息
		SendPropertyFailure(pISourceUserItem,TEXT("此道具不可赠送给玩家,只能自己使用,购买失败！"), 0L,cbRequestArea);

		return false;
	}

	//旁观范围
	if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_LOOKON)==0)  
	{
		//变量定义
		WORD wTableID = pTargetUserItem->GetTableID();
		if(wTableID!=INVALID_TABLE)
		{
			//变量定义
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//获取桌子
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	
			//枚举用户
			do
			{
				//获取用户
				pIServerUserItem=pTableFrame->EnumLookonUserItem(wEnumIndex++);
				if( pIServerUserItem==NULL) break;
				if( pIServerUserItem==pTargetUserItem )
				{
					//发送消息
					SendPropertyFailure(pISourceUserItem,TEXT("此道具不可赠送给旁观用户,购买失败！"), 0L,cbRequestArea);

					return false;
				}
			} while (true);
		}
	}

	//道具判断
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_SCORE_CLEAR :			//负分清零
		{
			//变量定义
			SCORE lCurrScore = pTargetUserItem->GetUserScore();
			if( lCurrScore >= 0)
			{
				//变量定义
				TCHAR szMessage[128]=TEXT("");
				if ( pISourceUserItem==pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您现在的积分已经是非负数，不需要使用负分清零道具！"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]现在的积分已经是非负数，不需要使用负分清零道具！"), pTargetUserItem->GetNickName());

				//发送消息
				SendPropertyFailure(pISourceUserItem,szMessage, 0L,cbRequestArea);

				return false;
			}
			break;
		}
	case PROPERTY_ID_ESCAPE_CLEAR :			 //逃跑清零
		{
			//变量定义
			DWORD dwCurrFleeCount = pTargetUserItem->GetUserInfo()->dwFleeCount;
			if ( dwCurrFleeCount==0 )
			{
				//变量定义
				TCHAR szMessage[128]=TEXT("");		
				if ( pISourceUserItem == pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您现在的逃跑率已经为0，不需要使用逃跑清零道具！"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]现在的逃跑率已经为0，不需要使用逃跑清零道具！"), pTargetUserItem->GetNickName());

				//发送消息
				SendPropertyFailure(pISourceUserItem,szMessage,0L,cbRequestArea);

				return false;
			}
			break;
		}
	}

	return true;
}

//绑定用户
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//获取参数
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//道具类型
WORD CAttemperEngineSink::GetPropertyType(WORD wPropertyIndex)
{
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_CAR:	case PROPERTY_ID_EGG: 	case PROPERTY_ID_CLAP: 	case PROPERTY_ID_KISS: 	case PROPERTY_ID_BEER:
	case PROPERTY_ID_CAKE: 	case PROPERTY_ID_RING:  case PROPERTY_ID_BEAT: 	case PROPERTY_ID_BOMB:  case PROPERTY_ID_SMOKE:
	case PROPERTY_ID_VILLA: case PROPERTY_ID_BRICK: case PROPERTY_ID_FLOWER: 
		{
			return PT_TYPE_PRESENT;
		};
    case PROPERTY_ID_TWO_CARD: 	case PROPERTY_ID_FOUR_CARD:  case PROPERTY_ID_SCORE_CLEAR:     case PROPERTY_ID_ESCAPE_CLEAR:
	case PROPERTY_ID_TRUMPET:	case PROPERTY_ID_TYPHON:     case PROPERTY_ID_GUARDKICK_CARD:  case PROPERTY_ID_POSSESS:
	case PROPERTY_ID_BLUERING_CARD: case PROPERTY_ID_YELLOWRING_CARD: case PROPERTY_ID_WHITERING_CARD: case PROPERTY_ID_REDRING_CARD:
	case PROPERTY_ID_VIPROOM_CARD: 
		{
			return PT_TYPE_PROPERTY;
		};
	}

	ASSERT(false);

	return PT_TYPE_ERROR;
}

//分配用户
bool CAttemperEngineSink::PerformDistribute()
{
	//人数校验
	if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinDistributeUser) return false;

	//分配用户
	while(true)
	{
		CString strMessage;
		TCHAR szMessage[128]={0};		

		try
		{
			//人数校验
			if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinPartakeGameUser) break;

			//变量定义
			CDistributeInfoArray DistributeInfoArray;

			strMessage.Append(TEXT("分配用户1\r\n"));

			//获取用户
			srand(GetTickCount());
			WORD wRandCount = __max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
			WORD wChairCount = m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
			WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
			if(wDistributeCount < wChairCount) break;

			strMessage.Append(TEXT("分配用户2\r\n"));

			//寻找位置
			CTableFrame * pCurrTableFrame=NULL;
			for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
			{
				//获取对象
				ASSERT(m_TableFrameArray[i]!=NULL);
				CTableFrame * pTableFrame=m_TableFrameArray[i];

				//状态判断
				if (pTableFrame->GetSitUserCount()==0)
				{
					pCurrTableFrame=pTableFrame;
					break;
				}
			}

			strMessage.Append(TEXT("分配用户3\r\n"));

			//桌子判断
			if(pCurrTableFrame==NULL) break;

			//玩家坐下
			bool bSitSuccess=true;
			INT_PTR nSitFailedIndex=INVALID_CHAIR;
			for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
			{
				//变量定义
				WORD wChairID=pCurrTableFrame->GetNullChairID();

				//分配用户
				if (wChairID!=INVALID_CHAIR)
				{
					//获取用户
					IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

					_sntprintf(szMessage,CountArray(szMessage),TEXT("分配用户4-%s\r\n"),pIServerUserItem->GetNickName());
					strMessage.Append(szMessage);

					//用户坐下
					if(pCurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
					{
						strMessage.Format(TEXT("坐下失败！"));
						bSitSuccess=false;
						nSitFailedIndex=nIndex;
						break;
					}					
				}
			}

			//坐下结果
			if(bSitSuccess)
			{
				strMessage.Append(TEXT("分配用户5"));

				//移除结点
				while(DistributeInfoArray.GetCount()>0)
				{
					m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[0].pPertainNode);
					DistributeInfoArray.RemoveAt(0);
				}

				strMessage.Append(TEXT("分配用户6"));
			}
			else
			{
				strMessage.Append(TEXT("分配用户7"));

				for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
				{
					if(nSitFailedIndex==nIndex)
					{
						m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
					}
					else
					{
						//用户起立
						if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
						{
							WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
							m_TableFrameArray[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
						}
					}
				}

				strMessage.Append(TEXT("分配用户8"));
			}	
		}catch(...)
		{
			CTraceService::TraceString(strMessage,TraceLevel_Exception);
		}
	}

	return true;
}

//配置机器
bool CAttemperEngineSink::InitAndroidUser()
{
	//机器参数
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//配置参数
	AndroidUserParameter.bServiceContinue=true;
	AndroidUserParameter.pGameParameter=m_pGameParameter;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;
	AndroidUserParameter.pGameMatchOption=m_pGameMatchOption;

	//服务组件
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pIGameServiceSustomTime=m_pIGameServiceSustomTime;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//服务配置
	if(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR)
	{
		AndroidUserParameter.bServiceContinue=false;
		AndroidUserParameter.dwMinSitInterval=MIN_INTERVAL_TIME;
		AndroidUserParameter.dwMaxSitInterval=MAX_INTERVAL_TIME;	
	}

	//设置对象
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//配置桌子
bool CAttemperEngineSink::InitTableFrameArray()
{
	//桌子参数
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//内核组件
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//服务组件
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;

	//配置参数
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameMatchOption=m_pGameMatchOption;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	//桌子容器
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(),m_pGameServiceOption->wTableCount*sizeof(CTableFrame *));

	//创建桌子
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//创建对象
		m_TableFrameArray[i]=new CTableFrame;

		//配置桌子
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		//绑定桌子
		if(m_pIMatchServiceManager!=NULL)
		{
			m_pIMatchServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}
	}	

	return true;
}

//配置比赛
bool CAttemperEngineSink::InitMatchServiceManager()
{
	//参数校验
	if(m_pIMatchServiceManager==NULL) return true;

	//桌子参数
	tagMatchManagerParameter MatchManagerParameter;
	ZeroMemory(&MatchManagerParameter,sizeof(MatchManagerParameter));

	//配置参数
	MatchManagerParameter.pGameMatchOption=m_pGameMatchOption;
	MatchManagerParameter.pGameServiceOption=m_pGameServiceOption;
	MatchManagerParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//内核组件
	MatchManagerParameter.pITimerEngine=m_pITimerEngine;
	MatchManagerParameter.pICorrespondManager=m_pIDBCorrespondManager;
	MatchManagerParameter.pTCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//服务组件
	MatchManagerParameter.pIMainServiceFrame=this;
	MatchManagerParameter.pIServerUserItemSink=this;
	MatchManagerParameter.pIAndroidUserManager=&m_AndroidUserManager;
	MatchManagerParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);		

	//初始化接口
	m_pIMatchServiceManager->InitMatchInterface(MatchManagerParameter);

	return true;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//断开协调
bool CAttemperEngineSink::DisconnectCorrespond()
{
	//设置变量
	m_bNeekCorrespond=false;

	//关闭连接
	if (m_pITCPSocketService!=NULL) m_pITCPSocketService->CloseSocket();

	return true;
}

//插入分配
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//状态判断
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//比赛类型
	if(m_pIMatchServiceManager!=NULL) 
	{
		//获取参数
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//加入比赛
		return m_pIMatchServiceManager->OnEventUserJoinMatch(pIServerUserItem,0,pBindParameter->dwSocketID);
	}

	//判断分数
	if (m_pGameServiceOption->lMinTableScore>0 && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore)
	{
		return false;
	}

	//变量定义
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//设置变量
	DistributeInfo.pIServerUserItem=pIServerUserItem;
	DistributeInfo.wLastTableID=pIServerUserItem->GetTableID();
	DistributeInfo.pPertainNode=NULL;

	//加入数组
	if(m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//用户起立
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
	}

	//发送通知
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	return true;
}

//敏感词过滤
void CAttemperEngineSink::SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	m_WordsFilter.Filtrate(pMsg,pszFiltered,nMaxLen);
}

//解锁机器人
VOID CAttemperEngineSink::UnLockAndroidUser(WORD wServerID, WORD wBatchID)
{
	//解锁机器
	DBR_GR_UnlockAndroidUser UnlockAndroidUser;
	UnlockAndroidUser.wServerID=wServerID;
	UnlockAndroidUser.wBatchID=wBatchID;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_ANDROID_USER,0,&UnlockAndroidUser,sizeof(UnlockAndroidUser));
}

//解散私人桌子
VOID CAttemperEngineSink::DismissPersonalTable(WORD wServerID, WORD wTableID)
{
	//解散桌子
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));

	DismissTable.dwServerID = wServerID;
	DismissTable.dwTableID = wTableID;

	//发送消息
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));

	CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
	if(pTableFrame)
	{
		pTableFrame->SetPersonalTableLlocked(false);
	}

	return;
}

//取消创建
VOID CAttemperEngineSink::CancelCreateTable(DWORD dwUserID, DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, DWORD dwReason, WORD wTableID, TCHAR * szRoomID)
{
	//退还费用
	DBR_GR_CancelCreateTable CancelCreateTable;
	ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

	CancelCreateTable.dwUserID = dwUserID;
	CancelCreateTable.dwReason = dwReason;
	CancelCreateTable.dwDrawCountLimit = dwDrawCountLimit;
	CancelCreateTable.dwDrawTimeLimit = dwDrawTimeLimit;
	CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;
	lstrcpyn(CancelCreateTable.szRoomID,  szRoomID, sizeof(CancelCreateTable.szRoomID) );

	IServerUserItem* pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	//ASSERT(pServerUserItem != NULL);

	if(pServerUserItem != NULL)
	{
		//判断在线
		DWORD dwSocketID;
		if(pServerUserItem->GetBindIndex() == INVALID_WORD)
		{
			dwSocketID = INVALID_DWORD;
		}
		else
		{
			tagBindParameter* pBindParameter = GetBindParameter(pServerUserItem->GetBindIndex());
			dwSocketID = pBindParameter->dwSocketID;
			CancelCreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CancelCreateTable.dwTableID = wTableID;
		}

		//投递数据
		m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwSocketID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));
	}
	else
	{
		//投递数据
		DBR_GR_CancelCreateTable CancelCreateTable;
		ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

		CancelCreateTable.dwUserID = dwUserID;
		CancelCreateTable.dwReason = CANCELTABLE_REASON_ENFOCE;
		CancelCreateTable.dwDrawCountLimit = dwDrawCountLimit;
		CancelCreateTable.dwDrawTimeLimit = dwDrawTimeLimit;
		CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;
		lstrcpyn(CancelCreateTable.szRoomID, szRoomID, sizeof(CancelCreateTable.szRoomID) );

		m_pIDBCorrespondManager->PostDataBaseRequest(CancelCreateTable.dwUserID, DBR_GR_HOST_CANCEL_CREATE_TABLE, 0, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));
	}

	//解散桌子
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));

	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID =wTableID;

	//发送消息
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));

	CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
	if(pTableFrame)
	{
			pTableFrame->SetPersonalTableLlocked(false);
			pTableFrame->SetPersonalTable(0, 0, 0);
			pTableFrame->SetTableOwner(0);
	}

	return;
}

//删除用户
bool CAttemperEngineSink::DeleteDistribute(IServerUserItem * pIServerUserItem)
{
	//移除节点
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}

//设置参数
void CAttemperEngineSink::SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if(wPageTableCount > m_pGameServiceOption->wTableCount)wPageTableCount=m_pGameServiceOption->wTableCount;
	pIServerUserItem->SetMobileUserRule(wBehaviorFlags);
}

//群发用户信息
bool CAttemperEngineSink::SendUserInfoPacketBatchToMobileUser(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

	//用户属性
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//用户属性
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//用户状态
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//用户局数
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;
	//用户成绩
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	pUserInfoHead->dwClientAddr=pIServerUserItem->GetClientAddr();

	lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//叠加信息
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);

	//发送数据
	WORD wHeadSize=sizeof(tagMobileUserInfoHead);
	if (dwSocketID==INVALID_DWORD)
	{
		SendDataBatchToMobileUser(pUserInfo->wTableID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}
	else
	{
		//获取用户
		WORD wBindIndex=LOWORD(dwSocketID);
		IServerUserItem * pITargetUserItem=GetBindUserItem(wBindIndex);
		ASSERT(pITargetUserItem!=NULL);
		if (pITargetUserItem==NULL) return true;
		if (pITargetUserItem->IsMobileUser()==false) return true;

		//发送玩家和目标玩家是同一人 或者 发送玩家已坐下
		if (pIServerUserItem==pITargetUserItem || pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
		}
	}

	return true;
}

//发可视用户信息
bool CAttemperEngineSink::SendVisibleTableUserInfoToMobileUser(IServerUserItem * pIServerUserItem,WORD wTablePos)
{
	//变量定义
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
	WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wTagerDeskCount = m_pInitParameter->m_wVisibleTableCount;
	WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;
	bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
	if(wTagerDeskCount==0) wTagerDeskCount=1;

	//枚举用户
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//过滤用户
		IServerUserItem *pIUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) continue;
		if (pIUserItem==pIServerUserItem) continue;
		if (pIUserItem->GetTableID()==INVALID_TABLE) continue;

		//部分可视
		if(wTablePos!=INVALID_WORD && !bViewModeAll)
		{
			WORD wUserPagePos=pIUserItem->GetMobileUserDeskPos();
			WORD wLastPagePos=(m_pGameServiceOption->wTableCount/wTagerDeskCount)*wTagerDeskCount;

			if (wTablePos==0)
			{
				if ((wUserPagePos < 0)||(wUserPagePos > 2*wTagerDeskCount))continue;
			}
			else if (wTablePos==wLastPagePos)
			{
				if ((wUserPagePos < wLastPagePos-2*wTagerDeskCount)||(wUserPagePos > wLastPagePos+wTagerDeskCount))continue;
			}
			else
			{
				if ((wUserPagePos < wTablePos-wEverySendPage*wTagerDeskCount)||(wUserPagePos > wTablePos+wEverySendPage*wTagerDeskCount+wTagerDeskCount-1))continue;
			}
		}

		//定义变量
		tagUserInfo * pUserInfo=pIUserItem->GetUserInfo();
		ZeroMemory(cbBuffer,sizeof(cbBuffer));
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

		//用户属性
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
		pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

		//用户属性
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//用户状态
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//用户局数
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;
		pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

		//用户成绩
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pIUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pIUserItem->GetFrozenedScore();
		pUserInfoHead->lIngot=pUserInfo->lIngot;
		pUserInfoHead->dBeans=pUserInfo->dBeans;
		pUserInfoHead->bAndroid = pIUserItem->IsAndroidUser();
		lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

		//叠加信息
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//发送数据
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//发送个人信息到某一页
bool CAttemperEngineSink::SendUserInfoPacketBatchToPage(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem!=NULL && pIServerUserItem->GetTableID()!=INVALID_TABLE);
	if (pIServerUserItem==NULL || pIServerUserItem->GetTableID()==INVALID_TABLE) return false;

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	ZeroMemory(cbBuffer,sizeof(cbBuffer));
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
	WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wTagerDeskCount=m_pInitParameter->m_wVisibleTableCount;
	WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;
	WORD wLastPagePos=(m_pGameServiceOption->wTableCount/wTagerDeskCount)*wTagerDeskCount;
	bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);

	//定义变量
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

	//用户属性
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//用户属性
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//用户状态
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//用户局数
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

	//用户成绩
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->dwClientAddr=pIServerUserItem->GetClientAddr();

	lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//叠加信息
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	//枚举用户
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//过滤用户
		IServerUserItem *pIUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) continue;
		if (pIUserItem->IsMobileUser()==false) continue;

		//部分可视
		if(!bViewModeAll)
		{
			WORD wDestPos=pIUserItem->GetMobileUserDeskPos();
			WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-wTagerDeskCount;
			if(wDestPos > wMaxDeskPos) wDestPos = wMaxDeskPos;

			if (wTagerDeskPos==0)
			{
				if ((wDestPos < 0)||(wDestPos > 2*wTagerDeskCount))continue;
			}
			else if (wTagerDeskPos==wLastPagePos)
			{
				if ((wDestPos < wLastPagePos-2*wTagerDeskCount)||(wDestPos > wLastPagePos+wTagerDeskCount))continue;
			}
			else
			{
				if ((wDestPos < wTagerDeskPos-wEverySendPage*wTagerDeskCount)||(wDestPos > wTagerDeskPos+wEverySendPage*wTagerDeskCount+wTagerDeskCount-1))continue;
			}
		}

		//发送数据
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//手机立即登录
bool CAttemperEngineSink::MobileUserImmediately(IServerUserItem * pIServerUserItem)
{
	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//查找桌子
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=((INT)i+nStartIndex)%(INT)(m_pGameServiceOption->wTableCount);

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//查找桌子
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=((INT)i+nStartIndex)%(INT)(m_pGameServiceOption->wTableCount);

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//失败
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("没找到可进入的游戏桌！"),REQUEST_FAILURE_NORMAL);
	return true;
}

//发送系统消息
bool CAttemperEngineSink::SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize)
{
	//消息处理
	ASSERT(pSendMessage!=NULL);

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength);
	if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength) 
		return false;

	//所有房间
	if(pSendMessage->cbAllRoom == TRUE)
	{
		pSendMessage->cbAllRoom=FALSE;
		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_SYSTEM_MESSAGE,pSendMessage,wDataSize);
	}
	else
	{
		//发送系统消息
		if(pSendMessage->cbGame == TRUE)
			SendGameMessage(pSendMessage->szSystemMessage,SMT_CHAT);
		if(pSendMessage->cbRoom == TRUE)
			SendRoomMessage(pSendMessage->szSystemMessage,SMT_CHAT);
	}

	return true;
}

//移除消息
void CAttemperEngineSink::RemoveSystemMessage()
{
	//缓存消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}

//开始游戏写入参与信息
VOID CAttemperEngineSink::PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, TCHAR * szRoomID)
{
	//退还费用
	DBR_GR_WriteJoinInfo JoinInfo;
	ZeroMemory(&JoinInfo, sizeof(DBR_GR_WriteJoinInfo));

	JoinInfo.dwUserID = dwUserID;
	JoinInfo.dwTableID = wTableID;
	lstrcpyn(JoinInfo.szRoomID,  szRoomID, sizeof(JoinInfo.szRoomID) );

	//投递数据
	m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_WRITE_JOIN_INFO, 0, &JoinInfo, sizeof(JoinInfo));

}
//更新控制
bool CAttemperEngineSink::OnDBUpdateControlInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(sizeof(DBO_GR_UpdateControlInfo)==wDataSize);
	if(sizeof(DBO_GR_UpdateControlInfo)!=wDataSize) return false;

	//变量定义
	DBO_GR_UpdateControlInfo * pUpdateControlInfo=(DBO_GR_UpdateControlInfo *)pData;
	if(pUpdateControlInfo==NULL) return false;

	//获取用户
	IServerUserItem * pIServerUserItem= m_ServerUserManager.SearchUserItem(pUpdateControlInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;
	pIServerUserItem->SetGameRate(pUpdateControlInfo->nWinRate);
	pIServerUserItem->SetZhaJinHuaGameRate(pUpdateControlInfo->nZhaJinHuaWinRate);
	if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true)
		pIServerUserItem->SetTailGameID(pUpdateControlInfo->dwTailGameID);
	else
		pIServerUserItem->SetTailGameID(0);

	if(pUpdateControlInfo->bRegister)
	{
		pIServerUserItem->SetTotalScore(pUpdateControlInfo->lTotalScore);
		pIServerUserItem->SetTdTotalScore(pUpdateControlInfo->lTdTotalScore);

		//更新玩家单日和总输赢
		SCORE lTdTotalScore = pIServerUserItem->GetTdTotalScore();
		SCORE lTotalScore = pIServerUserItem->GetTotalScore();

	}

	if(pUpdateControlInfo->cbNullity == 1)
	{
		TCHAR szChat[256] = TEXT("");
		SendGameMessage(pIServerUserItem,TEXT(""),SMT_CLOSE_GAME);
		_sntprintf(szChat,CountArray(szChat),TEXT("您的账号为异常账号，已经被系统踢下线!"));
		SendRoomMessage(pIServerUserItem, szChat, SMT_EJECT);
		SendRoomMessage(pIServerUserItem,szChat,SMT_CLOSE_ROOM);
		//SendRoomMessage(pIServerUserItem, szChat, SMT_EJECT);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
