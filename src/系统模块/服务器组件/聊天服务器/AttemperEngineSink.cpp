#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#include "AttemperEngineSink.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������
#define TEMP_MESSAGE_ID				INVALID_DWORD			

//ʱ���ʶ
#define IDI_CONNECT_CORRESPOND		1									//����ʱ��

//���ʱ��
#define MIN_INTERVAL_TIME			10									//���ʱ��
#define MAX_INTERVAL_TIME			1*60								//��ʱ��ʶ

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

	//״̬����
	m_pInitParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
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
	m_pNormalParameter=new tagBindParameter[m_pInitParameter->m_wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxPlayer);

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

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;

	//������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ���û�
	m_FriendGroupManager.DeleteFriendGroup();
	m_ServerUserManager.DeleteUserItem();

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
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));

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
	if(dwTimerID == IDI_CONNECT_CORRESPOND)
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
		CTraceService::TraceString(TEXT("����ע����Ϸ���������..."),TraceLevel_Normal);

		//ע������
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);

		return true;
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
		m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);

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

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnEventTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//�û���Ϣ
			{
				return OnEventTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	return true;
}

bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_PROPERTY_TRUMPET:  //������Ϣ
		{
			//�㲥����
			m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_TRUMPET_NOTIFY,pData,wDataSize,0xFF);
			return true;
		}
	case SUB_CS_S_HALL_MESSAGE:		//������Ϣ
		{
			//�㲥����
			m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_HALL_NOTIFY,pData,wDataSize,0xFF);
			return true;
		}
	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_CHAT_INSERT:	//�������
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}

	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_GAMESTATE:	//�������
		{
			return OnEventTCPSocketSubUserStatus(wSubCmdID, pData,wDataSize);
		}
	case SUB_CS_S_CHAT_FORCE_OFFLINE:	//ǿ������
		{
			return OnEventTCPSocketSubForceOffline(wSubCmdID,pData,wDataSize);
		}
	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketSubUserStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//������֤
	ASSERT(wDataSize==sizeof(CMD_CS_S_UserGameStatus));
	if(wDataSize!=sizeof(CMD_CS_S_UserGameStatus)) return false;

	//��ȡ����
	CMD_CS_S_UserGameStatus * pUserGameStatus = (CMD_CS_S_UserGameStatus *)pData;
	ASSERT(pUserGameStatus!=NULL);
	if(pUserGameStatus==NULL) return false;

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserGameStatus->dwUserID);
	if(pServerUserItem==NULL ) return true;

	//��ȡ����
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

	//�ذ��ж�
	if((pUserGameStatus->cbGameStatus==pLocalUserItem->GetGameStatus()) &&
		(pUserGameStatus->wServerID==pLocalUserItem->GetServerID()) && 
		(pUserGameStatus->wTableID==pLocalUserItem->GetTableID()) )
	{
		return true;
	}

	//����״̬
	tagFriendUserInfo* pUserInfo=pLocalUserItem->GetUserInfo();
	pUserInfo->cbGameStatus=pUserGameStatus->cbGameStatus;
	pUserInfo->wKindID = pUserGameStatus->wKindID;
	pUserInfo->wServerID=pUserGameStatus->wServerID;
	pUserInfo->wTableID=pUserGameStatus->wTableID;
	pUserInfo->wChairID=pUserGameStatus->wChairID;
	lstrcpyn(pUserInfo->szServerName,pUserGameStatus->szServerName,CountArray(pUserInfo->szServerName));

	//�㲥״̬
	CMD_GC_UserGameStatusNotify UserGameStatusNotify;
	UserGameStatusNotify.dwUserID=pUserGameStatus->dwUserID;
	UserGameStatusNotify.cbGameStatus=pUserGameStatus->cbGameStatus;
	UserGameStatusNotify.wKindID = pUserGameStatus->wKindID;
	UserGameStatusNotify.wServerID=pUserGameStatus->wServerID;
	UserGameStatusNotify.wTableID=pUserGameStatus->wTableID;
	lstrcpyn(UserGameStatusNotify.szServerName,pUserGameStatus->szServerName,CountArray(UserGameStatusNotify.szServerName));

	//�㲥������
	SendDataToUserFriend(pUserGameStatus->dwUserID,MDM_GC_USER,SUB_GC_GAME_STATUS_NOTIFY,&UserGameStatusNotify,sizeof(UserGameStatusNotify));


	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketSubForceOffline(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(CMD_CS_S_ForceOffline)==0);
	if (wDataSize%sizeof(CMD_CS_S_ForceOffline)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(CMD_CS_S_ForceOffline);
	CMD_CS_S_ForceOffline * pForceOffline=(CMD_CS_S_ForceOffline *)pData;

	ASSERT(pForceOffline!=NULL);
	if(pForceOffline==NULL) return false;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//�����û�
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pForceOffline->dwUserID);
		if(pServerUserItem==NULL ) return true;

		//��ȡ����
		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

		//֪ͨ���û���������
		CMD_GC_UserOnlineStatusNotify UserOnLine;
		UserOnLine.dwUserID=pServerUserItem->GetUserID();
		UserOnLine.cbMainStatus=FRIEND_US_OFFLINE;	
		SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

		//֪ͨ�ͻ����û�����
		DWORD dwSocketID = pLocalUserItem->GetSocketID();
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_FORCE_OFFLINE,0,0);

		//�Ͽ�����
		//m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

		//�����û�
		m_ServerUserManager.DeleteUserItem(UserOnLine.dwUserID);

		++pForceOffline;
	}

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
	CServerUserItem * pServerUserItem =pBindParameter->pServerUserItem;

	try
	{
		//�û�����
		if (pServerUserItem!=NULL)
		{
			//�û�����
			CMD_GC_UserOnlineStatusNotify UserOnLine;
			UserOnLine.dwUserID=pServerUserItem->GetUserID();
			UserOnLine.cbMainStatus=FRIEND_US_OFFLINE;	
			SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

			//֪ͨЭ���û�����
			CMD_CS_C_ChatUserLeave UserLeave = {0};
			UserLeave.dwUserID = pServerUserItem->GetUserID();
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_CHAT_USER_LEAVE,&UserLeave,sizeof(UserLeave));

			//�����û�
			m_ServerUserManager.DeleteUserItem(UserOnLine.dwUserID);
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�ر������쳣: dwSocketID=%d"),dwSocketID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GC_LOGON:		//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_USER:		//�û�����
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return true;
}


//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}




//��������
bool CAttemperEngineSink::SendChatServerConfig(DWORD dwUserID)
{
	CMD_GC_ConfigServer ConfigServer;
	ZeroMemory(&ConfigServer,sizeof(ConfigServer));

	ConfigServer.wMaxPlayer=m_pInitParameter->m_wMaxPlayer;
	ConfigServer.dwServiceRule=m_pInitParameter->m_dwServiceRule;
	ConfigServer.cbMinOrder=m_pInitParameter->m_cbMinOrder;

	//SendData(pIServerUserItem,MDM_GC_CONFIG,SUB_GC_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

	return true;
}

//��������
bool CAttemperEngineSink::SendSystemMessage(BYTE cbSendMask, LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize,0xFF);

	return true;
}

//��������
bool CAttemperEngineSink::SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}


//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_PC_LOGON_USERID:		//I D ��¼
	case SUB_GC_MB_LOGON_USERID:		//I D ��¼
		{
			return OnTCPNetworkSubMBLogonByUserID(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_MB_LOGON_ACCOUNTS:		//I D ��¼
		{
			return OnTCPNetworkSubMBLogonByAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_APPLYFOR_FRIEND:		//��������
		{
			return OnTCPNetworkSubApplyForFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_RESPOND_FRIEND:			//���ѻ�Ӧ
		{
			return OnTCPNetworkSubRespondFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_GAME:			//������Ϸ
		{
			return OnTCPNetworkSubRoomInvite(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_PERSONAL:
		{
			return OnTCPNetworkSubInvitePersonal(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_CHAT:				//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_SEARCH_USER:			//�����û�
		{
			return OnTCPNetworkSubSearchUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_TRUMPET:				//������Ϣ
		{
			return OnTCPNetworkSubTrumpet(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_DELETE_FRIEND:			//ɾ������
		{
			return OnTCPNetworkDeleteFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_UPDATE_COORDINATE:
		{
			return OnTCPNetworkUpdateCoordinate(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_GET_NEARUSER:
		{
			return OnTCPNetworkGetNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_QUERY_NEARUSER:
		{
			return OnTCPNetworkQueryNearuser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_SHARE:
		{
			return OnTCPNetworkSubUserShare(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}


//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByUserID));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByUserID)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//������Ϣ
	CMD_GC_MB_LogonByUserID * pLogonUserID=(CMD_GC_MB_LogonByUserID *)pData;

	//�ظ���¼
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pServerUserItem->GetUserID());
		//SendLogonFailure(_T("���Ѿ���¼��Ϸ��~"), 1, dwSocketID);
		//return false;
	}

	//��¼�û����뵽����
	if (std::find(m_LogonLockList.begin(), m_LogonLockList.end(), pLogonUserID->dwUserID) == m_LogonLockList.end())
	{
		m_LogonLockList.push_back(pLogonUserID->dwUserID);
	}
	else
	{
		SendLogonFailure(_T("��¼�����У����Ժ�~"), 1, dwSocketID);
		return false;
	}
	

	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;
	pLogonUserID->szPhoneMode[CountArray(pLogonUserID->szPhoneMode)-1]=0;

	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserID.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szPhoneMode,pLogonUserID->szPhoneMode,CountArray(LogonUserID.szPhoneMode));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));
	////�����û�
	//if(pServerUserItem!=NULL)
	//{
	//	//�����û�
	//	if(pServerUserItem->GetUserItemKind()==enLocalKind)
	//	{
	//		//��������
	//		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

	//		//У������
	//		if(pLocalUserItem->ContrastLogonPass(pLogonUserID->szPassword)==false)
	//		{
	//			SendLogonFailure(TEXT("��������˺Ż����벻��ȷ,��ȷ�Ϻ����µ�¼��"),0,dwSocketID);

	//			return true;
	//		}

	//		//�ذ��ж�
	//		if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
	//		{
	//			if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
	//			{
	//				return true;
	//			}
	//		}

	//		//����������Ϣ
	//		if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE && (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
	//		{
	//			if(pLocalUserItem->GetSocketID()!=dwSocketID)
	//			{
	//				//TCHAR szMessage[256]=TEXT("");
	//				//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
	//				//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

	//				////������Ϣ
	//				//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

	//				//����ṹ
	//				CMD_GC_UserLogonAfresh UserLogonAfresh;
	//				_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("�����˺��ڱ𴦵�¼,���������ߣ�"));

	//				//��������
	//				WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
	//				SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);
	//			}
	//		}			

	//		//�޸��û�״̬
	//		pLocalUserItem->SetSocketID(dwSocketID);
	//		pLocalUserItem->SetLogonTime(GetTickCount());
	//		pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
	//		pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
	//	}
	//}	

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));


	return true;
}

//�˺� ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByAccounts));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByAccounts)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//������Ϣ
	CMD_GC_MB_LogonByAccounts * pLogonAccounts=(CMD_GC_MB_LogonByAccounts *)pData;


	//�ظ���¼
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pServerUserItem->GetUserID());
		//SendLogonFailure(_T("���Ѿ���¼��Ϸ��~"), 1, dwSocketID);
		//return false;
	}


	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szPhoneMode[CountArray(pLogonAccounts->szPhoneMode)-1]=0;

	//��������
	DBR_GR_LogonUserAccounts LogonUserAccounts;
	ZeroMemory(&LogonUserAccounts,sizeof(LogonUserAccounts));

	//��������
	lstrcpyn(LogonUserAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonUserAccounts.szAccounts));
	LogonUserAccounts.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserAccounts.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonUserAccounts.szPassword));
	lstrcpyn(LogonUserAccounts.szPhoneMode,pLogonAccounts->szPhoneMode,CountArray(LogonUserAccounts.szPhoneMode));
	lstrcpyn(LogonUserAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonUserAccounts.szMachineID));

	//�����û�
	pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if(pServerUserItem!=NULL)
	{
		//�����û�
		if(pServerUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

			//У������
			if(pLocalUserItem->ContrastLogonPass(pLogonAccounts->szPassword)==false)
			{
				SendLogonFailure(TEXT("��������˺Ż����벻��ȷ,��ȷ�Ϻ����µ�¼��"),0,dwSocketID);

				return true;
			}

			//�ذ��ж�
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
				{
					return true;
				}
			}

			//����������Ϣ
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE && (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
			{
				if(pLocalUserItem->GetSocketID()!=dwSocketID)
				{
					//TCHAR szMessage[256]=TEXT("");
					//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
					//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

					////������Ϣ
					//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

					//����ṹ
					CMD_GC_UserLogonAfresh UserLogonAfresh;
					_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("�����˺��ڱ𴦵�¼,���������ߣ�"));

					//��������
					WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
					SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);
				}
			}			

			//�޸��û�״̬
			pLocalUserItem->SetSocketID(dwSocketID);
			pLocalUserItem->SetLogonTime(GetTickCount());
			pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
			pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
		}
	}	

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_ACCOUNTS,dwSocketID,&LogonUserAccounts,sizeof(LogonUserAccounts));


	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GC_ApplyForFriend));
	if (wDataSize!=sizeof(CMD_GC_ApplyForFriend)) return false;

	//������Ϣ
	CMD_GC_ApplyForFriend * pApplyForFriend=(CMD_GC_ApplyForFriend *)pData;
	ASSERT(pApplyForFriend!=NULL);

	//���Ҷ���
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pApplyForFriend->dwUserID);
	if(pUserFriendGroup!=NULL) 
	{
		tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->SearchFriendItem(pApplyForFriend->dwFriendID);
		if(pServerFriendInfo!=NULL && pServerFriendInfo->cbGroupID!=0) 
		{
			//���뷴��
			CMD_GC_ApplyForFriendEcho ApplyForFriend;
			ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
			ApplyForFriend.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(ApplyForFriend.szDescribeString,_T("�Ѿ��Ǻ��ѣ���������"),CountArray(ApplyForFriend.szDescribeString));
			WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
			WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);

			return true;
		}
	}

	//�����û�
	CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwFriendID);

	//��������
	bool bBufferMessage=false;
	if(pFriendUserItem==NULL) bBufferMessage=true;
	if(pFriendUserItem && pFriendUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true; 

	if(pFriendUserItem!=NULL)
	{
		//TCHAR szMessage[256]=TEXT("");
		//_sntprintf(szMessage,CountArray(szMessage),TEXT(".........%d�����%dΪ����,%d��ǰ״̬Ϊ%d"),pApplyForFriend->dwUserID,pApplyForFriend->dwFriendID,pApplyForFriend->dwFriendID,pFriendUserItem->GetMainStatus());

		////������Ϣ
		//CTraceService::TraceString(szMessage,TraceLevel_Normal);	
	}

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//����ṹ
	CMD_GC_ApplyForNotify ApplyForNotify;
	ApplyForNotify.dwRequestID = pApplyForFriend->dwUserID;
	ApplyForNotify.cbGroupID=pApplyForFriend->cbGroupID;
	lstrcpyn(ApplyForNotify.szNickName,((CLocalUserItem *)pServerUserItem)->GetNickName(),CountArray(ApplyForNotify.szNickName));

	//�����ж�
	if(bBufferMessage==true)
	{
		SaveOfflineMessage(pApplyForFriend->dwFriendID,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify),dwSocketID);
	}
	else
	{
		if(pFriendUserItem->GetUserItemKind()==enLocalKind)
		{
			SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_APPLYFOR_NOTIFY,&ApplyForNotify,sizeof(ApplyForNotify));
		}
	}

	//���뷴��
	CMD_GC_ApplyForFriendEcho ApplyForFriend;
	ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
	ApplyForFriend.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(ApplyForFriend.szDescribeString,_T(""),CountArray(ApplyForFriend.szDescribeString));
	WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
	WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);


	return true;
}

//���ѻ�Ӧ
bool CAttemperEngineSink::OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GC_RespondFriend));
	if (wDataSize!=sizeof(CMD_GC_RespondFriend)) return false;

	//������Ϣ
	CMD_GC_RespondFriend * pRespondFriend=(CMD_GC_RespondFriend *)pData;
	ASSERT(pRespondFriend!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwRequestID);
	if(pServerUserItem==NULL ) 
	{
		//��Ӧ����
		CMD_GC_RespondFriendEcho RespondFriendEcho;
		ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
		RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(RespondFriendEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(RespondFriendEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
		WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
		return true;
	}

	//���Ҷ���
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pRespondFriend->dwRequestID);
	if(pUserFriendGroup!=NULL) 
	{
		if(pUserFriendGroup->SearchFriendItem(pRespondFriend->dwUserID)!=NULL) 
		{
			//��Ӧ����
			CMD_GC_RespondFriendEcho RespondFriendEcho;
			ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
			RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(RespondFriendEcho.szDescribeString,_T("�Ѿ��Ǻ���"),CountArray(RespondFriendEcho.szDescribeString));
			WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
			WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
			return true;
		}
	}

	//��������
	if(pRespondFriend->bAccepted==true)
	{
		//����ṹ
		DBR_GR_AddFriend AddFriend;
		AddFriend.bLoadUserInfo = false;
		AddFriend.dwRequestUserID = pRespondFriend->dwRequestID;
		AddFriend.dwUserID = pRespondFriend->dwUserID;
		AddFriend.cbRequestGroupID = pRespondFriend->cbRequestGroupID;
		AddFriend.cbGroupID = pRespondFriend->cbGroupID;		
		if(pRequestUserItem==NULL )
		{
			AddFriend.bLoadUserInfo = true;
		}		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ADD_FRIEND,dwSocketID,&AddFriend,sizeof(AddFriend));
	}
	else
	{
		//��������
		bool bBufferMessage=false;
		if(pRequestUserItem==NULL) bBufferMessage=true;
		if(pRequestUserItem && pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true;

		//����ṹ
		CMD_GC_RespondNotify RespondNotify;
		_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s �ܾ������ĺ������룡"),((CLocalUserItem *)pServerUserItem)->GetNickName());

		//������Ϣ
		if(bBufferMessage==true)
		{
			WORD wSize = sizeof(RespondNotify)-sizeof(RespondNotify.szNotifyContent)+CountStringBuffer(RespondNotify.szNotifyContent);
			SaveOfflineMessage(pRespondFriend->dwRequestID,SUB_GC_RESPOND_NOTIFY,&RespondNotify,wSize,dwSocketID);

		}
		else
		{
			if(pRequestUserItem->GetUserItemKind()==enLocalKind)
			{
				SendData(((CLocalUserItem*)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}
	}

	//��Ӧ����
	CMD_GC_RespondFriendEcho RespondFriendEcho;
	ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
	RespondFriendEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(RespondFriendEcho.szDescribeString,_T(""),CountArray(RespondFriendEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
	WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);

	return true;
}

//������Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_InviteGame));
	if (wDataSize>sizeof(CMD_GC_InviteGame)) return false;

	//������Ϣ
	CMD_GC_InviteGame * pVipRoomInvite=(CMD_GC_InviteGame *)pData;
	ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_InviteGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.wServerID=pVipRoomInvite->wServerID;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//��������
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_GAME_NOTIFY,&VipRoomInviteNotify,wSize);

	//��Ӧ����
	CMD_GC_InviteGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//������Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_InvitePersonalGame));
	if (wDataSize>sizeof(CMD_GC_InvitePersonalGame)) return false;

	//������Ϣ
	CMD_GC_InvitePersonalGame * pVipRoomInvite=(CMD_GC_InvitePersonalGame *)pData;
	ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_InvitePersonalGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.dwServerNumber=pVipRoomInvite->dwServerNumber;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//��������
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_PERSONAL_NOTIFY,&VipRoomInviteNotify,wSize);

	//��Ӧ����
	CMD_GC_InvitePersonalGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_UserChat));
	if (wDataSize>sizeof(CMD_GC_UserChat)) return false;

	//������Ϣ
	CMD_GC_UserChat * pUserChat=(CMD_GC_UserChat *)pData;
	ASSERT(pUserChat!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSenderID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);
		return false;
	}


	//�����û�
	CServerUserItem * pTargetUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
	if(pTargetUserItem!=NULL)
	{
		CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pTargetUserItem;
		SendData(pLocalUerItem->GetSocketID(),MDM_GC_USER,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize);

		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(UserChatEcho.szDescribeString,_T(""),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

	}
	else if(pTargetUserItem==NULL)
	{
		//���췴��
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("����ʧ�ܣ��Է��Ѿ�����"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

		////������Ϣ
		//SaveOfflineMessage(pUserChat->dwTargetUserID,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize,dwSocketID);
	}

	return true;
}

//�����û�
bool CAttemperEngineSink::OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_SearchByGameID));
	if (wDataSize>sizeof(CMD_GC_SearchByGameID)) return false;

	//������Ϣ
	CMD_GC_SearchByGameID * pSearchUser=(CMD_GC_SearchByGameID *)pData;
	ASSERT(pSearchUser!=NULL);

	//��������
	DBR_GR_SearchUser SearchUser;
	SearchUser.dwSearchByGameID = pSearchUser->dwSearchByGameID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_USER,dwSocketID,&SearchUser,sizeof(SearchUser));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_Trumpet));
	if (wDataSize>sizeof(CMD_GC_Trumpet)) return false;

	//������Ϣ
	CMD_GC_Trumpet * pUserChat=(CMD_GC_Trumpet *)pData;
	ASSERT(pUserChat!=NULL);

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSendUserID);
	if(pServerUserItem==NULL ) 
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
		return false;
	}


	DBR_GR_Trumpet Trumpet;
	Trumpet.wPropertyID = pUserChat->wPropertyID;
	Trumpet.dwSendUserID = pUserChat->dwSendUserID;
	Trumpet.TrumpetColor = pUserChat->TrumpetColor;
	Trumpet.dwClientAddr =pBindParameter->dwClientAddr;
	lstrcpyn(Trumpet.szSendNickName, pUserChat->szSendNickName, CountArray(Trumpet.szSendNickName));
	lstrcpyn(Trumpet.szTrumpetContent, pUserChat->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_TRUMPET, dwSocketID, &Trumpet, sizeof(Trumpet));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_DeleteFriend));
	if (wDataSize>sizeof(CMD_GC_DeleteFriend)) return false;

	//������Ϣ
	CMD_GC_DeleteFriend * pDeleteFriend=(CMD_GC_DeleteFriend *)pData;
	ASSERT(pDeleteFriend!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//ɾ������
		CMD_GC_DeleteFriendEcho DeleteEcho;
		ZeroMemory(&DeleteEcho,sizeof(DeleteEcho));
		DeleteEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(DeleteEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(DeleteEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(DeleteEcho.szDescribeString);
		WORD wHeadSize=sizeof(DeleteEcho)-sizeof(DeleteEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_ECHO,&DeleteEcho,wHeadSize+wDataSize);
		return false;
	}

	//����ṹ
	DBR_GR_DeleteFriend DeleteFriend;
	DeleteFriend.dwUserID=pDeleteFriend->dwUserID;
	DeleteFriend.dwFriendUserID=pDeleteFriend->dwFriendUserID;
	DeleteFriend.cbGroupID=pDeleteFriend->cbGroupID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_FRIEND,dwSocketID,&DeleteFriend,sizeof(DeleteFriend));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_Update_Coordinate));
	if (wDataSize>sizeof(CMD_GC_Update_Coordinate)) return false;

	//������Ϣ
	CMD_GC_Update_Coordinate * pUpdateCoordinate=(CMD_GC_Update_Coordinate *)pData;
	ASSERT(pUpdateCoordinate!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateCoordinate->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//���·���
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	if (pUpdateCoordinate->dLatitude>180 || pUpdateCoordinate->dLongitude>180)
	{
		//���·���
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("�������"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}

	//���¶�λ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	tagFriendUserInfo * pUserInfo = ((CLocalUserItem *)pServerUserItem)->GetUserInfo();
	pUserInfo->dwClientAddr = pBindParameter->dwClientAddr;
	pUserInfo->dLongitude = pUpdateCoordinate->dLongitude;
	pUserInfo->dLatitude = pUpdateCoordinate->dLatitude;
	pUserInfo->cbCoordinate = 1;

	CMD_GC_Update_CoordinateNotify Notify;
	Notify.dLatitude = pUserInfo->dLatitude;
	Notify.dLongitude = pUserInfo->dLongitude;
	Notify.cbCoordinate = 1;
	Notify.dwClientAddr = pUserInfo->dwClientAddr;
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_NOTIFY,&Notify,sizeof(Notify));

	{
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
	}

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkGetNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_Get_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Get_Nearuser)) return false;

	//������Ϣ
	CMD_GC_Get_Nearuser * pGetNearuser=(CMD_GC_Get_Nearuser *)pData;
	ASSERT(pGetNearuser!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGetNearuser->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	//��ȡ����
	CNearUserManager  NearUserManager;
	m_ServerUserManager.GetNearUserItem(pGetNearuser->dwUserID,NearUserManager);

	CMD_GC_Get_NearuserResult Nearuser;
	Nearuser.cbUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (Nearuser.cbUserCount !=0)
	{
		int nIndex = 0;
		for (INT i = 0;i<Nearuser.cbUserCount;i++)
		{
			tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(i);
			CopyMemory(&(Nearuser.NearUserInfo[nIndex++]),pUserInfo,sizeof(tagNearUserInfo));
			if (nIndex == MAX_FRIEND_TRANSFER)
			{
				Nearuser.cbUserCount = nIndex;
                SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
				nIndex = 0;
			}
		}
		if (nIndex !=0)
		{
			Nearuser.cbUserCount = nIndex;
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
			nIndex = 0;
		}

	}    
	else
	{
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));
	}

	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

bool CAttemperEngineSink::OnTCPNetworkQueryNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_Query_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Query_Nearuser)) return false;

	//������Ϣ
	CMD_GC_Query_Nearuser * pQueryNearuser=(CMD_GC_Query_Nearuser *)pData;
	ASSERT(pQueryNearuser!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("��Ϣ��ʱ"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}
	CServerUserItem * pNearuserUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwNearuserUserID);
	if(pNearuserUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("����ʧ�ܣ��Է��Ѿ�����"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}
	//��ȡ����
	CNearUserManager  NearUserManager;
	m_ServerUserManager.QueryNearUserItem(pQueryNearuser->dwUserID,pQueryNearuser->dwNearuserUserID,NearUserManager);
	CMD_GC_Query_NearuserResult Nearuser;
	Nearuser.cbUserCount = (BYTE)(NearUserManager.GetUserItemCount());
	ZeroMemory(&Nearuser.NearUserInfo,sizeof(Nearuser.NearUserInfo));
	if (Nearuser.cbUserCount !=0)
	{
		tagNearUserInfo * pUserInfo = NearUserManager.EnumUserItem(0);
		CopyMemory(&Nearuser.NearUserInfo,pUserInfo,sizeof(tagNearUserInfo));
	}

	SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_RESULT,&Nearuser,sizeof(Nearuser));

	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(Echo.szDescribeString,_T(""),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
	}


	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GC_UserShare));
	if (wDataSize>sizeof(CMD_GC_UserShare)) return false;

	//������Ϣ
	CMD_GC_UserShare * pUserShare=(CMD_GC_UserShare *)pData;
	ASSERT(pUserShare!=NULL);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("��Ϣ��ʱ"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//�����û�
	CServerUserItem * pSharedUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwSharedUserID);
	if(pSharedUserItem==NULL ) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("δ�ҵ����û�"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//���߼��
	if(pSharedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//���뷴��
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("�û��Ѿ�����"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//��������
	CMD_GC_UserShareNotify UserShareNotify;
	UserShareNotify.dwShareUserID=pUserShare->dwUserID;
	lstrcpyn(UserShareNotify.szShareImageAddr,pUserShare->szShareImageAddr,CountArray(UserShareNotify.szShareImageAddr));
	lstrcpyn(UserShareNotify.szMessageContent,pUserShare->szMessageContent,CountArray(UserShareNotify.szMessageContent));

	//��������
	SendData(((CLocalUserItem *)pSharedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_USER_SHARE_NOTIFY,&UserShareNotify,sizeof(UserShareNotify));

	//��Ӧ����
	CMD_GC_UserShareEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//��������
	CMD_GC_LogonEcho LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//��������
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//��������
	SendData(dwSocketID,MDM_GC_LOGON,SUB_GC_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//�������
bool CAttemperEngineSink::SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID)
{
	//��������
	CMD_GC_OperateFailure OperateFailure;
	OperateFailure.lErrorCode=lErrorCode;
	OperateFailure.lResultCode=lOperateCode;
	lstrcpyn(OperateFailure.szDescribeString,pszString,CountArray(OperateFailure.szDescribeString));

	//��������
	SendData(dwSocketID,MDM_GC_USER,SUB_GP_OPERATE_FAILURE,&OperateFailure,sizeof(OperateFailure));

	return true;
}

//�������
bool CAttemperEngineSink::SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID)
{
	//����ṹ
	CMD_GC_OperateSuccess OperateSuccess;
	OperateSuccess.lResultCode=lOperateCode;
	lstrcpyn(OperateSuccess.szDescribeString,pszString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wSize = sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+CountStringBuffer(OperateSuccess.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_OPERATE_SUCCESS,&OperateSuccess,wSize);

	return true;
}


//��������
bool CAttemperEngineSink::SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize)
{
	//�Ƴ��û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem!=NULL)
	{
		//��ȡ����
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(dwUserID);
		if(pUserFriendGroup!=NULL)
		{
			//ö���û�
			WORD wEnumIndex=0;
			tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->EnumFriendItem(wEnumIndex);
			while(pServerFriendInfo!=NULL)
			{
				//���ұ����û�
				CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pServerFriendInfo->dwUserID);
				if(pServerUserItem!=NULL )
				{
					CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pServerUserItem;					
					if(pLocalUerItem->GetMainStatus()!=FRIEND_US_OFFLINE)
					{
						SendData(pLocalUerItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
					}
				}

				//ö�ٺ���
				pServerFriendInfo = pUserFriendGroup->EnumFriendItem(++wEnumIndex);
			}			
		}
	}

	return true;
}

//�洢������Ϣ
bool CAttemperEngineSink::SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID)
{
	//��������
	DBR_GR_SaveOfflineMessage OfflineMessage;
	OfflineMessage.dwUserID = dwUserID;
	OfflineMessage.wMessageType = wMessageType;
	OfflineMessage.wDataSize = wDataSize;
	OfflineMessage.szOfflineData[0]=0;

	//��������
	TCHAR szOffLineData[CountArray(OfflineMessage.szOfflineData)+1]=TEXT("");
	BYTE cbOffLineData[CountArray(OfflineMessage.szOfflineData)/2];
	ZeroMemory(cbOffLineData,sizeof(cbOffLineData));
	CopyMemory(cbOffLineData,pData,wDataSize);		

	//������Ϣ
	for (INT i=0;i<CountArray(cbOffLineData);i++) 
	{
		_stprintf(&szOffLineData[i*2],TEXT("%02X"),cbOffLineData[i]);
	}

	//���ñ���
	lstrcpyn(OfflineMessage.szOfflineData,szOffLineData,CountArray(OfflineMessage.szOfflineData));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SAVE_OFFLINEMESSAGE,dwSocketID,&OfflineMessage,sizeof(OfflineMessage));

	return true;
}

//���û�
CServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pServerUserItem;
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
	if (wBindIndex<m_pInitParameter->m_wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
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
		case DBO_GR_ACCOUNT_LOGON_FAILURE:
			{
				return OnDBAccountLogonFailure(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_GROUPINFO:			//���ط���
			{
				return OnDBLoadUserGroup(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_FRIENDINFO:		//���غ���
			{
				return OnDBLoadUserFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_SINDIVIDUAL:		//��������
			{
				return OnDBLoadUserSimpleIndividual(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_REMARKS:			//�û���ע
			{
				return OnDBLoadUserRemarksInfo(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOAD_OFFLINEMESSAGE:	//������Ϣ
			{
				return OnDBLoadUserOfflineMessage(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SEARCH_USER_RESULT:		//���ҽ��
			{
				return OnDBSearchUserResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_ADD_FRIEND:				//��Ӻ���
			{
				return OnDBUserAddFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_DELETE_FRIEND:			//ɾ������
			{
				return OnDBLoadDeleteFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_OPERATE_RESULT:			//�������
			{
				return OnDBUserOperateResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_TRUMPET_RESULT:
			{
				return OnDBTrumpetResult(dwContextID,pData,wDataSize);
			}
		}
	}catch(...)
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("OnEventDataBase-wRequestID=%d"),wRequestID);

		//������Ϣ
		CTraceService::TraceString(szMessage,TraceLevel_Exception);
	}

	return false;
}

bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_LogonSuccess));
	if(wDataSize>sizeof(DBO_GR_LogonSuccess)) return false;

	//��������
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	////�ظ���¼
	//CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	//if(pServerUserItem!=NULL)
	//{
	//	m_ServerUserManager.DeleteUserItem(pDBOLogonSuccess->dwUserID);
	//}

	std::list<DWORD>::iterator it = std::find(m_LogonLockList.begin(), m_LogonLockList.end(), pDBOLogonSuccess->dwUserID);
	if (it != m_LogonLockList.end())
	{
		m_LogonLockList.erase(it);
	}

	//�û�����
	tagFriendUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//��������
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szAccounts, pDBOLogonSuccess->szAccounts, CountArray(UserInfo.szAccounts));
	lstrcpyn(UserInfo.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfo.szPassword));
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.dwFaceID =pDBOLogonSuccess->dwFaceID;
	UserInfo.dwCustomID =pDBOLogonSuccess->dwCustomID;
	UserInfo.wMemberOrder = pDBOLogonSuccess->wMemberOrder;
	UserInfo.wGrowLevel =pDBOLogonSuccess->wGrowLevel;
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));
	lstrcpyn(UserInfo.szCompellation,pDBOLogonSuccess->szCompellation,CountArray(UserInfo.szCompellation));
	
	//�û�״̬
	UserInfo.cbMainStatus=FRIEND_US_ONLINE;
	UserInfo.cbGameStatus=US_NULL;
	UserInfo.wKindID=INVALID_KIND;
	UserInfo.wServerID=INVALID_SERVER;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;
	ZeroMemory(UserInfo.szServerName,CountArray(UserInfo.szServerName));
	UserInfo.dwClientAddr=pDBOLogonSuccess->dwClientAddr;
	lstrcpyn(UserInfo.szPhoneMode,pDBOLogonSuccess->szPhoneMode,CountArray(UserInfo.szPhoneMode));
	lstrcpyn(UserInfo.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfo.szMachineID));

	//�û�����	
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szEMail,pDBOLogonSuccess->szEMail,CountArray(UserInfo.szEMail));
	lstrcpyn(UserInfo.szSeatPhone,pDBOLogonSuccess->szSeatPhone,CountArray(UserInfo.szSeatPhone));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));
	lstrcpyn(UserInfo.szDwellingPlace,pDBOLogonSuccess->szDwellingPlace,CountArray(UserInfo.szDwellingPlace));
	lstrcpyn(UserInfo.szPostalCode,pDBOLogonSuccess->szPostalCode,CountArray(UserInfo.szPostalCode));

	//��������
	tagInsertLocalUserInfo InsertLocalUserInfo;
	InsertLocalUserInfo.dwSocketID=dwContextID;
	InsertLocalUserInfo.dwLogonTime=GetTickCount();

	//�����û�
	m_ServerUserManager.InsertLocalUserItem(InsertLocalUserInfo,UserInfo,pDBOLogonSuccess->szPassword);

	//��������
	CMD_GC_LogonEcho LogonSuccess;
	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
	WORD wLogonDataSize = sizeof(LogonSuccess);
	SendData(dwContextID,MDM_GC_LOGON,SUB_GC_LOGON_SUCCESS,&LogonSuccess,wLogonDataSize);

	//�����û�
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	pBindParameter->pServerUserItem=pServerUserItem;

	//�û�����
	CMD_GC_UserOnlineStatusNotify UserOnLine;
	UserOnLine.dwUserID=pServerUserItem->GetUserID();
	UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
	SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

	//֪ͨЭ���û�����
	CMD_CS_C_ChatUserEnter UserEnter = {0};
	UserEnter.dwUserID = pServerUserItem->GetUserID();
	lstrcpyn(UserEnter.szNickName, UserInfo.szNickName, CountArray(UserEnter.szNickName));
	m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_CHAT_USER_ENTER,&UserEnter,sizeof(UserEnter));

	//TCHAR szMessage[256]=TEXT("");
	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----����֪ͨ"),UserOnLine.dwUserID);
	//CTraceService::TraceString(szMessage,TraceLevel_Normal);

	//����Ⱥ��
	m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true,0xFF);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	std::list<DWORD>::iterator it = std::find(m_LogonLockList.begin(), m_LogonLockList.end(), pLogonFailure->dwUserID);
	if (it != m_LogonLockList.end())
	{
		m_LogonLockList.erase(it);
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBAccountLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//���ʹ���
	DBO_GR_AccountLogonFailure * pLogonFailure=(DBO_GR_AccountLogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	return true;
}



//���ط���
bool CAttemperEngineSink::OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_UserGroupInfo));
	if(wDataSize>sizeof(DBO_GR_UserGroupInfo)) return false;

	//��ȡ����
	DBO_GR_UserGroupInfo * pDBOUserGroupInfo = (DBO_GR_UserGroupInfo *)pData;
	ASSERT(pDBOUserGroupInfo);

	//��������
	tagClientGroupInfo * pTempGroupInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagClientGroupInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserGroupInfo->wGroupCount;++nIndex)
	{
		//���ñ���
		pTempGroupInfo = &pDBOUserGroupInfo->GroupInfo[nIndex];

		//��������		
		tagClientGroupInfo * pSendGroupInfo=(tagClientGroupInfo *)&cbDataBuffer;

		//���ñ���
		pSendGroupInfo->cbGroupIndex=pTempGroupInfo->cbGroupIndex;
		lstrcpyn(pSendGroupInfo->szGroupName,pTempGroupInfo->szGroupName,CountArray(pSendGroupInfo->szGroupName));

		//���ñ���
		wPacketSize = sizeof(tagClientGroupInfo)-sizeof(pSendGroupInfo->szGroupName)+CountStringBuffer(pSendGroupInfo->szGroupName);

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);			
	}


	return true;
}

//���غ���
bool CAttemperEngineSink::OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_UserFriendInfo));
	if(wDataSize>sizeof(DBO_GR_UserFriendInfo)) return false;

	//��ȡ����
	DBO_GR_UserFriendInfo * pUserFriendInfo = (DBO_GR_UserFriendInfo *)pData;
	ASSERT(pUserFriendInfo);

	//������Ϣ
	for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
	{
		//�����û�
		CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pUserFriendInfo->FriendInfo[wIndex].dwUserID);
		if(pServerUserItem==NULL)
		{
			//����״̬
			pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=FRIEND_US_OFFLINE;
			pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=US_NULL;
			pUserFriendInfo->FriendInfo[wIndex].wKindID=INVALID_KIND;
			pUserFriendInfo->FriendInfo[wIndex].wServerID=INVALID_SERVER;
			pUserFriendInfo->FriendInfo[wIndex].wTableID=INVALID_TABLE;
			pUserFriendInfo->FriendInfo[wIndex].wChairID=INVALID_CHAIR;
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
			ZeroMemory(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
			continue;
		}

		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();

		//���ñ���
		pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=pServerUserItem->GetMainStatus();
		pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=pServerUserItem->GetGameStatus();
		pUserFriendInfo->FriendInfo[wIndex].wKindID=pUserInfo->wKindID;
		pUserFriendInfo->FriendInfo[wIndex].wServerID=pUserInfo->wServerID;
		pUserFriendInfo->FriendInfo[wIndex].wTableID=pUserInfo->wTableID;
		pUserFriendInfo->FriendInfo[wIndex].wChairID=pUserInfo->wChairID;
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szServerName,pUserInfo->szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,pUserInfo->szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
	}

	//��������
	WORD wFriendCount=pUserFriendInfo->wFriendCount;
	CMD_GC_UserFriendInfo IPCUserFriendInfo;


	//ѭ������
	while(wFriendCount>CountArray(IPCUserFriendInfo.FriendInfo))
	{
		//��������
		IPCUserFriendInfo.wFriendCount=CountArray(IPCUserFriendInfo.FriendInfo);
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*IPCUserFriendInfo.wFriendCount);

		//��������
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,sizeof(IPCUserFriendInfo));

		//���ñ���
		wFriendCount -= CountArray(IPCUserFriendInfo.FriendInfo);
	}

	//ʣ�෢��
	if(wFriendCount>0)
	{
		//��������
		IPCUserFriendInfo.wFriendCount=wFriendCount;
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*wFriendCount);

		//��������
		WORD wPacketDataSize = sizeof(IPCUserFriendInfo)-sizeof(IPCUserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*wFriendCount;

		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,wPacketDataSize);

	}

	if(pUserFriendInfo->bLogonFlag==true)
	{
		//��¼���
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_LOGON_FINISH);

		////��ʾ��Ϣ
		//CTraceService::TraceString(TEXT("��������ͬ����Ϣ"),TraceLevel_Normal);
	}

	//��������
	tagServerFriendInfo ServerFriendInfo;

	//�������
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pUserFriendInfo->dwUserID);
	if(pUserFriendGroup==NULL)
	{
		//�������
		pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(pUserFriendInfo->wFriendCount);
		m_FriendGroupManager.InsertFriendGroup(pUserFriendInfo->dwUserID,pUserFriendGroup);
	}

	if(pUserFriendGroup!=NULL)
	{
		//���÷���
		if(pUserFriendInfo->bLogonFlag==true)
		{
			pUserFriendGroup->ResetFriendGroup();
			pUserFriendGroup->SetOwnerUserID(pUserFriendInfo->dwUserID);
		}

		for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
		{
			ServerFriendInfo.dwUserID=pUserFriendInfo->FriendInfo[wIndex].dwUserID;
			ServerFriendInfo.cbMainStatus=pUserFriendInfo->FriendInfo[wIndex].cbMainStatus;
			ServerFriendInfo.cbGameStatus=pUserFriendInfo->FriendInfo[wIndex].cbGameStatus;
			ServerFriendInfo.cbGroupID=pUserFriendInfo->FriendInfo[wIndex].cbGroupID;
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);
		}		
	}

	return true;
}

//���ѱ�ע
bool CAttemperEngineSink::OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_UserRemarksInfo));
	if(wDataSize>sizeof(DBO_GR_UserRemarksInfo)) return false;

	//��ȡ����
	DBO_GR_UserRemarksInfo * pDBOUserRemarksInfo = (DBO_GR_UserRemarksInfo *)pData;
	ASSERT(pDBOUserRemarksInfo);

	//��������
	tagUserRemarksInfo * pUserRemarksInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagUserRemarksInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserRemarksInfo->wFriendCount;++nIndex)
	{
		//���ñ���
		pUserRemarksInfo = &pDBOUserRemarksInfo->RemarksInfo[nIndex];

		//��������		
		tagUserRemarksInfo * pSendUserRemarksInfo=(tagUserRemarksInfo *)&cbDataBuffer;

		//���ñ���
		pSendUserRemarksInfo->dwFriendUserID=pUserRemarksInfo->dwFriendUserID;
		lstrcpyn(pSendUserRemarksInfo->szRemarksInfo,pUserRemarksInfo->szRemarksInfo,CountArray(pSendUserRemarksInfo->szRemarksInfo));

		//���ñ���
		wPacketSize = sizeof(tagUserRemarksInfo)-sizeof(pSendUserRemarksInfo->szRemarksInfo)+CountStringBuffer(pSendUserRemarksInfo->szRemarksInfo);

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_UserIndividual));
	if(wDataSize>sizeof(DBO_GR_UserIndividual)) return false;

	//��ȡ����
	DBO_GR_UserIndividual * pDBOUserSimpleIndividual = (DBO_GR_UserIndividual *)pData;
	ASSERT(pDBOUserSimpleIndividual);

	//��������
	tagUserIndividual * pUserSimpleIndividual=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	WORD wDataIndex=0,wPacketSize=0;

	//ѭ������
	for(int nIndex=0;nIndex<pDBOUserSimpleIndividual->wUserCount;++nIndex)
	{
		//���ñ���
		pUserSimpleIndividual = &pDBOUserSimpleIndividual->UserIndividual[nIndex];

		//��������		
		CMD_GC_UserIndividual * pUserIndividual=(CMD_GC_UserIndividual *)&cbDataBuffer;
		CSendPacketHelper SendPacket(pUserIndividual+1,sizeof(cbDataBuffer)-sizeof(CMD_GC_UserIndividual));

		//����ṹ
		pUserIndividual->dwUserID = pUserSimpleIndividual->dwUserID;

		//���ñ���
		wPacketSize = sizeof(CMD_GC_UserIndividual);

		//QQ����
		if (pUserSimpleIndividual->szQQ[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szQQ);
			SendPacket.AddPacket(pUserSimpleIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
		}

		//Email
		if (pUserSimpleIndividual->szEMail[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szEMail);
			SendPacket.AddPacket(pUserSimpleIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
		}

		//�̶��绰
		if (pUserSimpleIndividual->szSeatPhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szSeatPhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEATPHOHE);
		}


		//�ֻ�����
		if (pUserSimpleIndividual->szMobilePhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szMobilePhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILEPHONE);			
		}

		//��ϵ��ַ
		if (pUserSimpleIndividual->szDwellingPlace[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szDwellingPlace);
			SendPacket.AddPacket(pUserSimpleIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLINGPLACE);
		}

		//��������
		if (pUserSimpleIndividual->szPostalCode[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szPostalCode);
			SendPacket.AddPacket(pUserSimpleIndividual->szPostalCode,wBufferSize,DTP_GP_UI_POSTALCODE);
		}

		//���ñ���
		wPacketSize += SendPacket.GetDataSize();

		//�����ж�
		if(wDataIndex+wPacketSize+sizeof(WORD)>CountArray(cbSendBuffer))
		{			
			//������Ϣ
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);

			//���ñ���
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//��������
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(WORD));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(WORD)],cbDataBuffer,wPacketSize);

		//���ñ���
		wDataIndex += sizeof(WORD)+wPacketSize;
	}

	//ʣ�෢��
	if(wDataIndex>0)
	{
		//������Ϣ
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_UserOfflineMessage));
	if(wDataSize>sizeof(DBO_GR_UserOfflineMessage)) return false;

	//��ȡ����
	DBO_GR_UserOfflineMessage * pUserUserOfflineMessage = (DBO_GR_UserOfflineMessage *)pData;
	ASSERT(pUserUserOfflineMessage);

	//��������
	BYTE cbOfflineData[CountArray(pUserUserOfflineMessage->szOfflineData)];
	ZeroMemory(cbOfflineData,sizeof(cbOfflineData));

	//��������	
	if (pUserUserOfflineMessage->szOfflineData[0]!=0)
	{
		//��ȡ����
		INT nCustomRuleSize=lstrlen(pUserUserOfflineMessage->szOfflineData)/2;

		//ת���ַ�
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1=pUserUserOfflineMessage->szOfflineData[i*2];
			TCHAR cbChar2=pUserUserOfflineMessage->szOfflineData[i*2+1];

			//Ч���ַ�
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//���ɽ��
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) cbOfflineData[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) cbOfflineData[i]+=(cbChar2-TEXT('A')+0x0A);

			//���ɽ��
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) cbOfflineData[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) cbOfflineData[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//������Ϣ
	SendData(dwContextID,MDM_GC_USER,pUserUserOfflineMessage->wMessageType,&cbOfflineData,pUserUserOfflineMessage->wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_DeleteFriend));
	if(wDataSize>sizeof(DBO_GR_DeleteFriend)) return false;

	//��ȡ����
	DBO_GR_DeleteFriend * pDeleteFriend = (DBO_GR_DeleteFriend *)pData;
	ASSERT(pDeleteFriend);

	//��ȡ�û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem!=NULL)
	{
		CUserFriendGroup * pFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pFriendGroup!=NULL) 
		{
			//�Ƴ�����
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

		}

		//��ȡ����
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pUserFriendGroup==NULL) 
		{
			pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
		}

		if(pUserFriendGroup!=NULL)
		{
			//��ȡ���ѷ���
			CUserFriendGroup * pDestFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwFriendUserID);
			if(pDestFriendGroup!=NULL) 
			{
				//�Ƴ�����
				pUserFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
				pDestFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

			}



			CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwFriendUserID);
			if(pFriendUserItem!=NULL&&pFriendUserItem->GetMainStatus()==FRIEND_US_ONLINE)
			{
				//����ṹ
				CMD_GC_DeleteFriendNotify DeleteFriendNotify;
				DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwUserID;

				//��������
				SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));
			}
					
		}
	}

		//����ṹ
		CMD_GC_DeleteFriendNotify DeleteFriendNotify;
		DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwFriendUserID;

		//��������
		SendData(dwContextID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));

		return true;

}

//�����û�
bool CAttemperEngineSink::OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_SearchUserResult));
	if(wDataSize>sizeof(DBO_GR_SearchUserResult)) return false;

	//��ȡ����
	DBO_GR_SearchUserResult * pSearchUserResult = (DBO_GR_SearchUserResult *)pData;
	ASSERT(pSearchUserResult);

	//�������
	CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pSearchUserResult->FriendInfo.dwUserID);
	if(pServerUserItem==NULL)
	{
		pSearchUserResult->FriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
		pSearchUserResult->FriendInfo.cbGameStatus=US_NULL;
		pSearchUserResult->FriendInfo.wKindID=INVALID_KIND;
		pSearchUserResult->FriendInfo.wServerID=INVALID_SERVER;
		pSearchUserResult->FriendInfo.wTableID=INVALID_TABLE;
		pSearchUserResult->FriendInfo.wChairID=INVALID_CHAIR;
		ZeroMemory(pSearchUserResult->FriendInfo.szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		ZeroMemory(pSearchUserResult->FriendInfo.szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}
	else
	{
		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;
		tagFriendUserInfo * pUserInfo = pLocalUserItem->GetUserInfo();
		pSearchUserResult->FriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
		pSearchUserResult->FriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
		pSearchUserResult->FriendInfo.wKindID=pUserInfo->wKindID;
		pSearchUserResult->FriendInfo.wServerID=pUserInfo->wServerID;
		pSearchUserResult->FriendInfo.wTableID=pUserInfo->wTableID;
		pSearchUserResult->FriendInfo.wChairID=pUserInfo->wChairID;
		lstrcpyn(pSearchUserResult->FriendInfo.szServerName,pUserInfo->szServerName,CountArray(pSearchUserResult->FriendInfo.szServerName));
		lstrcpyn(pSearchUserResult->FriendInfo.szPhoneMode,pUserInfo->szPhoneMode,CountArray(pSearchUserResult->FriendInfo.szPhoneMode));
	}

	//��������	
	CMD_GC_SearchByGameIDResult SearchUserResult;
	ZeroMemory(&SearchUserResult,sizeof(SearchUserResult));
	SearchUserResult.cbUserCount =pSearchUserResult->cbUserCount;
	BYTE cbUserCount=pSearchUserResult->cbUserCount;
	if(cbUserCount>0)
	{
		CopyMemory(&(SearchUserResult.FriendInfo),&(pSearchUserResult->FriendInfo),sizeof(tagClientFriendInfo)*(pSearchUserResult->cbUserCount));

		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_RESULT,&SearchUserResult,sizeof(SearchUserResult));
	}
	else
	{
		//��Ӧ����
		CMD_GC_SearchByGameIDEcho SearchUserEcho;
		ZeroMemory(&SearchUserEcho,sizeof(SearchUserEcho));
		SearchUserEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(SearchUserEcho.szDescribeString,_T("��ѯ�����û�"),CountArray(SearchUserEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(SearchUserEcho.szDescribeString);
		WORD wHeadSize=sizeof(SearchUserEcho)-sizeof(SearchUserEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_ECHO,&SearchUserEcho,wHeadSize+wDataSize);
	}
	return  true;
}

//��Ӻ���
bool CAttemperEngineSink::OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_AddFriend));
	if(wDataSize!=sizeof(DBO_GR_AddFriend)) return false;

	//��ȡ����
	DBO_GR_AddFriend * pAddFriend = (DBO_GR_AddFriend *)pData;
	ASSERT(pAddFriend);

	//�����û�
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwRequestUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//�����û�
	if(pServerUserItem!=NULL)
	{
		if(pAddFriend->bLoadUserInfo==false && pRequestUserItem && pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pRequestLocalInfo = (CLocalUserItem *)pRequestUserItem;

			//��ȡ����
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//��������
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//���ñ���
			ServerFriendInfo.dwUserID=pAddFriend->dwRequestUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//��ȡ�û�
			if(pRequestUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pRequestUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pRequestUserItem->GetGameStatus();
			}		

			//��Ӻ���
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);

			//��������
			CMD_GC_UserFriendInfo UserFriendInfo;

			ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

			//����ṹ
			UserFriendInfo.wFriendCount = 1;

			//������Ϣ
			UserFriendInfo.FriendInfo[0].dwUserID=pRequestLocalInfo->GetUserID();
			UserFriendInfo.FriendInfo[0].dwGameID= pRequestLocalInfo->GetGameID();
			UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbGroupID;
			UserFriendInfo.FriendInfo[0].dwFaceID =pRequestLocalInfo->GetUserInfo()->dwFaceID;
			UserFriendInfo.FriendInfo[0].cbGender =pRequestLocalInfo->GetUserInfo()->cbGender;
			UserFriendInfo.FriendInfo[0].dwCustomID =pRequestLocalInfo->GetUserInfo()->dwCustomID;
			UserFriendInfo.FriendInfo[0].wMemberOrder =pRequestLocalInfo->GetUserInfo()->wMemberOrder;
			UserFriendInfo.FriendInfo[0].wGrowLevel =pRequestLocalInfo->GetUserInfo()->wGrowLevel;
			lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pRequestLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pRequestLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pRequestLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
			lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pRequestLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

			//������Ϣ
			UserFriendInfo.FriendInfo[0].cbMainStatus=pRequestLocalInfo->GetMainStatus();
			UserFriendInfo.FriendInfo[0].cbGameStatus=pRequestLocalInfo->GetGameStatus();
			UserFriendInfo.FriendInfo[0].wServerID=pRequestLocalInfo->GetServerID();
			UserFriendInfo.FriendInfo[0].wTableID = pRequestLocalInfo->GetTableID();


			//��������
			WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
			SendData(((CLocalUserItem *)pServerUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

		}		
	}

	//�����û�
	if(pRequestUserItem!=NULL)
	{
		if(pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//��������
			CLocalUserItem * pServerLocalInfo = (CLocalUserItem *)pServerUserItem;

			//��ȡ����
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwRequestUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//��������
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//���ñ���
			ServerFriendInfo.dwUserID=pAddFriend->dwUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbRequestGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//��ȡ�û�
			if(pServerUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
			}		

			//��Ӻ���
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);


			if(pRequestUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				//��������
				CMD_GC_UserFriendInfo UserFriendInfo;

				ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

				//����ṹ
				UserFriendInfo.wFriendCount = 1;

				//������Ϣ
				UserFriendInfo.FriendInfo[0].dwUserID=pServerLocalInfo->GetUserID();
				UserFriendInfo.FriendInfo[0].dwGameID= pServerLocalInfo->GetGameID();
				UserFriendInfo.FriendInfo[0].cbGroupID =pAddFriend->cbRequestGroupID;
				UserFriendInfo.FriendInfo[0].dwFaceID =pServerLocalInfo->GetUserInfo()->dwFaceID;
				UserFriendInfo.FriendInfo[0].dwCustomID =pServerLocalInfo->GetUserInfo()->dwCustomID;
				UserFriendInfo.FriendInfo[0].cbGender =pServerLocalInfo->GetUserInfo()->cbGender;
				UserFriendInfo.FriendInfo[0].wMemberOrder =pServerLocalInfo->GetUserInfo()->wMemberOrder;
				UserFriendInfo.FriendInfo[0].wGrowLevel =pServerLocalInfo->GetUserInfo()->wGrowLevel;
				lstrcpyn(UserFriendInfo.FriendInfo[0].szNickName,pServerLocalInfo->GetNickName(),CountArray(UserFriendInfo.FriendInfo[0].szNickName));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szUnderWrite,pServerLocalInfo->GetUserInfo()->szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szCompellation,pServerLocalInfo->GetUserInfo()->szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));
				lstrcpyn(UserFriendInfo.FriendInfo[0].szPhoneMode,pServerLocalInfo->GetUserInfo()->szPhoneMode,CountArray(UserFriendInfo.FriendInfo[0].szPhoneMode));

				//������Ϣ
				UserFriendInfo.FriendInfo[0].cbMainStatus=pServerLocalInfo->GetMainStatus();
				UserFriendInfo.FriendInfo[0].cbGameStatus=pServerLocalInfo->GetGameStatus();
				UserFriendInfo.FriendInfo[0].wServerID=pServerLocalInfo->GetServerID();
				UserFriendInfo.FriendInfo[0].wTableID = pServerLocalInfo->GetTableID();

				//��������
				WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

			}	

			//��Ϣ����
			CMD_GC_RespondNotify RespondNotify;
			_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s ͬ�������ĺ������룡"),pServerLocalInfo->GetNickName());

			if(pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE)
			{
				SaveOfflineMessage(pRequestUserItem->GetUserID(),SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify),dwContextID);
			}
			else
			{
				//��������
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}		
	}

	return true;
}

bool CAttemperEngineSink::OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_TrumpetResult));
	if(wDataSize!=sizeof(DBO_GR_TrumpetResult)) return false;

	//��ȡ����
	DBO_GR_TrumpetResult * pTrumpetResult = (DBO_GR_TrumpetResult *)pData;
	ASSERT(pTrumpetResult);



	if (pTrumpetResult->lResult == 0)
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_OK;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize2);

		CMD_GC_Trumpet Trumpet;
		Trumpet.wPropertyID = pTrumpetResult->wPropertyID;
		Trumpet.dwSendUserID = pTrumpetResult->dwSendUserID;
		Trumpet.TrumpetColor = pTrumpetResult->TrumpetColor;
		lstrcpyn(Trumpet.szSendNickName, pTrumpetResult->szSendNickName, CountArray(Trumpet.szSendNickName));
		lstrcpyn(Trumpet.szTrumpetContent, pTrumpetResult->szTrumpetContent, CountArray(Trumpet.szTrumpetContent));

		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,&Trumpet,sizeof(Trumpet));
		
	}
	else
	{
		//���ȷ���
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,pTrumpetResult->szNotifyContent,CountArray(TrumpetEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(TrumpetEcho.szDescribeString);
		WORD wHeadSize=sizeof(TrumpetEcho)-sizeof(TrumpetEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_TRUMPET_ECHO,&TrumpetEcho,wHeadSize+wDataSize);
	}



	return true;
}

//�������
bool CAttemperEngineSink::OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize<=sizeof(DBO_GR_OperateResult));
	if(wDataSize>sizeof(DBO_GR_OperateResult)) return false;

	//��ȡ����
	DBO_GR_OperateResult * pUserOperateResult = (DBO_GR_OperateResult *)pData;
	ASSERT(pUserOperateResult);

	//��ȡ�û�
	if(pUserOperateResult->bModifySucesss==true)
	{
		//���ͳɹ�
		SendOperateSuccess(pUserOperateResult->szDescribeString,pUserOperateResult->wOperateCode,dwContextID);
	}
	else
	{
		//����ʧ��
		SendOperateFailure(pUserOperateResult->szDescribeString,pUserOperateResult->dwErrorCode,pUserOperateResult->wOperateCode,dwContextID);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
