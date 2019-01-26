#include "StdAfx.h"
#include "AfxTempl.h"

#include "PersonalRoomServiceManager.h"
//#include "../../服务器组件/游戏服务器/AttemperEngineSink.h"
#include "../../消息定义\CMD_LogonServer.h"
#include "../../服务器组件/登录服务器/DataBasePacket.h"
#include "../../服务器组件/登录服务器/AttemperEngineSink.h"
////////////////////////////////////////////////////////////////////////

//构造函数
CPersonalRoomServiceManager::CPersonalRoomServiceManager(void)
{
	//状态变量
	m_bIsService=false;

	//设置变量
	m_pIPersonalRoomItem=NULL;

	return;
}

//析构函数
CPersonalRoomServiceManager::~CPersonalRoomServiceManager(void)
{	
	//释放指针
	if(m_pIPersonalRoomItem!=NULL) SafeDelete(m_pIPersonalRoomItem);
}

//停止服务
bool CPersonalRoomServiceManager::StopService()
{
	//状态判断
	ASSERT(m_bIsService==true);

	//设置状态
	m_bIsService=false;

	//释放指针
	if(m_pIPersonalRoomItem!=NULL) SafeRelease(m_pIPersonalRoomItem);

	return true;
}

//启动服务
bool CPersonalRoomServiceManager::StartService()
{
	//状态判断
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//设置状态
	m_bIsService=true;

	//启动通知
	if(m_pIPersonalRoomItem!=NULL) m_pIPersonalRoomItem->OnStartService();

	return true;
}
//接口查询
void *  CPersonalRoomServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPersonalRoomServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IPersonalRoomServiceManager,Guid,dwQueryVer);
	return NULL;
}





////初始化接口
//bool CPersonalRoomServiceManager::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
//{
//	ASSERT(m_pIPersonalRoomItem!=NULL);
//	if(m_pIPersonalRoomItem!=NULL)
//	{
//		return m_pIPersonalRoomItem->InitMatchInterface(MatchManagerParameter);
//	}
//
//	return true;
//}





//测试私人房
void CPersonalRoomServiceManager::TestPersonal()
{
	OutputDebugString(TEXT("ptdtsr *** 私人房测试"));
}



//查询房间
bool CPersonalRoomServiceManager::OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_MB_QueryGameServer));
	if(wDataSize != sizeof(CMD_MB_QueryGameServer)) return false;

	CMD_MB_QueryGameServer* pGameServer = (CMD_MB_QueryGameServer*)pData;

	//构造数据
	CMD_CS_C_QueryGameServer QueryGameServer;
	ZeroMemory(&QueryGameServer, sizeof(CMD_CS_C_QueryGameServer));
	QueryGameServer.dwUserID = pGameServer->dwUserID;
	QueryGameServer.dwKindID = pGameServer->dwKindID;
	QueryGameServer.cbIsJoinGame = pGameServer->cbIsJoinGame;
	QueryGameServer.dwSocketID = dwSocketID;
	QueryGameServer.dwClientAddr = (pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	//发送数据
	pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_GAME_SERVER, &QueryGameServer, sizeof(CMD_CS_C_QueryGameServer));

	return true;
}


//搜索房间桌号
bool CPersonalRoomServiceManager::OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_MB_SearchServerTable));
	if(wDataSize != sizeof(CMD_MB_SearchServerTable)) return false;

	CMD_MB_SearchServerTable* pSearchServerTable = (CMD_MB_SearchServerTable*)pData;

	CMD_CS_C_SearchTable SearchTable;
	ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

	SearchTable.dwSocketID = dwSocketID;
	SearchTable.dwKindID = pSearchServerTable->dwKindID;
	SearchTable.dwClientAddr = (pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(SearchTable.szPersonalTableID, pSearchServerTable->szServerID, CountArray(SearchTable.szPersonalTableID));

	pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_SEARCH_TABLE, &SearchTable, sizeof(CMD_CS_C_SearchTable));

	return true;
}

//强制解散搜索房间桌号
bool CPersonalRoomServiceManager::OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_MB_DissumeSearchServerTable));
	if(wDataSize != sizeof(CMD_MB_DissumeSearchServerTable)) return false;

	CMD_MB_DissumeSearchServerTable* pSearchServerTable = (CMD_MB_DissumeSearchServerTable*)pData;

	CMD_CS_C_SearchTable SearchTable;
	ZeroMemory(&SearchTable, sizeof(CMD_CS_C_SearchTable));

	SearchTable.dwSocketID = dwSocketID;
	SearchTable.dwClientAddr = (pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(SearchTable.szPersonalTableID, pSearchServerTable->szServerID, CountArray(SearchTable.szPersonalTableID));

	pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_C_DISSUME_SEARCH_TABLE, &SearchTable, sizeof(CMD_CS_C_SearchTable));

	return true;
}

//私人房间配置
bool CPersonalRoomServiceManager::OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID, IDataBaseEngine *	 pIDataBaseEngine)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_MB_GetPersonalParameter));
	if(wDataSize != sizeof(CMD_MB_GetPersonalParameter)) return false;

	CMD_MB_GetPersonalParameter* pGetPersonalParameter = (CMD_MB_GetPersonalParameter*)pData;



	//构造数据
	DBR_MB_GetPersonalParameter GetPersonalParameter;
	ZeroMemory(&GetPersonalParameter, sizeof(DBR_MB_GetPersonalParameter));
	GetPersonalParameter.dwServerID = pGetPersonalParameter->dwServerID;

	//投递数据
	pIDataBaseEngine->PostDataBaseRequest(DBR_MB_GET_PERSONAL_PARAMETER,dwSocketID, &GetPersonalParameter, sizeof(DBR_MB_GetPersonalParameter));

	return true;
}


//查询私人房间列表
bool CPersonalRoomServiceManager::OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID,  ITCPSocketService * pITCPSocketService)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_MB_QeuryPersonalRoomList));
	if(wDataSize != sizeof(CMD_MB_QeuryPersonalRoomList)) return false;

	CMD_MB_QeuryPersonalRoomList* pQueryPersonalRoomList = (CMD_MB_QeuryPersonalRoomList*)pData;

	CMD_MB_SC_QeuryPersonalRoomList  SC_QeuryPersonalRoomList; 
	SC_QeuryPersonalRoomList.dwUserID = pQueryPersonalRoomList->dwUserID;
	SC_QeuryPersonalRoomList.dwSocketID = dwSocketID;
	SC_QeuryPersonalRoomList.dwKindID = pQueryPersonalRoomList->dwKindID;

	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** 登录服务器 查询私人房间列表 dwSocketID = %d   SC_QeuryPersonalRoomList.dwUserID = %d"), dwSocketID,  SC_QeuryPersonalRoomList.dwUserID);
	OutputDebugString(szInfo);
	//构造数据
	pITCPSocketService->SendData(MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_PERSONAL_ROOM_LIST, &SC_QeuryPersonalRoomList, sizeof(CMD_MB_SC_QeuryPersonalRoomList));

	return true;
}


//玩家请求房间成绩+
bool CPersonalRoomServiceManager::OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID,  IDataBaseEngine *	 pIDataBaseEngine)
{
	OutputDebugString(TEXT("ptdt *** OnTCPNetworkSubQueryUserRoomScore"));

	//校验数据
	ASSERT(wDataSize == sizeof(CMD_GR_QUERY_USER_ROOM_SCORE));
	if(wDataSize != sizeof(CMD_GR_QUERY_USER_ROOM_SCORE)) return false;

	//提取数据
	CMD_GR_QUERY_USER_ROOM_SCORE * pQueryUserRoomScore = (CMD_GR_QUERY_USER_ROOM_SCORE*)pData;
	ASSERT(pQueryUserRoomScore!=NULL);

	//发送数据
	DBR_GR_QUERY_USER_ROOM_INFO QUERY_USER_ROOM_INFO;
	ZeroMemory(&QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));
	QUERY_USER_ROOM_INFO.dwUserID = pQueryUserRoomScore->dwUserID;//请求房间成绩的玩家

	//投递数据
	pIDataBaseEngine->PostDataBaseRequest(DBR_GR_QUERY_USER_ROOM_SCORE,dwSocketID, &QUERY_USER_ROOM_INFO, sizeof(DBR_GR_QUERY_USER_ROOM_INFO));

	return true;
}

//用户接口
IUnknownEx * CPersonalRoomServiceManager::GetServerUserItemSink()
{
	ASSERT(m_pIPersonalRoomItem!=NULL);
	if(m_pIPersonalRoomItem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIPersonalRoomItem,IUnknownEx);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(PersonalRoomServiceManager);

////////////////////////////////////////////////////////////////////////
