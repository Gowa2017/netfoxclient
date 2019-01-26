#include "StdAfx.h"
#include "AndroidUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ANDROID_LOGON_COUNT			32									//��¼��Ŀ
#define ADNDROID_PLAY_DRAW          10                                  //��Ϸ����  
#define ANDROID_UNLOAD_TIME			30*60								//������ʱ��
#define ANDROID_UNIN_TIME			10*60								//������ʱ��

//ʱ���ʶ
#define IDI_ANDROID_IN				(IDI_REBOT_MODULE_START+0)			//����ʱ��
#define IDI_ANDROID_OUT				(IDI_REBOT_MODULE_START+MAX_BATCH)	//�˳�ʱ��
#define IDI_ANDROID_PULSE			(IDI_REBOT_MODULE_START+2*MAX_BATCH)//����ʱ��

//����ʱ��
#define TIME_ANDROID_INOUT			15L									//��¼ʱ��
#define TIME_ANDROID_PULSE			1L									//����ʱ��
#define TIME_LOAD_ANDROID_INFO		900L								//�����û�
#define TIME_ANDROID_REPOSE_TIME	1800L								//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserManager::CAndroidUserManager()
{
	//ϵͳ����
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameMatchOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pIServerUserManager=NULL;
	m_pIGameServiceManager=NULL;
	m_pIGameServiceSustomTime=NULL;
	m_pITCPNetworkEngineEvent=NULL;
	m_pIMainServiceFrame=NULL;

	//���ñ���	
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_bServiceContinue=false;
	m_wAutoAndroidCount=ANDROID_LOGON_COUNT;

	//������
	m_wStockCount=0;
	ZeroMemory(&m_AndroidItemConfig,sizeof(m_AndroidItemConfig));

	//��������
	m_AndroidUserItemMap.InitHashTable(PRIME_ANDROID_USER);
	m_AndroidUserItemMap.RemoveAll();

	return;
}

//��������
CAndroidUserManager::~CAndroidUserManager()
{
	//�������
	ASSERT(m_AndroidUserItemMap.GetCount()==0L);
	ASSERT(m_AndroidParameterArray.GetCount()==0L);
	ASSERT(m_AndroidUserItemArray.GetCount()==0L);
	ASSERT(m_AndroidUserItemBuffer.GetCount()==0L);

	return;
}

//�ӿڲ�ѯ
VOID * CAndroidUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserManager,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CAndroidUserManager::StartService()
{
	//ʱ����
	DWORD dwTimeCell=TIME_CELL;
	DWORD dwElapse=TIME_ANDROID_PULSE*1000L;

	//����ʱ��
	if (m_pIGameServiceSustomTime!=NULL)
	{		
		dwTimeCell=m_pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		dwElapse=m_pIGameServiceSustomTime->GetAndroidTimerPulse();		
		if (dwElapse<dwTimeCell) dwElapse=dwTimeCell;
	}

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_ANDROID_PULSE,dwElapse,TIMES_INFINITY,0);

	return true;
}

//ֹͣ����
bool CAndroidUserManager::ConcludeService()
{
	//ɾ���洢
	for (INT_PTR i=0;i<m_AndroidUserItemArray.GetCount();i++)
	{
		SafeRelease(m_AndroidUserItemArray[i]);
	}

	//ɾ���洢
	for (INT_PTR i=0;i<m_AndroidParameterArray.GetCount();i++)
	{
		SafeDelete(m_AndroidParameterArray[i]);
	}

	//��������
	m_AndroidUserItemMap.RemoveAll();
	m_AndroidParameterArray.RemoveAll();
	m_AndroidUserItemArray.RemoveAll();
	m_AndroidUserItemBuffer.RemoveAll();

	//����û�
	m_wStockCount=0;
	ZeroMemory(&m_AndroidItemConfig,sizeof(m_AndroidItemConfig));

	return true;
}

//�������
bool CAndroidUserManager::InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter)
{
	//��������
	m_bServiceContinue=AndroidUserParameter.bServiceContinue;
	m_dwMinSitInterval=AndroidUserParameter.dwMinSitInterval;
	m_dwMaxSitInterval=AndroidUserParameter.dwMaxSitInterval;

	//���ñ���
	m_pGameParameter=AndroidUserParameter.pGameParameter;
	m_pGameServiceAttrib=AndroidUserParameter.pGameServiceAttrib;
	m_pGameServiceOption=AndroidUserParameter.pGameServiceOption;
	m_pGameMatchOption=AndroidUserParameter.pGameMatchOption;

	//����ӿ�
	m_pITimerEngine=AndroidUserParameter.pITimerEngine;
	m_pIServerUserManager=AndroidUserParameter.pIServerUserManager;
	m_pIGameServiceManager=AndroidUserParameter.pIGameServiceManager;
	m_pIGameServiceSustomTime=AndroidUserParameter.pIGameServiceSustomTime;
	m_pITCPNetworkEngineEvent=AndroidUserParameter.pITCPNetworkEngineEvent;

	return true;
}

//�Ƴ�����
bool CAndroidUserManager::RemoveAndroidParameter(DWORD dwBatchID)
{
	//��ȡ����
	tagAndroidParameterEx * pAndroidParameter = GetAndroidParameter(dwBatchID);
	if(pAndroidParameter==NULL) return false;

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount= GetTodayTickCount(SystemTime);

	//��������
	pAndroidParameter->bIsValided=false;
	pAndroidParameter->AndroidParameter.dwLeaveTime=dwTodayTickCount;

	//��ȡ����
	WORD wParameterIndex=GetAndroidParameterIndex(pAndroidParameter);
	if(wParameterIndex==INVALID_WORD) return true;

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN+wParameterIndex);

	//ɾ�����
	for (int i=m_wStockCount-1;i>=0;i--)
	{
		//�����ж�
		if(m_AndroidItemConfig[i].pAndroidParameter!=&pAndroidParameter->AndroidParameter) continue;

		//ɾ�����
		m_wStockCount--;
		m_AndroidItemConfig[i]=m_AndroidItemConfig[m_wStockCount];
	}

	//�����ж�
	if(pAndroidParameter->bIsLoadAndroid==false)
	{
		//�ͷ���Դ
		SafeDelete(pAndroidParameter);
		m_AndroidParameterArray[wParameterIndex]=NULL;

		return true;
	}

	//������ʱ��
	m_pITimerEngine->SetTimer(IDI_ANDROID_OUT+wParameterIndex,TIME_ANDROID_INOUT*1000L,TIMES_INFINITY,(WPARAM)pAndroidParameter);

	return true;
}

//���ò���
bool CAndroidUserManager::AddAndroidParameter(tagAndroidParameter AndroidParameter[], WORD wParameterCount)
{
	//��������
	tagAndroidParameterEx * pAndroidParameter=NULL;

	for(WORD wIndex=0;wIndex<wParameterCount;wIndex++)
	{
		//��Ŀ����
		if(m_AndroidParameterArray.GetCount()>=MAX_BATCH) break;

		//������Դ
		pAndroidParameter = GetAndroidParameter(AndroidParameter[wIndex].dwBatchID);
		if(pAndroidParameter==NULL)
		{
			//������Դ
			pAndroidParameter = new tagAndroidParameterEx();
			if(pAndroidParameter==NULL) return false;

			//���ñ���
			pAndroidParameter->bIsValided=true;
			pAndroidParameter->bIsLoadAndroid=false;

			//��������
			bool bVacancyExist=false;

			//���ҿ�ȱ
			for(INT_PTR nIndex=0;nIndex<m_AndroidParameterArray.GetCount();nIndex++)
			{
				if(m_AndroidParameterArray[nIndex]==NULL)
				{
					bVacancyExist=true;
					m_AndroidParameterArray[nIndex]=pAndroidParameter;
					break;
				}
			}

			//�����ڿ�ȱ
			if(bVacancyExist==false)
			{
				m_AndroidParameterArray.Add(pAndroidParameter);
			}
		}

		//��������
		CopyMemory(pAndroidParameter,&AndroidParameter[wIndex],sizeof(tagAndroidParameter));
	}

	return true;
}

//�������
bool CAndroidUserManager::InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[],WORD wAndroidCount,DWORD dwBatchID)
{
	//��ȡ����
	tagAndroidParameterEx * pAndroidParameter = GetAndroidParameter(dwBatchID);
	if(pAndroidParameter==NULL) return false;

	//��¼���	
	CAndroidUserItem * pAndroidUserItem=NULL;
	WORD wStockCount=m_wStockCount;
	bool bAndroidExist=false;

	//����»���
	for(WORD wIndex=0;wIndex<wAndroidCount;wIndex++)
	{
		//���ñ���
		bAndroidExist=false;

		//�ӿ���в���
		for(WORD wItem=0;wItem<wStockCount;wItem++)
		{
			if(m_AndroidItemConfig[wItem].AndroidAccountsInfo.dwUserID==AndroidAccountsInfo[wIndex].dwUserID)
			{
				bAndroidExist=true;
				break;
			}
		}

		//�����ж�
		if(bAndroidExist==true) continue;

		//��ӳ���в���
		if(m_AndroidUserItemMap.Lookup(AndroidAccountsInfo[wIndex].dwUserID,pAndroidUserItem)==TRUE)
		{
			continue;
		}

		//��ӻ���
		m_AndroidItemConfig[m_wStockCount].AndroidAccountsInfo=AndroidAccountsInfo[wIndex];
		m_AndroidItemConfig[m_wStockCount].pAndroidParameter=&pAndroidParameter->AndroidParameter;

		//���ӿ��
		m_wStockCount++;
    }

	//��ȡ����
	WORD wParameterIndex=GetAndroidParameterIndex(pAndroidParameter);
	if(wParameterIndex==INVALID_WORD) return true;

	//������ʱ��
	m_pITimerEngine->KillTimer(IDI_ANDROID_IN+wParameterIndex);
	m_pITimerEngine->SetTimer(IDI_ANDROID_IN+wParameterIndex,TIME_ANDROID_INOUT*1000L,TIMES_INFINITY,(WPARAM)pAndroidParameter);

	return true;
}

//ɾ������
bool CAndroidUserManager::DeleteAndroidUserItem(DWORD dwAndroidID,bool bStockRetrieve)
{
	//��ȡ����
	WORD wIndex=LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem=GetAndroidUserItem(wIndex);

	//����Ч��
	//ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem==NULL)||(pAndroidUserItem->m_wRoundID!=HIWORD(dwAndroidID))) return false;

	//�ر��¼�
	try
	{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwAndroidID,0,0L);
	}
	catch (...)
	{
		//�������
		ASSERT(FALSE);
	}

	//��������
	tagAndroidItemConfig AndroidItemConfig = pAndroidUserItem->m_AndroidItemConfig;

	//ɾ������
	FreeAndroidUserItem(pAndroidUserItem->GetUserID());

	////������
	//if(bStockRetrieve==true)
	//{		
	//	InsertAndroidInfo(&AndroidItemConfig.AndroidAccountsInfo,1,AndroidItemConfig.pAndroidParameter->dwBatchID);
	//}

	return true;
}

//���һ���
IAndroidUserItem * CAndroidUserManager::SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID)
{
	//���һ���
	CAndroidUserItem * pAndroidUserItem=NULL;
	m_AndroidUserItemMap.Lookup(dwUserID,pAndroidUserItem);

	//�����ж�
	if (pAndroidUserItem!=NULL)
	{
		WORD wRoundID=pAndroidUserItem->m_wRoundID;
		WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;
		if (MAKELONG(wAndroidIndex,wRoundID)==dwContextID) return pAndroidUserItem;
	}

	return NULL;
}

//��������
IAndroidUserItem * CAndroidUserManager::CreateAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig)
{
	//Ч�����
	ASSERT(AndroidItemConfig.AndroidAccountsInfo.dwUserID!=0L);
	if (AndroidItemConfig.AndroidAccountsInfo.dwUserID==0L) return NULL;

	//��������
	CAndroidUserItem * pAndroidUserItem=ActiveAndroidUserItem(AndroidItemConfig);
	if (pAndroidUserItem==NULL) return NULL;

	//���Ա���
	WORD wRoundID=pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;

	//����ģ��
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wAndroidIndex,wRoundID),0L)==false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		//�������
		ASSERT(FALSE);

		//�ͷ��û�
		FreeAndroidUserItem(AndroidItemConfig.AndroidAccountsInfo.dwUserID);

		return NULL;
	}

	//��������
	CMD_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//�汾��Ϣ
	LogonUserID.dwPlazaVersion=VERSION_PLAZA;
	LogonUserID.dwFrameVersion=VERSION_FRAME;
	LogonUserID.dwProcessVersion=m_pGameServiceAttrib->dwClientVersion;

	//�û���Ϣ
	LogonUserID.dwUserID=AndroidItemConfig.AndroidAccountsInfo.dwUserID;
	lstrcpyn(LogonUserID.szPassword,AndroidItemConfig.AndroidAccountsInfo.szPassword,CountArray(LogonUserID.szPassword));
	LogonUserID.wKindID=m_pGameServiceOption->wKindID;

	//��������
	TCP_Command Command;
	Command.wMainCmdID=MDM_GR_LOGON;
	Command.wSubCmdID=SUB_GR_LOGON_USERID;

	//��Ϣ����
	try
	{
		//��������
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex,wRoundID),Command,&LogonUserID,sizeof(LogonUserID))==false)
		{
			throw 0;
		}
	}
	catch (...) 
	{
		//�������
		ASSERT(FALSE);

		//ɾ������
		DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID),false);

		return NULL;
	}

	return pAndroidUserItem;
}

//���ýӿ�
VOID CAndroidUserManager::SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame)
{
	m_pIMainServiceFrame=pIMainServiceFrame;

	return;
}

//�����¼�
bool CAndroidUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//���봦��
	if(dwTimerID>=IDI_ANDROID_IN && dwTimerID<IDI_ANDROID_IN+MAX_BATCH)			
	{
		//��ȡʱ��
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);
		DWORD dwTodayTickCount= GetTodayTickCount(SystemTime);

		//��ȡ����
		tagAndroidParameter * pAndroidParameter=(tagAndroidParameter *)dwBindParameter;
		if(pAndroidParameter==NULL)
		{
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//��������
		bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);

		//�ɹ���ʶ
		bool bLogonSuccessed=false;

		//��¼����
		if ((bAllowAndroidAttend==true)||(bAllowAndroidSimulate==true))
		{
			//Ѱ�һ���
			for (WORD i=0;i<m_wStockCount;i++)
			{
				//�����ж�
				if(m_AndroidItemConfig[i].pAndroidParameter!=pAndroidParameter) continue;

				//ģ���ж�
				if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
					&& (pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
					&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
					&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
				{
					continue;
				}

				//��������
				if (CreateAndroidUserItem(m_AndroidItemConfig[i])!=NULL)
				{
					//ɾ�����
					m_wStockCount--;
					bLogonSuccessed=true;
					m_AndroidItemConfig[i]=m_AndroidItemConfig[m_wStockCount];

					break;
				}
			}
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��ʱ������
		if(bLogonSuccessed==true)
		{
			//��������ʱ��
			DWORD dwElapse = pAndroidParameter->dwEnterMinInterval+rand()%(pAndroidParameter->dwEnterMaxInterval-pAndroidParameter->dwEnterMinInterval+1);
			if(dwElapse==0) dwElapse=TIME_ANDROID_INOUT;

			//���ö�ʱ��
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
		{
			//ʣ��ʱ��
			DWORD dwRemaindTime = BatchServiceRemaindTime(pAndroidParameter,dwTodayTickCount);
			if(dwRemaindTime<=ANDROID_UNIN_TIME)
			{
				//���ö�ʱ��
				m_pITimerEngine->SetTimer(IDI_ANDROID_OUT+(dwTimerID-IDI_ANDROID_IN),TIME_ANDROID_INOUT*1000L,TIMES_INFINITY,dwBindParameter);

				CString str;
				str.Format(TEXT("�����λ����˿�ʼ�˳�, ����ID=%d"),pAndroidParameter->dwBatchID);
				CTraceService::TraceString(str,TraceLevel_Info);
			}
			else
			{
				//���ö�ʱ��
				m_pITimerEngine->SetTimer(dwTimerID,TIME_ANDROID_INOUT*1000L,TIMES_INFINITY,dwBindParameter);				
			}
		}

		return true;
	}

	//�˳�����
	if(dwTimerID>=IDI_ANDROID_OUT && dwTimerID<IDI_ANDROID_OUT+MAX_BATCH)
	{
		//��������
		DWORD dwUserID=0L;
		INT_PTR nActiveCount=m_AndroidUserItemMap.GetCount();
		POSITION Position=m_AndroidUserItemMap.GetStartPosition();

		//��ȡʱ��
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);
		DWORD dwTodayTickCount= GetTodayTickCount(SystemTime);	

		//��ȡ����
		tagAndroidParameterEx * pAndroidParameter=(tagAndroidParameterEx *)dwBindParameter;
		if(pAndroidParameter==NULL)
		{
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//ʱ���ж�
		if (BatchServiceRemaindTime(&pAndroidParameter->AndroidParameter,dwTodayTickCount)>0) 
		{			
			return true;
		}

		//��������
		bool bLogoutMarked=false;
		bool bLogoutFinished=true;		

		//�˳�����		
		while (Position!=NULL)
		{
			//��ȡ����
			CAndroidUserItem * pAndroidUserItem=NULL;
			m_AndroidUserItemMap.GetNextAssoc(Position,dwUserID,pAndroidUserItem);

			//�˳��ж�
			if (pAndroidUserItem->GetAndroidParameter()==&pAndroidParameter->AndroidParameter)
			{
				//���ñ���
				bLogoutFinished=false;

				//��������
				IServerUserItem * pIServerUserItem=pAndroidUserItem->m_pIServerUserItem;
				if(pIServerUserItem!=NULL)
				{
					//����״̬
					BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
					BYTE cbMatchStatus=pIServerUserItem->GetUserMatchStatus();
					if ((cbUserStatus!=US_FREE)&&(cbUserStatus!=US_SIT))
					{
						if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
						{
							//��ֹ��;����ʱUserStatus��ΪUS_FREE���¿�������ʱʹ��SignUpStatus�жϿɷ��߳�
							if(cbMatchStatus!=MUS_NULL && pAndroidUserItem->m_bWaitLeave==false)
							{
								pAndroidUserItem->m_bWaitLeave=true;
							}
						}
						else
						{
							//��Ƕ���
							if(bLogoutMarked==false && pAndroidUserItem->m_wAndroidAction==0)
							{
								bLogoutMarked=true;
								pAndroidUserItem->m_wAndroidAction|=ANDROID_WAITLEAVE;
							}
						}
						continue;
					}
					else
					{
						if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
						{
							WORD wTableID = pIServerUserItem->GetTableID();
							if((cbMatchStatus!=MUS_NULL ||  pAndroidUserItem->m_bWaitLeave==false) && wTableID != INVALID_TABLE)
							{
								pAndroidUserItem->m_bWaitLeave=true;
								continue;
							}
						}
					}
				}

				//ɾ���û�
				WORD wRoundID=pAndroidUserItem->m_wRoundID;
				WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;
				DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID),false);

				//���ñ�ʶ
				if(pIServerUserItem!=NULL) break;
			}			
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��ʱ������
		if(bLogoutFinished==false)
		{
			//��������ʱ��
			DWORD dwElapse = pAndroidParameter->AndroidParameter.dwLeaveMinInterval+rand()%(pAndroidParameter->AndroidParameter.dwLeaveMaxInterval-pAndroidParameter->AndroidParameter.dwLeaveMinInterval+1);
			if(dwElapse==0) dwElapse=TIME_ANDROID_INOUT;

			//���ö�ʱ��
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
		{
			if(pAndroidParameter->bIsValided==false)
			{
				//��������
				WORD wParameterIndex=(WORD)(dwTimerID-IDI_ANDROID_OUT);
				if(wParameterIndex==GetAndroidParameterIndex(pAndroidParameter))
				{
					//�ͷ���Դ
					SafeDelete(m_AndroidParameterArray[wParameterIndex]);
					m_AndroidParameterArray[wParameterIndex]=NULL;

					CString str;
					str.Format(TEXT("�����λ�����ȫ���˳�,�����ν���ɾ��, ����ID=%d"),pAndroidParameter->AndroidParameter.dwBatchID);
					CTraceService::TraceString(str,TraceLevel_Info);

					//����������
					if (m_pIMainServiceFrame!=NULL)
					{
						m_pIMainServiceFrame->UnLockAndroidUser(m_pGameServiceOption->wServerID,(WORD)pAndroidParameter->AndroidParameter.dwBatchID);
					}
				}
			}
			else
			{
				//�ָ���ʶ
				pAndroidParameter->bIsLoadAndroid=false;
				
				CString str;
				str.Format(TEXT("�����λ�����ȫ���˳�, ����ID=%d"),pAndroidParameter->AndroidParameter.dwBatchID);
				CTraceService::TraceString(str,TraceLevel_Info);

				//����������
				if (m_pIMainServiceFrame!=NULL)
				{
					m_pIMainServiceFrame->UnLockAndroidUser(m_pGameServiceOption->wServerID,(WORD)pAndroidParameter->AndroidParameter.dwBatchID);
				}
			}
		}

		return true;
	}

	//�û�����
	if(dwTimerID==IDI_ANDROID_PULSE)		
	{
		//��������
		DWORD dwUserID=0L;
		CAndroidUserItem * pAndroidUserItem=NULL;
		POSITION Position=m_AndroidUserItemMap.GetStartPosition();

		//�û�����
		while (Position!=NULL)
		{
			//��ȡ����
			m_AndroidUserItemMap.GetNextAssoc(Position,dwUserID,pAndroidUserItem);

			//ʱ�䴦��
			try
			{
				//ASSERT(pAndroidUserItem!=NULL);
				if (pAndroidUserItem->m_pIServerUserItem!=NULL) pAndroidUserItem->OnTimerPulse(dwTimerID,dwBindParameter);
			}
			catch (...)
			{
				ASSERT(FALSE);
				DeleteAndroidUserItem(MAKELONG(pAndroidUserItem->m_wAndroidIndex,pAndroidUserItem->m_wRoundID),true);
			}
		}

		return true;
	}

	return false;
}

//���ػ���
bool CAndroidUserManager::GetAndroidLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)
{
	//��Ŀ�ж�
	if(m_AndroidParameterArray.GetCount()==0) return false;
	
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount= GetTodayTickCount(SystemTime);

	//��������
	tagAndroidParameterEx * pAndroidParameter=NULL;
	DWORD dwRemaindTime;

	//�˳�����
	for(INT_PTR nIndex=0;nIndex<m_AndroidParameterArray.GetCount();nIndex++)
	{
		//��ȡ����		
		pAndroidParameter = m_AndroidParameterArray[nIndex];
		if(pAndroidParameter==NULL) continue;

		//�����ж�
		if(pAndroidParameter->bIsLoadAndroid==true) continue;

		//ʣ��ʱ��
		dwRemaindTime=BatchServiceRemaindTime(&pAndroidParameter->AndroidParameter,dwTodayTickCount);

		//���ػ���
		if(dwRemaindTime>ANDROID_UNLOAD_TIME)
		{
			//���ñ�ʶ
			pAndroidParameter->bIsLoadAndroid=true;

			//���ò���
			dwBatchID = pAndroidParameter->AndroidParameter.dwBatchID;
			dwLoadCount = __min(pAndroidParameter->AndroidParameter.dwAndroidCount,dwRemaindTime/pAndroidParameter->AndroidParameter.dwEnterMaxInterval);	
			if (dwLoadCount==0) continue;

			return true;
		}
	}

	return false;
}

//�û�״��
WORD CAndroidUserManager::GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount= GetTodayTickCount(SystemTime);	

	//���ñ���
	ZeroMemory(&AndroidUserInfo,sizeof(AndroidUserInfo));

	//��������
	DWORD dwTimeMask=(1L<<SystemTime.wHour);
	POSITION Position=m_AndroidUserItemMap.GetStartPosition();

	//ö�ٶ���
	while (Position!=NULL)
	{
		//��ȡ����
		DWORD dwUserID=0L;
		CAndroidUserItem * pAndroidUserItem=NULL;
		m_AndroidUserItemMap.GetNextAssoc(Position,dwUserID,pAndroidUserItem);

		//Ч�����
		ASSERT((dwUserID!=0L)&&(pAndroidUserItem!=NULL));
		if ((dwUserID==0L)||(pAndroidUserItem==NULL)) break;

		//���ж�
		if(pAndroidUserItem->m_pIServerUserItem==NULL) continue;

		//��������
		IServerUserItem * pIServerUserItem=pAndroidUserItem->m_pIServerUserItem;
		tagAndroidParameter * pAndroidParameter=pAndroidUserItem->GetAndroidParameter();

		//�뿪�ж�
		if (pAndroidUserItem->m_bWaitLeave) continue;

		//ģʽ�ж�
		if ((pAndroidParameter->dwServiceMode&dwServiceMode)==0L) continue;		

		//�뿪�ж�
		if (!m_bServiceContinue && BatchServiceRemaindTime(pAndroidParameter,dwTodayTickCount)==0) continue;

		//��Ϣʱ��
		DWORD dwCurrTime=(DWORD)time(NULL);
		if((pAndroidUserItem->m_dwStandupTickCount+pAndroidUserItem->m_dwReposeTickCount)>dwCurrTime) continue;

		//״̬�ж�
		switch (pIServerUserItem->GetUserStatus())
		{
		case US_FREE:
			{
				ASSERT(AndroidUserInfo.wFreeUserCount<CountArray(AndroidUserInfo.pIAndroidUserFree));
				AndroidUserInfo.pIAndroidUserFree[AndroidUserInfo.wFreeUserCount++]=pAndroidUserItem;
				break;
			}
		case US_SIT:
		case US_READY:
			{
				ASSERT(AndroidUserInfo.wSitdownUserCount<CountArray(AndroidUserInfo.pIAndroidUserSitdown));
				AndroidUserInfo.pIAndroidUserSitdown[AndroidUserInfo.wSitdownUserCount++]=pAndroidUserItem;
				break;
			}
		case US_PLAYING:
		case US_OFFLINE:
			{
				ASSERT(AndroidUserInfo.wPlayUserCount<CountArray(AndroidUserInfo.pIAndroidUserPlay));
				AndroidUserInfo.pIAndroidUserPlay[AndroidUserInfo.wPlayUserCount++]=pAndroidUserItem;
				break;
			}
		}
	};

	return AndroidUserInfo.wFreeUserCount+AndroidUserInfo.wPlayUserCount+AndroidUserInfo.wSitdownUserCount;
}

//��������
bool CAndroidUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	for (INT_PTR i=0;i<m_AndroidUserItemArray.GetCount();i++)
	{
		//��ȡ����
		CAndroidUserItem * pAndroidUserItem=m_AndroidUserItemArray[i];

		//״̬�ж�
		if (pAndroidUserItem->m_pIServerUserItem==NULL) continue;
		if (pAndroidUserItem->m_AndroidItemConfig.AndroidAccountsInfo.dwUserID==0L) continue;

		//��Ϣ����
		try
		{
			if (pAndroidUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
			{
				throw 0;
			}
		}
		catch (...) 
		{
			//�������
			ASSERT(FALSE);

			//�Ͽ��û�
			WORD wRoundID=pAndroidUserItem->m_wRoundID;
			WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID),true);
		}
	}

	return true;
}

//��������
bool CAndroidUserManager::SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	WORD wIndex=LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem=GetAndroidUserItem(wIndex);

	//����Ч��
	//ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRoundID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem==NULL)||(pAndroidUserItem->m_wRoundID!=HIWORD(dwAndroidID))) return false;

	//��Ϣ����
	try
	{
		if (pAndroidUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
		{
			ASSERT(FALSE);
			throw 0;
		}
	}
	catch (...) 
	{
		ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID,true);
	}

	return true;
}

//��������
bool CAndroidUserManager::SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	TCP_Command Command;
	Command.wSubCmdID=wSubCmdID;
	Command.wMainCmdID=wMainCmdID;

	//��Ϣ����
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwAndroidID,Command,pData,wDataSize)==false)
		{
			throw 0;
		}
	}
	catch (...) 
	{
		CString str;
		str.Format(TEXT("SendDataToServer: MainID=%d,SubID=%d"),Command.wMainCmdID,Command.wSubCmdID);
		CTraceService::TraceString(str,TraceLevel_Exception);

		ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID,true);
	}

	return true;
}

//��ȡ����
CAndroidUserItem * CAndroidUserManager::GetAndroidUserItem(WORD wIndex)
{
	//Ч������
	ASSERT(wIndex>=INDEX_ANDROID);
	if (wIndex<INDEX_ANDROID) return NULL;

	//Ч������
	ASSERT((wIndex-INDEX_ANDROID)<m_AndroidUserItemArray.GetCount());
	if (((wIndex-INDEX_ANDROID)>=m_AndroidUserItemArray.GetCount())) return NULL;

	//��ȡ����
	WORD wBufferIndex=wIndex-INDEX_ANDROID;
	CAndroidUserItem * pAndroidUserItem=m_AndroidUserItemArray[wBufferIndex];

	return pAndroidUserItem;
}

//�ͷŶ���
VOID CAndroidUserManager::FreeAndroidUserItem(DWORD dwUserID)
{
	//Ч�����
	ASSERT(dwUserID!=0);
	if (dwUserID==0) return;

	//��������
	CAndroidUserItem * pAndroidUserItem=NULL;
	m_AndroidUserItemMap.Lookup(dwUserID,pAndroidUserItem);

	//�����ж�
	if (pAndroidUserItem==NULL)
	{
		ASSERT(FALSE);
		return;
	}

	//��λ����
	pAndroidUserItem->RepositUserItem();

	//��������
	m_AndroidUserItemMap.RemoveKey(dwUserID);
	m_AndroidUserItemBuffer.Add(pAndroidUserItem);

	return;
}

//��ȡ����
tagAndroidParameterEx * CAndroidUserManager::GetAndroidParameter(DWORD dwBatchID)
{
	//��������
	for(INT_PTR nIndex=0;nIndex<m_AndroidParameterArray.GetCount();nIndex++)
	{
		if(m_AndroidParameterArray[nIndex]==NULL) continue;
		if(m_AndroidParameterArray[nIndex]->AndroidParameter.dwBatchID==dwBatchID)
		{
			return m_AndroidParameterArray[nIndex];
		}
	}

	return NULL;
}

//��ȡ����
WORD CAndroidUserManager::GetAndroidParameterIndex(tagAndroidParameterEx * pAndroidParameter)
{
	//����У��
	if(pAndroidParameter==NULL) return INVALID_WORD; 

	//��������
	for(INT_PTR nIndex=0;nIndex<m_AndroidParameterArray.GetCount();nIndex++)
	{
		if(m_AndroidParameterArray[nIndex]==pAndroidParameter)
		{
			return (WORD)nIndex;
		}
	}

	return INVALID_WORD;
}

//ת��ʱ��
DWORD CAndroidUserManager::GetTodayTickCount(SYSTEMTIME & SystemTime)
{
	return SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;
}

//ʣ��ʱ��
DWORD CAndroidUserManager::BatchServiceRemaindTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount)
{
	//����У��
	ASSERT(pAndroidParameter!=NULL);
	if(pAndroidParameter==NULL) return 0;

	//��������
	DWORD dwEnterTime,dwLeaveTime;
	DWORD dwRemaindTime=0;

	//���ñ���
	dwEnterTime=pAndroidParameter->dwEnterTime;
	dwLeaveTime=pAndroidParameter->dwLeaveTime;

	// ����ʱ��
	// ����ʱ����뿪ʱ����ͬһ��
	// ����ʱ�䣬�뿪ʱ�䶼��ͬһ��� 00:00:00 ~ 23:59:59 (0~86399)
	if(dwLeaveTime>dwEnterTime)
	{
		if(dwTodayTickCount>=dwEnterTime && dwTodayTickCount<=dwLeaveTime)
		{
			dwRemaindTime=dwLeaveTime-dwTodayTickCount;
		}
	}
	// ����ʱ����뿪ʱ�䲻��ͬһ��
	// ����ʱ���ڵ�һ��12:00:01 ~ 23:59:59 (43201~86399)
	// �뿪ʱ���ڵڶ��� 00:00:00 ~ 12:00:00 (0~43200)
	else if(dwLeaveTime<dwEnterTime)
	{
		//��һ��
		if(dwTodayTickCount>=dwEnterTime)
		{
			dwRemaindTime = dwLeaveTime+24*3600-dwTodayTickCount;
		}

		//�ڶ���
		if(dwTodayTickCount< dwLeaveTime)
		{
			dwRemaindTime = dwLeaveTime-dwTodayTickCount;
		}
	}

	return dwRemaindTime;
}

//�ȴ�ʱ��
DWORD CAndroidUserManager::BatchServiceAwaitTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount)
{
	//����У��
	ASSERT(pAndroidParameter!=NULL);
	if(pAndroidParameter==NULL) return 0;
	if(((tagAndroidParameterEx *)pAndroidParameter)->bIsLoadAndroid==false) return -1;

	//��������
	DWORD dwEnterTime,dwLeaveTime;

	//���ñ���
	dwEnterTime=pAndroidParameter->dwEnterTime;
	dwLeaveTime=pAndroidParameter->dwLeaveTime;

	//����ʱ��
	if(dwEnterTime<dwLeaveTime)
	{
		if(dwTodayTickCount<dwEnterTime)
		{
			return dwEnterTime-dwTodayTickCount;
		}
	}
	else
	{
		if(dwTodayTickCount<dwEnterTime && dwTodayTickCount>dwLeaveTime)
		{
			return dwEnterTime-dwTodayTickCount;
		}
	}

	return 0;
}

//�������
CAndroidUserItem * CAndroidUserManager::ActiveAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig)
{
	//��������
	CAndroidUserItem * pAndroidUserItem=NULL;
	INT_PTR nFreeItemCount=m_AndroidUserItemBuffer.GetCount();

	//��ȡ����
	if (nFreeItemCount>0)
	{
		//��ȡ����
		INT_PTR nItemPostion=nFreeItemCount-1;
		pAndroidUserItem=m_AndroidUserItemBuffer[nItemPostion];

		//ɾ������
		m_AndroidUserItemBuffer.RemoveAt(nItemPostion);
	}

	//��������
	if (pAndroidUserItem==NULL)
	{
		//��Ŀ�ж�
		if (m_AndroidUserItemArray.GetCount()>=MAX_ANDROID)
		{
			ASSERT(FALSE);
			return NULL;
		}

		try
		{
			//��������
			IAndroidUserItemSink * pIAndroidUserItemSink=NULL;
			
			//��������
			if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==true)
			{
				pIAndroidUserItemSink=(IAndroidUserItemSink *)m_pIGameServiceManager->CreateAndroidUserItemSink(IID_IAndroidUserItemSink,VER_IAndroidUserItemSink);
			}

			//��������
			try
			{
				pAndroidUserItem=new CAndroidUserItem;
			}
			catch (...)
			{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIAndroidUserItemSink);
			}

			//�����ж�
			if (pAndroidUserItem==NULL)
			{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//�����û�
			if ((pIAndroidUserItemSink!=NULL)&&(pIAndroidUserItemSink->Initialization(pAndroidUserItem)==false))
			{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pAndroidUserItem);
				SafeRelease(pIAndroidUserItemSink);

				return NULL;
			}

			//��������
			WORD wCurrentIndex=(WORD)m_AndroidUserItemArray.Add(pAndroidUserItem);

			//���ñ���
			pAndroidUserItem->m_dwMinSitInterval=m_dwMinSitInterval;
			pAndroidUserItem->m_dwMaxSitInterval=m_dwMaxSitInterval;

			//���ýӿ�
			pAndroidUserItem->m_wAndroidIndex=wCurrentIndex+INDEX_ANDROID;
			pAndroidUserItem->m_pIServerUserManager=m_pIServerUserManager;
			pAndroidUserItem->m_pIAndroidUserItemSink=pIAndroidUserItemSink;
			pAndroidUserItem->m_pIAndroidUserManager=QUERY_ME_INTERFACE(IAndroidUserManager);
		}
		catch (...) 
		{ 
			ASSERT(FALSE);
			return NULL; 
		}
	}

	//��Ϸ����
	if((AndroidItemConfig.pAndroidParameter->dwSwitchMinInnings!=0L)&&(AndroidItemConfig.pAndroidParameter->dwSwitchMaxInnings!=0L))
	{
		//��������
		DWORD dwSwitchMinInnings=AndroidItemConfig.pAndroidParameter->dwSwitchMinInnings;
		DWORD dwSwitchMaxInnings=AndroidItemConfig.pAndroidParameter->dwSwitchMaxInnings;

		//��������
		if ((dwSwitchMaxInnings-dwSwitchMinInnings)>0L)
			pAndroidUserItem->m_AndroidService.dwSwitchTableInnings=dwSwitchMinInnings+rand()%(dwSwitchMaxInnings-dwSwitchMinInnings);
		else
			pAndroidUserItem->m_AndroidService.dwSwitchTableInnings=dwSwitchMinInnings;
	}
	else
		pAndroidUserItem->m_AndroidService.dwSwitchTableInnings=0;

	//��������
	if(pAndroidUserItem->m_pIAndroidUserItemSink==NULL && (AndroidItemConfig.pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0)
		AndroidItemConfig.pAndroidParameter->dwServiceMode=ANDROID_SIMULATE;		

	//���ñ���
	pAndroidUserItem->m_AndroidItemConfig=AndroidItemConfig;

	//��������
	m_AndroidUserItemMap[AndroidItemConfig.AndroidAccountsInfo.dwUserID]=pAndroidUserItem;

	return pAndroidUserItem;
}

//ǿ������
VOID CAndroidUserManager::ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem)
{
	//У�����
	ASSERT(pAndroidUserItem!=NULL);
	if(pAndroidUserItem==NULL) return;

	//���Ա���
	WORD wRoundID=pAndroidUserItem->m_wRoundID;
	WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;

	//����ṹ
	CMD_GR_UserStandUp UserStandUp;
	UserStandUp.cbForceLeave=true;
	UserStandUp.wTableID=pAndroidUserItem->m_pIServerUserItem->GetTableID();
	UserStandUp.wChairID=pAndroidUserItem->m_pIServerUserItem->GetChairID();	

	//��������
	SendDataToServer(MAKELONG(wAndroidIndex,wRoundID),MDM_GR_USER,SUB_GR_USER_STANDUP,&UserStandUp,sizeof(UserStandUp));

	return;
}


//////////////////////////////////////////////////////////////////////////////////
