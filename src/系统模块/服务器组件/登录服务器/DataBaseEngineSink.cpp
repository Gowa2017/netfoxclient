#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��������
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
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

	try
	{
		//������Ϣ
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->szDataBaseAddr,pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//��������
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
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

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

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
	switch (wRequestID)
	{
	case DBR_GP_LOGON_GAMEID:			//��ʶ��¼
		{
			return OnRequestLogonGameID(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_ACCOUNTS:			//�ʺŵ�¼
		{
			return OnRequestLogonAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//ע���ʺ�
		{
			return OnRequestRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_PLATFORM_PARAMETER: //ƽ̨����
		{
			return OnRequestPlatformParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_VERIFY_INDIVIDUAL:		//��֤����
		{
			return OnRequestVerifyIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_VISITOR:			//�ο͵�½
		{
			return OnRequestLogonVisitor(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_MACHINE:			//�޸Ļ���
		{
			return OnRequestModifyMachine(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_LOGON_PASS:		//�޸�����
		{
			return OnRequestModifyLogonPass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INSURE_PASS:		//�޸�����
		{
			return OnRequestModifyInsurePass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_UNDER_WRITE:		//�޸�ǩ��
		{
			return OnRequestModifyUnderWrite(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_REAL_AUTH:		//ʵ����֤
		{
			return OnRequestModifyRealAuth(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_SYSTEM_FACE:		//�޸�ͷ��
		{
			return OnRequestModifySystemFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_CUSTOM_FACE:		//�޸�ͷ��
		{
			return OnRequestModifyCustomFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//�޸�����
		{
			return OnRequestModifyIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_ENABLE_INSURE:		//��ͨ����
		{
			return OnRequestUserEnableInsure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SAVE_SCORE:		//������Ϸ��
		{
			return OnRequestUserSaveScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_BANK_INFO:		//��ѯ�û���������Ϣ
		{
			return OnRequestQueryBankInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_BIND_BANK_INFO:		//�����п���Ϣ
		{
			return OnRequestBindBankInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_BANK_SCORE:	//�һ����
		{
			return OnRequestExchangeBankScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TAKE_SCORE:		//��ȡ��Ϸ��
		{
			return OnRequestUserTakeScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TRANSFER_SCORE:	//ת����Ϸ��
		{
			return OnRequestUserTransferScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INDIVIDUAL:		//��ѯ����
		{
			return OnRequestQueryIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INSURE_INFO:		//��ѯ����
		{
			return OnRequestQueryInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_USER_INFO:		//��ѯ�û�
		{
			return OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_TRANSFER_REBATE:		//��ѯ����
		{
			return OnRequestQueryTransferRebate(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_GAME_LIST:			//�����б�
		{
			return OnRequestLoadGameList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ONLINE_COUNT_INFO:		//������Ϣ
		{
			return OnRequestOnLineCountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_SERVER_DUMMY_ONLINE:	//������������
		{
			return OnRequestServerDummyOnLine(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_CHECKIN_REWARD:	//ǩ������
		{
			return OnRequestCheckInReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_TASK_LIST:			//��������
		{
			return OnRequestLoadTaskList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_BASEENSURE:		//���صͱ�
		{
			return OnRequestLoadBaseEnsure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_SPREAD_INFO:		//�ƹ���Ϣ
		{
			return OnRequestQuerySpreadInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_REAL_AUTH:			//������֤
		{
			return OnRequestLoadRealAuth(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GET_PARAMETER:			//��ȡ����
		{
			return OnRequestGetParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ADD_PARAMETER:			//��Ӳ���
		{
			return OnRequestAddParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_PARAMETER:		//�޸Ĳ���
		{
			return OnRequestModifyParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_DELETE_PARAMETER:		//ɾ������
		{
			return OnRequestDeleteParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKIN_QUERY_INFO:		//��ѯǩ��
		{
			return OnRequestCheckInQueryInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKIN_DONE:			//ִ��ǩ��
		{
			return OnRequestCheckInDone(dwContextID,pData,wDataSize);
		}
	case DBR_GP_TASK_QUERY_INFO:		//��ѯ����
		{
			return OnRequestTaskQueryInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_TASK_GIVEUP:			//��������
		{
			return OnRequestTaskGiveUp(dwContextID,pData,wDataSize);
		}
	case DBR_GP_TASK_TAKE:				//��ȡ����
		{
			return OnRequestTaskTake(dwContextID,pData,wDataSize);
		}
	case DBR_GP_TASK_REWARD:			//��ȡ����
		{
			return OnRequestTaskReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_BASEENSURE_TAKE:		//��ȡ�ͱ�
		{
			return OnRequestTakeBaseEnsure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GROWLEVEL_QUERY_IFNO:	//��ѯ�ȼ�
		{
			return OnRequestQueryGrowLevelParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_MEMBER_PARAMETER:	//��Ա����
		{
			return OnRequestMemberLoadParameter(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MEMBER_QUERY_INFO:	//��Ա��ѯ
		{
			return OnRequestMemberQueryInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MEMBER_DAY_PRESENT:	//��Ա����
		{
			return OnRequestMemberDayPresent(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MEMBER_DAY_GIFT:	//��Ա����
		{
			return OnRequestMemberDayGift(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_GROWLEVEL_CONFIG:	//�ȼ�����
		{
			return OnRequestLoadGrowLevelConfig(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_GAME_PROPERTY_LIST:	//���ص���
		{
			return OnRequestLoadGamePropertyList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_PROPERTY_BUY:			//�������
		{
			return OnRequestBuyGameProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GP_PROPERTY_USE:			//ʹ�õ���
		{
			return OnRequestUseProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_PROPERTY_BUFF:		//�������Buff
		{
			return OnRequestPropertyBuff(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_SINGLE:			 //��ѯ����
		{
			return OnRequestPropertyQuerySingle(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_BACKPACK:			 //��ѯ����
		{
			return OnRequestUserBackpackProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GP_PROPERTY_PRESENT:		//���͵���
		{
			return OnRequestPropertyPresent(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_SEND_PRESENT:		//��ѯ����
		{
			return OnRequestQuerySendPresent(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GET_SEND_PRESENT:		//��ȡ����
		{
			return OnRequestGetSendPresent(dwContextID,pData,wDataSize);
		}
	case DBR_GP_PURCHASE_MEMBER:		//�����Ա
		{
			return OnRequestPurchaseMember(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_SCORE_INGOT:	//�һ���Ϸ��
		{
			return OnRequestExchangeScoreByIngot(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_SCORE_BEANS:	//�һ���Ϸ��
		{
			return OnRequestExchangeScoreByBeans(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_GAMEID:			//��ʶ��¼
		{
			return OnMobileLogonGameID(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_ACCOUNTS:			//�ʺŵ�¼
		{
			return OnMobileLogonAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_MB_PHONE_REGISTER_ACCOUNTS://�ֻ���ע��
		{
			return OnMobilePhoneRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_MB_REGISTER_ACCOUNTS:		//ע���ʺ�
		{
			return OnMobileRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_MB_MODIFY_PASS:			//�޸��ֻ�����
		{
			return OnMobileModifyPass(dwContextID,pData,wDataSize);
		}
	case DBR_MB_MODIFY_BANKINFO:		//�޸�������Ϣ
		{
			return OnMobileModifyBankInfo(dwContextID,pData,wDataSize);
		}
	case DBR_MB_QUERY_USERDATA:			//��ѯ�û�����
		{
			return OnMobileQueryUserData(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_OTHERPLATFORM:	//������¼
		{
			return OnMobileLogonOtherPlatform(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_VISITOR:			//�ο͵�½
		{
			return OnMobileLogonVisitor(dwContextID,pData,wDataSize);
		}
	case DBR_MB_TOUSU_AGENT:			//Ͷ�ߴ���
		{
			return OnMobileTouSuAgent(dwContextID,pData,wDataSize);
		}
	case DBR_MB_SHENQING_AGENT_CHECK:
		{
			return OnMobileShenQingAgentCheck(dwContextID,pData,wDataSize);
		}
	case DBR_MB_SHENQING_AGENT:
		{
			return OnMobileShenQingAgent(dwContextID,pData,wDataSize);
		}
	case DBR_MB_QUERY_GAMELOCKINFO:
		{
			return OnMobileQueryGameLockInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOTTERY_CONFIG_REQ:		//��������
		{
			return OnRequestLotteryConfigReq(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOTTERY_START:			//�齱��ʼ
		{
			return OnRequestLotteryStart(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_USER_GAME_DATA:	//��Ϸ����
		{
			return OnRequestQueryUserGameData(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ACCOUNT_BIND:
		{
			return OnRequestAccountBind(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ACCOUNT_BIND_EXISTS:
		{
			return OnRequestAccountBindExists(dwContextID,pData,wDataSize);
		}
	case DBR_GP_IP_LOCATION:
		{
			return OnRequestIpLocation(dwContextID,pData,wDataSize);
		}
	case DBR_MB_GET_PERSONAL_PARAMETER: //˽�˷������
		{
			return OnRequestGetPersonalParameter(dwContextID,pData,wDataSize);
		}
	case DBR_MB_QUERY_PERSONAL_ROOM_INFO://˽�˷�����Ϣ
		{
			return OnRequestqueryPersonalRoomInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GR_QUERY_USER_ROOM_SCORE://˽�˷������
		{
			return OnRequestQueryUserRoomScore(dwContextID,pData,wDataSize);
		}
	case DBR_GR_CLOSE_ROOM_WRITE_DISSUME_TIME://д���ɢ˽�˷���ʱ��
		{
			return OnRequestCloseRoomWriteDissumeTime(dwContextID,pData,wDataSize);
		}
	case DBR_MB_QUERY_PERSONAL_ROOM_USER_INFO:
		{
			return OnRequestPersonalRoomUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_MB_ROOM_CARD_EXCHANGE_TO_SCORE:
		{
			return OnRequestRoomCardExchangeToScore(dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LogonGameID));
		if (wDataSize!=sizeof(DBR_GP_LogonGameID)) return false;

		//ִ�в�ѯ
		DBR_GP_LogonGameID * pLogonGameID=(DBR_GP_LogonGameID *)pData;

		//��������
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonGameID->cbNeeValidateMBCard);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,false);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonAccounts)) return false;

		//������
		DBR_GP_LogonAccounts * pLogonAccounts=(DBR_GP_LogonAccounts *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//��������
		LONG lResultCode=DB_SUCCESS;

		//�������
		TCHAR szDescribeString[128]=TEXT("");

		//���Ȩ��
		if(pLogonAccounts->dwCheckUserRight!=0)
		{
			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@dwcheckRight"),pLogonAccounts->dwCheckUserRight);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�й���
			lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckUserRight"),false);
		}

		//��֤�ʺ�
		if(lResultCode==DB_SUCCESS)
		{
			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
			m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
			m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonAccounts->cbNeeValidateMBCard);		
			m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pLogonAccounts->szPassPortID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�в�ѯ
			lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
		}

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),pLogonAccounts->dwCheckUserRight,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,false);

		return false;
	}

	return true;
}

//ע�ᴦ��
bool CDataBaseEngineSink::OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//������
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderGameID"),pRegisterAccounts->dwSpreaderGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pRegisterAccounts->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pRegisterAccounts->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentID"),pRegisterAccounts->szAgentID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,false);

		return false;
	}

	return true;
}

//��֤����
bool CDataBaseEngineSink::OnRequestVerifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_VerifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_VerifyIndividual)) return false;

		//������
		DBR_GP_VerifyIndividual * pVerifyIndividual=(DBR_GP_VerifyIndividual *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pVerifyIndividual->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wVerifyMask"),pVerifyIndividual->wVerifyMask);
		m_AccountsDBAide.AddParameter(TEXT("@strVerifyContent"),pVerifyIndividual->szVerifyContent);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_VerifyIndividual"),false);

		//�������
		DBO_GP_VerifyIndividualResult VerifyIndividualResult;
		VerifyIndividualResult.szErrorMsg[0]=0;
		VerifyIndividualResult.wVerifyMask=pVerifyIndividual->wVerifyMask;
		VerifyIndividualResult.bVerifyPassage=lResultCode==DB_SUCCESS;

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(VerifyIndividualResult.szErrorMsg,CW2CT(DBVarValue.bstrVal),CountArray(VerifyIndividualResult.szErrorMsg));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(VerifyIndividualResult.szErrorMsg);
		WORD wHeadSize=sizeof(VerifyIndividualResult)-sizeof(VerifyIndividualResult.szErrorMsg);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERIFY_RESULT,dwContextID,&VerifyIndividualResult,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		DBR_GP_VerifyIndividual * pVerifyIndividual=(DBR_GP_VerifyIndividual *)pData;

		//�������
		DBO_GP_VerifyIndividualResult VerifyIndividualResult;
		VerifyIndividualResult.bVerifyPassage=false;
		VerifyIndividualResult.wVerifyMask=pVerifyIndividual->wVerifyMask;		
		lstrcpyn(VerifyIndividualResult.szErrorMsg,TEXT("���ݿ�����쳣,��֤ʧ��!"),CountArray(VerifyIndividualResult.szErrorMsg));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(VerifyIndividualResult.szErrorMsg);
		WORD wHeadSize=sizeof(VerifyIndividualResult)-sizeof(VerifyIndividualResult.szErrorMsg);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERIFY_RESULT,dwContextID,&VerifyIndividualResult,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//�ο͵�¼
bool CDataBaseEngineSink::OnRequestLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LogonVisitor));
		if (wDataSize!=sizeof(DBR_GP_LogonVisitor)) return false;

		//������
		DBR_GP_LogonVisitor * pVisitorLogon=(DBR_GP_LogonVisitor *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pVisitorLogon->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pVisitorLogon->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pVisitorLogon->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pVisitorLogon->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyLogonVisitor"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,false);

		return false;
	}

	return true;
}

//I D ��¼
bool CDataBaseEngineSink::OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_LogonGameID));
		if (wDataSize!=sizeof(DBR_MB_LogonGameID)) return false;

		//ִ�в�ѯ
		DBR_MB_LogonGameID * pLogonGameID=(DBR_MB_LogonGameID *)pData;

		//��������
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonGameID->szMobilePhone);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyGameID"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_LogonAccounts));
		if (wDataSize!=sizeof(DBR_MB_LogonAccounts)) return false;

		//������
		DBR_MB_LogonAccounts * pLogonAccounts=(DBR_MB_LogonAccounts *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonAccounts->szMobilePhone);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

//������¼
bool CDataBaseEngineSink::OnMobileLogonOtherPlatform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_LogonOtherPlatform));
		if (wDataSize!=sizeof(DBR_MB_LogonOtherPlatform)) return false;

		//������
		DBR_MB_LogonOtherPlatform * pLogonOtherPlatform=(DBR_MB_LogonOtherPlatform *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonOtherPlatform->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonOtherPlatform->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pLogonOtherPlatform->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strUserUin"),pLogonOtherPlatform->szUserUin);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pLogonOtherPlatform->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pLogonOtherPlatform->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pLogonOtherPlatform->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonOtherPlatform->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonOtherPlatform->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentID"),pLogonOtherPlatform->szAgentID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyOtherPlatform"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

//�ο͵�¼
bool CDataBaseEngineSink::OnMobileLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_LogonVisitor));
		if (wDataSize!=sizeof(DBR_MB_LogonVisitor)) return false;

		//������
		DBR_MB_LogonVisitor * pVisitorLogon=(DBR_MB_LogonVisitor *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pVisitorLogon->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pVisitorLogon->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pVisitorLogon->cbPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pVisitorLogon->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pVisitorLogon->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentID"),pVisitorLogon->szAgentID);
		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyLogonVisitor"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnMobileTouSuAgent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_TouSuDaili));
		if (wDataSize!=sizeof(DBR_MB_TouSuDaili)) return false;

		//������
		DBR_MB_TouSuDaili * pDBRData=(DBR_MB_TouSuDaili *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strUserWx"),pDBRData->szUserWx);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentWx"),pDBRData->szAgentWx);
		m_AccountsDBAide.AddParameter(TEXT("@strReason"),pDBRData->szReason);
		m_AccountsDBAide.AddParameter(TEXT("@strImg"),pDBRData->szImageName);
		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_UserAccuseYS"),true);

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
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

//���������
bool CDataBaseEngineSink::OnMobileShenQingAgentCheck(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_ShenQingDailiCheck));
		if (wDataSize!=sizeof(DBR_MB_ShenQingDailiCheck)) return false;

		//������
		DBR_MB_ShenQingDailiCheck * pDBRData=(DBR_MB_ShenQingDailiCheck *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);
		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_UserApplyYsRight"),true);

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
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

//�������
bool CDataBaseEngineSink::OnMobileShenQingAgent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_ShenQingDaili));
		if (wDataSize!=sizeof(DBR_MB_ShenQingDaili)) return false;

		//������
		DBR_MB_ShenQingDaili * pDBRData=(DBR_MB_ShenQingDaili *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentAcc"),pDBRData->szAgentAcc);
		m_AccountsDBAide.AddParameter(TEXT("@strShowName"),pDBRData->szAgentName);
		m_AccountsDBAide.AddParameter(TEXT("@strAlipay"),pDBRData->szAlipay);
		m_AccountsDBAide.AddParameter(TEXT("@strWeChat"),pDBRData->szWeChat);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pDBRData->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strMemo"),pDBRData->szNote);
		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_UserApplyYs"),true);

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
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

//��ѯ��Ϸ������Ϣ
bool CDataBaseEngineSink::OnMobileQueryGameLockInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_QueryGameLockInfo));
		if (wDataSize!=sizeof(DBR_MB_QueryGameLockInfo)) return false;

		//������
		DBR_MB_QueryGameLockInfo * pDBRData=(DBR_MB_QueryGameLockInfo *)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("P_QueryOnlineUser"),true);

		//�������
		if (m_PlatformDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������m_PlatformDBAide.GetValue_WORD(TEXT("ItemQuota"));
			DBO_MB_QueryGameLockInfoResult DBOData;
			ZeroMemory(&DBOData,sizeof(DBOData));

			//�������
			DBOData.dwKindID=m_PlatformDBAide.GetValue_DWORD(_T("KindID"));
			DBOData.dwServerID=m_PlatformDBAide.GetValue_DWORD(_T("ServerID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_QUERY_GAMELOCKINFO_RESULT,dwContextID,&DBOData,sizeof(DBOData));
		}
		else
		{
			CTraceService::TraceString(_T("OnMobileQueryGameLockInfo δ�����쳣��"),TraceLevel_Warning);
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

bool  CDataBaseEngineSink::OnMobilePhoneRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_PhoneRegisterAccounts));
		if (wDataSize!=sizeof(DBR_MB_PhoneRegisterAccounts)) return false;

		//������
		DBR_MB_PhoneRegisterAccounts * pRegisterAccounts=(DBR_MB_PhoneRegisterAccounts *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderGameID"),pRegisterAccounts->dwSpreaderGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterAccounts->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pRegisterAccounts->cbDeviceType);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentID"),pRegisterAccounts->szAgentID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

//�ʺ�ע��
bool CDataBaseEngineSink::OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_MB_RegisterAccounts)) return false;

		//������
		DBR_MB_RegisterAccounts * pRegisterAccounts=(DBR_MB_RegisterAccounts *)pData;

		//ִ���ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderGameID"),pRegisterAccounts->dwSpreaderGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterAccounts->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@cbDeviceType"),pRegisterAccounts->cbDeviceType);
		m_AccountsDBAide.AddParameter(TEXT("@strAgentID"),pRegisterAccounts->szAgentID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),0,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),0,true);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnMobileModifyPass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_ModifyPass));
		if (wDataSize!=sizeof(DBR_MB_ModifyPass)) return false;

		//������
		DBR_MB_ModifyPass * pModifyPass=(DBR_MB_ModifyPass *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyPass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pModifyPass->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@IsLogon"),pModifyPass->cbMode);
		m_AccountsDBAide.AddParameter(TEXT("@Pwd"),pModifyPass->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyPass->szMobilePhone);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_Acc_SetPass"),true);

		if (lResultCode == DB_SUCCESS)
		{
			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//��ȡ��Ϣ
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�������
		OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
		lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnMobileModifyBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_ModifyBankInfo));
		if (wDataSize!=sizeof(DBR_MB_ModifyBankInfo)) return false;

		//������
		DBR_MB_ModifyBankInfo * pModifyPass=(DBR_MB_ModifyBankInfo *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyPass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strBankNO"),pModifyPass->szBankNo);
		m_AccountsDBAide.AddParameter(TEXT("@strBankName"),pModifyPass->szBankName);
		m_AccountsDBAide.AddParameter(TEXT("@strBankAddress"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_EditBankMsg"),true);

		if (lResultCode == DB_SUCCESS)
		{
			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//��ȡ��Ϣ
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�������
		OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
		lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnMobileQueryUserData(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_QueryUserData));
		if (wDataSize!=sizeof(DBR_MB_QueryUserData)) return false;

		//������
		DBR_MB_QueryUserData * pDBRData=(DBR_MB_QueryUserData *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@cbPlatformID"),pDBRData->cbType);
		m_AccountsDBAide.AddParameter(TEXT("@strWhere"),pDBRData->szWhere);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserID"),true);

		if (lResultCode == DB_SUCCESS)
		{
			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//��������
			DBO_GP_QueryUserDataResult DBOData;
			ZeroMemory(&DBOData,sizeof(DBOData));
			DBOData.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_USERDATA_RESULT,dwContextID,&DBOData,sizeof(DBOData));
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//��ȡ��Ϣ
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�������
		OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
		lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//�޸Ļ���
bool CDataBaseEngineSink::OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyMachine));
		if (wDataSize!=sizeof(DBR_GP_ModifyMachine)) return false;

		//������
		DBR_GP_ModifyMachine * pModifyMachine=(DBR_GP_ModifyMachine *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�󶨲���
		if (pModifyMachine->cbBind==TRUE)
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_MoorMachine"),false);
		}
		else
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnMoorMachine"),false);
		}

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�ͷ��
bool CDataBaseEngineSink::OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifySystemFace));
		if (wDataSize!=sizeof(DBR_GP_ModifySystemFace)) return false;

		//������
		DBR_GP_ModifySystemFace * pModifySystemFace=(DBR_GP_ModifySystemFace *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifySystemFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifySystemFace->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifySystemFace->wFaceID);

		//������Ϣ
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifySystemFace->szMachineID);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SystemFaceInsert"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//��ȡ��Ϣ
			UserFaceInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�ͷ��
bool CDataBaseEngineSink::OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyCustomFace));
		if (wDataSize!=sizeof(DBR_GP_ModifyCustomFace)) return false;

		//������
		DBR_GP_ModifyCustomFace * pModifyCustomFace=(DBR_GP_ModifyCustomFace *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyCustomFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//��������
		SAFEARRAYBOUND SafeArrayBound;
		ZeroMemory(&SafeArrayBound,sizeof(SafeArrayBound));

		//���ñ���
		SafeArrayBound.lLbound=0L;
		SafeArrayBound.cElements=sizeof(pModifyCustomFace->dwCustomFace);

		//��������
		BYTE * pcbCustomFace=(BYTE *)pModifyCustomFace->dwCustomFace;
		SAFEARRAY * pSafeArray=SafeArrayCreate(VT_UI1,1,&SafeArrayBound);

		//��������
		for (LONG nIndex=0;nIndex<sizeof(pModifyCustomFace->dwCustomFace);nIndex++)
		{
			SafeArrayPutElement(pSafeArray,&nIndex,pcbCustomFace+nIndex);
		}

		//���ñ���
		VARIANT VarChunk;
		VarChunk.parray=pSafeArray;
		VarChunk.vt=VT_ARRAY|VT_UI1;

		//��ȡ����
		ASSERT(m_AccountsDBModule.GetInterface()!=NULL);
		IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyCustomFace->szPassword);

		//ͷ����Ϣ
		pIDataBase->AddParameter(TEXT("@dwCustomFace"),adLongVarBinary,adParamInput,sizeof(pModifyCustomFace->dwCustomFace),CDBVarValue(VarChunk));

		//������Ϣ
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyCustomFace->szMachineID);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�ͷ���Դ
		SafeArrayDestroy(pSafeArray);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CustomFaceInsert"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//��ȡ��Ϣ
			UserFaceInfo.wFaceID=INVALID_WORD;
			UserFaceInfo.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//������
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyLogonPass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//������
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�ǩ��
bool CDataBaseEngineSink::OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyUnderWrite));
		if (wDataSize!=sizeof(DBR_GP_ModifyUnderWrite)) return false;

		//������
		DBR_GP_ModifyUnderWrite * pModifyUnderWrite=(DBR_GP_ModifyUnderWrite *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyUnderWrite->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyUnderWrite->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyUnderWrite->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyUnderWrite->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUnderWrite"),false);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestModifyRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_RealAuth));
		if (wDataSize!=sizeof(DBR_GP_RealAuth)) return false;

		//������
		DBR_GP_RealAuth * pDataPara=(DBR_GP_RealAuth *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDataPara->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDataPara->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDataPara->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pDataPara->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pDataPara->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RealAuth"),true);

		if (lResultCode == 0)
		{
			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			OnIndividualDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		}
		else
		{
			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//������
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyIndividual->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pModifyIndividual->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pModifyIndividual->szSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��ͨ����
bool CDataBaseEngineSink::OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_UserEnableInsure));
		if (wDataSize!=sizeof(DBR_GP_UserEnableInsure)) return false;

		//������
		DBR_GP_UserEnableInsure * pUserEnableInsure=(DBR_GP_UserEnableInsure *)pData;

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
		DBO_GP_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//���ñ���
		UserEnableInsureResult.cbInsureEnabled=(lResultCode==DB_SUCCESS)?TRUE:FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		DBO_GP_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//���ñ���
		UserEnableInsureResult.cbInsureEnabled=FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);

		return false;
	}
}

//������Ϸ��
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_UserSaveScore));
		if (wDataSize!=sizeof(DBR_GP_UserSaveScore)) return false;

		//������
		DBR_GP_UserSaveScore * pUserSaveScore=(DBR_GP_UserSaveScore *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		ShowLog(TEXT("%.2f---����Ľ��"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestQueryBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_QueryBankInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryBankInfo)) return false;

		//������
		DBR_GP_QueryBankInfo * pDBRData=(DBR_GP_QueryBankInfo *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_PlayerDrawInfo"),true);

		if (lResultCode == DB_SUCCESS)
		{
			DBO_GP_QueryBankInfoResult DBOData;
			ZeroMemory(&DBOData, sizeof(DBOData));

			//�������
			m_AccountsDBAide.GetValue_String(TEXT("RealName"), DBOData.szRealName, CountArray(DBOData.szRealName));
			m_AccountsDBAide.GetValue_String(TEXT("BankNo"), DBOData.szBankNo, CountArray(DBOData.szBankNo));
			m_AccountsDBAide.GetValue_String(TEXT("AlipayID"), DBOData.szAlipayID, CountArray(DBOData.szAlipayID));
			DBOData.lDrawScore=m_AccountsDBAide.GetValue_DOUBLE(TEXT("drawScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_BANKINFO_SUCCESS,dwContextID,&DBOData,sizeof(DBOData));
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

bool CDataBaseEngineSink::OnRequestBindBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_BindBankInfo));
		if (wDataSize!=sizeof(DBR_GP_BindBankInfo)) return false;


		//������
		DBR_GP_BindBankInfo * pDBRData=(DBR_GP_BindBankInfo *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szCompellation"),pDBRData->szRealName);
		m_AccountsDBAide.AddParameter(TEXT("@szBankNO"),pDBRData->szBankNo);
		m_AccountsDBAide.AddParameter(TEXT("@szBankName"),pDBRData->szBankName);
		m_AccountsDBAide.AddParameter(TEXT("@szBankAddress"),pDBRData->szBankAddress);
		m_AccountsDBAide.AddParameter(TEXT("@szAlipayID"),pDBRData->szAlipayID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_Acc_PLayerBindBank"),true);

		//�������
		//��ȡ����
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
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

//�һ����
bool CDataBaseEngineSink::OnRequestExchangeBankScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ExchangeBankScore));
		if (wDataSize!=sizeof(DBR_GP_ExchangeBankScore)) return false;

		//������
		DBR_GP_ExchangeBankScore * pDBRData=(DBR_GP_ExchangeBankScore *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pDBRData->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);


		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDBRData->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwType"),pDBRData->dwType);
		m_AccountsDBAide.AddParameter(TEXT("@dwScore"),pDBRData->dwScore);
		m_AccountsDBAide.AddParameter(TEXT("@strSafePwd"),pDBRData->szBankPass);
		m_AccountsDBAide.AddParameter(TEXT("@strOrderID"),pDBRData->szOrderID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);


		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("P_Acc_PlayerDraw"),true);

		//�������
		//��ȡ����
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
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

//��ȡ��Ϸ��
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_UserTakeScore));
		if (wDataSize!=sizeof(DBR_GP_UserTakeScore)) return false;

		//������
		DBR_GP_UserTakeScore * pUserTakeScore=(DBR_GP_UserTakeScore *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//ת����Ϸ��
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_UserTransferScore));
		if (wDataSize!=sizeof(DBR_GP_UserTransferScore)) return false;

		//������
		DBR_GP_UserTransferScore * pUserTransferScore=(DBR_GP_UserTransferScore *)pData;

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
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLotteryConfigReq(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LotteryConfigReq));
		if (wDataSize!=sizeof(DBR_GP_LotteryConfigReq)) return false;

		//������
		DBR_GP_LotteryConfigReq * pLotteryConfigReq=(DBR_GP_LotteryConfigReq *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLotteryConfigReq->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryConfigReq->wKindID);


		//�齱����
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadLotteryConfig"),true);
		ASSERT(lResultCode==DB_SUCCESS);
		if (lResultCode==DB_SUCCESS)
		{
			//������
			DBO_GP_LotteryConfig LotteryConfig;
			ZeroMemory(&LotteryConfig,sizeof(LotteryConfig));
			
			//��������
			WORD wItemCount=0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				LotteryConfig.LotteryItem[wItemCount].cbItemIndex = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemIndex"));
				LotteryConfig.LotteryItem[wItemCount].cbItemType = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemType"));
				LotteryConfig.LotteryItem[wItemCount].lItemQuota = m_PlatformDBAide.GetValue_WORD(TEXT("ItemQuota"));
				wItemCount++;

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			LotteryConfig.wLotteryCount = wItemCount;

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_CONFIG,dwContextID,&LotteryConfig,sizeof(LotteryConfig));
		}
		else
		{
			CTraceService::TraceString(TEXT("�齱���ü���ʧ�ܣ�"),TraceLevel_Exception);
		}

		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryConfigReq->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pLotteryConfigReq->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strLogonPass"),pLotteryConfigReq->szLogonPass);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�齱��Ϣ
		lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LotteryUserInfo"),true);
		if (lResultCode==DB_SUCCESS)
		{
			//������
			DBO_GP_LotteryUserInfo LotteryUserInfo;
			ZeroMemory(&LotteryUserInfo,sizeof(LotteryUserInfo));

			LotteryUserInfo.wKindID = pLotteryConfigReq->wKindID;
			LotteryUserInfo.dwUserID = pLotteryConfigReq->dwUserID;
			LotteryUserInfo.cbFreeCount = m_PlatformDBAide.GetValue_BYTE(TEXT("FreeCount"));
			LotteryUserInfo.cbAlreadyCount = m_PlatformDBAide.GetValue_BYTE(TEXT("AlreadyCount"));
			LotteryUserInfo.lChargeFee = m_PlatformDBAide.GetValue_DOUBLE(TEXT("ChargeFee"));

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_USER_INFO,dwContextID,&LotteryUserInfo,sizeof(LotteryUserInfo));
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_PlatformDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�齱��ʼ
bool CDataBaseEngineSink::OnRequestLotteryStart(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LotteryStart));
		if (wDataSize!=sizeof(DBR_GP_LotteryStart)) return false;

		//������
		DBR_GP_LotteryStart * pLotteryStart=(DBR_GP_LotteryStart *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLotteryStart->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),pLotteryStart->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pLotteryStart->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strLogonPass"),pLotteryStart->szLogonPass);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pLotteryStart->szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LotteryStart"),true);

		if (lResultCode==DB_SUCCESS)
		{
			//������
			DBO_GP_LotteryResult LotteryResult;
			ZeroMemory(&LotteryResult,sizeof(LotteryResult));

			LotteryResult.wKindID = pLotteryStart->wKindID;
			LotteryResult.dwUserID = pLotteryStart->dwUserID;
			LotteryResult.bWined = (m_PlatformDBAide.GetValue_BYTE(TEXT("Wined"))==TRUE);
			LotteryResult.lUserScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserScore"));
			LotteryResult.dUserBeans = m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserBeans"));
			LotteryResult.LotteryItem.cbItemIndex = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemIndex"));
			LotteryResult.LotteryItem.cbItemType = m_PlatformDBAide.GetValue_BYTE(TEXT("ItemType"));
			LotteryResult.LotteryItem.lItemQuota = m_PlatformDBAide.GetValue_DOUBLE(TEXT("ItemQuota"));

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(LotteryResult));
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_PlatformDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��Ϸ����
bool CDataBaseEngineSink::OnRequestQueryUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_QueryUserGameData));
		if (wDataSize!=sizeof(DBR_GP_QueryUserGameData)) return false;

		//������
		DBR_GP_QueryUserGameData * pQueryUserGameData=(DBR_GP_QueryUserGameData *)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),pQueryUserGameData->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryUserGameData->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strDynamicPass"),pQueryUserGameData->szDynamicPass);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserGameData"),true);

		if (lResultCode==DB_SUCCESS)
		{
			//������
			DBO_GP_QueryUserGameData QueryUserGameData;
			ZeroMemory(&QueryUserGameData,sizeof(QueryUserGameData));

			QueryUserGameData.wKindID=pQueryUserGameData->wKindID;
			QueryUserGameData.dwUserID=pQueryUserGameData->dwUserID;
			m_TreasureDBAide.GetValue_String(TEXT("UserGameData"),QueryUserGameData.szUserGameData,CountArray(QueryUserGameData.szUserGameData));

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_USER_GAME_DATA,dwContextID,&QueryUserGameData,sizeof(QueryUserGameData));
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_TreasureDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//˽�˷�������
bool CDataBaseEngineSink::OnRequestGetPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_GetPersonalParameter));
		if (wDataSize!=sizeof(DBR_MB_GetPersonalParameter)) return false;



		//������
		DBR_MB_GetPersonalParameter * pGetPersonalParameter=(DBR_MB_GetPersonalParameter *)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwServerID"), pGetPersonalParameter->dwServerID);


		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		LONG lResultCode = 0;
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadPersonalRoomParameter"), true);


		if(lResultCode == DB_SUCCESS)
		{
			tagPersonalRoomOption  pPersonalRoomOption;
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
			pPersonalRoomOption.dwPlayTurnCount =  m_PlatformDBAide.GetValue_LONGLONG(TEXT("PlayTurnCount"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_PARAMETER,dwContextID,&pPersonalRoomOption,sizeof(tagPersonalRoomOption));
		}
		else
		{
			//������Ϣ
			CTraceService::TraceString(TEXT("���ݿ��쳣,���Ժ����ԣ�  "),TraceLevel_Exception);

			//�������
			OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);
		}



		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwKindID"), pGetPersonalParameter->dwServerID);

		//�������
		//TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalFeeParameter"), true);

		if(lResultCode == DB_SUCCESS)
		{
			//��������
			DWORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagPersonalTableFeeList * pPersonalTableParameter=NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((dwPaketSize+sizeof(tagPersonalTableFeeList))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_FEE_LIST,dwContextID,cbBuffer,dwPaketSize);
					ZeroMemory(cbBuffer, sizeof(cbBuffer));
					dwPaketSize=0;
				}

				//��ȡ��Ϣ
				pPersonalTableParameter=(tagPersonalTableFeeList *)(cbBuffer+dwPaketSize);
				pPersonalTableParameter->dwKindID = m_PlatformDBAide.GetValue_DWORD(TEXT("KindID"));
				pPersonalTableParameter->dwDrawCountLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("DrawCountLimit"));
				pPersonalTableParameter->dwDrawTimeLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("DrawTimeLimit"));
				pPersonalTableParameter->lFeeScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("TableFee"));
				pPersonalTableParameter->lIniScore = m_PlatformDBAide.GetValue_DOUBLE(TEXT("IniScore"));

				//����λ��
				dwPaketSize+=sizeof(tagPersonalTableFeeList);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			if (dwPaketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_FEE_LIST,dwContextID,cbBuffer,dwPaketSize);
			}
		}
		else
		{
			//������Ϣ
			CTraceService::TraceString(TEXT("���ݿ��쳣,���Ժ����ԣ�"),TraceLevel_Exception);

			//�������
			OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);
		}
		return true;

	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);
	}

	return true;
}

__int64 TimeDiff(SYSTEMTIME left, SYSTEMTIME right)    
{  
	CTime tmLeft(left.wYear, left.wMonth, left.wDay, 0, 0, 0);  
	CTime tmRight(right.wYear, right.wMonth, right.wDay, 0, 0, 0);  

	CTimeSpan sp = tmLeft - tmRight;  
	long MillisecondsL = (left.wHour*3600 + left.wMinute*60 + left.wSecond)*1000 + left.wMilliseconds;    
	long MillisecondsR = (right.wHour*3600 + right.wMinute*60 + right.wSecond)*1000 + right.wMilliseconds;    

	return  (__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR);//�˴����غ���  
} 

//��ɢ����
bool CDataBaseEngineSink::OnRequsetDissumeRoomTest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTableResult));
	if(wDataSize != sizeof(CMD_CS_C_DismissTableResult)) return false;
	CMD_CS_C_DismissTableResult temp;
	CMD_CS_C_DismissTableResult* pDissumCreateTable = &temp;
	lstrcpyn(pDissumCreateTable->PersonalTableInfo.szRoomID, TEXT("305680"), 7 * sizeof(TCHAR));

	pDissumCreateTable->PersonalUserScoreInfo[0].lTaxCount = 100;
	pDissumCreateTable->PersonalUserScoreInfo[0].lScore = 1000;
	pDissumCreateTable->PersonalUserScoreInfo[0].dwUserID = 1;
	lstrcpyn(pDissumCreateTable->PersonalUserScoreInfo[0].szUserNicname, TEXT("4568987"), 20);

	pDissumCreateTable->PersonalUserScoreInfo[1].lTaxCount = 1111;
	pDissumCreateTable->PersonalUserScoreInfo[1].lScore = 11111;
	pDissumCreateTable->PersonalUserScoreInfo[1].dwUserID = 1;
	lstrcpyn(pDissumCreateTable->PersonalUserScoreInfo[1].szUserNicname, TEXT("11111"), 20);
	//�������
	m_PlatformDBAide.ResetParameter();
	m_PlatformDBAide.AddParameter(TEXT("@RoomID"), pDissumCreateTable->PersonalTableInfo.szRoomID);
	m_PlatformDBAide.AddParameter(TEXT("@lTaxCount"), pDissumCreateTable->PersonalUserScoreInfo->lTaxCount);

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




	//�ͷ���Դ
	SafeArrayDestroy(pSafeArray);
	//DBO_GR_DissumeResult  DissumeResult;
	//ZeroMemory(&DissumeResult, 0, sizeof(DBO_GR_DissumeResult) );

	//�������
	LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_DissumePersonalRoom"),true);
	//DissumeResult.dwUserID;
	//if (lResultCode == DB_SUCCESS)
	//{
	//	//DissumeResult.dwSocketID = pDissumCreateTable->dwSocketID;
	//	//DissumeResult.cbIsDissumSuccess = 1;
	//	//lstrcpyn(DissumeResult.szRoomID, pDissumCreateTable->PersonalTableInfo.szRoomID, sizeof(DissumeResult.szRoomID));
	//	//m_PlatformDBAide.GetValue_SystemTime(TEXT("DissumeDate"), DissumeResult.sysDissumeTime);

	//	//��ȡ������ҵ�
	//	for (int i = 0; i < PERSONAL_ROOM_CHAIR; i++)
	//	{
	//		memcpy(&DissumeResult.PersonalUserScoreInfo[i],  &pDissumCreateTable->PersonalUserScoreInfo[i],  sizeof(tagPersonalUserScoreInfo));
	//	}
	//}
	//else
	//{
	//	DissumeResult.dwSocketID = pDissumCreateTable->dwSocketID;
	//	DissumeResult.cbIsDissumSuccess = 0;
	//}

	//���Ͳ���
	//m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DISSUME_TABLE_RESULTE,dwContextID,&DissumeResult,sizeof(DBO_GR_DissumeResult));
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

bool CDataBaseEngineSink::OnRequestqueryPersonalRoomInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(CMD_CS_C_HostCreatRoomInfo));
		if (wDataSize!=sizeof(CMD_CS_C_HostCreatRoomInfo)) return false;

		//������

		CMD_CS_C_HostCreatRoomInfo * pGetParameter=(CMD_CS_C_HostCreatRoomInfo *)pData;
		//pGetParameter = &temp;
		DBO_MB_PersonalRoomInfoList  PersonalRoomInfoList;
		memset(&PersonalRoomInfoList, 0 , sizeof(DBO_MB_PersonalRoomInfoList));

		//���id
		PersonalRoomInfoList.dwUserID = pGetParameter->HostCreatRoomInfo.dwUserID;

		TCHAR szRoomID[7] = {0};
		int iCount = 0;
		for (int i = 0; i < MAX_CREATE_PERSONAL_ROOM; i++)
		{
			if(pGetParameter->HostCreatRoomInfo.szRoomID[i][0] == 0)
			{
				continue;
			}
			iCount = i;
			lstrcpyn(szRoomID, pGetParameter->HostCreatRoomInfo.szRoomID[i], sizeof(szRoomID));

			//�������
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@szRoomID"), szRoomID);



			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�в�ѯ
			LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomInfo"),true);
			if (DB_SUCCESS == lResultCode)
			{
				TCHAR szInfo[520] = {0};


				lstrcpyn(PersonalRoomInfoList.PersonalRoomInfo[i].szRoomID, pGetParameter->HostCreatRoomInfo.szRoomID[i], CountArray(PersonalRoomInfoList.PersonalRoomInfo[i].szRoomID));
				int x = m_TreasureDBAide.GetValue_BYTE(TEXT("CardOrBean"));
				PersonalRoomInfoList.PersonalRoomInfo[i].cbCardOrBean = m_TreasureDBAide.GetValue_BYTE(TEXT("CardOrBean"));
				PersonalRoomInfoList.PersonalRoomInfo[i].lFeeCardOrBeanCount = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CreateTableFee"));
				PersonalRoomInfoList.PersonalRoomInfo[i].dwPlayTimeLimit =  m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTimeLimit") );
				PersonalRoomInfoList.PersonalRoomInfo[i].dwPlayTurnCount = m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTurnCount"));
				PersonalRoomInfoList.PersonalRoomInfo[i].cbIsDisssumRoom = m_TreasureDBAide.GetValue_BYTE(TEXT("cbIsDisssumRoom"));
				m_TreasureDBAide.GetValue_SystemTime(TEXT("sysCreateTime"), PersonalRoomInfoList.PersonalRoomInfo[i].sysCreateTime);
				m_TreasureDBAide.GetValue_SystemTime(TEXT("sysDissumeTime"), PersonalRoomInfoList.PersonalRoomInfo[i].sysDissumeTime);
				PersonalRoomInfoList.PersonalRoomInfo[i].lTaxCount = m_TreasureDBAide.GetValue_LONGLONG(TEXT("lTaxCount"));

				//��ȡ���������ݲ�ת��Ϊ��Ӧ���ݽṹ
				byte szTempBinary[1858] = {0};
				char szTempChar[3712] = {0};
				ZeroMemory(szTempChar,  sizeof(szTempChar));
				ZeroMemory(szTempBinary, sizeof(szTempBinary));

				m_TreasureDBAide.GetValue_String(TEXT("strRoomScoreInfo"),  szTempChar, 3712);
				HexStr2CharStr( szTempChar, sizeof(szTempChar),  szTempBinary);
				
				
				for(int j = 0; j < PERSONAL_ROOM_CHAIR; j++)
				{
					memcpy(&PersonalRoomInfoList.PersonalRoomInfo[i].PersonalUserScoreInfo[j], szTempBinary + j * sizeof(tagPersonalUserScoreInfo), sizeof(tagPersonalUserScoreInfo));
				}

			}
		}

		//����ʱ������
		for (int i = 0; i <= iCount; i++)
		{
			for (int j = i + 1 ; j <= iCount; j++  )
			{
				if (TimeDiff(PersonalRoomInfoList.PersonalRoomInfo[i].sysCreateTime,  PersonalRoomInfoList.PersonalRoomInfo[j].sysCreateTime) > 0)
				{
					tagPersonalRoomInfo temp;
					memset(&temp, 0 , sizeof(tagPersonalRoomInfo));
					memcpy(&temp,  &PersonalRoomInfoList.PersonalRoomInfo[i],  sizeof(tagPersonalRoomInfo));
					memcpy(&PersonalRoomInfoList.PersonalRoomInfo[i], &PersonalRoomInfoList.PersonalRoomInfo[j],  sizeof(tagPersonalRoomInfo));
					memcpy(&PersonalRoomInfoList.PersonalRoomInfo[j],  &temp,  sizeof(tagPersonalRoomInfo));

				}
			}
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_PERSONAL_ROOM_LIST,dwContextID, &PersonalRoomInfoList, sizeof(PersonalRoomInfoList));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);

		return false;
	}
}

//д�뷿�����ʱ��
bool CDataBaseEngineSink::OnRequestCloseRoomWriteDissumeTime(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{
		DBR_GR_CLOSE_ROOM_SERVER_ID * pCloseRoomID = (DBR_GR_CLOSE_ROOM_SERVER_ID *)pData;
		ASSERT(wDataSize==sizeof(DBR_GR_CLOSE_ROOM_SERVER_ID));
		if (wDataSize!=sizeof(DBR_GR_CLOSE_ROOM_SERVER_ID)) return false;
		//�������
		m_PlatformDBAide.ResetParameter();

		//�û���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@dwServerID"),pCloseRoomID->dwServerID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_CloseRoomWriteDissumeTime"),true);

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return true;
	}

}

//˽������
bool CDataBaseEngineSink::OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	try
	{

		DBR_GR_QUERY_USER_ROOM_INFO * pQueryUserRoomInfo = (DBR_GR_QUERY_USER_ROOM_INFO *)pData;


		//�������
		m_PlatformDBAide.ResetParameter();

		//�û���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryUserRoomInfo->dwUserID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_GetPersonalRoomUserScore"),true);

		if(lResultCode == DB_SUCCESS)
		{
			TCHAR szInfo[260] = {0};
			//��������
			DWORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagQueryPersonalRoomUserScore * pPersonalRoomInfo=NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
			int nCount = 0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				if(nCount >= MAX_CREATE_PERSONAL_ROOM  ||  (dwPaketSize + sizeof(tagQueryPersonalRoomUserScore))  >= 16385)
				{
					break;
				}
				//��������Ϣ��Ϊ�ƶ���������Ϣ
				TCHAR szRoomID[ROOM_ID_LEN] = {0};
				ZeroMemory(szRoomID, sizeof(szRoomID));
				m_PlatformDBAide.GetValue_String(TEXT("RoomID"), szRoomID, sizeof(szRoomID));

				if (szRoomID == NULL || szRoomID[0] == 0)
				{
					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
					continue;
				}


				//��ȡ��Ӧ�ķ����¼
				m_TreasureDBAide.ResetParameter();
				m_TreasureDBAide.AddParameter(TEXT("@szRoomID"), szRoomID);

				//��ȡ��Ϣ
				pPersonalRoomInfo=(tagQueryPersonalRoomUserScore *)(cbBuffer+dwPaketSize);

				//�������
				TCHAR szDescribeString[128]=TEXT("");
				m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

				//ִ�в�ѯ
				LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomInfo"),true);
				if (DB_SUCCESS == lResultCode)
				{
					lstrcpyn(pPersonalRoomInfo->szRoomID, szRoomID, CountArray(pPersonalRoomInfo->szRoomID));
					m_TreasureDBAide.GetValue_String(TEXT("UserNicname") , pPersonalRoomInfo->szUserNicname, sizeof(pPersonalRoomInfo->szUserNicname));
					pPersonalRoomInfo->dwPlayTimeLimit =  m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTimeLimit") );
					pPersonalRoomInfo->dwPlayTurnCount = m_TreasureDBAide.GetValue_DWORD(TEXT("dwPlayTurnCount"));
					byte cbIsDisssumRoom = m_TreasureDBAide.GetValue_BYTE(TEXT("cbIsDisssumRoom"));
					m_TreasureDBAide.GetValue_SystemTime(TEXT("sysCreateTime"), pPersonalRoomInfo->sysCreateTime);
					m_TreasureDBAide.GetValue_SystemTime(TEXT("sysDissumeTime"), pPersonalRoomInfo->sysDissumeTime);
					
					//�������δ��ɢ �� ����ʱ�� ��0
					if (!cbIsDisssumRoom)
					{
						memset(&(pPersonalRoomInfo->sysDissumeTime),  0, sizeof(pPersonalRoomInfo->sysDissumeTime));
					}
					
		


		
					//if (cbIsDisssumRoom)
					{
						//��ȡ���������ݲ�ת��Ϊ��Ӧ���ݽṹ
						byte szTempBinary[1858] = {0};
						char szTempChar[3712] = {0};
						ZeroMemory(szTempChar, sizeof(szTempChar));
						ZeroMemory(szTempBinary, sizeof(szTempBinary));

						m_TreasureDBAide.GetValue_String(TEXT("strRoomScoreInfo"),  szTempChar, 3712);
						HexStr2CharStr( szTempChar, sizeof(szTempChar),  szTempBinary);

						//
						TCHAR szInfo[520] = {0};
						for(int j = 0; j < PERSONAL_ROOM_CHAIR; j++)
						{
							memcpy(&pPersonalRoomInfo->PersonalUserScoreInfo[j],  szTempBinary + j * sizeof(tagPersonalUserScoreInfo), sizeof(tagPersonalUserScoreInfo));

						}	

						//����λ��
						dwPaketSize+=sizeof(tagQueryPersonalRoomUserScore);
						nCount++;
					}
				}

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_USER_ROOM_SCORE,dwContextID,cbBuffer,dwPaketSize);
		}

	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
	}

	return true;
}

//�ʺŰ�
bool CDataBaseEngineSink::OnRequestAccountBind(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_AccountBind));
		if (wDataSize!=sizeof(DBR_GP_AccountBind)) return false;

		//������
		DBR_GP_AccountBind * pModifyMachine=(DBR_GP_AccountBind *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		m_AccountsDBAide.AddParameter(TEXT("@strBindAccounts"),pModifyMachine->szBindNewAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strBindPassword"),pModifyMachine->szBindNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strBindSpreader"),pModifyMachine->szBindNewSpreader);


		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AccountBind"),false);

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//�ʺŰ�
bool CDataBaseEngineSink::OnRequestAccountBindExists(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_AccountBind_Exists));
		if (wDataSize!=sizeof(DBR_GP_AccountBind_Exists)) return false;

		//������
		DBR_GP_AccountBind_Exists * pModifyMachine=(DBR_GP_AccountBind_Exists *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		m_AccountsDBAide.AddParameter(TEXT("@strBindAccounts"),pModifyMachine->szBindExistsAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strBindPassword"),pModifyMachine->szBindExistsPassword);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AccountBindExists"),false);

		//�������
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//IP������
bool CDataBaseEngineSink::OnRequestIpLocation(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_IpLocation));
		if (wDataSize!=sizeof(DBR_GP_IpLocation)) return false;

		//������
		DBR_GP_IpLocation * pIpAddr=(DBR_GP_IpLocation *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pIpAddr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pIpAddr->szLocation);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("P_EditPlayerIP"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

}


//��ȡ����
bool CDataBaseEngineSink::OnRequestGetParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_GetParameter));
		if (wDataSize!=sizeof(DBR_GP_GetParameter)) return false;

		//������
		DBR_GP_GetParameter * pGetParameter=(DBR_GP_GetParameter *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"),pGetParameter->wServerID);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidGetParameter"),true);

		//ִ�н��
		OnAndroidDisposeResult(dwContextID,lResultCode,SUB_GP_GET_PARAMETER,pGetParameter->wServerID);		
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);

		return false;
	}

	return true;
}

//��Ӳ���
bool CDataBaseEngineSink::OnRequestAddParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_AddParameter));
		if (wDataSize!=sizeof(DBR_GP_AddParameter)) return false;

		//������
		DBR_GP_AddParameter * pAddParameter=(DBR_GP_AddParameter *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"),pAddParameter->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"),pAddParameter->AndroidParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCount"),pAddParameter->AndroidParameter.dwAndroidCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"),pAddParameter->AndroidParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"),pAddParameter->AndroidParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"),pAddParameter->AndroidParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"),pAddParameter->AndroidParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"),pAddParameter->AndroidParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"),pAddParameter->AndroidParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"),pAddParameter->AndroidParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"),pAddParameter->AndroidParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"),pAddParameter->AndroidParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"),pAddParameter->AndroidParameter.dwSwitchMaxInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember0"),pAddParameter->AndroidParameter.AndroidCountMember0);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember1"),pAddParameter->AndroidParameter.AndroidCountMember1);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember2"),pAddParameter->AndroidParameter.AndroidCountMember2);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember3"),pAddParameter->AndroidParameter.AndroidCountMember3);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember4"),pAddParameter->AndroidParameter.AndroidCountMember4);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember5"),pAddParameter->AndroidParameter.AndroidCountMember5);
		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidAddParameter"),true);

		//ִ�н��
		OnAndroidDisposeResult(dwContextID,lResultCode,SUB_GP_ADD_PARAMETER,pAddParameter->wServerID);
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);

		return false;
	}

	return true;
}

//�޸Ĳ���
bool CDataBaseEngineSink::OnRequestModifyParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyParameter));
		if (wDataSize!=sizeof(DBR_GP_ModifyParameter)) return false;

		//������
		DBR_GP_ModifyParameter * pModifyParameter=(DBR_GP_ModifyParameter *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwDatchID"),pModifyParameter->AndroidParameter.dwBatchID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"),pModifyParameter->AndroidParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCount"),pModifyParameter->AndroidParameter.dwAndroidCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"),pModifyParameter->AndroidParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"),pModifyParameter->AndroidParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"),pModifyParameter->AndroidParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"),pModifyParameter->AndroidParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"),pModifyParameter->AndroidParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"),pModifyParameter->AndroidParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"),pModifyParameter->AndroidParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"),pModifyParameter->AndroidParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"),pModifyParameter->AndroidParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"),pModifyParameter->AndroidParameter.dwSwitchMaxInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember0"),pModifyParameter->AndroidParameter.AndroidCountMember0);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember1"),pModifyParameter->AndroidParameter.AndroidCountMember1);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember2"),pModifyParameter->AndroidParameter.AndroidCountMember2);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember3"),pModifyParameter->AndroidParameter.AndroidCountMember3);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember4"),pModifyParameter->AndroidParameter.AndroidCountMember4);
		m_AccountsDBAide.AddParameter(TEXT("@dwAndroidCountMember5"),pModifyParameter->AndroidParameter.AndroidCountMember5);
		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidModifyParameter"),true);

		//ִ�н��
		OnAndroidDisposeResult(dwContextID,lResultCode,SUB_GP_MODIFY_PARAMETER,pModifyParameter->wServerID);
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);

		return false;
	}

	return true;
}

//ɾ������
bool CDataBaseEngineSink::OnRequestDeleteParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_DeleteParameter));
		if (wDataSize!=sizeof(DBR_GP_DeleteParameter)) return false;

		//������
		DBR_GP_DeleteParameter * pDeleteParameter=(DBR_GP_DeleteParameter *)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwBatchID"),pDeleteParameter->dwBatchID);

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AndroidDeleteParameter"),true);

		//ִ�н��
		OnAndroidDisposeResult(dwContextID,lResultCode,SUB_GP_DELETE_PARAMETER,pDeleteParameter->wServerID);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("���ݿ��쳣,���Ժ����ԣ�"),false,false);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_QueryIndividual));
		if (wDataSize!=sizeof(DBR_GP_QueryIndividual)) return false;

		//������
		DBR_GP_QueryIndividual * pQueryIndividual=(DBR_GP_QueryIndividual *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pQueryIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GP_UserIndividual UserIndividual;
			ZeroMemory(&UserIndividual,sizeof(UserIndividual));

			//�û���Ϣ
			UserIndividual.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserIndividual.szUserNote,CountArray(UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserIndividual.szCompellation,CountArray(UserIndividual.szCompellation));
			m_AccountsDBAide.GetValue_String(TEXT("PassPortID"),UserIndividual.szPassPortID,CountArray(UserIndividual.szPassPortID));

			//�绰����
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.szSeatPhone,CountArray(UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.szMobilePhone,CountArray(UserIndividual.szMobilePhone));

			//��ϵ����
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.szQQ,CountArray(UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.szEMail,CountArray(UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.szDwellingPlace,CountArray(UserIndividual.szDwellingPlace));
	
			//�ƹ���Ϣ
			m_AccountsDBAide.GetValue_String(TEXT("Spreader"),UserIndividual.szSpreader,CountArray(UserIndividual.szSpreader));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL,dwContextID,&UserIndividual,sizeof(UserIndividual));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryInsureInfo)) return false;

		//������
		DBR_GP_QueryInsureInfo * pQueryInsureInfo=(DBR_GP_QueryInsureInfo *)pData;

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
			DBO_GP_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//������Ϣ
			UserInsureInfo.cbEnjoinTransfer=m_TreasureDBAide.GetValue_BYTE(TEXT("EnjoinTransfer"));
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wRevenueTransferMember=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransferMember"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_DOUBLE(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_DOUBLE(TEXT("Insure"));
			ShowLog(TEXT("%.2f---�û����"),UserInsureInfo.lUserScore);
			ShowLog(TEXT("%.2f---���н��"),UserInsureInfo.lUserInsure);
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_DOUBLE(TEXT("TransferPrerequisite"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��ѯ�û�
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureUserInfo));
	if (wDataSize!=sizeof(DBR_GP_QueryInsureUserInfo)) return false;

	//������
	DBR_GP_QueryInsureUserInfo * pQueryTransferUserInfo=(DBR_GP_QueryInsureUserInfo *)pData;

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
			DBO_GP_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//������Ϣ
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("Accounts"), TransferUserInfo.szAccounts, CountArray(TransferUserInfo.szAccounts));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestQueryTransferRebate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_QueryTransferRebate));
	if (wDataSize!=sizeof(DBR_GP_QueryTransferRebate)) return false;

	//������
	DBR_GP_QueryTransferRebate * pTransferRebate=(DBR_GP_QueryTransferRebate *)pData;

	try
	{
		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pTransferRebate->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pTransferRebate->szPassword);

		//��������
		DBO_GP_QueryTransferRebateResult RebateResult;
		ZeroMemory(&RebateResult,sizeof(RebateResult));
		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferRebate"),true)==DB_SUCCESS)
		{
			//������Ϣ
			RebateResult.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			RebateResult.cbRebateEnabled=m_TreasureDBAide.GetValue_DWORD(TEXT("RebateEnabled"));
			RebateResult.lIngot=m_TreasureDBAide.GetValue_DOUBLE(TEXT("Ingot"));
			RebateResult.lLoveLiness=m_TreasureDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));

		}
		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_TRANSFER_REBATE_RESULT,dwContextID,&RebateResult,sizeof(RebateResult));
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

//���ؽ���
bool CDataBaseEngineSink::OnRequestCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadCheckInReward"),true);

		//����ṹ
		DBO_GP_CheckInReward CheckInReward;
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_REWARD,dwContextID,&CheckInReward,sizeof(CheckInReward));
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
bool CDataBaseEngineSink::OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_CheckInQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_CheckInQueryInfo)) return false;

		//������
		DBR_GP_CheckInQueryInfo * pCheckInQueryInfo=(DBR_GP_CheckInQueryInfo *)pData;

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
			DBO_GP_CheckInInfo CheckInInfo;
			ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

			//������Ϣ
			CheckInInfo.wSeriesDate=m_PlatformDBAide.GetValue_WORD(TEXT("SeriesDate"));
			CheckInInfo.bTodayChecked = (m_PlatformDBAide.GetValue_BYTE(TEXT("TodayCheckIned"))==TRUE);

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_INFO,dwContextID,&CheckInInfo,sizeof(CheckInInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GP_CheckInResult CheckInResult;
			CheckInResult.bSuccessed=false;
			lstrcpyn(CheckInResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(CheckInResult.szNotifyContent));

			//���ͽ��
			WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(CheckInResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//ִ��ǩ��
bool CDataBaseEngineSink::OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_CheckInDone));
		if (wDataSize!=sizeof(DBR_GP_CheckInDone)) return false;

		//������
		DBR_GP_CheckInDone * pCheckInDone=(DBR_GP_CheckInDone *)pData;

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
		DBO_GP_CheckInResult CheckInResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_CheckInResult CheckInResult;
		CheckInResult.bSuccessed=false;
		lstrcpyn(CheckInResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(CheckInResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKIN_RESULT,dwContextID,&CheckInResult,wSendSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestTaskGiveUp( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_TaskGiveUP));
		if (wDataSize!=sizeof(DBR_GP_TaskGiveUP)) return false;

		//������
		DBR_GP_TaskGiveUP * pTake=(DBR_GP_TaskGiveUP *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTake->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTake->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskGiveUp"),false);

		//��������
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GP_TASK_GIVEUP;
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}


//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_TaskTake));
		if (wDataSize!=sizeof(DBR_GP_TaskTake)) return false;

		//������
		DBR_GP_TaskTake * pTaskTake=(DBR_GP_TaskTake *)pData;

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
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_TaskReward));
		if (wDataSize!=sizeof(DBR_GP_TaskReward)) return false;

		//������
		DBR_GP_TaskReward * pTaskReward=(DBR_GP_TaskReward *)pData;

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
		DBO_GP_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GP_TASK_REWARD;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_TaskQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_TaskQueryInfo)) return false;

		//������
		DBR_GP_TaskQueryInfo * pTaskQueryInfo=(DBR_GP_TaskQueryInfo *)pData;

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
			DBO_GP_TaskInfo TaskInfo;
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_INFO,dwContextID,&TaskInfo,wSendDataSize);
		}
		else
		{
			//��������
			DBO_GP_TaskResult TaskResult;
			ZeroMemory(&TaskResult,sizeof(TaskResult));

			//��ȡ����
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������Ϣ
			TaskResult.bSuccessed=false;
			TaskResult.wCommandID=SUB_GP_TASK_LOAD;
			lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

			//���ͽ��
			WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GP_TASK_LOAD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//�ͱ�����
bool CDataBaseEngineSink::OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadBaseEnsure"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GP_BaseEnsureParameter BaseEnsureParameter;
			ZeroMemory(&BaseEnsureParameter,sizeof(BaseEnsureParameter));

			//��������
			if(m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				BaseEnsureParameter.cbTakeTimes= m_PlatformDBAide.GetValue_BYTE(TEXT("TakeTimes"));
				BaseEnsureParameter.lScoreAmount = m_PlatformDBAide.GetValue_WORD(TEXT("ScoreAmount"));
				BaseEnsureParameter.lScoreCondition = m_PlatformDBAide.GetValue_WORD(TEXT("ScoreCondition"));				

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_PARAMETER,dwContextID,&BaseEnsureParameter,sizeof(BaseEnsureParameter));
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
bool CDataBaseEngineSink::OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_TakeBaseEnsure));
		if (wDataSize!=sizeof(DBR_GP_TakeBaseEnsure)) return false;

		//��ȡ����
		DBR_GP_TakeBaseEnsure * pTakeBaseEnsure=(DBR_GP_TakeBaseEnsure *)pData;

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
		DBO_GP_BaseEnsureResult BaseEnsureResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_BaseEnsureResult BaseEnsureResult;
		ZeroMemory(&BaseEnsureResult,sizeof(BaseEnsureResult));

		//���ñ���
		BaseEnsureResult.bSuccessed=false;
		lstrcpyn(BaseEnsureResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(BaseEnsureResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
		wSendSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BASEENSURE_RESULT,dwContextID,&BaseEnsureResult,wSendSize);

		return false;
	}

	return true;
}

//�ƹ���Ϣ
bool CDataBaseEngineSink::OnRequestQuerySpreadInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GP_QuerySpreadInfo * pLoadSpreadInfo = (DBR_GP_QuerySpreadInfo*)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pLoadSpreadInfo->dwUserID);

		//ִ������
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSpreadInfo"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GP_UserSpreadInfo UserSpreadInfo;
			ZeroMemory(&UserSpreadInfo,sizeof(UserSpreadInfo));

			//��������
			if(m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				UserSpreadInfo.dwSpreadCount = m_TreasureDBAide.GetValue_DWORD(TEXT("SpreadCount"));			
				UserSpreadInfo.lSpreadReward= m_TreasureDBAide.GetValue_DOUBLE(TEXT("SpreadReward"));

				//�ƶ���¼
				m_TreasureDBModule->MoveToNext();
			}

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SPREAD_INFO,dwContextID,&UserSpreadInfo,sizeof(UserSpreadInfo));
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

bool CDataBaseEngineSink::OnRequestLoadRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadRealAuth"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//����ṹ
			DBO_GP_RealAuthParameter CmdParameter;
			ZeroMemory(&CmdParameter,sizeof(CmdParameter));
			CmdParameter.dwAuthRealAward= m_PlatformDBAide.GetValue_INT(TEXT("AuthRealAward"));
			CmdParameter.dwAuthentDisable= m_PlatformDBAide.GetValue_INT(TEXT("AuthentDisable"));
			

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_REAL_AUTH_PARAMETER,dwContextID,&CmdParameter,sizeof(CmdParameter));
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

//�ȼ�����
bool CDataBaseEngineSink::OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGrowLevelConfig"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GP_GrowLevelConfig GrowLevelConfig;
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_CONFIG,dwContextID,&GrowLevelConfig,wSendDataSize);
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
bool CDataBaseEngineSink::OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_GrowLevelQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_GrowLevelQueryInfo)) return false;

		//������
		DBR_GP_GrowLevelQueryInfo * pGrowLevelQueryInfo=(DBR_GP_GrowLevelQueryInfo *)pData;

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
		DBO_GP_GrowLevelParameter GrowLevelParameter;
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
			DBO_GP_GrowLevelUpgrade GrowLevelUpgrade;
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
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_UPGRADE,dwContextID,&GrowLevelUpgrade,wSendDataSize);
			}
		}

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return false;
	}
}

//���ص���
bool CDataBaseEngineSink::OnRequestLoadGamePropertyList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//��������
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyTypeItem"),true);
		DBO_GP_GamePropertyTypeItem * pGamePropertyTypeItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyTypeItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGamePropertyTypeItem=(DBO_GP_GamePropertyTypeItem *)(cbBuffer+wPacketSize);
			pGamePropertyTypeItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			pGamePropertyTypeItem->dwTypeID=m_PlatformDBAide.GetValue_DWORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGamePropertyTypeItem->szTypeName,CountArray(pGamePropertyTypeItem->szTypeName));

			//����λ��
			wPacketSize+=sizeof(DBO_GP_GamePropertyTypeItem);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//���ع�ϵ
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyRelatItem"),true);
		DBO_GP_GamePropertyRelatItem * pGamePropertyRelatItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyRelatItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_RELAT_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGamePropertyRelatItem=(DBO_GP_GamePropertyRelatItem *)(cbBuffer+wPacketSize);
			pGamePropertyRelatItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
			pGamePropertyRelatItem->dwTypeID=m_PlatformDBAide.GetValue_DWORD(TEXT("TypeID"));

			//����λ��
			wPacketSize+=sizeof(DBO_GP_GamePropertyRelatItem);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_RELAT_ITEM,dwContextID,cbBuffer,wPacketSize);

		//���ص���
		wPacketSize = 0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertyItem"),true);
		DBO_GP_GamePropertyItem  * pGamePropertyItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GamePropertyItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGamePropertyItem=(DBO_GP_GamePropertyItem *)(cbBuffer+wPacketSize);
			pGamePropertyItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("ID"));
			pGamePropertyItem->dwPropertyKind=m_PlatformDBAide.GetValue_DWORD(TEXT("Kind"));
			pGamePropertyItem->dPropertyCash=m_PlatformDBAide.GetValue_DOUBLE(TEXT("Cash"));
			pGamePropertyItem->lPropertyGold=m_PlatformDBAide.GetValue_DOUBLE(TEXT("Gold"));
			pGamePropertyItem->lPropertyUserMedal=m_PlatformDBAide.GetValue_DOUBLE(TEXT("UserMedal"));
			pGamePropertyItem->lPropertyLoveLiness=m_PlatformDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
			pGamePropertyItem->cbUseArea=m_PlatformDBAide.GetValue_BYTE(TEXT("UseArea"));
			pGamePropertyItem->cbServiceArea=m_PlatformDBAide.GetValue_BYTE(TEXT("ServiceArea"));
			pGamePropertyItem->lSendLoveLiness=m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			pGamePropertyItem->lRecvLoveLiness=m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
			pGamePropertyItem->lUseResultsGold=m_PlatformDBAide.GetValue_DOUBLE(TEXT("UseResultsGold"));
			pGamePropertyItem->dwUseResultsValidTime=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTime"));
			pGamePropertyItem->dwUseResultsValidTimeScoreMultiple=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsValidTimeScoreMultiple"));
			pGamePropertyItem->dwUseResultsGiftPackage=m_PlatformDBAide.GetValue_DWORD(TEXT("UseResultsGiftPackage"));
			pGamePropertyItem->cbRecommend=m_PlatformDBAide.GetValue_BYTE(TEXT("Recommend"));
			pGamePropertyItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			m_PlatformDBAide.GetValue_String(TEXT("Name"),pGamePropertyItem->szName,CountArray(pGamePropertyItem->szName));
			m_PlatformDBAide.GetValue_String(TEXT("RegulationsInfo"),pGamePropertyItem->szRegulationsInfo,CountArray(pGamePropertyItem->szRegulationsInfo));
			//����λ��
			wPacketSize+=sizeof(DBO_GP_GamePropertyItem);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_ITEM,dwContextID,cbBuffer,wPacketSize);

		//�����ӵ���
		wPacketSize=0;
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePropertySubItem"),true);
		DBO_GP_GamePropertySubItem * pGamePropertySubItem=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GamePropertySubItem))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_SUB_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGamePropertySubItem=(DBO_GP_GamePropertySubItem *)(cbBuffer+wPacketSize);
			pGamePropertySubItem->dwPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("ID"));
			pGamePropertySubItem->dwOwnerPropertyID=m_PlatformDBAide.GetValue_DWORD(TEXT("OwnerID"));
			pGamePropertySubItem->dwPropertyCount=m_PlatformDBAide.GetValue_DWORD(TEXT("Count"));
			pGamePropertySubItem->dwSortID=m_PlatformDBAide.GetValue_DWORD(TEXT("SortID"));
			//����λ��
			wPacketSize+=sizeof(DBO_GP_GamePropertySubItem);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_SUB_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��������
		DBO_GP_GamePropertyListResult GamePropertyResult;
		ZeroMemory(&GamePropertyResult,sizeof(GamePropertyResult));

		//���ñ���
		GamePropertyResult.cbSuccess=TRUE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_LIST_RESULT,dwContextID,&GamePropertyResult,sizeof(GamePropertyResult));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//��������
		DBO_GP_GamePropertyListResult GamePropertyResult;
		ZeroMemory(&GamePropertyResult,sizeof(GamePropertyResult));

		//���ñ���
		GamePropertyResult.cbSuccess=FALSE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_LIST_RESULT,dwContextID,&GamePropertyResult,sizeof(GamePropertyResult));

		return false;
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_PropertyBuy));
		if (wDataSize!=sizeof(DBR_GP_PropertyBuy)) return false;

		//������
		DBR_GP_PropertyBuy * pPropertyBuy=(DBR_GP_PropertyBuy *)pData;
		
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyBuy->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

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
			DBO_GP_PropertyBuyResult PropertyBuyResult;
			ZeroMemory(&PropertyBuyResult,sizeof(DBO_GP_PropertyBuyResult));
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

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_BUY,dwContextID,&PropertyBuyResult,sizeof(DBO_GP_PropertyBuyResult));
		}
		else
		{
			//������
			DBO_GP_PropertyFailure PropertyFailure;
			ZeroMemory(&PropertyFailure,sizeof(DBO_GP_PropertyFailure));
			PropertyFailure.lErrorCode = lResultCode;

			//��ȡ��ʾ
			CDBVarValue DBVarValue;
			m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(PropertyFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(PropertyFailure.szDescribeString));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_FAILURE,dwContextID,&PropertyFailure,sizeof(PropertyFailure));

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


bool CDataBaseEngineSink::OnRequestUseProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_PropertyUse));
	if (wDataSize!=sizeof(DBR_GP_PropertyUse)) return false;

	//������
	DBR_GP_PropertyUse * pPropertyUse=(DBR_GP_PropertyUse *)pData;

	DBO_GP_PropertyUse DBOPropertyUseResult;
	ZeroMemory(&DBOPropertyUseResult, sizeof(DBOPropertyUseResult));
	DBOPropertyUseResult.dwPropID = pPropertyUse->dwPropID;
	DBOPropertyUseResult.dwUserID = pPropertyUse->dwUserID;
	DBOPropertyUseResult.dwRecvUserID = pPropertyUse->dwRecvUserID;
	DBOPropertyUseResult.wPropCount = pPropertyUse->wPropCount;

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
		DBOPropertyUseResult.lRecvLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));
		DBOPropertyUseResult.lSendLoveLiness = m_PlatformDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
		DBOPropertyUseResult.Score = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Score"));
		DBOPropertyUseResult.lUseResultsGold = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsGold"));
		DBOPropertyUseResult.UseResultsValidTime = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsValidTime"));
		DBOPropertyUseResult.dwScoreMultiple = m_PlatformDBAide.GetValue_INT(TEXT("UseResultsValidTimeScoreMultiple"));
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

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PROPERTY_USE,dwContextID,&DBOPropertyUseResult,sizeof(DBOPropertyUseResult));

	return (lResultCode==0);
}

bool CDataBaseEngineSink::OnRequestPropertyQuerySingle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_PropertyQuerySingle));
		if (wDataSize!=sizeof(DBR_GP_PropertyQuerySingle)) return false;

		//������
		DBR_GP_PropertyQuerySingle * pPropertyBuy=(DBR_GP_PropertyQuerySingle *)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pPropertyBuy->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwPropertyID"),pPropertyBuy->dwPropertyID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pPropertyBuy->szPassword);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_PropertQuerySingle"),true);

		if(lResultCode == DB_SUCCESS)
		{
			//������
			DBO_GP_PropertyQuerySingle PropertyQuerySingle;
			ZeroMemory(&PropertyQuerySingle,sizeof(PropertyQuerySingle));
			PropertyQuerySingle.dwUserID = pPropertyBuy->dwUserID;
			PropertyQuerySingle.dwPropertyID = m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyID"));
			PropertyQuerySingle.dwItemCount = m_PlatformDBAide.GetValue_DWORD(TEXT("ItemCount"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_QUERY_SINGLE,dwContextID,&PropertyQuerySingle,sizeof(PropertyQuerySingle));
		}
		else
		{
			//������
			DBO_GP_PropertyQuerySingle PropertyQuerySingle;
			ZeroMemory(&PropertyQuerySingle,sizeof(PropertyQuerySingle));
			PropertyQuerySingle.dwUserID = pPropertyBuy->dwUserID;
			PropertyQuerySingle.dwPropertyID = pPropertyBuy->dwPropertyID;
			PropertyQuerySingle.dwItemCount = 0;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_QUERY_SINGLE,dwContextID,&PropertyQuerySingle,sizeof(PropertyQuerySingle));


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

bool CDataBaseEngineSink::OnRequestUserBackpackProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_QueryBackpack));
		if (wDataSize!=sizeof(DBR_GP_QueryBackpack)) return false;

		//������
		DBR_GP_QueryBackpack * pQueryBackpack = (DBR_GP_QueryBackpack *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryBackpack->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		
		
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pQueryBackpack->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@dwKind"),pQueryBackpack->dwKindID);
		
		const DWORD dwDataHead = sizeof(DBO_GP_QueryBackpack)-sizeof(tagBackpackProperty);

		//���췵��
		BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
		DBO_GP_QueryBackpack* pQueryBackpackResult = (DBO_GP_QueryBackpack*)cbDataBuffer;

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
					//������ʾ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���͵�������:%d"),pQueryBackpackResult->dwCount);
					CTraceService::TraceString(szDescribe,TraceLevel_Exception);

					pQueryBackpackResult->dwStatus = 0;
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_BACKPACK,dwContextID,pQueryBackpackResult,dwDataBufferSize);
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

bool CDataBaseEngineSink::OnRequestPropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_GP_UserPropertyBuff));
		DBR_GP_UserPropertyBuff* PropertyBuffRequest = (DBR_GP_UserPropertyBuff*)pData;

		//��������
		DBO_GR_UserPropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));
		GamePropertyBuff.dwUserID = PropertyBuffRequest->dwUserID;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), PropertyBuffRequest->dwUserID);

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
			GamePropertyBuff.PropertyBuff[i].dwScoreMultiple=m_PlatformDBAide.GetValue_WORD(TEXT("UseResultsValidTimeScoreMultiple"));
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_UserPropertyBuff GamePropertyBuff;
		ZeroMemory(&GamePropertyBuff,sizeof(GamePropertyBuff));

		//�������
		GamePropertyBuff.cbBuffCount=0L;

		//������Ϣ
		WORD wHeadSize=sizeof(GamePropertyBuff)-sizeof(GamePropertyBuff.PropertyBuff);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_PROPERTY_BUFF,dwContextID,&GamePropertyBuff,wHeadSize);

		return false;
	}

	return true;
}

bool CDataBaseEngineSink::OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_GP_PropertyPresent));
		DBR_GP_PropertyPresent* PropertyPresentRequest = (DBR_GP_PropertyPresent*)pData;

		//��������
		DBO_GP_PropertyPresent PropertyPresent={0};
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

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(PropertyPresent.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PropertyPresent.szNotifyContent));

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_PRESENT,dwContextID,&PropertyPresent,sizeof(PropertyPresent));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		DBR_GP_PropertyPresent* PropertyPresentRequest = (DBR_GP_PropertyPresent*)pData;
		//��������
		DBO_GP_PropertyPresent PropertyPresent={0};
		PropertyPresent.dwUserID = PropertyPresentRequest->dwUserID;
		PropertyPresent.dwRecvGameID = PropertyPresentRequest->dwRecvGameID;
		PropertyPresent.dwPropID = PropertyPresentRequest->dwPropID;
		PropertyPresent.wPropCount = PropertyPresentRequest->wPropCount;
		PropertyPresent.wType = PropertyPresentRequest->wType; //0�ǳ�  1 GameID
		lstrcpyn(PropertyPresent.szRecvNickName,PropertyPresentRequest->szRecvNickName,CountArray(PropertyPresent.szRecvNickName));
		//�������
		PropertyPresent.nHandleCode = -1;
		lstrcpyn(PropertyPresent.szNotifyContent, TEXT("���͵���ʱ ���ݿ�����쳣"), CountArray(PropertyPresent.szNotifyContent));

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROPERTY_PRESENT,dwContextID,&PropertyPresent,sizeof(PropertyPresent));
		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_GP_QuerySendPresent));
		DBR_GP_QuerySendPresent* pQuerySendPresent = (DBR_GP_QuerySendPresent*)pData;

		//��������
		DBO_GP_QuerySendPresent QuerySendPresenResult;
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
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		//��������
		DBO_GP_QuerySendPresent QuerySendPresenResult;
		ZeroMemory(&QuerySendPresenResult,sizeof(QuerySendPresenResult));

		//�������
		QuerySendPresenResult.wPresentCount=0;

		//������Ϣ
		WORD wHeadSize=sizeof(QuerySendPresenResult)-sizeof(QuerySendPresenResult.Present);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_SEND_PRESENT,dwContextID,&QuerySendPresenResult,wHeadSize);

		return false;
	}

	return true;
}


bool CDataBaseEngineSink::OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_GP_GetSendPresent));
		DBR_GP_GetSendPresent* pGetSendPresent = (DBR_GP_GetSendPresent*)pData;

		//��������
		DBO_GP_GetSendPresent GetSendPresenResult;
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
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		//��������
		DBO_GP_GetSendPresent GetSendPresenResult;
		ZeroMemory(&GetSendPresenResult,sizeof(GetSendPresenResult));

		//�������
		GetSendPresenResult.wPresentCount=0;

		//������Ϣ
		WORD wHeadSize=sizeof(GetSendPresenResult)-sizeof(GetSendPresenResult.Present);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GET_SEND_PRESENT,dwContextID,&GetSendPresenResult,wHeadSize);

		return false;
	}
	return true;
}

//����������������
bool CDataBaseEngineSink::OnRequestServerDummyOnLine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAddOfOnline"),true);

		//�������ݷ���czz
		//���㵥�����������
		WORD wTotalCount = SOCKET_TCP_PACKET / sizeof(tagServerDummyOnLine);

		BYTE cbBuffer[SOCKET_TCP_PACKET];

		ZeroMemory(cbBuffer, SOCKET_TCP_PACKET);

		tagServerDummyOnLine * pResultData = (tagServerDummyOnLine *)cbBuffer;

		WORD wCount = 0;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SERVER_DUMMY_ONLINE_BEGIN,dwContextID, 0, 0);
		while(m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//��ȡ��Ϣ
			pResultData->wServerID=m_PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
			
			pResultData->dwDummyMaxOnLine = m_PlatformDBAide.GetValue_DWORD(TEXT("ChangeRate"));
			pResultData->dwDummyMinOnLine=m_PlatformDBAide.GetValue_DWORD(TEXT("AddCounts"));

			printf("wServerID:%d minDummy:%d, maxDummy:%d\n", pResultData->wServerID, pResultData->dwDummyMinOnLine, pResultData->dwDummyMaxOnLine);

			TCHAR szDataBuffer[128] = {0};
			m_PlatformDBAide.GetValue_String(TEXT("StartTime"), szDataBuffer, CountArray(szDataBuffer));

			CString str(szDataBuffer);

			int nHours = _ttoi(str.Left(2));
			int nMinute = _ttoi(str.Mid(3,4));

			//��֪Ϊ�� ��sscanf ����
			//_stscanf(szDataBuffer,_T("%[^:]"),nHours,nMinute);

			//printf("[2][][][][][]\n");

			SYSTEMTIME tempDate;
			GetSystemTime(&tempDate);

			//printf("�꣺%d �£�%d �ܣ�%d �գ�%d Сʱ��%d ���ӣ�%d �룺%d\n", tempDate.wYear, tempDate.wMonth, tempDate.wDayOfWeek, tempDate.wDay, tempDate.wHour, tempDate.wMinute, tempDate.wSecond);

			//��ֵ���Ǳ���ʱ��
			tempDate.wYear = 1970;
			tempDate.wMonth = 1;
			tempDate.wDayOfWeek = 1;
			tempDate.wDay = 1;
			tempDate.wMinute = nMinute;
			tempDate.wHour = nHours;

			//��ȥ8Сʱת��Ϊ��׼ʱ��
			pResultData->lTimeStart = SystemTimeToTimet(tempDate);

			//time_t nCurTime = 0;
			//nCurTime = time(NULL);

			//SYSTEMTIME tempData1;
			//GetSystemTime(&tempData1);
			//printf("SystemTimeToTimeT:%ld\n", SystemTimeToTimet(tempData1));

			//printf("000nMinuteStart��%d, nHoursStart��%d, CurTime��%ld, lTimeStart��%ld\n", nMinute, nHours, nCurTime, pServerDummyOnLine->lTimeStart);


			memset(szDataBuffer, 0, sizeof(szDataBuffer));
			m_PlatformDBAide.GetValue_String(TEXT("EndTime"), szDataBuffer, CountArray(szDataBuffer));

			str = szDataBuffer;
			nHours = _ttoi(str.Left(2));
			nMinute = _ttoi(str.Mid(3,4));

			tempDate.wMinute = nMinute;
			tempDate.wHour = nHours;

			pResultData->lTimeEnd = SystemTimeToTimet(tempDate);
			//printf("001nMinuteEnd��%d, nHoursEnd��%d, lTimeEnd��%ld\n", nMinute, nHours, pServerDummyOnLine->lTimeEnd);


			m_PlatformDBModule->MoveToNext();

			//ָ�����
			++pResultData;

			if (++wCount>wTotalCount)
			{
				//������������
				if (m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SERVER_DUMMY_ONLINE,dwContextID, cbBuffer, sizeof(tagServerDummyOnLine) * wCount)== false)
				{
					CTraceService::TraceString(_T("DBO_GP_SERVER_DUMMY_ONLINE ��Ϣ δ�ܷ��ͳɹ���"), TraceLevel_Warning);
					return false;
				}

				//����״̬
				wCount = 0;
				ZeroMemory(cbBuffer, SOCKET_TCP_PACKET * sizeof(BYTE));
				pResultData = (tagServerDummyOnLine *)cbBuffer;
			}
		}

		//���ͷ�������ʣ������

		if (wCount != 0)
		{
			if (m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SERVER_DUMMY_ONLINE,dwContextID, cbBuffer, sizeof(tagServerDummyOnLine) * wCount)==false)
			{
				CTraceService::TraceString(_T("DBO_GP_SERVER_DUMMY_ONLINE ��Ϣ δ�ܷ��ͳɹ���"), TraceLevel_Warning);
				return false;
			}
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_SERVER_DUMMY_ONLINE_END,dwContextID, 0, 0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}
}

//��Ա����
bool CDataBaseEngineSink::OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadMemberParameter"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GP_MemberParameter MemberParameter;
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
				m_PlatformDBAide.GetValue_String(TEXT("MemberName"),pMemberParameter->szMemberName,CountArray(pMemberParameter->szMemberName));
				pMemberParameter->dwMemberRight= m_PlatformDBAide.GetValue_DWORD(TEXT("UserRight"));
				pMemberParameter->dwMemberTask= m_PlatformDBAide.GetValue_DWORD(TEXT("TaskRate"));
				pMemberParameter->dwMemberShop= m_PlatformDBAide.GetValue_DWORD(TEXT("ShopRate"));
				pMemberParameter->dwMemberInsure= m_PlatformDBAide.GetValue_DWORD(TEXT("InsureRate"));
				pMemberParameter->dwMemberDayPresent= m_PlatformDBAide.GetValue_DWORD(TEXT("DayPresent"));
				pMemberParameter->dwMemberDayGiftID= m_PlatformDBAide.GetValue_DWORD(TEXT("DayGiftID"));
				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(MemberParameter)-sizeof(MemberParameter.MemberParameter);
			wSendDataSize += sizeof(tagMemberParameterNew)*MemberParameter.wMemberCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_PARAMETER,dwContextID,&MemberParameter,wSendDataSize);
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
bool CDataBaseEngineSink::OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_MemberQueryInfo));
		if (wDataSize!=sizeof(DBR_GP_MemberQueryInfo)) return false;

		//��ȡ����
		DBR_GP_MemberQueryInfo * pDBRMemberInfo=(DBR_GP_MemberQueryInfo *)pData;

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
		DBO_GP_MemberQueryInfoResult pDBOMemberInfoResult;
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
		wSendDataSize += sizeof(tagGiftPropertyInfo)*(WORD)(pDBOMemberInfoResult.GiftSubCount);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_QUERY_INFO_RESULT,dwContextID,&pDBOMemberInfoResult,wSendDataSize);

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

//��Ա����
bool CDataBaseEngineSink::OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_MemberDayPresent));
		if (wDataSize!=sizeof(DBR_GP_MemberDayPresent)) return false;

		//��ȡ����
		DBR_GP_MemberDayPresent * pDBRMemberInfo=(DBR_GP_MemberDayPresent *)pData;

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
		DBO_GP_MemberDayPresentResult pDBOMemberInfoResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_DAY_PRESENT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
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
bool CDataBaseEngineSink::OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_MemberDayGift));
		if (wDataSize!=sizeof(DBR_GP_MemberDayGift)) return false;

		//��ȡ����
		DBR_GP_MemberDayGift * pDBRMemberInfo=(DBR_GP_MemberDayGift *)pData;

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
		DBO_GP_MemberDayGiftResult pDBOMemberInfoResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_MEMBER_DAY_GIFT_RESULT,dwContextID,&pDBOMemberInfoResult,wSendSize);
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

//�����Ա
bool CDataBaseEngineSink::OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_PurchaseMember));
		if (wDataSize!=sizeof(DBR_GP_PurchaseMember)) return false;

		//������
		DBR_GP_PurchaseMember * pPurchaseMember=(DBR_GP_PurchaseMember *)pData;

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
		DBO_GP_PurchaseResult PurchaseResult;
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
				PurchaseResult.cbMemberOrder = m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			}	
		}

		//��ȡ��ʾ
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(PurchaseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//���ñ���
		lstrcpyn(PurchaseResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByIngot));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByIngot)) return false;

		//������
		DBR_GP_ExchangeScoreByIngot * pExchangeScore=(DBR_GP_ExchangeScoreByIngot *)pData;

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
		DBO_GP_ExchangeResult ExchangeResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}


//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByBeans));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByBeans)) return false;

		//������
		DBR_GP_ExchangeScoreByBeans * pExchangeScore=(DBR_GP_ExchangeScoreByBeans *)pData;

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
		DBO_GP_ExchangeResult ExchangeResult;
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestRoomCardExchangeToScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ExchangeScoreByRoomCard));
		if (wDataSize!=sizeof(DBR_GP_ExchangeScoreByRoomCard)) return false;

		//������
		DBR_GP_ExchangeScoreByRoomCard * pExchangeScore=(DBR_GP_ExchangeScoreByRoomCard *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeRoomCard"),pExchangeScore->lExchangeRoomCard);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeRoomCardByScore"),true);

		//����ṹ
		DBO_GP_RoomCardExchangeResult ExchangeResult;
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
				ExchangeResult.lCurrRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("RoomCard"));
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROOM_CARD_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GP_RoomCardExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));
		ExchangeResult.bSuccessed=false;
		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROOM_CARD_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}
//�����б�
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//��������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);

		//��������
		wPacketSize=0;
		DBO_GP_GameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GameType))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameType=(DBO_GP_GameType *)(cbBuffer+wPacketSize);
			pGameType->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//����λ��
			wPacketSize+=sizeof(DBO_GP_GameType);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);

		//��������
		wPacketSize=0;
		DBO_GP_GameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GameKind))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameKind=(DBO_GP_GameKind *)(cbBuffer+wPacketSize);
			pGameKind->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			pGameKind->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wGameID=m_PlatformDBAide.GetValue_WORD(TEXT("GameID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));
			m_PlatformDBAide.GetValue_String(TEXT("ProcessName"),pGameKind->szProcessName,CountArray(pGameKind->szProcessName));
			pGameKind->wRecommend=m_PlatformDBAide.GetValue_WORD(TEXT("Recommend"));
			pGameKind->wGameFlag =m_PlatformDBAide.GetValue_WORD(TEXT("GameFlag"));
			pGameKind->dwSuportType=m_PlatformDBAide.GetValue_WORD(TEXT("SuportType"));

			//����λ��
			wPacketSize+=sizeof(DBO_GP_GameKind);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��ȡ�ڵ�
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);

		//���ͽڵ�
		wPacketSize=0;
		DBO_GP_GameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GameNode))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameNode=(DBO_GP_GameNode *)(cbBuffer+wPacketSize);
			pGameNode->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));
			
			//����λ��
			wPacketSize+=sizeof(DBO_GP_GameNode);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePageItem"),true);

		//���Ͷ���
		wPacketSize=0;
		DBO_GP_GamePage * pGamePage=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(DBO_GP_GamePage))>(sizeof(cbBuffer)-sizeof(NTY_DataBaseEvent)))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGamePage=(DBO_GP_GamePage *)(cbBuffer+wPacketSize);
			pGamePage->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGamePage->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGamePage->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGamePage->wPageID=m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGamePage->wOperateType=m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"),pGamePage->szDisplayName,CountArray(pGamePage->szDisplayName));
			
			//����λ��
			wPacketSize+=sizeof(DBO_GP_GamePage);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��������
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//���ñ���
		GameListResult.cbSuccess=TRUE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//��������
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//���ñ���
		GameListResult.cbSuccess=FALSE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//������Ϣ
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//Ч������
		ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKindEx))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKindEx)))) return false;

		//������ʶ
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//������Ϣ
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			_sntprintf(&szOnLineCountKind[nLength],CountArray(szOnLineCountKind)-nLength,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[i].wKindID,pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//������Ϣ
		TCHAR szAndroidCountKind[2048]=TEXT("");
		for (WORD j=0;j<pOnLineCountInfo->wKindCount;j++)
		{
			INT nLength=lstrlen(szAndroidCountKind);
			_sntprintf(&szAndroidCountKind[nLength],CountArray(szAndroidCountKind)-nLength,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[j].wKindID,pOnLineCountInfo->OnLineCountKind[j].dwAndroidCount);
		}

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@dwAndroidCountSum"),pOnLineCountInfo->dwAndroidCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);
		m_PlatformDBAide.AddParameter(TEXT("@strAndroidCountKind"),szAndroidCountKind);

		//ִ������
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

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

//ƽ̨����
bool CDataBaseEngineSink::OnRequestPlatformParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadPlatformParameter"),true);

		//����ṹ
		DBO_GP_PlatformParameter PlatformParameter;
		ZeroMemory(&PlatformParameter,sizeof(PlatformParameter));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��¼�ж�
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				PlatformParameter.dwExchangeRate = m_PlatformDBAide.GetValue_DWORD(TEXT("ExchangeRate"));
				PlatformParameter.dwPresentExchangeRate = m_PlatformDBAide.GetValue_DWORD(TEXT("PresentExchangeRate"));
				PlatformParameter.dwRateGold = m_PlatformDBAide.GetValue_DWORD(TEXT("RateGold"));
			}						
		}

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PLATFORM_PARAMETER,dwContextID,&PlatformParameter,sizeof(PlatformParameter));

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

//��������
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
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
			DBO_GP_TaskListInfo * pTaskListInfo = (DBO_GP_TaskListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(DBO_GP_TaskListInfo);

			//���ñ���
			WORD wTaskCount=0;
			WORD wSendDataSize=sizeof(DBO_GP_TaskListInfo);		

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
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);

					//���ñ���
					wTaskCount=0;
					wSendDataSize=sizeof(DBO_GP_TaskListInfo);	
					pDataBuffer = cbDataBuffer+sizeof(DBO_GP_TaskListInfo);
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
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);
			}

			//����֪ͨ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TASK_LIST_END,dwContextID,NULL,0);			
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

//��¼�ɹ�
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString,DWORD dwCheckUserRight, bool bMobileClient)
{
	if (bMobileClient==false)
	{
		if (dwErrorCode==DB_SUCCESS)
		{ 
			//��������
			DBO_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//��������
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.lLoveLiness=m_AccountsDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("DynamicPass"),LogonSuccess.szDynamicPass,CountArray(LogonSuccess.szDynamicPass));

			//�û��ɼ�
			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Score"));
			LogonSuccess.lUserIngot=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Ingot"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Insure"));
			LogonSuccess.dUserBeans= m_AccountsDBAide.GetValue_DOUBLE(TEXT("Beans"));

			//�û�����
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//��Ա����
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//��չ��Ϣ
			LogonSuccess.cbInsureEnabled=m_AccountsDBAide.GetValue_BYTE(TEXT("InsureEnabled"));
			LogonSuccess.cbIsAgent=m_AccountsDBAide.GetValue_BYTE(TEXT("IsAgent"));
			LogonSuccess.dwCheckUserRight=dwCheckUserRight;

			//��ȡ��Ϣ
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));
            
			//////////////////////////////////////////////////////////////////////////
			try
			{
				//��������
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_TCP_PACKET];

				//��ȡ����
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),LogonSuccess.dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@dwDeviceID"),1);
				//�������
				TCHAR szDescribeString[128]=TEXT("");
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
				m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAgentGameKindItem"),true);

				//��������
				wPacketSize=0;
				tagAgentGameKind * pGameKind=NULL;
				while (m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//��ȡ��Ϣ
					pGameKind=(tagAgentGameKind *)(cbBuffer+wPacketSize);
					pGameKind->wKindID=m_AccountsDBAide.GetValue_WORD(TEXT("KindID"));
					pGameKind->wSortID=m_AccountsDBAide.GetValue_WORD(TEXT("SortID"));

					//����λ��
					wPacketSize+=sizeof(tagAgentGameKind);

					//�ƶ���¼
					m_AccountsDBModule->MoveToNext();
				}
				if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_AGENT_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
			}
			catch(...)
			{
				ASSERT(FALSE);
			}
			//////////////////////////////////////////////////////////////////////////

			//���ͽ��
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else if(dwErrorCode == DB_NEEDMB)
		{
			//��¼�ɹ�
			DBO_GP_ValidateMBCard ValidateMBCard;
			ZeroMemory(&ValidateMBCard,sizeof(ValidateMBCard));

			//��ȡ����
			ValidateMBCard.uMBCardID=m_AccountsDBAide.GetValue_UINT(TEXT("PasswordID"));

			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VALIDATE_MBCARD,dwContextID,&ValidateMBCard,sizeof(ValidateMBCard));
		}
		else if(dwErrorCode == DB_PASSPORT)
		{
			//Ͷ�ݽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VALIDATE_PASSPORT,dwContextID,NULL,0);			
		}
		else
		{
			//��������
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//��������
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//���ͽ��
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}
	else
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			//��������
			DBO_MB_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//��������
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.lLoveLiness=m_AccountsDBAide.GetValue_LONGLONG(TEXT("LoveLiness"));

			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("DynamicPass"),LogonSuccess.szDynamicPass,CountArray(LogonSuccess.szDynamicPass));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//�û��ɼ�
 			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Score"));
			LogonSuccess.lUserIngot=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Ingot"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_DOUBLE(TEXT("Insure"));
			LogonSuccess.dUserBeans= m_AccountsDBAide.GetValue_DOUBLE(TEXT("Beans"));

			//��Ա����
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//ע��ʱ��
			m_AccountsDBAide.GetValue_SystemTime(TEXT("RegisterDate"), LogonSuccess.RegisterData);

			//��չ��Ϣ
			LogonSuccess.cbInsureEnabled=m_AccountsDBAide.GetValue_BYTE(TEXT("InsureEnabled"));
			LogonSuccess.cbIsAgent=m_AccountsDBAide.GetValue_BYTE(TEXT("IsAgent"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			
			//������Ϣ����������
			LogonSuccess.lRoomCard=m_AccountsDBAide.GetValue_LONGLONG(TEXT("RoomCard"));
			LogonSuccess.dwLockServerID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockServerID"));
			LogonSuccess.dwKindID = m_AccountsDBAide.GetValue_DWORD(TEXT("KindID"));

			//�Ƿ��ֶ�������ip
			m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),LogonSuccess.szChangeLogonIP,CountArray(LogonSuccess.szChangeLogonIP));

			//ע��ʱ����ֻ�����-���˺���--�ο��Ƿ�����ֻ�
			m_AccountsDBAide.GetValue_String(TEXT("RegisterMobile"),LogonSuccess.szRegisterMobile,CountArray(LogonSuccess.szRegisterMobile));

			//��ȡ��Ϣ
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));

			//////////////////////////////////////////////////////////////////////////
			try
			{
				//��������
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_TCP_PACKET];

				//��ȡ����
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),LogonSuccess.dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@dwDeviceID"),2);
				//�������
				TCHAR szDescribeString[128]=TEXT("");
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
				m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAgentGameKindItem"),true);

				//��������
				wPacketSize=0;
				tagAgentGameKind * pGameKind=NULL;
				while (m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//��ȡ��Ϣ
					pGameKind=(tagAgentGameKind *)(cbBuffer+wPacketSize);
					pGameKind->wKindID=m_AccountsDBAide.GetValue_WORD(TEXT("KindID"));
					pGameKind->wSortID=m_AccountsDBAide.GetValue_WORD(TEXT("SortID"));

					//����λ��
					wPacketSize+=sizeof(tagAgentGameKind);

					//�ƶ���¼
					m_AccountsDBModule->MoveToNext();
				}
				if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AGENT_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
			}
			catch(...)
			{
				ASSERT(FALSE);
			}
			//////////////////////////////////////////////////////////////////////////

			//���ͽ��
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//��������
			DBO_MB_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//��������
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			//���ͽ��
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}

//���н��
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		DBO_GP_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//�������
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GP_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//�������
		UserInsureFailure.lResultCode=dwErrorCode;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//�������
VOID CDataBaseEngineSink::OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,bool bCloseSocket)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		DBO_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//�������
		OperateSuccess.bCloseSocket=bCloseSocket;
		OperateSuccess.lResultCode=dwErrorCode;
		lstrcpyn(OperateSuccess.szDescribeString,pszErrorString,CountArray(OperateSuccess.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.bCloseSocket=bCloseSocket;
		OperateFailure.lResultCode=dwErrorCode;
		lstrcpyn(OperateFailure.szDescribeString,pszErrorString,CountArray(OperateFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
	}

	return;
}

//�������
VOID CDataBaseEngineSink::OnAndroidDisposeResult(DWORD dwContextID,DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID)
{
	//����ṹ
	DBO_GP_AndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter,sizeof(AndroidParameter));

	//���ñ���
	AndroidParameter.wSubCommdID=wSubCommdID;
	AndroidParameter.wServerID=wServerID;

	if(dwErrorCode==DB_SUCCESS)
	{
		//��������
		WORD wParameterCount=0;

		while(m_AccountsDBModule->IsRecordsetEnd()==false)		
		{
			wParameterCount = AndroidParameter.wParameterCount;
			AndroidParameter.AndroidParameter[wParameterCount].dwBatchID = m_AccountsDBAide.GetValue_DWORD(TEXT("BatchID"));
			AndroidParameter.AndroidParameter[wParameterCount].dwAndroidCount = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCount"));
			AndroidParameter.AndroidParameter[wParameterCount].dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			AndroidParameter.AndroidParameter[wParameterCount].dwEnterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterTime"));
			AndroidParameter.AndroidParameter[wParameterCount].dwLeaveTime = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			AndroidParameter.AndroidParameter[wParameterCount].dwEnterMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			AndroidParameter.AndroidParameter[wParameterCount].dwEnterMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			AndroidParameter.AndroidParameter[wParameterCount].dwLeaveMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			AndroidParameter.AndroidParameter[wParameterCount].dwLeaveMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			AndroidParameter.AndroidParameter[wParameterCount].dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			AndroidParameter.AndroidParameter[wParameterCount].dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));
			AndroidParameter.AndroidParameter[wParameterCount].lTakeMinScore = m_AccountsDBAide.GetValue_DOUBLE(TEXT("TakeMinScore"));
			AndroidParameter.AndroidParameter[wParameterCount].lTakeMaxScore = m_AccountsDBAide.GetValue_DOUBLE(TEXT("TakeMaxScore"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember0 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember0"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember1 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember1"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember2 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember2"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember3 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember3"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember4 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember4"));
			AndroidParameter.AndroidParameter[wParameterCount].AndroidCountMember5 = m_AccountsDBAide.GetValue_DWORD(TEXT("AndroidCountMember5"));


			//���ñ���
			AndroidParameter.wParameterCount++;

			//����ж�
			if(AndroidParameter.wParameterCount>=CountArray(AndroidParameter.AndroidParameter))
			{
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ANDROID_PARAMETER,dwContextID,&AndroidParameter,sizeof(AndroidParameter));

				//���ñ���
				ZeroMemory(&AndroidParameter,sizeof(AndroidParameter));
			}

			//�ƶ��α�
			m_AccountsDBModule->MoveToNext();
		}	
	}
	
	//�����С
	WORD wSendDataSize = sizeof(AndroidParameter)-sizeof(AndroidParameter.AndroidParameter);
	wSendDataSize += AndroidParameter.wParameterCount*sizeof(tagAndroidParameter);

	//��������
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ANDROID_PARAMETER,dwContextID,&AndroidParameter,wSendDataSize);
}

VOID CDataBaseEngineSink::OnIndividualDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
		//����ṹ
		DBO_GP_IndividualResult individual;
		ZeroMemory(&individual,sizeof(individual));
		
		//�������
		individual.bSuccessed = true;
		individual.lScore=m_AccountsDBAide.GetValue_DOUBLE(TEXT("SourceScore"));
		lstrcpyn(individual.szDescribeString,pszErrorString,CountArray(individual.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(individual.szDescribeString);
		WORD wHeadSize=sizeof(individual)-sizeof(individual.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_INDIVIDUAL_RESULT,dwContextID,&individual,wHeadSize+wDataSize);
}

//��ȡ˽�˷�����Ϸ���ͷ���
bool CDataBaseEngineSink::OnRequestPersonalRoomUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO * pUserInfo = (DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO *)pData;
		ASSERT(wDataSize==sizeof(DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO));
		if (wDataSize!=sizeof(DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO)) return false;
		//�������
		m_PlatformDBAide.ResetParameter();

		//�û���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"), pUserInfo->dwUserID);

		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_QueryPersonalRoomUserInfo"),true);
		if (DB_SUCCESS == lResultCode)
		{
			DBO_MB_PersonalRoomUserInfo userInfo;
			userInfo.lRoomCard =  m_PlatformDBAide.GetValue_LONGLONG(TEXT("RoomCard") );
			userInfo.dBeans = m_PlatformDBAide.GetValue_DOUBLE(TEXT("Beans"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_PERSONAL_ROOM_USER_INFO_RESULT,dwContextID, &userInfo, sizeof(userInfo));
		}

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return true;
	}

}

//////////////////////////////////////////////////////////////////////////////////
