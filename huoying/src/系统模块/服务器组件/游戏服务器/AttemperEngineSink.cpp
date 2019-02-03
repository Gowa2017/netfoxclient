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
//ʱ���ʶ

#define IDI_LOAD_ANDROID_USER		(IDI_MAIN_MODULE_START+1)			//������Ϣ
#define IDI_REPORT_SERVER_INFO		(IDI_MAIN_MODULE_START+2)			//������Ϣ
#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//����ʱ��
#define IDI_GAME_SERVICE_PULSE		(IDI_MAIN_MODULE_START+4)			//��������
#define IDI_DISTRIBUTE_ANDROID		(IDI_MAIN_MODULE_START+5)			//�������
#define IDI_DBCORRESPOND_NOTIFY		(IDI_MAIN_MODULE_START+6)			//����֪ͨ
#define IDI_LOAD_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+7)			//ϵͳ��Ϣ
#define IDI_SEND_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+8)			//ϵͳ��Ϣ
#define IDI_LOAD_SENSITIVE_WORD		(IDI_MAIN_MODULE_START+9)			//�������д�
#define IDI_DISTRIBUTE_USER		    (IDI_MAIN_MODULE_START+10)			//�����û�
#define IDI_ANDROID_JION_MATCH		(IDI_MAIN_MODULE_START+11)			//�����û�
#define IDI_LOAD_ANDROID_CONFIG		(IDI_MAIN_MODULE_START+12)			//����������
//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�� ��

#define TIME_LOAD_ANDROID_USER		15L									//���ػ���
#define TIME_DISTRIBUTE_ANDROID		15L									//�����û�
#define TIME_REPORT_SERVER_INFO		25L									//�ϱ�ʱ��
#define TIME_DBCORRESPOND_NOTIFY	3L									//����֪ͨʱ��
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //ϵͳ��Ϣʱ��
#define TIME_LOAD_SENSITIVE_WORD	5L									//�������д�ʱ��
#define TIME_LOAD_ANDROID_CONFIG	60L									//���ػ���������
//////////////////////////////////////////////////////////////////////////////////
//��������
#define TEMP_MESSAGE_ID				INVALID_DWORD			


//���ʱ��
#define MIN_INTERVAL_TIME			10									//���ʱ��
#define MAX_INTERVAL_TIME			1*60								//��ʱ��ʶ

#define ZZMJ_KIND_ID  386
#define HZMJ_KIND_ID  389
#define SET_RULE			1

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//���Ʊ���
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//������
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//��Ա����
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//�������
	m_wTaskCount=0;
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//�ȼ�����
	m_wLevelCount=0;
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));

	//״̬����
	m_pInitParameter=NULL;
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameMatchOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;
	m_bHasPersonalRoomService = FALSE;

	//��������
	ZeroMemory(&m_DataConfigColumn,sizeof(m_DataConfigColumn));
	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));
	//��������
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

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//ɾ����Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i=0;i<m_SystemMessageBuffer.GetCount();i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//�������
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	//��������	
	m_KickUserItemMap.RemoveAll();
	m_DistributeManage.RemoveAll();
	DeleteCriticalSection(&m_cs_test);
	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//����Ϣ
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//���ñ���
	InitMatchServiceManager();

	//���û���
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ýӿ�
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

	//��������
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ýӿ�
	m_AndroidUserManager.SetMainServiceFrame(QUERY_ME_INTERFACE(IMainServiceFrame));

	//���ù���
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//��ʱ�������д�
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	//������ģʽ
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//�����˲���
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		m_pITimerEngine->SetTimer(IDI_ANDROID_JION_MATCH,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//���ػ���
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

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//������Ϣ
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameMatchOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//�������
	m_wTaskCount=0;
	ZeroMemory(&m_TaskParameter,sizeof(m_TaskParameter));

	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));

	//��Ա����
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//�ȼ�����
	m_wLevelCount=0;
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));


	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//ɾ���û�
	m_TableFrameArray.RemoveAll();
	m_DistributeManage.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_ServerListManager.ResetServerList();
	
	//ֹͣ����
	m_AndroidUserManager.ConcludeService();

	//ֹͣ����
	if(m_pIMatchServiceManager!=NULL)
		m_pIMatchServiceManager->StopService();

	//�Ƴ���Ϣ
	RemoveSystemMessage();

	//�Ƴ�����
	m_UserTaskManager.ResetTaskManager();

	//��λ�ؼ���
	m_WordsFilter.ResetSensitiveWordArray();

	//˽������
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

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//�����б�
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_COLUMN,0L,NULL,0L);

			//���ػ���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);			

			//������Ϣ
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//��Ա����
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_MEMBER_PARAMETER,0,NULL,0);	


			//�ɳ�����
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GROWLEVEL_CONFIG,0,NULL,0);	

			//����ǩ��
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_CHECKIN_REWARD,0L,NULL,0L);

			//���صͱ�
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_BASEENSURE,0L,NULL,0L);

			if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				//˽������
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PERSONAL_PARAMETER,0,&m_pGameServiceOption->wKindID,sizeof(WORD),0);
			}

			return true;
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	try
	{

		//����ʱ��
		if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
		{
			//ʱ�䴦��
			switch (dwTimerID)
			{
			case IDI_LOAD_ANDROID_USER:		//���ػ���
				{
					//��������
					DWORD dwBatchID=0;
					DWORD dwAndroidCount=0;

					//�����ж�
					if(m_AndroidUserManager.GetAndroidLoadInfo(dwBatchID,dwAndroidCount))
					{
						//��������
						tagAndroidParameterEx * pAndroidParameter=NULL;
						pAndroidParameter = m_AndroidUserManager.GetAndroidParameter(dwBatchID);

						//����ṹ
						DBR_GR_LoadAndroidUser LoadAndroidUser;
						LoadAndroidUser.dwBatchID=dwBatchID;
						LoadAndroidUser.dwAndroidCount=dwAndroidCount;
						LoadAndroidUser.AndroidCountMember0=pAndroidParameter->AndroidParameter.AndroidCountMember0;
						LoadAndroidUser.AndroidCountMember1=pAndroidParameter->AndroidParameter.AndroidCountMember1;
						LoadAndroidUser.AndroidCountMember2=pAndroidParameter->AndroidParameter.AndroidCountMember2;
						LoadAndroidUser.AndroidCountMember3=pAndroidParameter->AndroidParameter.AndroidCountMember3;
						LoadAndroidUser.AndroidCountMember4=pAndroidParameter->AndroidParameter.AndroidCountMember4;
						LoadAndroidUser.AndroidCountMember5=pAndroidParameter->AndroidParameter.AndroidCountMember5;

						//���ػ���
						m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,&LoadAndroidUser,sizeof(LoadAndroidUser));
					}

					return true;
				}
			case IDI_REPORT_SERVER_INFO:	//������Ϣ
				{
					//��������
					CMD_CS_C_ServerOnLine ServerOnLine;
					ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

					//���ñ���				
					ServerOnLine.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
					ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerOnLine.dwAndroidCount;

					//��������
					m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine));

					return true;
				}
			case IDI_CONNECT_CORRESPOND:	//����Э��
				{
					//��������
					tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
					m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//������ʾ
					TCHAR szString[512]=TEXT("");
					_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//��ʾ��Ϣ
					CTraceService::TraceString(szString,TraceLevel_Normal);

					return true;
				}
			case IDI_GAME_SERVICE_PULSE:	//����ά��
				{
					return true;
				}
			case IDI_LOAD_ANDROID_CONFIG:
				{
					//���ػ���
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);		
					return true;
				}
			case IDI_DISTRIBUTE_ANDROID:	//�������
				{
					//Լս�������������
					if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
					{
						return true;
					}

					//��������
					if (m_AndroidUserManager.GetAndroidCount()>0 && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
					{
						//��������
						bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
						bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));

						//ģ�⴦��
						if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
						{
							//����״̬
							tagAndroidUserInfo AndroidSimulate; 
							m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);

							//��������
							if (AndroidSimulate.wFreeUserCount>0)
							{
								for (WORD i=0;i<8;i++)
								{
									//�������
									WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));

									//��ȡ����
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

									//����״��
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//�����ж�
									if (TableUserInfo.wTableUserCount>0) continue;
									if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

									//�����ж�
									if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
									{
										//��������
										WORD wHandleCount=0;
										WORD wWantAndroidCount=TableUserInfo.wMinUserCount;

										//���ݵ���
										if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
										{
											WORD wChairCount=m_pGameServiceAttrib->wChairCount;
											WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
											WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
											wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
										}

										WORD wAndroid = pTableFrame->GetTableAndroidConut();
										//���´���
										for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
										{
											//��������
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
											//��Ч����
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											//�û�����
											IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
											if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
											{
												//���ñ���
												wHandleCount++;

												//����ж�
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

						//�����
						if (bAllowAndroidAttend==true)
						{
							//����״̬
							tagAndroidUserInfo AndroidPassivity;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

							if(bAllowAvertCheatMode)
							{
								//���´���
								for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
									if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
								//��������
								if (AndroidPassivity.wFreeUserCount>0)
								{
									//������Ϸ
									if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
									{
										for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
										{
											//��ȡ����
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
											//����״��
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//�����ж�
											if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

											//��������
											IServerUserItem * pIServerUserItem=NULL;
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//��Ч����
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											WORD wHandleCount=0;
											WORD wAndroid = pTableFrame->GetTableAndroidConut();
											//���´���
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
											//��ȡ����
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
											//����״��
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//�����ж�
											if (wUserSitCount==0) continue;
											if (TableUserInfo.wTableUserCount==0) continue;
											if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%100>50)) continue;

											//��������
											IServerUserItem * pIServerUserItem=NULL;
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//��Ч����
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											WORD wHandleCount=0;
											WORD wAndroid = pTableFrame->GetTableAndroidConut();
											//���´���
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

						//�����
						if (bAllowAndroidAttend==true)
						{
							//����״̬
							tagAndroidUserInfo AndroidInitiative;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
							WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;

							if(bAllowAvertCheatMode)
							{
								//���´���
								for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
									if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
								//��������
								if (AndroidInitiative.wFreeUserCount>0)
								{
									for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
									{
										//��ȡ����
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
										//����״��
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//�����ж�
										if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1) && (rand()%100>50)) continue;

										//��������
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//��Ч����
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										WORD wHandleCount=0;
										WORD wAndroid = pTableFrame->GetTableAndroidConut();
										//���´���
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
							//��ȡʱ��
							DWORD dwCurrTime = (DWORD)time(NULL);
							if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

							//���ñ���
							m_dwLastDisposeTime=dwCurrTime;
							m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
						}

						//��������
						bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

						//�����ж�
						if(bMaxPlayerGame)
						{
							//��ȡʱ��
							DWORD dwCurrTime = (DWORD)time(NULL);
							if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

							//���ñ���
							m_dwLastDisposeTime=dwCurrTime;
							m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
						}

						//��������
						WORD wStandUpCount=0;
						WORD wRandCount=((rand()%3)+1);
						INT_PTR nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
						for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
						{
							//��ȡ����
							INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
							CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
							if (pTableFrame->IsGameStarted()==true) continue;

							//����״��
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//�û�����
							bool bRand = ((rand()%100)>50);
							if (TableUserInfo.wTableAndroidCount==0) continue;
							if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
							if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;

							//��������
							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
								if (pIServerUserItem==NULL) continue;

								//�û�����
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

						//��������
						nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
						for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
						{
							//��ȡ����
							INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
							CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
							if (pTableFrame->IsGameStarted()==true) continue;

							//����״��
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//�û�����
							bool bRand = ((rand()%100)>50);
							if (TableUserInfo.wTableAndroidCount==0) continue;
							if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;

							//��������
							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
							{
								//��ȡ�û�
								IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
								if (pIServerUserItem==NULL) continue;

								//�û�����
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
			case IDI_DBCORRESPOND_NOTIFY: //���涨ʱ����
				{
					if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
					return true;
				}
			case IDI_LOAD_SYSTEM_MESSAGE: //ϵͳ��Ϣ
				{
					//�����Ϣ����
					RemoveSystemMessage();

					//������Ϣ
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

					return true;
				}
			case IDI_LOAD_SENSITIVE_WORD:	//�������д�
				{
					//Ͷ������
					m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
					return true;
				}
			case IDI_SEND_SYSTEM_MESSAGE: //ϵͳ��Ϣ
				{
					//�����ж�
					if(m_SystemMessageActive.GetCount()==0) return true;

					//ʱЧ�ж�
					DWORD dwCurrTime = (DWORD)time(NULL);
					for(INT_PTR nIndex=m_SystemMessageActive.GetCount()-1;nIndex>=0;nIndex--)
					{
						tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

						//ʱЧ�ж�
						if(pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
						{
							m_SystemMessageActive.RemoveAt(nIndex);
							SafeDelete(pTagSystemMessage);

							continue;
						}

						//����ж�
						if(pTagSystemMessage->dwLastTime+pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
						{
							//��������
							pTagSystemMessage->dwLastTime=dwCurrTime;

							//������Ϣ
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

							//������Ϣ
							WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
							SendSystemMessage(&SendMessage,wSendSize);
						}
					}

					return true;
				}
			case IDI_DISTRIBUTE_USER: //�����û�
				{
					//ִ�з���
					PerformDistribute();

					return true;
				}
			case IDI_ANDROID_JION_MATCH:	//�����˲���
				{
					//��������
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
					{
						//�����ʶ
						bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);

						//���洦��
						if (bAllowAndroidAttend==true)
						{
							//����״̬
							tagAndroidUserInfo AndroidPassivity;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

							//���´���
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

		//����ʱ��
		if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
		{
			//ʱ�䴦��
			m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

			return true;
		}

		//������ʱ��
		if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
		{
			if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
			return true;
		}

		//����ʱ��
		if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
		{
			//���Ӻ���
			DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
			WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

			//ʱ��Ч��
			if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
			{
				ASSERT(FALSE);
				return false;
			}

			//ʱ��֪ͨ
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
			return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
		}
	}catch(...)
	{
		TCHAR szMessage[128];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("��ʱ���쳣��TimerID=%d"),dwTimerID);
		CTraceService::TraceString(szMessage,TraceLevel_Warning);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PARAMETER:			//��Ϸ����
		{
			return OnDBGameParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_COLUMN_INFO:		//�б���Ϣ
		{
			return OnDBGameColumnInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_PARAMETER:	//��������
		{
			return OnDBGameAndroidParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_INFO:		//������Ϣ
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}

	case DBO_GR_GAME_PROPERTY_BUFF:		//����Buff
		{
			return OnDBGamePropertyBuff(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOAD_TRUMPET:			//������Ϣ
		{
			return OnDBGameLoadTrumpet(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SEND_TRUMPET:			//ʹ������
		{
			return	OnDBGameSendTrumpet(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_MEMBER_PAREMETER:  //��Ա����
		{
			return OnDBGameMemberParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_QUERY_INFO_RESULT://��Ա��ѯ
		{
			return OnDBPCMemberDayQueryInfoResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_DAY_PRESENT_RESULT://��Ա�ͽ�
		{
			return OnDBPCMemberDayPresentResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_MEMBER_DAY_GIFT_RESULT://��Ա���
		{
			return OnDBPCMemberDayGiftResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_INFO:		//������Ϣ
		{
			return OnDBUserInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_SUCCESS:	//���гɹ�
		{
			return OnDBUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_FAILURE:	//����ʧ��
		{
			return OnDBUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_USER_INFO:  //�û���Ϣ
		{
			return OnDBUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_ENABLE_RESULT: //��ͨ���
		{
			return OnDBUserInsureEnableResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_SUCCESS:		//���߳ɹ�
		{
			return OnDBPropertySuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_RESULT:  //ϵͳ��Ϣ
		{
			return OnDBSystemMessage(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_FINISH:	//�������
		{
			return OnDBSystemMessageFinish(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SENSITIVE_WORDS:		//�������д�
		{
			return OnDBSensitiveWords(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_INFO:			//ǩ����Ϣ
		{
			return OnDBPCUserCheckInInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_RESULT:			//ǩ�����
		{
			return OnDBPCUserCheckInResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CHECKIN_REWARD:			//ǩ������
		{
			return OnDBPCCheckInReward(dwContextID,pData,wDataSize);
		}
	case DBO_GR_BASEENSURE_PARAMETER:	//�ͱ�����
		{
			return OnDBPCBaseEnsureParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_BASEENSURE_RESULT:		//�ͱ����
		{
			return OnDBPCBaseEnsureResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST:				//�����б�
		{
			return OnDBUserTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST_END:			//�������
		{
			return OnDBUserTaskListEnd(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_INFO:				//������Ϣ
		{
			return OnDBUserTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_RESULT:			//������
		{
			return OnDBUserTaskResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PURCHASE_RESULT:		//������
		{
			return OnDBPurchaseResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_EXCHANGE_RESULT:		//�һ����
		{
			return OnDBExChangeResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_CONFIG:		//�ȼ�����
		{
			return OnDBPCGrowLevelConfig(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_PARAMETER:	//�ȼ�����
		{
			return OnDBPCGrowLevelParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GROWLEVEL_UPGRADE:		//�ȼ�����
		{
			return OnDBPCGrowLevelUpgrade(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_BUY:		//������
		{
			return OnDBGamePropertyBuy(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_FAILURE:		//����ʧ��
		{
			return OnDBPropertyFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_QUERY_BACKPACK:			//�������
		{
			return OnDBQueryPropertyBackpack(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_USE:			//ʹ�ý��
		{
			return OnDBQueryPropertyUse(dwContextID,pData,wDataSize);
		}
	case DBO_GR_QUERY_SEND_PRESENT:		//��ѯ����
		{
			return OnDBQuerySendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_PRESENT:		//��������
		{
			return OnDBQueryPropertyPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GET_SEND_PRESENT:		//��ȡ����
		{
			return OnDBQueryGetSendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CREATE_SUCCESS:			//�����ɹ�
		{
			return OnDBCreateSucess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CREATE_FAILURE:			//����ʧ��
		{
			return OnDBCreateFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_CANCEL_CREATE_RESULT:		//ȡ������
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
	case DBO_GR_UPDATE_CONTROLINFO:		//���¿���
		{
			return OnDBUpdateControlInfo(dwContextID,pData,wDataSize);
		}
	}

	//�����¼�
	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//����Ч��
		if(m_pIMatchServiceManager==NULL) return false;

		//�����û�
		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem * pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		return m_pIMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize,dwContextID);
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//�����ж�
		if (m_bNeekCorrespond==true)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��Э�������������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڷ�����Ϸ����ע����Ϣ..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//����˿�
		CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
		RegisterServer.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//��������
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

		//��������
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVER,&RegisterServer,sizeof(RegisterServer));

		//�����Լս�����ʹ�����������
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			CMD_CS_S_RegisterPersonal  RegisterPersonal;
			RegisterPersonal.dwKindID = m_pGameServiceOption->wKindID;
			RegisterPersonal.dwMaxCreate = m_PersonalRoomOption.wCanCreateCount;
			m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_PERSONAL, &RegisterPersonal, sizeof(RegisterPersonal));
		}
		
		//�Ƿ�ʹ�ý�ҿ�
		bool bIsTreasureDB = false;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
		{
			bIsTreasureDB = true;
		}

		//����״̬
		for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[i];
			//���ʹ�õ��ǽ�����ݿ�
			if (bIsTreasureDB)
			{
				pTableFrame->SetDataBaseMode(0);
			}
			else
			{
				pTableFrame->SetDataBaseMode(1);
			}

		}
		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//�û�����
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_ANDROID_SERVICE: //��������
			{
				return OnTCPSocketMainAndroidService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;
	WORD wTableID=INVALID_WORD;

	try
	{
		//�û�����
		if (pIServerUserItem!=NULL)
		{
			//��������
			wTableID=pIServerUserItem->GetTableID();

			WORD wDrawCount = 0;
			if(wTableID!=INVALID_TABLE)
			{
				CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
				wDrawCount = pTableFrame->GetDrawCount();
			}

			//���ߴ���
			BYTE cbMatchStatus = pIServerUserItem->GetUserMatchStatus();
			bool bImmediteMatch = (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE);
			if (wTableID!=INVALID_TABLE || (bImmediteMatch==true && cbMatchStatus == MUS_PLAYING)|| (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && wDrawCount!=0 ))
			{
				//�����
				pIServerUserItem->DetachBindStatus();
				if(wTableID < m_pGameServiceOption->wTableCount)
				{
					//����֪ͨ
					ASSERT(wTableID<m_pGameServiceOption->wTableCount);
					m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
				}
				else //�Ȳ�������ʲô����
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
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�ر������쳣: wTableID=%d"),wTableID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize == 866)
	{
		TCHAR szMessage[260]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�ᷢ���쳣����Ϣ: Command.wMainCmdID =%d   Command.wSubCmdID = %d"), Command.wMainCmdID, Command.wSubCmdID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//�û�����
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_LOGON:		//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//��Ϸ����
		{
			bool bReturnVal = OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				//�����Ϣ
				TCHAR szBuffer[128]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("MDM_GF_GAME ��Ϸ����� [ %ld ]"),Command.wSubCmdID);
				//�����Ϣ
				CTraceService::TraceString(szBuffer,TraceLevel_Normal);

			}
			return bReturnVal;
		}
	case MDM_GF_FRAME:		//�������
		{
			bool bReturnVal =  OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				CTraceService::TraceString(TEXT("MDM_GF_FRAME �������� false"),TraceLevel_Normal);
			}
			return bReturnVal;
		}
	case MDM_GR_INSURE:		//��������
		{
			return OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_TASK:		//��������
		{
			return OnTCPNetworkMainTask(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_EXCHANGE:	//�һ�����
		{
			return OnTCPNetworkMainExchange(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGE:		//��������
		{
			return OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//��������
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MEMBER:	//��Ա����
		{
			return OnTCPNetworkMainMember(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PROPERTY:	//��������
		{
			return OnTCPNetworkMainProperty(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_BASEENSURE:	//�ͱ�����
		{
			return OnTCPNetworkMainBaseEnsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_CHECKIN: //ǩ������
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

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_ALL_CLIENT);

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_ALL_CLIENT);

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendHallMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_HALL_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	if (bAndroid)
	{
		//�����û�
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//�û�����
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

//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//�����û�
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//�����û�
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}

	return false;
}


//Ⱥ������
bool CAttemperEngineSink::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pITargetUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pITargetUserItem==NULL) continue;
		//if(!pITargetUserItem->IsAndroidUser())
			if(pITargetUserItem->IsMobileUser() == false) continue;

		//�������
		WORD wMobileUserRule = pITargetUserItem->GetMobileUserRule();
		WORD wTagerTableID = pITargetUserItem->GetTableID();
		bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
		bool bRecviceGameChat = ((wMobileUserRule&RECVICE_GAME_CHAT)!=0);
		bool bRecviceRoomChat = ((wMobileUserRule&RECVICE_ROOM_CHAT)!=0);
		bool bRecviceRoomWhisper = ((wMobileUserRule&RECVICE_ROOM_WHISPER)!=0);

		//״̬����
		if((pITargetUserItem->GetUserStatus() >= US_SIT) && pITargetUserItem->IsClientReady() &&(m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH))
		{
			if(wCmdTable!=INVALID_WORD && wTagerTableID != wCmdTable)continue;
		}

		//�������
		if(wSubCmdID==SUB_GR_USER_CHAT || wSubCmdID==SUB_GR_USER_EXPRESSION)
		{
			if(!bRecviceGameChat || !bRecviceRoomChat) continue;
		}
		if(wSubCmdID==SUB_GR_WISPER_CHAT || wSubCmdID==SUB_GR_WISPER_EXPRESSION)
		{
			if(!bRecviceRoomWhisper) continue;
		}

		//���ֿ���
		if(!bViewModeAll)
		{
			//��Ϣ����
			if(wMainCmdID==MDM_GR_USER && wSubCmdID==SUB_GR_USER_ENTER && wCmdTable==INVALID_TABLE) continue;
			
			//�������
			WORD wTagerDeskPos = pITargetUserItem->GetMobileUserDeskPos();
			WORD wTagerDeskCount = m_pInitParameter->m_wVisibleTableCount;
			WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;

			//״̬��Ϣ����
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

		//������Ϣ
		SendData(pITargetUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//�������
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//��������
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//��������
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount = pUserInfo->lIntegralCount;

	//�������
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//��������
	SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));


	//��ʱд��
	if (
		(CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
		&&(pIServerUserItem->IsVariation()==true) || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//��������
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

		//��������
		tagUserTaskEntry * pUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(),TASK_STATUS_UNFINISH); 
		if(pUserTaskEntry!=NULL)
		{
			ShowLog(TEXT("�����������ô��"));
			WriteGameScore.bTaskForward = true;
			ShowLog(TEXT("%d--����-----"),WriteGameScore.bTaskForward);
		}

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);


		//˽�˷���ID
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

		//˽�˷���������ݽṹ
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

		//�������ݿ�д�����
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{
			m_pIDBCorrespondManager->PostDataBaseRequest(WritePersonalScore.dwUserID,DBR_GR_WRITE_PERSONAL_GAME_SCORE,0L,&WritePersonalScore,sizeof(WritePersonalScore), TRUE);
		}

		//�ƽ�����
		if(pUserTaskEntry!=NULL)
		{
			PerformTaskProgress(pIServerUserItem,pUserTaskEntry,WriteGameScore.VariationInfo.dwWinCount,WriteGameScore.VariationInfo.dwLostCount,WriteGameScore.VariationInfo.dwDrawCount);
		}
	}

	//֪ͨ����
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		m_TableFrameArray[pIServerUserItem->GetTableID()]->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	}

	return true;
}

//�û�״̬
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if (pIServerUserItem->GetUserInfo()->dwUserID == 6346   || pIServerUserItem->GetUserInfo()->dwUserID == 6354)
	{
		printf(("OnEventUserItemStatus��Ϸ��������й��˳�________________________________________%d\n"),pIServerUserItem->GetUserInfo()->dwUserID);
	}
	//��������
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//��������
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus=pIServerUserItem->GetUserStatus();

	//�û���Ϣ
	WORD wTableID=pIServerUserItem->GetTableID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�޸���Ϣ
	if(pIServerUserItem->GetUserStatus()>=US_SIT)
	{
		//�������
		WORD wNewDeskPos = (pIServerUserItem->GetTableID()/m_pInitParameter->m_wVisibleTableCount)*m_pInitParameter->m_wVisibleTableCount;
		WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-m_pInitParameter->m_wVisibleTableCount;
		if(wNewDeskPos > wMaxDeskPos) wNewDeskPos = wMaxDeskPos;
		pIServerUserItem->SetMobileUserDeskPos(wNewDeskPos);
	}

	//��������
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

		//������Ϣ
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

	//�������״̬
	CMD_CS_C_UserStatus  Status;
	Status.dwUserID = UserStatus.dwUserID;
	Status.cbUserStatus = UserStatus.UserStatus.cbUserStatus;
	Status.wKindID  = m_pGameServiceOption->wKindID;
	Status.wServerID =m_pGameServiceOption->wServerID;
	Status.wTableID = UserStatus.UserStatus.wTableID;
	Status.wChairID = UserStatus.UserStatus.wChairID;
	m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_STATUS,&Status,sizeof(Status));


	//�뿪�ж�
	if (pIServerUserItem->GetUserStatus()==US_NULL)
	{
		//��ȡ��
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		if (pBindParameter!=NULL)
		{
			//�󶨴���
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//�ж�����
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

		//�뿪����
		OnEventUserLogout(pIServerUserItem,0L);
	}
	return true;
}

//�û�Ȩ��
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//����Ȩ��
	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;
	ManageUserRight.bGameRight=cbRightKind==UR_KIND_GAME;
	
	//��������
	m_pIDBCorrespondManager->PostDataBaseRequest(ManageUserRight.dwUserID,DBR_GR_MANAGE_USER_RIGHT,0,&ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	//Ч�����
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

	//��������
	WORD wDataSize=CountStringBuffer(UserGameData.szUserGameData);
	WORD wHeadSize=sizeof(UserGameData)-sizeof(UserGameData.szUserGameData);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_GAME_DATA,&UserGameData,wHeadSize+wDataSize);

	//д���ݿ�
	DBR_GR_WriteUserGameData WriteUserGameData;
	ZeroMemory(&WriteUserGameData,sizeof(WriteUserGameData));

	WriteUserGameData.wKindID=m_pGameServiceOption->wKindID;
	WriteUserGameData.dwUserID=pIServerUserItem->GetUserID();
	WriteUserGameData.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(WriteUserGameData.szUserGameData,pszUserGameData,CountArray(WriteUserGameData.szUserGameData));

	//��������
	DWORD dwUserID=pIServerUserItem->GetUserID();
	WORD wDataSize1=CountStringBuffer(WriteUserGameData.szUserGameData);
	WORD wHeadSize1=sizeof(WriteUserGameData)-sizeof(WriteUserGameData.szUserGameData);
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_WRITE_USER_GAME_DATA,0,&WriteUserGameData,wHeadSize1+wDataSize1);

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//�����ж�
	if ((pBindParameter->pIServerUserItem!=NULL)||(pBindParameter->dwSocketID!=dwContextID))
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_NORMAL);

		return true;
	}

	//��������
	bool bAndroidUser=(wBindIndex>=INDEX_ANDROID);
	bool bMobileUser=(pBindParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//�����ж�
	DWORD dwKickTime;
	if(m_KickUserItemMap.Lookup(pDBOLogonSuccess->dwUserID,dwKickTime)==TRUE)
	{
		//ʱЧ�ж�
		DWORD dwCurrTime = (DWORD)time(NULL);
		if(dwKickTime+VALID_TIME_KICK_BY_MANAGER > dwCurrTime)
		{
			//����ʧ��
			SendLogonFailure(TEXT("���ѱ�����Ա�������,1Сʱ֮�ڲ��ܽ��룡"),FAILURE_TYPE_ADMIN_KICK_OUT,pBindParameter->dwSocketID);

			//�������
			PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
			//�������
			DBR_GR_Deblocking Deblocking;
			Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
			Deblocking.dwServerID=m_pGameServiceOption->wServerID;

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

			return true;
		}
		else
		{
			//�Ƴ����
			m_KickUserItemMap.RemoveKey(pDBOLogonSuccess->dwUserID);
		}
	}

	//�л��ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		//�л��û�
		SwitchUserItemConnect(pIServerUserItem,pDBOLogonSuccess->szMachineID,wBindIndex,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_USER_IMPACT);

		return true;
	}

	//ά���ж�
	if ((CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule)==true)&&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ������ϵͳά����ԭ�򣬵�ǰ��Ϸ�����ֹ�û����룡"),FAILURE_TYPE_SYSTEM_MAINTENANCE,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SYSTEM);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//���һ���
	if (bAndroidUser==true && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
	{
		//���һ���
		DWORD dwUserID=pDBOLogonSuccess->dwUserID;
		IAndroidUserItem * pIAndroidUserItem=m_AndroidUserManager.SearchAndroidUserItem(dwUserID,dwContextID);

		//�޸Ļ���
		if (pIAndroidUserItem!=NULL)
		{
			//��ȡ����
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//��������
			if ((pAndroidParameter->lTakeMinScore!=0L)&&(pAndroidParameter->lTakeMaxScore!=0L))
			{
				//��������
				SCORE lTakeMinScore=(SCORE)pAndroidParameter->lTakeMinScore;
				SCORE lTakeMaxScore=(SCORE)__max(pAndroidParameter->lTakeMaxScore,pAndroidParameter->lTakeMinScore);

				//��������
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

	//��ͷ���
	if ((m_pGameServiceOption->lMinEnterScore!=0L)&&(pDBOLogonSuccess->lScore<m_pGameServiceOption->lMinEnterScore))
	{
		//����ʧ��
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("����÷�����Ҫ%.2f��ң�"), m_pGameServiceOption->lMinEnterScore);
		SendLogonFailure(szMsg,FAILURE_TYPE_ROOM_COIN_NOTENOUGH,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//��߷���
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(pDBOLogonSuccess->lScore>m_pGameServiceOption->lMaxEnterScore))
	{
		//����ʧ��
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("����÷�����Ҫ%.2f��ң�"), m_pGameServiceOption->lMaxEnterScore);
		SendLogonFailure(szMsg,FAILURE_TYPE_ROOM_COIN_NOTENOUGH,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//��Ա�ж�
	if(m_pGameServiceOption->cbMinEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder < m_pGameServiceOption->cbMinEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������ͽ����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),FAILURE_TYPE_VIP_LIMIT_MIN,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//��Ա�ж�
	if(m_pGameServiceOption->cbMaxEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder > m_pGameServiceOption->cbMaxEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������߽����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),FAILURE_TYPE_VIP_LIMIT_MAX,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//�����ж�
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	if ((pDBOLogonSuccess->cbMemberOrder==0)&&(pDBOLogonSuccess->cbMasterOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-RESERVE_USER_COUNT)))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����ڴ˷����Ѿ���������ͨ��Ҳ��ܼ��������ˣ�"),FAILURE_TYPE_ROOM_FULL,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);
		//�������
		DBR_GR_Deblocking Deblocking;
		Deblocking.dwUserID=pDBOLogonSuccess->dwUserID;
		Deblocking.dwServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_USER,0,&Deblocking,sizeof(Deblocking));

		return true;
	}

	//�û�����
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//��������
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID=pDBOLogonSuccess->dwGameID;
	UserInfo.dwGroupID=pDBOLogonSuccess->dwGroupID;
	UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));

	//IP������
	lstrcpyn(UserInfo.szChangeLogonIP, pDBOLogonSuccess->szChangeLogonIP, CountArray(UserInfo.szChangeLogonIP));

	//�û�����
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogonSuccess->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogonSuccess->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//��������
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));

	//״̬����
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//������Ϣ
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

	//������Ϣ
	UserInfo.dwAgentID=pDBOLogonSuccess->dwAgentID;

	//��¼��Ϣ
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

	//�û�Ȩ��
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogonSuccess->dwMasterRight;

	//��������
	UserInfoPlus.bMobileUser=bMobileUser;
	UserInfoPlus.bAndroidUser=bAndroidUser;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfoPlus.szPassword));
	lstrcpyn(UserInfoPlus.szUserGameData,pDBOLogonSuccess->szUserGameData,CountArray(UserInfoPlus.szUserGameData));

	//������Ϣ
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//�����û�
	m_ServerUserManager.InsertUserItem(&pIServerUserItem,UserInfo,UserInfoPlus);

	//�û�����
	if(pDBOLogonSuccess->wTaskCount>0)
	{
		m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),pDBOLogonSuccess->UserTaskInfo,pDBOLogonSuccess->wTaskCount);
	}

	//�����ж�
	if (pIServerUserItem==NULL)
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		//�Ͽ��û�
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

	//�����û�
	pBindParameter->pIServerUserItem=pIServerUserItem;

	//�޸Ĳ���
	if(pIServerUserItem->IsMobileUser())
	{
		SetMobileUserParameter(pIServerUserItem,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);
	}

	//����״̬
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		pIServerUserItem->SetUserMatchStatus(CUserRight::IsGameMatchUser(pIServerUserItem->GetUserRight())?MUS_SIGNUPED:MUS_NULL);
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,false);

	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserEnter UserEnter;
		ZeroMemory(&UserEnter,sizeof(UserEnter));

		//���ñ���
		UserEnter.dwUserID=pIServerUserItem->GetUserID();
		UserEnter.dwGameID=pIServerUserItem->GetGameID();
		lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

		//������Ϣ
		UserEnter.cbGender=pIServerUserItem->GetGender();
		UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
		UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();
		UserEnter.cbAndroidUser = bAndroidUser;

		//�û���ϸ��Ϣ
		tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
		if(pUserInfo != NULL) memcpy(&UserEnter.userInfo, pUserInfo, sizeof(tagUserInfo));

		//������Ϣ
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
	}
	
	//������Buff
	DWORD UserID = pDBOLogonSuccess->dwUserID;
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GAME_BUFF,dwContextID,(void*)&UserID,sizeof(UserID));

	//��������
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_TRUMPET,dwContextID,(void*)&UserID,sizeof(UserID));

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem!=NULL)) return true;

	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	//�Ͽ�����
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

//������Ϣ
bool CAttemperEngineSink::OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GR_GameParameter));
	if (wDataSize!=sizeof(DBO_GR_GameParameter)) return false;

	//��������
	DBO_GR_GameParameter * pGameParameter=(DBO_GR_GameParameter *)pData;

	//������Ϣ
	m_pGameParameter->dwMedalRate=pGameParameter->dwMedalRate;
	m_pGameParameter->dwRevenueRate=pGameParameter->dwRevenueRate;
	m_pGameParameter->dwExchangeRate=pGameParameter->dwExchangeRate;	
	m_pGameParameter->dwPresentExchangeRate=pGameParameter->dwPresentExchangeRate;
	m_pGameParameter->dwRateGold=pGameParameter->dwRateGold;
	m_pGameParameter->lEducateGrantScore=pGameParameter->lEducateGrantScore;
	m_pGameParameter->lHallNoticeThreshold=pGameParameter->lHallNoticeThreshold;
	//���齱��
	m_pGameParameter->dwWinExperience=pGameParameter->dwWinExperience;	

	//�汾��Ϣ
	m_pGameParameter->dwClientVersion=pGameParameter->dwClientVersion;
	m_pGameParameter->dwServerVersion=pGameParameter->dwServerVersion;

	//�汾Ч��
	if (VERSION_EFFICACY==TRUE)
	{
		//�汾�ж�
		bool bVersionInvalid=false;
		if (m_pGameParameter->dwClientVersion!=m_pGameServiceAttrib->dwClientVersion) bVersionInvalid=true;
		if (m_pGameParameter->dwServerVersion!=m_pGameServiceAttrib->dwServerVersion) bVersionInvalid=true;

		//��ʾ��Ϣ
		if (bVersionInvalid==true)
		{
			CTraceService::TraceString(TEXT("ƽ̨���ݿ��������汾ע����Ϣ�뵱ǰ����汾��Ϣ��һ��"),TraceLevel_Warning);
		}
	}

	return true;
}

//�б���Ϣ
bool CAttemperEngineSink::OnDBGameColumnInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameColumnInfo * pGameColumnInfo=(DBO_GR_GameColumnInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameColumnInfo)-sizeof(pGameColumnInfo->ColumnItemInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0])))) return false;

	//���ݴ���
	if (pGameColumnInfo->cbColumnCount==0)
	{
		//Ĭ���б�
	}
	else
	{
		//��������
		m_DataConfigColumn.cbColumnCount=pGameColumnInfo->cbColumnCount;
		CopyMemory(m_DataConfigColumn.ColumnItem,pGameColumnInfo->ColumnItemInfo,pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]));
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidAccountsInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0])))) return false;

	//���û���
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.InsertAndroidInfo(pGameAndroidInfo->AndroidAccountsInfo,pGameAndroidInfo->wAndroidCount,pGameAndroidInfo->dwBatchID);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameAndroidParameter * pGameAndroidParameter=(DBO_GR_GameAndroidParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidParameter)-sizeof(pGameAndroidParameter->AndroidParameter);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0])))) return false;

	//���û���
	if (pGameAndroidParameter->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.AddAndroidParameter(pGameAndroidParameter->AndroidParameter,pGameAndroidParameter->wParameterCount);
	}

	return true;
}


bool CAttemperEngineSink::OnDBGamePropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_UserGamePropertyBuff * pGamePropertyBuff=(DBO_GR_UserGamePropertyBuff *)pData;
	WORD wHeadSize=sizeof(DBO_GR_UserGamePropertyBuff)-sizeof(pGamePropertyBuff->PropertyBuff);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGamePropertyBuff->cbBuffCount*sizeof(pGamePropertyBuff->PropertyBuff[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGamePropertyBuff->cbBuffCount*sizeof(pGamePropertyBuff->PropertyBuff[0])))) return false;
	
	//���ù���
	m_GamePropertyManager.ClearUserBuff(pGamePropertyBuff->dwUserID);
	m_GamePropertyManager.SetGamePropertyBuff(pGamePropertyBuff->dwUserID, pGamePropertyBuff->PropertyBuff,pGamePropertyBuff->cbBuffCount);

	//��������
	CMD_GR_S_PropertyBuff PropertyBuffResult;
	PropertyBuffResult.dwUserID = pGamePropertyBuff->dwUserID;
	PropertyBuffResult.cbBuffCount = pGamePropertyBuff->cbBuffCount;
	memcpy(PropertyBuffResult.PropertyBuff, pGamePropertyBuff->PropertyBuff, sizeof(tagPropertyBuff)*pGamePropertyBuff->cbBuffCount);

	WORD dwHeadSize=sizeof(PropertyBuffResult)-sizeof(PropertyBuffResult.PropertyBuff);
	WORD dwDataSize=PropertyBuffResult.cbBuffCount*sizeof(PropertyBuffResult.PropertyBuff[0]);

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_USER_PROP_BUFF,&PropertyBuffResult,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBGameLoadTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_UserGameTrumpet * pUserGameTrumpet=(DBO_GR_UserGameTrumpet *)pData;
	
	CMD_GR_S_UserTrumpet UserTrumpet;
	UserTrumpet.dwTrumpetCount = pUserGameTrumpet->dwTrumpetCount;
	UserTrumpet.dwTyphonCount = pUserGameTrumpet->dwTyphonCount;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_USER_TRUMPET,&UserTrumpet,sizeof(UserTrumpet));
	return true;
}

bool CAttemperEngineSink::OnDBGameSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_Send_Trumpet* pSendTrumpetResult = (DBO_GR_Send_Trumpet*)pData;
	if(pSendTrumpetResult->bSuccful)
	{
		//��������
		BYTE cbIndex=(pSendTrumpetResult->dwPropID==PROPERTY_ID_TRUMPET)?2:3;

		//����ṹ
		CMD_GR_S_SendTrumpet  SendTrumpet;
		SendTrumpet.dwSendUserID=pSendTrumpetResult->dwUserID;
		SendTrumpet.wPropertyIndex=(WORD)pSendTrumpetResult->dwPropID;
		SendTrumpet.TrumpetColor=pSendTrumpetResult->TrumpetColor;
		ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
		CopyMemory(SendTrumpet.szSendNickName,pSendTrumpetResult->szSendNickName,sizeof(SendTrumpet.szSendNickName));

		//�ַ�����
		SensitiveWordFilter(pSendTrumpetResult->szTrumpetContent,SendTrumpet.szTrumpetContent,CountArray(SendTrumpet.szTrumpetContent));

		//����ת��
		if(cbIndex==3)
		{
			//�㲥����
			if(m_pITCPSocketService)
			{
				m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet));
			}
		}

		//��Ϸת��
		if(cbIndex==2)
		{
			//��������
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_PROPERTY,SUB_GR_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet),BG_COMPUTER);
		}
	}
	return true;
}

//��Ա����
bool CAttemperEngineSink::OnDBGameMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_MemberParameter * pMemberParameter=(DBO_GR_MemberParameter *)pData;

	//��������	
	m_wMemberCount = pMemberParameter->wMemberCount;
	CopyMemory(m_MemberParameter,pMemberParameter->MemberParameter,sizeof(tagMemberParameterNew)*m_wMemberCount);

	return true;
}

//��Ա��ѯ
bool CAttemperEngineSink::OnDBPCMemberDayQueryInfoResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_MemberQueryInfoResult * pMemberResult=(DBO_GR_MemberQueryInfoResult *)pData;

	//����ṹ
	CMD_GR_MemberQueryInfoResult MemberResult;
	MemberResult.bPresent=pMemberResult->bPresent;
	MemberResult.bGift=pMemberResult->bGift;
	//��������	
	MemberResult.GiftSubCount = pMemberResult->GiftSubCount;
	CopyMemory(MemberResult.GiftSub,pMemberResult->GiftSub,sizeof(tagGiftPropertyInfo)*MemberResult.GiftSubCount);

	//��������
	WORD wSendDataSize=sizeof(MemberResult);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_QUERY_INFO_RESULT,&MemberResult,wSendDataSize);

	return true;
}

//��Ա�ͽ�
bool CAttemperEngineSink::OnDBPCMemberDayPresentResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_MemberDayPresentResult * pMemberResult=(DBO_GR_MemberDayPresentResult *)pData;

	//������Ϸ��
	if( pMemberResult->bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(pMemberResult->lGameScore-pIServerUserItem->GetUserScore(),0,0);
	}

	//����ṹ
	CMD_GR_MemberDayPresentResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	MemberResult.lGameScore=pMemberResult->lGameScore;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_DAY_PRESENT_RESULT,&MemberResult,wSendDataSize);

	return true;

}

//��Ա���
bool CAttemperEngineSink::OnDBPCMemberDayGiftResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_MemberDayGiftResult * pMemberResult=(DBO_GR_MemberDayGiftResult *)pData;

	//����ṹ
	CMD_GR_MemberDayGiftResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_MEMBER,SUB_GR_MEMBER_DAY_GIFT_RESULT,&MemberResult,wSendDataSize);
	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureInfo * pUserInsureInfo=(DBO_GR_UserInsureInfo *)pData;

	//��������
	CMD_GR_S_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//��������
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

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	return true;
}

//���гɹ�
bool CAttemperEngineSink::OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureSuccess * pUserInsureSuccess=(DBO_GR_UserInsureSuccess *)pData;

	//��������
	SCORE lFrozenedScore=pUserInsureSuccess->lFrozenedScore;
	SCORE lInsureRevenue=pUserInsureSuccess->lInsureRevenue;
	SCORE lVariationScore=pUserInsureSuccess->lVariationScore;
	SCORE lVariationInsure=pUserInsureSuccess->lVariationInsure;

	//�ⶳ����
	if ((lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//���в���
	if (pIServerUserItem->ModifyUserInsure(lVariationScore,lVariationInsure,lInsureRevenue)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CMD_GR_S_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//�������
	UserInsureSuccess.cbActivityGame=pUserInsureSuccess->cbActivityGame;
	UserInsureSuccess.lUserScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//���ʹ���
	DBO_GR_UserInsureFailure * pUserInsureFailure=(DBO_GR_UserInsureFailure *)pData;
	SendInsureFailure(pIServerUserItem,pUserInsureFailure->szDescribeString,pUserInsureFailure->lResultCode,pUserInsureFailure->cbActivityGame);

	//�ⶳ����
	if ((pUserInsureFailure->lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(pUserInsureFailure->lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserTransferUserInfo * pTransferUserInfo=(DBO_GR_UserTransferUserInfo *)pData;

	//��������
	CMD_GR_S_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//�������
	UserTransferUserInfo.cbActivityGame=pTransferUserInfo->cbActivityGame;
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szAccounts,pTransferUserInfo->szAccounts,CountArray(UserTransferUserInfo.szAccounts));

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_TRANSFER_USER_INFO,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

//��ͨ���
bool CAttemperEngineSink::OnDBUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureEnableResult * pUserInsureEnableResult=(DBO_GR_UserInsureEnableResult *)pData;

	//��������
	CMD_GR_S_UserInsureEnableResult UserInsureEnableResult;
	ZeroMemory(&UserInsureEnableResult,sizeof(UserInsureEnableResult));

	//�������
	UserInsureEnableResult.cbInsureEnabled=pUserInsureEnableResult->cbInsureEnabled;
	lstrcpyn(UserInsureEnableResult.szDescribeString,pUserInsureEnableResult->szDescribeString,CountArray(UserInsureEnableResult.szDescribeString));

	//�����С
	WORD wHeadSize=CountStringBuffer(UserInsureEnableResult.szDescribeString);
	wHeadSize+= sizeof(UserInsureEnableResult)-sizeof(UserInsureEnableResult.szDescribeString);

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_ENABLE_RESULT,&UserInsureEnableResult,wHeadSize);

	return true;
}

//���߳ɹ�
bool CAttemperEngineSink::OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_S_PropertySuccess * pPropertySuccess=(DBO_GR_S_PropertySuccess *)pData;

	//��ȡ�û�
	IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwSourceUserID);
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwTargetUserID);

	//�����û�
	if (pISourceUserItem!=NULL)
	{
		//��������
		SCORE lFrozenedScore=pPropertySuccess->lFrozenedScore;

		//�ⶳ����
		if ((lFrozenedScore>0L)&&(pISourceUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	if (pISourceUserItem!=NULL && pPropertySuccess->lSendLoveLiness != 0)
	{
		pISourceUserItem->ModifyUserProperty(0,(LONG)pPropertySuccess->lSendLoveLiness);
	}

	//��������
	if(pITargetUserItem!=NULL && pPropertySuccess->lRecvLoveLiness != 0)
	{
		pITargetUserItem->ModifyUserProperty(0,(LONG)pPropertySuccess->lRecvLoveLiness);
	}

	//��������
	DWORD dwCurrentTime=(DWORD)time(NULL);


	//��������
	if( pPropertySuccess->wPropKind == PROP_KIND_NEGATIVE_SCORE_CLEAR )
	{
		if( m_pGameServiceOption->wServerType&GAME_GENRE_SCORE )
		{
			//��������
			SCORE lCurrScore = pITargetUserItem->GetUserScore();
			if ( lCurrScore < 0)
			{
				//�û���Ϣ
				tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
				if(pUserInfo==NULL) return true;
				//�޸Ļ���
				pUserInfo->lScore=0;
				SendPropertyEffect(pITargetUserItem, PROP_KIND_NEGATIVE_SCORE_CLEAR);
			}
		}
		
	}
	//��������
	else if( pPropertySuccess->wPropKind == PROP_KIND_ESCAPE_CLEAR )
	{
		//�û���Ϣ
		tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
		if(pUserInfo==NULL) return true;

		//�޸�������
		if(pUserInfo->dwFleeCount > 0)
		{
			pUserInfo->dwFleeCount=0;
			SendPropertyEffect(pITargetUserItem, PROP_KIND_ESCAPE_CLEAR);
		}
	}
	//VIP��
	else if( pPropertySuccess->wPropKind == PROP_KIND_VIP )
	{
		//�û���Ϣ
		tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
		if(pUserInfo==NULL) return true;

		//���»�Ա
		pUserInfo->cbMemberOrder=pPropertySuccess->cbMemberOrder;

		//�޸�Ȩ��
		pITargetUserItem->ModifyUserRight(pPropertySuccess->dwUserRight,0);

		//������Ϣ
		SendPropertyEffect(pITargetUserItem, PROP_KIND_VIP);
	}



	//�㲥����
	if( pPropertySuccess->wPropKind==PROP_KIND_GIFT )
	{
		//��������
		CMD_GR_S_PropertySuccess PropertySuccess;
		ZeroMemory(&PropertySuccess,sizeof(PropertySuccess));

		//���ñ���
		PropertySuccess.wKind = pPropertySuccess->wPropKind;
		PropertySuccess.cbRequestArea=pPropertySuccess->cbRequestArea;
		PropertySuccess.wItemCount=pPropertySuccess->wItemCount;
		PropertySuccess.wPropertyIndex=pPropertySuccess->wPropertyIndex;
		PropertySuccess.dwSourceUserID=pPropertySuccess->dwSourceUserID;
		PropertySuccess.dwTargetUserID=pPropertySuccess->dwTargetUserID;

		//������Ϣ
		if (pISourceUserItem!=NULL)
		{
			SendData(pISourceUserItem,MDM_GR_PROPERTY,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
		}

		//������Ϣ
		if (pITargetUserItem!=NULL && pITargetUserItem!=pISourceUserItem)
		{
			SendData(pITargetUserItem,MDM_GR_PROPERTY,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
		}

		SendPropertyMessage(PropertySuccess.dwSourceUserID,PropertySuccess.dwTargetUserID,PropertySuccess.wPropertyIndex,
		PropertySuccess.wItemCount);
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_PropertyFailure));
	if(wDataSize!=sizeof(DBO_GR_PropertyFailure)) return false;

	//��ȡ����
	DBO_GR_PropertyFailure * pPropertyFailure = (DBO_GR_PropertyFailure *)pData;
	if(pPropertyFailure==NULL) return false;

	//����ṹ
	CMD_GR_GamePropertyFailure PropertyFailure;
	PropertyFailure.lErrorCode = pPropertyFailure->lErrorCode;
	lstrcpyn(PropertyFailure.szDescribeString,pPropertyFailure->szDescribeString,CountArray(PropertyFailure.szDescribeString));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_GAME_PROPERTY_FAILURE,&PropertyFailure,sizeof(PropertyFailure));

	return true;
}

//���߹�����
bool CAttemperEngineSink::OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_PropertyBuyResult * pPropertyBuyResult=(DBO_GR_PropertyBuyResult *)pData;

	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();

	//����ṹ
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

	//���췵��
	BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
	CMD_GR_S_BackpackProperty* pBackpackPropertyResult = (CMD_GR_S_BackpackProperty*)cbDataBuffer;

	//��ʼ������
	WORD dwDataBufferSize = sizeof(CMD_GR_S_BackpackProperty);
	pBackpackPropertyResult->dwUserID = pQueryBackpackResult->dwUserID;
	pBackpackPropertyResult->dwCount = pQueryBackpackResult->dwCount;
	pBackpackPropertyResult->dwStatus = pQueryBackpackResult->dwStatus;
	if(pQueryBackpackResult->dwCount > 0)
	{
		memcpy(pBackpackPropertyResult->PropertyInfo, pQueryBackpackResult->PropertyInfo, sizeof(tagBackpackProperty)*(pQueryBackpackResult->dwCount));
		dwDataBufferSize += sizeof(tagBackpackProperty)*(WORD)(pQueryBackpackResult->dwCount-1);
	}
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_PROPERTY_BACKPACK_RESULT,pBackpackPropertyResult,dwDataBufferSize);
	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_PropertyUse* pPropertyUseResult = (DBO_GR_PropertyUse*)pData;

	//����ṹ
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
		//���½��
		IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertyUseResult->dwUserID);
		if(pISourceUserItem) pISourceUserItem->ModifyUserInsure(pPropertyUseResult->lUseResultsGold, 0, 0);
	}

	//�ж�ʱ��
	if(pPropertyUseResult->UseResultsValidTime > 0)
	{
		//����Buff
		DWORD UserID = pPropertyUseResult->dwUserID;
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_GAME_BUFF,dwContextID,(void*)&UserID,sizeof(UserID));
	}

	//���ý��
	DBO_GR_S_PropertySuccess PropertySuccess={0};
	PropertySuccess.wItemCount = (WORD)pPropertyUseResult->wPropCount;
	PropertySuccess.wPropertyIndex = (WORD)pPropertyUseResult->dwPropID;
	PropertySuccess.dwSourceUserID = pPropertyUseResult->dwUserID;
	PropertySuccess.dwTargetUserID = pPropertyUseResult->dwRecvUserID;
	PropertySuccess.wPropKind = (WORD)pPropertyUseResult->dwPropKind;
	PropertySuccess.cbRequestArea = (BYTE)pPropertyUseResult->wServiceArea; //���ѷ�ʽ
	PropertySuccess.wServiceArea = pPropertyUseResult->wServiceArea;
	PropertySuccess.lSendLoveLiness = pPropertyUseResult->lSendLoveLiness;
	PropertySuccess.lRecvLoveLiness = pPropertyUseResult->lRecvLoveLiness;
	OnDBPropertySuccess(dwContextID, &PropertySuccess, sizeof(PropertySuccess));
	
	return true;
}

bool CAttemperEngineSink::OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����ṹ
	DBO_GR_QuerySendPresent* pQuerySendPresent = (DBO_GR_QuerySendPresent*)pData;
	CMD_GR_S_QuerySendPresent QuerySendPresentResult={0};
	QuerySendPresentResult.wPresentCount = pQuerySendPresent->wPresentCount;
	memcpy(QuerySendPresentResult.Present, pQuerySendPresent->Present, sizeof(pQuerySendPresent->wPresentCount*sizeof(pQuerySendPresent->Present[0])));

	WORD dwHeadSize=sizeof(QuerySendPresentResult)-sizeof(QuerySendPresentResult.Present);
	WORD dwDataSize=QuerySendPresentResult.wPresentCount*sizeof(QuerySendPresentResult.Present[0]);;
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_QUERY_SEND_PRESENT_RESULT,&QuerySendPresentResult,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_PropertyPresent* pPropertyPresent = (DBO_GR_PropertyPresent*)pData;

	//����ṹ
	CMD_GR_S_PropertyPresent PropertyPresentResult={0};
	PropertyPresentResult.dwUserID = pPropertyPresent->dwUserID;
	PropertyPresentResult.dwRecvGameID = pPropertyPresent->dwRecvGameID;
	PropertyPresentResult.dwPropID = pPropertyPresent->dwPropID;
	PropertyPresentResult.wPropCount = pPropertyPresent->wPropCount;
	PropertyPresentResult.wType = pPropertyPresent->wType;
	PropertyPresentResult.nHandleCode = pPropertyPresent->nHandleCode;

	lstrcpyn(PropertyPresentResult.szRecvNickName, pPropertyPresent->szRecvNickName,CountArray(PropertyPresentResult.szRecvNickName));
	lstrcpyn(PropertyPresentResult.szNotifyContent, pPropertyPresent->szNotifyContent,CountArray(PropertyPresentResult.szNotifyContent));
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_PROPERTY_PRESENT_RESULT,&PropertyPresentResult,sizeof(PropertyPresentResult));
	
	//�����û�
	IServerUserItem* pServerUserItem =  m_ServerUserManager.SearchUserItem(pPropertyPresent->dwRecvUserID);
	if(pServerUserItem != NULL)
	{
		WORD wBindIndex=pServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
		//����ṹ
		DBR_GR_QuerySendPresent QuerySendPresentRequest={0};
		QuerySendPresentRequest.dwUserID = pPropertyPresent->dwRecvUserID;
		QuerySendPresentRequest.dwClientAddr=pServerUserItem->GetClientAddr();
		//��������
		m_pIDBCorrespondManager->PostDataBaseRequest(pServerUserItem->GetUserID(),DBR_GR_QUERY_SEND_PRESENT,pBindParameter->dwSocketID,&QuerySendPresentRequest,sizeof(QuerySendPresentRequest));
	}
	
	return true;
}

bool CAttemperEngineSink::OnDBQueryGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����ṹ
	DBO_GR_GetSendPresent* pGetSendPresent = (DBO_GR_GetSendPresent*)pData;
	CMD_GR_S_GetSendPresent GetSendPresentResult={0};
	GetSendPresentResult.wPresentCount = pGetSendPresent->wPresentCount;
	memcpy(GetSendPresentResult.Present, pGetSendPresent->Present, pGetSendPresent->wPresentCount*sizeof(SendPresent));

	WORD dwHeadSize=sizeof(GetSendPresentResult)-sizeof(GetSendPresentResult.Present);
	WORD dwDataSize=GetSendPresentResult.wPresentCount*sizeof(GetSendPresentResult.Present[0]);
	
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_PROPERTY,SUB_GR_GET_SEND_PRESENT_RESULT,&GetSendPresentResult,dwHeadSize+dwDataSize);

	return true;
}

//������
bool CAttemperEngineSink::OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_PurchaseResult * pPurchaseResult=(DBO_GR_PurchaseResult *)pData;

	//����ṹ
	CMD_GR_PurchaseResult PurchaseResult;
	ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

	//���ñ���
	PurchaseResult.bSuccessed=pPurchaseResult->bSuccessed;
	PurchaseResult.lCurrScore=pPurchaseResult->lCurrScore;
	PurchaseResult.dCurrBeans=pPurchaseResult->dCurrBeans;
	PurchaseResult.cbMemberOrder=pPurchaseResult->cbMemberOrder;
	lstrcpyn(PurchaseResult.szNotifyContent,pPurchaseResult->szNotifyContent,CountArray(PurchaseResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_PURCHASE_RESULT,&PurchaseResult,wSendDataSize);

	//����ж�
	if(PurchaseResult.bSuccessed==true)
	{
		//�޸Ļ�Ա�ȼ�
		tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->cbMemberOrder=pPurchaseResult->cbMemberOrder;
		pUserInfo->dBeans=pPurchaseResult->dCurrBeans;
		SendUserInfoPacket(pIServerUserItem,dwContextID);

		//����Ȩ��
		pIServerUserItem->ModifyUserRight(pPurchaseResult->dwUserRight,0,UR_KIND_SERVER);		

		//������Ϸ��
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

//�һ����
bool CAttemperEngineSink::OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_ExchangeResult * pExchangeResult=(DBO_GR_ExchangeResult *)pData;

	//����ṹ
	CMD_GR_ExchangeResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//���ñ���
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lCurrIngot=pExchangeResult->lCurrIngot;
	ExchangeResult.dCurrBeans=pExchangeResult->dCurrBeans;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_RESULT,&ExchangeResult,wSendDataSize);

	//����Ԫ��
	if (ExchangeResult.bSuccessed==true)
	{
		//�޸���Ϣ
		tagUserInfo *pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->lIngot=pExchangeResult->lCurrIngot;
		pUserInfo->dBeans=pExchangeResult->dCurrBeans;
	}

	//������Ϸ��
	if(/*m_pGameServiceOption->wServerType==GAME_GENRE_GOLD && */ExchangeResult.bSuccessed==true)
	{
		SCORE lCurrScore=pIServerUserItem->GetUserScore();
		SCORE lVarInsureScore=pExchangeResult->lCurrScore-lCurrScore;
		pIServerUserItem->ModifyUserInsure(lVarInsureScore,0,0);
	}

	return true;
}

//ǩ����Ϣ
bool CAttemperEngineSink::OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//��������
	DBO_GR_CheckInInfo * pCheckInInfo=(DBO_GR_CheckInInfo *)pData;

	//��������
	CMD_GR_CheckInInfo CheckInInfo;
	ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

	//�������
	CheckInInfo.bTodayChecked=pCheckInInfo->bTodayChecked;
	CheckInInfo.wSeriesDate=pCheckInInfo->wSeriesDate;
	CopyMemory(CheckInInfo.lRewardGold,m_lCheckInReward,sizeof(CheckInInfo.lRewardGold));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_CHECKIN,SUB_GR_CHECKIN_INFO,&CheckInInfo,sizeof(CheckInInfo));

	return true;
}

//ǩ�����
bool CAttemperEngineSink::OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_CheckInResult * pCheckInResult=(DBO_GR_CheckInResult *)pData;

	//��������
	CMD_GR_CheckInResult CheckInResult;
	ZeroMemory(&CheckInResult,sizeof(CheckInResult));

	//�������
	CheckInResult.bSuccessed=pCheckInResult->bSuccessed;
	CheckInResult.lScore=pCheckInResult->lScore;
	lstrcpyn(CheckInResult.szNotifyContent,pCheckInResult->szNotifyContent,CountArray(CheckInResult.szNotifyContent));

	//������Ϸ��
	if( CheckInResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(CheckInResult.lScore-pIServerUserItem->GetUserScore(),0,0);
	}


	//��������
	WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_CHECKIN,SUB_GR_CHECKIN_RESULT,&CheckInResult,wSendSize);

	return true;
}
//����ǩ��
bool CAttemperEngineSink::OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_CheckInReward));
	if(wDataSize!=sizeof(DBO_GR_CheckInReward)) return false;

	//��ȡ����
	DBO_GR_CheckInReward * pCheckInReward=(DBO_GR_CheckInReward *)pData;

	//��������
	CopyMemory(m_lCheckInReward,pCheckInReward->lRewardGold,sizeof(m_lCheckInReward));

	return true;
}

//�ͱ�����
bool CAttemperEngineSink::OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_BaseEnsureParameter * pEnsureParameter=(DBO_GR_BaseEnsureParameter *)pData;

	//���ñ���
	m_BaseEnsureParameter.cbTakeTimes = pEnsureParameter->cbTakeTimes;
	m_BaseEnsureParameter.lScoreAmount = pEnsureParameter->lScoreAmount;
	m_BaseEnsureParameter.lScoreCondition = pEnsureParameter->lScoreCondition;

	//��ȡ״̬
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	enServiceStatus ServiceStatus=pServiceUnits->GetServiceStatus();
	//�¼�֪ͨ
	if (ServiceStatus!=ServiceStatus_Service)
	{
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}

	return true;
}

//�ͱ����
bool CAttemperEngineSink::OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_BaseEnsureResult * pBaseEnsureResult=(DBO_GR_BaseEnsureResult *)pData;

	//����ṹ
	CMD_GR_BaseEnsureResult BaseEnsureResult;
	BaseEnsureResult.bSuccessed=pBaseEnsureResult->bSuccessed;
	BaseEnsureResult.lGameScore=pBaseEnsureResult->lGameScore;
	lstrcpyn(BaseEnsureResult.szNotifyContent,pBaseEnsureResult->szNotifyContent,CountArray(BaseEnsureResult.szNotifyContent));

	//������Ϸ��
	if( BaseEnsureResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(BaseEnsureResult.lGameScore-pIServerUserItem->GetUserScore(),0,0);
	}

	//��������
	WORD wSendDataSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
	wSendDataSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_BASEENSURE,SUB_GR_BASEENSURE_RESULT,&BaseEnsureResult,wSendDataSize);

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_TaskListInfo * pTaskListInfo=(DBO_GR_TaskListInfo *)pData;

	//��������	
	WORD wTaskCount = __min(pTaskListInfo->wTaskCount, CountArray(m_TaskParameter)-m_wTaskCount);
	CopyMemory(m_TaskParameter+m_wTaskCount, ++pTaskListInfo, sizeof(tagTaskParameter)*wTaskCount);

	//���ñ���
	m_wTaskCount += wTaskCount;

	return true;
}

//�������
bool CAttemperEngineSink::OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�Ƴ�����
	m_UserTaskManager.RemoveTaskParameter();

	//�������
	m_UserTaskManager.AddTaskParameter(m_TaskParameter, m_wTaskCount);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskInfo));
	if (wDataSize>sizeof(DBO_GR_TaskInfo)) return false;

	//��������
	DBO_GR_TaskInfo * pTaskInfo=(DBO_GR_TaskInfo *)pData;

	//��������
	WORD wTaskCount=m_wTaskCount;
	WORD wSendSize=0,wParameterSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	LPBYTE pDataBuffer = cbDataBuffer;

	//��������
	pDataBuffer += sizeof(wTaskCount);
	wSendSize = sizeof(wTaskCount);

	//�ְ�����
	for(WORD i=0;i<wTaskCount;i++)
	{
		//Խ���ж�
		if(wSendSize+sizeof(tagTaskParameter)>SOCKET_TCP_PACKET)
		{
			//���ñ���
			pDataBuffer = cbDataBuffer;
			wTaskCount -= i;
			*(WORD*)pDataBuffer = i; 

			//��������
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,&cbDataBuffer,wSendSize);

			//���ñ���
			pDataBuffer += sizeof(wSendSize);
			wSendSize = wSendSize;
		}

		//�����С
		wParameterSize = sizeof(tagTaskParameter)-sizeof(m_TaskParameter[i].szTaskDescribe)+CountStringBuffer(m_TaskParameter[i].szTaskDescribe);
		*(WORD *)pDataBuffer = wParameterSize;
		pDataBuffer += sizeof(wParameterSize);
		wSendSize += sizeof(wParameterSize);

		//��������
		CopyMemory(pDataBuffer,&m_TaskParameter[i],wParameterSize);

		//�ƽ�ָ��
		pDataBuffer += wParameterSize;
		wSendSize += wParameterSize;
	}

	//ʣ�෢��
	if(wTaskCount>0 && wSendSize)
	{
		//���ñ���
		pDataBuffer = cbDataBuffer;
		*(WORD *)pDataBuffer = wTaskCount; 

		//��������
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);
	}

	//����ṹ
	CMD_GR_S_TaskInfo TaskInfo;
	TaskInfo.wTaskCount = pTaskInfo->wTaskCount;
	CopyMemory(TaskInfo.TaskStatus,pTaskInfo->TaskStatus,sizeof(TaskInfo.TaskStatus[0])*pTaskInfo->wTaskCount);

	//��������
	WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
	wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_INFO,&TaskInfo,wSendDataSize);

	return true;
}

//������
bool CAttemperEngineSink::OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskResult));
	if(wDataSize>sizeof(DBO_GR_TaskResult)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_TaskResult * pTaskResult=(DBO_GR_TaskResult *)pData;

	//��������
	CMD_GR_S_TaskResult TaskResult;
	ZeroMemory(&TaskResult,sizeof(TaskResult));

	//�������
	TaskResult.wCommandID=pTaskResult->wCommandID;
	TaskResult.bSuccessed=pTaskResult->bSuccessed;
	TaskResult.lCurrScore=pTaskResult->lCurrScore;
	TaskResult.lCurrIngot=pTaskResult->lCurrIngot;
	lstrcpyn(TaskResult.szNotifyContent,pTaskResult->szNotifyContent,CountArray(TaskResult.szNotifyContent));

	//��������
	WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_RESULT,&TaskResult,wSendSize);

	//������Ϣ
	if (TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_REWARD)
	{
		//��������
		tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
		pUserInfo->lIngot=pTaskResult->lCurrIngot;

		SCORE lOldScore=pIServerUserItem->GetUserScore();
		lOldScore+=pIServerUserItem->GetFrozenedScore();
		lOldScore+=pIServerUserItem->GetTrusteeScore();
		pIServerUserItem->ModifyUserInsure(pTaskResult->lCurrScore-lOldScore,0,0);
	}

	//��������
	if(TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_TAKE)
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.SearchTaskParameter(pTaskResult->wCurrTaskID);
		if(pTaskParameter!=NULL && pTaskParameter->wKindID==m_pGameServiceOption->wKindID)
		{
			//��������
			tagUserTaskInfo UserTaskInfo;

			//��������		
			UserTaskInfo.wTaskProgress=0;
			UserTaskInfo.cbTaskStatus=TASK_STATUS_UNFINISH;
			UserTaskInfo.wTaskID=pTaskResult->wCurrTaskID;
			UserTaskInfo.dwResidueTime=pTaskParameter->dwTimeLimit;
			UserTaskInfo.dwLastUpdateTime=(DWORD)time(NULL);

			//��������
			m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),&UserTaskInfo,1);
		}
	}

	return true;
}

//ϵͳ��Ϣ
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBR_GR_SystemMessage));
	if(wDataSize!=sizeof(DBR_GR_SystemMessage)) return false;

	//��ȡ����
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

	//�������
	tagSystemMessage  *pSendMessage=new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//���ñ���
	pSendMessage->dwLastTime=(DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage,pSystemMessage, sizeof(DBR_GR_SystemMessage));

	//��¼��Ϣ
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//�������
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������ʱ��Ϣ
	if(m_SystemMessageBuffer.GetCount()>0)
	{
		//��������
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

//�������д�
bool CAttemperEngineSink::OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if(dwContextID==0xfffe)
	{
		m_WordsFilter.ResetSensitiveWordArray();
		m_pITimerEngine->KillTimer(IDI_LOAD_SENSITIVE_WORD);
		return true;			
	}

	//�������
	if(dwContextID==0xffff)
	{
		m_WordsFilter.FinishAdd();
		return true;
	}

	//�������д�
	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.AddSensitiveWords(pWords);
	return true;
}

//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GrowLevelConfig * pGrowLevelConfig=(DBO_GR_GrowLevelConfig *)pData;

	//��������	
	m_wLevelCount = pGrowLevelConfig->wLevelCount;
	CopyMemory(m_GrowLevelConfig,&pGrowLevelConfig->GrowLevelConfig,sizeof(tagGrowLevelConfig)*m_wLevelCount);

	return true;
}

//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_GrowLevelParameter * pGrowLevelParameter=(DBO_GR_GrowLevelParameter *)pData;

	//����ṹ
	CMD_GR_GrowLevelParameter GrowLevelParameter;
	GrowLevelParameter.wCurrLevelID=pGrowLevelParameter->wCurrLevelID;	
	GrowLevelParameter.dwExperience=pGrowLevelParameter->dwExperience;
	GrowLevelParameter.dwUpgradeExperience=pGrowLevelParameter->dwUpgradeExperience;
	GrowLevelParameter.lUpgradeRewardGold=pGrowLevelParameter->lUpgradeRewardGold;
	GrowLevelParameter.lUpgradeRewardIngot=pGrowLevelParameter->lUpgradeRewardIngot;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_USER,SUB_GR_GROWLEVEL_PARAMETER,&GrowLevelParameter,sizeof(GrowLevelParameter));

	return true;
}

//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_GrowLevelUpgrade * pGrowLevelUpgrade=(DBO_GR_GrowLevelUpgrade *)pData;

	//����ṹ
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

	//��������
	WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
	wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_USER,SUB_GR_GROWLEVEL_UPGRADE,&GrowLevelUpgrade,wSendDataSize);

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CreateSuccess));
	if(wDataSize != sizeof(DBO_GR_CreateSuccess)) return false;

	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateSuccess* pCreateSuccess = (DBO_GR_CreateSuccess*)pData;

	//��ȡ��Ϣ
	CTableFrame* pTableFrame = m_TableFrameArray[pCreateSuccess->dwTableID];
	WORD wChairID = pTableFrame->GetNullChairID();

	//��ʼ����
	LONGLONG lIniScore = 0;
	byte cbIsJoinGame = 0;
	INT_PTR nSize = m_PersonalTableParameterArray.GetCount();
	for(INT_PTR i = 0; i < nSize; ++i)
	{
		tagPersonalTableParameter* pTableParameter = m_PersonalTableParameterArray.GetAt(i);
		if(pTableParameter->dwPlayTurnCount == pCreateSuccess->dwDrawCountLimit && pTableParameter->dwPlayTimeLimit == pCreateSuccess->dwDrawTimeLimit)
		{
			lIniScore = pTableParameter->lIniScore;

			//��������Ϣ��Ϊ�ƶ���������Ϣ
			pTableParameter->wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;	//�μ���Ϸ���������
			pTableParameter->lCellScore = pCreateSuccess->lCellScore;													//˽�˷������׷�

			pTableParameter->dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//˽�˷Ž�����Ϸ��������
			pTableParameter->dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;							//˽�˷�������Ϸ�����ʱ�� ��λ��

			//cbIsJoinGame = pTableParameter->cbIsJoinGame ;

			break;
		}
	}
	cbIsJoinGame = m_PersonalRoomOption.cbIsJoinGame;
	pTableFrame->SetCellScore(pCreateSuccess->lCellScore);
	pTableFrame->SetGameRule(pCreateSuccess->cbGameRule, RULE_LEN);


	//���ʹ�õ��ǽ�����ݿ�
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


	//��������������Ϣ
	tagPersonalTableParameter PersonalTableParameter;
	PersonalTableParameter.lIniScore = lIniScore;
	PersonalTableParameter.wJoinGamePeopleCount = pCreateSuccess->wJoinGamePeopleCount;		//�μ���Ϸ���������
	PersonalTableParameter.lCellScore = pCreateSuccess->lCellScore;													//˽�˷������׷�
	PersonalTableParameter.dwPlayTurnCount = pCreateSuccess->dwDrawCountLimit; 						//˽�˷Ž�����Ϸ��������
	PersonalTableParameter.dwPlayTimeLimit = pCreateSuccess->dwDrawTimeLimit;								//˽�˷�������Ϸ�����ʱ�� ��λ��
	PersonalTableParameter.cbIsJoinGame = cbIsJoinGame;

	PersonalTableParameter.dwTimeAfterBeginCount = m_PersonalRoomOption.dwTimeAfterBeginCount;
	PersonalTableParameter.dwTimeAfterCreateRoom = m_PersonalRoomOption.dwTimeAfterCreateRoom;
	PersonalTableParameter.dwTimeNotBeginGame = m_PersonalRoomOption.dwTimeNotBeginGame;
	PersonalTableParameter.dwTimeOffLineCount = m_PersonalRoomOption.dwTimeOffLineCount;

	pTableFrame->SetPersonalTableParameter(PersonalTableParameter, m_PersonalRoomOption);

	//תת�齫����һ�����������ӵĸ���
	if (m_pGameServiceAttrib->wKindID == ZZMJ_KIND_ID || m_pGameServiceAttrib->wKindID  == HZMJ_KIND_ID)
	{
		if (pCreateSuccess->cbGameRule[0] == SET_RULE) 
		{
			pTableFrame->SetTableChairCount(pCreateSuccess->cbGameRule[1]);
		}
	}

	//��������	
	tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
	pUserInfo->dBeans = pCreateSuccess->dCurBeans;
	pUserInfo->lRoomCard = pCreateSuccess->lRoomCard;
	pTableFrame->SetTableOwner(pUserInfo->dwUserID);
	pTableFrame->SetTimerNotBeginAfterCreate();

	tagUserRule* pUserRule = pIServerUserItem->GetUserRule();
	lstrcpyn(pUserRule->szPassword, pCreateSuccess->szPassword, CountArray(pUserRule->szPassword));

	//�������������Ϸ
	if (pCreateSuccess->cbIsJoinGame)
	{

		//ִ������
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, pCreateSuccess->szPassword)== FALSE)
		{
			CTraceService::TraceString(TEXT("������������ʧ��"), TraceLevel_Info);
			//��������
			pTableFrame->SetPersonalTableLlocked(false);
			pTableFrame->SetPersonalTable(0, 0, 0);
			pTableFrame->SetCellScore(0);
			pTableFrame->SetTableOwner(0);

			//�˻�����
			DBR_GR_CancelCreateTable CancelCreateTable;
			ZeroMemory(&CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			CancelCreateTable.dwUserID = pCreateSuccess->dwUserID;
			CancelCreateTable.dwClientAddr = pBindParameter->dwClientAddr;
			CancelCreateTable.dwTableID = pCreateSuccess->dwTableID;
			CancelCreateTable.dwReason = CANCELTABLE_REASON_SYSTEM;
			CancelCreateTable.dwDrawCountLimit = pCreateSuccess->dwDrawCountLimit;
			CancelCreateTable.dwDrawTimeLimit = pCreateSuccess->dwDrawTimeLimit;
			CancelCreateTable.dwServerID = m_pGameServiceOption->wServerID;

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(pCreateSuccess->dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwContextID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));

			return true;
		}
	}




	//���ݻ���
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

	//��������
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_CREATE_TABLE, &CS_CreateTable, sizeof(CMD_CS_C_CreateTable));


	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CreateFailure));
	if(wDataSize != sizeof(DBO_GR_CreateFailure)) return false;

	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	DBO_GR_CreateFailure* pCreateFailure = (DBO_GR_CreateFailure*)pData;

	//��������
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = pCreateFailure->lErrorCode;
	lstrcpyn(CreateFailure.szDescribeString, pCreateFailure->szDescribeString, CountArray(CreateFailure.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}

//ȡ������
bool CAttemperEngineSink::OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_CancelCreateResult));
	if(wDataSize != sizeof(DBO_GR_CancelCreateResult)) return false;

	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if(pBindParameter == NULL) return true;
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_CancelCreateResult* pCancelCreateResult = (DBO_GR_CancelCreateResult*)pData;
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelCreateResult->dwTableID];
	ASSERT(pTableFrame != NULL);

	//��������
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

		//�󶨲���
		WORD wBindIndex = pUserItem->GetBindIndex();
		tagBindParameter* pBind = GetBindParameter(wBindIndex);

		//��������
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = pCancelCreateResult->dwReason;
		if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_SYSTEM)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸ�Զ���ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_PLAYER)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸδ��ʼ����Ϸ�Զ���ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_ENFOCE)
			lstrcpyn(CancelTable.szDescribeString, TEXT("�����˳���Ϸ����Ϸ��ʱ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_OVER_TIME)
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸ��ʱ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));
		else if(pCancelCreateResult->dwReason == CANCELTABLE_REASON_NOT_START)
			lstrcpyn(CancelTable.szDescribeString, TEXT("Լս�涨��ʼʱ�䵽δ��ʼ��Ϸ����Ϸ��ɢ��"), CountArray(CancelTable.szDescribeString));
		
		if (CANCELTABLE_REASON_NOT_START == pCancelCreateResult->dwReason || CANCELTABLE_REASON_OVER_TIME == pCancelCreateResult->dwReason)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		}
		//��ɢ��Ϣ
		m_pITCPNetworkEngine->SendData(pBind->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//�û�״̬
		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		pTableFrame->PerformStandUpAction(pUserItem);
	}

	////��������
	//CMD_GR_CancelTable CancelTable;
	//ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
	//CancelTable.dwReason = pCancelCreateResult->dwReason;
	//lstrcpyn(CancelTable.szDescribeString, pCancelCreateResult->szDescribeString, CountArray(CancelTable.szDescribeString));

	////��������
	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

	////�˳��û�
	//pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);

	return true;
}

//˽������
bool CAttemperEngineSink::OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagPersonalTableParameter)==0);
	if (wDataSize%sizeof(tagPersonalTableParameter)!=0) return false;

	//�������
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

	//����ת��
	WORD wCount = wDataSize/sizeof(tagPersonalTableParameter);
	tagPersonalTableParameter* pParameterData = (tagPersonalTableParameter*)pData;

	//��������
	for(int i = 0; i < wCount; ++i)
	{
		tagPersonalTableParameter* pPersonalTableParameter = new tagPersonalTableParameter;
		CopyMemory(pPersonalTableParameter,&pParameterData[i],sizeof(tagPersonalTableParameter));
		m_PersonalTableParameterArray.Add(pPersonalTableParameter);
	}

	return true;
}

//˽������
bool CAttemperEngineSink::OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GR_DissumeResult));
	if (sizeof(DBO_GR_DissumeResult)!=wDataSize) return false;
	DBO_GR_DissumeResult * pDissumeResult = (DBO_GR_DissumeResult *)pData;
	CMD_GR_DissumeTable  DissumeTable;
	DissumeTable.cbIsDissumSuccess = pDissumeResult->cbIsDissumSuccess;
	lstrcpyn(DissumeTable.szRoomID , pDissumeResult->szRoomID, sizeof(DissumeTable.szRoomID));
	DissumeTable.sysDissumeTime = pDissumeResult->sysDissumeTime;

	//��ȡ������ҵ���Ϣ
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		memcpy(&DissumeTable.PersonalUserScoreInfo[i],  &pDissumeResult->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
	}

	m_pITCPNetworkEngine->SendData(pDissumeResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_HOST_DISSUME_TABLE_RESULT, &DissumeTable, sizeof(CMD_GR_DissumeTable));

	return true;

}

//˽�˷���������󷿼���Ϣ
bool CAttemperEngineSink::OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	//ASSERT(wDataSize%sizeof(tagQueryPersonalRoomUserScore)==0);
	//if (wDataSize%sizeof(tagQueryPersonalRoomUserScore)!=0) return false;

	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_USER_QUERY_ROOM_SCORE_RESULT, pData, wDataSize);

	return true;

}

//˽�˷���������󷿼���Ϣ
bool CAttemperEngineSink::OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GR_CurrenceRoomCardAndBeans));
	if (sizeof(DBO_GR_CurrenceRoomCardAndBeans)!=wDataSize) return false;
	DBO_GR_CurrenceRoomCardAndBeans * pCardAndBeans = (DBO_GR_CurrenceRoomCardAndBeans *)pData;
	CMD_GR_CurrenceRoomCardAndBeans  CurrenceRoomCardAndBeans;
	CurrenceRoomCardAndBeans.dbBeans = pCardAndBeans->dbBeans;
	CurrenceRoomCardAndBeans.lRoomCard = pCardAndBeans->lRoomCard;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_PERSONAL_TABLE, SUB_GR_CURRECE_ROOMCARD_AND_BEAN, &CurrenceRoomCardAndBeans, sizeof(CMD_GR_CurrenceRoomCardAndBeans));

	return true;

}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();		

			//ע�����
			if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				//����ṹ
				CMD_CS_C_RegisterMatch RegitsterMatch;
				ZeroMemory(&RegitsterMatch,sizeof(RegitsterMatch));

				RegitsterMatch.wServerID = m_pGameServiceOption->wServerID;
				//������Ϣ
				RegitsterMatch.dwMatchID=m_pGameMatchOption->dwMatchID;
				RegitsterMatch.lMatchNo=m_pGameMatchOption->lMatchNo;	
				RegitsterMatch.cbMatchStatus=m_pGameMatchOption->cbMatchStatus;
				RegitsterMatch.cbMatchType=m_pGameMatchOption->cbMatchType;	
				lstrcpyn(RegitsterMatch.szMatchName,m_pGameMatchOption->szMatchName,CountArray(RegitsterMatch.szMatchName));
				//������Ϣ
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
				//������ʽ
				RegitsterMatch.cbRankingMode=m_pGameMatchOption->cbRankingMode;
				RegitsterMatch.wCountInnings=m_pGameMatchOption->wCountInnings;
				RegitsterMatch.cbFilterGradesMode=m_pGameMatchOption->cbFilterGradesMode;
				//��������
				RegitsterMatch.cbDistributeRule=m_pGameMatchOption->cbDistributeRule;
				RegitsterMatch.wMinDistributeUser=m_pGameMatchOption->wMinDistributeUser;
				RegitsterMatch.wDistributeTimeSpace=m_pGameMatchOption->wDistributeTimeSpace;
				RegitsterMatch.wMinPartakeGameUser=m_pGameMatchOption->wMinPartakeGameUser;
				RegitsterMatch.wMaxPartakeGameUser=m_pGameMatchOption->wMaxPartakeGameUser;

				CopyMemory(&RegitsterMatch.cbMatchRule,&m_pGameMatchOption->cbMatchRule,sizeof(RegitsterMatch.cbMatchRule));
				RegitsterMatch.wRewardCount=m_pGameMatchOption->wRewardCount;
				CopyMemory(&RegitsterMatch.MatchRewardInfo,m_pGameMatchOption->MatchRewardInfo,sizeof(RegitsterMatch.MatchRewardInfo));			

				//��������
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_MATCH,&RegitsterMatch,sizeof(RegitsterMatch));
			}

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//��������
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);

			//��������
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
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
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_CREATE_TABLE_RESULT:		//�������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_CreateTableResult));
			if (wDataSize!=sizeof(CMD_CS_S_CreateTableResult)) return false;

			//��������
			CMD_CS_S_CreateTableResult * pCreateTableResult=(CMD_CS_S_CreateTableResult *)pData;

			//��ȡ�û�
			WORD wBindIndex=LOWORD(pCreateTableResult->dwSocketID);
			IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
			if (pIServerUserItem==NULL) return false;

			if(pCreateTableResult->PersonalTable.dwDrawCountLimit == 0 && pCreateTableResult->PersonalTable.dwDrawTimeLimit == 0)
			{
				//��������
				CMD_GR_CreateFailure CreateFailure;
				ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

				CreateFailure.lErrorCode = 1;
				lstrcpyn(CreateFailure.szDescribeString, TEXT("���������������ȷ��"), CountArray(CreateFailure.szDescribeString));

				//��������
				m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

				return false;
			}

			//��ȡ����
			CTableFrame* pTableFrame = m_TableFrameArray[pCreateTableResult->PersonalTable.dwTableID];
			ASSERT(pTableFrame != NULL);

			tagPersonalTableParameter PersonalTableParameter = pTableFrame->GetPersonalTableParameter();
			pTableFrame->SetPersonalTableID(pCreateTableResult->PersonalTable.szRoomID);

			//��������
			CMD_GR_CreateSuccess CreateSuccess;
			ZeroMemory(&CreateSuccess, sizeof(CMD_GR_CreateSuccess));

			CreateSuccess.dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
			CreateSuccess.dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
			CreateSuccess.dBeans = pIServerUserItem->GetUserInfo()->dBeans;
			CreateSuccess.lRoomCard = pIServerUserItem->GetUserInfo()->lRoomCard;
			lstrcpyn(CreateSuccess.szServerID, pCreateTableResult->PersonalTable.szRoomID, CountArray(CreateSuccess.szServerID));
			
			m_pITCPNetworkEngine->SendData(pCreateTableResult->dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_SUCCESS, &CreateSuccess, sizeof(CMD_GR_CreateSuccess));


			//�������Ӵ�����¼
			DBR_GR_InsertCreateRecord  CreateRecord;
			ZeroMemory(&CreateRecord, sizeof(DBR_GR_InsertCreateRecord));

			//��������
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
	case SUB_CS_C_DISMISS_TABLE_RESULT:		//��ɢ���
		{
			ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
			if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;

			CMD_CS_C_DismissTableResult* pDismissTable = (CMD_CS_C_DismissTableResult*)pData;
			//��ȡ����
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
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ClearUser));
			if (wDataSize!=sizeof(CMD_CS_S_ClearUser)) return false;

			//��������
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
							pTableFrame->SendRoomMessage(pIUserItem,TEXT("�����˺�Ϊ�쳣�˺ţ��Ѿ���ϵͳ������!"),SMT_EJECT|SMT_CHAT|SMT_GLOBAL);
							pTableFrame->PerformStandUpAction(pIUserItem);
							//pIUserItem->SetUserStatus(US_NULL,pIUserItem->GetTableID(),pIUserItem->GetChairID());
						}
					}
					break;
				}
			}
			if(bOK == true)
			{
				str.Format(_T("����ɹ��գӣţңɣ�_%d"),pClearUser->dwUserID);
			}
			else
			{
				str.Format(_T("����δ�ɹ��գӣţңɣ�_%d"),pClearUser->dwUserID);
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

//�����¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//�û�����
		{
			//��������
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//�����û�
			WORD wIndex=0;
			do
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//���ñ���
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				UserEnter.dwGameID=pIServerUserItem->GetGameID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//������Ϣ
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();
				UserEnter.cbAndroidUser=pIServerUserItem->IsAndroidUser();

				//��������
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));

			} while (true);

			//�㱨���
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:	//ϵͳ��Ϣ
		{
			//��Ϣ����
			SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);

			return true;
		}
	case SUB_CS_S_PROPERTY_TRUMPET:  //������Ϣ
		{
			//��������
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_PROPERTY,SUB_GR_PROPERTY_TRUMPET,pData,wDataSize,BG_COMPUTER);

			return true;
		}
	case SUB_CS_S_PLATFORM_PARAMETER: //ƽ̨����
		{
			//�������			
			m_wTaskCount=0;
			ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//��Ա����
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_MEMBER_PARAMETER,0,NULL,0);	

			//�ɳ�����
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GROWLEVEL_CONFIG,0,NULL,0);	

			//����ǩ��
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_CHECKIN_REWARD,0,NULL,0);	

			//���صͱ�
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_BASEENSURE,0L,NULL,0L);
			return true;
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_ADDPARAMETER:		//��Ӳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_S_AddParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_AddParameter * pAddParameter = (CMD_CS_S_AddParameter *)pData;

			//��Ӳ���
			m_AndroidUserManager.AddAndroidParameter(&pAddParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_MODIFYPARAMETER:  //�޸Ĳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_S_ModifyParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_ModifyParameter * pModifyParameter = (CMD_CS_S_ModifyParameter *)pData;

			//�޸Ĳ���
			m_AndroidUserManager.AddAndroidParameter(&pModifyParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_DELETEPARAMETER:  //ɾ������
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_S_DeleteParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_DeleteParameter * pDeleteParameter = (CMD_CS_S_DeleteParameter *)pData;

			//ɾ������
			m_AndroidUserManager.RemoveAndroidParameter(pDeleteParameter->dwBatchID);

			return true;
		}
	}

	return true;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//I D ��¼
		{
			return OnTCPNetworkSubLogonUserID(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_MOBILE:		//�ֻ���¼
		{
			return OnTCPNetworkSubLogonMobile(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubLogonAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//�û�����
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_LOOKON:		//�û��Թ�
		{
			return OnTCPNetworkSubUserLookon(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_SITDOWN:		//�û�����
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_STANDUP:		//�û�����
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAT:			//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_EXPRESSION:	//�û�����
		{
			return OnTCPNetworkSubUserExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_CHAT:		//�û�˽��
		{
			return OnTCPNetworkSubWisperChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_EXPRESSION:	//˽�ı���
		{
			return OnTCPNetworkSubWisperExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INVITE_REQ:    //�����û�
		{
			return OnTCPNetworkSubUserInviteReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_REPULSE_SIT:   //�ܾ�����
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_KICK_USER:    //�߳��û�
		{
			return OnTCPNetworkSubMemberKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INFO_REQ:     //�����û���Ϣ
		{
			return OnTCPNetworkSubUserInfoReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //�������λ��
		{
			//���뿪ʼ�������ݣ�����������Դֻ��һ���̵߳���
			EnterCriticalSection(&m_cs_test); 
			printf("SUB_GR_USER_CHAIR_REQ_____1\n");
			bool bOK = false;
			bOK =  OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
			if(bOK == true)
			{
				//��������		
				WORD wUserIndex=0;
				bool bSendAndroidFlag=false;
				IServerUserItem * pIServerUserItemSend=NULL;
				//��ȡ�û�
				WORD wBindIndex=LOWORD(dwSocketID);
				IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
				if(pIServerUserItem != NULL)
				{
					//Ȩ���ж�		
					if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
					CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
					{
						bSendAndroidFlag=true;
					}

					//���������������û����Լ������Լ����⣩		
					while (true)
					{
						pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
						if (pIServerUserItemSend==NULL) break;
						if (pIServerUserItemSend==pIServerUserItem) continue;
						SendUserInfoPacket(pIServerUserItemSend,dwSocketID,bSendAndroidFlag);
					}
				}				
			}

			//�뿪���������ݣ���������Դ���������̵߳���
						printf("SUB_GR_USER_CHAIR_REQ_____2\n");
			LeaveCriticalSection(&m_cs_test);

			return bOK;
		}
	case SUB_GR_USER_CHAIR_INFO_REQ: //���������û���Ϣ
		{
			return OnTCPNetworkSubChairUserInfoReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GROWLEVEL_QUERY:		  //��ѯ�ȼ�
		{
			return OnTCPNetworkSubGrowLevelQuery(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//���д���
bool CAttemperEngineSink::OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_GR_ENABLE_INSURE_REQUEST:	//��ͨ����
		{
			return OnTCPNetworkSubEnableInsureRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_INSURE_INFO:		//���в�ѯ
		{
			return OnTCPNetworkSubQueryInsureInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SAVE_SCORE_REQUEST:		//�������
		{
			return OnTCPNetworkSubSaveScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TAKE_SCORE_REQUEST:		//ȡ������
		{
			return OnTCPNetworkSubTakeScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TRANSFER_SCORE_REQUEST:	//ת������
		{
			return OnTCPNetworkSubTransferScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_USER_INFO_REQUEST://��ѯ�û�
		{
			return OnTCPNetworkSubQueryUserInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_TASK_TAKE:				//��ȡ����
		{
			return OnTCPNetworkSubTakeTaskRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_REWARD:			//��ȡ����
		{
			return OnTCPNetworkSubTaskRewardRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_LOAD_INFO:			//��������
		{
			return OnTCPNetworkSubLoadTaskInfoRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_GIVEUP:			//��������
		{
			return OnTCPNetworkSubGiveUpTaskRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�һ�����
bool CAttemperEngineSink::OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_EXCHANGE_LOAD_INFO:		//��ѯ��Ϣ
		{
			return OnTCPNetworkSubQueryExchangeInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PURCHASE_MEMBER:		//�����Ա
		{
			return OnTCPNetworkSubPurchaseMember(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE_BYINGOT:	//�һ���Ϸ��
		{
			return OnTCPNetworkSubExchangeScoreByIngot(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE_BYBEANS:	//�һ���Ϸ��
		{
			return OnTCPNetworkSubExchangeScoreByBeans(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�ͱ�����
bool CAttemperEngineSink::OnTCPNetworkMainBaseEnsure( WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	switch (wSubCmdID)
	{
	case SUB_GR_BASEENSURE_LOAD:				//���صͱ�
		{
			return OnTCPNetworkSubBaseEnsureQueryRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_BASEENSURE_TAKE:			//��ȡ�ͱ�
		{
			return OnTCPNetworkSubBaseEnsureTakeRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��Ա����
bool CAttemperEngineSink::OnTCPNetworkMainMember( WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	switch (wSubCmdID)
	{
	case SUB_GR_MEMBER_QUERY_INFO:		//��Ա��ѯ
		{
			return OnTCPNetworkSubMemberQueryInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_MEMBER_DAY_PRESENT:		//��Ա�ͽ�
		{
			return OnTCPNetworkSubMemberDayPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_MEMBER_DAY_GIFT:		//��Ա���
		{
			return OnTCPNetworkSubMemberDayGift(pData,wDataSize,dwSocketID);
		}

	}

	return false;
}


//���ߴ���
bool CAttemperEngineSink::OnTCPNetworkMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_GAME_PROPERTY_BUY:	//�������
		{
			return OnTCPNetworkSubGamePropertyBuy(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_USE:		//����ʹ��
		{
			return OnTCPNetworkSubPropertyUse(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_SEND_PRESENT:	//��ѯ����
		{
			return	OnTCPNetworkSubQuerySendPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_PRESENT:	//���͵���
		{
			return OnTCPNetworkSubPropertyPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GET_SEND_PRESENT:	//��ȡ����
		{
			return OnTCPNetworkSubGetSendPresent(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_BACKPACK:	//��������
		{
			return OnTCPNetworkSubPropertyBackpack(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_TRUMPET:   //���ȵ���
		{
			return OnTCPNetwordSubSendTrumpet(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//ǩ������
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

//������
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_OPTION:		//��ѯ����
		{
			return OnTCPNetworkSubQueryOption(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_OPTION_SERVER:		//��������
		{
			return OnTCPNetworkSubOptionServer(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KILL_USER:          //�߳��û�
		{
			return OnTCPNetworkSubManagerKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LIMIT_USER_CHAT:	//��������
		{
			return OnTCPNetworkSubLimitUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KICK_ALL_USER:		//�߳��û�
		{
			return OnTCPNetworkSubKickAllUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_MESSAGE:		//������Ϣ
		{
			return OnTCPNetworkSubSendMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_DISMISSGAME:        //��ɢ��Ϸ
		{
			return OnTCPNetworkSubDismissGame(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_WARNING:		//�����û�
		{
			return OnTCPNetworkSubWarningUser(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//Ч��ӿ�
	ASSERT(m_pIMatchServiceManager!=NULL);
	if (m_pIMatchServiceManager==NULL) return false;

	//��Ϣ����
	return m_pIMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//˽�˷���
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
	case SUB_GR_HOSTL_DISSUME_TABLE://����ǿ�ƽ�ɢ����
		{
			return OnTCPNetworkSubHostDissumeTable(pData, wDataSize, dwSocketID);
		}
		//case SUB_GR_USER_QUERY_ROOM_SCORE: //���󷿼�ɼ�
		//	{
		//		return OnTCPNetworkSubQueryUserRoomScore(pData, wDataSize, dwSocketID);
		//	}
	}
	return false;
}

//��Ϸ����
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ܴ���
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���뿪ʼ�������ݣ�����������Դֻ��һ���̵߳���
	EnterCriticalSection(&m_cs_test);    
	printf("OnTCPNetworkMainFrame_____1\n");
	bool bOK = false;
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	bOK =  pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
	//�뿪���������ݣ���������Դ���������̵߳���
	printf("OnTCPNetworkMainFrame_____2\n");
	LeaveCriticalSection(&m_cs_test);

	return bOK;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonUserID));
	if (wDataSize<sizeof(CMD_GR_LogonUserID)) return false;

	//������Ϣ
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//����У��
	if(((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0) && (LOWORD(dwSocketID)<INDEX_ANDROID))
	{
		//����Ƚ�
		if(lstrcmp(pLogonUserID->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
		{
			//����ʧ��
			SendLogonFailure(TEXT("��Ǹ��������ķ������벻��ȷ�����������룡"),FAILURE_TYPE_ROOM_PASSWORD_INCORRECT,dwSocketID);

			return true;
		}
	}

	//�����ж�
	if(pLogonUserID->wKindID != m_pGameServiceOption->wKindID)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ������Ϸ�����Ѿ��ر��ˣ�������������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return true;
		}
	}

	//�����ж�
	CString strPassword = pLogonUserID->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonUserID->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonUserID->dwProcessVersion;

	//�����ж�
	if(LOWORD(dwSocketID) < INDEX_ANDROID)
	{
		//�����汾
		DWORD dwPlazaVersion=pLogonUserID->dwPlazaVersion;
		DWORD dwFrameVersion=pLogonUserID->dwFrameVersion;
		DWORD dwClientVersion=pLogonUserID->dwProcessVersion;
		if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;
	}

	//�л��ж�

	if((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonUserID->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonUserID->szMachineID,wBindIndex);
		return true;
	}

	//����Ч��
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,false))
	{
		return true;
	}

	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwMatchID=m_pGameMatchOption->dwMatchID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));

	return true;
}

//�ֻ���¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonMobile));
	if (wDataSize<sizeof(CMD_GR_LogonMobile)) return false;

	//������Ϣ
	CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
	pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
	pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//����У��
	if(((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0) && (LOWORD(dwSocketID)<INDEX_ANDROID))
	{
		//����Ƚ�
		if(lstrcmp(pLogonMobile->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
		{
			//����ʧ��
			SendLogonFailure(TEXT("��Ǹ��������ķ������벻��ȷ�����������룡"),FAILURE_TYPE_ROOM_PASSWORD_INCORRECT,dwSocketID);

			return true;
		}
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonMobile->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//�����ж�
	CString strPassword = pLogonMobile->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->dwProcessVersion=pLogonMobile->dwProcessVersion;

	//�����汾
	DWORD dwClientVersion=pLogonMobile->dwProcessVersion;
	if (PerformCheckVersion(0L,0L,dwClientVersion,dwSocketID)==false) return true;

	//�л��ж�
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonMobile->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonMobile->szMachineID,wBindIndex,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);
		return true;
	}

	//����Ч��
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,true))
	{
		return true;
	}

	//��������
	DBR_GR_LogonMobile LogonMobile;
	ZeroMemory(&LogonMobile,sizeof(LogonMobile));

	//��������
	LogonMobile.dwUserID=pLogonMobile->dwUserID;
	LogonMobile.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
	lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));
	LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
	LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
	LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;

	//˽�˷���
	if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		LogonMobile.cbPersonalServer = 1;
	}
	else
	{
		LogonMobile.cbPersonalServer = 0;
	}

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonAccounts));
	if (wDataSize<=sizeof(CMD_GR_LogonAccounts)) return false;

	//������Ϣ
	CMD_GR_LogonAccounts * pLogonAccounts=(CMD_GR_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if (pIBindUserItem!=NULL)
	{ 
		ASSERT(FALSE);
		return false;
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//�����ж�
	CString strPassword = pLogonAccounts->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonAccounts->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonAccounts->dwProcessVersion;

	//�����汾
	DWORD dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	DWORD dwFrameVersion=pLogonAccounts->dwFrameVersion;
	DWORD dwClientVersion=pLogonAccounts->dwProcessVersion;
	if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;

	//�л��ж�
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonAccounts->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonAccounts->szMachineID,wBindIndex);
		return true;
	}

	//��������
	DBR_GR_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//��������
	LogonAccounts.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true||m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) return true;

	//��Ϣ����
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//�����־
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//��������
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//��������
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//��������
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//��ȡ����
		while (true)
		{
			//��ȡ����
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//���ݷ���
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//��������
				{
					//Ч������
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//�����ж�
					ASSERT(CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==false);
					if (CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==true) break;

					//��������
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

//�û��Թ�
bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserLookon));
	if (wDataSize!=sizeof(CMD_GR_UserLookon)) return false;

	//Ч������
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//Ч������
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�ظ��ж�
	if ((pUserSitDown->wTableID<m_pGameServiceOption->wTableCount)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//�������
		InsertDistribute(pIServerUserItem);	
		
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		if(m_TableFrameArray[0]->EfficacyEnterTableScoreRule(0, pIServerUserItem))
		{
			InsertDistribute(pIServerUserItem);
		}

		return true;
	}

	//�������
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	DWORD wTailChairID = INVALID_CHAIR;
	//���ӵ���
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//��ʼ����
		WORD wStartTableID=0;
		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
		if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//��̬����
		bool bDynamicJoin=true;
		if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
		if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true && pIServerUserItem->GetTailGameID() != 0)
		{
			//Ѱ��λ��
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
								//���ñ���
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
				//��ȡ��λ
				WORD wNullChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();
				//wNullChairID = INVALID_CHAIR;
				if (wNullChairID == INVALID_CHAIR)
				{
					//��Ϸ״̬
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
										//���ñ���
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
					//���´���
					CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
					pTableFrame->PerformLookonAction(wTailChairID,pIServerUserItem);
					return true;
				}
			}
			else
			{
				//Ѱ��λ��
				for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
				{
					//��Ϸ״̬
					if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

					//��ȡ��λ
					WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

					//�������
					if (wNullChairID!=INVALID_CHAIR)
					{
						//���ñ���
						wRequestTableID=i;
						wRequestChairID=wNullChairID;

						break;
					}
				}
			}

		}
		else
		{
			//Ѱ��λ��
			for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
			{
				//��Ϸ״̬
				if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

				//��ȡ��λ
				WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

				//�������
				if (wNullChairID!=INVALID_CHAIR)
				{
					//���ñ���
					wRequestTableID=i;
					wRequestChairID=wNullChairID;

					break;
				}
			}
		}


		//����ж�
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�"),0);
			return true;
		}
	}

	//���ӵ���
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//Ч�����
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//���ҿ�λ
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//����ж�
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),0);
			return true;
		}
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;

	//ȡ������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		//ȡ������
		DeleteDistribute(pIServerUserItem);

		if(pUserStandUp->wTableID==INVALID_TABLE) return true;
	}

	//Ч������
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//�û��ж�
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//Լս�ж�
	CTableFrame* pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
	ASSERT(pTableFrame != NULL);
	if(pTableFrame->GetDrawCount() != 0 && m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		SendRequestFailure(pIServerUserItem,TEXT("Լսδ�����������뿪��Ϸ���������Լս���뿪��"),0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_UserChat * pUserChat=(CMD_GR_C_UserChat *)pData;

	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GR_C_UserChat));
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

	//Ч�����
	if (wDataSize>sizeof(CMD_GR_C_UserChat)) return false;
	if (wDataSize<(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"),SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pUserChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//������Ϣ
	CMD_GR_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//�ַ�����
	SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));
	
	//��������
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pUserChat->dwSendUserID;
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_UserExpression));
	if (wDataSize!=sizeof(CMD_GR_C_UserExpression)) return false;

	//��������
	CMD_GR_C_UserExpression * pUserExpression=(CMD_GR_C_UserExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserExpression->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserExpression->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"),SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pUserExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//������Ϣ
	CMD_GR_S_UserExpression UserExpression;
	ZeroMemory(&UserExpression,sizeof(UserExpression));

	//��������
	UserExpression.wItemIndex=pUserExpression->wItemIndex;
	UserExpression.dwSendUserID=pUserExpression->dwSendUserID;
	UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(BG_ALL_CLIENT,MDM_GR_USER,SUB_GR_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));

	return true;
}

//�û�˽��
bool CAttemperEngineSink::OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_WisperChat * pWisperChat=(CMD_GR_C_WisperChat *)pData;

	//Ч�����
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0])));

	//Ч�����
	if (wDataSize<(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperChat->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//״̬�ж�
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û�˽�ģ�"),SMT_CHAT);
		return true;
	}

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pWisperChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//��������
	CMD_GR_S_WisperChat WisperChat;
	ZeroMemory(&WisperChat,sizeof(WisperChat));

	//�ַ�����
	SensitiveWordFilter(pWisperChat->szChatString,WisperChat.szChatString,CountArray(WisperChat.szChatString));

	//��������
	WisperChat.dwChatColor=pWisperChat->dwChatColor;
	WisperChat.wChatLength=pWisperChat->wChatLength;
	WisperChat.dwSendUserID=pISendUserItem->GetUserID();
	WisperChat.dwTargetUserID=pIRecvUserItem->GetUserID();
	WisperChat.wChatLength=CountStringBuffer(WisperChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(WisperChat)-sizeof(WisperChat.szChatString);
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//ת����Ϣ
	WisperChat.dwSendUserID=pWisperChat->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//�����ж�
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//ö���û�
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

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_WisperExpression));
	if (wDataSize!=sizeof(CMD_GR_C_WisperExpression)) return false;

	//��������
	CMD_GR_C_WisperExpression * pWisperExpression=(CMD_GR_C_WisperExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperExpression->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//״̬�ж�
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û�˽�ģ�"),SMT_CHAT);
		return true;
	}

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pWisperExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//��������
	CMD_GR_S_WisperExpression WisperExpression;
	ZeroMemory(&WisperExpression,sizeof(WisperExpression));

	//��������
	WisperExpression.wItemIndex=pWisperExpression->wItemIndex;
	WisperExpression.dwSendUserID=pISendUserItem->GetUserID();
	WisperExpression.dwTargetUserID=pWisperExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//ת����Ϣ
	WisperExpression.dwSendUserID=pWisperExpression->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//�����ж�
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//ö���û�
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




//�������
bool CAttemperEngineSink::OnTCPNetworkSubGamePropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_PropertyBuy));
	if (wDataSize!=sizeof(CMD_GR_PropertyBuy)) return false;

	//��������
	CMD_GR_PropertyBuy * pPropertyBuy=(CMD_GR_PropertyBuy *)pData;

	//����Ч��
	ASSERT(pPropertyBuy->dwItemCount>0);
	if (pPropertyBuy->dwItemCount==0) return false;

	//��������
	DBR_GR_PropertyBuy PropertyRequest;
	ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//������Ϣ
	PropertyRequest.dwUserID=pPropertyBuy->dwUserID;
	PropertyRequest.dwPropertyID=pPropertyBuy->dwPropertyID;
	PropertyRequest.dwItemCount=pPropertyBuy->dwItemCount;
	PropertyRequest.cbConsumeType = pPropertyBuy->cbConsumeType;
	PropertyRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(PropertyRequest.szPassword,pPropertyBuy->szPassword,CountArray(PropertyRequest.szPassword));
	lstrcpyn(PropertyRequest.szMachineID,pPropertyBuy->szMachineID,CountArray(PropertyRequest.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_GAME_PROPERTY_BUY,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));
	
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyBackpack(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_BackpackProperty));
	if (wDataSize!=sizeof(CMD_GR_C_BackpackProperty)) return false;

	CMD_GR_C_BackpackProperty* pBackpackProperty = (CMD_GR_C_BackpackProperty*)pData;

	DBR_GR_QueryBackpack QueryBackpack;
	QueryBackpack.dwUserID = pBackpackProperty->dwUserID;
	QueryBackpack.dwKindID = pBackpackProperty->dwKindID;
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//ϵͳ��Ϣ
	QueryBackpack.dwClientAddr=pIServerUserItem->GetClientAddr();

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_BACKPACK,dwSocketID,&QueryBackpack,sizeof(QueryBackpack));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyUse(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyUse));
	if (wDataSize!=sizeof(CMD_GR_C_PropertyUse)) return false;

	//��������
	CMD_GR_C_PropertyUse * pPropertyUse = (CMD_GR_C_PropertyUse *)pData;

	//����Ч��
	ASSERT(pPropertyUse->wPropCount>0);
	if (pPropertyUse->wPropCount==0) return false;

	//��������
	DBR_GR_PropertyUse PropertyUseRequest;
	ZeroMemory(&PropertyUseRequest,sizeof(PropertyUseRequest));
	PropertyUseRequest.dwUserID = pPropertyUse->dwUserID;
	PropertyUseRequest.dwRecvUserID = pPropertyUse->dwRecvUserID;
	PropertyUseRequest.wPropCount = pPropertyUse->wPropCount;
	PropertyUseRequest.dwPropID = pPropertyUse->dwPropID;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//ϵͳ��Ϣ
	PropertyUseRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
	
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_USE,dwSocketID,&PropertyUseRequest,sizeof(PropertyUseRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubQuerySendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QuerySendPresent));
	if (wDataSize!=sizeof(CMD_GR_C_QuerySendPresent)) return false;

	//��������
	CMD_GR_C_QuerySendPresent *pQuerySendPresent = (CMD_GR_C_QuerySendPresent *)pData;
	DBR_GR_QuerySendPresent QuerySendPresentRequest={0};
	QuerySendPresentRequest.dwUserID = pQuerySendPresent->dwUserID;
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//ϵͳ��Ϣ
	QuerySendPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_SEND_PRESENT,dwSocketID,&QuerySendPresentRequest,sizeof(QuerySendPresentRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubPropertyPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyPresent));
	if (wDataSize!=sizeof(CMD_GR_C_PropertyPresent)) return false;

	//��������
	CMD_GR_C_PropertyPresent *PropertyPresent = (CMD_GR_C_PropertyPresent *)pData;
	DBR_GR_PropertyPresent PropertyPresentRequest={0};
	PropertyPresentRequest.dwUserID = PropertyPresent->dwUserID;
	PropertyPresentRequest.dwRecvGameID = PropertyPresent->dwRecvGameID;
	PropertyPresentRequest.dwPropID = PropertyPresent->dwPropID;
	PropertyPresentRequest.wPropCount = PropertyPresent->wPropCount;
	PropertyPresentRequest.wType = PropertyPresent->wType;
	lstrcpyn(PropertyPresentRequest.szRecvNickName,PropertyPresent->szRecvNickName,CountArray(PropertyPresentRequest.szRecvNickName));
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//ϵͳ��Ϣ
	PropertyPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_PRESENT,dwSocketID,&PropertyPresentRequest,sizeof(PropertyPresentRequest));
	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubGetSendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_GetSendPresent));
	if (wDataSize!=sizeof(CMD_GR_C_GetSendPresent)) return false;

	//��������
	CMD_GR_C_GetSendPresent *pGetSendPresent = (CMD_GR_C_GetSendPresent *)pData;
	DBR_GR_GetSendPresent GetSendPresentRequest={0};
	GetSendPresentRequest.dwUserID = pGetSendPresent->dwUserID;
	lstrcpyn(GetSendPresentRequest.szPassword,pGetSendPresent->szPassword,CountArray(GetSendPresentRequest.szPassword));
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//ϵͳ��Ϣ
	GetSendPresentRequest.dwClientAddr=pIServerUserItem->GetClientAddr();

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_GET_SEND_PRESENT,dwSocketID,&GetSendPresentRequest,sizeof(GetSendPresentRequest));

	return true;
}


//ʹ�õ���
bool CAttemperEngineSink::OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_SendTrumpet));
	if (wDataSize!=sizeof(CMD_GR_C_SendTrumpet)) return false;

	//��������
	CMD_GR_C_SendTrumpet * pSendTrumpet=(CMD_GR_C_SendTrumpet *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if(pIServerUserItem==NULL) return false;

	//��������
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		//������Ϣ
		SendPropertyFailure(pIServerUserItem,TEXT("�������䲻����ʹ�ô˹��ܣ�"),0L,pSendTrumpet->cbRequestArea);
		return RESULT_FAIL;
	}

	//��ϰ����
	if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
	{
		SendPropertyFailure(pIServerUserItem,TEXT("��ϰ���䲻����ʹ�ô˹��ܣ�"),0L,pSendTrumpet->cbRequestArea);
		return RESULT_FAIL;
	}

	//����ṹ
	DBR_GR_Send_Trumpet SendTrumpetRequest;
	ZeroMemory(&SendTrumpetRequest,sizeof(SendTrumpetRequest));

	SendTrumpetRequest.dwUserID = pIServerUserItem->GetUserID();
	SendTrumpetRequest.dwRecvUserID = pIServerUserItem->GetUserID();
	SendTrumpetRequest.wPropCount =1;
	SendTrumpetRequest.dwPropID = pSendTrumpet->wPropertyIndex;
	SendTrumpetRequest.TrumpetColor = pSendTrumpet->TrumpetColor;//������ɫ
	lstrcpyn(SendTrumpetRequest.szTrumpetContent,pSendTrumpet->szTrumpetContent,CountArray(SendTrumpetRequest.szTrumpetContent));
	lstrcpyn(SendTrumpetRequest.szSendNickName,pIServerUserItem->GetNickName(),CountArray(SendTrumpetRequest.szSendNickName));
	SendTrumpetRequest.dwClientAddr=pIServerUserItem->GetClientAddr();//ϵͳ��Ϣ

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_SEND_TRUMPET,dwSocketID,&SendTrumpetRequest,sizeof(SendTrumpetRequest));

	return true;
}

//�����û�
bool CAttemperEngineSink::OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//��Ϣ����
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//������Ϣ
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

	return true;
}

//�ܾ�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//��ȡ�û�
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("��������������˲�����ͬ����Ϸ��"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//�������
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//�߳�����
bool CAttemperEngineSink::OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//Ȩ���ж�
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//���߹���Ա
	if(pITargetUserItem->GetMasterOrder() > 0)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ���߳�����Ա�ǲ�������ģ�"),SMT_EJECT);
		return true;
	}

	//������Ϸ
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"),SMT_EJECT);
		return true;
	}

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ������Ϸ��,�����ܽ����߳���Ϸ��"),pITargetUserItem->GetNickName());

		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	//���ҷ��߿�
	tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE_TICK);
	if(pPropertyBuff != NULL)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ����ʹ�÷��߿�,���޷������߳���Ϸ��"),pITargetUserItem->GetNickName());
		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true; 
	}

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//�������
		TCHAR szMessage[64]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("���ѱ�%s�������ӣ�"),pIServerUserItem->GetNickName());

		//������Ϣ
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//�����û���Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_UserInfoReq)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL)
	{
		CTraceService::TraceString(TEXT("OnTCPNetworkSubUserInfoReq"),TraceLevel_Exception);

		return false;
	}

	//��������
	CMD_GR_UserInfoReq * pUserInfoReq = (CMD_GR_UserInfoReq *)pData;
	WORD wNewDeskPos = pUserInfoReq->wTablePos;
	WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-m_pInitParameter->m_wVisibleTableCount;

	//����Ч��
	if(wNewDeskPos > wMaxDeskPos) wNewDeskPos = wMaxDeskPos;

	//������Ϣ
	pIServerUserItem->SetMobileUserDeskPos(wNewDeskPos);
	
	//������Ϣ
	SendVisibleTableUserInfoToMobileUser(pIServerUserItem,wNewDeskPos);

	return true;
}

//�������λ��
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL)
	{
		CTraceService::TraceString(TEXT("OnTCPNetworkSubUserChairReq"),TraceLevel_Exception);
		return false;
	}

	//�û�״̬
	printf("���״̬_%d\n",pIServerUserItem->GetUserStatus());
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//ʧ��
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	//������ʾ
	TCHAR szDescribe[128]=TEXT("");
	if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD  && pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore)
	{
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore);
		//������Ϣ
		if (pIServerUserItem->IsAndroidUser()==true)
			SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_CLOSE_ROOM|SMT_EJECT);
		else
			SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

		//�û�����
		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}
		return true;
	}
	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;
	DWORD wRequestTableID = INVALID_CHAIR;
	DWORD wRequestChairID = INVALID_CHAIR;
	DWORD wTailChairID = INVALID_CHAIR;
	if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true && pIServerUserItem->GetTailGameID() != 0)
	{
		//Ѱ��λ��
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
							//���ñ���
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
			//��ȡ��λ
			WORD wNullChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();
			if (wNullChairID == INVALID_CHAIR)
			{
				//��Ϸ״̬
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
									//���ñ���
									pTableFrame->PerformStandUpAction(pIServerUser);
									wRequestChairID=j;

									//�뿪����
									if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
									{
										CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
										if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
									}

									//�û�����
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

				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//�û�����
				return m_TableFrameArray[wRequestTableID]->PerformSitDownAction(wRequestChairID,pIServerUserItem);;
			}
			if(wRequestChairID == INVALID_CHAIR)
			{
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//���´���
				CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
				return pTableFrame->PerformLookonAction(wTailChairID,pIServerUserItem);
			}
		}
	}
	//��������
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=((INT)i+nStartIndex)%(INT)m_pGameServiceOption->wTableCount;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//�û�����
		return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
	}

	//��������
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=((INT)i+nStartIndex)%(INT)m_pGameServiceOption->wTableCount;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//�û�����
		return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
	}

	//����ͬ��
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//��Ч����
			WORD wTableID=pIServerUserItem->GetTableID();
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//�û�����
				return pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);;
			}
		}
	}

	//ʧ��
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	return true;
}

//���������û���Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubChairUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_ChairUserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_ChairUserInfoReq)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_ChairUserInfoReq * pUserInfoReq = (CMD_GR_ChairUserInfoReq *)pData;
	if(pUserInfoReq->wTableID == INVALID_TABLE) return true;
	if(pUserInfoReq->wTableID >= m_pGameServiceOption->wTableCount)return true;

	//������Ϣ
	WORD wChairCout = m_TableFrameArray[pUserInfoReq->wTableID]->GetChairCount();
	for(WORD wIndex = 0; wIndex < wChairCout; wIndex++)
	{
		//��ȡ�û�
		if(pUserInfoReq->wChairID != INVALID_CHAIR && wIndex != pUserInfoReq->wChairID)continue;
		IServerUserItem * pTagerIServerUserItem=m_TableFrameArray[pUserInfoReq->wTableID]->GetTableUserItem(wIndex);
		if(pTagerIServerUserItem==NULL)continue;

		//��������
		BYTE cbBuffer[SOCKET_TCP_PACKET]={0};
		tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));
		tagUserInfo *pUserInfo = pTagerIServerUserItem->GetUserInfo();

		//�û�����
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
		pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

		//�û�����
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//�û�״̬
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;
		pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

		//�û��ɼ�
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pTagerIServerUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pTagerIServerUserItem->GetFrozenedScore();
		pUserInfoHead->dBeans=pUserInfo->dBeans;
		pUserInfoHead->lIngot=pUserInfo->lIngot;
		pUserInfoHead->bAndroid = pTagerIServerUserItem->IsAndroidUser();
		lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

		//������Ϣ
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//������Ϣ
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//�ȼ���ѯ
bool CAttemperEngineSink::OnTCPNetworkSubGrowLevelQuery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	ASSERT(pBindParameter!=NULL);
	if (pBindParameter==NULL) return true;


	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_GrowLevelQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_GrowLevelQueryInfo)) return false;

	//��ȡ����
	CMD_GR_GrowLevelQueryInfo * pGrowLevelQueryInfo= (CMD_GR_GrowLevelQueryInfo *)pData;
	pGrowLevelQueryInfo->szPassword[CountArray(pGrowLevelQueryInfo->szPassword)-1]=0;
	pGrowLevelQueryInfo->szMachineID[CountArray(pGrowLevelQueryInfo->szMachineID)-1]=0;


	//����ṹ
	DBR_GR_GrowLevelQueryInfo GrowLevelQueryInfo;
	GrowLevelQueryInfo.dwUserID = pGrowLevelQueryInfo->dwUserID;
	GrowLevelQueryInfo.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(GrowLevelQueryInfo.szPassword,pGrowLevelQueryInfo->szPassword,CountArray(GrowLevelQueryInfo.szPassword));
	lstrcpyn(GrowLevelQueryInfo.szMachineID,pGrowLevelQueryInfo->szMachineID,CountArray(GrowLevelQueryInfo.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pGrowLevelQueryInfo->dwUserID,DBR_GR_GROWLEVEL_QUERY_IFNO,dwSocketID,&GrowLevelQueryInfo,sizeof(GrowLevelQueryInfo));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����ṹ
	CMD_GR_ExchangeParameter ExchangeParameter;
	ZeroMemory(&ExchangeParameter,sizeof(ExchangeParameter));

	//���ñ���
	ExchangeParameter.wMemberCount=m_wMemberCount;
	ExchangeParameter.dwExchangeRate=m_pGameParameter->dwExchangeRate;
	ExchangeParameter.dwPresentExchangeRate=m_pGameParameter->dwPresentExchangeRate;	
	ExchangeParameter.dwRateGold=m_pGameParameter->dwRateGold;	
	CopyMemory(ExchangeParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameter)*m_wMemberCount);

	//�����С
	WORD wSendDataSize = sizeof(ExchangeParameter)-sizeof(ExchangeParameter.MemberParameter);
	wSendDataSize += sizeof(tagMemberParameter)*ExchangeParameter.wMemberCount;

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_PARAM_INFO,&ExchangeParameter,wSendDataSize);

	return true;
}

//�����Ա
bool CAttemperEngineSink::OnTCPNetworkSubPurchaseMember(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_PurchaseMember));
	if(wDataSize!=sizeof(CMD_GR_PurchaseMember)) return false;

	//����ṹ
	CMD_GR_PurchaseMember * pPurchaseMember = (CMD_GR_PurchaseMember*)pData;
	pPurchaseMember->szMachineID[CountArray(pPurchaseMember->szMachineID)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_PurchaseMember PurchaseMember;
	ZeroMemory(&PurchaseMember,sizeof(PurchaseMember));

	//���ñ���
	PurchaseMember.dwUserID=pPurchaseMember->dwUserID;
	PurchaseMember.cbMemberOrder=pPurchaseMember->cbMemberOrder;
	PurchaseMember.wPurchaseTime=pPurchaseMember->wPurchaseTime;
	PurchaseMember.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(PurchaseMember.szMachineID,pPurchaseMember->szMachineID,CountArray(PurchaseMember.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(PurchaseMember.dwUserID,DBR_GR_PURCHASE_MEMBER,dwSocketID,&PurchaseMember,sizeof(PurchaseMember));

	return true;
}

//�һ���Ϸ��
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScoreByIngot(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScoreByIngot));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScoreByIngot)) return false;

	//����ṹ
	CMD_GR_ExchangeScoreByIngot * pExchangeScore = (CMD_GR_ExchangeScoreByIngot*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����У��
	ASSERT(pExchangeScore->lExchangeIngot>0);
	if (pExchangeScore->lExchangeIngot<=0) return false;

	//����ṹ
	DBR_GR_ExchangeScoreByIngot ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//���ñ���
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.lExchangeIngot=pExchangeScore->lExchangeIngot;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE_INGOT,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//�һ���Ϸ��
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScoreByBeans(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScoreByBeans));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScoreByBeans)) return false;

	//����ṹ
	CMD_GR_ExchangeScoreByBeans * pExchangeScore = (CMD_GR_ExchangeScoreByBeans*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_ExchangeScoreByBeans ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//���ñ���
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.dExchangeBeans=pExchangeScore->dExchangeBeans;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE_BEANS,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//��������
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//��������
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(QueryInsureInfo.szPassword,pQueryInsureInfoRequest->szInsurePass,CountArray(QueryInsureInfo.szPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return true;
}

//��ͨ����
bool CAttemperEngineSink::OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_EnableInsureRequest));
	if (wDataSize!=sizeof(CMD_GR_C_EnableInsureRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_EnableInsureRequest * pEnableInsureRequest = (CMD_GR_C_EnableInsureRequest *)pData;

	//��������
	DBR_GR_UserEnableInsure UserEnableInsure;
	ZeroMemory(&UserEnableInsure,sizeof(UserEnableInsure));

	//��������	
	UserEnableInsure.dwUserID=pIServerUserItem->GetUserID();
	UserEnableInsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserEnableInsure.cbActivityGame=pEnableInsureRequest->cbActivityGame;
	lstrcpyn(UserEnableInsure.szLogonPass,pEnableInsureRequest->szLogonPass,CountArray(UserEnableInsure.szLogonPass));
	lstrcpyn(UserEnableInsure.szInsurePass,pEnableInsureRequest->szInsurePass,CountArray(UserEnableInsure.szInsurePass));
	lstrcpyn(UserEnableInsure.szMachineID,pEnableInsureRequest->szMachineID,CountArray(UserEnableInsure.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(UserEnableInsure.dwUserID,DBR_GR_USER_ENABLE_INSURE,dwSocketID,&UserEnableInsure,sizeof(UserEnableInsure));

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_SaveScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_SaveScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_SaveScoreRequest * pSaveScoreRequest=(CMD_GR_C_SaveScoreRequest *)pData;

	//Ч�����
	ASSERT(pSaveScoreRequest->lSaveScore>0L);
	if (pSaveScoreRequest->lSaveScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendSaveInRoom(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ������������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸ���������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	SCORE lConsumeQuota=0L;
	SCORE lUserWholeScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();

	//��ȡ�޶�
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		lConsumeQuota=m_TableFrameArray[wTableID]->QueryConsumeQuota(pIServerUserItem);
	}
	else
	{
		lConsumeQuota=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	}

	//�޶��ж�
	if (pSaveScoreRequest->lSaveScore>lConsumeQuota)
	{
		if (lConsumeQuota<lUserWholeScore)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������������Ϸ�У���Ϸ�ҿɴ�����Ϊ %.2f���������ʧ�ܣ�"),lConsumeQuota);

			//��������
			SendInsureFailure(pIServerUserItem,szDescribe,0L,pSaveScoreRequest->cbActivityGame);
		}
		else
		{
			//��������
			SendInsureFailure(pIServerUserItem,TEXT("������Ϸ�����㣬�������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		}

		return true;
	}

	//��������
	if (pIServerUserItem->FrozenedUserScore(pSaveScoreRequest->lSaveScore)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DBR_GR_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

	//��������
	UserSaveScore.cbActivityGame=pSaveScoreRequest->cbActivityGame;
	UserSaveScore.dwUserID=pIServerUserItem->GetUserID();
	UserSaveScore.lSaveScore=pSaveScoreRequest->lSaveScore;
	UserSaveScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserSaveScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserSaveScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

	return true;
}

//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_TakeScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_TakeScoreRequest * pTakeScoreRequest=(CMD_GR_C_TakeScoreRequest *)pData;
	pTakeScoreRequest->szInsurePass[CountArray(pTakeScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTakeScoreRequest->lTakeScore>0L);
	if (pTakeScoreRequest->lTakeScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendTakeInRoom(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ����ȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendTakeInGame(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	DBR_GR_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

	//��������
	UserTakeScore.cbActivityGame=pTakeScoreRequest->cbActivityGame;
	UserTakeScore.dwUserID=pIServerUserItem->GetUserID();
	UserTakeScore.lTakeScore=pTakeScoreRequest->lTakeScore;
	UserTakeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserTakeScore.szPassword,pTakeScoreRequest->szInsurePass,CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTakeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_C_TransferScoreRequest));
	if (wDataSize!=sizeof(CMD_GP_C_TransferScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GP_C_TransferScoreRequest * pTransferScoreRequest=(CMD_GP_C_TransferScoreRequest *)pData;
	pTransferScoreRequest->szAccounts[CountArray(pTransferScoreRequest->szAccounts)-1]=0;
	pTransferScoreRequest->szInsurePass[CountArray(pTransferScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTransferScoreRequest->lTransferScore>0L);
	if (pTransferScoreRequest->lTransferScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBR_GR_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

	//��������
	UserTransferScore.cbActivityGame=pTransferScoreRequest->cbActivityGame;
	UserTransferScore.dwUserID=pIServerUserItem->GetUserID();
	UserTransferScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserTransferScore.lTransferScore=pTransferScoreRequest->lTransferScore;
	lstrcpyn(UserTransferScore.szAccounts,pTransferScoreRequest->szAccounts,CountArray(UserTransferScore.szAccounts));
	lstrcpyn(UserTransferScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTransferScore.szMachineID));
	lstrcpyn(UserTransferScore.szPassword,pTransferScoreRequest->szInsurePass,CountArray(UserTransferScore.szPassword));
	lstrcpyn(UserTransferScore.szTransRemark,pTransferScoreRequest->szTransRemark,CountArray(UserTransferScore.szTransRemark));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

	return true;
}

//��ѯ�û�����
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryUserInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryUserInfoRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GR_C_QueryUserInfoRequest *)pData;
	pQueryUserInfoRequest->szAccounts[CountArray(pQueryUserInfoRequest->szAccounts)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//ID�ж�
	if(pQueryUserInfoRequest->cbByNickName==FALSE)
	{
		//�����ж�
		int nLen=lstrlen(pQueryUserInfoRequest->szAccounts);
		if(nLen>=8)
		{
			SendInsureFailure(pIServerUserItem,TEXT("������Ϸ������ID��"), 0, pQueryUserInfoRequest->cbActivityGame);
			return true;
		}

		//�Ϸ��ж�
		for(int i=0; i<nLen; i++)
		{
			if(pQueryUserInfoRequest->szAccounts[i] < TEXT('0') || pQueryUserInfoRequest->szAccounts[i] > TEXT('9'))
			{
				SendInsureFailure(pIServerUserItem,TEXT("������Ϸ������ID��"), 0, pQueryUserInfoRequest->cbActivityGame);
				return true;
			}
		}
	}

	//��������
	DBR_GR_QueryTransferUserInfo QueryTransferUserInfo;
	ZeroMemory(&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	//��������	
	QueryTransferUserInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryTransferUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
	QueryTransferUserInfo.cbActivityGame=pQueryUserInfoRequest->cbActivityGame;	
	lstrcpyn(QueryTransferUserInfo.szAccounts,pQueryUserInfoRequest->szAccounts,CountArray(QueryTransferUserInfo.szAccounts));	

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_TRANSFER_USER_INFO,dwSocketID,&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubCheckInQueryRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_CheckInQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_CheckInQueryInfo)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//��ȡ����
	CMD_GR_CheckInQueryInfo *pCheckInQueryInfo = (CMD_GR_CheckInQueryInfo *)pData;
	pCheckInQueryInfo->szPassword[CountArray(pCheckInQueryInfo->szPassword)-1]=0;


	//����ṹ
	DBR_GR_CheckInQueryInfo CheckInQueryInfo;
	CheckInQueryInfo.dwUserID = pCheckInQueryInfo->dwUserID;
	lstrcpyn(CheckInQueryInfo.szPassword,pCheckInQueryInfo->szPassword,CountArray(CheckInQueryInfo.szPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_CHECKIN_QUERY_INFO,dwSocketID,&CheckInQueryInfo,sizeof(CheckInQueryInfo));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubCheckInDoneRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_CheckInDone));
	if(wDataSize!=sizeof(CMD_GR_CheckInDone)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//��ȡ����
	CMD_GR_CheckInDone *pCheckInDone = (CMD_GR_CheckInDone *)pData;
	pCheckInDone->szPassword[CountArray(pCheckInDone->szPassword)-1]=0;
	pCheckInDone->szMachineID[CountArray(pCheckInDone->szMachineID)-1]=0;

	//����ṹ
	DBR_GR_CheckInDone CheckInDone;
	CheckInDone.dwUserID = pCheckInDone->dwUserID;
	CheckInDone.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(CheckInDone.szPassword,pCheckInDone->szPassword,CountArray(CheckInDone.szPassword));
	lstrcpyn(CheckInDone.szMachineID,pCheckInDone->szMachineID,CountArray(CheckInDone.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_CHECKIN_DONE,dwSocketID,&CheckInDone,sizeof(CheckInDone));

	return true;
}

//��ȡ����
bool CAttemperEngineSink::OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeTask));
	if(wDataSize!=sizeof(CMD_GR_C_TakeTask)) return false;

	//��ȡ����
	CMD_GR_C_TakeTask * pTaskTake = (CMD_GR_C_TakeTask *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskTake TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_TAKE,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//��ȡ����
bool CAttemperEngineSink::OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_TaskReward));
	if(wDataSize!=sizeof(CMD_GR_C_TaskReward)) return false;

	//��ȡ����
	CMD_GR_C_TaskReward * pTaskReward = (CMD_GR_C_TaskReward *)pData;
	pTaskReward->szPassword[CountArray(pTaskReward->szPassword)-1]=0;
	pTaskReward->szMachineID[CountArray(pTaskReward->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskReward->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskReward TaskReward;
	TaskReward.dwUserID = pTaskReward->dwUserID;
	TaskReward.wTaskID = pTaskReward->wTaskID;
	TaskReward.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskReward.szPassword,pTaskReward->szPassword,CountArray(TaskReward.szPassword));
	lstrcpyn(TaskReward.szMachineID,pTaskReward->szMachineID,CountArray(TaskReward.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_REWARD,dwSocketID,&TaskReward,sizeof(TaskReward));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_LoadTaskInfo));
	if(wDataSize!=sizeof(CMD_GR_C_LoadTaskInfo)) return false;

	//��ȡ����
	CMD_GR_C_LoadTaskInfo * pLoadTaskInfo = (CMD_GR_C_LoadTaskInfo *)pData;
	pLoadTaskInfo->szPassword[CountArray(pLoadTaskInfo->szPassword)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLoadTaskInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskQueryInfo TaskQueryInfo;
	TaskQueryInfo.dwUserID = pLoadTaskInfo->dwUserID;
	lstrcpyn(TaskQueryInfo.szPassword,pLoadTaskInfo->szPassword,CountArray(TaskQueryInfo.szPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_QUERY_INFO,dwSocketID,&TaskQueryInfo,sizeof(TaskQueryInfo));

	return true;
}
//��������
bool CAttemperEngineSink::OnTCPNetworkSubGiveUpTaskRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeGiveUp));
	if(wDataSize!=sizeof(CMD_GR_C_TakeGiveUp)) return false;

	//��ȡ����
	CMD_GR_C_TakeGiveUp * pTaskTake = (CMD_GR_C_TakeGiveUp *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskGiveUp TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_GIVEUP,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//�����ѯ
bool CAttemperEngineSink::OnTCPNetworkSubMemberQueryInfo( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_MemberQueryInfo));
	if(wDataSize!=sizeof(CMD_GR_MemberQueryInfo)) return false;

	//��ȡ����
	CMD_GR_MemberQueryInfo * pMemberInfo= (CMD_GR_MemberQueryInfo *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_MemberQueryInfo MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_QUERY_INFO,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//�����ͽ�
bool CAttemperEngineSink::OnTCPNetworkSubMemberDayPresent( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_MemberDayPresent));
	if(wDataSize!=sizeof(CMD_GR_MemberDayPresent)) return false;

	//��ȡ����
	CMD_GR_MemberDayPresent * pMemberInfo= (CMD_GR_MemberDayPresent *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_MemberDayPresent MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_DAY_PRESENT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkSubMemberDayGift( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_MemberDayGift));
	if(wDataSize!=sizeof(CMD_GR_MemberDayGift)) return false;

	//��ȡ����
	CMD_GR_MemberDayGift * pMemberInfo= (CMD_GR_MemberDayGift *)pData;
	pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
	pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pMemberInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_MemberDayGift MemberInfo;
	MemberInfo.dwUserID = pMemberInfo->dwUserID;
	MemberInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
	lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_MEMBER_DAY_GIFT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

	return true;
}

//��ѯ�ͱ�
bool CAttemperEngineSink::OnTCPNetworkSubBaseEnsureQueryRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����ṹ
	CMD_GR_BaseEnsureParamter BaseEnsureParameter;
	BaseEnsureParameter.cbTakeTimes=m_BaseEnsureParameter.cbTakeTimes;
	BaseEnsureParameter.lScoreAmount=m_BaseEnsureParameter.lScoreAmount;
	BaseEnsureParameter.lScoreCondition=m_BaseEnsureParameter.lScoreCondition;

	//Ͷ������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_BASEENSURE,SUB_GR_BASEENSURE_PARAMETER,&BaseEnsureParameter,sizeof(BaseEnsureParameter));

	return true;
}

//��ȡ�ͱ�
bool CAttemperEngineSink::OnTCPNetworkSubBaseEnsureTakeRequest( VOID * pData, WORD wDataSize, DWORD dwSocketID )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_BaseEnsureTake));
	if(wDataSize!=sizeof(CMD_GR_BaseEnsureTake)) return false;

	//��ȡ����
	CMD_GR_BaseEnsureTake * pBaseEnsureTake = (CMD_GR_BaseEnsureTake *)pData;
	pBaseEnsureTake->szPassword[CountArray(pBaseEnsureTake->szPassword)-1]=0;
	pBaseEnsureTake->szMachineID[CountArray(pBaseEnsureTake->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pBaseEnsureTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TakeBaseEnsure TakeBaseEnsure;
	TakeBaseEnsure.dwUserID = pBaseEnsureTake->dwUserID;
	TakeBaseEnsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TakeBaseEnsure.szPassword,pBaseEnsureTake->szPassword,CountArray(TakeBaseEnsure.szPassword));
	lstrcpyn(TakeBaseEnsure.szMachineID,pBaseEnsureTake->szMachineID,CountArray(TakeBaseEnsure.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_BASEENSURE_TAKE,dwSocketID,&TakeBaseEnsure,sizeof(TakeBaseEnsure));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	CMD_GR_OptionCurrent OptionCurrent;
	ZeroMemory(&OptionCurrent,sizeof(OptionCurrent));

	//�ҽ�����
	OptionCurrent.ServerOptionInfo.wKindID=m_pGameServiceOption->wKindID;
	OptionCurrent.ServerOptionInfo.wNodeID=m_pGameServiceOption->wNodeID;
	OptionCurrent.ServerOptionInfo.wSortID=m_pGameServiceOption->wSortID;

	//��������
	OptionCurrent.ServerOptionInfo.wRevenueRatio=m_pGameServiceOption->wRevenueRatio;
	OptionCurrent.ServerOptionInfo.lServiceScore=m_pGameServiceOption->lServiceScore;
	OptionCurrent.ServerOptionInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	OptionCurrent.ServerOptionInfo.lMinTableScore=m_pGameServiceOption->lMinTableScore;
	OptionCurrent.ServerOptionInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	OptionCurrent.ServerOptionInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

	//��Ա����
	OptionCurrent.ServerOptionInfo.cbMinEnterMember=m_pGameServiceOption->cbMinEnterMember;
	OptionCurrent.ServerOptionInfo.cbMaxEnterMember=m_pGameServiceOption->cbMaxEnterMember;

	//��������
	OptionCurrent.ServerOptionInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	lstrcpyn(OptionCurrent.ServerOptionInfo.szServerName,m_pGameServiceOption->szServerName,CountArray(OptionCurrent.ServerOptionInfo.szServerName));

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_ON_GAME;

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_LOOKON;

	//���й���
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_GAME;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_GAME;

	//��������
	//OptionCurrent.dwRuleMask|=SR_RECORD_GAME_TRACK;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_RULE;
	OptionCurrent.dwRuleMask|=SR_FORFEND_LOCK_TABLE;
	OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_SIMULATE;

	//�������
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_DYNAMIC_JOIN;
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_ATTEND;
	//if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_OFFLINE_TRUSTEE;

	//ģʽ����
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_RECORD_GAME_SCORE;
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_IMMEDIATE_WRITE_SCORE;

	//��������
	SendData(pIServerUserItem,MDM_GR_MANAGE,SUB_GR_OPTION_CURRENT,&OptionCurrent,sizeof(OptionCurrent));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_ServerOption));
	if (wDataSize!=sizeof(CMD_GR_ServerOption)) return false;

	//��������
	CMD_GR_ServerOption * pServerOption=(CMD_GR_ServerOption *)pData;
	tagServerOptionInfo * pServerOptionInfo=&pServerOption->ServerOptionInfo;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	bool bModifyServer=false;

	//�ҽӽڵ�
	if (m_pGameServiceOption->wNodeID!=pServerOptionInfo->wNodeID)
	{
		bModifyServer=true;
		m_pGameServiceOption->wNodeID=pServerOptionInfo->wNodeID;
	}

	//�ҽ�����
	if ((pServerOptionInfo->wKindID!=0)&&(m_pGameServiceOption->wKindID!=pServerOptionInfo->wKindID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wKindID=pServerOptionInfo->wKindID;
	}
	
	//�ҽ�����
	if ((pServerOptionInfo->wSortID!=0)&&(m_pGameServiceOption->wSortID!=pServerOptionInfo->wSortID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wSortID=pServerOptionInfo->wSortID;
	}

	//��������
	if ((pServerOptionInfo->szServerName[0]!=0)&&(lstrcmp(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName)!=0))
	{
		bModifyServer=true;
		lstrcpyn(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName,CountArray(m_pGameServiceOption->szServerName));
	}

	//˰������
	m_pGameServiceOption->wRevenueRatio=pServerOptionInfo->wRevenueRatio;
	m_pGameServiceOption->lServiceScore=pServerOptionInfo->lServiceScore;

	//��������
	m_pGameServiceOption->lRestrictScore=pServerOptionInfo->lRestrictScore;
	m_pGameServiceOption->lMinTableScore=pServerOptionInfo->lMinTableScore;
	m_pGameServiceOption->lMinEnterScore=pServerOptionInfo->lMinEnterScore;
	m_pGameServiceOption->lMaxEnterScore=pServerOptionInfo->lMaxEnterScore;

	//��Ա����
	m_pGameServiceOption->cbMinEnterMember=pServerOptionInfo->cbMinEnterMember;
	m_pGameServiceOption->cbMaxEnterMember=pServerOptionInfo->cbMaxEnterMember;

	//�������
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendRoomChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperOnGame(pServerOptionInfo->dwServerRule));

	//�������
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameLookon(pServerOptionInfo->dwServerRule));

	//���й���
	CServerRule::SetForfendTakeInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendTakeInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInGame(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInGame(pServerOptionInfo->dwServerRule));

	//��������
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameTrack(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameRule(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendLockTable(pServerOptionInfo->dwServerRule));

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowDynamicJoin(pServerOptionInfo->dwServerRule));
	}

	//��������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowAndroidAttend(pServerOptionInfo->dwServerRule));
	}

	//�����й�
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowOffLineTrustee(pServerOptionInfo->dwServerRule));
	}

	//��¼�ɼ�
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameScore(pServerOptionInfo->dwServerRule));
	}

	//����д��
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,CServerRule::IsImmediateWriteScore(pServerOptionInfo->dwServerRule));
	}

	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->RectifyServiceParameter();

	//�����޸�
	if (bModifyServer==true)
	{
		//��������
		CMD_CS_C_ServerModify ServerModify;
		ZeroMemory(&ServerModify,sizeof(ServerModify));

		//����˿�
		ServerModify.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//������Ϣ
		ServerModify.wKindID=m_pGameServiceOption->wKindID;
		ServerModify.wNodeID=m_pGameServiceOption->wNodeID;
		ServerModify.wSortID=m_pGameServiceOption->wSortID;
		ServerModify.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
		ServerModify.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerModify.dwAndroidCount;
		ServerModify.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		lstrcpyn(ServerModify.szServerName,m_pGameServiceOption->szServerName,CountArray(ServerModify.szServerName));
		lstrcpyn(ServerModify.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(ServerModify.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_MODIFY,&ServerModify,sizeof(ServerModify));
	}

	//������Ϣ
	SendRoomMessage(pIServerUserItem,TEXT("��ǰ��Ϸ����������ġ�����ֵ��״̬���������޸ĳɹ�"),SMT_CHAT|SMT_EJECT);

	//�����Ϣ
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("Զ���޸ķ�������֪ͨ ����Ա %s [ %ld ]"),pIServerUserItem->GetNickName(),pIServerUserItem->GetUserID());

	//�����Ϣ
	CTraceService::TraceString(szBuffer,TraceLevel_Info);

	return true;
}

//�߳��û�
bool CAttemperEngineSink::OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//�߳���¼
	m_KickUserItemMap[pITargetUserItem->GetUserID()]=(DWORD)time(NULL);

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//������Ϣ
		SendGameMessage(pITargetUserItem,TEXT("���ѱ�����Ա�������ӣ�"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	//����֪ͨ
	LPCTSTR pszMessage=TEXT("���ѱ�����Ա�������Ϸ���䣡");
	SendRoomMessage(pITargetUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

	pITargetUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
	if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

	//��Ϣ����
	CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ŀ���û�
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
	if (pITargerUserItem==NULL) return true;

	//��������
	DWORD dwAddRight = 0, dwRemoveRight = 0;

	//��������
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
	{
		if (CMasterRight::CanLimitRoomChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_ROOM_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
	}

	//��Ϸ����
	if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
	{
		if (CMasterRight::CanLimitGameChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_GAME_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_GAME_CHAT;
	}

	//����˽��
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
	{
		if (CMasterRight::CanLimitWisper(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_WISPER;
		else
			dwRemoveRight |= UR_CANNOT_WISPER;
	}

	//��������
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

		//����֪ͨ
		CMD_GR_ConfigUserRight cur = {0};
		cur.dwUserRight = pITargerUserItem->GetUserRight();

		SendData( pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�û�����Ȩ�����óɹ���"),SMT_CHAT);
	}
	else return false;

	return true;
}

//�߳������û�
bool CAttemperEngineSink::OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��Ϣ����
	CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;

	//Ч������
	ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
	if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
	ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
	if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//��ɢ������Ϸ
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//tagBindParameter *pBindParameter = m_pNormalParameter;
	//for( INT i = 0; i < m_pGameServiceOption->wMaxPlayer; i++ )
	//{
	//	//Ŀ¼�û�
	//	IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
	//	if (pITargerUserItem==NULL || pITargerUserItem==pIServerUserItem ) 
	//	{
	//		pBindParameter++;
	//		continue;
	//	}

	//	//������Ϣ
	//	SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

	//	pBindParameter++;
	//} 

	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	while(pITargerUserItem!=NULL)
	{
		if(pITargerUserItem!=pIServerUserItem)
		{
			//������Ϣ
			SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);
		}

		//ö���û�
		pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

	//��������
	CMD_GR_SendMessage *pSysMessage = (CMD_GR_SendMessage *)pData;

	if(pSysMessage->cbLoop==TRUE)
	{
		//���ڱ���
		tagSystemMessage  *pSendMessage=new tagSystemMessage;
		ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

		//������Ϣ
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

		//��¼��Ϣ
		m_SystemMessageActive.Add(pSendMessage);
	}


	//��Ϣ����
	return SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);
}

//��ɢ��Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
	if (wDataSize!=sizeof(CMD_GR_DismissGame)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

	//��Ϣ����
	CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;

	//Ч������
	if(pDismissGame->wDismissTableNum >= m_TableFrameArray.GetCount()) return true;

	//��ɢ��Ϸ
	CTableFrame *pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
	if(pTableFrame)
	{
		if(pTableFrame->IsGameStarted()) pTableFrame->DismissGame();
		pTableFrame->SendGameMessage(TEXT("��Ǹ�������ѱ�����Ա��ɢ��Ϸ"),SMT_EJECT);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_SendWarning));
	if (wDataSize!=sizeof(CMD_GR_SendWarning)) return false;

	//��ȡ����
	CMD_GR_SendWarning * pSendWarning = (CMD_GR_SendWarning*)pData;
	ASSERT(pSendWarning!=NULL);

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pSendWarning->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==false) return false;

	//Ȩ���ж�
	ASSERT(pIServerUserItem->GetMasterOrder()>pITargetUserItem->GetMasterOrder());
	if(pIServerUserItem->GetMasterOrder()<pITargetUserItem->GetMasterOrder()) return false;

	//���;���
	TCHAR szMessage[128]=TEXT("");
	_sntprintf(szMessage,CountArray(szMessage),TEXT("���棺%s"),pSendWarning->szWarningMessage);
	SendRoomMessage(pITargetUserItem,szMessage,SMT_EJECT);

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_CreateTable));
	if (wDataSize!=sizeof(CMD_GR_CreateTable)) return false;

	//��ȡ����
	CMD_GR_CreateTable * pCreateTable = (CMD_GR_CreateTable*)pData;
	ASSERT(pCreateTable!=NULL);

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	tagBindParameter* pBindParameter = GetBindParameter(wBindIndex);


	//Ѱ�ҿ�������
	INT_PTR nSize = m_TableFrameArray.GetCount();

	for(INT_PTR i = 0; i < nSize; ++i)
	{
		CTableFrame* pTableFrame = m_TableFrameArray.GetAt(i);
		if(pTableFrame->GetNullChairCount() == pTableFrame->GetChairCount() && pTableFrame->IsPersonalTableLocked() == false)
		{
			//��������
			pTableFrame->SetPersonalTableLlocked(true);

			//���Ӻ�
			DWORD dwTableID = pTableFrame->GetTableID();

			//��������
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

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(CreateTable.dwUserID, DBR_GR_CREATE_TABLE, dwSocketID, &CreateTable, sizeof(DBR_GR_CreateTable));

			return true;
		}
	}

	//��������
	CMD_GR_CreateFailure CreateFailure;
	ZeroMemory(&CreateFailure, sizeof(CMD_GR_CreateFailure));

	CreateFailure.lErrorCode = 1;
	lstrcpyn(CreateFailure.szDescribeString, TEXT("Ŀǰ����ϷԼս�������������Ժ����ԣ�"), CountArray(CreateFailure.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_PERSONAL_TABLE, SUB_GR_CREATE_FAILURE, &CreateFailure, sizeof(CMD_GR_CreateFailure));

	return true;
}
//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_CancelRequest));
	if(wDataSize != sizeof(CMD_GR_CancelRequest)) return false;

	//��ȡ����
	CMD_GR_CancelRequest * pCancelRequest = (CMD_GR_CancelRequest*)pData;
	ASSERT(pCancelRequest!=NULL);
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelRequest->dwTableID];
	ASSERT(pTableFrame != NULL);
	//if(pCancelRequest->dwUserID != pTableFrame->GetTableOwner()->GetUserID()) return false;

	if(pTableFrame->CancelTableRequest(pCancelRequest->dwUserID, pCancelRequest->dwChairID) == false) return false;

	//ת������
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//�����û�

		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_REQUEST, pData, wDataSize);
	}

	return true;
}
//ȡ����
bool CAttemperEngineSink::OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_RequestReply));
	if(wDataSize != sizeof(CMD_GR_RequestReply)) return false;

	//��ȡ����
	CMD_GR_RequestReply * pRequestReply = (CMD_GR_RequestReply*)pData;
	ASSERT(pRequestReply!=NULL);

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = m_TableFrameArray[pRequestReply->dwTableID];
	ASSERT(pTableFrame != NULL);
	//if(pRequestReply->dwUserID == pTableFrame->GetTableOwner()->GetUserID()) return false;

	//ת������
	WORD wChairCount = pTableFrame->GetChairCount();
	for(int i = 0; i < wChairCount; ++i)
	{
		//�����û�
		IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(i);
		if(pUserItem == NULL || pUserItem == pIServerUserItem) continue;

		SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_REPLY, pData, wDataSize);
	}

	if(pTableFrame->CancelTableRequestReply(pRequestReply->dwUserID, pRequestReply->cbAgree) == false) return false;

	return true;
}

//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GR_HostDissumeGame));
	if(wDataSize != sizeof(CMD_GR_HostDissumeGame)) return false;

	//��ȡ����
	CMD_GR_HostDissumeGame * pCancelRequest = (CMD_GR_HostDissumeGame*)pData;
	ASSERT(pCancelRequest!=NULL);
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame* pTableFrame = m_TableFrameArray[pCancelRequest->dwTableID];
	ASSERT(pTableFrame != NULL);

	//���ǿ�ƽ�ɢ����Ҳ��Ƿ������������ɢ
	if (pTableFrame->GetTableOwner() != pCancelRequest->dwUserID)
	{
		return true;
	}

	//Ͷ������
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


	//��������
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));
	DismissTable.dwSocketID = dwSocketID;
	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID = pTableFrame->GetTableID();

	//������Ϣ
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));


	return true;
}

//������󷿼�ɼ�
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	////OutputDebugString(TEXT("ptdt *** OnTCPNetworkSubHostDissumeTable"));
	////У������
	//ASSERT(wDataSize == sizeof(CMD_GR_QUERY_USER_ROOM_SCORE));
	//if(wDataSize != sizeof(CMD_GR_QUERY_USER_ROOM_SCORE)) return false;

	////��ȡ����
	//CMD_GR_QUERY_USER_ROOM_SCORE * pQueryUserRoomScore = (CMD_GR_QUERY_USER_ROOM_SCORE*)pData;
	//ASSERT(pQueryUserRoomScore!=NULL);
	////��ȡ�û�
	//WORD wBindIndex=LOWORD(dwSocketID);
	//IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	//if (pIServerUserItem==NULL) return false;

	////��������
	//DBR_GR_QUERY_USER_ROOM_INFO QUERY_USER_ROOM_INFO;
	//ZeroMemory(&QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));
	//QUERY_USER_ROOM_INFO.dwUserID = pQueryUserRoomScore->dwUserID;//���󷿼�ɼ������

	////Ͷ������
	//m_pIDBCorrespondManager->PostDataBaseRequest(QUERY_USER_ROOM_INFO.dwUserID, DBR_GR_QUERY_USER_ROOM_SCORE, dwSocketID, &QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));

	return true;
}


//�û���¼
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//��ȡ����
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bAndroidUser=pIServerUserItem->IsAndroidUser();
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��¼����
	if (pIServerUserItem->IsMobileUser()==false)
	{
		//��������
		CMD_GR_LogonSuccess LogonSuccess;
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));

		//��¼�ɹ�
		LogonSuccess.dwUserRight=pIServerUserItem->GetUserRight();
		LogonSuccess.dwMasterRight=pIServerUserItem->GetMasterRight();
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

		//��¼֪ͨ
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

		//��������
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

		//�б�����
		WORD wConfigColumnHead=sizeof(m_DataConfigColumn)-sizeof(m_DataConfigColumn.ColumnItem);
		WORD wConfigColumnInfo=m_DataConfigColumn.cbColumnCount*sizeof(m_DataConfigColumn.ColumnItem[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_COLUMN,&m_DataConfigColumn,wConfigColumnHead+wConfigColumnInfo);


		//�������
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//�����Լ���Ϣ���Լ�
		SendUserInfoPacket(pIServerUserItem,pBindParameter->dwSocketID);

		//��������		
		WORD wUserIndex=0;
		bool bSendAndroidFlag=false;
		IServerUserItem * pIServerUserItemSend=NULL;

		//Ȩ���ж�		
		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
		   CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
		{
			bSendAndroidFlag=true;
		}

		//���������������û����Լ������Լ����⣩		
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend==pIServerUserItem) continue;
			SendUserInfoPacket(pIServerUserItemSend,pBindParameter->dwSocketID,bSendAndroidFlag);
		}

		//����״̬
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

		//����״̬
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//�����Լ�����Ϣ�����������û�
		if (bAlreadyOnLine==false)
		{
			//�Լ��ǻ�����
			if (pIServerUserItem->IsAndroidUser()==true)
			{
				//���ñ���
				wUserIndex=0;
				while (true)
				{
					pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
					if (pIServerUserItemSend==NULL) break;
					if (pIServerUserItemSend==pIServerUserItem) continue;

					//����Ȩ��
					bSendAndroidFlag=false;
					if(CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight())==true ||
					   CMasterRight::CanManagerAndroid(pIServerUserItemSend->GetMasterRight())==true )
					{
						bSendAndroidFlag=true;
					}

					//��ȡ����
					WORD wBindIndexSend=pIServerUserItemSend->GetBindIndex();
					tagBindParameter * pBindParameterSend=GetBindParameter(wBindIndexSend);
					if (pBindParameterSend!=NULL)
					{
						//������Ϣ
						SendUserInfoPacket(pIServerUserItem,pBindParameterSend->dwSocketID,bSendAndroidFlag);
					}
				}
			}
			else
			{
				SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);
			}
		}

		//����ṹ
		CMD_GR_LogonFinish LogonFinish;
		bool bHasTaskParameter=m_UserTaskManager.GetTaskParameterCount()>0;
		bool bHasUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID())!=NULL;
		LogonFinish.bGuideTask = bHasTaskParameter&&!bHasUserTaskEntry;

		//��¼���
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish));

		//��¼���
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogonFinish(pIServerUserItem);

		//��ӭ��Ϣ
		if (bAndroidUser==false)
		{
			//������ʾ
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s����Ϸ���䣬ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName);

			//������Ϣ
			SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT);
		}
	}
	else
	{
		//��¼֪ͨ
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

		//��������
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));		

		//��������
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));		

		//�������
		SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//�����Լ���Ϣ���Լ�
		SendUserInfoPacket(pIServerUserItem,pBindParameter->dwSocketID);

		//��������		
		WORD wUserIndex=0;
		bool bSendAndroidFlag=false;
		IServerUserItem * pIServerUserItemSend=NULL;

		//Ȩ���ж�		
		if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==true ||
			CMasterRight::CanManagerAndroid(pIServerUserItem->GetMasterRight())==true)
		{
			bSendAndroidFlag=true;
		}

		//���������������û����Լ������Լ����⣩	
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend==pIServerUserItem) continue;
			SendUserInfoPacket(pIServerUserItemSend,pBindParameter->dwSocketID,bSendAndroidFlag);
		}

		//�����Լ�����Ϣ�����������û�
		SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);

		//��¼���
		SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);

		//�Թ��û�
		if (pIServerUserItem->GetUserStatus()==US_LOOKON)
		{
			m_TableFrameArray[pIServerUserItem->GetTableID()]->PerformStandUpAction(pIServerUserItem);
		}

		//����״̬
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

		//����״̬
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//������¼
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

		//��¼���
		if (m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogonFinish(pIServerUserItem);
	}

	//��������
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

	//��¼֪ͨ
	if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);

	return;
}

//�û��뿪
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//��ȡ�ɼ�
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

    //�û���Ϣ
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//��������
	if(pIServerUserItem->IsAndroidUser()==true)
	{
		LeaveGameServer.VariationInfo.lScore=0;
		LeaveGameServer.VariationInfo.lGrade=0;
		LeaveGameServer.VariationInfo.lInsure=0;
		LeaveGameServer.VariationInfo.lRevenue=0;
	}

	//������Ϣ
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//�Ƿ�ΪԼս����
	if(m_bHasPersonalRoomService)
	{
		LeaveGameServer.cbIsPersonalRoom = 1;
	}
	else
	{
		LeaveGameServer.cbIsPersonalRoom = 0;
	}

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//���Ҷ���
	tagUserTaskEntry * pUserTaskEntry=m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(),TASK_STATUS_UNFINISH|TASK_STATUS_FAILED); 

	//�ƽ�����
	if(pUserTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem,pUserTaskEntry,LeaveGameServer.VariationInfo.dwWinCount,LeaveGameServer.VariationInfo.dwLostCount,LeaveGameServer.VariationInfo.dwDrawCount);
	}
	
	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//���ñ���
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//������Ϣ
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//�������
	if (m_pIMatchServiceManager!=NULL) 
	{
		m_pIMatchServiceManager->OnEventUserQuitMatch(pIServerUserItem, 0);
	}

	//ɾ������
	DeleteDistribute(pIServerUserItem);

	//�Ƴ�����
	m_UserTaskManager.RemoveUserTask(pIServerUserItem->GetUserID());

	//ɾ���û�
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	//�ǳ�֪ͨ
	if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventUserLogout(pIServerUserItem);

	return;
}

//������Ϸ��
bool CAttemperEngineSink::PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//���ñ���
	LeaveGameServer.dwUserID=dwUserID;
	LeaveGameServer.dwInoutIndex=dwInoutIndex;
	LeaveGameServer.dwLeaveReason=dwLeaveReason;

	//�Ƿ�ΪԼս����
	if(m_bHasPersonalRoomService)
	{
		LeaveGameServer.cbIsPersonalRoom = 1;
	}
	else
	{
		LeaveGameServer.cbIsPersonalRoom = 0;
	}
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return true;
}

//�ƽ�����
bool CAttemperEngineSink::PerformTaskProgress(IServerUserItem * pIServerUserItem,tagUserTaskEntry * pUserTaskEntry,DWORD dwWinCount,DWORD dwLostCount,DWORD dwDrawCount)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL && pUserTaskEntry!=NULL);
	if(pIServerUserItem==NULL || pUserTaskEntry==NULL) return false;

	//״̬У��
	if(pUserTaskEntry->pTaskParameter==NULL) return true;
	if(pUserTaskEntry->cbTaskStatus!=TASK_STATUS_UNFINISH) return true;
	if(pUserTaskEntry->pTaskParameter->wKindID!=m_pGameServiceOption->wKindID) return true;

	//�������
	tagTaskParameter * pTaskParameter = pUserTaskEntry->pTaskParameter;

	//��ȡʱ��
	DWORD dwSpanSecond=(DWORD)time(NULL)-pUserTaskEntry->dwLastUpdateTime;
	dwSpanSecond=__min(dwSpanSecond,pUserTaskEntry->dwResidueTime);

	//����ʱ��	
	pUserTaskEntry->dwResidueTime-=dwSpanSecond;
	pUserTaskEntry->dwLastUpdateTime=(DWORD)time(NULL);

	//����ʱ
	if(pUserTaskEntry->dwResidueTime == 0)
	{		
		pUserTaskEntry->wTaskProgress=0;
		pUserTaskEntry->pTaskParameter=NULL;		
		pUserTaskEntry->cbTaskStatus=TASK_STATUS_FAILED;		
	}

	//����δ���
	if(pUserTaskEntry->cbTaskStatus==TASK_STATUS_UNFINISH)
	{
		//��������
		switch(pTaskParameter->wTaskType)
		{
		case TASK_TYPE_WIN_INNINGS:			//Ӯ������
			{
				//����״̬
				pUserTaskEntry->wTaskProgress += (WORD)dwWinCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}

				break;
			};
		case TASK_TYPE_SUM_INNINGS:			//�ܾ�����
			{
				//����״̬
				pUserTaskEntry->wTaskProgress += (WORD)dwWinCount+(WORD)dwLostCount+(WORD)dwDrawCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}
				break;
			};
		case TASK_TYPE_FIRST_WIN:			//��ʤ����
			{
				//����״̬
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
		case TASK_TYPE_KEEP_WIN:			//��Ӯ����
			{

				if (dwLostCount>0 || dwDrawCount>0)
				{
					pUserTaskEntry->wTaskProgress=0;
				}
				else
				{
					//����״̬
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

	//�������
	if(pUserTaskEntry->cbTaskStatus==TASK_STATUS_SUCCESS)
	{
		//����ṹ
		CMD_GR_S_TaskFinish TaskFinish;
		TaskFinish.wFinishTaskID=pTaskParameter->wTaskID;
		lstrcpyn(TaskFinish.szTaskName,pTaskParameter->szTaskName,CountArray(TaskFinish.szTaskName));

		//������Ϣ
		WORD wHeadSize = sizeof(TaskFinish)-sizeof(TaskFinish.szTaskName);
		WORD wDataSize = wHeadSize + CountStringBuffer(TaskFinish.szTaskName);
		SendData(pIServerUserItem,MDM_GR_TASK,SUB_GR_TASK_FINISH,&TaskFinish,wDataSize);
	}

	//�ݹ����
	if(pUserTaskEntry->pNextTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem,pUserTaskEntry->pNextStatusEntry,dwWinCount,dwLostCount,dwDrawCount);
	}

	return true;
}

//�汾���
bool CAttemperEngineSink::PerformCheckVersion(DWORD dwPlazaVersion, DWORD dwFrameVersion, DWORD dwClientVersion, DWORD dwSocketID)
{
	//��������
	bool bMustUpdateClient=false;
	bool bAdviceUpdateClient=false;

	//��Ϸ�汾
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

	//����֪ͨ
	if ((bMustUpdateClient==true)||(bAdviceUpdateClient==true))
	{
		//��������
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdatePlaza=false;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;
		UpdateNotify.cbAdviceUpdateClient=bAdviceUpdateClient;

		//��ǰ�汾
		UpdateNotify.dwCurrentPlazaVersion=VERSION_PLAZA;
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

		//������Ϣ
		SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//��ֹ�ж�
		if (bMustUpdateClient==true)
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return false;
		}
	}

	return true;
}

//�л�����
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD)) return false;

	//�Ͽ��û�
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//����֪ͨ
		LPCTSTR pszMessage=TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�������뿪��");
		SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

		//�󶨲���
		WORD wSourceIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pSourceParameter=GetBindParameter(wSourceIndex);

		//�����
		ASSERT((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem));
		if ((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem)) pSourceParameter->pIServerUserItem=NULL;

		//�Ͽ��û�
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID,false);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//�����ж�
	LPCTSTR pszMachineID=pIServerUserItem->GetMachineID();
	bool bSameMachineID=(lstrcmp(pszMachineID,szMachineID)==0);

	//��������
	bool bAndroidUser=(wTargetIndex>=INDEX_ANDROID);
	tagBindParameter * pTargetParameter=GetBindParameter(wTargetIndex);

	//�����û�
	pTargetParameter->pIServerUserItem=pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr,wTargetIndex,szMachineID,bAndroidUser,false);

	//�ֻ���ʶ
	if(pTargetParameter->cbClientKind==CLIENT_KIND_MOBILE)
	{
		pIServerUserItem->SetMobileUser(true);
		SetMobileUserParameter(pIServerUserItem,cbDeviceType,wBehaviorFlags,wPageTableCount);
	}

	//״̬�л�
	bool bIsOffLine=false;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		//��������
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();

		//����״̬
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

	//ȡ���й�
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		if (pIServerUserItem->IsTrusteeUser()==true && pIServerUserItem->GetUserStatus()!=US_PLAYING)
		{
			pIServerUserItem->SetTrusteeUser(false);
		}
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,true);

	//��ȫ��ʾ
	if ((bAndroidUser==false)&&(bIsOffLine==false)&&(bSameMachineID==false))
	{
		SendRoomMessage(pIServerUserItem,TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�Է������뿪��"),SMT_EJECT|SMT_CHAT|SMT_GLOBAL);
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//��������
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//��������
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//��������
	SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszString, LONG lErrorCode,BYTE cbActivityGame)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_S_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	UserInsureFailure.cbActivityGame=cbActivityGame;
	UserInsureFailure.lErrorCode=lErrorCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszString,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//���ñ���
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	//WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	//WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	//SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,sizeof(RequestFailure));
	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea)
{
	//��������
	CMD_GR_PropertyFailure PropertyFailure;
	ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

	//���ñ���
	PropertyFailure.lErrorCode=lErrorCode;
	PropertyFailure.wRequestArea=wRequestArea;
	lstrcpyn(PropertyFailure.szDescribeString,pszDescribe,CountArray(PropertyFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
	WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_PROPERTY_FAILURE,&PropertyFailure,wHeadSize+wDataSize);

	return true;
}

//�����û�
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID,bool bSendAndroidFalg)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwGroupID=pUserInfo->dwGroupID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//�û�����	
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserInfo->cbMasterOrder;
	pUserInfoHead->bIsAndroid=pIServerUserItem->IsAndroidUser()&&bSendAndroidFalg;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//�û�����
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lLoveLiness=pUserInfo->lLoveLiness;
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

	//�û�����
	pUserInfoHead->lGrade=pUserInfo->lGrade;
	pUserInfoHead->lInsure=pUserInfo->lInsure;
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	//�û��ɼ�
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();

	//lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//������Ϣ
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	SendPacket.AddPacket(pUserInfo->szGroupName,DTP_GR_GROUP_NAME);
	SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);
	SendPacket.AddPacket(pUserInfo->szQQ,DTP_GR_QQ);
	SendPacket.AddPacket(pUserInfo->szMobilePhone,DTP_GR_MOBILE_PHONE);

	//��������
	if (dwSocketID==INVALID_DWORD)
	{
		WORD wHeadSize=sizeof(tagUserInfoHead);
		SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
		SendUserInfoPacketBatchToMobileUser(pIServerUserItem,INVALID_DWORD);
	}
	else
	{
		//��ȡ�û�
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


//�㲥����
bool CAttemperEngineSink::SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount)
{
	//����ṹ
	CMD_GR_S_PropertyMessage  PropertyMessage;
	PropertyMessage.wPropertyIndex=wPropertyIndex;
	PropertyMessage.dwSourceUserID=dwSourceID;
	PropertyMessage.dwTargerUserID=dwTargerID;
	PropertyMessage.wPropertyCount=wPropertyCount;

	//�����û�
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

//����ЧӦ
bool CAttemperEngineSink::SendPropertyEffect(IServerUserItem * pIServerUserItem, WORD wPropKind)
{
	//����У��
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	CMD_GR_S_PropertyEffect  PropertyEffect={0};
	PropertyEffect.wPropKind = (BYTE)wPropKind;
	PropertyEffect.wUserID = pIServerUserItem->GetUserID();
	PropertyEffect.cbMemberOrder=pIServerUserItem->GetMemberOrder();
	PropertyEffect.dwFleeCount = pIServerUserItem->GetUserInfo()->dwFleeCount;
	PropertyEffect.lScore = pIServerUserItem->GetUserInfo()->lScore;

	//�����û�
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

//�����¼�
bool CAttemperEngineSink::OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem)
{
	//Ŀ�����
	if ( pTargetUserItem == NULL )
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("����ʧ�ܣ���Ҫ���͵�����Ѿ��뿪��"), 0L,cbRequestArea);

		return false;
	}

	//�����ж�
	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH ) && 
		(wPropertyIndex== PROPERTY_ID_SCORE_CLEAR||wPropertyIndex==PROPERTY_ID_TWO_CARD||wPropertyIndex == PROPERTY_ID_FOUR_CARD||wPropertyIndex == PROPERTY_ID_POSSESS) )
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˷��䲻����ʹ�ô˵���,����ʧ��"), 0L,cbRequestArea);

		return false;
	}

	//���ҵ���
	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(wPropertyIndex);

	//��ЧЧ��
	if(pPropertyInfo==NULL)
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߻�δ����,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//�Լ�ʹ��
    if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_MESELF)==0 && pISourceUserItem==pTargetUserItem) 
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲����Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//���ʹ��
	if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_PLAYER)==0 && pISourceUserItem!=pTargetUserItem) 
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸����,ֻ���Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//�Թ۷�Χ
	if((pPropertyInfo->wServiceArea&PT_SERVICE_AREA_LOOKON)==0)  
	{
		//��������
		WORD wTableID = pTargetUserItem->GetTableID();
		if(wTableID!=INVALID_TABLE)
		{
			//��������
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//��ȡ����
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	
			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=pTableFrame->EnumLookonUserItem(wEnumIndex++);
				if( pIServerUserItem==NULL) break;
				if( pIServerUserItem==pTargetUserItem )
				{
					//������Ϣ
					SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸��Թ��û�,����ʧ�ܣ�"), 0L,cbRequestArea);

					return false;
				}
			} while (true);
		}
	}

	//�����ж�
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_SCORE_CLEAR :			//��������
		{
			//��������
			SCORE lCurrScore = pTargetUserItem->GetUserScore();
			if( lCurrScore >= 0)
			{
				//��������
				TCHAR szMessage[128]=TEXT("");
				if ( pISourceUserItem==pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�����ڵĻ����Ѿ��ǷǸ���������Ҫʹ�ø���������ߣ�"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]���ڵĻ����Ѿ��ǷǸ���������Ҫʹ�ø���������ߣ�"), pTargetUserItem->GetNickName());

				//������Ϣ
				SendPropertyFailure(pISourceUserItem,szMessage, 0L,cbRequestArea);

				return false;
			}
			break;
		}
	case PROPERTY_ID_ESCAPE_CLEAR :			 //��������
		{
			//��������
			DWORD dwCurrFleeCount = pTargetUserItem->GetUserInfo()->dwFleeCount;
			if ( dwCurrFleeCount==0 )
			{
				//��������
				TCHAR szMessage[128]=TEXT("");		
				if ( pISourceUserItem == pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�����ڵ��������Ѿ�Ϊ0������Ҫʹ������������ߣ�"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]���ڵ��������Ѿ�Ϊ0������Ҫʹ������������ߣ�"), pTargetUserItem->GetNickName());

				//������Ϣ
				SendPropertyFailure(pISourceUserItem,szMessage,0L,cbRequestArea);

				return false;
			}
			break;
		}
	}

	return true;
}

//���û�
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//��������
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//��������
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

//�����û�
bool CAttemperEngineSink::PerformDistribute()
{
	//����У��
	if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinDistributeUser) return false;

	//�����û�
	while(true)
	{
		CString strMessage;
		TCHAR szMessage[128]={0};		

		try
		{
			//����У��
			if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinPartakeGameUser) break;

			//��������
			CDistributeInfoArray DistributeInfoArray;

			strMessage.Append(TEXT("�����û�1\r\n"));

			//��ȡ�û�
			srand(GetTickCount());
			WORD wRandCount = __max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
			WORD wChairCount = m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
			WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
			if(wDistributeCount < wChairCount) break;

			strMessage.Append(TEXT("�����û�2\r\n"));

			//Ѱ��λ��
			CTableFrame * pCurrTableFrame=NULL;
			for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
			{
				//��ȡ����
				ASSERT(m_TableFrameArray[i]!=NULL);
				CTableFrame * pTableFrame=m_TableFrameArray[i];

				//״̬�ж�
				if (pTableFrame->GetSitUserCount()==0)
				{
					pCurrTableFrame=pTableFrame;
					break;
				}
			}

			strMessage.Append(TEXT("�����û�3\r\n"));

			//�����ж�
			if(pCurrTableFrame==NULL) break;

			//�������
			bool bSitSuccess=true;
			INT_PTR nSitFailedIndex=INVALID_CHAIR;
			for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
			{
				//��������
				WORD wChairID=pCurrTableFrame->GetNullChairID();

				//�����û�
				if (wChairID!=INVALID_CHAIR)
				{
					//��ȡ�û�
					IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

					_sntprintf(szMessage,CountArray(szMessage),TEXT("�����û�4-%s\r\n"),pIServerUserItem->GetNickName());
					strMessage.Append(szMessage);

					//�û�����
					if(pCurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
					{
						strMessage.Format(TEXT("����ʧ�ܣ�"));
						bSitSuccess=false;
						nSitFailedIndex=nIndex;
						break;
					}					
				}
			}

			//���½��
			if(bSitSuccess)
			{
				strMessage.Append(TEXT("�����û�5"));

				//�Ƴ����
				while(DistributeInfoArray.GetCount()>0)
				{
					m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[0].pPertainNode);
					DistributeInfoArray.RemoveAt(0);
				}

				strMessage.Append(TEXT("�����û�6"));
			}
			else
			{
				strMessage.Append(TEXT("�����û�7"));

				for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
				{
					if(nSitFailedIndex==nIndex)
					{
						m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
					}
					else
					{
						//�û�����
						if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
						{
							WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
							m_TableFrameArray[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
						}
					}
				}

				strMessage.Append(TEXT("�����û�8"));
			}	
		}catch(...)
		{
			CTraceService::TraceString(strMessage,TraceLevel_Exception);
		}
	}

	return true;
}

//���û���
bool CAttemperEngineSink::InitAndroidUser()
{
	//��������
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//���ò���
	AndroidUserParameter.bServiceContinue=true;
	AndroidUserParameter.pGameParameter=m_pGameParameter;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;
	AndroidUserParameter.pGameMatchOption=m_pGameMatchOption;

	//�������
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pIGameServiceSustomTime=m_pIGameServiceSustomTime;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//��������
	if(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR)
	{
		AndroidUserParameter.bServiceContinue=false;
		AndroidUserParameter.dwMinSitInterval=MIN_INTERVAL_TIME;
		AndroidUserParameter.dwMaxSitInterval=MAX_INTERVAL_TIME;	
	}

	//���ö���
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//��������
bool CAttemperEngineSink::InitTableFrameArray()
{
	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//�ں����
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//�������
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;

	//���ò���
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameMatchOption=m_pGameMatchOption;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	//��������
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(),m_pGameServiceOption->wTableCount*sizeof(CTableFrame *));

	//��������
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//��������
		m_TableFrameArray[i]=new CTableFrame;

		//��������
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		//������
		if(m_pIMatchServiceManager!=NULL)
		{
			m_pIMatchServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}
	}	

	return true;
}

//���ñ���
bool CAttemperEngineSink::InitMatchServiceManager()
{
	//����У��
	if(m_pIMatchServiceManager==NULL) return true;

	//���Ӳ���
	tagMatchManagerParameter MatchManagerParameter;
	ZeroMemory(&MatchManagerParameter,sizeof(MatchManagerParameter));

	//���ò���
	MatchManagerParameter.pGameMatchOption=m_pGameMatchOption;
	MatchManagerParameter.pGameServiceOption=m_pGameServiceOption;
	MatchManagerParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//�ں����
	MatchManagerParameter.pITimerEngine=m_pITimerEngine;
	MatchManagerParameter.pICorrespondManager=m_pIDBCorrespondManager;
	MatchManagerParameter.pTCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//�������
	MatchManagerParameter.pIMainServiceFrame=this;
	MatchManagerParameter.pIServerUserItemSink=this;
	MatchManagerParameter.pIAndroidUserManager=&m_AndroidUserManager;
	MatchManagerParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);		

	//��ʼ���ӿ�
	m_pIMatchServiceManager->InitMatchInterface(MatchManagerParameter);

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//�Ͽ�Э��
bool CAttemperEngineSink::DisconnectCorrespond()
{
	//���ñ���
	m_bNeekCorrespond=false;

	//�ر�����
	if (m_pITCPSocketService!=NULL) m_pITCPSocketService->CloseSocket();

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//��������
	if(m_pIMatchServiceManager!=NULL) 
	{
		//��ȡ����
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		return m_pIMatchServiceManager->OnEventUserJoinMatch(pIServerUserItem,0,pBindParameter->dwSocketID);
	}

	//�жϷ���
	if (m_pGameServiceOption->lMinTableScore>0 && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore)
	{
		return false;
	}

	//��������
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//���ñ���
	DistributeInfo.pIServerUserItem=pIServerUserItem;
	DistributeInfo.wLastTableID=pIServerUserItem->GetTableID();
	DistributeInfo.pPertainNode=NULL;

	//��������
	if(m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//�û�����
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
	}

	//����֪ͨ
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	return true;
}

//���дʹ���
void CAttemperEngineSink::SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	m_WordsFilter.Filtrate(pMsg,pszFiltered,nMaxLen);
}

//����������
VOID CAttemperEngineSink::UnLockAndroidUser(WORD wServerID, WORD wBatchID)
{
	//��������
	DBR_GR_UnlockAndroidUser UnlockAndroidUser;
	UnlockAndroidUser.wServerID=wServerID;
	UnlockAndroidUser.wBatchID=wBatchID;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(0,DBR_GR_UNLOCK_ANDROID_USER,0,&UnlockAndroidUser,sizeof(UnlockAndroidUser));
}

//��ɢ˽������
VOID CAttemperEngineSink::DismissPersonalTable(WORD wServerID, WORD wTableID)
{
	//��ɢ����
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));

	DismissTable.dwServerID = wServerID;
	DismissTable.dwTableID = wTableID;

	//������Ϣ
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE, &DismissTable, sizeof(CMD_CS_C_DismissTable));

	CTableFrame* pTableFrame = m_TableFrameArray[wTableID];
	if(pTableFrame)
	{
		pTableFrame->SetPersonalTableLlocked(false);
	}

	return;
}

//ȡ������
VOID CAttemperEngineSink::CancelCreateTable(DWORD dwUserID, DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, DWORD dwReason, WORD wTableID, TCHAR * szRoomID)
{
	//�˻�����
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
		//�ж�����
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

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID, DBR_GR_CANCEL_CREATE_TABLE, dwSocketID, &CancelCreateTable, sizeof(DBR_GR_CancelCreateTable));
	}
	else
	{
		//Ͷ������
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

	//��ɢ����
	CMD_CS_C_DismissTable DismissTable;
	ZeroMemory(&DismissTable, sizeof(CMD_CS_C_DismissTable));

	DismissTable.dwServerID = m_pGameServiceOption->wServerID;
	DismissTable.dwTableID =wTableID;

	//������Ϣ
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

//ɾ���û�
bool CAttemperEngineSink::DeleteDistribute(IServerUserItem * pIServerUserItem)
{
	//�Ƴ��ڵ�
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}

//���ò���
void CAttemperEngineSink::SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if(wPageTableCount > m_pGameServiceOption->wTableCount)wPageTableCount=m_pGameServiceOption->wTableCount;
	pIServerUserItem->SetMobileUserRule(wBehaviorFlags);
}

//Ⱥ���û���Ϣ
bool CAttemperEngineSink::SendUserInfoPacketBatchToMobileUser(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//�û�����
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//�û�����
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;
	//�û��ɼ�
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	pUserInfoHead->dwClientAddr=pIServerUserItem->GetClientAddr();

	lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//������Ϣ
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);

	//��������
	WORD wHeadSize=sizeof(tagMobileUserInfoHead);
	if (dwSocketID==INVALID_DWORD)
	{
		SendDataBatchToMobileUser(pUserInfo->wTableID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}
	else
	{
		//��ȡ�û�
		WORD wBindIndex=LOWORD(dwSocketID);
		IServerUserItem * pITargetUserItem=GetBindUserItem(wBindIndex);
		ASSERT(pITargetUserItem!=NULL);
		if (pITargetUserItem==NULL) return true;
		if (pITargetUserItem->IsMobileUser()==false) return true;

		//������Һ�Ŀ�������ͬһ�� ���� �������������
		if (pIServerUserItem==pITargetUserItem || pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
		}
	}

	return true;
}

//�������û���Ϣ
bool CAttemperEngineSink::SendVisibleTableUserInfoToMobileUser(IServerUserItem * pIServerUserItem,WORD wTablePos)
{
	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
	WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wTagerDeskCount = m_pInitParameter->m_wVisibleTableCount;
	WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;
	bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
	if(wTagerDeskCount==0) wTagerDeskCount=1;

	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) continue;
		if (pIUserItem==pIServerUserItem) continue;
		if (pIUserItem->GetTableID()==INVALID_TABLE) continue;

		//���ֿ���
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

		//�������
		tagUserInfo * pUserInfo=pIUserItem->GetUserInfo();
		ZeroMemory(cbBuffer,sizeof(cbBuffer));
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

		//�û�����
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
		pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

		//�û�����
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//�û�״̬
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;
		pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

		//�û��ɼ�
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pIUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pIUserItem->GetFrozenedScore();
		pUserInfoHead->lIngot=pUserInfo->lIngot;
		pUserInfoHead->dBeans=pUserInfo->dBeans;
		pUserInfoHead->bAndroid = pIUserItem->IsAndroidUser();
		lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

		//������Ϣ
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//��������
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//���͸�����Ϣ��ĳһҳ
bool CAttemperEngineSink::SendUserInfoPacketBatchToPage(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem!=NULL && pIServerUserItem->GetTableID()!=INVALID_TABLE);
	if (pIServerUserItem==NULL || pIServerUserItem->GetTableID()==INVALID_TABLE) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	ZeroMemory(cbBuffer,sizeof(cbBuffer));
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
	WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wTagerDeskCount=m_pInitParameter->m_wVisibleTableCount;
	WORD wEverySendPage = m_pInitParameter->m_wEverySendPageCount;
	WORD wLastPagePos=(m_pGameServiceOption->wTableCount/wTagerDeskCount)*wTagerDeskCount;
	bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);

	//�������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwAgentID=pUserInfo->dwAgentID;

	//�û�����
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//�û�����
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lIntegralCount=pUserInfo->lIntegralCount;

	//�û��ɼ�
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();
	pUserInfoHead->lIngot=pUserInfo->lIngot;
	pUserInfoHead->dBeans=pUserInfo->dBeans;
	pUserInfoHead->dwClientAddr=pIServerUserItem->GetClientAddr();

	lstrcpyn(pUserInfoHead->szChangeLogonIP, pUserInfo->szChangeLogonIP, CountArray(pUserInfoHead->szChangeLogonIP));

	//������Ϣ
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	pUserInfoHead->bAndroid = pIServerUserItem->IsAndroidUser();
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) continue;
		if (pIUserItem->IsMobileUser()==false) continue;

		//���ֿ���
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

		//��������
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//�ֻ�������¼
bool CAttemperEngineSink::MobileUserImmediately(IServerUserItem * pIServerUserItem)
{
	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//��������
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=((INT)i+nStartIndex)%(INT)(m_pGameServiceOption->wTableCount);

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//��������
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=((INT)i+nStartIndex)%(INT)(m_pGameServiceOption->wTableCount);

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//ʧ��
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	return true;
}

//����ϵͳ��Ϣ
bool CAttemperEngineSink::SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize)
{
	//��Ϣ����
	ASSERT(pSendMessage!=NULL);

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength);
	if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength) 
		return false;

	//���з���
	if(pSendMessage->cbAllRoom == TRUE)
	{
		pSendMessage->cbAllRoom=FALSE;
		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_SYSTEM_MESSAGE,pSendMessage,wDataSize);
	}
	else
	{
		//����ϵͳ��Ϣ
		if(pSendMessage->cbGame == TRUE)
			SendGameMessage(pSendMessage->szSystemMessage,SMT_CHAT);
		if(pSendMessage->cbRoom == TRUE)
			SendRoomMessage(pSendMessage->szSystemMessage,SMT_CHAT);
	}

	return true;
}

//�Ƴ���Ϣ
void CAttemperEngineSink::RemoveSystemMessage()
{
	//������Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}

//��ʼ��Ϸд�������Ϣ
VOID CAttemperEngineSink::PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, TCHAR * szRoomID)
{
	//�˻�����
	DBR_GR_WriteJoinInfo JoinInfo;
	ZeroMemory(&JoinInfo, sizeof(DBR_GR_WriteJoinInfo));

	JoinInfo.dwUserID = dwUserID;
	JoinInfo.dwTableID = wTableID;
	lstrcpyn(JoinInfo.szRoomID,  szRoomID, sizeof(JoinInfo.szRoomID) );

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(0, DBR_GR_WRITE_JOIN_INFO, 0, &JoinInfo, sizeof(JoinInfo));

}
//���¿���
bool CAttemperEngineSink::OnDBUpdateControlInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(sizeof(DBO_GR_UpdateControlInfo)==wDataSize);
	if(sizeof(DBO_GR_UpdateControlInfo)!=wDataSize) return false;

	//��������
	DBO_GR_UpdateControlInfo * pUpdateControlInfo=(DBO_GR_UpdateControlInfo *)pData;
	if(pUpdateControlInfo==NULL) return false;

	//��ȡ�û�
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

		//������ҵ��պ�����Ӯ
		SCORE lTdTotalScore = pIServerUserItem->GetTdTotalScore();
		SCORE lTotalScore = pIServerUserItem->GetTotalScore();

	}

	if(pUpdateControlInfo->cbNullity == 1)
	{
		TCHAR szChat[256] = TEXT("");
		SendGameMessage(pIServerUserItem,TEXT(""),SMT_CLOSE_GAME);
		_sntprintf(szChat,CountArray(szChat),TEXT("�����˺�Ϊ�쳣�˺ţ��Ѿ���ϵͳ������!"));
		SendRoomMessage(pIServerUserItem, szChat, SMT_EJECT);
		SendRoomMessage(pIServerUserItem,szChat,SMT_CLOSE_ROOM);
		//SendRoomMessage(pIServerUserItem, szChat, SMT_EJECT);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
