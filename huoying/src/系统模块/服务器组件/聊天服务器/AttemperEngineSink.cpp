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
//常量定义
#define TEMP_MESSAGE_ID				INVALID_DWORD			

//时间标识
#define IDI_CONNECT_CORRESPOND		1									//连接时间

//间隔时间
#define MIN_INTERVAL_TIME			10									//间隔时间
#define MAX_INTERVAL_TIME			1*60								//临时标识

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

	//状态变量
	m_pInitParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	return;
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
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
	m_pNormalParameter=new tagBindParameter[m_pInitParameter->m_wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxPlayer);

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

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIDataBaseEngine=NULL;

	//绑定数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//删除用户
	m_FriendGroupManager.DeleteFriendGroup();
	m_ServerUserManager.DeleteUserItem();

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
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));

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
	if(dwTimerID == IDI_CONNECT_CORRESPOND)
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
		CTraceService::TraceString(TEXT("正在注册游戏聊天服务器..."),TraceLevel_Normal);

		//注册聊天
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_CHAT);

		return true;
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
		m_pITimerEngine->KillTimer(IDI_CONNECT_CORRESPOND);

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

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_SERVICE_INFO:	//服务信息
			{
				return OnEventTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//用户信息
			{
				return OnEventTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //管理服务
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
	case SUB_CS_S_PROPERTY_TRUMPET:  //喇叭消息
		{
			//广播数据
			m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_TRUMPET_NOTIFY,pData,wDataSize,0xFF);
			return true;
		}
	case SUB_CS_S_HALL_MESSAGE:		//大厅消息
		{
			//广播数据
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
	case SUB_CS_S_CHAT_INSERT:	//聊天服务
		{
			//事件通知
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
	case SUB_CS_S_USER_GAMESTATE:	//聊天服务
		{
			return OnEventTCPSocketSubUserStatus(wSubCmdID, pData,wDataSize);
		}
	case SUB_CS_S_CHAT_FORCE_OFFLINE:	//强制离线
		{
			return OnEventTCPSocketSubForceOffline(wSubCmdID,pData,wDataSize);
		}
	}	

	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketSubUserStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize==sizeof(CMD_CS_S_UserGameStatus));
	if(wDataSize!=sizeof(CMD_CS_S_UserGameStatus)) return false;

	//提取数据
	CMD_CS_S_UserGameStatus * pUserGameStatus = (CMD_CS_S_UserGameStatus *)pData;
	ASSERT(pUserGameStatus!=NULL);
	if(pUserGameStatus==NULL) return false;

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserGameStatus->dwUserID);
	if(pServerUserItem==NULL ) return true;

	//获取对象
	CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

	//重包判断
	if((pUserGameStatus->cbGameStatus==pLocalUserItem->GetGameStatus()) &&
		(pUserGameStatus->wServerID==pLocalUserItem->GetServerID()) && 
		(pUserGameStatus->wTableID==pLocalUserItem->GetTableID()) )
	{
		return true;
	}

	//更改状态
	tagFriendUserInfo* pUserInfo=pLocalUserItem->GetUserInfo();
	pUserInfo->cbGameStatus=pUserGameStatus->cbGameStatus;
	pUserInfo->wKindID = pUserGameStatus->wKindID;
	pUserInfo->wServerID=pUserGameStatus->wServerID;
	pUserInfo->wTableID=pUserGameStatus->wTableID;
	pUserInfo->wChairID=pUserGameStatus->wChairID;
	lstrcpyn(pUserInfo->szServerName,pUserGameStatus->szServerName,CountArray(pUserInfo->szServerName));

	//广播状态
	CMD_GC_UserGameStatusNotify UserGameStatusNotify;
	UserGameStatusNotify.dwUserID=pUserGameStatus->dwUserID;
	UserGameStatusNotify.cbGameStatus=pUserGameStatus->cbGameStatus;
	UserGameStatusNotify.wKindID = pUserGameStatus->wKindID;
	UserGameStatusNotify.wServerID=pUserGameStatus->wServerID;
	UserGameStatusNotify.wTableID=pUserGameStatus->wTableID;
	lstrcpyn(UserGameStatusNotify.szServerName,pUserGameStatus->szServerName,CountArray(UserGameStatusNotify.szServerName));

	//广播给好友
	SendDataToUserFriend(pUserGameStatus->dwUserID,MDM_GC_USER,SUB_GC_GAME_STATUS_NOTIFY,&UserGameStatusNotify,sizeof(UserGameStatusNotify));


	return true;
}

bool CAttemperEngineSink::OnEventTCPSocketSubForceOffline(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(CMD_CS_S_ForceOffline)==0);
	if (wDataSize%sizeof(CMD_CS_S_ForceOffline)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(CMD_CS_S_ForceOffline);
	CMD_CS_S_ForceOffline * pForceOffline=(CMD_CS_S_ForceOffline *)pData;

	ASSERT(pForceOffline!=NULL);
	if(pForceOffline==NULL) return false;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//查找用户
		CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pForceOffline->dwUserID);
		if(pServerUserItem==NULL ) return true;

		//获取对象
		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;

		//通知该用户好友离线
		CMD_GC_UserOnlineStatusNotify UserOnLine;
		UserOnLine.dwUserID=pServerUserItem->GetUserID();
		UserOnLine.cbMainStatus=FRIEND_US_OFFLINE;	
		SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

		//通知客户端用户下线
		DWORD dwSocketID = pLocalUserItem->GetSocketID();
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_FORCE_OFFLINE,0,0);

		//断开连接
		//m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

		//清理用户
		m_ServerUserManager.DeleteUserItem(UserOnLine.dwUserID);

		++pForceOffline;
	}

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
	CServerUserItem * pServerUserItem =pBindParameter->pServerUserItem;

	try
	{
		//用户处理
		if (pServerUserItem!=NULL)
		{
			//用户离线
			CMD_GC_UserOnlineStatusNotify UserOnLine;
			UserOnLine.dwUserID=pServerUserItem->GetUserID();
			UserOnLine.cbMainStatus=FRIEND_US_OFFLINE;	
			SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

			//通知协调用户下线
			CMD_CS_C_ChatUserLeave UserLeave = {0};
			UserLeave.dwUserID = pServerUserItem->GetUserID();
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_CHAT_USER_LEAVE,&UserLeave,sizeof(UserLeave));

			//清理用户
			m_ServerUserManager.DeleteUserItem(UserOnLine.dwUserID);
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("关闭连接异常: dwSocketID=%d"),dwSocketID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GC_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GC_USER:		//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return true;
}


//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}




//发送配置
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

//发送数据
bool CAttemperEngineSink::SendSystemMessage(BYTE cbSendMask, LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize,0xFF);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_GC_S_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GC_USER,SUB_GC_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}


//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_PC_LOGON_USERID:		//I D 登录
	case SUB_GC_MB_LOGON_USERID:		//I D 登录
		{
			return OnTCPNetworkSubMBLogonByUserID(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_MB_LOGON_ACCOUNTS:		//I D 登录
		{
			return OnTCPNetworkSubMBLogonByAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GC_APPLYFOR_FRIEND:		//好友申请
		{
			return OnTCPNetworkSubApplyForFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_RESPOND_FRIEND:			//好友回应
		{
			return OnTCPNetworkSubRespondFriend(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_GAME:			//邀请游戏
		{
			return OnTCPNetworkSubRoomInvite(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_INVITE_PERSONAL:
		{
			return OnTCPNetworkSubInvitePersonal(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_USER_CHAT:				//用户聊天
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_SEARCH_USER:			//查找用户
		{
			return OnTCPNetworkSubSearchUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_TRUMPET:				//喇叭消息
		{
			return OnTCPNetworkSubTrumpet(pData,wDataSize,dwSocketID);
		}
	case SUB_GC_DELETE_FRIEND:			//删除好友
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


//I D 登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByUserID));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByUserID)) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//处理消息
	CMD_GC_MB_LogonByUserID * pLogonUserID=(CMD_GC_MB_LogonByUserID *)pData;

	//重复登录
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pServerUserItem->GetUserID());
		//SendLogonFailure(_T("你已经登录游戏了~"), 1, dwSocketID);
		//return false;
	}

	//登录用户插入到数组
	if (std::find(m_LogonLockList.begin(), m_LogonLockList.end(), pLogonUserID->dwUserID) == m_LogonLockList.end())
	{
		m_LogonLockList.push_back(pLogonUserID->dwUserID);
	}
	else
	{
		SendLogonFailure(_T("登录进行中，请稍后~"), 1, dwSocketID);
		return false;
	}
	

	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;
	pLogonUserID->szPhoneMode[CountArray(pLogonUserID->szPhoneMode)-1]=0;

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserID.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szPhoneMode,pLogonUserID->szPhoneMode,CountArray(LogonUserID.szPhoneMode));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));
	////查找用户
	//if(pServerUserItem!=NULL)
	//{
	//	//本地用户
	//	if(pServerUserItem->GetUserItemKind()==enLocalKind)
	//	{
	//		//变量定义
	//		CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

	//		//校验密码
	//		if(pLocalUserItem->ContrastLogonPass(pLogonUserID->szPassword)==false)
	//		{
	//			SendLogonFailure(TEXT("您输入的账号或密码不正确,请确认后重新登录！"),0,dwSocketID);

	//			return true;
	//		}

	//		//重包判断
	//		if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
	//		{
	//			if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
	//			{
	//				return true;
	//			}
	//		}

	//		//发送下线消息
	//		if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE && (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
	//		{
	//			if(pLocalUserItem->GetSocketID()!=dwSocketID)
	//			{
	//				//TCHAR szMessage[256]=TEXT("");
	//				//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
	//				//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

	//				////错误信息
	//				//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

	//				//构造结构
	//				CMD_GC_UserLogonAfresh UserLogonAfresh;
	//				_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("您的账号在别处登录,您被迫下线！"));

	//				//发送数据
	//				WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
	//				SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);
	//			}
	//		}			

	//		//修改用户状态
	//		pLocalUserItem->SetSocketID(dwSocketID);
	//		pLocalUserItem->SetLogonTime(GetTickCount());
	//		pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
	//		pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
	//	}
	//}	

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));


	return true;
}

//账号 登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_MB_LogonByAccounts));
	if (wDataSize>sizeof(CMD_GC_MB_LogonByAccounts)) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//处理消息
	CMD_GC_MB_LogonByAccounts * pLogonAccounts=(CMD_GC_MB_LogonByAccounts *)pData;


	//重复登录
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if(pServerUserItem!=NULL)
	{
		m_ServerUserManager.DeleteUserItem(pServerUserItem->GetUserID());
		//SendLogonFailure(_T("你已经登录游戏了~"), 1, dwSocketID);
		//return false;
	}


	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szPhoneMode[CountArray(pLogonAccounts->szPhoneMode)-1]=0;

	//变量定义
	DBR_GR_LogonUserAccounts LogonUserAccounts;
	ZeroMemory(&LogonUserAccounts,sizeof(LogonUserAccounts));

	//构造数据
	lstrcpyn(LogonUserAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonUserAccounts.szAccounts));
	LogonUserAccounts.dwClientAddr=pBindParameter->dwClientAddr;
	LogonUserAccounts.wLogonComand=LOGON_COMMAND_USERINFO|LOGON_COMMAND_GROUPINFO|LOGON_COMMAND_FRIENDS;
	lstrcpyn(LogonUserAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonUserAccounts.szPassword));
	lstrcpyn(LogonUserAccounts.szPhoneMode,pLogonAccounts->szPhoneMode,CountArray(LogonUserAccounts.szPhoneMode));
	lstrcpyn(LogonUserAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonUserAccounts.szMachineID));

	//查找用户
	pServerUserItem = m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if(pServerUserItem!=NULL)
	{
		//本地用户
		if(pServerUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pLocalUserItem = (CLocalUserItem *)pServerUserItem;			

			//校验密码
			if(pLocalUserItem->ContrastLogonPass(pLogonAccounts->szPassword)==false)
			{
				SendLogonFailure(TEXT("您输入的账号或密码不正确,请确认后重新登录！"),0,dwSocketID);

				return true;
			}

			//重包判断
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				if(GetTickCount()-pLocalUserItem->GetLogonTime()< LOGON_TIME_SPACE)
				{
					return true;
				}
			}

			//发送下线消息
			if(pLocalUserItem->GetMainStatus()!=FRIEND_US_OFFLINE && (pServerUserItem->GetClientAddr()!=pBindParameter->dwClientAddr ))
			{
				if(pLocalUserItem->GetSocketID()!=dwSocketID)
				{
					//TCHAR szMessage[256]=TEXT("");
					//_sntprintf(szMessage,CountArray(szMessage),TEXT("dwSocketID0=%d,Address0=%d,dwSocketID1=%d,Address1=%d"),
					//	pLocalUserItem->GetSocketID(),pLocalUserItem->GetClientAddr(),dwSocketID,pBindParameter->dwClientAddr);

					////错误信息
					//CTraceService::TraceString(szMessage,TraceLevel_Normal);	

					//构造结构
					CMD_GC_UserLogonAfresh UserLogonAfresh;
					_sntprintf(UserLogonAfresh.wNotifyMessage,CountArray(UserLogonAfresh.wNotifyMessage),TEXT("您的账号在别处登录,您被迫下线！"));

					//发送数据
					WORD wSize = sizeof(UserLogonAfresh)-sizeof(UserLogonAfresh.wNotifyMessage)+CountStringBuffer(UserLogonAfresh.wNotifyMessage);
					SendData(pLocalUserItem->GetSocketID(),MDM_GC_LOGON,SUB_S_LOGON_AFRESH,&UserLogonAfresh,wSize);
				}
			}			

			//修改用户状态
			pLocalUserItem->SetSocketID(dwSocketID);
			pLocalUserItem->SetLogonTime(GetTickCount());
			pLocalUserItem->GetUserInfo()->cbMainStatus=FRIEND_US_ONLINE;
			pLocalUserItem->GetUserInfo()->dwClientAddr=pBindParameter->dwClientAddr;
		}
	}	

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_ACCOUNTS,dwSocketID,&LogonUserAccounts,sizeof(LogonUserAccounts));


	return true;
}

//好友申请
bool CAttemperEngineSink::OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GC_ApplyForFriend));
	if (wDataSize!=sizeof(CMD_GC_ApplyForFriend)) return false;

	//处理消息
	CMD_GC_ApplyForFriend * pApplyForFriend=(CMD_GC_ApplyForFriend *)pData;
	ASSERT(pApplyForFriend!=NULL);

	//查找对象
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pApplyForFriend->dwUserID);
	if(pUserFriendGroup!=NULL) 
	{
		tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->SearchFriendItem(pApplyForFriend->dwFriendID);
		if(pServerFriendInfo!=NULL && pServerFriendInfo->cbGroupID!=0) 
		{
			//申请反馈
			CMD_GC_ApplyForFriendEcho ApplyForFriend;
			ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
			ApplyForFriend.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(ApplyForFriend.szDescribeString,_T("已经是好友，无需申请"),CountArray(ApplyForFriend.szDescribeString));
			WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
			WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);

			return true;
		}
	}

	//查找用户
	CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwFriendID);

	//变量定义
	bool bBufferMessage=false;
	if(pFriendUserItem==NULL) bBufferMessage=true;
	if(pFriendUserItem && pFriendUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true; 

	if(pFriendUserItem!=NULL)
	{
		//TCHAR szMessage[256]=TEXT("");
		//_sntprintf(szMessage,CountArray(szMessage),TEXT(".........%d申请加%d为好友,%d当前状态为%d"),pApplyForFriend->dwUserID,pApplyForFriend->dwFriendID,pApplyForFriend->dwFriendID,pFriendUserItem->GetMainStatus());

		////错误信息
		//CTraceService::TraceString(szMessage,TraceLevel_Normal);	
	}

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pApplyForFriend->dwUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//构造结构
	CMD_GC_ApplyForNotify ApplyForNotify;
	ApplyForNotify.dwRequestID = pApplyForFriend->dwUserID;
	ApplyForNotify.cbGroupID=pApplyForFriend->cbGroupID;
	lstrcpyn(ApplyForNotify.szNickName,((CLocalUserItem *)pServerUserItem)->GetNickName(),CountArray(ApplyForNotify.szNickName));

	//缓冲判断
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

	//申请反馈
	CMD_GC_ApplyForFriendEcho ApplyForFriend;
	ZeroMemory(&ApplyForFriend,sizeof(ApplyForFriend));
	ApplyForFriend.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(ApplyForFriend.szDescribeString,_T(""),CountArray(ApplyForFriend.szDescribeString));
	WORD wDataSize2=CountStringBuffer(ApplyForFriend.szDescribeString);
	WORD wHeadSize=sizeof(ApplyForFriend)-sizeof(ApplyForFriend.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_APPLYFOR_FRIEND_ECHO,&ApplyForFriend,wHeadSize+wDataSize2);


	return true;
}

//好友回应
bool CAttemperEngineSink::OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GC_RespondFriend));
	if (wDataSize!=sizeof(CMD_GC_RespondFriend)) return false;

	//处理消息
	CMD_GC_RespondFriend * pRespondFriend=(CMD_GC_RespondFriend *)pData;
	ASSERT(pRespondFriend!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pRespondFriend->dwRequestID);
	if(pServerUserItem==NULL ) 
	{
		//回应反馈
		CMD_GC_RespondFriendEcho RespondFriendEcho;
		ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
		RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(RespondFriendEcho.szDescribeString,_T("未找到此用户"),CountArray(RespondFriendEcho.szDescribeString));
		WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
		WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
		return true;
	}

	//查找对象
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pRespondFriend->dwRequestID);
	if(pUserFriendGroup!=NULL) 
	{
		if(pUserFriendGroup->SearchFriendItem(pRespondFriend->dwUserID)!=NULL) 
		{
			//回应反馈
			CMD_GC_RespondFriendEcho RespondFriendEcho;
			ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
			RespondFriendEcho.lErrorCode=CHAT_MSG_ERR;
			lstrcpyn(RespondFriendEcho.szDescribeString,_T("已经是好友"),CountArray(RespondFriendEcho.szDescribeString));
			WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
			WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
			SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);
			return true;
		}
	}

	//接受申请
	if(pRespondFriend->bAccepted==true)
	{
		//构造结构
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

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ADD_FRIEND,dwSocketID,&AddFriend,sizeof(AddFriend));
	}
	else
	{
		//变量定义
		bool bBufferMessage=false;
		if(pRequestUserItem==NULL) bBufferMessage=true;
		if(pRequestUserItem && pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE) bBufferMessage=true;

		//构造结构
		CMD_GC_RespondNotify RespondNotify;
		_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s 拒绝了您的好友申请！"),((CLocalUserItem *)pServerUserItem)->GetNickName());

		//缓存消息
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

	//回应反馈
	CMD_GC_RespondFriendEcho RespondFriendEcho;
	ZeroMemory(&RespondFriendEcho,sizeof(RespondFriendEcho));
	RespondFriendEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(RespondFriendEcho.szDescribeString,_T(""),CountArray(RespondFriendEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(RespondFriendEcho.szDescribeString);
	WORD wHeadSize=sizeof(RespondFriendEcho)-sizeof(RespondFriendEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_RESPOND_FRIEND_ECHO,&RespondFriendEcho,wHeadSize+wDataSize2);

	return true;
}

//邀请游戏
bool CAttemperEngineSink::OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_InviteGame));
	if (wDataSize>sizeof(CMD_GC_InviteGame)) return false;

	//处理消息
	CMD_GC_InviteGame * pVipRoomInvite=(CMD_GC_InviteGame *)pData;
	ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_InviteGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_InviteGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.wServerID=pVipRoomInvite->wServerID;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//发送数据
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_GAME_NOTIFY,&VipRoomInviteNotify,wSize);

	//回应反馈
	CMD_GC_InviteGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_GAME_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//邀请游戏
bool CAttemperEngineSink::OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_InvitePersonalGame));
	if (wDataSize>sizeof(CMD_GC_InvitePersonalGame)) return false;

	//处理消息
	CMD_GC_InvitePersonalGame * pVipRoomInvite=(CMD_GC_InvitePersonalGame *)pData;
	ASSERT(pVipRoomInvite!=NULL);
	pVipRoomInvite->szInviteMsg[128]='\0';

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pInvitedUserItem = m_ServerUserManager.SearchUserItem(pVipRoomInvite->dwInvitedUserID);
	if(pInvitedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pInvitedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_InvitePersonalGameEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_InvitePersonalGameNotify VipRoomInviteNotify;
	VipRoomInviteNotify.dwInviteUserID=pVipRoomInvite->dwUserID;
	VipRoomInviteNotify.wKindID = pVipRoomInvite->wKindID;
	VipRoomInviteNotify.dwServerNumber=pVipRoomInvite->dwServerNumber;
	VipRoomInviteNotify.wTableID=pVipRoomInvite->wTableID;
	lstrcpyn(VipRoomInviteNotify.szInviteMsg,pVipRoomInvite->szInviteMsg,CountArray(VipRoomInviteNotify.szInviteMsg));

	//发送数据
	WORD wSize = sizeof(VipRoomInviteNotify);
	SendData(((CLocalUserItem *)pInvitedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_INVITE_PERSONAL_NOTIFY,&VipRoomInviteNotify,wSize);

	//回应反馈
	CMD_GC_InvitePersonalGameEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_INVITE_PERSONAL_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}


//用户聊天
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_UserChat));
	if (wDataSize>sizeof(CMD_GC_UserChat)) return false;

	//处理消息
	CMD_GC_UserChat * pUserChat=(CMD_GC_UserChat *)pData;
	ASSERT(pUserChat!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSenderID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//聊天反馈
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("信息超时"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);
		return false;
	}


	//查找用户
	CServerUserItem * pTargetUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
	if(pTargetUserItem!=NULL)
	{
		CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pTargetUserItem;
		SendData(pLocalUerItem->GetSocketID(),MDM_GC_USER,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize);

		//聊天反馈
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
		//聊天反馈
		CMD_GC_UserChatEcho UserChatEcho;
		ZeroMemory(&UserChatEcho,sizeof(UserChatEcho));
		UserChatEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(UserChatEcho.szDescribeString,_T("发送失败，对方已经离线"),CountArray(UserChatEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(UserChatEcho.szDescribeString);
		WORD wHeadSize=sizeof(UserChatEcho)-sizeof(UserChatEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_CHAT_ECHO,&UserChatEcho,wHeadSize+wDataSize);

		////保存消息
		//SaveOfflineMessage(pUserChat->dwTargetUserID,SUB_GC_USER_CHAT_NOTIFY,pData,wDataSize,dwSocketID);
	}

	return true;
}

//查找用户
bool CAttemperEngineSink::OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_SearchByGameID));
	if (wDataSize>sizeof(CMD_GC_SearchByGameID)) return false;

	//处理消息
	CMD_GC_SearchByGameID * pSearchUser=(CMD_GC_SearchByGameID *)pData;
	ASSERT(pSearchUser!=NULL);

	//变量定义
	DBR_GR_SearchUser SearchUser;
	SearchUser.dwSearchByGameID = pSearchUser->dwSearchByGameID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_USER,dwSocketID,&SearchUser,sizeof(SearchUser));

	return true;
}

//发送喇叭
bool CAttemperEngineSink::OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_Trumpet));
	if (wDataSize>sizeof(CMD_GC_Trumpet)) return false;

	//处理消息
	CMD_GC_Trumpet * pUserChat=(CMD_GC_Trumpet *)pData;
	ASSERT(pUserChat!=NULL);

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserChat->dwSendUserID);
	if(pServerUserItem==NULL ) 
	{
		//喇叭反馈
		CMD_GC_TrumpetEcho TrumpetEcho;
		ZeroMemory(&TrumpetEcho,sizeof(TrumpetEcho));
		TrumpetEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(TrumpetEcho.szDescribeString,_T("信息超时"),CountArray(TrumpetEcho.szDescribeString));
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

	//投递数据
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_TRUMPET, dwSocketID, &Trumpet, sizeof(Trumpet));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_DeleteFriend));
	if (wDataSize>sizeof(CMD_GC_DeleteFriend)) return false;

	//处理消息
	CMD_GC_DeleteFriend * pDeleteFriend=(CMD_GC_DeleteFriend *)pData;
	ASSERT(pDeleteFriend!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//删除反馈
		CMD_GC_DeleteFriendEcho DeleteEcho;
		ZeroMemory(&DeleteEcho,sizeof(DeleteEcho));
		DeleteEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(DeleteEcho.szDescribeString,_T("信息超时"),CountArray(DeleteEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(DeleteEcho.szDescribeString);
		WORD wHeadSize=sizeof(DeleteEcho)-sizeof(DeleteEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_ECHO,&DeleteEcho,wHeadSize+wDataSize);
		return false;
	}

	//构造结构
	DBR_GR_DeleteFriend DeleteFriend;
	DeleteFriend.dwUserID=pDeleteFriend->dwUserID;
	DeleteFriend.dwFriendUserID=pDeleteFriend->dwFriendUserID;
	DeleteFriend.cbGroupID=pDeleteFriend->cbGroupID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_FRIEND,dwSocketID,&DeleteFriend,sizeof(DeleteFriend));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_Update_Coordinate));
	if (wDataSize>sizeof(CMD_GC_Update_Coordinate)) return false;

	//处理消息
	CMD_GC_Update_Coordinate * pUpdateCoordinate=(CMD_GC_Update_Coordinate *)pData;
	ASSERT(pUpdateCoordinate!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUpdateCoordinate->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		//更新反馈
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	if (pUpdateCoordinate->dLatitude>180 || pUpdateCoordinate->dLongitude>180)
	{
		//更新反馈
		CMD_GC_Update_CoordinateEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("坐标错误"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_UPDATE_COORDINATE_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}

	//更新定位
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
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_Get_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Get_Nearuser)) return false;

	//处理消息
	CMD_GC_Get_Nearuser * pGetNearuser=(CMD_GC_Get_Nearuser *)pData;
	ASSERT(pGetNearuser!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pGetNearuser->dwUserID);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Get_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_GET_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return false;
	}

	//获取附近
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
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_Query_Nearuser));
	if (wDataSize>sizeof(CMD_GC_Query_Nearuser)) return false;

	//处理消息
	CMD_GC_Query_Nearuser * pQueryNearuser=(CMD_GC_Query_Nearuser *)pData;
	ASSERT(pQueryNearuser!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pQueryNearuser->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		CMD_GC_Query_NearuserEcho Echo;
		ZeroMemory(&Echo,sizeof(Echo));
		Echo.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(Echo.szDescribeString,_T("信息超时"),CountArray(Echo.szDescribeString));
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
		lstrcpyn(Echo.szDescribeString,_T("请求失败，对方已经离线"),CountArray(Echo.szDescribeString));
		WORD wDataSize=CountStringBuffer(Echo.szDescribeString);
		WORD wHeadSize=sizeof(Echo)-sizeof(Echo.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_QUERY_NEARUSER_ECHO,&Echo,wHeadSize+wDataSize);
		return true;
	}
	//获取附近
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

//分享消息
bool CAttemperEngineSink::OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GC_UserShare));
	if (wDataSize>sizeof(CMD_GC_UserShare)) return false;

	//处理消息
	CMD_GC_UserShare * pUserShare=(CMD_GC_UserShare *)pData;
	ASSERT(pUserShare!=NULL);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("信息超时"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return false;
	}

	//查找用户
	CServerUserItem * pSharedUserItem = m_ServerUserManager.SearchUserItem(pUserShare->dwSharedUserID);
	if(pSharedUserItem==NULL ) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("未找到此用户"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);

		return true;
	}

	//离线检查
	if(pSharedUserItem->GetMainStatus()==FRIEND_US_OFFLINE) 
	{
		//邀请反馈
		CMD_GC_UserShareEcho InviteGameEcho;
		ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
		InviteGameEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(InviteGameEcho.szDescribeString,_T("用户已经离线"),CountArray(InviteGameEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(InviteGameEcho.szDescribeString);
		WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
		SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize);
		return true;
	}

	//发送邀请
	CMD_GC_UserShareNotify UserShareNotify;
	UserShareNotify.dwShareUserID=pUserShare->dwUserID;
	lstrcpyn(UserShareNotify.szShareImageAddr,pUserShare->szShareImageAddr,CountArray(UserShareNotify.szShareImageAddr));
	lstrcpyn(UserShareNotify.szMessageContent,pUserShare->szMessageContent,CountArray(UserShareNotify.szMessageContent));

	//发送数据
	SendData(((CLocalUserItem *)pSharedUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_USER_SHARE_NOTIFY,&UserShareNotify,sizeof(UserShareNotify));

	//回应反馈
	CMD_GC_UserShareEcho InviteGameEcho;
	ZeroMemory(&InviteGameEcho,sizeof(InviteGameEcho));
	InviteGameEcho.lErrorCode=CHAT_MSG_OK;
	lstrcpyn(InviteGameEcho.szDescribeString,_T(""),CountArray(InviteGameEcho.szDescribeString));
	WORD wDataSize2=CountStringBuffer(InviteGameEcho.szDescribeString);
	WORD wHeadSize=sizeof(InviteGameEcho)-sizeof(InviteGameEcho.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_USER_SHARE_ECHO,&InviteGameEcho,wHeadSize+wDataSize2);


	return true;
}

//登录失败
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//变量定义
	CMD_GC_LogonEcho LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//构造数据
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//发送数据
	SendData(dwSocketID,MDM_GC_LOGON,SUB_GC_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//操作结果
bool CAttemperEngineSink::SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID)
{
	//变量定义
	CMD_GC_OperateFailure OperateFailure;
	OperateFailure.lErrorCode=lErrorCode;
	OperateFailure.lResultCode=lOperateCode;
	lstrcpyn(OperateFailure.szDescribeString,pszString,CountArray(OperateFailure.szDescribeString));

	//发送数据
	SendData(dwSocketID,MDM_GC_USER,SUB_GP_OPERATE_FAILURE,&OperateFailure,sizeof(OperateFailure));

	return true;
}

//操作结果
bool CAttemperEngineSink::SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID)
{
	//构造结构
	CMD_GC_OperateSuccess OperateSuccess;
	OperateSuccess.lResultCode=lOperateCode;
	lstrcpyn(OperateSuccess.szDescribeString,pszString,CountArray(OperateSuccess.szDescribeString));

	//发送数据
	WORD wSize = sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+CountStringBuffer(OperateSuccess.szDescribeString);
	SendData(dwSocketID,MDM_GC_USER,SUB_GC_OPERATE_SUCCESS,&OperateSuccess,wSize);

	return true;
}


//发送数据
bool CAttemperEngineSink::SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize)
{
	//移除用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(dwUserID);
	ASSERT(pServerUserItem!=NULL);
	if(pServerUserItem!=NULL)
	{
		//获取好友
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(dwUserID);
		if(pUserFriendGroup!=NULL)
		{
			//枚举用户
			WORD wEnumIndex=0;
			tagServerFriendInfo * pServerFriendInfo = pUserFriendGroup->EnumFriendItem(wEnumIndex);
			while(pServerFriendInfo!=NULL)
			{
				//查找本地用户
				CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pServerFriendInfo->dwUserID);
				if(pServerUserItem!=NULL )
				{
					CLocalUserItem * pLocalUerItem = (CLocalUserItem *)pServerUserItem;					
					if(pLocalUerItem->GetMainStatus()!=FRIEND_US_OFFLINE)
					{
						SendData(pLocalUerItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
					}
				}

				//枚举好友
				pServerFriendInfo = pUserFriendGroup->EnumFriendItem(++wEnumIndex);
			}			
		}
	}

	return true;
}

//存储离线消息
bool CAttemperEngineSink::SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID)
{
	//变量定义
	DBR_GR_SaveOfflineMessage OfflineMessage;
	OfflineMessage.dwUserID = dwUserID;
	OfflineMessage.wMessageType = wMessageType;
	OfflineMessage.wDataSize = wDataSize;
	OfflineMessage.szOfflineData[0]=0;

	//变量定义
	TCHAR szOffLineData[CountArray(OfflineMessage.szOfflineData)+1]=TEXT("");
	BYTE cbOffLineData[CountArray(OfflineMessage.szOfflineData)/2];
	ZeroMemory(cbOffLineData,sizeof(cbOffLineData));
	CopyMemory(cbOffLineData,pData,wDataSize);		

	//离线消息
	for (INT i=0;i<CountArray(cbOffLineData);i++) 
	{
		_stprintf(&szOffLineData[i*2],TEXT("%02X"),cbOffLineData[i]);
	}

	//设置变量
	lstrcpyn(OfflineMessage.szOfflineData,szOffLineData,CountArray(OfflineMessage.szOfflineData));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SAVE_OFFLINEMESSAGE,dwSocketID,&OfflineMessage,sizeof(OfflineMessage));

	return true;
}

//绑定用户
CServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//获取参数
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pServerUserItem;
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
	if (wBindIndex<m_pInitParameter->m_wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
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
		case DBO_GR_ACCOUNT_LOGON_FAILURE:
			{
				return OnDBAccountLogonFailure(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_GROUPINFO:			//加载分组
			{
				return OnDBLoadUserGroup(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_FRIENDINFO:		//加载好友
			{
				return OnDBLoadUserFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_SINDIVIDUAL:		//简易资料
			{
				return OnDBLoadUserSimpleIndividual(dwContextID,pData,wDataSize);
			}
		case DBO_GR_USER_REMARKS:			//用户备注
			{
				return OnDBLoadUserRemarksInfo(dwContextID,pData,wDataSize);
			}
		case DBO_GR_LOAD_OFFLINEMESSAGE:	//加载消息
			{
				return OnDBLoadUserOfflineMessage(dwContextID,pData,wDataSize);
			}
		case DBO_GR_SEARCH_USER_RESULT:		//查找结果
			{
				return OnDBSearchUserResult(dwContextID,pData,wDataSize);
			}
		case DBO_GR_ADD_FRIEND:				//添加好友
			{
				return OnDBUserAddFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_DELETE_FRIEND:			//删除好友
			{
				return OnDBLoadDeleteFriend(dwContextID,pData,wDataSize);
			}
		case DBO_GR_OPERATE_RESULT:			//操作结果
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

		//错误信息
		CTraceService::TraceString(szMessage,TraceLevel_Exception);
	}

	return false;
}

bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_LogonSuccess));
	if(wDataSize>sizeof(DBO_GR_LogonSuccess)) return false;

	//变量定义
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	////重复登录
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

	//用户变量
	tagFriendUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//基本资料
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
	
	//用户状态
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

	//用户资料	
	lstrcpyn(UserInfo.szQQ,pDBOLogonSuccess->szQQ,CountArray(UserInfo.szQQ));
	lstrcpyn(UserInfo.szEMail,pDBOLogonSuccess->szEMail,CountArray(UserInfo.szEMail));
	lstrcpyn(UserInfo.szSeatPhone,pDBOLogonSuccess->szSeatPhone,CountArray(UserInfo.szSeatPhone));
	lstrcpyn(UserInfo.szMobilePhone,pDBOLogonSuccess->szMobilePhone,CountArray(UserInfo.szMobilePhone));
	lstrcpyn(UserInfo.szDwellingPlace,pDBOLogonSuccess->szDwellingPlace,CountArray(UserInfo.szDwellingPlace));
	lstrcpyn(UserInfo.szPostalCode,pDBOLogonSuccess->szPostalCode,CountArray(UserInfo.szPostalCode));

	//变量定义
	tagInsertLocalUserInfo InsertLocalUserInfo;
	InsertLocalUserInfo.dwSocketID=dwContextID;
	InsertLocalUserInfo.dwLogonTime=GetTickCount();

	//激活用户
	m_ServerUserManager.InsertLocalUserItem(InsertLocalUserInfo,UserInfo,pDBOLogonSuccess->szPassword);

	//发送数据
	CMD_GC_LogonEcho LogonSuccess;
	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));
	WORD wLogonDataSize = sizeof(LogonSuccess);
	SendData(dwContextID,MDM_GC_LOGON,SUB_GC_LOGON_SUCCESS,&LogonSuccess,wLogonDataSize);

	//设置用户
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	pBindParameter->pServerUserItem=pServerUserItem;

	//用户上线
	CMD_GC_UserOnlineStatusNotify UserOnLine;
	UserOnLine.dwUserID=pServerUserItem->GetUserID();
	UserOnLine.cbMainStatus=pServerUserItem->GetMainStatus();	
	SendDataToUserFriend(UserOnLine.dwUserID,MDM_GC_USER,SUB_GC_USER_STATUS_NOTIFY,&UserOnLine,sizeof(UserOnLine));

	//通知协调用户上线
	CMD_CS_C_ChatUserEnter UserEnter = {0};
	UserEnter.dwUserID = pServerUserItem->GetUserID();
	lstrcpyn(UserEnter.szNickName, UserInfo.szNickName, CountArray(UserEnter.szNickName));
	m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_CHAT_USER_ENTER,&UserEnter,sizeof(UserEnter));

	//TCHAR szMessage[256]=TEXT("");
	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%d-----上线通知"),UserOnLine.dwUserID);
	//CTraceService::TraceString(szMessage,TraceLevel_Normal);

	//允许群发
	m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true,0xFF);

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//发送错误
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	std::list<DWORD>::iterator it = std::find(m_LogonLockList.begin(), m_LogonLockList.end(), pLogonFailure->dwUserID);
	if (it != m_LogonLockList.end())
	{
		m_LogonLockList.erase(it);
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBAccountLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//发送错误
	DBO_GR_AccountLogonFailure * pLogonFailure=(DBO_GR_AccountLogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	return true;
}



//加载分组
bool CAttemperEngineSink::OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_UserGroupInfo));
	if(wDataSize>sizeof(DBO_GR_UserGroupInfo)) return false;

	//提取数据
	DBO_GR_UserGroupInfo * pDBOUserGroupInfo = (DBO_GR_UserGroupInfo *)pData;
	ASSERT(pDBOUserGroupInfo);

	//变量定义
	tagClientGroupInfo * pTempGroupInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagClientGroupInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserGroupInfo->wGroupCount;++nIndex)
	{
		//设置变量
		pTempGroupInfo = &pDBOUserGroupInfo->GroupInfo[nIndex];

		//变量定义		
		tagClientGroupInfo * pSendGroupInfo=(tagClientGroupInfo *)&cbDataBuffer;

		//设置变量
		pSendGroupInfo->cbGroupIndex=pTempGroupInfo->cbGroupIndex;
		lstrcpyn(pSendGroupInfo->szGroupName,pTempGroupInfo->szGroupName,CountArray(pSendGroupInfo->szGroupName));

		//设置变量
		wPacketSize = sizeof(tagClientGroupInfo)-sizeof(pSendGroupInfo->szGroupName)+CountStringBuffer(pSendGroupInfo->szGroupName);

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_GROUP,cbSendBuffer,wDataIndex);			
	}


	return true;
}

//加载好友
bool CAttemperEngineSink::OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_UserFriendInfo));
	if(wDataSize>sizeof(DBO_GR_UserFriendInfo)) return false;

	//提取数据
	DBO_GR_UserFriendInfo * pUserFriendInfo = (DBO_GR_UserFriendInfo *)pData;
	ASSERT(pUserFriendInfo);

	//在线信息
	for(WORD wIndex=0;wIndex<pUserFriendInfo->wFriendCount;++wIndex)
	{
		//查找用户
		CServerUserItem * pServerUserItem=m_ServerUserManager.SearchUserItem(pUserFriendInfo->FriendInfo[wIndex].dwUserID);
		if(pServerUserItem==NULL)
		{
			//设置状态
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

		//设置变量
		pUserFriendInfo->FriendInfo[wIndex].cbMainStatus=pServerUserItem->GetMainStatus();
		pUserFriendInfo->FriendInfo[wIndex].cbGameStatus=pServerUserItem->GetGameStatus();
		pUserFriendInfo->FriendInfo[wIndex].wKindID=pUserInfo->wKindID;
		pUserFriendInfo->FriendInfo[wIndex].wServerID=pUserInfo->wServerID;
		pUserFriendInfo->FriendInfo[wIndex].wTableID=pUserInfo->wTableID;
		pUserFriendInfo->FriendInfo[wIndex].wChairID=pUserInfo->wChairID;
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szServerName,pUserInfo->szServerName,CountArray(pUserFriendInfo->FriendInfo[wIndex].szServerName));
		lstrcpyn(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode,pUserInfo->szPhoneMode,CountArray(pUserFriendInfo->FriendInfo[wIndex].szPhoneMode));
	}

	//变量定义
	WORD wFriendCount=pUserFriendInfo->wFriendCount;
	CMD_GC_UserFriendInfo IPCUserFriendInfo;


	//循环发送
	while(wFriendCount>CountArray(IPCUserFriendInfo.FriendInfo))
	{
		//拷贝数据
		IPCUserFriendInfo.wFriendCount=CountArray(IPCUserFriendInfo.FriendInfo);
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*IPCUserFriendInfo.wFriendCount);

		//发送数据
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,sizeof(IPCUserFriendInfo));

		//设置变量
		wFriendCount -= CountArray(IPCUserFriendInfo.FriendInfo);
	}

	//剩余发送
	if(wFriendCount>0)
	{
		//拷贝数据
		IPCUserFriendInfo.wFriendCount=wFriendCount;
		CopyMemory(IPCUserFriendInfo.FriendInfo,&pUserFriendInfo->FriendInfo[pUserFriendInfo->wFriendCount-wFriendCount],sizeof(tagClientFriendInfo)*wFriendCount);

		//发送数据
		WORD wPacketDataSize = sizeof(IPCUserFriendInfo)-sizeof(IPCUserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*wFriendCount;

		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_FRIEND,&IPCUserFriendInfo,wPacketDataSize);

	}

	if(pUserFriendInfo->bLogonFlag==true)
	{
		//登录完成
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_LOGON_FINISH);

		////提示信息
		//CTraceService::TraceString(TEXT("发送上线同步信息"),TraceLevel_Normal);
	}

	//变量定义
	tagServerFriendInfo ServerFriendInfo;

	//插入好友
	CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pUserFriendInfo->dwUserID);
	if(pUserFriendGroup==NULL)
	{
		//激活分组
		pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(pUserFriendInfo->wFriendCount);
		m_FriendGroupManager.InsertFriendGroup(pUserFriendInfo->dwUserID,pUserFriendGroup);
	}

	if(pUserFriendGroup!=NULL)
	{
		//重置分组
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

//好友备注
bool CAttemperEngineSink::OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_UserRemarksInfo));
	if(wDataSize>sizeof(DBO_GR_UserRemarksInfo)) return false;

	//提取数据
	DBO_GR_UserRemarksInfo * pDBOUserRemarksInfo = (DBO_GR_UserRemarksInfo *)pData;
	ASSERT(pDBOUserRemarksInfo);

	//变量定义
	tagUserRemarksInfo * pUserRemarksInfo=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[sizeof(tagUserRemarksInfo)];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserRemarksInfo->wFriendCount;++nIndex)
	{
		//设置变量
		pUserRemarksInfo = &pDBOUserRemarksInfo->RemarksInfo[nIndex];

		//变量定义		
		tagUserRemarksInfo * pSendUserRemarksInfo=(tagUserRemarksInfo *)&cbDataBuffer;

		//设置变量
		pSendUserRemarksInfo->dwFriendUserID=pUserRemarksInfo->dwFriendUserID;
		lstrcpyn(pSendUserRemarksInfo->szRemarksInfo,pUserRemarksInfo->szRemarksInfo,CountArray(pSendUserRemarksInfo->szRemarksInfo));

		//设置变量
		wPacketSize = sizeof(tagUserRemarksInfo)-sizeof(pSendUserRemarksInfo->szRemarksInfo)+CountStringBuffer(pSendUserRemarksInfo->szRemarksInfo);

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(BYTE)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(BYTE));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(BYTE)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(BYTE)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_REMARKS,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//好友资料
bool CAttemperEngineSink::OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_UserIndividual));
	if(wDataSize>sizeof(DBO_GR_UserIndividual)) return false;

	//提取数据
	DBO_GR_UserIndividual * pDBOUserSimpleIndividual = (DBO_GR_UserIndividual *)pData;
	ASSERT(pDBOUserSimpleIndividual);

	//变量定义
	tagUserIndividual * pUserSimpleIndividual=NULL;
	BYTE cbSendBuffer[SOCKET_TCP_PACKET];
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	WORD wDataIndex=0,wPacketSize=0;

	//循环发送
	for(int nIndex=0;nIndex<pDBOUserSimpleIndividual->wUserCount;++nIndex)
	{
		//设置变量
		pUserSimpleIndividual = &pDBOUserSimpleIndividual->UserIndividual[nIndex];

		//变量定义		
		CMD_GC_UserIndividual * pUserIndividual=(CMD_GC_UserIndividual *)&cbDataBuffer;
		CSendPacketHelper SendPacket(pUserIndividual+1,sizeof(cbDataBuffer)-sizeof(CMD_GC_UserIndividual));

		//构造结构
		pUserIndividual->dwUserID = pUserSimpleIndividual->dwUserID;

		//设置变量
		wPacketSize = sizeof(CMD_GC_UserIndividual);

		//QQ号码
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

		//固定电话
		if (pUserSimpleIndividual->szSeatPhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szSeatPhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEATPHOHE);
		}


		//手机号码
		if (pUserSimpleIndividual->szMobilePhone[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szMobilePhone);
			SendPacket.AddPacket(pUserSimpleIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILEPHONE);			
		}

		//联系地址
		if (pUserSimpleIndividual->szDwellingPlace[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szDwellingPlace);
			SendPacket.AddPacket(pUserSimpleIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLINGPLACE);
		}

		//邮政编码
		if (pUserSimpleIndividual->szPostalCode[0]!=0)
		{
			WORD wBufferSize=CountStringBuffer(pUserSimpleIndividual->szPostalCode);
			SendPacket.AddPacket(pUserSimpleIndividual->szPostalCode,wBufferSize,DTP_GP_UI_POSTALCODE);
		}

		//设置变量
		wPacketSize += SendPacket.GetDataSize();

		//发送判断
		if(wDataIndex+wPacketSize+sizeof(WORD)>CountArray(cbSendBuffer))
		{			
			//发送消息
			SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);

			//设置变量
			ZeroMemory(cbSendBuffer,sizeof(cbSendBuffer));
			wDataIndex = 0;
		}

		//拷贝数据
		CopyMemory(&cbSendBuffer[wDataIndex],&wPacketSize,sizeof(WORD));
		CopyMemory(&cbSendBuffer[wDataIndex+sizeof(WORD)],cbDataBuffer,wPacketSize);

		//设置变量
		wDataIndex += sizeof(WORD)+wPacketSize;
	}

	//剩余发送
	if(wDataIndex>0)
	{
		//发送消息
		SendData(dwContextID,MDM_GC_LOGON,SUB_S_USER_SINDIVIDUAL,cbSendBuffer,wDataIndex);			
	}

	return true;
}

//离线消息
bool CAttemperEngineSink::OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_UserOfflineMessage));
	if(wDataSize>sizeof(DBO_GR_UserOfflineMessage)) return false;

	//提取数据
	DBO_GR_UserOfflineMessage * pUserUserOfflineMessage = (DBO_GR_UserOfflineMessage *)pData;
	ASSERT(pUserUserOfflineMessage);

	//变量定义
	BYTE cbOfflineData[CountArray(pUserUserOfflineMessage->szOfflineData)];
	ZeroMemory(cbOfflineData,sizeof(cbOfflineData));

	//解析数据	
	if (pUserUserOfflineMessage->szOfflineData[0]!=0)
	{
		//获取长度
		INT nCustomRuleSize=lstrlen(pUserUserOfflineMessage->szOfflineData)/2;

		//转换字符
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//获取字符
			TCHAR cbChar1=pUserUserOfflineMessage->szOfflineData[i*2];
			TCHAR cbChar2=pUserUserOfflineMessage->szOfflineData[i*2+1];

			//效验字符
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//生成结果
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) cbOfflineData[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) cbOfflineData[i]+=(cbChar2-TEXT('A')+0x0A);

			//生成结果
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) cbOfflineData[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) cbOfflineData[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//发送消息
	SendData(dwContextID,MDM_GC_USER,pUserUserOfflineMessage->wMessageType,&cbOfflineData,pUserUserOfflineMessage->wDataSize);

	return true;
}

bool CAttemperEngineSink::OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_DeleteFriend));
	if(wDataSize>sizeof(DBO_GR_DeleteFriend)) return false;

	//提取数据
	DBO_GR_DeleteFriend * pDeleteFriend = (DBO_GR_DeleteFriend *)pData;
	ASSERT(pDeleteFriend);

	//获取用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwUserID);
	if(pServerUserItem!=NULL)
	{
		CUserFriendGroup * pFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pFriendGroup!=NULL) 
		{
			//移除好友
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
			pFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

		}

		//获取好友
		CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwUserID);
		if(pUserFriendGroup==NULL) 
		{
			pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
		}

		if(pUserFriendGroup!=NULL)
		{
			//获取好友分组
			CUserFriendGroup * pDestFriendGroup = m_FriendGroupManager.SearchGroupItem(pDeleteFriend->dwFriendUserID);
			if(pDestFriendGroup!=NULL) 
			{
				//移除好友
				pUserFriendGroup->RemoveFriendInfo(pDeleteFriend->dwFriendUserID);
				pDestFriendGroup->RemoveFriendInfo(pDeleteFriend->dwUserID);

			}



			CServerUserItem * pFriendUserItem = m_ServerUserManager.SearchUserItem(pDeleteFriend->dwFriendUserID);
			if(pFriendUserItem!=NULL&&pFriendUserItem->GetMainStatus()==FRIEND_US_ONLINE)
			{
				//构造结构
				CMD_GC_DeleteFriendNotify DeleteFriendNotify;
				DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwUserID;

				//发送数据
				SendData(((CLocalUserItem *)pFriendUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));
			}
					
		}
	}

		//构造结构
		CMD_GC_DeleteFriendNotify DeleteFriendNotify;
		DeleteFriendNotify.dwFriendUserID=pDeleteFriend->dwFriendUserID;

		//发送数据
		SendData(dwContextID,MDM_GC_USER,SUB_GC_DELETE_FRIEND_NOTIFY,&DeleteFriendNotify,sizeof(DeleteFriendNotify));

		return true;

}

//查找用户
bool CAttemperEngineSink::OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_SearchUserResult));
	if(wDataSize>sizeof(DBO_GR_SearchUserResult)) return false;

	//提取数据
	DBO_GR_SearchUserResult * pSearchUserResult = (DBO_GR_SearchUserResult *)pData;
	ASSERT(pSearchUserResult);

	//填充数据
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

	//变量定义	
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
		//回应反馈
		CMD_GC_SearchByGameIDEcho SearchUserEcho;
		ZeroMemory(&SearchUserEcho,sizeof(SearchUserEcho));
		SearchUserEcho.lErrorCode=CHAT_MSG_ERR;
		lstrcpyn(SearchUserEcho.szDescribeString,_T("查询不到用户"),CountArray(SearchUserEcho.szDescribeString));
		WORD wDataSize=CountStringBuffer(SearchUserEcho.szDescribeString);
		WORD wHeadSize=sizeof(SearchUserEcho)-sizeof(SearchUserEcho.szDescribeString);
		SendData(dwContextID,MDM_GC_USER,SUB_GC_SEARCH_USER_ECHO,&SearchUserEcho,wHeadSize+wDataSize);
	}
	return  true;
}

//添加好友
bool CAttemperEngineSink::OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_AddFriend));
	if(wDataSize!=sizeof(DBO_GR_AddFriend)) return false;

	//提取数据
	DBO_GR_AddFriend * pAddFriend = (DBO_GR_AddFriend *)pData;
	ASSERT(pAddFriend);

	//查找用户
	CServerUserItem * pServerUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwUserID);
	CServerUserItem * pRequestUserItem = m_ServerUserManager.SearchUserItem(pAddFriend->dwRequestUserID);
	if(pServerUserItem==NULL || pServerUserItem->GetUserItemKind()!=enLocalKind) return false;

	//处理用户
	if(pServerUserItem!=NULL)
	{
		if(pAddFriend->bLoadUserInfo==false && pRequestUserItem && pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pRequestLocalInfo = (CLocalUserItem *)pRequestUserItem;

			//获取好友
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//变量定义
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//设置变量
			ServerFriendInfo.dwUserID=pAddFriend->dwRequestUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//获取用户
			if(pRequestUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pRequestUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pRequestUserItem->GetGameStatus();
			}		

			//添加好友
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);

			//变量定义
			CMD_GC_UserFriendInfo UserFriendInfo;

			ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

			//构造结构
			UserFriendInfo.wFriendCount = 1;

			//基本信息
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

			//在线信息
			UserFriendInfo.FriendInfo[0].cbMainStatus=pRequestLocalInfo->GetMainStatus();
			UserFriendInfo.FriendInfo[0].cbGameStatus=pRequestLocalInfo->GetGameStatus();
			UserFriendInfo.FriendInfo[0].wServerID=pRequestLocalInfo->GetServerID();
			UserFriendInfo.FriendInfo[0].wTableID = pRequestLocalInfo->GetTableID();


			//发送数据
			WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
			SendData(((CLocalUserItem *)pServerUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

		}		
	}

	//处理用户
	if(pRequestUserItem!=NULL)
	{
		if(pRequestUserItem->GetUserItemKind()==enLocalKind)
		{
			//变量定义
			CLocalUserItem * pServerLocalInfo = (CLocalUserItem *)pServerUserItem;

			//获取好友
			CUserFriendGroup * pUserFriendGroup = m_FriendGroupManager.SearchGroupItem(pAddFriend->dwRequestUserID);
			if(pUserFriendGroup==NULL) 
			{
				pUserFriendGroup = m_FriendGroupManager.ActiveFriendGroup(MIN_FRIEND_CONTENT);
			}

			//变量定义
			tagServerFriendInfo ServerFriendInfo; 
			ZeroMemory(&ServerFriendInfo,sizeof(ServerFriendInfo));

			//设置变量
			ServerFriendInfo.dwUserID=pAddFriend->dwUserID;
			ServerFriendInfo.cbGroupID=pAddFriend->cbRequestGroupID;
			ServerFriendInfo.cbMainStatus=FRIEND_US_OFFLINE;
			ServerFriendInfo.cbGameStatus=US_OFFLINE;

			//获取用户
			if(pServerUserItem!=NULL)
			{
				ServerFriendInfo.cbMainStatus=pServerUserItem->GetMainStatus();
				ServerFriendInfo.cbGameStatus=pServerUserItem->GetGameStatus();
			}		

			//添加好友
			pUserFriendGroup->AppendFriendInfo(ServerFriendInfo);


			if(pRequestUserItem->GetMainStatus()!=FRIEND_US_OFFLINE)
			{
				//变量定义
				CMD_GC_UserFriendInfo UserFriendInfo;

				ZeroMemory(&UserFriendInfo.FriendInfo,sizeof(tagClientFriendInfo)*MAX_FRIEND_TRANSFER);

				//构造结构
				UserFriendInfo.wFriendCount = 1;

				//基本信息
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

				//在线信息
				UserFriendInfo.FriendInfo[0].cbMainStatus=pServerLocalInfo->GetMainStatus();
				UserFriendInfo.FriendInfo[0].cbGameStatus=pServerLocalInfo->GetGameStatus();
				UserFriendInfo.FriendInfo[0].wServerID=pServerLocalInfo->GetServerID();
				UserFriendInfo.FriendInfo[0].wTableID = pServerLocalInfo->GetTableID();

				//发送数据
				WORD wSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_LOGON,SUB_S_USER_FRIEND,&UserFriendInfo,wSize);

			}	

			//消息提醒
			CMD_GC_RespondNotify RespondNotify;
			_sntprintf(RespondNotify.szNotifyContent,CountArray(RespondNotify.szNotifyContent),TEXT("%s 同意了您的好友申请！"),pServerLocalInfo->GetNickName());

			if(pRequestUserItem->GetMainStatus()==FRIEND_US_OFFLINE)
			{
				SaveOfflineMessage(pRequestUserItem->GetUserID(),SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify),dwContextID);
			}
			else
			{
				//发送数据
				SendData(((CLocalUserItem *)pRequestUserItem)->GetSocketID(),MDM_GC_USER,SUB_GC_RESPOND_NOTIFY,&RespondNotify,sizeof(RespondNotify));
			}
		}		
	}

	return true;
}

bool CAttemperEngineSink::OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_TrumpetResult));
	if(wDataSize!=sizeof(DBO_GR_TrumpetResult)) return false;

	//提取数据
	DBO_GR_TrumpetResult * pTrumpetResult = (DBO_GR_TrumpetResult *)pData;
	ASSERT(pTrumpetResult);



	if (pTrumpetResult->lResult == 0)
	{
		//喇叭反馈
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
		//喇叭反馈
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

//操作结果
bool CAttemperEngineSink::OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize<=sizeof(DBO_GR_OperateResult));
	if(wDataSize>sizeof(DBO_GR_OperateResult)) return false;

	//提取数据
	DBO_GR_OperateResult * pUserOperateResult = (DBO_GR_OperateResult *)pData;
	ASSERT(pUserOperateResult);

	//获取用户
	if(pUserOperateResult->bModifySucesss==true)
	{
		//发送成功
		SendOperateSuccess(pUserOperateResult->szDescribeString,pUserOperateResult->wOperateCode,dwContextID);
	}
	else
	{
		//发送失败
		SendOperateFailure(pUserOperateResult->szDescribeString,pUserOperateResult->dwErrorCode,pUserOperateResult->wOperateCode,dwContextID);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
