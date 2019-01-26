#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;
	//辅助变量
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//连接信息
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->szDataBaseAddr,pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//请求处理
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:			//I D 登录
		{
			return OnRequestLogonUserID(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOGON_ACCOUNTS:			//账号登录
		{
			return OnRequestLogonUserAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GR_MODIFY_FRIEND:			//修改好友
		{
			return OnRequestModifyFriend(dwContextID,pData,wDataSize);
		}
	case DBR_GR_ADD_FRIEND:				//添加好友
		{
			return OnRequestAddFriend(dwContextID,pData,wDataSize);
		}
	case DBR_GR_DELETE_FRIEND:			//删除好友
		{
			return OnRequestDeleteFriend(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SEARCH_USER:			//查找用户
		{
			return OnRequestSearchUser(dwContextID,pData,wDataSize);
		}

	case DBR_GR_MODIFY_GROUP:			//修改分组
		{
			return OnRequestModifyGroup(dwContextID,pData,wDataSize);
		}
	case DBR_GR_MODIFY_REMARKS:			//修改备注
		{
			return OnRequestModifyRemarks(dwContextID,pData,wDataSize);
		}

	case DBR_GR_SAVE_OFFLINEMESSAGE:	//离线消息
		{
			return OnRequestSaveOfflineMessage(dwContextID,pData,wDataSize);
		}
	case DBR_GR_TRUMPET:
		{
			return OnRequestTrumpet(dwContextID,pData,wDataSize);
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	return false;
}


//I D 登录
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;
	//执行查询
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;

	try
	{
		//用户信息
		if(pLogonUserID->wLogonComand&LOGON_COMMAND_USERINFO)
		{
			//转化地址
			TCHAR szClientAddr[16]=TEXT("");
			BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
			_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccountsChat"),true);

			//用户信息
			lstrcpyn(m_LogonSuccess.szPassword,pLogonUserID->szPassword,CountArray(m_LogonSuccess.szPassword));
			lstrcpyn(m_LogonSuccess.szPhoneMode,pLogonUserID->szPhoneMode,CountArray(m_LogonSuccess.szPhoneMode));
			lstrcpyn(m_LogonSuccess.szMachineID, pLogonUserID->szMachineID, CountArray(m_LogonSuccess.szPhoneMode));
			m_LogonSuccess.dwClientAddr = pLogonUserID->dwClientAddr;

			//结果处理
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


			if (lResultCode==DB_SUCCESS)
			{
				//基本资料
				m_LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
				m_LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
				m_AccountsDBAide.GetValue_String(TEXT("NickName"),m_LogonSuccess.szNickName,CountArray(m_LogonSuccess.szNickName));
				m_LogonSuccess.dwFaceID=m_AccountsDBAide.GetValue_DWORD(TEXT("FaceID"));
				m_LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
				m_LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
				m_LogonSuccess.wMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
				m_LogonSuccess.wGrowLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("GrowLevel"));
				m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));
				m_AccountsDBAide.GetValue_String(TEXT("Compellation"),m_LogonSuccess.szCompellation,CountArray(m_LogonSuccess.szCompellation));

				//用户资料
				m_AccountsDBAide.GetValue_String(TEXT("QQ"),m_LogonSuccess.szQQ,CountArray(m_LogonSuccess.szQQ));
				m_AccountsDBAide.GetValue_String(TEXT("EMail"),m_LogonSuccess.szEMail,CountArray(m_LogonSuccess.szEMail));
				m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),m_LogonSuccess.szSeatPhone,CountArray(m_LogonSuccess.szSeatPhone));
				m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),m_LogonSuccess.szMobilePhone,CountArray(m_LogonSuccess.szMobilePhone));
				m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),m_LogonSuccess.szDwellingPlace,CountArray(m_LogonSuccess.szDwellingPlace));
				m_AccountsDBAide.GetValue_String(TEXT("PostalCode"),m_LogonSuccess.szPostalCode,CountArray(m_LogonSuccess.szPostalCode));

				//发送结果
				WORD wHeadSize=sizeof(m_LogonSuccess);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_LogonSuccess,wHeadSize);
			}
			else
			{
				//变量定义
				DBO_GR_LogonFailure LogonFailure;
				ZeroMemory(&LogonFailure,sizeof(LogonFailure));

				//构造数据
				LogonFailure.lResultCode=lResultCode;
				LogonFailure.dwUserID = pLogonUserID->dwUserID;
				lstrcpyn(LogonFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(LogonFailure.szDescribeString));

				//发送结果
				WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
				WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
			}


			if(lResultCode!=DB_SUCCESS) return true;
		}

		//加载分组
		if(pLogonUserID->wLogonComand&LOGON_COMMAND_GROUPINFO)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserGroups"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				TCHAR szGroupID[64]=TEXT("");
				TCHAR szGroupName[GROUP_LEN_INFO-64]=TEXT("");
				TCHAR szTempGroupName[GROUP_LEN_NAME]=TEXT("");

				//变量定义
				DBO_GR_UserGroupInfo UserGroupInfo;	
				ZeroMemory(&UserGroupInfo,sizeof(UserGroupInfo));

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//获取记录
					UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].cbGroupIndex = m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
					m_AccountsDBAide.GetValue_String(TEXT("GroupName"),UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].szGroupName,CountArray(UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].szGroupName));

					//设置变量
					++UserGroupInfo.wGroupCount;

					//退出判断
					if(UserGroupInfo.wGroupCount>=GROUP_MAX_COUNT) break;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//投递数据
				WORD wDataSize = sizeof(UserGroupInfo)-sizeof(UserGroupInfo.GroupInfo)+UserGroupInfo.wGroupCount*sizeof(tagClientGroupInfo);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GROUPINFO,dwContextID,&UserGroupInfo,wDataSize);
			}

			if(lResultCode!=DB_SUCCESS) return true;
		}

		//加载好友
		if(pLogonUserID->wLogonComand&LOGON_COMMAND_FRIENDS)
		{
			//读取好友
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserFriends"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				DBO_GR_UserIndividual UserIndividual;
				DBO_GR_UserFriendInfo	UserFriendInfo;

				//重置数据
				ZeroMemory(&UserFriendInfo,sizeof(UserFriendInfo));
				ZeroMemory(&UserIndividual,sizeof(UserIndividual));

				//设置变量
				UserFriendInfo.dwUserID = pLogonUserID->dwUserID;
				UserFriendInfo.bLogonFlag = true;
				UserIndividual.dwUserID = pLogonUserID->dwUserID;

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//好友信息
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].cbGroupID =m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwFaceID =m_AccountsDBAide.GetValue_BYTE(TEXT("FaceID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwCustomID =m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].cbGender =m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].wMemberOrder =m_AccountsDBAide.GetValue_WORD(TEXT("MemberOrder"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].wGrowLevel =m_AccountsDBAide.GetValue_WORD(TEXT("GrowLevel"));
					m_AccountsDBAide.GetValue_String(TEXT("NickName"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szNickName,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szNickName));
					m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szUnderWrite,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szUnderWrite));
					m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szCompellation,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szCompellation));

					//简易资料					
					UserIndividual.UserIndividual[UserIndividual.wUserCount].dwUserID =m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szQQ,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szQQ));
					m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szEMail,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szEMail));
					m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szSeatPhone,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szSeatPhone));
					m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szMobilePhone,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szMobilePhone));
					m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szDwellingPlace,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szDwellingPlace));
					m_AccountsDBAide.GetValue_String(TEXT("PostalCode"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szPostalCode,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szPostalCode));

					//设置变量
					++UserFriendInfo.wFriendCount;
					++UserIndividual.wUserCount;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//投递数据
				WORD wDataSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_FRIENDINFO,dwContextID,&UserFriendInfo,wDataSize);


				//投递数据
				wDataSize = sizeof(UserIndividual)-sizeof(UserIndividual.UserIndividual)+sizeof(tagUserIndividual)*UserIndividual.wUserCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_SINDIVIDUAL,dwContextID,&UserIndividual,wDataSize);
			}
		}

		//加载备注
		if(pLogonUserID->wLogonComand&LOGON_COMMAND_REMARKS)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadFriendRemarks"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				DBO_GR_UserRemarksInfo UserRemarksInfo;
				ZeroMemory(&UserRemarksInfo,sizeof(UserRemarksInfo));

				//设置变量
				UserRemarksInfo.dwUserID = pLogonUserID->dwUserID;

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//好友信息
					UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].dwFriendUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].szRemarksInfo,CountArray(UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].szRemarksInfo));

					//设置变量
					++UserRemarksInfo.wFriendCount;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//数目判断
				if(UserRemarksInfo.wFriendCount>0)
				{
					//投递数据
					wDataSize = sizeof(UserRemarksInfo)-sizeof(UserRemarksInfo.RemarksInfo)+sizeof(tagUserRemarksInfo)*UserRemarksInfo.wFriendCount;
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_REMARKS,dwContextID,&UserRemarksInfo,wDataSize);
				}
			}
		}

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");

		//加载离线消息
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadOfflineMessage"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
		{
			//变量定义
			DBO_GR_UserOfflineMessage UserOfflineMessage;
			ZeroMemory(&UserOfflineMessage,sizeof(UserOfflineMessage));

			//遍历记录集
			while(m_AccountsDBModule->IsRecordsetEnd()==false)
			{				
				UserOfflineMessage.wMessageType=m_AccountsDBAide.GetValue_WORD(TEXT("MessageType"));
				UserOfflineMessage.wDataSize =m_AccountsDBAide.GetValue_WORD(TEXT("DataSize"));
				m_AccountsDBAide.GetValue_String(TEXT("OfflineData"),UserOfflineMessage.szOfflineData,CountArray(UserOfflineMessage.szOfflineData));

				//移动记录
				m_AccountsDBModule->MoveToNext();	

				//投递数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_OFFLINEMESSAGE,dwContextID,&UserOfflineMessage,sizeof(UserOfflineMessage));		
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);


		//变量定义
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode=DB_ERROR;
		LogonFailure.dwUserID = pLogonUserID->dwUserID;
		lstrcpyn(LogonFailure.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),CountArray(LogonFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);


		return false;
	}

	return true;
}

//账号 登录
bool CDataBaseEngineSink::OnRequestLogonUserAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserAccounts));
	if (wDataSize!=sizeof(DBR_GR_LogonUserAccounts)) return false;
	//执行查询
	DBR_GR_LogonUserAccounts * pLogonUserAccounts=(DBR_GR_LogonUserAccounts *)pData;

	try
	{
		//用户信息
		if(pLogonUserAccounts->wLogonComand&LOGON_COMMAND_USERINFO)
		{
			//转化地址
			TCHAR szClientAddr[16]=TEXT("");
			BYTE * pClientAddr=(BYTE *)&pLogonUserAccounts->dwClientAddr;
			_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonUserAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonUserAccounts->szPassword);
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccChat"),true);

			//用户信息
			lstrcpyn(m_LogonSuccess.szAccounts, pLogonUserAccounts->szAccounts, CountArray(m_LogonSuccess.szAccounts));
			lstrcpyn(m_LogonSuccess.szPassword,pLogonUserAccounts->szPassword,CountArray(m_LogonSuccess.szPassword));
			lstrcpyn(m_LogonSuccess.szPhoneMode,pLogonUserAccounts->szPhoneMode,CountArray(m_LogonSuccess.szPhoneMode));
			lstrcpyn(m_LogonSuccess.szMachineID, pLogonUserAccounts->szMachineID, CountArray(m_LogonSuccess.szPhoneMode));
			m_LogonSuccess.dwClientAddr = pLogonUserAccounts->dwClientAddr;

			//结果处理
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			if (lResultCode==DB_SUCCESS)
			{
				//基本资料
				m_LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
				m_LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
				m_AccountsDBAide.GetValue_String(TEXT("NickName"),m_LogonSuccess.szNickName,CountArray(m_LogonSuccess.szNickName));
				m_LogonSuccess.dwFaceID=m_AccountsDBAide.GetValue_DWORD(TEXT("FaceID"));
				m_LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
				m_LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
				m_LogonSuccess.wMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
				m_LogonSuccess.wGrowLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("GrowLevel"));
				m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));
				m_AccountsDBAide.GetValue_String(TEXT("Compellation"),m_LogonSuccess.szCompellation,CountArray(m_LogonSuccess.szCompellation));

				//用户资料
				m_AccountsDBAide.GetValue_String(TEXT("QQ"),m_LogonSuccess.szQQ,CountArray(m_LogonSuccess.szQQ));
				m_AccountsDBAide.GetValue_String(TEXT("EMail"),m_LogonSuccess.szEMail,CountArray(m_LogonSuccess.szEMail));
				m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),m_LogonSuccess.szSeatPhone,CountArray(m_LogonSuccess.szSeatPhone));
				m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),m_LogonSuccess.szMobilePhone,CountArray(m_LogonSuccess.szMobilePhone));
				m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),m_LogonSuccess.szDwellingPlace,CountArray(m_LogonSuccess.szDwellingPlace));
				m_AccountsDBAide.GetValue_String(TEXT("PostalCode"),m_LogonSuccess.szPostalCode,CountArray(m_LogonSuccess.szPostalCode));

				//发送结果
				WORD wHeadSize=sizeof(m_LogonSuccess);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_LogonSuccess,wHeadSize);
			}
			else
			{
				//变量定义
				DBO_GR_AccountLogonFailure LogonFailure;
				ZeroMemory(&LogonFailure,sizeof(LogonFailure));

				//构造数据
				LogonFailure.lResultCode=lResultCode;
				lstrcpyn(LogonFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(LogonFailure.szDescribeString));

				//发送结果
				WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
				WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ACCOUNT_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
			}

			if(lResultCode!=DB_SUCCESS) return true;
		}

		//加载分组
		if(pLogonUserAccounts->wLogonComand&LOGON_COMMAND_GROUPINFO)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserGroups"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				TCHAR szGroupID[64]=TEXT("");
				TCHAR szGroupName[GROUP_LEN_INFO-64]=TEXT("");
				TCHAR szTempGroupName[GROUP_LEN_NAME]=TEXT("");

				//变量定义
				DBO_GR_UserGroupInfo UserGroupInfo;	
				ZeroMemory(&UserGroupInfo,sizeof(UserGroupInfo));

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//获取记录
					UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].cbGroupIndex = m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
					m_AccountsDBAide.GetValue_String(TEXT("GroupName"),UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].szGroupName,CountArray(UserGroupInfo.GroupInfo[UserGroupInfo.wGroupCount].szGroupName));

					//设置变量
					++UserGroupInfo.wGroupCount;

					//退出判断
					if(UserGroupInfo.wGroupCount>=GROUP_MAX_COUNT) break;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//投递数据
				WORD wDataSize = sizeof(UserGroupInfo)-sizeof(UserGroupInfo.GroupInfo)+UserGroupInfo.wGroupCount*sizeof(tagClientGroupInfo);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GROUPINFO,dwContextID,&UserGroupInfo,wDataSize);
			}

			if(lResultCode!=DB_SUCCESS) return true;
		}

		//加载好友
		if(pLogonUserAccounts->wLogonComand&LOGON_COMMAND_FRIENDS)
		{
			//读取好友
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserFriends"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				DBO_GR_UserIndividual UserIndividual;
				DBO_GR_UserFriendInfo	UserFriendInfo;

				//重置数据
				ZeroMemory(&UserFriendInfo,sizeof(UserFriendInfo));
				ZeroMemory(&UserIndividual,sizeof(UserIndividual));

				//设置变量
				UserFriendInfo.dwUserID = m_LogonSuccess.dwUserID;
				UserFriendInfo.bLogonFlag = true;
				UserIndividual.dwUserID = m_LogonSuccess.dwUserID;

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//好友信息
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].cbGroupID =m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwFaceID =m_AccountsDBAide.GetValue_BYTE(TEXT("FaceID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].dwCustomID =m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].cbGender =m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].wMemberOrder =m_AccountsDBAide.GetValue_WORD(TEXT("MemberOrder"));
					UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].wGrowLevel =m_AccountsDBAide.GetValue_WORD(TEXT("GrowLevel"));
					m_AccountsDBAide.GetValue_String(TEXT("NickName"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szNickName,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szNickName));
					m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szUnderWrite,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szUnderWrite));
					m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szCompellation,CountArray(UserFriendInfo.FriendInfo[UserFriendInfo.wFriendCount].szCompellation));

					//简易资料					
					UserIndividual.UserIndividual[UserIndividual.wUserCount].dwUserID =m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szQQ,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szQQ));
					m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szEMail,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szEMail));
					m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szSeatPhone,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szSeatPhone));
					m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szMobilePhone,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szMobilePhone));
					m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szDwellingPlace,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szDwellingPlace));
					m_AccountsDBAide.GetValue_String(TEXT("PostalCode"),UserIndividual.UserIndividual[UserIndividual.wUserCount].szPostalCode,CountArray(UserIndividual.UserIndividual[UserIndividual.wUserCount].szPostalCode));

					//设置变量
					++UserFriendInfo.wFriendCount;
					++UserIndividual.wUserCount;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//投递数据
				WORD wDataSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_FRIENDINFO,dwContextID,&UserFriendInfo,wDataSize);


				//投递数据
				wDataSize = sizeof(UserIndividual)-sizeof(UserIndividual.UserIndividual)+sizeof(tagUserIndividual)*UserIndividual.wUserCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_SINDIVIDUAL,dwContextID,&UserIndividual,wDataSize);
			}
		}

		//加载备注
		if(pLogonUserAccounts->wLogonComand&LOGON_COMMAND_REMARKS)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

			//执行查询
			LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadFriendRemarks"),true);

			//执行成功
			if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
			{
				//变量定义
				DBO_GR_UserRemarksInfo UserRemarksInfo;
				ZeroMemory(&UserRemarksInfo,sizeof(UserRemarksInfo));

				//设置变量
				UserRemarksInfo.dwUserID = m_LogonSuccess.dwUserID;

				//遍历记录集
				while(m_AccountsDBModule->IsRecordsetEnd()==false)
				{
					//好友信息
					UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].dwFriendUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
					m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].szRemarksInfo,CountArray(UserRemarksInfo.RemarksInfo[UserRemarksInfo.wFriendCount].szRemarksInfo));

					//设置变量
					++UserRemarksInfo.wFriendCount;

					//移动记录
					m_AccountsDBModule->MoveToNext();						
				}

				//数目判断
				if(UserRemarksInfo.wFriendCount>0)
				{
					//投递数据
					wDataSize = sizeof(UserRemarksInfo)-sizeof(UserRemarksInfo.RemarksInfo)+sizeof(tagUserRemarksInfo)*UserRemarksInfo.wFriendCount;
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_REMARKS,dwContextID,&UserRemarksInfo,wDataSize);
				}
			}
		}

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");

		//加载离线消息
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadOfflineMessage"),true);

		//执行成功
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
		{
			//变量定义
			DBO_GR_UserOfflineMessage UserOfflineMessage;
			ZeroMemory(&UserOfflineMessage,sizeof(UserOfflineMessage));

			//遍历记录集
			while(m_AccountsDBModule->IsRecordsetEnd()==false)
			{				
				UserOfflineMessage.wMessageType=m_AccountsDBAide.GetValue_WORD(TEXT("MessageType"));
				UserOfflineMessage.wDataSize =m_AccountsDBAide.GetValue_WORD(TEXT("DataSize"));
				m_AccountsDBAide.GetValue_String(TEXT("OfflineData"),UserOfflineMessage.szOfflineData,CountArray(UserOfflineMessage.szOfflineData));

				//移动记录
				m_AccountsDBModule->MoveToNext();	

				//投递数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_OFFLINEMESSAGE,dwContextID,&UserOfflineMessage,sizeof(UserOfflineMessage));		
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_AccountLogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode=DB_ERROR;
		lstrcpyn(LogonFailure.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),CountArray(LogonFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ACCOUNT_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//修改好友
bool CDataBaseEngineSink::OnRequestModifyFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取变量
	DBR_GR_ModifyFriend * pModifyFriend=(DBR_GR_ModifyFriend * )pData;
	ASSERT(pModifyFriend!=NULL);

	try
	{
		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyFriend->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwFriendUserID"),pModifyFriend->dwFriendUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyFriend->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbOperateKind"),pModifyFriend->cbOperateKind);
		m_AccountsDBAide.AddParameter(TEXT("@cbGroupID"),pModifyFriend->cbGroupID);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserFriend"),true);

		//变量定义
		DBO_GR_ModifyFriend ModifyFriend;
		ModifyFriend.dwUserID=pModifyFriend->dwUserID;
		ModifyFriend.dwFriendUserID=pModifyFriend->dwFriendUserID;
		ModifyFriend.bSuccessed=lResultCode==DB_SUCCESS;
		ModifyFriend.cbOperateKind=pModifyFriend->cbOperateKind;
		ModifyFriend.cbGroupID=pModifyFriend->cbGroupID;

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_FRIEND,dwContextID,&ModifyFriend,sizeof(ModifyFriend));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_ModifyFriend ModifyFriend;
		ModifyFriend.dwUserID=pModifyFriend->dwUserID;
		ModifyFriend.dwFriendUserID=pModifyFriend->dwFriendUserID;
		ModifyFriend.bSuccessed=false;
		ModifyFriend.cbOperateKind=pModifyFriend->cbOperateKind;
		ModifyFriend.cbGroupID=pModifyFriend->cbGroupID;

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_FRIEND,dwContextID,&ModifyFriend,sizeof(ModifyFriend));

		return false;
	}

	return true;
}

//添加好友
bool CDataBaseEngineSink::OnRequestAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取变量
		DBR_GR_AddFriend * pAddFriend=(DBR_GR_AddFriend * )pData;
		ASSERT(pAddFriend!=NULL);

		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pAddFriend->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDestUserID"),pAddFriend->dwRequestUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGroupID"),pAddFriend->cbGroupID);
		m_AccountsDBAide.AddParameter(TEXT("@cbDestGroupID"),pAddFriend->cbRequestGroupID);
		m_AccountsDBAide.AddParameter(TEXT("@cbLoadUserInfo"),pAddFriend->bLoadUserInfo);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_AddUserFriend"),true);

		if(lResultCode==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_AddFriend AddFriend;
			AddFriend.dwUserID = pAddFriend->dwUserID;
			AddFriend.dwRequestUserID = pAddFriend->dwRequestUserID;
			AddFriend.cbGroupID = pAddFriend->cbGroupID;
			AddFriend.cbRequestGroupID = pAddFriend->cbRequestGroupID;
			AddFriend.bLoadUserInfo = pAddFriend->bLoadUserInfo;

			//加载用户信息
			if(AddFriend.bLoadUserInfo==true)
			{
				//构造结构
				DBO_GR_UserFriendInfo UserFriendInfo;
				UserFriendInfo.dwUserID = AddFriend.dwUserID;
				UserFriendInfo.wFriendCount = 1;
				UserFriendInfo.bLogonFlag=false;

				//获取记录
				UserFriendInfo.FriendInfo[0].dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
				UserFriendInfo.FriendInfo[0].dwGameID =m_AccountsDBAide.GetValue_BYTE(TEXT("GameID"));
				UserFriendInfo.FriendInfo[0].cbGroupID =m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
				UserFriendInfo.FriendInfo[0].dwFaceID =m_AccountsDBAide.GetValue_BYTE(TEXT("FaceID"));
				UserFriendInfo.FriendInfo[0].dwCustomID =m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
				UserFriendInfo.FriendInfo[0].cbGender =m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
				UserFriendInfo.FriendInfo[0].wMemberOrder =m_AccountsDBAide.GetValue_WORD(TEXT("MemberOrder"));
				UserFriendInfo.FriendInfo[0].wGrowLevel =m_AccountsDBAide.GetValue_WORD(TEXT("GrowLevel"));
				m_AccountsDBAide.GetValue_String(TEXT("NickName"),UserFriendInfo.FriendInfo[0].szNickName,CountArray(UserFriendInfo.FriendInfo[0].szNickName));
				m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),UserFriendInfo.FriendInfo[0].szUnderWrite,CountArray(UserFriendInfo.FriendInfo[0].szUnderWrite));
				m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserFriendInfo.FriendInfo[0].szCompellation,CountArray(UserFriendInfo.FriendInfo[0].szCompellation));

				//投递数据
				WORD wDataSize = sizeof(UserFriendInfo)-sizeof(UserFriendInfo.FriendInfo)+sizeof(tagClientFriendInfo)*UserFriendInfo.wFriendCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_FRIENDINFO,dwContextID,&UserFriendInfo,wDataSize);

			}

			//投递数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ADD_FRIEND,dwContextID,&AddFriend,sizeof(AddFriend));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//删除好友
bool CDataBaseEngineSink::OnRequestDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取变量
		DBR_GR_DeleteFriend * pDeleteFriend=(DBR_GR_DeleteFriend * )pData;
		ASSERT(pDeleteFriend!=NULL);

		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pDeleteFriend->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwFriendUserID"),pDeleteFriend->dwFriendUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGroupID"),pDeleteFriend->cbGroupID);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_DeleteUserFriend"),true);

		//变量定义
		DBO_GR_DeleteFriend DeleteFriend;
		ZeroMemory(&DeleteFriend,sizeof(DeleteFriend));

		if(lResultCode==DB_SUCCESS)
		{
			DeleteFriend.dwUserID=pDeleteFriend->dwUserID;
			DeleteFriend.dwFriendUserID=pDeleteFriend->dwFriendUserID;
			DeleteFriend.bSuccessed=true;
			DeleteFriend.cbGroupID=pDeleteFriend->cbGroupID;
		}

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_FRIEND,dwContextID,&DeleteFriend,sizeof(DeleteFriend));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//修改分组
bool CDataBaseEngineSink::OnRequestModifyGroup(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取变量
	DBR_GR_ModifyGroup * pModifyGroup=(DBR_GR_ModifyGroup * )pData;
	ASSERT(pModifyGroup!=NULL);

	try
	{
		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyGroup->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyGroup->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbGroupID"),pModifyGroup->cbGroupID);
		m_AccountsDBAide.AddParameter(TEXT("@cbOperateKind"),pModifyGroup->cbOperateKind);
		m_AccountsDBAide.AddParameter(TEXT("@strGroupName"),pModifyGroup->szGroupName);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserGroup"),true);

		//变量定义
		DBO_GR_ModifyGroup	ModifyGroup;
		ModifyGroup.bSuccessed=lResultCode==DB_SUCCESS;
		ModifyGroup.dwUserID=pModifyGroup->dwUserID;
		ModifyGroup.cbOperateKind = pModifyGroup->cbOperateKind;
		ModifyGroup.lErrorCode=lResultCode;
		lstrcpyn(ModifyGroup.szGroupName,pModifyGroup->szGroupName,CountArray(ModifyGroup.szGroupName));

		//结果判断
		if(lResultCode==DB_SUCCESS)
		{			
			ModifyGroup.cbGroupID =m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));			
			ModifyGroup.szDescribeString[0]=0;
		}
		else
		{			
			ModifyGroup.cbGroupID =pModifyGroup->cbGroupID;

			//变量定义
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(ModifyGroup.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(ModifyGroup.szDescribeString));
		}

		//投递数据
		WORD wSize = sizeof(ModifyGroup)-sizeof(ModifyGroup.szDescribeString)+CountStringBuffer(ModifyGroup.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_GROUP,dwContextID,&ModifyGroup,wSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_ModifyGroup	ModifyGroup;
		ModifyGroup.bSuccessed=false;
		ModifyGroup.dwUserID=pModifyGroup->dwUserID;
		ModifyGroup.cbOperateKind = pModifyGroup->cbOperateKind;
		ModifyGroup.cbGroupID =m_AccountsDBAide.GetValue_BYTE(TEXT("GroupID"));
		lstrcpyn(ModifyGroup.szGroupName,pModifyGroup->szGroupName,CountArray(ModifyGroup.szGroupName));

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_GROUP,dwContextID,&ModifyGroup,sizeof(ModifyGroup));

		return false;
	}

	return true;
}

//修改备注
bool CDataBaseEngineSink::OnRequestModifyRemarks(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取变量
	DBR_GR_ModifyRemarks * pModifyRemarks=(DBR_GR_ModifyRemarks * )pData;
	ASSERT(pModifyRemarks!=NULL);

	try
	{	
		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyRemarks->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwFriendUserID"),pModifyRemarks->dwFriendID);
		m_AccountsDBAide.AddParameter(TEXT("@szRemarksInfo"),pModifyRemarks->szUserRemarks);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserRemarks"),true);

		//变量定义
		DBO_GR_ModifyRemarks  ModifyRemarks;
		ModifyRemarks.bSuccessed=lResultCode==DB_SUCCESS;
		ModifyRemarks.dwUserID=pModifyRemarks->dwUserID;
		ModifyRemarks.dwFriendID = pModifyRemarks->dwFriendID;
		lstrcpyn(ModifyRemarks.szUserRemarks,pModifyRemarks->szUserRemarks,CountArray(ModifyRemarks.szUserRemarks));

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_REMARKS,dwContextID,&ModifyRemarks,sizeof(ModifyRemarks));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_ModifyRemarks  ModifyRemarks;
		ModifyRemarks.bSuccessed=false;
		ModifyRemarks.dwUserID=pModifyRemarks->dwUserID;
		ModifyRemarks.dwFriendID = pModifyRemarks->dwFriendID;
		lstrcpyn(ModifyRemarks.szUserRemarks,pModifyRemarks->szUserRemarks,CountArray(ModifyRemarks.szUserRemarks));

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_REMARKS,dwContextID,&ModifyRemarks,sizeof(ModifyRemarks));

		return false;
	}

	return true;
}

//查找用户
bool CDataBaseEngineSink::OnRequestSearchUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取变量
		DBR_GR_SearchUser * pSearchUser=(DBR_GR_SearchUser * )pData;
		ASSERT(pSearchUser!=NULL);

		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pSearchUser->dwSearchByGameID);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SearchUser"),true);

		//变量定义
		DBO_GR_SearchUserResult	SearchUserResult;
		ZeroMemory(&SearchUserResult,sizeof(SearchUserResult));

		//执行成功
		if(lResultCode==DB_SUCCESS && m_AccountsDBModule.GetInterface())
		{
			if(m_AccountsDBModule->IsRecordsetEnd()==false)
			{
				SearchUserResult.FriendInfo.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
				SearchUserResult.FriendInfo.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
				SearchUserResult.FriendInfo.cbGroupID =0;
				SearchUserResult.FriendInfo.dwFaceID =m_AccountsDBAide.GetValue_BYTE(TEXT("FaceID"));
				SearchUserResult.FriendInfo.dwCustomID =m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
				SearchUserResult.FriendInfo.cbGender =m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
				SearchUserResult.FriendInfo.wMemberOrder =m_AccountsDBAide.GetValue_WORD(TEXT("MemberOrder"));
				SearchUserResult.FriendInfo.wGrowLevel =m_AccountsDBAide.GetValue_WORD(TEXT("GrowLevel"));
				m_AccountsDBAide.GetValue_String(TEXT("NickName"),    SearchUserResult.FriendInfo.szNickName    ,CountArray(SearchUserResult.FriendInfo.szNickName));
				m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),  SearchUserResult.FriendInfo.szUnderWrite  ,CountArray(SearchUserResult.FriendInfo.szUnderWrite));
				SearchUserResult.cbUserCount = 1;
			}
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEARCH_USER_RESULT,dwContextID,&SearchUserResult,sizeof(SearchUserResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//操作失败
		OnUserOperateResult(dwContextID,false,0L,SUB_GC_SEARCH_USER,TEXT("抱歉,由于数据库操作异常,您的请求无法完成！"));

		return false;
	}

	return true;
}


//存储消息
bool CDataBaseEngineSink::OnRequestSaveOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取变量
	try
	{
		DBR_GR_SaveOfflineMessage * pSaveOfflineMessage=(DBR_GR_SaveOfflineMessage * )pData;
		ASSERT(pSaveOfflineMessage!=NULL);

		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pSaveOfflineMessage->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wMessageType"),pSaveOfflineMessage->wMessageType);
		m_AccountsDBAide.AddParameter(TEXT("@wDataSize"),pSaveOfflineMessage->wDataSize);
		m_AccountsDBAide.AddParameter(TEXT("@szOfflineData"),pSaveOfflineMessage->szOfflineData);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SaveOfflineMessage"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//喇叭消息
bool CDataBaseEngineSink::OnRequestTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//提取变量
	try
	{
		DBR_GR_Trumpet * pTrumpet=(DBR_GR_Trumpet * )pData;
		ASSERT(pTrumpet!=NULL);

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTrumpet->dwSendUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP "),pTrumpet->dwClientAddr);

		//变量定义
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UseTrumpet"),true);

		//变量定义
		DBO_GR_TrumpetResult  TrumpetResult;
		TrumpetResult.lResult=lResultCode;
		TrumpetResult.wPropertyID = pTrumpet->wPropertyID;
		TrumpetResult.dwSendUserID = pTrumpet->dwSendUserID;
		TrumpetResult.TrumpetColor = pTrumpet->TrumpetColor;
		lstrcpyn(TrumpetResult.szSendNickName, pTrumpet->szSendNickName, CountArray(TrumpetResult.szSendNickName));
		lstrcpyn(TrumpetResult.szTrumpetContent, pTrumpet->szTrumpetContent, CountArray(TrumpetResult.szTrumpetContent));
		lstrcpyn(TrumpetResult.szNotifyContent, szDescribeString, CountArray(TrumpetResult.szNotifyContent));

		//投递数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TRUMPET_RESULT,dwContextID,&TrumpetResult,sizeof(TrumpetResult));


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//操作结果
VOID CDataBaseEngineSink::OnUserOperateResult(DWORD dwContextID,bool bOperateSucess,DWORD dwErrorCode,WORD wOperateCode,LPCTSTR pszDescribeString)
{
	//变量定义
	DBO_GR_OperateResult OperateResult;

	//设置变量
	OperateResult.bModifySucesss=bOperateSucess;
	OperateResult.dwErrorCode=dwErrorCode;
	OperateResult.wOperateCode=wOperateCode;
	lstrcpyn(OperateResult.szDescribeString,pszDescribeString,CountArray(OperateResult.szDescribeString));

	//发送结果
	WORD wHeadSize=sizeof(OperateResult);
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_RESULT,dwContextID,&OperateResult,wHeadSize);
}

//////////////////////////////////////////////////////////////////////////////////
