#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////
//��ʱ������

#define IDI_LOAD_PLATFORM_PARAMETER		1							//���ز���
#define TIME_LOAD_PLATFORM_PARAMETER	600000						//ʱ����

#define IDI_HTTP_EVENT_LOOP		2									//HTTP�¼��ַ�
#define TIME_HTTP_EVENT_LOOP	1						//ʱ����

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

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_wCollectItem=INVALID_WORD;

	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pHttpServerService = NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
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
	//�󶨲���
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//���ö�ʱ��
#ifndef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,TIME_LOAD_PLATFORM_PARAMETER,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_PLATFORM_PARAMETER,60000,TIMES_INFINITY,0);
#endif

	m_pHttpServerService = new CHttpKernel<CAttemperEngineSink>();
	
	if (m_pHttpServerService->InitServer(PORT_CORRESPOND_HTTP) == false)
	{
		CTraceService::TraceString(_T("HTTP����ʧ�ܣ�����˿�ʱ���ѱ�ռ��"),TraceLevel_Warning);
	}
	else
	{
		m_pHttpServerService->SetReqPostCallBack(&CAttemperEngineSink::OnServerPostCallBack, this, NULL);
		m_pHttpServerService->SetReqGetCallBack(&CAttemperEngineSink::OnServerGetCallBack, this, NULL);
		m_pITimerEngine->SetTimer(IDI_HTTP_EVENT_LOOP,TIME_HTTP_EVENT_LOOP,TIMES_INFINITY,0);
	}

	AllocConsole();
	freopen("CONIN$", "r+t", stdin); // �ض��� STDIN
	freopen("CONOUT$", "w+t", stdout); // �ض���STDOUT 

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	if (m_pHttpServerService)
		SafeDelete(m_pHttpServerService);

	//�������
	m_GlobalInfoManager.ResetData();

	FreeConsole();

	return true;
}

void CAttemperEngineSink::OnServerPostCallBack(struct evhttp_request *req, void * pArg)
{
	int	wMainCmdID = -1;							//��������

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
			//����������վ��post������
			Json::Reader reader;
			Json::Value root;

			bool ret = reader.parse(out, root);
			if (ret)  // json����
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
			{//�ݲ�֧�ֵ����ݽ�����ʽ
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

	//	//������ȡHttp���� Head
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
	//	/* ������ȡGet URI��ֵ�� */
	//	struct evkeyvalq params;
	//	evhttp_parse_query_str(szUri, &params);

	//	std::string strKeyValue;

	//	std::map<std::string, std::string>	keyValueSet;
	//	//·��ƥ��������ʽ
	//	std::regex pathPattern("(?:/(.*)\\?)*(.*)", std::regex::icase);
	//	//std::regex keyValuePattern("([a-z0-9]+)=([a-z0-9]+)", std::regex::icase);
	//	std::cmatch cm;
	//	//������ȡHttp���� Head
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

	//	//�ļ����ڣ�ֱ�ӷ����ļ����ݣ����򷵻�404
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

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
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
	//���ز���
	if(IDI_LOAD_PLATFORM_PARAMETER==dwTimerID)
	{
		//����֪ͨ
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

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	bool ret = true;
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:		//����ע��
		{
			ret = OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_SERVICE_INFO:	//������Ϣ
		{
			ret = OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_USER_COLLECT:	//�û�����
		{
			ret = OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_REMOTE_SERVICE:	//Զ�̷���
		{
			ret = OnTCPNetworkMainRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_MANAGER_SERVICE: //�������
		{
			ret = OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			break;
		}
	case MDM_CS_ANDROID_SERVICE: //��������
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

	m_pHttpServerService->SendData(req, "�Ƿ�����", sizeof("Error"), "txt", HTTP_INTERNAL);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��Ϸ����
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);

		//���ܴ���
		if (wBindIndex==m_wCollectItem)
		{
			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//��ȡ����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//ɾ���ȴ�
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		//��������
		CMD_CS_S_ServerRemove ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//ɾ��֪ͨ
		ServerRemove.wServerID=pBindParameter->wServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_REMOVE,&ServerRemove,sizeof(ServerRemove),0L);

		//ע������
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);
	}

	//�㳡����
	if (pBindParameter->ServiceKind==ServiceKind_Plaza)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//ע������
		m_GlobalInfoManager.DeletePlazaItem(pBindParameter->wServiceID);
	}

	//�������
	if (pBindParameter->ServiceKind==ServiceKind_Chat)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//ע������
		m_GlobalInfoManager.DeleteChatItem(pBindParameter->wServiceID);
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_PLAZA:	//ע��㳡
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterPlaza));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterPlaza)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterPlaza * pRegisterPlaza=(CMD_CS_C_RegisterPlaza *)pData;

			//��Ч�ж�
			if ((pRegisterPlaza->szServerName[0]==0)||(pRegisterPlaza->szServerAddr[0]==0))
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("������ע��ʧ�ܣ��������ַ���롰�������������Ϸ���"),CountArray(RegisterFailure.szDescribeString));

				//������Ϣ
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Plaza;

			//��������
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//��������
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerName,pRegisterPlaza->szServerName,CountArray(GamePlaza.szServerName));
			lstrcpyn(GamePlaza.szServerAddr,pRegisterPlaza->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//ע�᷿��
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//�����б�
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_SERVER:	//ע�᷿��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//���ҷ���
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("�Ѿ�������ͬ��ʶ����Ϸ������񣬷������ע��ʧ��"),CountArray(RegisterFailure.szDescribeString));

				//������Ϣ
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//��������
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//��������
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

			//ע�᷿��
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,&GameServer,sizeof(GameServer),0L);

			//�����б�
			SendServerListItem(dwSocketID);

			SendMatchListItem(dwSocketID);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//����֪ͨ
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}	
	case SUB_CS_C_REGISTER_MATCH:	//ע�����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterMatch));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterMatch)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterMatch * pRegisterMatch=(CMD_CS_C_RegisterMatch *)pData;

			//���ҷ���
			CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pRegisterMatch->wServerID);
			if(pGlobalServerItem==NULL) return true;

			//��������
			tagGameMatch GameMatch;
			ZeroMemory(&GameMatch,sizeof(GameMatch));

			//��������
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

			//��������
			CopyMemory(&pGlobalServerItem->m_GameMatch,&GameMatch,sizeof(pGlobalServerItem->m_GameMatch));

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,&GameMatch,sizeof(GameMatch),0L);

			return true;
		}
	case SUB_CS_C_REGISTER_CHAT:	//ע������
		{		
			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Chat;

			tagChatServer ChatServer ;
			ChatServer.wChatID = wBindIndex;
			ChatServer.dwClientAddr = (m_pBindParameter+wBindIndex)->dwClientAddr;
			ChatServer.dwSocketID = dwSocketID;

			m_GlobalInfoManager.ActiveChatItem(wBindIndex,ChatServer);

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT,&ChatServer,sizeof(ChatServer),0L);

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CHAT_INSERT);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case SUB_CS_C_REGISTER_PERSONAL:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_S_RegisterPersonal));
			if (wDataSize!=sizeof(CMD_CS_S_RegisterPersonal)) return false;
			CMD_CS_S_RegisterPersonal * RegisterPersonal = (CMD_CS_S_RegisterPersonal *)pData;
			m_GlobalInfoManager.AddPersonalMaxCreate(* RegisterPersonal);
			return true;
		}
	}

	return false;
}

//����״̬
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVER_ONLINE:	//��������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_C_ServerOnLine)) return false;

			//��Ϣ����
			CMD_CS_C_ServerOnLine * pServerOnLine=(CMD_CS_C_ServerOnLine *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//���ҷ���
			WORD wServerID=pBindParameter->wServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//��������
			if (pGlobalServerItem!=NULL)
			{
				//��������
				CMD_CS_S_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//���ñ���
				pGlobalServerItem->m_GameServer.dwAndroidCount=pServerOnLine->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;

				//���ñ���
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;
				ServerOnLine.dwAndroidCount=pServerOnLine->dwAndroidCount;

				//����֪ͨ
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
	case SUB_CS_C_SERVER_MODIFY:	//�����޸�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//��Ϣ����
			CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//���ҷ���
			ASSERT(m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID)!=NULL);
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//�����޸�
			if (pGlobalServerItem!=NULL)
			{
				//���ñ���
				pGlobalServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGlobalServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGlobalServerItem->m_GameServer.dwAndroidCount = pServerModify->dwAndroidCount;
				pGlobalServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;				
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGlobalServerItem->m_GameServer.szServerName));
				lstrcpyn(pGlobalServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGlobalServerItem->m_GameServer.szServerAddr));

				//��������
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify,sizeof(ServerModify));

				//���ñ���
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

				//����֪ͨ
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
			}

			return true;
		}
	case SUB_CS_C_SEARCH_TABLE:			//��������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//��������
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->szPersonalTableID);

			//��������
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
	case SUB_CS_C_DISSUME_SEARCH_TABLE:			//��������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchTable));
			if (wDataSize!=sizeof(CMD_CS_C_SearchTable)) return false;

			CMD_CS_C_SearchTable *pSearchTable = (CMD_CS_C_SearchTable*)pData;

			//��������
			tagPersonalTableInfo* pPersonalTable = m_GlobalInfoManager.SearchTableByID(pSearchTable->szPersonalTableID);

			//��������
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
	case SUB_CS_C_QUERY_GAME_SERVER:		//��ѯ���÷���
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_QueryGameServer));
			if (wDataSize!=sizeof(CMD_CS_C_QueryGameServer)) return false;

			CMD_CS_C_QueryGameServer *pQueryGameServer = (CMD_CS_C_QueryGameServer *)pData;

			//�ж��Ƿ���Դ���˽�˷���
			DWORD dwServerID = 0;
			bool bCanCreateRoom = false;

			CMD_CS_S_QueryGameServerResult QueryGameServerResult;
			ZeroMemory(&QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			if (m_GlobalInfoManager.CanCreatePersonalRoom(pQueryGameServer->dwKindID, pQueryGameServer->dwUserID))
			{
				//��������
				dwServerID = m_GlobalInfoManager.GetFreeServer(pQueryGameServer->dwUserID, pQueryGameServer->dwKindID, pQueryGameServer->cbIsJoinGame);
				bCanCreateRoom = true;

				if (dwServerID == 0)
				{
					lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("�������������Ϸģʽ��ֻ�ܴ���һ�����䣬����Ϸ����������˽�˷���������"),  sizeof(QueryGameServerResult.szErrDescrybe));
				}
			}
			else
			{
				lstrcpyn(QueryGameServerResult.szErrDescrybe, TEXT("�Ѿ��ﵽ����˽�˷����������������ٴ������䣡"),  sizeof(QueryGameServerResult.szErrDescrybe));
			}

			//��������
			QueryGameServerResult.dwSocketID = pQueryGameServer->dwSocketID;
			QueryGameServerResult.dwClientAddr = pQueryGameServer->dwClientAddr;
			QueryGameServerResult.dwServerID = dwServerID;
			QueryGameServerResult.bCanCreateRoom= bCanCreateRoom;


			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_SERVICE_INFO, SUB_CS_S_QUERY_GAME_SERVER_RESULT, &QueryGameServerResult, sizeof(CMD_CS_S_QueryGameServerResult));

			return true;
		}
	case SUB_CS_C_CREATE_TABLE:		//��������
		{
			//У������
			ASSERT(wDataSize == sizeof(CMD_CS_C_CreateTable));
			if(wDataSize != sizeof(CMD_CS_C_CreateTable)) return false;

			CMD_CS_C_CreateTable* pCreateTable = (CMD_CS_C_CreateTable*)pData;

			//����ID
			TCHAR szServerID[7] = TEXT("");
			m_GlobalInfoManager.RandServerID(szServerID, 6);

			lstrcpyn(pCreateTable->PersonalTable.szRoomID, szServerID, CountArray(pCreateTable->PersonalTable.szRoomID));
			//��������
			m_GlobalInfoManager.AddServerTable(szServerID, pCreateTable->PersonalTable);
			m_GlobalInfoManager.RemoveFreeServerTable(pCreateTable->PersonalTable.dwServerID);

			//��������
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
	case SUB_CS_C_DISMISS_TABLE:		//��ɢ����
		{
			//У������
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

			//��������
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
			//У������
			ASSERT((wDataSize % sizeof(CMD_CS_C_DummyOnLine)) == 0);
			if((wDataSize % sizeof(CMD_CS_C_DummyOnLine)) != 0) return false;

			CMD_CS_C_DummyOnLine* pDumOnline = (CMD_CS_C_DummyOnLine*)pData;

			//��������
			WORD wItemCount=wDataSize/sizeof(CMD_CS_C_DummyOnLine);

			//��������
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
	case SUB_CS_S_QUERY_PERSONAL_ROOM_LIST:		//���󷿼��б�
		{
			//У������
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

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//�û�����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//��Ϣ����
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			GlobalUserInfo.cbAndroidUser = pUserEnter->cbAndroidUser;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//������Ϣ
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;
			
			//������ϸ��Ϣ
			memcpy(&GlobalUserInfo.userInfo, &pUserEnter->userInfo, sizeof(tagUserInfo));

			//�����û�
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->wServiceID);


			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserEnter->dwUserID;
			UserGameStatus.cbGameStatus=pUserEnter->userInfo.cbUserStatus;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));
			

			//��������
			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//�û��뿪
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//��Ϣ����
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//ɾ���û�
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserLeave->dwUserID;
			UserGameStatus.cbGameStatus=US_NULL;
			UserGameStatus.wKindID   = pBindParameter->ServiceKind;
			UserGameStatus.wServerID = pBindParameter->wServiceID;
			UserGameStatus.wTableID  = INVALID_TABLE;
			UserGameStatus.wChairID  = INVALID_CHAIR;

			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	case SUB_CS_C_CHAT_USER_ENTER:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_ChatUserEnter));
			if (wDataSize!=sizeof(CMD_CS_C_ChatUserEnter)) return false;

			//��Ϣ����
			CMD_CS_C_ChatUserEnter * pUserEnter=(CMD_CS_C_ChatUserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Chat);
			if (pBindParameter->ServiceKind!=ServiceKind_Chat) return false;


			//��������
			tagGlobalChatUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//�����û�
			m_GlobalInfoManager.ActiveChatUserItem(GlobalUserInfo,pBindParameter->wServiceID);

			return true;
		}
	case SUB_CS_C_CHAT_USER_LEAVE:
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_ChatUserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_ChatUserLeave)) return false;

			//��Ϣ����
			CMD_CS_C_ChatUserLeave * pUserLeave=(CMD_CS_C_ChatUserLeave *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Chat);
			if (pBindParameter->ServiceKind!=ServiceKind_Chat) return false;



			//ɾ���û�
			m_GlobalInfoManager.DeleteChatUserItem(pUserLeave->dwUserID,pBindParameter->wServiceID);


			return true;
		}
	case SUB_CS_C_USER_FINISH:		//�û����
		{
			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//�л�����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_STATUS:		//�û�״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserStatus));
			if (wDataSize!=sizeof(CMD_CS_C_UserStatus)) return false;

			//��Ϣ����
			CMD_CS_C_UserStatus * pUserStatus=(CMD_CS_C_UserStatus *)pData;

			CGlobalUserItem* pGlobalUserItem = m_GlobalInfoManager.SearchUserItemByUserID(pUserStatus->dwUserID);
			if(pGlobalUserItem != NULL)
			{
				pGlobalUserItem->UpdateStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);

			}

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);

			//ͬ��״̬
			CMD_CS_S_UserGameStatus UserGameStatus;
			UserGameStatus.dwUserID=pUserStatus->dwUserID;
			UserGameStatus.cbGameStatus=pUserStatus->cbUserStatus;
			UserGameStatus.wKindID =pUserStatus->wKindID;
			UserGameStatus.wServerID=pUserStatus->wServerID;
			UserGameStatus.wTableID=pUserStatus->wTableID;
			lstrcpyn(UserGameStatus.szServerName,pGlobalServerItem->m_GameServer.szServerName,CountArray(UserGameStatus.szServerName));

			//��������
			POSITION posChat=NULL;

			//���ҷ���
			do
			{
				//���ҷ���
				CGlobalChatServerItem * pGlobalChatItem=m_GlobalInfoManager.EnumChatItem(posChat);

				//��ֹ�ж�
				if (pGlobalChatItem==NULL) break;

				//����״̬
				m_pITCPNetworkEngine->SendData(pGlobalChatItem->m_ChatServer.dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_USER_GAMESTATE,&UserGameStatus,sizeof(UserGameStatus));

			}while(posChat != NULL);

			return true;
		}
	}

	return false;
}

//Զ�̷���
bool CAttemperEngineSink::OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SEARCH_CORRESPOND:	//Э������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_CS_C_SearchCorrespond)) return false;

			//������Ϣ
			CMD_CS_C_SearchCorrespond * pSearchCorrespond=(CMD_CS_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//��������
			CMD_CS_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//����û�
			CGlobalUserItem * ResultUserItem[2];
			ZeroMemory(ResultUserItem,sizeof(ResultUserItem));

			//���ñ���
			SearchCorrespond.dwSocketID=pSearchCorrespond->dwSocketID;
			SearchCorrespond.dwClientAddr=pSearchCorrespond->dwClientAddr;

			//�����û�
			if (pSearchCorrespond->dwGameID!=0L)
			{
				ResultUserItem[0]=m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID);
			}

			//�����û�
			if (pSearchCorrespond->szNickName[0]!=0)
			{
				_tcslwr(pSearchCorrespond->szNickName);
				ResultUserItem[1]=m_GlobalInfoManager.SearchUserItemByNickName(pSearchCorrespond->szNickName);
			}

			//���ý��
			for (BYTE i=0;i<CountArray(ResultUserItem);i++)
			{
				if (ResultUserItem[i]!=NULL)
				{
					//��������
					WORD wServerIndex=0;

					//���ҷ���
					do
					{
						//���ҷ���
						CGlobalServerItem * pGlobalServerItem=ResultUserItem[i]->EnumServerItem(wServerIndex++);

						//��ֹ�ж�
						if (pGlobalServerItem==NULL) break;
						if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

						//��������
						WORD wIndex=SearchCorrespond.wUserCount++;

						//������Ϣ
						SearchCorrespond.UserRemoteInfo[wIndex].cbGender=ResultUserItem[i]->GetGender();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMemberOrder=ResultUserItem[i]->GetMemberOrder();
						SearchCorrespond.UserRemoteInfo[wIndex].cbMasterOrder=ResultUserItem[i]->GetMasterOrder();

						//�û���Ϣ
						SearchCorrespond.UserRemoteInfo[wIndex].dwUserID=ResultUserItem[i]->GetUserID();
						SearchCorrespond.UserRemoteInfo[wIndex].dwGameID=ResultUserItem[i]->GetGameID();
						lstrcpyn(SearchCorrespond.UserRemoteInfo[wIndex].szNickName,ResultUserItem[i]->GetNickName(),LEN_NICKNAME);

						//������Ϣ
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

			//��������
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REMOTE_SERVICE,SUB_CS_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_C_SEARCH_ALLCORRESPOND:	//Э������
		{
			CMD_CS_C_AllSearchCorrespond * pSearchCorrespond=(CMD_CS_C_AllSearchCorrespond *)pData;
			if(pSearchCorrespond->dwCount > 512) return false;

			BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			CMD_CS_S_SearchAllCorrespond* pAllSearchCorrespond = (CMD_CS_S_SearchAllCorrespond*)cbDataBuffer;
			//���ñ���
			pAllSearchCorrespond->dwSocketID=pSearchCorrespond->dwSocketID;
			pAllSearchCorrespond->dwClientAddr=pSearchCorrespond->dwClientAddr;

			int nCount = 0;
			const int cbPacketHeadSize = sizeof(CMD_CS_S_SearchAllCorrespond) - sizeof(tagUserRemoteInfo);
			int cbPacketSize = cbPacketHeadSize;
			//�����û�
			int nSearchCount = (int)pSearchCorrespond->dwCount;
			for(int i = 0; i < nSearchCount; i++)
			{
				CGlobalUserItem* pGlobalUserItem =  m_GlobalInfoManager.SearchUserItemByGameID(pSearchCorrespond->dwGameID[i]);
				if(pGlobalUserItem != NULL)
				{
					tagUserInfo* pUserInfo = pGlobalUserItem->GetUserInfo();
					//���ҷ���
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

//�������
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SYSTEM_MESSAGE:	//ϵͳ��Ϣ
		{
			//����֪ͨ
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_SYSTEM_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_HALL_MESSAGE:		//������Ϣ
		{
			//����֪ͨ
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_HALL_MESSAGE,pData,wDataSize,0L);
			return true;
		}
	case SUB_CS_C_PROPERTY_TRUMPET:  //������Ϣ
		{
			//����֪ͨ
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE,SUB_CS_S_PROPERTY_TRUMPET,pData,wDataSize,0L);

			return true;
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_ADDPARAMETER:		//��Ӳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_C_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_C_AddParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_AddParameter * pAddParameter = (CMD_CS_C_AddParameter *)pData;

			//����ṹ
			CMD_CS_S_AddParameter AddParameter;
			CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//������Ϣ
			SendDataToGame(pAddParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_ADDPARAMETER,&AddParameter,sizeof(AddParameter));

			return true;
		}
	case SUB_CS_C_MODIFYPARAMETER:  //�޸Ĳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_C_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_C_ModifyParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_ModifyParameter * pModifyParameter = (CMD_CS_C_ModifyParameter *)pData;

			//����ṹ
			CMD_CS_S_ModifyParameter ModifyParameter;
			CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//����֪ͨ
			SendDataToGame(pModifyParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));

			return true;
		}
	case SUB_CS_C_DELETEPARAMETER:  //ɾ������
		{
			//����У��
			ASSERT(sizeof(CMD_CS_C_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_C_DeleteParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_C_DeleteParameter * pDeleteParameter = (CMD_CS_C_DeleteParameter *)pData;

			//����ṹ
			CMD_CS_S_DeleteParameter DeleteParameter;
			DeleteParameter.dwBatchID=pDeleteParameter->dwBatchID;

			//����֪ͨ
			SendDataToGame(pDeleteParameter->wServerID,MDM_CS_ANDROID_SERVICE,SUB_CS_S_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));

			return true;
		}
	}

	return false;
}

//��ѯ�����û�
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

	//���� �����û�
	std::map<int, bool> chatUserList;
	POSITION Position=NULL;
	do
	{
		CGlobalChatUserItem * pGlobalServerItem=m_GlobalInfoManager.EnumChatUserItem(Position);

		//��������
		if (pGlobalServerItem!=NULL )
		{
			chatUserList[pGlobalServerItem->GetUserID()] = true;
		}

	} while (Position!=NULL);


	//������Ϸ��������
	POSITION PositionServer =NULL;
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(PositionServer);
		if (pGlobalServerItem!=NULL )
		{
			//������Ϸ�û�
			POSITION Position = NULL;
			do
			{
				CGlobalUserItem * pGlobalUserItem=NULL;

				DWORD dwUserID=0L;
				//��ȡ����
				if (Position==NULL)
					Position=pGlobalServerItem->m_MapUserID.GetStartPosition();

				if (Position!=NULL)
					pGlobalServerItem->m_MapUserID.GetNextAssoc(Position,dwUserID,pGlobalUserItem);

				//��������
				if (pGlobalUserItem!=NULL )
				{
					if (!pGlobalUserItem->IsAndroid())
					{//���� �������û�
						//CGlobalUserItem * pUserItem = m_GlobalInfoManager.SearchChatUserItemByUserID(dwUserID);
						std::map<int, bool>::iterator itChatUser = chatUserList.find(dwUserID);
						//���û�Ҳͬʱ����������� Ϊ����վ������Ҫ����
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

//��վ����
bool CAttemperEngineSink::OnHttpClearUser(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//����������վ��post������
	Json::Reader reader;
	Json::Value root;

	if (reader.parse((const char *)pData, root))  // json����
	{ 
		int msgid =  root["msgid"].asInt();
		const Json::Value content = root["content"];
		Json::Value::Members mem = content.getMemberNames();


		CMD_CS_S_ClearUser ClearUser;
		ZeroMemory(&ClearUser,sizeof(ClearUser));
		//����json ��������
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
			//		//�����������
			//		const Json::Value arrayNum = content[*iter];
			//		for (unsigned int i = 0; i < arrayNum.size(); i++)
			//		{
			//			arrayNum[i];
			//		}
			//	}
			//	break;
			//case Json::objectValue:
			//	{
			//		//����Json����
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
			//���ҷ���
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(ClearUser.dwServerID);
			if (pGlobalServerItem==NULL) return true;

			//��ȡ����
			WORD wBindIndex=pGlobalServerItem->GetIndex();
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//��������
			DWORD dwSocketID=pBindParameter->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CLEARUER,&ClearUser,sizeof(ClearUser));
		}
		else
		{
			//��������
			WORD wPacketSize=0L;
			POSITION Position=NULL;

			//�ռ�����
			do
			{
				CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

				//��������
				if (pGlobalServerItem!=NULL )
				{

					//��ȡ����
					WORD wBindIndex=pGlobalServerItem->GetIndex();
					tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

					//��������
					DWORD dwSocketID=pBindParameter->dwSocketID;
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_CLEARUER,&ClearUser,sizeof(ClearUser));
				}

			} while (Position!=NULL);
		}

	}

	m_pHttpServerService->SendData(req, "OK", sizeof("OK"));

	return true;
}

//�ߵ����������û���ǿ���û����ߣ�
bool CAttemperEngineSink::OnHttpForceOffline(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//����������վ��post������
	Json::Reader reader;
	Json::Value root;

	if (reader.parse((const char *)pData, root))  // json����
	{ 
		int msgid =  root["msgid"].asInt();
		const Json::Value content = root["content"];
		Json::Value::Members mem = content.getMemberNames();


		WORD wPacketSize=0L;
		BYTE cbBuffer[SOCKET_TCP_PACKET];

		//����json ��������
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)        
		{
			const char * pkey = (*iter).c_str();

			if(strcmp(pkey,"userid")==0)
			{
				std::string str_userid_list = content[pkey].asString();
				std::vector<std::string> userid_list = split(str_userid_list, ",");

				for (std::vector<std::string>::const_iterator it = userid_list.begin(); it != userid_list.end(); ++it)
				{	
					//��������
					if ((wPacketSize+sizeof(CMD_CS_S_ForceOffline))>sizeof(cbBuffer))
					{
						m_pITCPNetworkEngine->SendDataBatch(MDM_CS_USER_COLLECT, SUB_CS_S_CHAT_FORCE_OFFLINE, cbBuffer, wPacketSize, 0L);
						wPacketSize=0;
					}

					CMD_CS_S_ForceOffline * pOffLine=(CMD_CS_S_ForceOffline *)(cbBuffer+wPacketSize);

					//��������
					wPacketSize+=sizeof(CMD_CS_S_ForceOffline);
					pOffLine->dwUserID = atoi(it->c_str());


					//�����������������ɾ�����û�
					m_GlobalInfoManager.DeleteChatUserItem(pOffLine->dwUserID,0);
				}
				//��������
				if (wPacketSize>0) m_pITCPNetworkEngine->SendDataBatch(MDM_CS_USER_COLLECT, SUB_CS_S_CHAT_FORCE_OFFLINE, cbBuffer, wPacketSize, 0L);
			}
		}

	}

	return m_pHttpServerService->SendData(req, "OK", sizeof("OK"));
}

//��ѯ��������
bool CAttemperEngineSink::OnHttpQueryDummyOnlineUser(DWORD dwMsgID,VOID * pData,WORD wDataSize, struct evhttp_request *req)
{
	//����������վ��post������

	Json::Value root;
	Json::FastWriter writer;

	//������Ϸ��������
	POSITION PositionServer =NULL;
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(PositionServer);
		if (pGlobalServerItem!=NULL )
		{
			//��Ϸ��������

			Json::Value ServerItem;
			ServerItem["KindID"] = (int)pGlobalServerItem->GetKindID();
			ServerItem["ServerID"] = (int)pGlobalServerItem->GetServerID();
			ServerItem["UserCount"] = (int)pGlobalServerItem->GetRealUserItemCount();
			ServerItem["AndroidCount"] = (int)pGlobalServerItem->GetAndroidUserItemCount();

			//�����û�
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

//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INFO);

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//��������
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize);

	//�������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_FINISH);

	return true;
}
//�����б�
bool CAttemperEngineSink::SendMatchListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		tagGameMatch * pGameMatch=(tagGameMatch *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);

		//��������
		if (pGlobalServerItem!=NULL && pGlobalServerItem->IsMatchServer())
		{
			wPacketSize+=sizeof(tagGameMatch);
			CopyMemory(pGameMatch,&pGlobalServerItem->m_GameMatch,sizeof(tagGameMatch));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVICE_INFO,SUB_CS_S_MATCH_INSERT,cbBuffer,wPacketSize);

	return true;
}

//���䷢��
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���ҷ���
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//��ȡ����
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��������
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//�û�����
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
