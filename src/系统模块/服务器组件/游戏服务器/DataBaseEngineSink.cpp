#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//���ñ���
	m_pGameParameter=NULL;
	m_pInitParameter=NULL;
	m_pDataBaseParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);

	return;
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_GameDBModule.GetInterface()!=NULL);
	if (m_GameDBModule.GetInterface()==NULL) return NULL;

	//��ѯ����
	IDataBase * pIDataBase=m_GameDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//��ѯ����
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//Ͷ�ݽ��
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��ѯ����
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//��������
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϸ
	try
	{
		//������Ϣ
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_GameDBModule->SetConnectionInfo(m_pDataBaseParameter->szDataBaseAddr,m_pDataBaseParameter->wDataBasePort,
			m_pDataBaseParameter->szDataBaseName,m_pDataBaseParameter->szDataBaseUser,m_pDataBaseParameter->szDataBasePass);

		//��������
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//��������
		m_GameDBModule->OpenConnection();
		m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());

		//��������
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//���ݹ���
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//���ö���
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//�������
			ASSERT(FALSE);

			//�����Ϣ
			CTraceService::TraceString(TEXT("��Ϸ���ݿ���չ���������������ʧ��"),TraceLevel_Exception);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//��������
	DWORD dwUserID=0;
	DBR_GR_UnlockAndroidUser UnlockAndroidUser;
	UnlockAndroidUser.wServerID=m_pGameServiceOption->wServerID;
	UnlockAndroidUser.wBatchID=0;
	OnRequestUnlockAndroidUser(0,&UnlockAndroidUser,sizeof(UnlockAndroidUser),dwUserID);

	//���ñ���
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//���ö���
	m_GameDBAide.SetDataBase(NULL);

	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);

	//�ر�����
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//�ر�����
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//������
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:			//I D ��¼
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_MOBILE:			//�ֻ���¼
		{
			bSucceed = OnRequestLogonMobile(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_ACCOUNTS:			//�ʺŵ�¼
		{
			bSucceed = OnRequestLogonAccounts(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_GAME_SCORE:		//��Ϸд��
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_PERSONAL_GAME_SCORE:		//��Ϸд��˽�˷���
		{
			bSucceed = OnRequestWritePersonalGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//�뿪����
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//��Ϸ��¼
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_PARAMETER:			//���ز���
		{
			bSucceed = OnRequestLoadParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_COLUMN:		//�����б�
		{
			bSucceed = OnRequestLoadGameColumn(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_PARAMETER:	//��������
		{
			bSucceed = OnRequestLoadAndroidParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_USER:		//���ػ���
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GROWLEVEL_CONFIG:	//�ȼ�����
		{
			bSucceed = OnRequestLoadGrowLevelConfig(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_PROPERTY_BUY:		//�������
		{
			bSucceed = OnRequestBuyGameProperty(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_BACKPACK:			//��ѯ����
		{
			bSucceed = OnRequestPropertyBackpack(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_USE:			//ʹ�õ���
		{
			bSucceed = OnRequestPropertyUse(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_SEND_PRESENT:		//��ѯ����
		{
			bSucceed = OnRequestQuerySendPresent(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_PRESENT:		//���͵���
		{
			bSucceed = OnRequestPropertyPresent(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GET_SEND_PRESENT:		//��ȡ����
		{
			bSucceed = OnRequestGetSendPresent(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_BUFF:			//����Buff
		{
			bSucceed = OnRequestLoadGameBuff(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_TRUMPET:			//��������
		{
			bSucceed = OnRequestLoadGameTrumpet(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_SEND_TRUMPET:			//��������
		{
			bSucceed = OnRequestSendTrumpet(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_ENABLE_INSURE:		//��ͨ����
		{
			bSucceed = OnRequestUserEnableInsure(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_SAVE_SCORE:		//������Ϸ��
		{
			bSucceed = OnRequestUserSaveScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TAKE_SCORE:		//��ȡ��Ϸ��
		{
			bSucceed = OnRequestUserTakeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TRANSFER_SCORE:	//ת����Ϸ��
		{
			bSucceed = OnRequestUserTransferScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_INSURE_INFO:		//��ѯ����
		{
			bSucceed = OnRequestQueryInsureInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_TRANSFER_USER_INFO:		//��ѯ�û�
		{
			bSucceed = OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_USER_GAME_DATA:	//�û���Ϸ����
		{
			bSucceed = OnRequestWriteUserGameData(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MANAGE_USER_RIGHT:		//�û�Ȩ��
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SYSTEM_MESSAGE:   //ϵͳ��Ϣ
		{
			bSucceed = OnRequestLoadSystemMessage(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SENSITIVE_WORDS://�������д�
		{
			bSucceed = OnRequestLoadSensitiveWords(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_UNLOCK_ANDROID_USER://����������
		{
			bSucceed = OnRequestUnlockAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_UNLOCK_USER://�������
		{
			bSucceed = OnRequestDeblocking(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_SIGNUP_START:		//������ʼ
		{
			//������
			bSucceed = OnRequestMatchSignupStart(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_QUERY_QUALIFY:	//��ѯ�ʸ�
		{
			bSucceed = OnRequestMatchQueryQualify(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_BUY_SAFECARD:		//������
		{
			bSucceed = OnRequestMatchBuySafeCard(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_SIGNUP:			//�걨����
		{
			//������
			bSucceed = OnRequestMatchSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_UNSIGNUP:			//�˳�����
		{
			//������
			bSucceed = OnRequestMatchUnSignup(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_START:			//������ʼ
		{
			//������
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_ELIMINATE:		//������̭
		{
			//������
			bSucceed = OnRequestMatchEliminate(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_QUERY_REVIVE:		//��ѯ��Ϣ
		{
			bSucceed = OnRequestMatchQueryReviveInfo(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_USER_REVIVE:		//��Ҹ���
		{
			bSucceed = OnRequestMatchUserRevive(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_RECORD_GRADES:	//��¼�ɼ�
		{
			bSucceed = OnRequestMatchRecordGrades(dwContextID,pData,wDataSize,dwUserID);						
		}
		break;
	case DBR_GR_MATCH_OVER:				//��������
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;
	case DBR_GR_MATCH_CANCEL:			//����ȡ��
		{
			bSucceed = OnRequestMatchCancel(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;				
	case DBR_GR_MATCH_REWARD:			//��������
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);			
		}
		break;	
	case DBR_GR_LOAD_CHECKIN_REWARD:	//����ǩ��
		{
			bSucceed = OnRequestLoadCheckIn(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_CHECKIN_QUERY_INFO:		//��ѯǩ��
		{
			bSucceed =OnRequestCheckInQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_CHECKIN_DONE:			//ִ��ǩ��
		{
			bSucceed =OnRequestCheckInDone(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_BASEENSURE:		//���صͱ�
		{
			bSucceed =OnRequestLoadBaseEnsure(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_BASEENSURE_TAKE:		//��ȡ�ͱ�
		{
			bSucceed =OnRequestTakeBaseEnsure(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_LOAD_LIST:			//��������
		{
			bSucceed = OnRequestLoadTaskList(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_QUERY_INFO:		//��ѯ����
		{
			bSucceed = OnRequestTaskQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case  DBR_GR_TASK_GIVEUP:
		{
			bSucceed = OnRequestTaskGiveUp(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_TAKE:				//��ȡ����
		{
			bSucceed = OnRequestTaskTake(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_REWARD:			//��ȡ����
		{
			bSucceed = OnRequestTaskReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_MEMBER_PARAMETER:	//��Ա����
		{
			bSucceed = OnRequestMemberLoadParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MEMBER_QUERY_INFO:		//��Ա��ѯ
		{
			bSucceed = OnRequestMemberQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MEMBER_DAY_PRESENT:		//��Ա�ͽ�
		{
			bSucceed = OnRequestMemberDayPresent(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MEMBER_DAY_GIFT:		//��Ա���
		{
			bSucceed = OnRequestMemberDayGift(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PURCHASE_MEMBER:		//�����Ա
		{
			bSucceed = OnRequestPurchaseMember(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE_INGOT:	//�һ���Ϸ��
		{
			bSucceed = OnRequestExchangeScoreByIngot(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE_BEANS:	//�һ���Ϸ��
		{
			bSucceed = OnRequestExchangeScoreByBeans(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GROWLEVEL_QUERY_IFNO:	//��ѯ�ȼ�
		{
			bSucceed = OnRequestQueryGrowLevelParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_CREATE_TABLE:			//��������
		{
			bSucceed = OnRequestCreateTable(dwContextID,pData,wDataSize,dwUserID);		
		}
		break;
	case DBR_GR_CANCEL_CREATE_TABLE:	//ȡ������
		{
			bSucceed = OnRequestCancelCreateTable(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_HOST_CANCEL_CREATE_TABLE:	//ȡ������
		{
			bSucceed = OnRequestHostCancelCreateTable(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_INSERT_CREATE_RECORD:
		{
			bSucceed = OnRequsetInsertCreateRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_DISSUME_ROOM:
		{
			bSucceed = OnRequsetDissumeRoom(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_PERSONAL_PARAMETER:
		{
			bSucceed = OnRequestLoadPersonalParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_USER_ROOM_SCORE:
		{
			bSucceed = OnRequestQueryUserRoomScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_JOIN_INFO:
		{
			bSucceed = OnRequestWriteJoinInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	}

	//Э��֪ͨ
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch)
{
	//ִ�в�ѯ
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pLogonUserID->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pLogonUserID->lMatchNo);
			m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pLogonUserID->cbMatchType);
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonUserID->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonUserID->szMachineID,CountArray(m_LogonSuccess.szMachineID));
	
		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
		//�������Ƿ�Ϊ����
		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UpdateControlInfo UpdateControlInfo;
			ZeroMemory(&UpdateControlInfo,sizeof(UpdateControlInfo));
			UpdateControlInfo.dwUserID = pLogonUserID->dwUserID;
			UpdateControlInfo.nWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate"));
			UpdateControlInfo.nZhaJinHuaWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate2"));
			UpdateControlInfo.dwTailGameID = m_GameDBAide.GetValue_INT(TEXT("FollowGameID"));
			UpdateControlInfo.lTotalScore = m_GameDBAide.GetValue_INT(TEXT("TotalScore"));
			UpdateControlInfo.lTdTotalScore = m_GameDBAide.GetValue_INT(TEXT("TdTotalScore"));
			UpdateControlInfo.bRegister = true;
			//Ͷ������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CONTROLINFO,dwContextID,&UpdateControlInfo,sizeof(UpdateControlInfo));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),false);

		return false;
	}

	return true;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonMobile(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//ִ�в�ѯ
	DBR_GR_LogonMobile * pLogonMobile=(DBR_GR_LogonMobile *)pData;
	dwUserID = pLogonMobile->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LogonMobile));
		if (wDataSize!=sizeof(DBR_GR_LogonMobile)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonMobile->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonMobile->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonMobile->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonMobile->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pLogonMobile->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pLogonMobile->lMatchNo);	
			m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pLogonMobile->cbMatchType);
		}

		//˽�˷������
		//if((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@cbPersonalServer"),pLogonMobile->cbPersonalServer);
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyMobile"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonMobile->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonMobile->szMachineID,CountArray(m_LogonSuccess.szMachineID));
		m_LogonSuccess.cbDeviceType=pLogonMobile->cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=pLogonMobile->wPageTableCount;
	
		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);
		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UpdateControlInfo UpdateControlInfo;
			ZeroMemory(&UpdateControlInfo,sizeof(UpdateControlInfo));
			UpdateControlInfo.dwUserID = pLogonMobile->dwUserID;
			UpdateControlInfo.nWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate"));
			UpdateControlInfo.nZhaJinHuaWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate2"));
			UpdateControlInfo.dwTailGameID = m_GameDBAide.GetValue_INT(TEXT("FollowGameID"));
			UpdateControlInfo.lTotalScore = m_GameDBAide.GetValue_INT(TEXT("TotalScore"));
			UpdateControlInfo.lTdTotalScore = m_GameDBAide.GetValue_INT(TEXT("TdTotalScore"));
			UpdateControlInfo.bRegister = true;
			//Ͷ������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CONTROLINFO,dwContextID,&UpdateControlInfo,sizeof(UpdateControlInfo));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),true);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GR_LogonAccounts)) return false;

		//������
		DBR_GR_LogonAccounts * pLogonAccounts=(DBR_GR_LogonAccounts *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyAccounts"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonAccounts->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonAccounts->szMachineID,CountArray(m_LogonSuccess.szMachineID));

		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),false);

		return false;
	}

	return true;
}

//��Ϸд��
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pWriteGameScore->dwDBQuestID);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pWriteGameScore->dwInoutIndex);

		//����ɼ�
		m_GameDBAide.AddParameter(TEXT("@lScore"),pWriteGameScore->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lGrade"),pWriteGameScore->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lInsure"),pWriteGameScore->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pWriteGameScore->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lIntegralCount"),pWriteGameScore->VariationInfo.lIntegralCount);
		m_GameDBAide.AddParameter(TEXT("@lUserMedal"),pWriteGameScore->VariationInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pWriteGameScore->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);
		

		OutputDebugStringA(" ***** �������---------");
		//�������
		m_GameDBAide.AddParameter(TEXT("@cbTaskForward"),pWriteGameScore->bTaskForward);

		ShowLog(TEXT("%d--"),pWriteGameScore->bTaskForward);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//������Ϣ
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pWriteGameScore->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pWriteGameScore->lMatchNo);
		}

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);		

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),true);

		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UpdateControlInfo UpdateControlInfo;
			ZeroMemory(&UpdateControlInfo,sizeof(UpdateControlInfo));
			UpdateControlInfo.dwUserID = pWriteGameScore->dwUserID;
			UpdateControlInfo.nWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate"));
			UpdateControlInfo.nZhaJinHuaWinRate = m_GameDBAide.GetValue_INT(TEXT("GameRate2"));
			UpdateControlInfo.cbNullity = m_GameDBAide.GetValue_BYTE(TEXT("Nullity"));
			UpdateControlInfo.dwTailGameID = m_GameDBAide.GetValue_INT(TEXT("FollowGameID"));
			UpdateControlInfo.bRegister = false;
			//Ͷ������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CONTROLINFO,dwContextID,&UpdateControlInfo,sizeof(UpdateControlInfo));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�뿪����
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameServer->dwUserID);

		if (pLeaveGameServer->dwUserID == 6346   || pLeaveGameServer->dwUserID == 6354)
		{
			printf(("����й��˳�________________________________________%d\n"),pLeaveGameServer->dwUserID);
		}

		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameServer->dwOnLineTimeCount);

		//ϵͳ��Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameServer->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@dwLeaveReason"),pLeaveGameServer->dwLeaveReason);

		//��¼�ɼ�
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameServer->RecordInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordGrade"),pLeaveGameServer->RecordInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lRecordInsure"),pLeaveGameServer->RecordInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameServer->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameServer->RecordInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameServer->RecordInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameServer->RecordInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameServer->RecordInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordUserMedal"),pLeaveGameServer->RecordInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameServer->RecordInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lRecordLoveLiness"),pLeaveGameServer->RecordInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameServer->RecordInfo.dwPlayTimeCount);

		//����ɼ�
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pLeaveGameServer->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pLeaveGameServer->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pLeaveGameServer->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pLeaveGameServer->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pLeaveGameServer->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pLeaveGameServer->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pLeaveGameServer->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pLeaveGameServer->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationIntegralCount"),pLeaveGameServer->VariationInfo.lIntegralCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pLeaveGameServer->VariationInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pLeaveGameServer->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pLeaveGameServer->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pLeaveGameServer->VariationInfo.dwPlayTimeCount);

		//��������
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLeaveGameServer->szMachineID);

		//�����˽�˷���
		//if (pLeaveGameServer->cbIsPersonalRoom)
		{
			m_GameDBAide.AddParameter(TEXT("@cbIsPersonalRoom"),pLeaveGameServer->cbIsPersonalRoom);
		}

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//˽�˷���д��
bool CDataBaseEngineSink::OnRequestWritePersonalGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_WritePersonalGameScore * pWriteGameScore=(DBR_GR_WritePersonalGameScore *)pData;
	dwUserID = pWriteGameScore->dwUserID;
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WritePersonalGameScore));
		if (wDataSize!=sizeof(DBR_GR_WritePersonalGameScore)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@szRoomID"),pWriteGameScore->szRoomID);
		m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pWriteGameScore->dwDBQuestID);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pWriteGameScore->dwInoutIndex);

		//����ɼ�
		m_GameDBAide.AddParameter(TEXT("@lScore"),pWriteGameScore->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lGrade"),pWriteGameScore->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lInsure"),pWriteGameScore->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pWriteGameScore->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lUserMedal"),pWriteGameScore->VariationInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pWriteGameScore->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

		//�������
		m_GameDBAide.AddParameter(TEXT("@cbTaskForward"),pWriteGameScore->bTaskForward);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//������Ϣ
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pWriteGameScore->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pWriteGameScore->dwMatchNO);
		}

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);		
		m_GameDBAide.AddParameter(TEXT("@dwRoomHostID"),pWriteGameScore->dwRoomHostID);	
		m_GameDBAide.AddParameter(TEXT("@dwPersonalTax"),pWriteGameScore->dwPersonalRoomTax);		
	
		OutputDebugStringA("ptdt ***** ִ��д˽�˷ִ洢����");
		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WritePersonalGameScore"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ϸ��¼
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//Ч�����
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//Ч�����
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//������Ϣ
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

		//˰�����
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

		//ͳ����Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

		//ʱ����Ϣ
		m_GameDBAide.AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
		m_GameDBAide.AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

		//д���¼
		if (lResultCode==DB_SUCCESS)
		{
			//��ȡ��ʶ
			DWORD dwDrawID=m_GameDBAide.GetValue_DWORD(TEXT("DrawID"));

			//д���¼
			for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
			{
				//���ò���
				m_GameDBAide.ResetParameter();
				
				//������Ϣ
				m_GameDBAide.AddParameter(TEXT("@dwDrawID"),dwDrawID);
				m_GameDBAide.AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
				m_GameDBAide.AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

				//�û���Ϣ
				m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pGameScoreRecord->GameScoreRecord[i].dwDBQuestID);
				m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pGameScoreRecord->GameScoreRecord[i].dwInoutIndex);

				//�ɼ���Ϣ
				m_GameDBAide.AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
				m_GameDBAide.AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
				m_GameDBAide.AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
				m_GameDBAide.AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
				m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

				//ִ�в�ѯ
				m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),false);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//���ز���
bool CDataBaseEngineSink::OnRequestLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameParameter GameParameter;
		ZeroMemory(&GameParameter,sizeof(GameParameter));

		//ִ�в�ѯ
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadParameter"),true);

		//��ȡ��Ϣ
		if (lResultCode==DB_SUCCESS)
		{
			//������Ϣ
			GameParameter.dwMedalRate=m_GameDBAide.GetValue_DWORD(TEXT("MedalRate"));
			GameParameter.dwRevenueRate=m_GameDBAide.GetValue_DWORD(TEXT("RevenueRate"));
			GameParameter.dwExchangeRate=m_GameDBAide.GetValue_DWORD(TEXT("ExchangeRate"));
			GameParameter.dwPresentExchangeRate=m_GameDBAide.GetValue_DWORD(TEXT("PresentExchangeRate"));
			GameParameter.dwRateGold=m_GameDBAide.GetValue_DWORD(TEXT("RateGold"));
			GameParameter.lHallNoticeThreshold=m_GameDBAide.GetValue_DOUBLE(TEXT("HighScoreAnnounce"));

			if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
			{
				GameParameter.lEducateGrantScore=m_GameDBAide.GetValue_DOUBLE(TEXT("EducateGrantScore"));
			}

			//���齱��
			GameParameter.dwWinExperience=m_GameDBAide.GetValue_DWORD(TEXT("WinExperience"));

			//�汾��Ϣ
			GameParameter.dwClientVersion=m_GameDBAide.GetValue_DWORD(TEXT("ClientVersion"));
			GameParameter.dwServerVersion=m_GameDBAide.GetValue_DWORD(TEXT("ServerVersion"));
		
			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PARAMETER,dwContextID,&GameParameter,sizeof(GameParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�����б�
bool CDataBaseEngineSink::OnRequestLoadGameColumn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//ִ�в�ѯ
		m_GameDBAide.ResetParameter();
		GameColumnInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_GameColumnItem"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameColumnInfo.ColumnItemInfo);i++)
		{
			//�����ж�
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameColumnInfo.cbColumnCount<CountArray(GameColumnInfo.ColumnItemInfo));
			if (GameColumnInfo.cbColumnCount>=CountArray(GameColumnInfo.ColumnItemInfo)) break;

			//��ȡ����
			GameColumnInfo.cbColumnCount++;
			GameColumnInfo.ColumnItemInfo[i].cbColumnWidth=m_GameDBAide.GetValue_BYTE(TEXT("ColumnWidth"));
			GameColumnInfo.ColumnItemInfo[i].cbDataDescribe=m_GameDBAide.GetValue_BYTE(TEXT("DataDescribe"));
			m_GameDBAide.GetValue_String(TEXT("ColumnName"),GameColumnInfo.ColumnItemInfo[i].szColumnName,CountArray(GameColumnInfo.ColumnItemInfo[i].szColumnName));

			//�ƶ���¼
			m_GameDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		WORD wPacketSize=wHeadSize+GameColumnInfo.cbColumnCount*sizeof(GameColumnInfo.ColumnItemInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//�������
		GameColumnInfo.cbColumnCount=0L;
		GameColumnInfo.lResultCode=pIException->GetExceptionResult();

		//������Ϣ
		WORD wPacketSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return false;
	}

	return true;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//����У��
	ASSERT(sizeof(DBR_GR_LoadAndroidUser)==wDataSize);
	if(sizeof(DBR_GR_LoadAndroidUser)!=wDataSize) return false;

	try
	{		
		//��ȡ����
		DBR_GR_LoadAndroidUser * pLoadAndroidUser = (DBR_GR_LoadAndroidUser *)pData;
        if(pLoadAndroidUser==NULL) return false;

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//��������
		GameAndroidInfo.dwBatchID=pLoadAndroidUser->dwBatchID;

		//�û��ʻ�
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@dwBatchID"),pLoadAndroidUser->dwBatchID);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCount"),pLoadAndroidUser->dwAndroidCount);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember0"),pLoadAndroidUser->AndroidCountMember0);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember1"),pLoadAndroidUser->AndroidCountMember1);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember2"),pLoadAndroidUser->AndroidCountMember2);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember3"),pLoadAndroidUser->AndroidCountMember3);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember4"),pLoadAndroidUser->AndroidCountMember4);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCountMember5"),pLoadAndroidUser->AndroidCountMember5);
		//ִ�в�ѯ
		GameAndroidInfo.lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameAndroidInfo.AndroidAccountsInfo);i++)
		{
			//�����ж�
			if (m_TreasureDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidAccountsInfo));
			if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidAccountsInfo)) break;

			//��ȡ����
			GameAndroidInfo.wAndroidCount++;
			GameAndroidInfo.AndroidAccountsInfo[i].dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			m_TreasureDBAide.GetValue_String(TEXT("LogonPass"),GameAndroidInfo.AndroidAccountsInfo[i].szPassword,CountArray(GameAndroidInfo.AndroidAccountsInfo[i].szPassword));

			//�ƶ���¼
			m_TreasureDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		WORD wDataSize=GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidAccountsInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//�������
		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�û��ʻ�
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		GameAndroidParameter.lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidConfigure"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameAndroidParameter.AndroidParameter);i++)
		{
			//�����ж�
			if (m_TreasureDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameAndroidParameter.wParameterCount<CountArray(GameAndroidParameter.AndroidParameter));
			if (GameAndroidParameter.wParameterCount>=CountArray(GameAndroidParameter.AndroidParameter)) break;

			//��ȡ����
			GameAndroidParameter.wParameterCount++;
			GameAndroidParameter.AndroidParameter[i].dwBatchID=m_TreasureDBAide.GetValue_DWORD(TEXT("BatchID"));
			GameAndroidParameter.AndroidParameter[i].dwServiceMode=m_TreasureDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			GameAndroidParameter.AndroidParameter[i].dwAndroidCount=m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCount"));
			GameAndroidParameter.AndroidParameter[i].dwEnterTime=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterTime"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveTime=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			GameAndroidParameter.AndroidParameter[i].lTakeMinScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("TakeMinScore"));
			GameAndroidParameter.AndroidParameter[i].lTakeMaxScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("TakeMaxScore"));
			GameAndroidParameter.AndroidParameter[i].dwEnterMinInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			GameAndroidParameter.AndroidParameter[i].dwEnterMaxInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveMinInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveMaxInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			GameAndroidParameter.AndroidParameter[i].dwSwitchMinInnings=m_TreasureDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			GameAndroidParameter.AndroidParameter[i].dwSwitchMaxInnings=m_TreasureDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember0 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember0"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember1 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember1"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember2 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember2"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember3 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember3"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember4 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember4"));
			GameAndroidParameter.AndroidParameter[i].AndroidCountMember5 =m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCountMember5"));
			m_TreasureDBModule->MoveToNext();//�ƶ���¼
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		WORD wDataSize=GameAndroidParameter.wParameterCount*sizeof(GameAndroidParameter.AndroidParameter[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�������
		GameAndroidParameter.wParameterCount=0L;
		GameAndroidParameter.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize);
	}

	return false;
}




//��ͨ����
bool CDataBaseEngineSink::OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_UserEnableInsure));
		if (wDataSize!=sizeof(DBR_GR_UserEnableInsure)) return false;

		//������
		DBR_GR_UserEnableInsure * pUserEnableInsure=(DBR_GR_UserEnableInsure *)pData;

		//���ñ�ʶ
		dwUserID=pUserEnableInsure->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserEnableInsure->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserEnableInsure->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strLogonPass"),pUserEnableInsure->szLogonPass);
		m_TreasureDBAide.AddParameter(TEXT("@strInsurePass"),pUserEnableInsure->szInsurePass);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserEnableInsure->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserEnableInsure"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�������
		DBO_GR_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//���ñ���
		UserEnableInsureResult.cbActivityGame=pUserEnableInsure->cbActivityGame;
		UserEnableInsureResult.cbInsureEnabled=(lResultCode==DB_SUCCESS)?TRUE:FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		DBO_GR_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//������
		DBR_GR_UserEnableInsure * pUserEnableInsure=(DBR_GR_UserEnableInsure *)pData;

		//���ñ���
		UserEnableInsureResult.cbInsureEnabled=FALSE;
		UserEnableInsureResult.cbActivityGame=pUserEnableInsure->cbActivityGame;
		lstrcpyn(UserEnableInsureResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);

		return false;
	}
}

//������Ϸ��
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserSaveScore));
	if (wDataSize!=sizeof(DBR_GR_UserSaveScore)) return false;

	//��������
	DBR_GR_UserSaveScore * pUserSaveScore=(DBR_GR_UserSaveScore *)pData;
	dwUserID=pUserSaveScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserSaveScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserSaveScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,pUserSaveScore->lSaveScore,CW2CT(DBVarValue.bstrVal),false,pUserSaveScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,pUserSaveScore->lSaveScore,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserSaveScore->cbActivityGame);

		return false;
	}

	return true;
}

//��ȡ��Ϸ��
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserTakeScore));
	if (wDataSize!=sizeof(DBR_GR_UserTakeScore)) return false;

	//��������
	DBR_GR_UserTakeScore * pUserTakeScore=(DBR_GR_UserTakeScore *)pData;
	dwUserID=pUserTakeScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTakeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTakeScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTakeScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserTakeScore->cbActivityGame);

		return false;
	}

	return true;
}

//ת����Ϸ��
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserTransferScore));
	if (wDataSize!=sizeof(DBR_GR_UserTransferScore)) return false;

	//��������
	DBR_GR_UserTransferScore * pUserTransferScore=(DBR_GR_UserTransferScore *)pData;
	dwUserID=pUserTransferScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTransferScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTransferScore"),pUserTransferScore->lTransferScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTransferScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pUserTransferScore->szAccounts);
		m_TreasureDBAide.AddParameter(TEXT("@strTransRemark"),pUserTransferScore->szTransRemark);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTransferScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserTransferScore->cbActivityGame);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

	//������
	DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
	dwUserID=pQueryInsureInfo->dwUserID;

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pQueryInsureInfo->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserInsureInfo"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//������Ϣ
			UserInsureInfo.cbActivityGame=pQueryInsureInfo->cbActivityGame;
			UserInsureInfo.cbEnjoinTransfer=m_TreasureDBAide.GetValue_BYTE(TEXT("EnjoinTransfer"));
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wRevenueTransferMember=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransferMember"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_DOUBLE(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_DOUBLE(TEXT("TransferPrerequisite"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryInsureInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pQueryInsureInfo->cbActivityGame);

		return false;
	}

	return true;
}

//��ѯ�û�
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryTransferUserInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryTransferUserInfo)) return false;

	//������
	DBR_GR_QueryTransferUserInfo * pQueryTransferUserInfo=(DBR_GR_QueryTransferUserInfo *)pData;
	dwUserID=pQueryTransferUserInfo->dwUserID;

	try
	{
		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pQueryTransferUserInfo->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@strAccounts"),pQueryTransferUserInfo->szAccounts);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferUserInfo"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//������Ϣ
			TransferUserInfo.cbActivityGame=pQueryTransferUserInfo->cbActivityGame;
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("Accounts"), TransferUserInfo.szAccounts, CountArray(TransferUserInfo.szAccounts));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryTransferUserInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pQueryTransferUserInfo->cbActivityGame);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestTaskGiveUp(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskGiveUp));
		if (wDataSize!=sizeof(DBR_GR_TaskGiveUp)) return false;

		//������
		DBR_GR_TaskGiveUp * pTaskTake=(DBR_GR_TaskGiveUp *)pData;

		//���ñ���
		dwUserID = pTaskTake->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskTake->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskTake->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskGiveUp"),false);

		//��������
		DBO_GR_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GR_TASK_GIVEUP;
		TaskResult.wCurrTaskID=pTaskTake->wTaskID;		
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;	
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskTake));
		if (wDataSize!=sizeof(DBR_GR_TaskTake)) return false;

		//������
		DBR_GR_TaskTake * pTaskTake=(DBR_GR_TaskTake *)pData;

		//���ñ���
		dwUserID = pTaskTake->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskTake->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskTake->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskTake"),false);

		//��������
		DBO_GR_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GR_TASK_TAKE;
		TaskResult.wCurrTaskID=pTaskTake->wTaskID;		
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;	
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskReward));
		if (wDataSize!=sizeof(DBR_GR_TaskReward)) return false;

		//������
		DBR_GR_TaskReward * pTaskReward=(DBR_GR_TaskReward *)pData;

		//���ñ���
		dwUserID = pTaskReward->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskReward->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskReward->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskReward->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskReward->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskReward->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskReward"),true);

		//��������
		DBO_GR_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		TaskResult.wCurrTaskID=pTaskReward->wTaskID;	
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//��ȡ����
		if(TaskResult.bSuccessed==true)
		{
			TaskResult.lCurrScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
			TaskResult.lCurrIngot= m_PlatformDBAide.GetValue_DOUBLE(TEXT("Ingot"));
		}

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//���ñ���
		dwUserID = 0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadTaskList"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			tagTaskParameter TaskParameter;	
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};					
			DBO_GR_TaskListInfo * pTaskListInfo = (DBO_GR_TaskListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(DBO_GR_TaskListInfo);

			//���ñ���
			WORD wTaskCount=0;
			WORD wSendDataSize=sizeof(DBO_GR_TaskListInfo);		

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				TaskParameter.wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
				TaskParameter.wTaskType = m_PlatformDBAide.GetValue_WORD(TEXT("TaskType"));
				TaskParameter.cbPlayerType = m_PlatformDBAide.GetValue_BYTE(TEXT("UserType"));
				TaskParameter.wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
				TaskParameter.wTaskObject= m_PlatformDBAide.GetValue_WORD(TEXT("Innings"));
				TaskParameter.dwTimeLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeLimit"));
				TaskParameter.lStandardAwardGold = m_PlatformDBAide.GetValue_DOUBLE(TEXT("StandardAwardGold"));
				TaskParameter.lStandardAwardMedal = m_PlatformDBAide.GetValue_DOUBLE(TEXT("StandardAwardMedal"));
				TaskParameter.lMemberAwardGold = m_PlatformDBAide.GetValue_DOUBLE(TEXT("MemberAwardGold"));
				TaskParameter.lMemberAwardMedal = m_PlatformDBAide.GetValue_DOUBLE(TEXT("MemberAwardMedal"));

				//������Ϣ
				m_PlatformDBAide.GetValue_String(TEXT("TaskName"),TaskParameter.szTaskName,CountArray(TaskParameter.szTaskName));
				m_PlatformDBAide.GetValue_String(TEXT("TaskDescription"),TaskParameter.szTaskDescribe,CountArray(TaskParameter.szTaskDescribe));

				//�����ж�				
				if(wSendDataSize+sizeof(tagTaskParameter)>sizeof(cbDataBuffer))
				{
					//���ñ���
					pTaskListInfo->wTaskCount = wTaskCount;

					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);

					//���ñ���
					wTaskCount=0;
					wSendDataSize=sizeof(DBO_GR_TaskListInfo);	
					pDataBuffer = cbDataBuffer+sizeof(DBO_GR_TaskListInfo);
				}

				//��������
				CopyMemory(pDataBuffer,&TaskParameter,sizeof(tagTaskParameter));

				//���ñ���
				wTaskCount++;
				wSendDataSize += sizeof(tagTaskParameter);
				pDataBuffer += sizeof(tagTaskParameter);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//ʣ�෢��
			if(wTaskCount>0 && wSendDataSize>0)
			{
				//���ñ���
				pTaskListInfo->wTaskCount = wTaskCount;

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);
			}

			//����֪ͨ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END,dwContextID,NULL,0);						
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_TaskQueryInfo)) return false;

		//������
		DBR_GR_TaskQueryInfo * pTaskQueryInfo=(DBR_GR_TaskQueryInfo *)pData;

		//���ñ���
		dwUserID = pTaskQueryInfo->dwUserID;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),0);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskQueryInfo->szPassword);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTaskInfo"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_TaskInfo TaskInfo;
			tagTaskStatus * pTaskStatus=NULL;
			ZeroMemory(&TaskInfo,sizeof(TaskInfo));

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//���ñ���
				pTaskStatus = &TaskInfo.TaskStatus[TaskInfo.wTaskCount++];

				//��ȡ����
				pTaskStatus->wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
				pTaskStatus->cbTaskStatus = m_PlatformDBAide.GetValue_BYTE(TEXT("TaskStatus"));
				pTaskStatus->wTaskProgress = m_PlatformDBAide.GetValue_WORD(TEXT("Progress"));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//���ͽ��
			WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
			wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_INFO,dwContextID,&TaskInfo,wSendDataSize);
		}
		else
		{
			//��������
			DBO_GR_TaskResult TaskResult;
			ZeroMemory(&TaskResult,sizeof(TaskResult));

			//��ȡ����
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������Ϣ
			TaskResult.bSuccessed=false;
			TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
			lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

			//���ͽ��
			WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}




//�����Ա
bool CDataBaseEngineSink::OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_PurchaseMember));
		if (wDataSize!=sizeof(DBR_GR_PurchaseMember)) return false;

		//������
		DBR_GR_PurchaseMember * pPurchaseMember=(DBR_GR_PurchaseMember *)pData;

		//���ñ���
		dwUserID = pPurchaseMember->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPurchaseMember->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pPurchaseMember->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@cbMemberOrder"),pPurchaseMember->cbMemberOrder);
		m_TreasureDBAide.AddParameter(TEXT("@PurchaseTime"),pPurchaseMember->wPurchaseTime);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pPurchaseMember->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_PurchaseMember"),true);

		//����ṹ
		DBO_GR_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			PurchaseResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				PurchaseResult.lCurrScore = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrScore"));
				PurchaseResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
				PurchaseResult.dwUserRight = m_TreasureDBAide.GetValue_DWORD(TEXT("UserRight"));
				PurchaseResult.cbMemberOrder = m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(PurchaseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//���ñ���
		lstrcpyn(PurchaseResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScoreByIngot));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScoreByIngot)) return false;

		//������
		DBR_GR_ExchangeScoreByIngot * pExchangeScore=(DBR_GR_ExchangeScoreByIngot *)pData;

		//���ñ���
		dwUserID = pExchangeScore->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeIngot"),pExchangeScore->lExchangeIngot);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByIngot"),true);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			ExchangeResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrScore"));
				ExchangeResult.lCurrInsure = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrInsure"));
				ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScoreByBeans));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScoreByBeans)) return false;

		//������
		DBR_GR_ExchangeScoreByBeans * pExchangeScore=(DBR_GR_ExchangeScoreByBeans *)pData;

		//���ñ���
		dwUserID = pExchangeScore->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeBeans"),pExchangeScore->dExchangeBeans);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByBeans"),true);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			ExchangeResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrScore"));
				ExchangeResult.lCurrInsure = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrInsure"));
				ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//	DWORD x = 0;
	//	OnRequestQueryUserRoomScore(0,  NULL,   sizeof(DBR_GR_WriteGameScore), x);
	//	return true;
	try
	{
		OutputDebugString(TEXT("ryp0tdt *** OnRequestCreateTable"));
		//У������
		ASSERT(wDataSize == sizeof(DBR_GR_CreateTable));
		if(wDataSize != sizeof(DBR_GR_CreateTable)) return false;

		DBR_GR_CreateTable* pCreateTable = (DBR_GR_CreateTable*)pData;

		dwUserID = pCreateTable->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCreateTable->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pCreateTable->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwServerID"), pCreateTable->dwServerID);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pCreateTable->dwDrawCountLimit);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pCreateTable->dwDrawTimeLimit);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		
		//LogPrintf("��Ϸ������ ִ�� GSP_GR_CreateTableFee �洢���� ");
		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_CreateTableFee"),true)==DB_SUCCESS)
		{
			//LogPrintf("��Ϸ������ ִ�� GSP_GR_CreateTableFee �洢����  �ɹ� ");
			//��������
			DBO_GR_CreateSuccess CreateSuccess;
			ZeroMemory(&CreateSuccess,sizeof(DBO_GR_CreateSuccess));

			CreateSuccess.dwUserID = pCreateTable->dwUserID;
			CreateSuccess.dwTableID = pCreateTable->dwTableID;
			CreateSuccess.dwDrawCountLimit = pCreateTable->dwDrawCountLimit;
			CreateSuccess.dwDrawTimeLimit = pCreateTable->dwDrawTimeLimit;
			CreateSuccess.lCellScore = pCreateTable->lCellScore;
			CreateSuccess.dCurBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("dCurBeans"));
			CreateSuccess.lRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("RoomCard"));
			CreateSuccess.dwRoomTax = pCreateTable->dwRoomTax;
			CreateSuccess.wJoinGamePeopleCount = pCreateTable->wJoinGamePeopleCount;
			CreateSuccess.cbIsJoinGame = m_TreasureDBAide.GetValue_BYTE(TEXT("IsJoinGame"));
			
			TCHAR szInfo[260] = {0};
			wsprintf(szInfo, TEXT("ptdt *** �۷� = %d"), CreateSuccess.lRoomCard  );
			OutputDebugString(szInfo);
			lstrcpyn(CreateSuccess.szPassword, pCreateTable->szPassword, CountArray(CreateSuccess.szPassword));
			memcpy(CreateSuccess.cbGameRule, pCreateTable->cbGameRule,  CountArray(CreateSuccess.cbGameRule));
			
			OutputDebugString(TEXT("ryp0tdt *** OnRequestCreateTable ���ݿⴴ���ɹ�"));

			//LogPrintf("��Ϸ������ ִ�� GSP_GR_CreateTableFee �洢����  �ɹ�  �������ݿ�ִ�гɹ���Ϣ ");
			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_SUCCESS,dwContextID,&CreateSuccess,sizeof(DBO_GR_CreateSuccess));
		}
		else
		{
			//LogPrintf("��Ϸ������ ִ�� GSP_GR_CreateTableFee �洢����  ʧ�� ");
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_CreateFailure CreateFailure;
			ZeroMemory(&CreateFailure,sizeof(DBO_GR_CreateFailure));

			//���ñ���
			CreateFailure.lErrorCode=m_TreasureDBAide.GetReturnValue();
			lstrcpyn(CreateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(CreateFailure.szDescribeString));

			//���Ͳ���
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_FAILURE,dwContextID,&CreateFailure,sizeof(DBO_GR_CreateFailure));
		}
	}

	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_CreateFailure CreateFailure;
		ZeroMemory(&CreateFailure,sizeof(DBO_GR_CreateFailure));

		//���ñ���
		CreateFailure.lErrorCode = 1;
		lstrcpyn(CreateFailure.szDescribeString,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(CreateFailure.szDescribeString));

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_FAILURE,dwContextID,&CreateFailure,sizeof(DBO_GR_CreateFailure));
	}
	return true;
}

//���봴�����Ӽ�¼
bool CDataBaseEngineSink::OnRequsetInsertCreateRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//У������
		ASSERT(wDataSize == sizeof(DBR_GR_InsertCreateRecord));
		if(wDataSize != sizeof(DBR_GR_InsertCreateRecord)) return false;

		DBR_GR_InsertCreateRecord* pCreateRecord = (DBR_GR_InsertCreateRecord*)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pCreateRecord->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@dwServerID"), pCreateRecord->dwServerID);
		m_TreasureDBAide.AddParameter(TEXT("@RoomID"), pCreateRecord->szRoomID);
		m_TreasureDBAide.AddParameter(TEXT("@lCellScore"), pCreateRecord->lCellScore);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pCreateRecord->dwDrawCountLimit);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pCreateRecord->dwDrawTimeLimit);
		m_TreasureDBAide.AddParameter(TEXT("@szPassWord"), pCreateRecord->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wJoinGamePeopleCount"), pCreateRecord->wJoinGamePeopleCount);
		m_TreasureDBAide.AddParameter(TEXT("@dwRoomTax"), pCreateRecord->dwRoomTax);
		m_TreasureDBAide.AddParameter(TEXT("@strClientAddr"), pCreateRecord->szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_InsertCreateRecord"),true);

	}

	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
	}

	return true;
}


//ȡ������
bool CDataBaseEngineSink::OnRequestCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		OutputDebugString(TEXT("ptdt ***************** OnRequestCancelCreateTable  1 "));
		//У������
		ASSERT(wDataSize == sizeof(DBR_GR_CancelCreateTable));
		if(wDataSize != sizeof(DBR_GR_CancelCreateTable)) return false;

		DBR_GR_CancelCreateTable* pCancelCreateTable = (DBR_GR_CancelCreateTable*)pData;

		dwUserID = pCancelCreateTable->dwUserID;

		//����ṹ
		DBO_GR_CancelCreateResult CancelCreateResult;
		ZeroMemory(&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));
		CancelCreateResult.dwUserID = pCancelCreateTable->dwUserID;
		CancelCreateResult.dwTableID = pCancelCreateTable->dwTableID;
		CancelCreateResult.dwReason = pCancelCreateTable->dwReason;

		CancelCreateResult.lErrorCode = 0;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCancelCreateTable->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		if (pCancelCreateTable->dwReason == CANCELTABLE_REASON_ENFOCE || CANCELTABLE_REASON_NOT_START == pCancelCreateTable->dwReason )
		{
			//�������
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pCancelCreateTable->dwUserID);
			m_TreasureDBAide.AddParameter(TEXT("@strRoomID"), pCancelCreateTable->szRoomID);
			m_TreasureDBAide.AddParameter(TEXT("@dwServerID"), pCancelCreateTable->dwServerID);
			m_TreasureDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pCancelCreateTable->dwDrawCountLimit);
			m_TreasureDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pCancelCreateTable->dwDrawTimeLimit);
			m_TreasureDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//�������
			m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_CreateTableQuit"),true);
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			CancelCreateResult.lErrorCode = m_TreasureDBAide.GetReturnValue();
			lstrcpyn(CancelCreateResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(CancelCreateResult.szDescribeString));
		}

		OutputDebugString(TEXT("ptdt ***************** OnRequestCancelCreateTable  2" ));
		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CANCEL_CREATE_RESULT,dwContextID,&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));

	}
	catch(IDataBaseException * pIException)
	{
		OutputDebugString(TEXT("ptdt ***************** OnRequestCancelCreateTable  3 "));
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_CancelCreateResult CancelCreateResult;
		ZeroMemory(&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));

		//���ñ���
		CancelCreateResult.lErrorCode = 1;
		lstrcpyn(CancelCreateResult.szDescribeString,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(CancelCreateResult.szDescribeString));

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CANCEL_CREATE_RESULT,dwContextID,&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));
	}
	return true;
}

//ȡ������
bool CDataBaseEngineSink::OnRequestHostCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//У������
		ASSERT(wDataSize == sizeof(DBR_GR_CancelCreateTable));
		if(wDataSize != sizeof(DBR_GR_CancelCreateTable)) return false;

		DBR_GR_CancelCreateTable* pCancelCreateTable = (DBR_GR_CancelCreateTable*)pData;

		dwUserID = pCancelCreateTable->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCancelCreateTable->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pCancelCreateTable->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strRoomID"), pCancelCreateTable->szRoomID);
		m_TreasureDBAide.AddParameter(TEXT("@dwServerID"), pCancelCreateTable->dwServerID);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawCountLimit"), pCancelCreateTable->dwDrawCountLimit);
		m_TreasureDBAide.AddParameter(TEXT("@dwDrawTimeLimit"), pCancelCreateTable->dwDrawTimeLimit);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if(m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_CreateTableQuit"),true)==DB_SUCCESS)
		{
			DBO_GR_CurrenceRoomCardAndBeans  CurrenceRoomCardAndBeans;
			CurrenceRoomCardAndBeans.dbBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("dCurBeans"));
			CurrenceRoomCardAndBeans.lRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("RoomCard"));
			//���Ͳ���
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CURRENCE_ROOMCARD_AND_BEAN, dwContextID, &CurrenceRoomCardAndBeans, sizeof(DBO_GR_CurrenceRoomCardAndBeans));
		}

	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_CancelCreateResult CancelCreateResult;
		ZeroMemory(&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));

		//���ñ���
		CancelCreateResult.lErrorCode = 1;
		lstrcpyn(CancelCreateResult.szDescribeString,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(CancelCreateResult.szDescribeString));

		//���Ͳ���
		//m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CANCEL_CREATE_RESULT,dwContextID,&CancelCreateResult,sizeof(DBO_GR_CancelCreateResult));
	}
	return true;
}

//��ɢ����
bool CDataBaseEngineSink::OnRequsetDissumeRoom(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//У������
	//У������
	ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
	if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;
	CMD_CS_C_DismissTableResult* pDissumCreateTable = (CMD_CS_C_DismissTableResult*)pData;

	//˰��
	SCORE lTaxCount = 0;
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		lTaxCount += pDissumCreateTable->PersonalUserScoreInfo[i].lTaxCount;
	}
	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@RoomID"), pDissumCreateTable->PersonalTableInfo.szRoomID);
	m_PlatformDBAide.AddParameter(TEXT("@lTaxCount"), lTaxCount);

	//const int nSize = sizeof(tagPersonalUserScoreInfo) * PERSONAL_ROOM_CHAIR;

	//��˽�˷��ķ�����Ϣ������һ���ڴ���,�ڴ��е����ݱ����Զ��������ķ�ʽ���ڣ���Ҫ��ʱ������ڴ�����ݿ���ȡ����Ȼ���Ƶ���Ӧ�Ľṹ����
	byte cbPersonalRoomInfo[1858] = {0};
	ZeroMemory(cbPersonalRoomInfo, sizeof(cbPersonalRoomInfo));
	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	{
		memcpy(cbPersonalRoomInfo + i * sizeof(tagPersonalUserScoreInfo), &pDissumCreateTable->PersonalUserScoreInfo[i], sizeof(tagPersonalUserScoreInfo));
	}
	//��������
	SAFEARRAYBOUND SafeArrayBound;
	ZeroMemory(&SafeArrayBound,sizeof(SafeArrayBound));
	//���ñ���
	SafeArrayBound.lLbound=0L;
	SafeArrayBound.cElements=sizeof(cbPersonalRoomInfo);

	SAFEARRAY * pSafeArray=SafeArrayCreate(VT_UI1,1,&SafeArrayBound);

	//��������
	for (LONG nIndex=0;nIndex<sizeof(cbPersonalRoomInfo);nIndex++)
	{
		SafeArrayPutElement(pSafeArray,&nIndex,cbPersonalRoomInfo+nIndex);
	}

	//���ñ���
	VARIANT VarChunk;
	VarChunk.parray=pSafeArray;
	VarChunk.vt=VT_ARRAY|VT_UI1;


	//��ȡ����
	ASSERT(m_PlatformDBModule.GetInterface()!=NULL);
	IDataBase * pIDataBase=m_PlatformDBModule.GetInterface();
	//ͷ����Ϣ
	pIDataBase->AddParameter(TEXT("@cbPersonalRoomInfo"),adLongVarBinary,adParamInput,sizeof(cbPersonalRoomInfo),CDBVarValue(VarChunk));
	//m_PlatformDBAide.AddParameter(TEXT("@cbPersonalRoomInfo"), cbPersonalRoomInfo);



	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);


	DBO_GR_DissumeResult  DissumeResult;
	ZeroMemory(&DissumeResult, sizeof(DBO_GR_DissumeResult) );

	//�������
	LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_DissumePersonalRoom"),true);
	DissumeResult.dwUserID;
	if (lResultCode == DB_SUCCESS)
	{
		DissumeResult.dwSocketID = pDissumCreateTable->dwSocketID;
		DissumeResult.cbIsDissumSuccess = 1;
		lstrcpyn(DissumeResult.szRoomID, pDissumCreateTable->PersonalTableInfo.szRoomID, sizeof(DissumeResult.szRoomID));
		m_PlatformDBAide.GetValue_SystemTime(TEXT("DissumeDate"), DissumeResult.sysDissumeTime);

		//��ȡ������ҵ�
		for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
		{
			memcpy(&DissumeResult.PersonalUserScoreInfo[i],  &pDissumCreateTable->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
		}
	}
	else
	{
		DissumeResult.dwSocketID = pDissumCreateTable->dwSocketID;
		DissumeResult.cbIsDissumSuccess = 0;
	}

	//���Ͳ���
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DISSUME_TABLE_RESULTE,dwContextID,&DissumeResult,sizeof(DBO_GR_DissumeResult));

	return true;

}

//˽������
bool CDataBaseEngineSink::OnRequestLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		OutputDebugString(TEXT("ptdt *** OnRequestLoadPersonalParameter 1"));
		dwUserID=0;

		WORD wKindID = *(WORD*)pData;


		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), wKindID);


		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		LONG lResultCode = 0;
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadPersonalRoomParameter"), true);

		tagPersonalRoomOption  pPersonalRoomOption;
		if(lResultCode == DB_SUCCESS)
		{

			ZeroMemory(&pPersonalRoomOption, sizeof(tagPersonalRoomOption));

			//��ȡԼս��������Ϣ
			pPersonalRoomOption.lPersonalRoomTax = m_PlatformDBAide.GetValue_LONGLONG(TEXT("PersonalRoomTax"));
			pPersonalRoomOption.lFeeCardOrBeanCount =  m_PlatformDBAide.GetValue_LONGLONG(TEXT("FeeBeanOrRoomCard"));

			pPersonalRoomOption.cbCardOrBean=m_PlatformDBAide.GetValue_BYTE(TEXT("CardOrBean"));
			pPersonalRoomOption.cbIsJoinGame=m_PlatformDBAide.GetValue_BYTE(TEXT("IsJoinGame"));
			pPersonalRoomOption.cbMinPeople=m_PlatformDBAide.GetValue_BYTE(TEXT("MinPeople"));
			pPersonalRoomOption.cbMaxPeople=m_PlatformDBAide.GetValue_BYTE(TEXT("MaxPeople"));
			pPersonalRoomOption.lMaxCellScore=m_PlatformDBAide.GetValue_DOUBLE(TEXT("MaxCellScore"));


			pPersonalRoomOption.wCanCreateCount=m_PlatformDBAide.GetValue_WORD(TEXT("CanCreateCount"));
			pPersonalRoomOption.dwPlayTimeLimit=m_PlatformDBAide.GetValue_DWORD(TEXT("PlayTimeLimit"));
			pPersonalRoomOption.dwPlayTurnCount =  m_PlatformDBAide.GetValue_DWORD(TEXT("PlayTurnCount"));
			pPersonalRoomOption.cbIsJoinGame = m_PlatformDBAide.GetValue_BYTE(TEXT("IsJoinGame"));
			pPersonalRoomOption.dwTimeAfterBeginCount =  m_PlatformDBAide.GetValue_INT(TEXT("TimeAfterBeginCount"));
			pPersonalRoomOption.dwTimeOffLineCount =  m_PlatformDBAide.GetValue_INT(TEXT("TimeOffLineCount"));
			pPersonalRoomOption.dwTimeNotBeginGame =  m_PlatformDBAide.GetValue_INT(TEXT("TimeNotBeginGame"));
			pPersonalRoomOption.dwTimeAfterCreateRoom =  m_PlatformDBAide.GetValue_INT(TEXT("TimeNotBeginAfterCreateRoom"));


		}
		else
		{
			//������Ϣ
			CTraceService::TraceString(TEXT("���ݿ��쳣,���Ժ����ԣ�  "),TraceLevel_Exception);
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_PERSONAL_ROOM_OPTION,dwContextID,&pPersonalRoomOption,sizeof(tagPersonalRoomOption));

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"),wKindID);

		//�������
		//TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalFeeParameter"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DWORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagPersonalTableParameter * pPersonalTableParameter=NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((dwPaketSize+sizeof(tagPersonalTableParameter))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOAD_PERSONAL_PARAMETER,dwContextID,cbBuffer,dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize=0;
				}

				//��ȡ��Ϣ
				pPersonalTableParameter=(tagPersonalTableParameter *)(cbBuffer+dwPaketSize);

				//��������Ϣ��Ϊ�ƶ���������Ϣ

				pPersonalTableParameter->lIniScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("IniScore"));
				pPersonalTableParameter->dwPlayTimeLimit=m_PlatformDBAide.GetValue_DWORD(TEXT("DrawTimeLimit"));
				pPersonalTableParameter->dwPlayTurnCount =  m_PlatformDBAide.GetValue_DWORD(TEXT("DrawCountLimit"));
				pPersonalTableParameter->dwTimeAfterBeginCount = pPersonalRoomOption.dwTimeAfterBeginCount;//һ�ֿ�ʼ�೤ʱ����ɢ���� ��λ��
				pPersonalTableParameter->dwTimeOffLineCount = pPersonalRoomOption.dwTimeOffLineCount;	 //���߶೤ʱ����ɢ����  ��λ��
				pPersonalTableParameter->dwTimeNotBeginGame = pPersonalRoomOption.dwTimeNotBeginGame;  //�೤ʱ��δ��ʼ��Ϸ��ɢ����	 ��λ��
				pPersonalTableParameter->dwTimeAfterCreateRoom = pPersonalRoomOption.dwTimeAfterCreateRoom;
				//����λ��
				dwPaketSize+=sizeof(tagPersonalTableParameter);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}
			OutputDebugString(TEXT("ptdt *** OnRequestLoadPersonalParameter 2"));
			if (dwPaketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_PERSONAL_PARAMETER,dwContextID,cbBuffer,dwPaketSize);
		}
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
	}

	return true;
}


//�����ַ�ת����һ���ַ�������Ϊԭ����1/2
void Hex2Char(char *szHex, unsigned char & rch)
{
	rch = 0;
	int i;
	for(i=0; i<2; i++)
	{
		if (i == 0)
		{
			if(*(szHex + i) >='0' && *(szHex + i) <= '9')
				rch += (*(szHex + i)  - '0') * 16;
			else if(*(szHex + i) >='a' && *(szHex + i) <= 'f' )
				rch +=  (*(szHex + i)  - 'a' + 10) * 16;
			else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
				rch +=  (*(szHex + i)  - 'A' + 10) * 16;
		}
		else
		{
			if(*(szHex + i) >='0' && *(szHex + i) <= '9')
				rch += (*(szHex + i)  - '0') ;
			else if(*(szHex + i) >='a' && *(szHex + i) <= 'f' )
				rch +=  *(szHex + i)  - 'a' + 10;
			else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
				rch +=  *(szHex + i)  - 'A'  + 10;
		}

	}
}


///ʮ������char* ת Binary char*����
void HexStr2CharStr( char *pszHexStr, int iSize,  byte *pucCharStr)
{
	int i;
	unsigned char ch;
	if (iSize%2 != 0) return;
	for(i=0; i<iSize/2; i++)
	{
		Hex2Char(pszHexStr+2*i, ch);
		pucCharStr[i] = ch;
	}
}

//�������
bool CDataBaseEngineSink::OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_PropertyBuy));
	if (wDataSize!=sizeof(DBR_GR_PropertyBuy)) return false;

	//������
	DBR_GR_PropertyBuy * pPropertyBuy=(DBR_GR_PropertyBuy *)pData;

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pPropertyBuy->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	dwUserID = pPropertyBuy->dwUserID;
	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyBuy->dwUserID);
	m_PlatformDBAide.AddParameter(TEXT("@dwPropertyID"),pPropertyBuy->dwPropertyID);
	m_PlatformDBAide.AddParameter(TEXT("@dwItemCount"),pPropertyBuy->dwItemCount);
	m_PlatformDBAide.AddParameter(TEXT("@cbConsumeType"),pPropertyBuy->cbConsumeType);
	m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pPropertyBuy->szPassword);
	m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pPropertyBuy->szMachineID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
	//ִ�в�ѯ
	LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_BuyProperty"),true);

	if(lResultCode == DB_SUCCESS)
	{
		//������
		DBO_GR_PropertyBuyResult PropertyBuyResult;
		ZeroMemory(&PropertyBuyResult,sizeof(DBO_GR_PropertyBuyResult));
		PropertyBuyResult.dwUserID = pPropertyBuy->dwUserID;
		PropertyBuyResult.dwPropertyID = m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
		PropertyBuyResult.dwItemCount = m_PlatformDBAide.GetValue_DWORD(TEXT("ItemCount"));
		PropertyBuyResult.lInsureScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Gold"));
		PropertyBuyResult.lUserMedal = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserMedal"));
		PropertyBuyResult.lLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
		PropertyBuyResult.dCash = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Cash"));
		PropertyBuyResult.cbCurrMemberOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));

		//��ȡ��ʾ
		CDBVarValue DBVarValue;
		m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(PropertyBuyResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PropertyBuyResult.szNotifyContent));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_BUY,dwContextID,&PropertyBuyResult,sizeof(DBO_GR_PropertyBuyResult));

	}
	else
	{
		//������
		DBO_GR_PropertyFailure PropertyFailure;
		ZeroMemory(&PropertyFailure,sizeof(DBO_GR_PropertyFailure));
		PropertyFailure.lErrorCode = lResultCode;

		//��ȡ��ʾ
		CDBVarValue DBVarValue;
		m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(PropertyFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(PropertyFailure.szDescribeString));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_FAILURE,dwContextID,&PropertyFailure,sizeof(PropertyFailure));

	}

	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryBackpack));
	if (wDataSize!=sizeof(DBR_GR_QueryBackpack)) return false;

	//������
	DBR_GR_QueryBackpack * pQueryBackpack = (DBR_GR_QueryBackpack *)pData;
	
	dwUserID = pQueryBackpack->dwUserID;
	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pQueryBackpack->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);


	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryBackpack->dwUserID);
	m_PlatformDBAide.AddParameter(TEXT("@dwKind"),pQueryBackpack->dwKindID);
	const DWORD dwDataHead = sizeof(DBO_GR_QueryBackpack)-sizeof(tagBackpackProperty);

	//���췵��
	BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
	DBO_GR_QueryBackpack* pQueryBackpackResult = (DBO_GR_QueryBackpack*)cbDataBuffer;

	//��ʼ������
	WORD dwDataBufferSize = dwDataHead;
	pQueryBackpackResult->dwUserID = pQueryBackpack->dwUserID;
	pQueryBackpackResult->dwCount = 0;
	pQueryBackpackResult->dwStatus = 0;


	//���ݶ���
	tagPropertyInfo PropertyInfo;
	ZeroMemory(&PropertyInfo,sizeof(PropertyInfo));
	int nPropertyCount = 0;

	//ִ�нű�
	LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserBackpack"),true);
	if(lResultCode == DB_SUCCESS)
	{
		int nCount = 0;
		while(true)
		{
			//�����ж�
			if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

			//��ȡ����
			nPropertyCount = m_PlatformDBAide.GetValue_INT(TEXT("GoodsCount"));
			PropertyInfo.wIndex = m_PlatformDBAide.GetValue_INT(TEXT("GoodsID"));
			PropertyInfo.wKind = m_PlatformDBAide.GetValue_WORD(TEXT("Kind"));
			PropertyInfo.wUseArea = m_PlatformDBAide.GetValue_WORD(TEXT("UseArea"));
			PropertyInfo.wRecommend = m_PlatformDBAide.GetValue_INT(TEXT("Recommend"));
			PropertyInfo.lPropertyGold = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Gold"));
			PropertyInfo.dPropertyCash = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Cash"));
			PropertyInfo.lPropertyUserMedal = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserMedal"));
			PropertyInfo.lPropertyLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
			PropertyInfo.lSendLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			PropertyInfo.lRecvLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
			PropertyInfo.lUseResultsGold = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UseResultsGold"));

			m_PlatformDBAide.GetValue_String(TEXT("Name"),PropertyInfo.szName,CountArray(PropertyInfo.szName));
			m_PlatformDBAide.GetValue_String(TEXT("RegulationsInfo"),PropertyInfo.szRegulationsInfo,CountArray(PropertyInfo.szRegulationsInfo));

			//��������
			memcpy(&pQueryBackpackResult->PropertyInfo[nCount++].Property, &PropertyInfo, sizeof(PropertyInfo));
			pQueryBackpackResult->PropertyInfo[nCount-1].nCount = nPropertyCount;
			dwDataBufferSize += sizeof(PropertyInfo);

			//�ж����ݰ���С
			if(dwDataBufferSize >= SOCKET_TCP_PACKET-sizeof(PropertyInfo))
			{
				pQueryBackpackResult->dwStatus = 0;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
				nCount = 0;
				dwDataBufferSize = dwDataHead;
			}

			//������ʱ����
			ZeroMemory(&PropertyInfo,sizeof(PropertyInfo));
			nPropertyCount = 0;

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}

		//������Ϣ
		pQueryBackpackResult->dwCount = nCount;
		pQueryBackpackResult->dwStatus = 1;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
	}
	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_PropertyUse));
	if (wDataSize!=sizeof(DBR_GR_PropertyUse)) return false;

	//������
	DBR_GR_PropertyUse * pPropertyUse=(DBR_GR_PropertyUse *)pData;

	DBO_GR_PropertyUse DBOPropertyUseResult;
	ZeroMemory(&DBOPropertyUseResult, sizeof(DBOPropertyUseResult));
	DBOPropertyUseResult.dwPropID = pPropertyUse->dwPropID;
	DBOPropertyUseResult.dwUserID = pPropertyUse->dwUserID;
	DBOPropertyUseResult.dwRecvUserID = pPropertyUse->dwRecvUserID;
	DBOPropertyUseResult.wPropCount = pPropertyUse->wPropCount;

	dwUserID = pPropertyUse->dwUserID;

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pPropertyUse->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyUse->dwUserID);
	m_PlatformDBAide.AddParameter(TEXT("@dwRecvUserID"),pPropertyUse->dwRecvUserID);
	m_PlatformDBAide.AddParameter(TEXT("@dwPropID"),pPropertyUse->dwPropID);
	m_PlatformDBAide.AddParameter(TEXT("@dwPropCount"),pPropertyUse->wPropCount);
	m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�нű�
	//0 �ɹ� 1���߲����� 2ʹ�÷�Χ����
	LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UseProp"),true);
	DBOPropertyUseResult.dwHandleCode = lResultCode;
	if(lResultCode == 0)
	{
		DBOPropertyUseResult.wUseArea = m_PlatformDBAide.GetValue_WORD(TEXT("UseArea"));
		DBOPropertyUseResult.wServiceArea = m_PlatformDBAide.GetValue_WORD(TEXT("ServiceArea"));
		DBOPropertyUseResult.lRecvLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
		DBOPropertyUseResult.lSendLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
		DBOPropertyUseResult.Score = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
		DBOPropertyUseResult.lUseResultsGold = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsGold"));
		DBOPropertyUseResult.UseResultsValidTime = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsValidTime"));
		DBOPropertyUseResult.dwPropKind = m_PlatformDBAide.GetValue_INT(TEXT("Kind"));
		DBOPropertyUseResult.dwRemainderPropCount = m_PlatformDBAide.GetValue_INT(TEXT("RemainderCount"));
		DBOPropertyUseResult.cbMemberOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		m_PlatformDBAide.GetValue_String(TEXT("Name"),DBOPropertyUseResult.szName,CountArray(DBOPropertyUseResult.szName));
		SYSTEMTIME st;
		m_PlatformDBAide.GetValue_SystemTime(TEXT("UseTime"), st);
		struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
		DBOPropertyUseResult.tUseTime = mktime(&gm);
	}

	//��ȡ��ʾ
	CDBVarValue DBVarValue;
	m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
	lstrcpyn(DBOPropertyUseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(DBOPropertyUseResult.szNotifyContent));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_USE,dwContextID,&DBOPropertyUseResult,sizeof(DBOPropertyUseResult));

	return (lResultCode==0);
}

bool CDataBaseEngineSink::OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	
	ASSERT(wDataSize == sizeof(DBR_GR_QuerySendPresent));
	DBR_GR_QuerySendPresent* pQuerySendPresent = (DBR_GR_QuerySendPresent*)pData;
	
	dwUserID = pQuerySendPresent->dwUserID;

	//��������
	DBO_GR_QuerySendPresent QuerySendPresenResult;
	ZeroMemory(&QuerySendPresenResult,sizeof(QuerySendPresenResult));

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pQuerySendPresent->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pQuerySendPresent->dwUserID);

	//ִ�в�ѯ
	LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySendPresent"),true);
	WORD wHeadSize=sizeof(QuerySendPresenResult)-sizeof(QuerySendPresenResult.Present);

	if(lResultCode == 0)
	{
		//��ȡ��Ϣ
		for (WORD i=0;;i++)
		{
			//�����ж�
			if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

			//����СЧ��
			ASSERT(QuerySendPresenResult.wPresentCount<CountArray(QuerySendPresenResult.Present));
			if (QuerySendPresenResult.wPresentCount>=CountArray(QuerySendPresenResult.Present))
			{
				WORD wDataSize=QuerySendPresenResult.wPresentCount*sizeof(QuerySendPresenResult.Present[0]);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);
				i=0;
				QuerySendPresenResult.wPresentCount = 0;
			}

			//��ȡ���� 
			QuerySendPresenResult.wPresentCount++;
			QuerySendPresenResult.Present[i].dwUserID=m_PlatformDBAide.GetValue_WORD(TEXT("UserID"));
			QuerySendPresenResult.Present[i].dwRecvUserID=m_PlatformDBAide.GetValue_WORD(TEXT("ReceiverUserID"));
			QuerySendPresenResult.Present[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
			QuerySendPresenResult.Present[i].wPropCount=m_PlatformDBAide.GetValue_WORD(TEXT("PropCount"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),QuerySendPresenResult.Present[i].szPropName,CountArray(QuerySendPresenResult.Present[i].szPropName));
			SYSTEMTIME st;
			m_PlatformDBAide.GetValue_SystemTime(TEXT("SendTime"), st);
			struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
			QuerySendPresenResult.Present[i].tSendTime = mktime(&gm);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
	}
	//������Ϣ
	wDataSize=QuerySendPresenResult.wPresentCount*sizeof(QuerySendPresenResult.Present[0]);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);

	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	
	ASSERT(wDataSize == sizeof(DBR_GR_PropertyPresent));
	DBR_GR_PropertyPresent* PropertyPresentRequest = (DBR_GR_PropertyPresent*)pData;

	dwUserID = PropertyPresentRequest->dwUserID;

	//��������
	DBO_GR_PropertyPresent PropertyPresent={0};
	PropertyPresent.dwUserID = PropertyPresentRequest->dwUserID;
	PropertyPresent.dwRecvGameID = PropertyPresentRequest->dwRecvGameID;
	PropertyPresent.dwPropID = PropertyPresentRequest->dwPropID;
	PropertyPresent.wPropCount = PropertyPresentRequest->wPropCount;
	PropertyPresent.wType = PropertyPresentRequest->wType; //0�ǳ�  1 GameID
	lstrcpyn(PropertyPresent.szRecvNickName,PropertyPresentRequest->szRecvNickName,CountArray(PropertyPresent.szRecvNickName));

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&PropertyPresentRequest->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);


	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), PropertyPresentRequest->dwUserID);
	if( PropertyPresentRequest->wType == 0 )		//�����ǳ�����
	{
		m_PlatformDBAide.AddParameter(TEXT("@strReceiverNickName"), PropertyPresentRequest->szRecvNickName);
	}
	else if( PropertyPresentRequest->wType == 1 )	//���� GameID
	{
		m_PlatformDBAide.AddParameter(TEXT("@dwReceiverGameID"), PropertyPresentRequest->dwRecvGameID);
	}
	m_PlatformDBAide.AddParameter(TEXT("@dwPropID"), PropertyPresentRequest->dwPropID);
	m_PlatformDBAide.AddParameter(TEXT("@dwPropCount"), PropertyPresentRequest->wPropCount);
	m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

	//�������
	TCHAR szDescribeString[64]=TEXT("");
	m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = -1;
	if( PropertyPresentRequest->wType == 0 )
	{
		lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UserSendPresentByNickName"),true);
	}
	else if( PropertyPresentRequest->wType == 1 )
	{
		lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UserSendPresentByID"),true);
	}

	//������
	PropertyPresent.nHandleCode = lResultCode;
	if(lResultCode == 0)
	{
		PropertyPresent.dwRecvUserID = m_PlatformDBAide.GetValue_WORD(TEXT("RecvUserID"));
	}

	//��ȡ����
	CDBVarValue DBVarValue;
	m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
	lstrcpyn(PropertyPresent.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PropertyPresent.szNotifyContent));

	//������Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_PRESENT,dwContextID,&PropertyPresent,sizeof(PropertyPresent));

	return true;
}

bool CDataBaseEngineSink::OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	ASSERT(wDataSize == sizeof(DBR_GR_GetSendPresent));
	DBR_GR_GetSendPresent* pGetSendPresent = (DBR_GR_GetSendPresent*)pData;
	dwUserID = pGetSendPresent->dwUserID;
	//��������
	DBO_GR_GetSendPresent GetSendPresenResult;
	ZeroMemory(&GetSendPresenResult,sizeof(GetSendPresenResult));

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pGetSendPresent->dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pGetSendPresent->dwUserID);
	m_PlatformDBAide.AddParameter(TEXT("@szPassword"), pGetSendPresent->szPassword);
	m_PlatformDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);

	//ִ�в�ѯ
	LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_GetSendPresent"),true);
	WORD wHeadSize=sizeof(GetSendPresenResult)-sizeof(GetSendPresenResult.Present);

	if( lResultCode == 0 )
	{
		//��ȡ��Ϣ
		for (WORD i=0;;i++)
		{
			//�����ж�
			if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

			//����СЧ��
			ASSERT(GetSendPresenResult.wPresentCount<CountArray(GetSendPresenResult.Present));
			if (GetSendPresenResult.wPresentCount>=CountArray(GetSendPresenResult.Present))
			{
				WORD wDataSize=GetSendPresenResult.wPresentCount*sizeof(GetSendPresenResult.Present[0]);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
				i=0;
				GetSendPresenResult.wPresentCount = 0;
			}

			//��ȡ���� 
			GetSendPresenResult.wPresentCount++;
			GetSendPresenResult.Present[i].dwUserID=m_PlatformDBAide.GetValue_WORD(TEXT("UserID"));
			GetSendPresenResult.Present[i].dwRecvUserID=m_PlatformDBAide.GetValue_WORD(TEXT("ReceiverUserID"));
			GetSendPresenResult.Present[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
			GetSendPresenResult.Present[i].wPropCount=m_PlatformDBAide.GetValue_WORD(TEXT("PropCount"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),GetSendPresenResult.Present[i].szPropName,CountArray(GetSendPresenResult.Present[i].szPropName));
			SYSTEMTIME st;
			m_PlatformDBAide.GetValue_SystemTime(TEXT("SendTime"), st);
			struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
			GetSendPresenResult.Present[i].tSendTime = mktime(&gm);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
	}
	//������Ϣ
	wDataSize=GetSendPresenResult.wPresentCount*sizeof(GetSendPresenResult.Present[0]);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
	
	return true;
}

bool CDataBaseEngineSink::OnRequestLoadGameBuff(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		ASSERT(wDataSize == sizeof(DWORD));
		DWORD dUserID = *((DWORD*)pData);

		dwUserID = dUserID;

		//��������
		DBO_GR_UserGamePropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));
		GamePropertyBuff.dwUserID = dUserID;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), dUserID);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserGameBuff"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GamePropertyBuff.PropertyBuff);i++)
		{
			//�����ж�
			if (m_PlatformDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GamePropertyBuff.cbBuffCount<CountArray(GamePropertyBuff.PropertyBuff));
			if (GamePropertyBuff.cbBuffCount>=CountArray(GamePropertyBuff.PropertyBuff)) break;

			//��ȡ���� 
			GamePropertyBuff.cbBuffCount++;
			//GamePropertyBuff.PropertyBuff[i].dwPropID=m_PlatformDBAide.GetValue_WORD(TEXT("PropID"));
			GamePropertyBuff.PropertyBuff[i].dwKind=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			GamePropertyBuff.PropertyBuff[i].dwScoreMultiple=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTimeScoreMultiple"));
			GamePropertyBuff.PropertyBuff[i].UseResultsValidTime=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTime"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),GamePropertyBuff.PropertyBuff[i].szName,CountArray(GamePropertyBuff.PropertyBuff[i].szName));
			SYSTEMTIME st;
			m_PlatformDBAide.GetValue_SystemTime(TEXT("UseTime"), st);
			struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
			GamePropertyBuff.PropertyBuff[i].tUseTime = mktime(&gm);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GamePropertyBuff)-sizeof(GamePropertyBuff.PropertyBuff);
		WORD wDataSize=GamePropertyBuff.cbBuffCount*sizeof(GamePropertyBuff.PropertyBuff[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_UserGamePropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));

		//�������
		GamePropertyBuff.cbBuffCount=0L;

		//������Ϣ
		WORD wHeadSize=sizeof(GamePropertyBuff)-sizeof(GamePropertyBuff.PropertyBuff);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadGameTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		ASSERT(wDataSize == sizeof(DWORD));
		DWORD dUserID = *((DWORD*)pData);

		dwUserID = dUserID;

		//��������
		DBO_GR_UserGameTrumpet UserGameTrumpet;
		ZeroMemory(&UserGameTrumpet,sizeof(UserGameTrumpet));

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), dUserID);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserGameTrumpet"),true);
		
		if(lResultCode == 0)
		{
			UserGameTrumpet.dwTrumpetCount=m_PlatformDBAide.GetValue_WORD(TEXT("TrumpetCount"));
			UserGameTrumpet.dwTyphonCount=m_PlatformDBAide.GetValue_WORD(TEXT("TyphonCount"));
		}

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_TRUMPET,dwContextID,&UserGameTrumpet,sizeof(UserGameTrumpet));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_UserGameTrumpet UserGameTrumpet;
		ZeroMemory(&UserGameTrumpet,sizeof(UserGameTrumpet));

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_TRUMPET,dwContextID,&UserGameTrumpet,sizeof(UserGameTrumpet));

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_GR_Send_Trumpet));
		DBR_GR_Send_Trumpet* pSendTrumpet = (DBR_GR_Send_Trumpet*)pData;
		
		dwUserID = pSendTrumpet->dwUserID;
		
		//���ɽṹ
		DBO_GR_Send_Trumpet SendTrumpetResult={0};
		SendTrumpetResult.dwUserID = pSendTrumpet->dwUserID;
		SendTrumpetResult.dwRecvUserID = pSendTrumpet->dwRecvUserID;
		SendTrumpetResult.dwPropID = pSendTrumpet->dwPropID;
		SendTrumpetResult.wPropCount = pSendTrumpet->wPropCount;
		SendTrumpetResult.TrumpetColor = pSendTrumpet->TrumpetColor;
		lstrcpyn(SendTrumpetResult.szSendNickName,pSendTrumpet->szSendNickName,CountArray(SendTrumpetResult.szSendNickName));
		lstrcpyn(SendTrumpetResult.szTrumpetContent,pSendTrumpet->szTrumpetContent,CountArray(SendTrumpetResult.szTrumpetContent));

		//ʹ�ýṹ
		DBR_GR_PropertyUse PropertyUse={0};
		PropertyUse.dwUserID = pSendTrumpet->dwUserID;
		PropertyUse.dwRecvUserID = pSendTrumpet->dwRecvUserID;
		PropertyUse.dwPropID = pSendTrumpet->dwPropID;
		PropertyUse.wPropCount = pSendTrumpet->wPropCount;
		PropertyUse.dwClientAddr = pSendTrumpet->dwClientAddr;
		SendTrumpetResult.bSuccful = OnRequestPropertyUse(dwContextID, &PropertyUse, sizeof(PropertyUse),dwUserID);

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_TRUMPET,dwContextID,&SendTrumpetResult,sizeof(SendTrumpetResult));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_Send_Trumpet SendTrumpetResult={0};
		ZeroMemory(&SendTrumpetResult,sizeof(SendTrumpetResult));

		//������Ϣ	
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_TRUMPET,dwContextID,&SendTrumpetResult,sizeof(SendTrumpetResult));

		return false;
	}

	return true;
}


//��Ա����
bool CDataBaseEngineSink::OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//���ñ���
		dwUserID = 0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadMemberParameter"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_MemberParameter MemberParameter;
			ZeroMemory(&MemberParameter,sizeof(MemberParameter));

			//���ñ���
			WORD wMemberCount=0;
			tagMemberParameterNew * pMemberParameter=NULL;

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//����ж�
				if(MemberParameter.wMemberCount>=CountArray(MemberParameter.MemberParameter)) break;

				//���ñ���
				pMemberParameter = &MemberParameter.MemberParameter[MemberParameter.wMemberCount++];

				//��ȡ����
				pMemberParameter->cbMemberOrder= m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));				
				pMemberParameter->dwMemberRight= m_PlatformDBAide.GetValue_DWORD(TEXT("UserRight"));
				pMemberParameter->dwMemberTask= m_PlatformDBAide.GetValue_DWORD(TEXT("TaskRate"));
				pMemberParameter->dwMemberShop= m_PlatformDBAide.GetValue_DWORD(TEXT("ShopRate"));
				pMemberParameter->dwMemberInsure= m_PlatformDBAide.GetValue_DWORD(TEXT("InsureRate"));
				pMemberParameter->dwMemberDayPresent= m_PlatformDBAide.GetValue_DWORD(TEXT("DayPresent"));
				pMemberParameter->dwMemberDayGiftID= m_PlatformDBAide.GetValue_DWORD(TEXT("DayGiftID"));
				m_PlatformDBAide.GetValue_String(TEXT("MemberName"),pMemberParameter->szMemberName,CountArray(pMemberParameter->szMemberName));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(MemberParameter)-sizeof(MemberParameter.MemberParameter);
			wSendDataSize += sizeof(tagMemberParameterNew)*MemberParameter.wMemberCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_MEMBER_PAREMETER,dwContextID,&MemberParameter,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ա��ѯ
bool CDataBaseEngineSink::OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_MemberQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_MemberQueryInfo)) return false;

		//��ȡ����
		DBR_GR_MemberQueryInfo * pDBRMemberInfo=(DBR_GR_MemberQueryInfo *)pData;

		//���ñ���
		dwUserID = pDBRMemberInfo->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberQueryInfo"),true);

		//��������
		DBO_GR_MemberQueryInfoResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//��ȡ����
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			pDBOMemberInfoResult.bPresent = (m_PlatformDBAide.GetValue_BYTE(TEXT("Present"))==TRUE);
			pDBOMemberInfoResult.bGift = (m_PlatformDBAide.GetValue_BYTE(TEXT("Gift"))==TRUE);
		}
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//ִ������
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberGiftQuery"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			WORD wMemberCount=0;
			tagGiftPropertyInfo * pMemberParameter=NULL;

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//����ж�
				if(pDBOMemberInfoResult.GiftSubCount>=CountArray(pDBOMemberInfoResult.GiftSub)) break;

				//���ñ���
				pMemberParameter = &pDBOMemberInfoResult.GiftSub[pDBOMemberInfoResult.GiftSubCount++];

				//��ȡ����
				pMemberParameter->wPropertyID= m_PlatformDBAide.GetValue_WORD(TEXT("ID"));
				pMemberParameter->wPropertyCount= m_PlatformDBAide.GetValue_WORD(TEXT("Count"));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}
		}

		//�����С
		WORD wSendDataSize = sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.GiftSub);
		wSendDataSize += (WORD)(sizeof(tagGiftPropertyInfo))*(WORD)pDBOMemberInfoResult.GiftSubCount;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEMBER_QUERY_INFO_RESULT,dwContextID,&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;

}
//��Ա�ͽ�
bool CDataBaseEngineSink::OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_MemberDayPresent));
		if (wDataSize!=sizeof(DBR_GR_MemberDayPresent)) return false;

		//��ȡ����
		DBR_GR_MemberDayPresent * pDBRMemberInfo=(DBR_GR_MemberDayPresent *)pData;

		//���ñ���
		dwUserID = pDBRMemberInfo->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberDayPresent"),true);

		//��������
		DBO_GR_MemberDayPresentResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//��ȡ����
		pDBOMemberInfoResult.bSuccessed= (lResultCode ==DB_SUCCESS);


		//��ȡ����
		if(lResultCode==DB_SUCCESS )
		{
			pDBOMemberInfoResult.lGameScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
		}

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(pDBOMemberInfoResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(pDBOMemberInfoResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.szNotifyContent);
		wSendSize+=CountStringBuffer(pDBOMemberInfoResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEMBER_DAY_PRESENT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}
//��Ա���
bool CDataBaseEngineSink::OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_MemberDayGift));
		if (wDataSize!=sizeof(DBR_GR_MemberDayGift)) return false;

		//��ȡ����
		DBR_GR_MemberDayGift * pDBRMemberInfo=(DBR_GR_MemberDayGift *)pData;

		//���ñ���
		dwUserID = pDBRMemberInfo->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRMemberInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRMemberInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pDBRMemberInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pDBRMemberInfo->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_MemberDayGift"),true);

		//��������
		DBO_GR_MemberDayGiftResult pDBOMemberInfoResult;
		ZeroMemory(&pDBOMemberInfoResult,sizeof(pDBOMemberInfoResult));

		//��ȡ����
		pDBOMemberInfoResult.bSuccessed= (lResultCode ==DB_SUCCESS);
		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(pDBOMemberInfoResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(pDBOMemberInfoResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(pDBOMemberInfoResult)-sizeof(pDBOMemberInfoResult.szNotifyContent);
		wSendSize+=CountStringBuffer(pDBOMemberInfoResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MEMBER_DAY_GIFT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//����ǩ��
bool CDataBaseEngineSink::OnRequestLoadCheckIn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		dwUserID = 0;
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadCheckInReward"),true);

		//����ṹ
		DBO_GR_CheckInReward CheckInReward;
		ZeroMemory(&CheckInReward,sizeof(CheckInReward));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			WORD wDayIndex=0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				wDayIndex = m_PlatformDBAide.GetValue_WORD(TEXT("DayID"));
				if(wDayIndex<=LEN_WEEK)
				{
					CheckInReward.lRewardGold[wDayIndex-1] = m_PlatformDBAide.GetValue_DOUBLE(TEXT("RewardGold")); 
				}

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_REWARD,dwContextID,&CheckInReward,sizeof(CheckInReward));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ѯǩ��
bool CDataBaseEngineSink::OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{

		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_CheckInQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_CheckInQueryInfo)) return false;

		//������
		DBR_GR_CheckInQueryInfo * pCheckInQueryInfo=(DBR_GR_CheckInQueryInfo *)pData;

		dwUserID = pCheckInQueryInfo->dwUserID;
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pCheckInQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pCheckInQueryInfo->szPassword);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_CheckInQueryInfo"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_CheckInInfo CheckInInfo;
			ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

			//������Ϣ
			CheckInInfo.wSeriesDate=m_PlatformDBAide.GetValue_WORD(TEXT("SeriesDate"));
			CheckInInfo.bTodayChecked = (m_PlatformDBAide.GetValue_BYTE(TEXT("TodayCheckIned"))==TRUE);

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_INFO,dwContextID,&CheckInInfo,sizeof(CheckInInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_CheckInResult CheckInResult;
			CheckInResult.bSuccessed=false;
			lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

			//���ͽ��
			WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(CheckInResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//ִ��ǩ��
bool CDataBaseEngineSink::OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_CheckInDone));
		if (wDataSize!=sizeof(DBR_GR_CheckInDone)) return false;

		//������
		DBR_GR_CheckInDone * pCheckInDone=(DBR_GR_CheckInDone *)pData;

		dwUserID = pCheckInDone->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pCheckInDone->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pCheckInDone->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pCheckInDone->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pCheckInDone->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_CheckInDone"),true);

		//��������
		DBO_GR_CheckInResult CheckInResult;
		ZeroMemory(&CheckInResult,sizeof(CheckInResult));

		//��ȡ����
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			CheckInResult.lScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
		}

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		CheckInResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(CheckInResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//���صͱ�
bool CDataBaseEngineSink::OnRequestLoadBaseEnsure( DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID )
{
	try
	{
		
		dwUserID = 0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadBaseEnsure"),true);

		//ִ�гɹ�
		if (lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_BaseEnsureParameter BaseEnsureParameter;
			ZeroMemory(&BaseEnsureParameter,sizeof(BaseEnsureParameter));

			//��������
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				BaseEnsureParameter.cbTakeTimes= m_PlatformDBAide.GetValue_BYTE(TEXT("TakeTimes"));
				BaseEnsureParameter.lScoreAmount = m_PlatformDBAide.GetValue_DOUBLE(TEXT("ScoreAmount"));
				BaseEnsureParameter.lScoreCondition = m_PlatformDBAide.GetValue_DOUBLE(TEXT("ScoreCondition"));				

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BASEENSURE_PARAMETER,dwContextID,&BaseEnsureParameter,sizeof(BaseEnsureParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}

//��ȡ�ͱ�
bool CDataBaseEngineSink::OnRequestTakeBaseEnsure( DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID )
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TakeBaseEnsure));
		if (wDataSize!=sizeof(DBR_GR_TakeBaseEnsure)) return false;

		//��ȡ����
		DBR_GR_TakeBaseEnsure * pTakeBaseEnsure=(DBR_GR_TakeBaseEnsure *)pData;

		dwUserID = pTakeBaseEnsure->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTakeBaseEnsure->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTakeBaseEnsure->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTakeBaseEnsure->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTakeBaseEnsure->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_TakeBaseEnsure"),true);

		//��������
		DBO_GR_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//��ȡ����
		if(lResultCode==DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			BaseEnsureResult.lGameScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
		}

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);

		//������Ϣ
		BaseEnsureResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(BaseEnsureResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(BaseEnsureResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//���ñ���
		BaseEnsureResult.bSuccessed=false;
		lstrcpyn(BaseEnsureResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(BaseEnsureResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return false;
	}

	return true;
}

//�û���Ϸ����
bool CDataBaseEngineSink::OnRequestWriteUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//������
		DBR_GR_WriteUserGameData * pPropertyRequest=(DBR_GR_WriteUserGameData *)pData;
		dwUserID=pPropertyRequest->dwUserID;

		//Ч�����
		WORD wDataSize1=CountStringBuffer(pPropertyRequest->szUserGameData);
		WORD wHeadSize1=sizeof(DBR_GR_WriteUserGameData)-sizeof(pPropertyRequest->szUserGameData);

		ASSERT(wDataSize1<wDataSize && wDataSize==wDataSize1+wHeadSize1);
		if (wDataSize1>=wDataSize && wDataSize!=wDataSize1+wHeadSize1) return false;		

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();

		//������Ϣ
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),pPropertyRequest->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pPropertyRequest->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strUserGameData"),pPropertyRequest->szUserGameData);

		//�������
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_WriteUserGameData"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//��������
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchSignup->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@InitScore"),pMatchSignup->lInitScore);		
		m_GameDBAide.AddParameter(TEXT("@cbSignupMode"),pMatchSignup->cbSignupMode);		
		m_GameDBAide.AddParameter(TEXT("@wMaxSignupUser"),pMatchSignup->wMaxSignupUser);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchSignup->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchSignup->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchSignup->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���
 		MatchSignupResult.dwUserID=pMatchSignup->dwUserID;
		MatchSignupResult.lMatchNo=pMatchSignup->lMatchNo;		
		MatchSignupResult.bResultCode=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchSignupResult.bResultCode==true)
		{
			MatchSignupResult.lCurrGold=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
			MatchSignupResult.lCurrIngot=m_GameDBAide.GetValue_DOUBLE(TEXT("UserIngot"));
			MatchSignupResult.cbSignupMode=m_GameDBAide.GetValue_BYTE(TEXT("SignupMode"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));
		}

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���		
		MatchSignupResult.bResultCode=false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}

	return true;
}

//�˳�����
bool CDataBaseEngineSink::OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchUnSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchUnSignup)) return false;



	//��������
	DBR_GR_MatchUnSignup * pMatchUnSignup=(DBR_GR_MatchUnSignup *)pData;
	dwUserID=pMatchUnSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchUnSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchUnSignup->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchUnSignup->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchUnSignup->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchUnSignup->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

		//���ȡ��
		if (pMatchUnSignup->dwReason==UNSIGNUP_REASON_PLAYER)
		{
			//����ṹ
			DBO_GR_MatchSingupResult MatchSignupResult;
			ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

			//���ñ���
			MatchSignupResult.dwReason=pMatchUnSignup->dwReason;
			MatchSignupResult.dwUserID=pMatchUnSignup->dwUserID;
			MatchSignupResult.lMatchNo=pMatchUnSignup->lMatchNo;		
			MatchSignupResult.bResultCode=lReturnValue==DB_SUCCESS;

			//��ȡ�Ƹ�
			if(MatchSignupResult.bResultCode==true)
			{				
				MatchSignupResult.lCurrGold=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
				MatchSignupResult.lCurrIngot=m_GameDBAide.GetValue_DOUBLE(TEXT("UserIngot"));
				MatchSignupResult.cbSignupMode=m_GameDBAide.GetValue_BYTE(TEXT("SignupMode"));
			}
			else
			{
				//��������
				CDBVarValue DBVarValue;
				m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
				lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));
			}

			//���ͽ��
			WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
			wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_UNSIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//���ȡ��
		if (pMatchUnSignup->dwReason==UNSIGNUP_REASON_PLAYER)
		{
			//����ṹ
			DBO_GR_MatchSingupResult MatchSignupResult;
			ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

			//���ñ���
			MatchSignupResult.bResultCode=false;
			MatchSignupResult.dwReason=pMatchUnSignup->dwReason;
			MatchSignupResult.dwUserID=pMatchUnSignup->dwUserID;
			MatchSignupResult.lMatchNo=pMatchUnSignup->lMatchNo;
			lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

			//���ͽ��
			WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
			wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);
		}

		return false;
	}

	return true;
}

//������ʼ
bool CDataBaseEngineSink::OnRequestMatchSignupStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_SignupStart));
	if (wDataSize!=sizeof(DBR_GR_SignupStart)) return false;

	//��������
	DBR_GR_SignupStart * pSignupStart=(DBR_GR_SignupStart *)pData;	
	dwUserID=pSignupStart->dwMatchID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pSignupStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pSignupStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pSignupStart->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pSignupStart->cbMatchType);
		m_GameDBAide.AddParameter(TEXT("@cbSignupMode"),pSignupStart->cbSignupMode);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchSignupStart"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��ѯ�ʸ�
bool CDataBaseEngineSink::OnRequestMatchQueryQualify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryMatchQualify));
	if (wDataSize!=sizeof(DBR_GR_QueryMatchQualify)) return false;

	//��������
	DBR_GR_QueryMatchQualify * pQueryMatchQualify=(DBR_GR_QueryMatchQualify *)pData;
	dwUserID=pQueryMatchQualify->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pQueryMatchQualify->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pQueryMatchQualify->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pQueryMatchQualify->lMatchNo);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchQueryQualify"),false);

		//������
		DBO_GR_MatchQueryResult MatchQueryResult;
		MatchQueryResult.bHoldQualify=lReturnValue==DB_SUCCESS;

		//��������			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_QUERY_RESULT,dwContextID,&MatchQueryResult,sizeof(MatchQueryResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//������
bool CDataBaseEngineSink::OnRequestMatchBuySafeCard(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchBuySafeCard));
	if (wDataSize!=sizeof(DBR_GR_MatchBuySafeCard)) return false;

	//��������
	DBR_GR_MatchBuySafeCard * pMatchBuySafeCard=(DBR_GR_MatchBuySafeCard *)pData;
	dwUserID=pMatchBuySafeCard->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchBuySafeCard->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchBuySafeCard->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchBuySafeCard->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@lSafeCardFee"),pMatchBuySafeCard->lSafeCardFee);		

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchBuySafeCard"),true);

		//����ṹ
		DBO_GR_MatchBuyResult MatchBuyResult;
		ZeroMemory(&MatchBuyResult,sizeof(MatchBuyResult));

		//���ñ���
		MatchBuyResult.bSuccessed=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchBuyResult.bSuccessed==true)
		{
			MatchBuyResult.lCurrGold=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(MatchBuyResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchBuyResult.szDescribeString));
		}

		//��������		
		WORD wSendDataSize=sizeof(MatchBuyResult)-sizeof(MatchBuyResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchBuyResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_BUY_RESULT,dwContextID,&MatchBuyResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchBuyResult MatchBuyResult;
		ZeroMemory(&MatchBuyResult,sizeof(MatchBuyResult));

		//���ñ���
		MatchBuyResult.bSuccessed=false;
		lstrcpyn(MatchBuyResult.szDescribeString,TEXT("���ݿ��쳣�����տ�����ʧ�ܣ�"),CountArray(MatchBuyResult.szDescribeString));

		//��������			
		WORD wSendDataSize=sizeof(MatchBuyResult)-sizeof(MatchBuyResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchBuyResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_BUY_RESULT,dwContextID,&MatchBuyResult,wSendDataSize);

		return false;
	}

	return true;
}

//������ʼ
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//��������
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;
	dwUserID=pMatchStart->dwMatchID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchStart->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchStart->cbMatchType);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//��������
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;
	dwUserID=pMatchOver->dwMatchID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchOver->wServerID);	
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchOver->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchOver->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchOver->cbMatchType);					
		m_GameDBAide.AddParameter(TEXT("@cbPlayCount"),pMatchOver->wPlayCount);	
		m_GameDBAide.AddParameter(TEXT("@bMatchFinish"),pMatchOver->bMatchFinish);	
		m_GameDBAide.AddParameter(TEXT("@MatchStartTime"),pMatchOver->MatchStartTime);	
		m_GameDBAide.AddParameter(TEXT("@MatchEndTime"),pMatchOver->MatchEndTime);	

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

		//ִ�гɹ�
		if(lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_MatchRankList MatchRankList;
			ZeroMemory(&MatchRankList,sizeof(MatchRankList));

			//��������
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//���ñ���
			MatchRankList.dwMatchID=pMatchOver->dwMatchID;
			MatchRankList.lMatchNo=pMatchOver->lMatchNo;
			MatchRankList.bMatchFinish=pMatchOver->bMatchFinish;

			//��ȡ��¼
			while(m_GameDBModule->IsRecordsetEnd()==false)
			{
				pMatchRankInfo = &MatchRankList.MatchRankInfo[MatchRankList.wUserCount++];
				pMatchRankInfo->wRankID=m_GameDBAide.GetValue_WORD(TEXT("RankID"));
				pMatchRankInfo->dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
				pMatchRankInfo->lMatchScore=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
				pMatchRankInfo->lRewardGold=m_GameDBAide.GetValue_DOUBLE(TEXT("RewardGold"));
				pMatchRankInfo->lRewardIngot=m_GameDBAide.GetValue_DOUBLE(TEXT("RewardIngot"));
				pMatchRankInfo->dwRewardExperience=m_GameDBAide.GetValue_DWORD(TEXT("RewardExperience"));

				//�ƶ��α� 
				m_GameDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(MatchRankList)-sizeof(MatchRankList.MatchRankInfo);
			wSendDataSize += MatchRankList.wUserCount*sizeof(MatchRankList.MatchRankInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK_LIST,dwContextID,&MatchRankList,wSendDataSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//����ȡ��
bool CDataBaseEngineSink::OnRequestMatchCancel(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchCancel));
	if (wDataSize!=sizeof(DBR_GR_MatchCancel)) return false;

	//��������
	DBR_GR_MatchCancel * pMatchCancel=(DBR_GR_MatchCancel *)pData;
	dwUserID=pMatchCancel->dwMatchID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchCancel->wServerID);	
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchCancel->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchCancel->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@lSafeCardFee"),pMatchCancel->lSafeCardFee);
		m_GameDBAide.AddParameter(TEXT("@bMatchFinish"),pMatchCancel->bMatchFinish);	

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchCancel"),false);

		//����ṹ
		DBO_GR_MatchCancelResult MatchCancelResult;		
		MatchCancelResult.bSuccessed=lReturnValue==DB_SUCCESS;
		MatchCancelResult.bMatchFinish=pMatchCancel->bMatchFinish;

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_CANCEL_RESULT,dwContextID,&MatchCancelResult,sizeof(MatchCancelResult));
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//��������
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=pMatchReward->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);		
		m_GameDBAide.AddParameter(TEXT("@lRewardGold"),pMatchReward->lRewardGold);
		m_GameDBAide.AddParameter(TEXT("@lRewardIngot"),pMatchReward->lRewardIngot);		
		m_GameDBAide.AddParameter(TEXT("@dwRewardExperience"),pMatchReward->dwRewardExperience);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

		//����ṹ
		DBO_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		//���ñ���
		MatchRewardResult.dwUserID=pMatchReward->dwUserID;
		MatchRewardResult.bResultCode=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchRewardResult.bResultCode==true)
		{
			MatchRewardResult.lCurrGold=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		//DBO_GR_MatchRewardResult MatchRewardResult;
		//ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		////���ñ���
		//MatchRewardResult.bResultCode=false;

		////���ͽ��
		//m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return false;
	}

	return true;
}

//������̭
bool CDataBaseEngineSink::OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchEliminate));
	if (wDataSize!=sizeof(DBR_GR_MatchEliminate)) return false;

	//��������
	DBR_GR_MatchEliminate * pMatchEliminate=(DBR_GR_MatchEliminate *)pData;
	dwUserID=pMatchEliminate->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchEliminate->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchEliminate->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchEliminate->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchEliminate->lMatchNo);	
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchEliminate->cbMatchType);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchEliminate"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}


//��¼�ɼ�
bool CDataBaseEngineSink::OnRequestMatchRecordGrades(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchRecordGrades));
	if (wDataSize!=sizeof(DBR_GR_MatchRecordGrades)) return false;

	//��������
	DBR_GR_MatchRecordGrades * pMatchRecordGrades=(DBR_GR_MatchRecordGrades *)pData;
	dwUserID=pMatchRecordGrades->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchRecordGrades->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchRecordGrades->wServerID);		
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchRecordGrades->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchRecordGrades->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@lInitScore"),pMatchRecordGrades->lInitScore);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchRecordGrades"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ҹ���
bool CDataBaseEngineSink::OnRequestMatchUserRevive(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchUserRevive));
	if (wDataSize!=sizeof(DBR_GR_MatchUserRevive)) return false;

	//��������
	DBR_GR_MatchUserRevive * pMatchUserRevive=(DBR_GR_MatchUserRevive *)pData;
	dwUserID=pMatchUserRevive->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchUserRevive->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchUserRevive->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchUserRevive->lMatchNo);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);		
		m_GameDBAide.AddParameter(TEXT("@InitScore"),pMatchUserRevive->lInitScore);
		m_GameDBAide.AddParameter(TEXT("@CullScore"),pMatchUserRevive->lCullScore);
		m_GameDBAide.AddParameter(TEXT("@lReviveFee"),pMatchUserRevive->lReviveFee);		
		m_GameDBAide.AddParameter(TEXT("@cbReviveTimes"),pMatchUserRevive->cbReviveTimes);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchUserRevive"),true);

		//����ṹ
		DBO_GR_MatchReviveResult MatchReviveResult;
		ZeroMemory(&MatchReviveResult,sizeof(MatchReviveResult));

		//���ñ���
		MatchReviveResult.bSuccessed=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchReviveResult.bSuccessed==true)
		{
			MatchReviveResult.lCurrGold=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(MatchReviveResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchReviveResult.szDescribeString));
		}

		//��������	
		WORD wSendDataSize=sizeof(MatchReviveResult)-sizeof(MatchReviveResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchReviveResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REVIVE_RESULT,dwContextID,&MatchReviveResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchReviveResult MatchReviveResult;
		ZeroMemory(&MatchReviveResult,sizeof(MatchReviveResult));

		//���ñ���
		MatchReviveResult.bSuccessed=false;
		lstrcpyn(MatchReviveResult.szDescribeString,TEXT("���ݿ��쳣������ʧ�ܣ�"),CountArray(MatchReviveResult.szDescribeString));

		//��������			
		WORD wSendDataSize=sizeof(MatchReviveResult)-sizeof(MatchReviveResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchReviveResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REVIVE_RESULT,dwContextID,&MatchReviveResult,wSendDataSize);

		return true;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestMatchQueryReviveInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchQueryRevive));
	if (wDataSize!=sizeof(DBR_GR_MatchQueryRevive)) return false;

	//��������
	DBR_GR_MatchQueryRevive * pMatchQueryRevive=(DBR_GR_MatchQueryRevive *)pData;
	dwUserID=pMatchQueryRevive->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchQueryRevive->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchQueryRevive->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pMatchQueryRevive->lMatchNo);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchQueryReviveInfo"),true);

		//������
		DBO_GR_MatchReviveInfo MatchReviveInfo;
		ZeroMemory(&MatchReviveInfo,sizeof(MatchReviveInfo));

		//��ȡ����
		if(lReturnValue==DB_SUCCESS)
		{
			MatchReviveInfo.bSuccessed=true;
			MatchReviveInfo.bSafeCardRevive=(m_GameDBAide.GetValue_BYTE(TEXT("SafeCardRevive"))==TRUE);
			MatchReviveInfo.cbReviveTimesed=m_GameDBAide.GetValue_BYTE(TEXT("ReviveTimes"));
		}

		//��������			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REVIVE_INFO,dwContextID,&MatchReviveInfo,sizeof(MatchReviveInfo));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		DBO_GR_MatchReviveInfo MatchReviveInfo;
		ZeroMemory(&MatchReviveInfo,sizeof(MatchReviveInfo));

		//��������			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REVIVE_INFO,dwContextID,&MatchReviveInfo,sizeof(MatchReviveInfo));

		return true;
	}

	return true;
}


//�ȼ�����
bool CDataBaseEngineSink::OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		dwUserID=0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGrowLevelConfig"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_GrowLevelConfig GrowLevelConfig;
			ZeroMemory(&GrowLevelConfig,sizeof(GrowLevelConfig));

			//���ñ���
			WORD wLevelCount=0;
			tagGrowLevelConfig * pGrowLevelConfig=NULL;

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//����ж�
				if(GrowLevelConfig.wLevelCount>=CountArray(GrowLevelConfig.GrowLevelConfig)) break;

				//���ñ���
				pGrowLevelConfig = &GrowLevelConfig.GrowLevelConfig[GrowLevelConfig.wLevelCount++];

				//��ȡ����
				pGrowLevelConfig->wLevelID= m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
				pGrowLevelConfig->dwExperience= m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(GrowLevelConfig)-sizeof(GrowLevelConfig.GrowLevelConfig);
			wSendDataSize += sizeof(tagGrowLevelConfig)*GrowLevelConfig.wLevelCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_CONFIG,dwContextID,&GrowLevelConfig,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��ѯ�ȼ�
bool CDataBaseEngineSink::OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_GrowLevelQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_GrowLevelQueryInfo)) return false;

		//������
		DBR_GR_GrowLevelQueryInfo * pGrowLevelQueryInfo=(DBR_GR_GrowLevelQueryInfo *)pData;
		dwUserID=pGrowLevelQueryInfo->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pGrowLevelQueryInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pGrowLevelQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pGrowLevelQueryInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pGrowLevelQueryInfo->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strUpgradeDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGrowLevel"),true);

		//����ṹ
		DBO_GR_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				GrowLevelParameter.wCurrLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("CurrLevelID"));				
				GrowLevelParameter.dwExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));
				GrowLevelParameter.dwUpgradeExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("UpgradeExperience"));
				GrowLevelParameter.lUpgradeRewardGold = m_PlatformDBAide.GetValue_DOUBLE(TEXT("RewardGold"));
				GrowLevelParameter.lUpgradeRewardIngot = m_PlatformDBAide.GetValue_DOUBLE(TEXT("RewardMedal"));			
			}	

			//����ṹ
			DBO_GR_GrowLevelUpgrade GrowLevelUpgrade;
			ZeroMemory(&GrowLevelUpgrade,sizeof(GrowLevelUpgrade));

			//������ʾ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strUpGradeDescribe"),DBVarValue);
			lstrcpyn(GrowLevelUpgrade.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(GrowLevelUpgrade.szNotifyContent));
			if(GrowLevelUpgrade.szNotifyContent[0]!=0)
			{
				//��ȡ�Ƹ�
				GrowLevelUpgrade.lCurrScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
				GrowLevelUpgrade.lCurrIngot = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Ingot"));	

				//������ʾ
				WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
				wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_UPGRADE,dwContextID,&GrowLevelUpgrade,wSendDataSize);
			}
		}

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return false;
	}
}

//�û�Ȩ��
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//������
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
		dwUserID=pManageUserRight->dwUserID;

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//����Ȩ��
bool CDataBaseEngineSink::OnRequestManageMatchRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//������
		DBR_GR_ManageMatchRight * pManageMatchRight=(DBR_GR_ManageMatchRight *)pData;
		dwUserID=pManageMatchRight->dwUserID;

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageMatchRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageMatchRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageMatchRight->dwRemoveRight);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pManageMatchRight->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pManageMatchRight->dwMatchNO);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageMatchRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//ϵͳ��Ϣ
bool CDataBaseEngineSink::OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSystemMessage"),true);

		//�������
		if(lReturnValue==0)
		{
			TCHAR szServerID[32]={0};
			_sntprintf(szServerID, CountArray(szServerID), TEXT("%d"), m_pGameServiceOption->wServerID);

			while(true)
			{
				//�����ж�
				if (m_GameDBModule->IsRecordsetEnd()==true) break;

				//�������
				TCHAR szServerRange[1024]={0};
				CString strServerRange;
				bool bSendMessage=false;
				bool bAllRoom=false;

				//��ȡ��Χ
				m_GameDBAide.GetValue_String(TEXT("ServerRange"), szServerRange, CountArray(szServerRange));
				szServerRange[1023]=0;
				strServerRange.Format(TEXT("%s"), szServerRange);

				//��Χ�ж�
				while(true)
				{
					int nfind=strServerRange.Find(TEXT(','));
					if(nfind!=-1 && nfind>0)
					{
						CString strID=strServerRange.Left(nfind);
						WORD wServerID=StrToInt(strID);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						if(bSendMessage) break;

						strServerRange=strServerRange.Right(strServerRange.GetLength()-nfind-1);
					}
					else
					{
						WORD wServerID=StrToInt(szServerRange);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						break;
					}
				}

				//������Ϣ
				if(bSendMessage)
				{
					//�������
					DBR_GR_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//��ȡ��Ϣ
					SystemMessage.dwMessageID=m_GameDBAide.GetValue_DWORD(TEXT("ID"));
					SystemMessage.cbMessageType=m_GameDBAide.GetValue_BYTE(TEXT("MessageType"));
					SystemMessage.dwTimeRate=m_GameDBAide.GetValue_DWORD(TEXT("TimeRate"));
					SystemMessage.cbAllRoom=bAllRoom?TRUE:FALSE;
					m_GameDBAide.GetValue_String(TEXT("MessageString"),SystemMessage.szSystemMessage,CountArray(SystemMessage.szSystemMessage));

					//��ȡʱ��
					SYSTEMTIME systime;
					ZeroMemory(&systime, sizeof(systime));

					//��ʼʱ��
					m_GameDBAide.GetValue_SystemTime(TEXT("StartTime"),systime);
					CTime StarTime(systime);
					SystemMessage.tStartTime=StarTime.GetTime();

					//����ʱ��
					m_GameDBAide.GetValue_SystemTime(TEXT("ConcludeTime"),systime);
					CTime ConcludeTime(systime);
					SystemMessage.tConcludeTime=ConcludeTime.GetTime();

					//���ͽ��
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_RESULT,dwContextID,&SystemMessage,sizeof(SystemMessage));
				}

				//��һ��
				m_GameDBModule->MoveToNext();
			}

			//�������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_FINISH,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}


//�������д�
bool CDataBaseEngineSink::OnRequestLoadSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSensitiveWords"),true);

		//��ȡ��Ϣ
		if (lReturnValue==DB_SUCCESS)
		{
			//��ʼ��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xfffe,NULL,0);

			//��ȡ��Ϣ
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��������
				TCHAR szSensitiveWords[32]=TEXT("");

				//��ȡ��Ϣ
				m_PlatformDBAide.GetValue_String(TEXT("SensitiveWords"),szSensitiveWords,CountArray(szSensitiveWords));				

				//������Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0,szSensitiveWords,sizeof(szSensitiveWords));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			};

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xffff,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//����������
bool CDataBaseEngineSink::OnRequestUnlockAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_UnlockAndroidUser));
		if (wDataSize!=sizeof(DBR_GR_UnlockAndroidUser)) return false;

		//������
		DBR_GR_UnlockAndroidUser * pUnlockAndroidUser=(DBR_GR_UnlockAndroidUser *)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),pUnlockAndroidUser->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@wBatchID"),pUnlockAndroidUser->wBatchID);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UnlockAndroidUser"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

	return true;
}
//�������
bool CDataBaseEngineSink::OnRequestDeblocking(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		DBR_GR_Deblocking * pDeblocking=(DBR_GR_Deblocking *)pData;
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_Deblocking));
		if (wDataSize!=sizeof(DBR_GR_Deblocking)) return false;


		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pDeblocking->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),pDeblocking->dwServerID);


		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("P_UnLockUserInGame"),false);
	}
	catch(...)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("OnRequestWriteJoinInfo �������д���") );
		CTraceService::TraceString(szInfo,TraceLevel_Exception);
	}


	return true;
}
//��¼���
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		m_LogonSuccess.wFaceID=m_GameDBAide.GetValue_WORD(TEXT("FaceID"));
		m_LogonSuccess.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		m_LogonSuccess.dwGameID=m_GameDBAide.GetValue_DWORD(TEXT("GameID"));
		m_LogonSuccess.dwGroupID=m_GameDBAide.GetValue_DWORD(TEXT("GroupID"));
		m_LogonSuccess.dwCustomID=m_GameDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_GameDBAide.GetValue_String(TEXT("NickName"),m_LogonSuccess.szNickName,CountArray(m_LogonSuccess.szNickName));
		m_GameDBAide.GetValue_String(TEXT("GroupName"),m_LogonSuccess.szGroupName,CountArray(m_LogonSuccess.szGroupName));

		//�û�����
		m_LogonSuccess.cbGender=m_GameDBAide.GetValue_BYTE(TEXT("Gender"));
		m_LogonSuccess.cbMemberOrder=m_GameDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		m_LogonSuccess.cbMasterOrder=m_GameDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_GameDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));

		//IP������
		m_GameDBAide.GetValue_String(TEXT("LastLogonIP"),m_LogonSuccess.szChangeLogonIP,CountArray(m_LogonSuccess.szChangeLogonIP));

		//�û�����
		m_GameDBAide.GetValue_String(TEXT("QQ"),m_LogonSuccess.szQQ,CountArray(m_LogonSuccess.szQQ));
		m_GameDBAide.GetValue_String(TEXT("EMail"),m_LogonSuccess.szEMail,CountArray(m_LogonSuccess.szEMail));
		m_GameDBAide.GetValue_String(TEXT("SeatPhone"),m_LogonSuccess.szSeatPhone,CountArray(m_LogonSuccess.szSeatPhone));
		m_GameDBAide.GetValue_String(TEXT("MobilePhone"),m_LogonSuccess.szMobilePhone,CountArray(m_LogonSuccess.szMobilePhone));
		m_GameDBAide.GetValue_String(TEXT("DwellingPlace"),m_LogonSuccess.szDwellingPlace,CountArray(m_LogonSuccess.szDwellingPlace));
		m_GameDBAide.GetValue_String(TEXT("PostalCode"),m_LogonSuccess.szPostalCode,CountArray(m_LogonSuccess.szPostalCode));

		//������Ϣ
		m_LogonSuccess.lScore=m_GameDBAide.GetValue_DOUBLE(TEXT("Score"));
		m_LogonSuccess.lIngot=m_GameDBAide.GetValue_DOUBLE(TEXT("Ingot"));
		m_LogonSuccess.lGrade=m_GameDBAide.GetValue_DOUBLE(TEXT("Grade"));
		m_LogonSuccess.lInsure=m_GameDBAide.GetValue_DOUBLE(TEXT("Insure"));
		m_LogonSuccess.dBeans=m_GameDBAide.GetValue_DOUBLE(TEXT("Beans"));

		//������Ϣ
		m_LogonSuccess.dwWinCount=m_GameDBAide.GetValue_LONG(TEXT("WinCount"));
		m_LogonSuccess.dwLostCount=m_GameDBAide.GetValue_LONG(TEXT("LostCount"));
		m_LogonSuccess.dwDrawCount=m_GameDBAide.GetValue_LONG(TEXT("DrawCount"));
		m_LogonSuccess.dwFleeCount=m_GameDBAide.GetValue_LONG(TEXT("FleeCount"));		
		m_LogonSuccess.dwExperience=m_GameDBAide.GetValue_LONG(TEXT("Experience"));
		m_LogonSuccess.lLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("LoveLiness"));
		m_LogonSuccess.lIntegralCount=m_GameDBAide.GetValue_LONGLONG(TEXT("IntegralCount"));

		//������Ϣ
		m_LogonSuccess.dwAgentID=m_GameDBAide.GetValue_LONG(TEXT("AgentID"));

		//������Ϣ		
		m_LogonSuccess.dwUserRight=m_GameDBAide.GetValue_DWORD(TEXT("UserRight"));
		m_LogonSuccess.dwMasterRight=m_GameDBAide.GetValue_DWORD(TEXT("MasterRight"));
		m_LogonSuccess.cbDeviceType=cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=wPageTableCount;

		//��������
		m_LogonSuccess.dwInoutIndex=m_GameDBAide.GetValue_DWORD(TEXT("InoutIndex"));

		//��ȡ��Ϣ
		if(pszErrorString!=NULL) lstrcpyn(m_LogonSuccess.szDescribeString,pszErrorString,CountArray(m_LogonSuccess.szDescribeString));
		
		//�������
		m_LogonSuccess.wTaskCount=0;
		ZeroMemory(m_LogonSuccess.UserTaskInfo,sizeof(m_LogonSuccess.UserTaskInfo));

		try
		{
			//�������
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
			m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_PlatformDBAide.AddParameter(TEXT("@strPassword"),m_LogonSuccess.szPassword);

			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�нű�
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTaskInfo"),true);

			//ִ�гɹ�
			if(lResultCode==DB_SUCCESS)
			{
				//��������
				tagUserTaskInfo * pUserTaskInfo=NULL;

				//ѭ����ȡ
				while (m_PlatformDBModule->IsRecordsetEnd()==false)
				{
					//���ñ���
					pUserTaskInfo = &m_LogonSuccess.UserTaskInfo[m_LogonSuccess.wTaskCount++];

					//��ȡ����
					pUserTaskInfo->wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
					pUserTaskInfo->cbTaskStatus = m_PlatformDBAide.GetValue_BYTE(TEXT("TaskStatus"));
					pUserTaskInfo->wTaskProgress = m_PlatformDBAide.GetValue_WORD(TEXT("Progress"));
					pUserTaskInfo->dwResidueTime = m_PlatformDBAide.GetValue_DWORD(TEXT("ResidueTime"));
					pUserTaskInfo->dwLastUpdateTime=(DWORD)time(NULL);

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
				}
			}
		}catch(...)
		{
			ASSERT(FALSE);
		}

		try
		{
			//�������
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
			m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_TreasureDBAide.AddParameter(TEXT("@strPassword"),m_LogonSuccess.szPassword);

			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�нű�
			LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserGameData"),true);

			//ִ�гɹ�
			if(lResultCode==DB_SUCCESS)
			{
				//��ȡ����
				m_TreasureDBAide.GetValue_String(TEXT("UserGameData"),m_LogonSuccess.szUserGameData,CountArray(m_LogonSuccess.szUserGameData));
			}
		}catch(...)
		{
			ASSERT(FALSE);
		}

		//���ͽ��
		WORD wDataSize=sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize=sizeof(m_LogonSuccess)-sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_LogonSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//��������
		LogonFailure.lResultCode=dwErrorCode;
		lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
	}

	return;
}

//���н��
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbActivityGame)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		DBO_GR_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//�������
		UserInsureSuccess.cbActivityGame=cbActivityGame;
		UserInsureSuccess.lFrozenedScore=lFrozenedScore;
		UserInsureSuccess.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserInsureSuccess.lSourceScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("SourceScore"));
		UserInsureSuccess.lSourceInsure=m_TreasureDBAide.GetValue_DOUBLE(TEXT("SourceInsure"));
		UserInsureSuccess.lInsureRevenue=m_TreasureDBAide.GetValue_DOUBLE(TEXT("InsureRevenue"));
		UserInsureSuccess.lVariationScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("VariationScore"));
		UserInsureSuccess.lVariationInsure=m_TreasureDBAide.GetValue_DOUBLE(TEXT("VariationInsure"));
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GR_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//�������
		UserInsureFailure.cbActivityGame=cbActivityGame;
		UserInsureFailure.lResultCode=dwErrorCode;
		UserInsureFailure.lFrozenedScore=lFrozenedScore;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//˽������
bool CDataBaseEngineSink::OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	return true;
}

//˽������
bool CDataBaseEngineSink::OnRequestWriteJoinInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		DBR_GR_WriteJoinInfo * pWriteJoinScore=(DBR_GR_WriteJoinInfo *)pData;
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteJoinInfo));
		if (wDataSize!=sizeof(DBR_GR_WriteJoinInfo)) return false;


		//�������
		m_PlatformDBAide.ResetParameter();

		//�û���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pWriteJoinScore->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwTableID"),pWriteJoinScore->dwTableID);
		m_PlatformDBAide.AddParameter(TEXT("@szRoomID"),pWriteJoinScore->szRoomID);
		
			
		OutputDebugStringA("ptdt ***** ִ��д ������Ϸ �洢����");
		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_WritePersonalRoomJoinInfo"),false);
	}
	catch(...)
	{
		TCHAR szInfo[260] = {0};
		wsprintf(szInfo, TEXT("OnRequestWriteJoinInfo �������д���") );
		CTraceService::TraceString(szInfo,TraceLevel_Exception);
	}


		return true;
}

//////////////////////////////////////////////////////////////////////////////////
