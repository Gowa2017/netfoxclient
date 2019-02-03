#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////
//定时器定义

#define IDI_LOAD_PLATFORM_PARAMETER		1							//加载参数
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//时间间隔

#define IDI_HTTP_EVENT_LOOP		2									//HTTP事件分发
#define TIME_HTTP_EVENT_LOOP	1						//时间间隔

std::vector<std::string> split(const  std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			std::string result = s.substr(pos, len - pos);
			if (result.size())
				elems.push_back(result);
			break;
		}
		std::string result = s.substr(pos, find_pos - pos);
		if (result.size())
			elems.push_back(result);
		pos = find_pos + delim_len;
	}
	return elems;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_wCollectItem=INVALID_WORD;

	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pHttpServerService = NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
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
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//设置定时器
#ifndef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,TIME_LOAD_PLATFORM_PARAMETER,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,60000,TIMES_INFINITY,0);
#endif

	m_pHttpServerService = new CHttpKernel<CAttemperEngineSink>();
	
	if (m_pHttpServerService->InitServer(PORT_CORRESPOND_HTTP) == false)
	{
		CTraceService::TraceString(_T("HTTP启动失败，请检查端口时候已被占用"),TraceLevel_Warning);
	}
	else
	{
		m_pHttpServerService->SetReqPostCallBack(&CAttemperEngineSink::OnServerPostCallBack, this, NULL);
		m_pHttpServerService->SetReqGetCallBack(&CAttemperEngineSink::OnServerGetCallBack, this, NULL);
		m_pITimerEngine->SetTimer(IDI_HTTP_EVENT_LOOP,TIME_HTTP_EVENT_LOOP,TIMES_INFINITY,0);
	}

	AllocConsole();
	freopen("CONIN$", "r+t", stdin); // 重定向 STDIN
	freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT 

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	if (m_pHttpServerService)
		SafeDelete(m_pHttpServerService);

	//设置组件
	m_GlobalInfoManager.ResetData();

	FreeConsole();

	return true;
}

void CAttemperEngineSink::OnServerPostCallBack(struct evhttp_request *req, void * pArg)
{
	int	wMainCmdID = -1;							//主命令码

	//CAttemperEngineSink * pAttemerEngineSink = (CAttemperEngineSink *)pArg;

	if (req == NULL)
	{
		printf("some error occur!");
		return;
	}

	struct evbuffer * pBuffer = evhttp_request_get_input_buffer(req);
	size_t dataLength = evbuffer_get_length(pBuffer);
	char * out = (char*)malloc(dataLength);
	do 
	{
		if (evbuffer_remove(pBuffer, out, dataLength) > 0)
		{
			//处理来自网站的post的数据
			Json::Reader reader;
			Json::Value root;

			bool ret = reader.parse(out, root);
			if (ret)  // json解析
			{ 
				if (!root.isObject())
				{
					evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
					break;
				}

				int msgid =  root["msgid"].asInt();
				this->OnEventServerHttpNetworkRead(msgid, out, dataLength, req);
			}
			else
			{//暂不支持的数据解析方式
				evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
				break;
			}
		}
		else
		{
			evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
			break;
		}
	} while (0);

	free(out);
}

void CAttemperEngineSink::OnServerGetCallBack(struct evhttp_request *req, void * pArg)
{
	struct evbuffer * pResponce = NULL;
	struct evhttp_uri * pUriDecoded = NULL;
	char* szUriDecodedPath = NULL;

	evhttp_send_reply(req, HTTP_OK, "OK", NULL);


	//do
	//{
	//	auto szUri = evhttp_request_get_uri(req);

	//	struct evkeyvalq *headers = evhttp_request_get_input_headers(req);

	//	//遍历获取Http所有 Head
	//	for (auto header = headers->tqh_first; header != NULL; header = header->next.tqe_next)
	//	{
	//		printf("< %s: %s\n", header->key, header->value);
	//	}

	//	pUriDecoded = evhttp_uri_parse(szUri);
	//	if (!pUriDecoded)
	//	{
	//		printf("It's not a good URI, Sending BADREQUEST\n");
	//		evhttp_send_error(req, HTTP_BADREQUEST, 0);
	//		break;
	//	}

	//	/* Let's see what path the user asked for. */
	//	auto szUriPath = evhttp_uri_get_path(pUriDecoded);
	//	if (!szUriPath)
	//	{
	//		szUriPath = "/";
	//	}

	//	/* We need to decode it, to see what path the user really wanted */
	//	szUriDecodedPath = evhttp_uridecode(szUriPath, 0, NULL);
	//	if (szUriDecodedPath == NULL)
	//	{
	//		evhttp_send_error(req, HTTP_BADREQUEST, "uri decode error");
	//		break;
	//	}

	//	std::string strLocalPath;
	//	/* 遍历获取Get URI键值对 */
	//	struct evkeyvalq params;
	//	evhttp_parse_query_str(szUri, &params);

	//	std::string strKeyValue;

	//	std::map<std::string, std::string>	keyValueSet;
	//	//路径匹配正则表达式
	//	std::regex pathPattern("(?:/(.*)\\?)*(.*)", std::regex::icase);
	//	//std::regex keyValuePattern("([a-z0-9]+)=([a-z0-9]+)", std::regex::icase);
	//	std::cmatch cm;
	//	//遍历获取Http所有 Head
	//	for (auto header = params.tqh_first; header != NULL; header = header->next.tqe_next)
	//	{
	//		std::regex_match(header->key, cm, pathPattern);
	//		std::string parm1 = cm.format("$1");
	//		if (!parm1.empty())
	//		{
	//			strLocalPath = cm.format("$1");
	//			keyValueSet.insert(std::make_pair(cm.format("$2"), header->value));
	//		}
	//		else
	//		{
	//			//std::regex_match(header->key, cm, keyValuePattern);
	//			keyValueSet.insert(std::make_pair(header->key, header->value));
	//		}
	//	}

	//	/* This holds the content we're sending */
	//	pResponce = evbuffer_new();
	//	if (!pResponce)
	//	{
	//		evhttp_send_error(req, HTTP_INTERNAL, "got err,please try again later!");
	//		break;
	//	}

	//	//文件存在，直接返回文件内容，否则返回404
	//	if (FileUtils::getInstance()->isFileExist(strLocalPath))
	//	{   /* it's a file; and it to the buffer to get send via sendfile */
	//		auto type = CHttpKernel::MappedMimeType(strLocalPath.c_str());
	//		std::string strContent;
	//		FileUtils::getInstance()->getContents(strLocalPath, &strContent);
	//		evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", type);
	//		evbuffer_add(pResponce, strContent.c_str(), strContent.length());
	//	}
	//	else/* if (FileUtils::getInstance()->isDirectoryExist(szDecodedPath))*/
	//	{
	//		//auto type = CHttpKernel::MappedMimeType(NULL);
	//		//evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", type);
	//		evhttp_send_error(req, HTTP_NOTFOUND, "Not Support!");
	//		break;
	//	}

	//	evhttp_send_reply(req, HTTP_OK, "OK", pResponce);
	//} while (0);

	//if (pUriDecoded)
	//	evhttp_uri_free(pUriDecoded);
	//if (szUriDecodedPath)
	//	free(szUriDecodedPath);
	//if (pResponce)
	//	evbuffer_free(pResponce);
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
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
	//加载参数
	if(IDI_LOAD_PLATFORM_PARAMETER==dwTimerID)
	{
		//发送通知
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PLATFORM_PARAMETER,0,0,0L);

		return true;
	}
	else if (IDI_HTTP_EVENT_LOOP == dwTimerID)
	{
		if (m_pHttpServerService)
			m_pHttpServerService->EventDispatch();
		return true;
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	bool ret = true;
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//服务注册
		{
			ret = OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_SERVICE_INFO:	//服务信息
		{
			ret = OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_USER_COLLECT:	//用户命令
		{
			ret = OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_REMOTE_SERVICE:	//远程服务
		{
			ret = OnTCPNetworkMainRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_MANAGER_SERVICE: //管理服务
		{
			ret = OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_ANDROID_SERVICE: //机器服务
		{
			ret = OnTCPNetworkMainAndroidService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	}

	return ret;
}

bool CAttemperEngineSink::OnEventServerHttpNetworkRead(DWORD dwMsgID, VOID * pData, WORD wDataSize,struct evhttp_request *req)
{
	switch(dwMsgID)
	{
	case SUB_CS_C_HTTP_QUERY_ONLINE:
		{
			return OnHttpQueryChatServerUserOnline(dwMsgID,pData,wDataSize,req);
		}
	case SUB_CS_C_CLEARUSER:
		{
			return OnHttpClearUser(dwMsgID, pData, wDataSize, req);
		}
	case SUB_CS_C_HTTP_QUERY_DUMMY_ONLINE:
		{
			return OnHttpQueryDummyOnlineUser(dwMsgID, pData, wDataSize, req);
		}
	case SUB_CS_C_HTTP_CHANGE_DUMMY_ONLINE:
		{
			return OnHttpChangeDummyOnlineUser(dwMsgID, pData, wDataSize, req);
		}
	case SUB_CS_C_HTTP_FORCE_OFFLINE:
		{
			return OnHttpForceOffline(dwMsgID, pData, wDataSize, req);
		}
	}

	m_pHttpServerService->SendData(req, "非法请求！", sizeof("Error"), "txt", HTTP_INTERNAL);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);

		//汇总处理
		if (wBindIndex==m_wCollectItem)
		{
			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//提取变量
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//删除等待
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		//变量定义
		CMD_CS_S_ServerRemove ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//删除通知
		ServerRemove.wServerID=pBindParameter->wServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_REMOVE,&ServerRemove,sizeof(ServerRemove),0L);

		//注销房间
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);
	}

	//广场服务
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//注销房间
		m_GlobalInfoManager.DeletePlazaItem(pBindParameter->wServiceID);
	}

	//聊天服务
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//注销房间
		m_GlobalInfoManager.DeleteChatItem(pBindParameter->wServiceID);
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_PLAZA:	//注册广场
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterPlaza));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterPlaza)) return false;

			//消息定义
			CMD_CS_C_RegisterPlaza * pRegisterPlaza=(CMD_CS_C_RegisterPlaza *)pData;

			//有效判断
			if ((pRegisterPlaza->szServerName[0]==0)||(pRegisterPlaza->szServerAddr[0]==0))
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("服务器注册失败，“服务地址”与“服务器名”不合法！"),CountArray(RegisterFailure.szDescribeString));

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Plaza;

			//变量定义
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//构造数据
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerName,pRegisterPlaza->szServerName,CountArray(GamePlaza.szServerName));
			lstrcpyn(GamePlaza.szServerAddr,pRegisterPlaza->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//注册房间
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//发送列表
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_SERVER:	//注册房间
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//消息定义
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//查找房间
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("已经存在相同标识的游戏房间服务，房间服务注册失败"),CountArray(RegisterFailure.szDescribeString));

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//变量定义
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//构造数据
			GameServer.wKindID=pRegisterServer->wKindID;
			GameServer.wNodeID=pRegisterServer->wNodeID;
			GameServer.wSortID=pRegisterServer->wSortID;
			GameServer.wServerID=pRegisterServer->wServerID;
			GameServer.wServerKind=pRegisterServer->wServerKind;
			GameServer.wServerType=pRegisterServer->wServerType;
			GameServer.wServerLevel=pRegisterServer->wServerLevel;
			GameServer.wServerPort=pRegisterServer->wServerPort;
			GameServer.lCellScore=pRegisterServer->lCellScore;
			GameServer.cbEnterMember=pRegisterServer->cbEnterMember;
			GameServer.lEnterScore=pRegisterServer->lEnterScore;
			GameServer.dwServerRule=pRegisterServer->dwServerRule;
			GameServer.dwOnLineCount=pRegisterServer->dwOnLineCount;
			GameServer.dwFullCount=pRegisterServer->dwFullCount;
			GameServer.wTableCount = pRegisterServer->wTableCount;
			lstrcpyn(GameServer.szServerName,pRegisterServer->szServerName,CountArray(GameServer.szServerName));
			lstrcpyn(GameServer.szServerAddr,pRegisterServer->szServerAddr,CountArray(GameServer.szServerAddr));

			//注册房间
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,&GameServer,sizeof(GameServer),0L);

			//发送列表
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//汇总通知
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}	
	case SUB_CS_C_REGISTER_MATCH:	//注册比赛
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterMatch));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterMatch)) return false;

			//消息定义
			CMD_CS_C_RegisterMatch * pRegisterMatch=(CMD_CS_C_RegisterMatch *)pData;

			//查找房间
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pRegisterMatch->wServerID);
			if(pGlobalServerItem==NULL) return true;

			//变量定义
			tagGameMatch GameMatch;
			ZeroMemory(&GameMatch,sizeof(GameMatch));

			//构造数据
			GameMatch.wServerID = pRegisterMatch->wServerID;
			GameMatch.dwMatchID = pRegisterMatch->dwMatchID;
			GameMatch.cbMatchType = pRegisterMatch->cbMatchType;
			lstrcpyn(GameMatch.szMatchName,pRegisterMatch->szMatchName,CountArray(GameMatch.szMatchName));

			GameMatch.cbFeeType = pRegisterMatch->cbFeeType;
			GameMatch.cbDeductArea = pRegisterMatch->cbDeductArea;
			GameMatch.lSignupFee = pRegisterMatch->lSignupFee;
			GameMatch.cbSignupMode = pRegisterMatch->cbSignupMode;
			GameMatch.cbJoinCondition = pRegisterMatch->cbJoinCondition;
			GameMatch.cbMemberOrder = pRegisterMatch->cbMemberOrder;
			GameMatch.lExperience = pRegisterMatch->dwExperience;

			GameMatch.cbRankingMode = pRegisterMatch->cbRankingMode;
			GameMatch.wCountInnings = pRegisterMatch->wCountInnings;
			GameMatch.cbFilterGradesMode = pRegisterMatch->cbFilterGradesMode;

			CopyMemory(&GameMatch.cbMatchRule,&pRegisterMatch->cbMatchRule,sizeof(GameMatch.cbMatchRule));

			GameMatch.wRewardCount = pRegisterMatch->wRewardCount;
			CopyMemory(&GameMatch.MatchRewardInfo,pRegisterMatch->MatchRewardInfo,sizeof(GameMatch.MatchRewardInfo));

			//拷贝数据
			CopyMemory(&pGlobalServerItem->m_GameMatch,&GameMatch,sizeof(pGlobalServerItem->m_GameMatch));

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,&GameMatch,sizeof(GameMatch),0L);

			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//注册聊天
		{		
			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Chat;

			tagChatServer ChatServer ;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter+wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex,ChatServer);

			//群发房间
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT,&ChatServer,sizeof(ChatServer),0L);

			//发送信息
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_PERSONAL:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_S_RegisterPersonal));
			if (wDataSize!=sizeof(CMD_CS_S_RegisterPersonal)) return false;
			CMD_CS_S_RegisterPersonal * RegisterPersonal = (CMD_CS_S_RegisterPersonal *)pData;
			m_GlobalInfoManager.AddPersonalMaxCreate(* RegisterPersonal);
			return true;
		}
	}

	return false;
}

//服务状态
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_ONLINE:	//房间人数
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_C_ServerOnLine)) return false;

			//消息定义
			CMD_CS_C_ServerOnLine * pServerOnLine=(CMD_CS_C_ServerOnLine *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//查找房间
			WORD wServerID=pBindParameter->wServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//设置人数
			if (pGlobalServerItem!=NULL)
			{
				//变量定义
				CMD_CS_S_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//设置变量
				pGlobalServerItem->m_GameServer.dwAndroidCount=pServerOnLine->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;

				//设置变量
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;
				ServerOnLine.dwAndroidCount=pServerOnLine->dwAndroidCount;

				//发送通知
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
	case SUB_CS_C_SERVER_MODIFY:	//房间修改
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//消息定义
			CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//查找房间
			ASSERT(m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID)!=NULL);
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//房间修改
			if (pGlobalServerItem!=NULL)
			{
				//设置变量
				pGlobalServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwAndroidCount = pServerModify->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;				
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGlobalServerItem->m_GameServer.szServerName));
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGlobalServerItem->m_GameServer.szServerAddr));

				//变量定义
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify,sizeof(ServerModify));

				//设置变量
				ServerModify.wKindID=pServerModify->wKindID;
				ServerModify.wSortID=pServerModify->wSortID;
				ServerModify.wNodeID=pServerModify->wNodeID;
				ServerModify.wServerID=pBindParameter->wServiceID;
				ServerModify.wServerPort=pServerModify->wServerPort;
				ServerModify.dwOnLineCount=pServerModify->dwOnLineCount;
				ServerModify.dwFullCount=pServerModify->dwFullCount;
				ServerModify.dwAndroidCount= pServerModify->dwAndroidCount;
				lstrcpyn(ServerModify.szServerAddr,pServerModify->szServerAddr,CountArray(ServerModify.szServerAddr));
				lstrcpyn(ServerModify.szServerName,pServerModify->szServerName,CountArray(ServerModify.szServerName));

				//发送通知
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
			}

			return true;
		}
	case SUB_CS_C_SEARCH_TABLE:			//查找桌子
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//查找桌子
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->szPersonalTableID);

			//构造数据
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_C_DISSUME_SEARCH_TABLE:			//查找桌子
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//查找桌子
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->szPersonalTableID);

			//构造数据
			CMD_CS_S_SearchTableResult SearchTableResult;
			ZeroMemory(&SearchTableResult, sizeof(CMD_CS_S_SearchTableResult));

			SearchTableResult.dwSocketID = pSearchTable->dwSocketID;
			SearchTableResult.dwClientAddr = pSearchTable->dwClientAddr;

			if(pPersonalTable == NULL)
			{
				SearchTableResult.dwServerID = 0;
				SearchTableResult.dwTableID = 0;
			}
			else
			{
				SearchTableResult.dwServerID = pPersonalTable->dwServerID;
				SearchTableResult.dwTableID = pPersonalTable->dwTableID;
			}
			
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT, &SearchTableResult, sizeof(SearchTableResult));

			return true;
		}
	case SUB_CS_C_QUERY_GAME_SERVER:		//查询可用房间
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_QueryGameServer));
			if (wDataSize!=sizeof(CMD_CS_C_QueryGameServer)) return false;

			CMD_CS_C_QueryGameServer *pQueryGameServer = (CMD_CS_C_QueryGameServer *)pData;

			//判断是否可以创建私人房间
			DWORD dwServerID = 0;
			bool bCanCreateRoom = false;

			CMD_CS_S_QueryGameServerResult QueryGameServerResult;
			ZeroMemory(&QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			if (m_GlobalInfoManager.CanCreatePersonalRoom(pQueryGameServer->dwKindID, pQueryGameServer->dwUserID))
			{
				//空闲桌子
				dwServerID = m_GlobalInfoManager.GetFreeServer(pQueryGameServer->dwUserID, pQueryGameServer->dwKindID, pQueryGameServer->cbIsJoinGame);
				bCanCreateRoom = true;

				if (dwServerID == 0)
				{
					lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("房主必须参与游戏模式下只能创建一个房间，或游戏服务器创建私人房间已满！"),  sizeof(QueryGameServerResult.szErrDescrybe));
				}
			}
			else
			{
				lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("已经达到创建私人房间的最大数，不能再创建房间！"),  sizeof(QueryGameServerResult.szErrDescrybe));
			}

			//构造数据
			QueryGameServerResult.dwSocketID = pQueryGameServer->dwSocketID;
			QueryGameServerResult.dwClientAddr = pQueryGameServer->dwClientAddr;
			QueryGameServerResult.dwServerID = dwServerID;
			QueryGameServerResult.bCanCreateRoom= bCanCreateRoom;


			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_GAME_SERVER_RESULT, &QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			return true;
		}
	case SUB_CS_C_CREATE_TABLE:		//创建桌子
		{
			//校验数据
			ASSERT(wDataSize == sizeof(CMD_CS_C_CreateTable));
			if(wDataSize != sizeof(CMD_CS_C_CreateTable)) return false;

			CMD_CS_C_CreateTable* pCreateTable = (CMD_CS_C_CreateTable*)pData;

			//生成ID
			TCHAR szServerID[7] = TEXT("");
			m_GlobalInfoManager.RandServerID(szServerID, 6);

			lstrcpyn(pCreateTable->PersonalTable.szRoomID, szServerID, CountArray(pCreateTable->PersonalTable.szRoomID));
			//汇总桌子
			m_GlobalInfoManager.AddServerTable(szServerID, pCreateTable->PersonalTable);
			m_GlobalInfoManager.RemoveFreeServerTable(pCreateTable->PersonalTable.dwServerID);

			//构造数据
			CMD_CS_S_CreateTableResult CreateTableResult;
			ZeroMemory(&CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));

			CreateTableResult.dwSocketID = pCreateTable->dwSocketID;
			CreateTableResult.PersonalTable.dwTableID = pCreateTable->PersonalTable.dwTableID;
			CreateTableResult.PersonalTable.dwServerID = pCreateTable->PersonalTable.dwServerID;
			CreateTableResult.PersonalTable.dwKindID = pCreateTable->PersonalTable.dwKindID;
			CreateTableResult.PersonalTable.dwUserID = pCreateTable->PersonalTable.dwUserID;
			CreateTableResult.PersonalTable.lCellScore = pCreateTable->PersonalTable.lCellScore;
			CreateTableResult.PersonalTable.dwDrawTimeLimit = pCreateTable->PersonalTable.dwDrawTimeLimit;
			CreateTableResult.PersonalTable.dwDrawCountLimit = pCreateTable->PersonalTable.dwDrawCountLimit;
			lstrcpyn(CreateTableResult.PersonalTable.szRoomID, szServerID, CountArray(CreateTableResult.PersonalTable.szRoomID));
			lstrcpyn(CreateTableResult.PersonalTable.szPassword, pCreateTable->PersonalTable.szPassword, CountArray(CreateTableResult.PersonalTable.szPassword));
			lstrcpyn(CreateTableResult.szClientAddr, pCreateTable->szClientAddr, CountArray(CreateTableResult.szClientAddr));
			CreateTableResult.PersonalTable.wJoinGamePeopleCount = pCreateTable->PersonalTable.wJoinGamePeopleCount;
			CreateTableResult.PersonalTable.dwRoomTax = pCreateTable->PersonalTable.dwRoomTax;

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_CREATE_TABLE_RESULT, &CreateTableResult, sizeof(CMD_CS_S_CreateTableResult));


			return true;
		}
	case SUB_CS_C_DISMISS_TABLE:		//解散桌子
		{
			//校验数据
			ASSERT(wDataSize == sizeof(CMD_CS_C_DismissTable));
			if(wDataSize != sizeof(CMD_CS_C_DismissTable)) return false;

			CMD_CS_C_DismissTable* pDismissTable = (CMD_CS_C_DismissTable*)pData;

			tagPersonalTableInfo * pPersonalTableInfo = m_GlobalInfoManager.SearchTableByTableIDAndServerID(pDismissTable->dwServerID, pDismissTable->dwTableID);
			if (pPersonalTableInfo == NULL)
			{
				return true;
			}

			CMD_CS_C_DismissTableResult  DismissTableResult;
			DismissTableResult.dwSocketID = pDismissTable->dwSocketID;
			memcpy(&DismissTableResult.PersonalTableInfo, pPersonalTableInfo, sizeof(tagPersonalTableInfo));

			//汇总桌子
			m_GlobalInfoManager.RemoveServerTable(pDismissTable->dwServerID, pDismissTable->dwTableID);
			m_GlobalInfoManager.AddFreeServerTable(pDismissTable->dwServerID);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_DISMISS_TABLE_RESULT, &DismissTableResult, sizeof(CMD_CS_C_DismissTableResult));

			return true;
		}
	case SUB_CS_S_SERVER_DUMMY_INFO_START:
		{
			m_GlobalInfoManager.ClearDummyItem();
			return true;
		}
	case SUB_CS_S_SERVER_DUMMY_INFO:
		{
			//校验数据
			ASSERT((wDataSize % sizeof(CMD_CS_C_DummyOnLine)) == 0);
			if((wDataSize % sizeof(CMD_CS_C_DummyOnLine)) != 0) return false;

			CMD_CS_C_DummyOnLine* pDumOnline = (CMD_CS_C_DummyOnLine*)pData;

			//变量定义
			WORD wItemCount=wDataSize/sizeof(CMD_CS_C_DummyOnLine);

			//更新数据
			for (WORD i=0;i<wItemCount;i++)
			{
				tagDummyInfo DummyInfo;
				DummyInfo.wServerID = pDumOnline->wServerID;
				DummyInfo.dwDummyCount = pDumOnline->dwDummyCount;
				 m_GlobalInfoManager.ActiveDummyItem(DummyInfo, pDumOnline->wServerID);

				pDumOnline++;
			}

			return true;
		}
	case SUB_CS_S_SERVER_DUMMY_INFO_END:
		{
			return true;
		}
	case SUB_CS_S_QUERY_PERSONAL_ROOM_LIST:		//请求房间列表
		{
			//校验数据
			ASSERT(wDataSize == sizeof(CMD_MB_SC_QeuryPersonalRoomList));
			if(wDataSize != sizeof(CMD_MB_SC_QeuryPersonalRoomList)) return false;

			CMD_MB_SC_QeuryPersonalRoomList * QeuryPersonalRoomList = (CMD_MB_SC_QeuryPersonalRoomList * ) pData;

			CMD_CS_C_HostCreatRoomInfo  cmdHostCreatRoomInfo;
			ZeroMemory(&cmdHostCreatRoomInfo,  sizeof(CMD_CS_C_HostCreatRoomInfo));

			cmdHostCreatRoomInfo.wSocketID = QeuryPersonalRoomList->dwSocketID;
			cmdHostCreatRoomInfo.HostCreatRoomInfo.dwUserID = QeuryPersonalRoomList->dwUserID;
			cmdHostCreatRoomInfo.HostCreatRoomInfo.dwKindID = QeuryPersonalRoomList->dwKindID;
			m_GlobalInfoManager.GetHostCreatePersonalRoom(cmdHostCreatRoomInfo.HostCreatRoomInfo);

			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT, &cmdHostCreatRoomInfo, sizeof(CMD_CS_C_HostCreatRoomInfo));

			return true;
		}
	}

	return false;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//用户进入
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//消息处理
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			GlobalUserInfo.cbAndroidUser = pUserEnter->cbAndroidUser;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//辅助信息
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;
			
			//拷贝详细信息
			memcpy(&GlobalUserInfo.userInfo, &pUserEnter->userInfo, sizeof(tagUserInfo));

			//激活用户
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->wServiceID);


			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserEnter->dwUserID;
			UserGameStatus.cbGameStatus=pUserEnter->userInfo.cbUserStatus;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));
			

			//变量定义
			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//用户离开
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//消息处理
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//删除用户
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserLeave->dwUserID;
			UserGameStatus.cbGameStatus=US_NULL;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;

			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_CHAT_USER_ENTER:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_ChatUserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_ChatUserEnter)) return false;

			//消息处理
			CMD_CS_C_ChatUserEnter * pUserEnter=(CMD_CS_C_ChatUserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Chat);
			if (pBindParameter->ServiceKind!=ServiceKind_Chat) return false;


			//变量定义
			tagGlobalChatUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//激活用户
			m_GlobalInfoManager.ActiveChatUserItem(GlobalUserInfo,pBindParameter->wServiceID);

			return true;
		}
	case SUB_CS_C_CHAT_USER_LEAVE:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_ChatUserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_ChatUserLeave)) return false;

			//消息处理
			CMD_CS_C_ChatUserLeave * pUserLeave=(CMD_CS_C_ChatUserLeave *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Chat);
			if (pBindParameter->ServiceKind!=ServiceKind_Chat) return false;



			//删除用户
			m_GlobalInfoManager.DeleteChatUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);


			return true;
		}
	case SUB_CS_C_USER_FINISH:		//用户完成
		{
			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//切换汇总
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_STATUS:		//用户状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserStatus));
			if (wDataSize!=sizeof(CMD_CS_C_UserStatus)) return false;

			//消息处理
			CMD_CS_C_UserStatus * pUserStatus=(CMD_CS_C_UserStatus *)pData;

			CGlobalUserItem* pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUserStatus->dwUserID);
			if(pGlobalUserItem != NULL)
			{
				pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

			}

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//同步状态
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserStatus->dwUserID;
			UserGameStatus.cbGameStatus=pUserStatus->cbUserStatus;
			UserGameStatus.wKindID =pUserStatus->wKindID;
			UserGameStatus.wServerID=pUserStatus->wServerID;
			UserGameStatus.wTableID=pUserStatus->wTableID;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			//变量定义
			POSITION posChat=NULL;

			//查找房间
			do
			{
				//查找房间
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//终止判断
				if (pGlobalChatItem==NULL) break;

				//发送状态
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	}

	return false;
}

//远程服务
bool CAttemperEngineSink::OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SEARCH_CORRESPOND:	//协调查找
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			//处理消息
			CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//变量定义
			CMD_CS_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//结果用户
			CGlobalUserItem * ResultUserItem[2];
			ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			//设置变量
			SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			//查找用户
			if (pSearchCorrespond->dwGameID!=0L)
			{
				ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			}

			//查找用户
			if (pSearchCorrespond->szNickName[0]!=0)
			{
				_tcslwr(pSearchCorrespond->szNickName);
				ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			}

			//设置结果
			for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			{
				if (ResultUserItem[i]!=NULL)
				{
					//变量定义
					WORD wServerIndex=0;

					//查找房间
					do
					{
						//查找房间
						CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

						//终止判断
						if (pGlobalServerItem==NULL) break;
						if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

						//索引定义
						WORD wIndex=SearchCorrespond.wUserCount++;

						//辅助信息
						SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

						//用户信息
						SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
						SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

						//房间信息
						SearchCorrespond.UserRemoteInfo[wIndex].wKindID=pGlobalServerItem->GetKindID();
						SearchCorrespond.UserRemoteInfo[wIndex].wServerID=pGlobalServerItem->GetServerID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szGameServer,pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);

						tagUserInfo* pUserInfo = ResultUserItem[i]->GetUserInfo();
						SearchCorrespond.UserRemoteInfo[wIndex].wFaceID = pUserInfo->wFaceID;
						SearchCorrespond.UserRemoteInfo[wIndex].wChairID = pUserInfo->wChairID;
						SearchCorrespond.UserRemoteInfo[wIndex].cbUserStatus = pUserInfo->cbUserStatus;
						SearchCorrespond.UserRemoteInfo[wIndex].wTableID = pUserInfo->wTableID;
						SearchCorrespond.UserRemoteInfo[wIndex].wLastTableID = pUserInfo->wLastTableID;
						

					} while (true);
				}
			}

			//发送数据
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//协调查找
		{
			CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			if(pSearchCorrespond->dwCount > 512) return false;

			BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			//设置变量
			pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			int nCount = 0;
			const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			int cbPacketSize = cbPacketHeadSize;
			//查找用户
			int nSearchCount = (int)pSearchCorrespond->dwCount;
			for(int i = 0; i < nSearchCount; i++)
			{
				CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
				if(pGlobalUserItem != NULL)
				{
					tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
					//查找房间
					WORD wServerIndex=0;
					CGlobalServerItem * pGlobalServerItem = pGlobalUserItem->EnumServerItem(wServerIndex++);
					
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwUserID = pUserInfo->dwUserID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].dwGameID = pUserInfo->dwGameID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMasterOrder = pUserInfo->cbMasterOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbMemberOrder = pUserInfo->cbMemberOrder;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbGender = pUserInfo->cbGender;
					pAllSearchCorrespond->UserRemoteInfo[nCount].cbUserStatus = pUserInfo->cbUserStatus;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wFaceID = pUserInfo->wFaceID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wChairID = pUserInfo->wChairID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wTableID = pUserInfo->wTableID;
					pAllSearchCorrespond->UserRemoteInfo[nCount].wLastTableID = pUserInfo->wLastTableID;
					lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szNickName,pUserInfo->szNickName, LEN_NICKNAME);
					if(pGlobalServerItem != NULL)
					{
						pAllSearchCorrespond->UserRemoteInfo[nCount].wServerID = pGlobalServerItem->GetServerID();
						pAllSearchCorrespond->UserRemoteInfo[nCount].wKindID = pGlobalServerItem->GetKindID();
						lstrcpyn(pAllSearchCorrespond->UserRemoteInfo[nCount].szGameServer, pGlobalServerItem->m_GameServer.szServerName,LEN_SERVER);
					}
					nCount+=1;
					cbPacketSize += sizeof(tagUserRemoteInfo);
				}
				if( cbPacketSize > (SOCKET_TCP_PACKET-sizeof(CMD_CS_S_SearchAllCorrespond)) )
				{
					pAllSearchCorrespond->dwCount = nCount;
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
					cbPacketSize = cbPacketHeadSize;
					nCount = 0;
				}
			}
			
			pAllSearchCorrespond->dwCount = nCount;
			if(nCount == 0) cbPacketSize = sizeof(CMD_CS_S_SearchAllCorrespond);
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_ALLCORRESPOND, pAllSearchCorrespond, cbPacketSize);
			
			return true;
		}
	}

	return false;
}

//管理服务
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SYSTEM_MESSAGE:	//系统消息
		{
			//发送通知
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_SYSTEM_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_HALL_MESSAGE:		//大厅消息
		{
			//发送通知
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_HALL_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_PROPERTY_TRUMPET:  //喇叭消息
		{
			//发送通知
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PROPERTY_TRUMPET,pData,wDataSize,0L);

			return true;
		}
	}

	return false;
}

//机器服务
bool CAttemperEngineSink::OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_ADDPARAMETER:		//添加参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

			//构造结构
			CMD_CS_S_AddParameter AddParameter;
			CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//发送消息
			SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

			return true;
		}
	case SUB_CS_C_MODIFYPARAMETER:  //修改参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

			//构造结构
			CMD_CS_S_ModifyParameter ModifyParameter;
			CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//发送通知
			SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

			return true;
		}
	case SUB_CS_C_DELETEPARAMETER:  //删除参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

			//构造结构
			CMD_CS_S_DeleteParameter DeleteParameter;
			DeleteParameter.dwBatchID=pDeleteParameter->dwBatchID;

			//发送通知
			SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

			return true;
		}
	}

	return false;
}

//查询在线用户
bool CAttemperEngineSink::OnHttpQueryChatServerUserOnline(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	/*
		[
			{
			"UserID": 1,
			"KindID": 6,
			"ServerID": 74
			},
			{
			"UserID": 2,
			"KindID": 6,
			"ServerID": 74
			},
			{
			"UserID": 3,
			"KindID": 6,
			"ServerID": 74
			}
		]
	*/

	Json::Value root;
	Json::FastWriter writer;

	//遍历 聊天用户
	std::map<int, bool> chatUserList;
	POSITION Position=NULL;
	do
	{
		CGlobalChatUserItem * pGlobalServerItem=m_GlobalInfoManager.EnumChatUserItem(Position);

		//设置数据
		if (pGlobalServerItem!=NULL )
		{
			chatUserList[pGlobalServerItem->GetUserID()] = true;
		}

	} while (Position!=NULL);


	//遍历游戏房间数据
	POSITION PositionServer =NULL;
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(PositionServer);
		if (pGlobalServerItem!=NULL )
		{
			//遍历游戏用户
			POSITION Position = NULL;
			do
			{
				CGlobalUserItem * pGlobalUserItem=NULL;

				DWORD dwUserID=0L;
				//获取对象
				if (Position==NULL)
					Position=pGlobalServerItem->m_MapUserID.GetStartPosition();

				if (Position!=NULL)
					pGlobalServerItem->m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserItem);

				//设置数据
				if (pGlobalUserItem!=NULL )
				{
					if (!pGlobalUserItem->IsAndroid())
					{//过滤 机器人用户
						//CGlobalUserItem * pUserItem = m_GlobalInfoManager.SearchChatUserItemByUserID(dwUserID);
						std::map<int, bool>::iterator itChatUser = chatUserList.find(dwUserID);
						//该用户也同时在聊天服务器 为了网站需求，需要过滤
						if (itChatUser != chatUserList.end())
						{
							chatUserList.erase(itChatUser);
						}

						Json::Value person;

						person["UserID"] = (int)dwUserID;
						person["KindID"] = pGlobalServerItem->GetKindID();
						person["ServerID"] = pGlobalServerItem->GetServerID();

						root.append(person);
					}
				}

			} while (Position!=NULL);
		}

	} while (PositionServer!=NULL);



	for (std::map<int, bool>::const_iterator it = chatUserList.begin(); it != chatUserList.end(); ++it)
	{
		Json::Value person;

		person["UserID"] = (int)it->first;
		person["KindID"] = 0;
		person["ServerID"] = 0;

		root.append(person);
	}

	std::string data= writer.write(root);

	m_pHttpServerService->SendData(req, (void*)data.data(), data.size(), "json");

	return true;
}

//网站踢人
bool CAttemperEngineSink::OnHttpClearUser(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//处理来自网站的post的数据
	Json::Reader reader;
	Json::Value root;

	if (reader.parse((const char *)pData, root))  // json解析
	{ 
		int msgid =  root["msgid"].asInt();
		const Json::Value content = root["content"];
		Json::Value::Members mem = content.getMemberNames();


		CMD_CS_S_ClearUser ClearUser;
		ZeroMemory(&ClearUser,sizeof(ClearUser));
		//遍历json 处理数据
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)        
		{
			const char * pkey = (*iter).c_str();
			//switch (content[*iter].type())
			//{
			//case Json::intValue:
			//	{
			//		int nAcceptData = content[*iter].asInt();
			//	}
			//	break;
			//case Json::uintValue:
			//	{
			//		content[*iter].asUInt();
			//	}
			//	break;
			//case Json::realValue:
			//	{
			//		content[*iter].asDouble();
			//	}
			//	break;
			//case Json::stringValue:
			//	{
			//		char szArray[64] = {0};
			//		memcpy(szArray, content[*iter].asString().c_str(), content[*iter].asString().length());
			//		break;
			//	}
			//case Json::booleanValue:
			//	{
			//		content[*iter].asBool();
			//	}
			//	break;
			//case Json::arrayValue:
			//	{
			//		//解析数组对象
			//		const Json::Value arrayNum = content[*iter];
			//		for (unsigned int i = 0; i < arrayNum.size(); i++)
			//		{
			//			arrayNum[i];
			//		}
			//	}
			//	break;
			//case Json::objectValue:
			//	{
			//		//解析Json对象
			//		const Json::Value obj = content[*iter];
			//	}
			//	break;
			//default:
			//	break;
			//}     
			if(strcmp(pkey,"userid")==0)
			{
				ClearUser.dwUserID = content[pkey].asInt();
			}
			else if(strcmp(pkey,"serverid")==0)
			{
				ClearUser.dwServerID = content[pkey].asInt();
			}
		}
		if(ClearUser.dwServerID != 0)
		{
			//查找房间
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(ClearUser.dwServerID);
			if (pGlobalServerItem==NULL) return true;

			//获取参数
			WORD wBindIndex=pGlobalServerItem->GetIndex();
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//发送数据
			DWORD dwSocketID=pBindParameter->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CLEARUER,&ClearUser,sizeof(ClearUser));
		}
		else
		{
			//变量定义
			WORD wPacketSize=0L;
			POSITION Position=NULL;

			//收集数据
			do
			{
				CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

				//设置数据
				if (pGlobalServerItem!=NULL )
				{

					//获取参数
					WORD wBindIndex=pGlobalServerItem->GetIndex();
					tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

					//发送数据
					DWORD dwSocketID=pBindParameter->dwSocketID;
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CLEARUER,&ClearUser,sizeof(ClearUser));
				}

			} while (Position!=NULL);
		}

	}

	m_pHttpServerService->SendData(req, "OK", sizeof("OK"));

	return true;
}

//踢掉大厅在线用户（强制用户下线）
bool CAttemperEngineSink::OnHttpForceOffline(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//处理来自网站的post的数据
	Json::Reader reader;
	Json::Value root;

	if (reader.parse((const char *)pData, root))  // json解析
	{ 
		int msgid =  root["msgid"].asInt();
		const Json::Value content = root["content"];
		Json::Value::Members mem = content.getMemberNames();


		WORD wPacketSize=0L;
		BYTE cbBuffer[SOCKET_TCP_PACKET];

		//遍历json 处理数据
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)        
		{
			const char * pkey = (*iter).c_str();

			if(strcmp(pkey,"userid")==0)
			{
				std::string str_userid_list = content[pkey].asString();
				std::vector<std::string> userid_list = split(str_userid_list, ",");

				for (std::vector<std::string>::const_iterator it = userid_list.begin(); it != userid_list.end(); ++it)
				{	
					//发送数据
					if ((wPacketSize+sizeof(CMD_CS_S_ForceOffline))>sizeof(cbBuffer))
					{
						m_pITCPNetworkEngine->SendDataBatch(MDM_CS_USER_COLLECT, SUB_CS_S_CHAT_FORCE_OFFLINE, cbBuffer, wPacketSize, 0L);
						wPacketSize=0;
					}

					CMD_CS_S_ForceOffline * pOffLine=(CMD_CS_S_ForceOffline *)(cbBuffer+wPacketSize);

					//设置数据
					wPacketSize+=sizeof(CMD_CS_S_ForceOffline);
					pOffLine->dwUserID = atoi(it->c_str());


					//从所有聊天服务器中删除该用户
					m_GlobalInfoManager.DeleteChatUserItem(pOffLine->dwUserID,0);
				}
				//发送数据
				if (wPacketSize>0) m_pITCPNetworkEngine->SendDataBatch(MDM_CS_USER_COLLECT, SUB_CS_S_CHAT_FORCE_OFFLINE, cbBuffer, wPacketSize, 0L);
			}
		}

	}

	return m_pHttpServerService->SendData(req, "OK", sizeof("OK"));
}

//查询虚拟人数
bool CAttemperEngineSink::OnHttpQueryDummyOnlineUser(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//处理来自网站的post的数据

	Json::Value root;
	Json::FastWriter writer;

	//遍历游戏房间数据
	POSITION PositionServer =NULL;
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(PositionServer);
		if (pGlobalServerItem!=NULL )
		{
			//游戏在线人数

			Json::Value ServerItem;
			ServerItem["KindID"] = (int)pGlobalServerItem->GetKindID();
			ServerItem["ServerID"] = (int)pGlobalServerItem->GetServerID();
			ServerItem["UserCount"] = (int)pGlobalServerItem->GetRealUserItemCount();
			ServerItem["AndroidCount"] = (int)pGlobalServerItem->GetAndroidUserItemCount();

			//搜索用户
			CGlobalDummyItem * pGlobalDummyItem=m_GlobalInfoManager.SearchDummyItem(pGlobalServerItem->GetServerID());
			if (pGlobalDummyItem)
			{
				ServerItem["AddCounts"] = (int)pGlobalDummyItem->m_DummyInfo.dwDummyCount;
			}
			else
			{
				ServerItem["AddCounts"] = 0;
			}

			root.append(ServerItem);
		}

	} while (PositionServer!=NULL);


	std::string data= writer.write(root);

	return m_pHttpServerService->SendData(req, (void*)data.data(), data.size(), "json");
}

bool CAttemperEngineSink::OnHttpChangeDummyOnlineUser(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO, SUB_CS_S_PLAZA_CHANGE_DUMMY, NULL, NULL, 0L);
	//m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO, SUB_CS_S_PLAZA_MODIFY_DUMMY, &ModifyDummy, sizeof(ModifyDummy), 0L);
	return m_pHttpServerService->SendData(req, "OK", sizeof("OK"));
}

//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//发送信息
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//获取对象
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//设置数据
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	//发送完成
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//发送列表
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//获取对象
		tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//设置数据
		if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
		{
			wPacketSize+=sizeof(tagGameMatch);
			CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//房间发送
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//查找房间
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//获取参数
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//发送数据
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//用户发送
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
