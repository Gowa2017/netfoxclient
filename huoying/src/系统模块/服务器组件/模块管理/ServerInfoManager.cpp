#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoBuffer::CServerInfoBuffer()
{
}

//析构函数
CServerInfoBuffer::~CServerInfoBuffer()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//删除引用
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//重置数据
bool CServerInfoBuffer::ResetServerInfo()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//删除索引
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//查找类型
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//删除数据
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//插入数据
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//效验参数
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//查找现存
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//创建判断
	if (pGameServerInsert==NULL)
	{
		//创建对象
		pGameServerInsert=CreateServerInfo();

		//结果判断
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//获取数目
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//查找数据
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//创建对象
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//变量定义
	tagGameServerInfo * pGameServerInfo=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameServerInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameServerInfo=m_GameServerInfoArray[nArrayCount-1];
			m_GameServerInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameServerInfo=new tagGameServerInfo;
			if (pGameServerInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchOptionBuffer::CMatchOptionBuffer()
{
}

//析构函数
CMatchOptionBuffer::~CMatchOptionBuffer()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//删除数组
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);
	for (INT_PTR i=0;i<m_GameMatchOptionBuffer.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionBuffer[i];
		SafeDelete(pGameMatchOption);
	}

	//删除引用
	m_GameMatchOptionArray.RemoveAll();
	m_GameMatchOptionBuffer.RemoveAll();

	return;
}

//重置数据
bool CMatchOptionBuffer::ResetMatchOption()
{
	//添加对象
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);

	//删除元素
	m_GameMatchOptionArray.RemoveAll();

	return true;
}

//删除数据
bool CMatchOptionBuffer::DeleteMatchOption(DWORD dwMatchID,DWORD wMatchNO)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID && pGameMatchOption->lMatchNo==wMatchNO)
		{
			//删除数据
			m_GameMatchOptionArray.RemoveAt(i);
			m_GameMatchOptionBuffer.Add(pGameMatchOption);

			//设置变量
			ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

			break;
		}
	}	

	return true;
}

//插入数据
bool CMatchOptionBuffer::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//效验参数
	ASSERT(pGameMatchOption!=NULL);
	if (pGameMatchOption==NULL) return false;

	//查找现存
	tagGameMatchOption * pGameMatchInsert=SearchMatchOption(pGameMatchOption->dwMatchID,pGameMatchOption->lMatchNo);

	//创建判断
	if (pGameMatchInsert==NULL)
	{
		//创建对象
		pGameMatchInsert=CreateMatchOption();

		//结果判断
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameMatchOptionArray.Add(pGameMatchInsert);
	CopyMemory(pGameMatchInsert,pGameMatchOption,sizeof(tagGameMatchOption));

	return true;
}

//获取数目
DWORD CMatchOptionBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_GameMatchOptionArray.GetCount());
}

//查找数据
tagGameMatchOption * CMatchOptionBuffer::SearchMatchOption(DWORD dwMatchID,LONGLONG wMatchNO)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID && pGameMatchOption->lMatchNo==wMatchNO)
		{
			return pGameMatchOption;
		}
	}

	return NULL;
}

//创建对象
tagGameMatchOption * CMatchOptionBuffer::CreateMatchOption()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameMatchOptionBuffer.GetCount();
		if (nArrayCount>0)
		{
			pGameMatchOption=m_GameMatchOptionBuffer[nArrayCount-1];
			m_GameMatchOptionBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameMatchOption=new tagGameMatchOption;
			if (pGameMatchOption==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchOption;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoManager::CServerInfoManager()
{
}

//析构函数
CServerInfoManager::~CServerInfoManager()
{
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),0);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//清空列表
		ServerInfoBuffer.ResetServerInfo();

		//读取列表
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//读取数据
			tagGameServerInfo GameServerResult;
			ReadGameServerInfo(PlatformDBAide,GameServerResult);

			//插入列表
			ServerInfoBuffer.InsertServerInfo(&GameServerResult);

			//移动记录
			PlatformDBModule->MoveToNext();
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取数据
		if (PlatformDBModule->IsRecordsetEnd()==false)
		{
			ReadGameServerInfo(PlatformDBAide,GameServerResult);
			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//删除房间
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoom"),false)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//插入房间
bool CServerInfoManager::InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wGameID"),pGameServerCreate->wGameID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pGameServerCreate->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pGameServerCreate->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pGameServerCreate->wSortID);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pGameServerCreate->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@wRevenueRatio"),pGameServerCreate->wRevenueRatio);
		PlatformDBAide.AddParameter(TEXT("@lServiceScore"),pGameServerCreate->lServiceScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pGameServerCreate->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lMinTableScore"),pGameServerCreate->lMinTableScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pGameServerCreate->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pGameServerCreate->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@cbMinEnterMember"),pGameServerCreate->cbMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@cbMaxEnterMember"),pGameServerCreate->cbMaxEnterMember);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pGameServerCreate->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pGameServerCreate->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pGameServerCreate->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pGameServerCreate->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pGameServerCreate->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerKind"),pGameServerCreate->wServerKind);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pGameServerCreate->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"),pGameServerCreate->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pGameServerCreate->szServerName);
		PlatformDBAide.AddParameter(TEXT("@strServerPasswd"),pGameServerCreate->szServerPasswd);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pGameServerCreate->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pGameServerCreate->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pGameServerCreate->wDistributeTimeSpace);
		PlatformDBAide.AddParameter(TEXT("@wDistributeDrawCount"),pGameServerCreate->wDistributeDrawCount);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pGameServerCreate->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pGameServerCreate->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseName"),pGameServerCreate->szDataBaseName);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),pGameServerCreate->szDataBaseAddr);
		PlatformDBAide.AddParameter(TEXT("@strCustomRule"),szCustomRule);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pGameServerCreate->szServiceMachine);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameRoom"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		if (pGameServerCreate->wServerType == GAME_GENRE_PERSONAL)
		{
			InsertPersonalRoomParameter(PlatformDBAide, m_PersonalRoomOption, m_PersonalRoomOptionRightAndFee, GameServerResult.wServerID,  GameServerResult.wKindID);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//修改房间
bool CServerInfoManager::ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),pGameServerCreate->wServerID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pGameServerCreate->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pGameServerCreate->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pGameServerCreate->wSortID);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pGameServerCreate->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@wRevenueRatio"),pGameServerCreate->wRevenueRatio);
		PlatformDBAide.AddParameter(TEXT("@lServiceScore"),pGameServerCreate->lServiceScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pGameServerCreate->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lMinTableScore"),pGameServerCreate->lMinTableScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pGameServerCreate->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pGameServerCreate->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@cbMinEnterMember"),pGameServerCreate->cbMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@cbMaxEnterMember"),pGameServerCreate->cbMaxEnterMember);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pGameServerCreate->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pGameServerCreate->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pGameServerCreate->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pGameServerCreate->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pGameServerCreate->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerKind"),pGameServerCreate->wServerKind);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pGameServerCreate->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"),pGameServerCreate->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pGameServerCreate->szServerName);
		PlatformDBAide.AddParameter(TEXT("@strServerPasswd"),pGameServerCreate->szServerPasswd);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pGameServerCreate->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pGameServerCreate->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pGameServerCreate->wDistributeTimeSpace);
		PlatformDBAide.AddParameter(TEXT("@wDistributeDrawCount"),pGameServerCreate->wDistributeDrawCount);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pGameServerCreate->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pGameServerCreate->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseName"),pGameServerCreate->szDataBaseName);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),pGameServerCreate->szDataBaseAddr);
		PlatformDBAide.AddParameter(TEXT("@strCustomRule"),szCustomRule);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pGameServerCreate->szServiceMachine);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyGameRoom"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);
		if(pGameServerCreate->wServerType == GAME_GENRE_PERSONAL)
		{
			//修改私人房间参数
			ModifyPersonalRoomParameter(PlatformDBAide, m_PersonalRoomOption,  m_PersonalRoomOptionRightAndFee ,GameServerResult.wServerID, GameServerResult.wKindID);
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//连接数据
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//发起连接
	PlatformDBModule->OpenConnection();

	return true;
}

//读取房间
bool CServerInfoManager::ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult)
{
	//设置变量
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//索引变量
	GameServerResult.wGameID=PlatformDBAide.GetValue_WORD(TEXT("GameID"));
	GameServerResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));

	//挂接属性
	GameServerResult.wKindID=PlatformDBAide.GetValue_WORD(TEXT("KindID"));
	GameServerResult.wNodeID=PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
	GameServerResult.wSortID=PlatformDBAide.GetValue_WORD(TEXT("SortID"));

	//版本信息
	GameServerResult.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
	GameServerResult.dwServerVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));

	//模块属性
	PlatformDBAide.GetValue_String(TEXT("GameName"),GameServerResult.szGameName,CountArray(GameServerResult.szGameName));
	PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));
	PlatformDBAide.GetValue_String(TEXT("ClientEXEName"),GameServerResult.szClientEXEName,CountArray(GameServerResult.szClientEXEName));

	//税收配置
	GameServerResult.lCellScore=PlatformDBAide.GetValue_DOUBLE(TEXT("CellScore"));
	GameServerResult.wRevenueRatio=PlatformDBAide.GetValue_WORD(TEXT("RevenueRatio"));
	GameServerResult.lServiceScore=PlatformDBAide.GetValue_DOUBLE(TEXT("ServiceScore"));

	//房间配置
	GameServerResult.lRestrictScore=PlatformDBAide.GetValue_DOUBLE(TEXT("RestrictScore"));
	GameServerResult.lMinTableScore=PlatformDBAide.GetValue_DOUBLE(TEXT("MinTableScore"));
	GameServerResult.lMinEnterScore=PlatformDBAide.GetValue_DOUBLE(TEXT("MinEnterScore"));
	GameServerResult.lMaxEnterScore=PlatformDBAide.GetValue_DOUBLE(TEXT("MaxEnterScore"));

	//会员限制
	GameServerResult.cbMinEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MinEnterMember"));
	GameServerResult.cbMaxEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MaxEnterMember"));

	//房间配置
	GameServerResult.dwServerRule=PlatformDBAide.GetValue_DWORD(TEXT("ServerRule"));
	GameServerResult.dwAttachUserRight=PlatformDBAide.GetValue_DWORD(TEXT("AttachUserRight"));

	//房间属性
	GameServerResult.wMaxPlayer=PlatformDBAide.GetValue_WORD(TEXT("MaxPlayer"));
	GameServerResult.wTableCount=PlatformDBAide.GetValue_WORD(TEXT("TableCount"));
	GameServerResult.wServerPort=PlatformDBAide.GetValue_WORD(TEXT("ServerPort"));
	GameServerResult.wServerKind=PlatformDBAide.GetValue_WORD(TEXT("ServerKind"));
	GameServerResult.wServerType=PlatformDBAide.GetValue_WORD(TEXT("ServerType"));
	GameServerResult.wServerLevel=PlatformDBAide.GetValue_WORD(TEXT("ServerLevel"));
	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameServerResult.szServerName,CountArray(GameServerResult.szServerName));
	PlatformDBAide.GetValue_String(TEXT("ServerPasswd"),GameServerResult.szServerPasswd,CountArray(GameServerResult.szServerPasswd));

	//分组属性
	GameServerResult.cbDistributeRule=PlatformDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameServerResult.wMinDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameServerResult.wDistributeTimeSpace=PlatformDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));
	GameServerResult.wDistributeDrawCount=PlatformDBAide.GetValue_WORD(TEXT("DistributeDrawCount"));
	GameServerResult.wMinPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MinPartakeGameUser"));
	GameServerResult.wMaxPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MaxPartakeGameUser"));

	//数据属性
	PlatformDBAide.GetValue_String(TEXT("DataBaseName"),GameServerResult.szDataBaseName,CountArray(GameServerResult.szDataBaseName));
	PlatformDBAide.GetValue_String(TEXT("DataBaseAddr"),GameServerResult.szDataBaseAddr,CountArray(GameServerResult.szDataBaseAddr));

	//扩展配置
	TCHAR szCustomRule[CountArray(GameServerResult.cbCustomRule)*2+1];
	PlatformDBAide.GetValue_String(TEXT("CustomRule"),szCustomRule,CountArray(szCustomRule));
	PlatformDBAide.GetValue_String(TEXT("ServiceMachine"),GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));

	//扩展配置
	if (szCustomRule[0]!=0)
	{
		//获取长度
		ASSERT(lstrlen(szCustomRule)%2==0);
		INT nCustomRuleSize=lstrlen(szCustomRule)/2;

		//转换字符
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//获取字符
			TCHAR cbChar1=szCustomRule[i*2];
			TCHAR cbChar2=szCustomRule[i*2+1];

			//效验字符
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//生成结果
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) GameServerResult.cbCustomRule[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) GameServerResult.cbCustomRule[i]+=(cbChar2-TEXT('A')+0x0A);

			//生成结果
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) GameServerResult.cbCustomRule[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) GameServerResult.cbCustomRule[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//本地版本
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CWHService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	return true;
}

//加载私人房间配置
bool CServerInfoManager::LoadPersonalRoomParameter(tagPersonalRoomOption  & pesronalRoomOption, tagPersonalRoomOptionRightAndFee  & personalRoomOptionRightAndFee,  WORD wServerID)
{

	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadPersonalRoomParameter"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}



		pesronalRoomOption.cbCardOrBean=PlatformDBAide.GetValue_BYTE(TEXT("CardOrBean"));
		pesronalRoomOption.lFeeCardOrBeanCount=PlatformDBAide.GetValue_LONGLONG(TEXT("FeeBeanOrRoomCard"));
		pesronalRoomOption.cbIsJoinGame=PlatformDBAide.GetValue_BYTE(TEXT("IsJoinGame"));
		pesronalRoomOption.cbMinPeople=PlatformDBAide.GetValue_BYTE(TEXT("MinPeople"));
		pesronalRoomOption.cbMaxPeople=PlatformDBAide.GetValue_BYTE(TEXT("MaxPeople"));
		pesronalRoomOption.lMaxCellScore=PlatformDBAide.GetValue_DOUBLE(TEXT("MaxCellScore"));
		pesronalRoomOption.lPersonalRoomTax=PlatformDBAide.GetValue_LONGLONG(TEXT("PersonalRoomTax"));

		pesronalRoomOption.wCanCreateCount=PlatformDBAide.GetValue_WORD(TEXT("CanCreateCount"));
		pesronalRoomOption.dwPlayTimeLimit=PlatformDBAide.GetValue_DWORD(TEXT("PlayTurnCount"));
		pesronalRoomOption.dwPlayTurnCount=PlatformDBAide.GetValue_DWORD(TEXT("PlayTimeLimit"));

		pesronalRoomOption.dwTimeAfterBeginCount=PlatformDBAide.GetValue_DWORD(TEXT("TimeAfterBeginCount"));
		pesronalRoomOption.dwTimeOffLineCount=PlatformDBAide.GetValue_DWORD(TEXT("TimeOffLineCount"));
		pesronalRoomOption.dwTimeNotBeginGame=PlatformDBAide.GetValue_DWORD(TEXT("TimeNotBeginGame"));
		pesronalRoomOption.dwTimeAfterCreateRoom = PlatformDBAide.GetValue_DWORD(TEXT("TimeNotBeginAfterCreateRoom"));

		//读取创建私人房间权限
		personalRoomOptionRightAndFee.cbCreateRight =  PlatformDBAide.GetValue_BYTE(TEXT("CreateRight"));

		//获取游戏类型id

		//获取私人房间配置信息
		//构造参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@dwKindID"), wServerID);

		//输出参数
		//TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		int lResultCode = PlatformDBAide.ExecuteProcess(TEXT("GSP_MB_GetPersonalFeeParameter"), true);

		if(lResultCode == DB_SUCCESS)
		{
			//变量定义
			DWORD dwPaketSize = 0;
			BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
			tagPersonalTableFeeList * pPersonalTableParameter=NULL;
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
			int iCount = 0;
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//读取信息
				personalRoomOptionRightAndFee.stCreateRoomPayFee[iCount].dwPlayTurnLimit = PlatformDBAide.GetValue_DWORD(TEXT("DrawCountLimit"));
				personalRoomOptionRightAndFee.stCreateRoomPayFee[iCount].dwPayFee = PlatformDBAide.GetValue_DOUBLE(TEXT("TableFee"));
				personalRoomOptionRightAndFee.stCreateRoomPayFee[iCount].dwIniScore = PlatformDBAide.GetValue_DOUBLE(TEXT("IniScore"));
				iCount++;
				//移动记录
				PlatformDBModule->MoveToNext();
			}
		}


		return true;



	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);


		return false;
	}

	return false;
}

//插入私人房间参数
bool CServerInfoManager::InsertPersonalRoomParameter(CDataBaseAide PlatformDBAide, tagPersonalRoomOption  PersonalRoomOption, tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee , WORD dwServerID,   WORD dwKindID)
{
	try
	{
		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),dwServerID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);
		PlatformDBAide.AddParameter(TEXT("@cbCardOrBean"),PersonalRoomOption.cbCardOrBean);
		PlatformDBAide.AddParameter(TEXT("@cbIsJoinGame"),PersonalRoomOption.cbIsJoinGame);
		PlatformDBAide.AddParameter(TEXT("@lFeeBeanOrRoomCard"),PersonalRoomOption.lFeeCardOrBeanCount);
		PlatformDBAide.AddParameter(TEXT("@cbMinPeople"),PersonalRoomOption.cbMinPeople);
		PlatformDBAide.AddParameter(TEXT("@cbMaxPeople"),PersonalRoomOption.cbMaxPeople);
		PlatformDBAide.AddParameter(TEXT("@lPersonalRoomTax"),PersonalRoomOption.lPersonalRoomTax);
		PlatformDBAide.AddParameter(TEXT("@lMaxCellScore"),PersonalRoomOption.lMaxCellScore);
		PlatformDBAide.AddParameter(TEXT("@wCanCreateCount"),PersonalRoomOption.wCanCreateCount);
		PlatformDBAide.AddParameter(TEXT("@wPlayTurnCount"),PersonalRoomOption.dwPlayTurnCount);
		PlatformDBAide.AddParameter(TEXT("@wPlayTimeLimit"),PersonalRoomOption.dwPlayTimeLimit);
		if (PersonalRoomOption.dwTimeAfterBeginCount == 0)
		{
			PersonalRoomOption.dwTimeAfterBeginCount  = 1400;
		}

		if (PersonalRoomOption.dwTimeOffLineCount == 0)
		{
			PersonalRoomOption.dwTimeOffLineCount  = 1400;
		}

		if (PersonalRoomOption.dwTimeNotBeginGame == 0)
		{
			PersonalRoomOption.dwTimeNotBeginGame  = 1400;
		}

		if (PersonalRoomOption.dwTimeAfterCreateRoom == 0)
		{
			PersonalRoomOption.dwTimeAfterCreateRoom  = 1400;
		}

		PlatformDBAide.AddParameter(TEXT("@wTimeAfterBeginCount"),PersonalRoomOption.dwTimeAfterBeginCount);
		PlatformDBAide.AddParameter(TEXT("@wTimeOffLineCount"),PersonalRoomOption.dwTimeOffLineCount);
		PlatformDBAide.AddParameter(TEXT("@wTimeNotBeginGame"),PersonalRoomOption.dwTimeNotBeginGame);
		PlatformDBAide.AddParameter(TEXT("@wTimeNotBeginAfterCreateRoom"),PersonalRoomOption.dwTimeAfterCreateRoom);
		PlatformDBAide.AddParameter(TEXT("@cbCreateRight"),PersonalRoomOptionRightAndFee.cbCreateRight);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertPersonalRoomParameter"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);

		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeletePersonalFeeParameter"),true);

		if (PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTimeLimit == 0 && PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTurnLimit == 0)
		{
			PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTurnLimit = 5;
			PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPayFee = 1;
			PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwIniScore = 1000;
		}

		for (int i = 0; i < FEE_OPTION_COUNT; i++)
		{

			if (PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTimeLimit != 0 || PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTurnLimit != 0)
			{
				//插入参数
				PlatformDBAide.ResetParameter();
				PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);
				PlatformDBAide.AddParameter(TEXT("@DrawCountLimit"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTurnLimit );
				PlatformDBAide.AddParameter(TEXT("@DrawTimeLimit"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTimeLimit);
				PlatformDBAide.AddParameter(TEXT("@TableFee"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPayFee);
				PlatformDBAide.AddParameter(TEXT("@IniScore"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwIniScore);
				//输出信息
				TCHAR szDescribeString[128]=TEXT("");
				PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
				//执行命令
				if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_WritePersonalFeeParameter"),true)!=DB_SUCCESS)
				{
					//获取信息
					PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

					//错误提示
					CTraceService::TraceString(TEXT("插入游戏费用出错"),TraceLevel_Exception);

					//错误提示
					AfxMessageBox(szDescribeString,MB_ICONERROR);

					return false;
				}
			}
		}


	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}


}

//设置私人房间参数
void CServerInfoManager::SetPersonalRoomOption(tagPersonalRoomOption PersonalRoomOption, tagPersonalRoomOptionRightAndFee  PersonalRoomOptionRightAndFee)
{
	m_PersonalRoomOption = PersonalRoomOption;
	m_PersonalRoomOptionRightAndFee = PersonalRoomOptionRightAndFee;
}


//修改私人房间参数
bool CServerInfoManager::ModifyPersonalRoomParameter(CDataBaseAide PlatformDBAide, tagPersonalRoomOption  PersonalRoomOption,  tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee , WORD dwServerID, WORD dwKindID)
{
	try
	{
		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@dwServerID"),dwServerID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);
		PlatformDBAide.AddParameter(TEXT("@cbCardOrBean"),PersonalRoomOption.cbCardOrBean);
		PlatformDBAide.AddParameter(TEXT("@lFeeBeanOrRoomCard"),PersonalRoomOption.lFeeCardOrBeanCount);
		PlatformDBAide.AddParameter(TEXT("@cbIsJoinGame"),PersonalRoomOption.cbIsJoinGame);
		PlatformDBAide.AddParameter(TEXT("@cbMinPeople"),PersonalRoomOption.cbMinPeople);
		PlatformDBAide.AddParameter(TEXT("@cbMaxPeople"),PersonalRoomOption.cbMaxPeople);
		PlatformDBAide.AddParameter(TEXT("@lPersonalRoomTax"),PersonalRoomOption.lPersonalRoomTax);
		PlatformDBAide.AddParameter(TEXT("@lMaxCellScore"),PersonalRoomOption.lMaxCellScore);
		PlatformDBAide.AddParameter(TEXT("@wCanCreateCount"),PersonalRoomOption.wCanCreateCount);
		PlatformDBAide.AddParameter(TEXT("@wPlayTurnCount"),PersonalRoomOption.dwPlayTurnCount);
		PlatformDBAide.AddParameter(TEXT("@wPlayTimeLimit"),PersonalRoomOption.dwPlayTimeLimit);

		if (PersonalRoomOption.dwTimeAfterBeginCount == 0)
		{
			PersonalRoomOption.dwTimeAfterBeginCount  = 1400;
		}

		if (PersonalRoomOption.dwTimeOffLineCount == 0)
		{
			PersonalRoomOption.dwTimeOffLineCount  = 1400;
		}

		if (PersonalRoomOption.dwTimeNotBeginGame == 0)
		{
			PersonalRoomOption.dwTimeNotBeginGame  = 1400;
		}

		if (PersonalRoomOption.dwTimeAfterCreateRoom == 0)
		{
			PersonalRoomOption.dwTimeAfterCreateRoom  = 1400;
		}
		PlatformDBAide.AddParameter(TEXT("@wTimeAfterBeginCount"),PersonalRoomOption.dwTimeAfterBeginCount);
		PlatformDBAide.AddParameter(TEXT("@wTimeOffLineCount"),PersonalRoomOption.dwTimeOffLineCount);
		PlatformDBAide.AddParameter(TEXT("@wTimeNotBeginGame"),PersonalRoomOption.dwTimeNotBeginGame);
		PlatformDBAide.AddParameter(TEXT("@wTimeNotBeginAfterCreateRoom"),PersonalRoomOption.dwTimeAfterCreateRoom);
		PlatformDBAide.AddParameter(TEXT("@cbCreateRight"),PersonalRoomOptionRightAndFee.cbCreateRight);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyPersonalRoomParameter"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeletePersonalFeeParameter"),true) ==DB_SUCCESS)
		{
			if (PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTimeLimit == 0 && PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTurnLimit == 0)
			{
				PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPlayTurnLimit = 5;
				PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwPayFee = 1;
				PersonalRoomOptionRightAndFee.stCreateRoomPayFee[0].dwIniScore = 1000;
			}

			for (int i = 0; i < FEE_OPTION_COUNT; i++)
			{
				if (PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTimeLimit != 0 || PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTurnLimit != 0)
				{
					//插入参数
					PlatformDBAide.ResetParameter();
					PlatformDBAide.AddParameter(TEXT("@wKindID"),dwKindID);
					PlatformDBAide.AddParameter(TEXT("@DrawCountLimit"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTurnLimit );
					PlatformDBAide.AddParameter(TEXT("@DrawTimeLimit"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPlayTimeLimit);
					PlatformDBAide.AddParameter(TEXT("@TableFee"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwPayFee);
					PlatformDBAide.AddParameter(TEXT("@IniScore"),PersonalRoomOptionRightAndFee.stCreateRoomPayFee[i].dwIniScore);
					//输出信息
					TCHAR szDescribeString[128]=TEXT("");
					PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
					//执行命令
					if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_WritePersonalFeeParameter"),true)!=DB_SUCCESS)
					{
						//获取信息
						PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

						//错误提示
						CTraceService::TraceString(TEXT("插入游戏费用出错"),TraceLevel_Exception);

						//错误提示
						AfxMessageBox(szDescribeString,MB_ICONERROR);

						return false;
					}
				}
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

}


//////////////////////////////////////////////////////////////////////////////////
