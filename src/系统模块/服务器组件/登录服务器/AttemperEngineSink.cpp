#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_LOAD_GAME_LIST			1									//�����б�
#define IDI_CONNECT_CORRESPOND		2									//������ʶ
#define IDI_COLLECT_ONLINE_INFO		3									//ͳ������

//ʱ�䶨��
#define	TIME_VALID_VERIFY_CODE		1 * 60								//ע��������֤��ʱ��������λ �룩

//ͳ����������ʱ����
#define	TIME_COLLECT_ONLINE_INFO	20									//������

#define IDI_HTTP_EVENT_LOOP		4									//HTTP�¼��ַ�
#define TIME_HTTP_EVENT_LOOP	1									//ʱ����



#define VERIFY_CODE_TYPE_REGISTER	1								//ע�����
#define VERIFY_CODE_TYPE_MODIFYPASS	2								//�޸ĵ�¼�������
#define VERIFY_CODE_TYPE_MODIFYBANKINFO 3							//�޸�������Ϣ����

time_t SystemTimeToTimet(SYSTEMTIME st)
{

	FILETIME ft;

	SystemTimeToFileTime( &st, &ft );
	LONGLONG nLL;

	ULARGE_INTEGER ui;

	ui.LowPart = ft.dwLowDateTime;

	ui.HighPart = ft.dwHighDateTime;

	nLL = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;

	time_t pt = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);

	pt -= 8 * 60 * 60;

	return pt;

}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bNeekCorrespond=true;
	m_bShowServerStatus=false;

	//״̬����
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pHttpClientService = NULL;
	m_pIPersonalRoomServiceManager = NULL;

	//��Ƶ����
	m_wAVServerPort=0;
	m_dwAVServerAddr=0;

	//�������
	m_wTaskCount=0;
	m_wLevelCount=0;
	m_wMemberCount=0;	
	m_wTaskCountBuffer=0;

	ZeroMemory(&m_PlatformParameter,sizeof(&m_PlatformParameter));
	ZeroMemory(&m_BaseEnsureParameter,sizeof(&m_BaseEnsureParameter));	
	ZeroMemory(&m_AuthRealParameter,sizeof(&m_AuthRealParameter));
	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));	
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));
	ZeroMemory(m_TaskParameterBuffer,sizeof(m_TaskParameterBuffer));	
	ZeroMemory(m_szHttpRegisterValidCode, sizeof(m_szHttpRegisterValidCode));
	ZeroMemory(m_szHttpModifyPassValidCode, sizeof(m_szHttpModifyPassValidCode));
//��������
	m_VerifyCodeArray.RemoveAll();

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//������Դ
	for (INT i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pValidateCode=m_VerifyCodeArray[i];
		if (pValidateCode!=NULL)
		{
			SafeDelete(pValidateCode);
		}
		m_VerifyCodeArray.RemoveAt(i);
	}
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

	//����ʱ��
	ASSERT(m_pITimerEngine!=NULL);
#ifndef _DEBUG
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,TIME_COLLECT_ONLINE_INFO*1000L,TIMES_INFINITY,0);
#else
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,TIME_COLLECT_ONLINE_INFO*1000L,TIMES_INFINITY,0);
#endif

	m_pHttpClientService = new CHttpKernel<CAttemperEngineSink>();

	if (m_pHttpClientService->InitClient() == false)
	{
		CTraceService::TraceString(_T("HTTP�ͻ�������ʧ��"),TraceLevel_Warning);
	}
	else
	{
		m_pITimerEngine->SetTimer(IDI_HTTP_EVENT_LOOP,TIME_HTTP_EVENT_LOOP,TIMES_INFINITY,0);
	}

	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	CString szFileName;
	GetModuleFileName(AfxGetInstanceHandle(),szPath,sizeof(szPath));
	szFileName=szPath;
	int nIndex = szFileName.ReverseFind(TEXT('\\'));
	szFileName = szFileName.Left(nIndex);
	szFileName += TEXT("\\ServerConfig.ini");

	if((_waccess(szFileName.GetString(), 0)) == -1)
	{
		CTraceService::TraceString(_T("��ȡServerConfig.iniʧ�ܣ�"),TraceLevel_Warning);
		CTraceService::TraceString(_T("OtherConfig:HttpValidCode ����ע��ӿ�����ʧ�ܣ�"),TraceLevel_Warning);
	}
	else
	{
		TCHAR szValueBuffer[MAX_PATH] = TEXT("");
		m_bShowServerStatus = (GetPrivateProfileInt(TEXT("ServerStatus"),TEXT("ShowServerStatus"),0,szFileName) != 0);

		
		CString str;
		//��ȡע����֤����ŵ�ַ
		GetPrivateProfileString(TEXT("OtherConfig"), TEXT("HttpRegisterValidCode"), TEXT(""), szValueBuffer, MAX_PATH, szFileName);
		str = szValueBuffer;
		int nLength = str.GetLength();
		if (nLength == 0)
		{
			CTraceService::TraceString(_T("OtherConfig:HttpRegisterValidCode ����ע��ӿ�����ʧ�ܣ�,ע����Ž��޷�����"),TraceLevel_Warning);
		}
		else
		{
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, m_szHttpRegisterValidCode, nBytes, NULL, NULL); 
		}


		//��ȡ�޸�������֤���ַ
		GetPrivateProfileString(TEXT("OtherConfig"), TEXT("HttpModifyPassValidCode"), TEXT(""), szValueBuffer, MAX_PATH, szFileName);
		str = szValueBuffer;
		nLength = str.GetLength();
		if (nLength == 0)
		{
			CTraceService::TraceString(_T("OtherConfig:HttpModifyPassValidCode ����ע��ӿ�����ʧ�ܣ�,�޸�������Ž��޷�����"),TraceLevel_Warning);
		}
		else
		{
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, m_szHttpModifyPassValidCode, nBytes, NULL, NULL); 
		}

		//��ȡ�޸�������֤���ַ
		GetPrivateProfileString(TEXT("OtherConfig"), TEXT("HttpModifyBankInfoValidCode"), TEXT(""), szValueBuffer, MAX_PATH, szFileName);
		str = szValueBuffer;
		nLength = str.GetLength();
		if (nLength == 0)
		{
			CTraceService::TraceString(_T("OtherConfig:HttpModifyBankInfoValidCode ����ע��ӿ�����ʧ�ܣ�,�޸�������Ϣ���Ž��޷�����"),TraceLevel_Warning);
		}
		else
		{
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, m_szHttpModifyBankInfoValidCode, nBytes, NULL, NULL); 
		}
	}

	//��ȡĿ¼
	TCHAR szServerAddr[LEN_SERVER]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//��ȡ����
	TCHAR szVideoFileName[MAX_PATH]=TEXT("");
	_sntprintf(szVideoFileName,CountArray(szVideoFileName),TEXT("%s\\VideoOption.ini"),szPath);
	m_wAVServerPort=GetPrivateProfileInt(TEXT("VideoOption"),TEXT("ServerPort"), 0,szVideoFileName);
	DWORD dwAddrLen=GetPrivateProfileString(TEXT("VideoOption"),TEXT("ServerAddr"), TEXT(""), szServerAddr,LEN_SERVER,szVideoFileName);
	if(dwAddrLen>0)
	{
		CT2CA strServerDomain(szServerAddr);
		m_dwAVServerAddr=inet_addr(strServerDomain);
	}
	else
	{
		m_dwAVServerAddr=0;
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
	m_bNeekCorrespond=true;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketService=NULL;

	//�������
	m_wTaskCount=0;
	m_wTaskCountBuffer=0;
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));
	ZeroMemory(m_TaskParameterBuffer,sizeof(m_TaskParameterBuffer));

	//��Ա����
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//�ȼ�����
	m_wLevelCount=0;	
	ZeroMemory(m_GrowLevelConfig,sizeof(m_GrowLevelConfig));

	//ǩ������
	ZeroMemory(m_lCheckInReward,sizeof(m_lCheckInReward));	

	if (m_pHttpClientService)
		SafeDelete(m_pHttpClientService);

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�б����
	m_ServerListManager.ResetServerList();

	//�������
	m_GamePropertyListManager.ResetPropertyListManager();

	//�ͷŵ��ɵ�����
	for (int i = 0; i < m_ServerDummyOnLineArray.GetCount(); ++i)
	{
		tagServerDummyOnLine * pCursor = m_ServerDummyOnLineArray.GetAt(i);
		free(pCursor);
	}
	m_ServerDummyOnLineArray.RemoveAll();
	m_ServerDummyOnLineMap.RemoveAll();

	FreeConsole();

	return true;
}

////POST����ص�
//void CAttemperEngineSink::OnClientPostCallBack(struct evhttp_request *req, void * pArg)
//{
//	if (req == NULL)
//	{
//		printf("some error occur!");
//		return;
//	}
//
//	struct evbuffer * pBuffer = evhttp_request_get_input_buffer(req);
//	size_t dataLength = evbuffer_get_length(pBuffer);
//	char* out = new char[dataLength];
//	do 
//	{
//		if (evbuffer_remove(pBuffer, out, dataLength) > 0)
//		{
//			//����������վ������
//		}
//		else
//		{
//			evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
//			break;
//		}
//	} while (0);
//
//	delete[]out;
//}
void CAttemperEngineSink::OnClientGetVerifyCodeCallBack(struct evhttp_request *req, void * pArg)
{
	tagVerifyCode * pNewVerifyCode=(tagVerifyCode*)pArg;
	if (req == NULL)
	{
		delete pNewVerifyCode;
		printf("some error occur!");
		return;
	}

	bool bSuccessed = false;

	switch(req->response_code)
	{
	case HTTP_OK:
		{
			struct evbuffer * pBuffer = evhttp_request_get_input_buffer(req);
			size_t dataLength = evbuffer_get_length(pBuffer);
			char* out = new char[dataLength];
			do 
			{
				if (evbuffer_remove(pBuffer, out, dataLength) > 0)
				{
					//����������վ��post������
					Json::Reader reader;
					Json::Value root;

					if (reader.parse(out, root))  // json����
					{ 
						if (!root.isObject())
						{
							evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
							break;
						}

						int code =  root["code"].asInt();
						const Json::Value content = root["data"];
						Json::Value::Members mem = content.getMemberNames();

						if (code == 0)
						{
							//����json ��������
							for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)        
							{
								const char * pkey = (*iter).c_str();
								if(strcmp(pkey,"code")==0)
								{
									//����ʱ��
									pNewVerifyCode->dwUpdateTime=(DWORD)time(NULL);
									//������֤��
									std::string validcode = content[pkey].asString();
									TCHAR szValidCode[MAX_PATH] = TEXT("");
									MultiByteToWideChar(CP_ACP, 0, validcode.c_str(), -1, szValidCode, CountArray(szValidCode));
									lstrcpyn(pNewVerifyCode->szValidateCode,szValidCode,CountArray(pNewVerifyCode->szValidateCode));
									//��ӵ���֤������
									m_VerifyCodeArray.Add(pNewVerifyCode);

									//֪ͨ�ͻ�����֤�����ɳɹ�
									CMD_GP_VerifyCodeResult VerifyCodeResult;
									ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

									VerifyCodeResult.cbResultCode=0;
									lstrcpy(VerifyCodeResult.szDescString,TEXT("���ͳɹ���"));

									//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
									m_pITCPNetworkEngine->SendData(pNewVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));

									bSuccessed = true;
									break;
								}
							}
						}
						else
						{
							CMD_GP_VerifyCodeResult VerifyCodeResult;
							ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

							VerifyCodeResult.cbResultCode=2;
							lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ȡ�쳣�����Ժ�����C301��"));

							//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
							m_pITCPNetworkEngine->SendData(pNewVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
						}
					}
					else
					{
						CMD_GP_VerifyCodeResult VerifyCodeResult;
						ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

						VerifyCodeResult.cbResultCode=2;
						lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ȡ�쳣�����Ժ�����C302��"));

						//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
						m_pITCPNetworkEngine->SendData(pNewVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					}
				}
				else
				{//��ȡ��֤���ڲ�����, ֪ͨ�ͻ���
					CMD_GP_VerifyCodeResult VerifyCodeResult;
					ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

					VerifyCodeResult.cbResultCode=2;
					lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����ȡ��֤���ڲ��������Ժ�����C303��"));

					//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
					m_pITCPNetworkEngine->SendData(pNewVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					break;
				}
			} while (0);

			delete[]out;
		}
		break;
	case HTTP_MOVEPERM:
		break;
	case HTTP_MOVETEMP:
		{
			//HTTP�����ض���
			const char *new_location = evhttp_find_header(req->input_headers, "Location");   
			int i = 0;
			//struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);    
			//evhttp_uri_free(http_req_post->uri);    
			//http_req_post->uri = new_uri;    
			//start_url_request((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);    
		}
		break;
	}

	if (!bSuccessed)
	{
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����ȡ��֤���ڲ��������Ժ�����C304��"));

		//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
		m_pITCPNetworkEngine->SendData(pNewVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));

		delete pNewVerifyCode;
	}
}


//GET����ص�
void CAttemperEngineSink::OnClientGetReBuildVerifyCodeCallBack(struct evhttp_request *req, void * pArg)
{
	if (req == NULL)
	{
		printf("some error occur!");
		return;
	}

	tagVerifyCode * pVerifyCode = (tagVerifyCode *)pArg;

	switch(req->response_code)
	{
	case HTTP_OK:
		{

			struct evbuffer * pBuffer = evhttp_request_get_input_buffer(req);
			size_t dataLength = evbuffer_get_length(pBuffer);
			char* out = new char[dataLength];
			do 
			{
				if (evbuffer_remove(pBuffer, out, dataLength) > 0)
				{
					//����������վ��post������
					Json::Reader reader;
					Json::Value root;

					if (reader.parse(out, root))  // json����
					{ 
						if (!root.isObject())
						{
							evhttp_send_reply(req, HTTP_BADREQUEST, "OK", NULL);
							break;
						}

						int code =  root["code"].asInt();

						/*TCHAR szJson[MAX_PATH] = TEXT("");
						MultiByteToWideChar(CP_ACP, 0, out, -1, szJson, CountArray(szJson));
						ShowLog(TEXT("-------------------------------%s"),szJson);*/

						const Json::Value content = root["data"];
						Json::Value::Members mem = content.getMemberNames();

						if (code == 0)
						{
							//����json ��������
							for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)        
							{
								const char * pkey = (*iter).c_str();
								if(strcmp(pkey,"code")==0)
								{
									std::string validcode = content[pkey].asString();
									TCHAR szValidCode[MAX_PATH] = TEXT("");
									MultiByteToWideChar(CP_ACP, 0, validcode.c_str(), -1, szValidCode, CountArray(szValidCode));
									//����������վ������

									//������֤��ʱ��
									pVerifyCode->dwUpdateTime=(DWORD)time(NULL);
									//������֤��
									lstrcpyn(pVerifyCode->szValidateCode,szValidCode,CountArray(pVerifyCode->szValidateCode));

									//֪ͨ�ͻ�����֤�����ɳɹ�
									CMD_GP_VerifyCodeResult VerifyCodeResult;
									ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

									VerifyCodeResult.cbResultCode=0;
									lstrcpy(VerifyCodeResult.szDescString,TEXT("���ͳɹ���"));

									//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
									m_pITCPNetworkEngine->SendData(pVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
								}
								//else
								//{//��֤���ȡ�쳣 ֪ͨ�ͻ���
								//	CMD_GP_VerifyCodeResult VerifyCodeResult;
								//	ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

								//	VerifyCodeResult.cbResultCode=2;
								//	lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ȡ�쳣�����Ժ����ԣ�"));

								//	//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
								//	m_pITCPNetworkEngine->SendData(pVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
								//}
							}

						}
						else
						{
							CMD_GP_VerifyCodeResult VerifyCodeResult;
							ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

							VerifyCodeResult.cbResultCode=2;
							lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ȡ�쳣�����Ժ����ԣ�"));

							//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
							m_pITCPNetworkEngine->SendData(pVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
						}
					}
					else
					{//��ȡ��֤���ڲ�����, ֪ͨ�ͻ���
						CMD_GP_VerifyCodeResult VerifyCodeResult;
						ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

						VerifyCodeResult.cbResultCode=2;
						lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����ȡ��֤���ڲ��������Ժ����ԣ�"));

						//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
						m_pITCPNetworkEngine->SendData(pVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
						break;
					}
				}
				else
				{//��ȡ��֤���ڲ�����, ֪ͨ�ͻ���
					CMD_GP_VerifyCodeResult VerifyCodeResult;
					ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

					VerifyCodeResult.cbResultCode=2;
					lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����ȡ��֤���ڲ��������Ժ����ԣ�"));

					//֪ͨ�ͻ�����Ϣ ��֤�볬ʱ
					m_pITCPNetworkEngine->SendData(pVerifyCode->dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					break;
				}
			} while (0);

			delete[]out;
		}
		break;
	case HTTP_MOVEPERM:
		break;
	case HTTP_MOVETEMP:
		{
			//HTTP�����ض���
			const char *new_location = evhttp_find_header(req->input_headers, "Location");   
			int i = 0;
			//struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);    
			//evhttp_uri_free(http_req_post->uri);    
			//http_req_post->uri = new_uri;    
			//start_url_request((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);    
		}
		break;
	}
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_LOAD_DB_GAME_LIST:		//�����б�
		{
			//�����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			//ƽ̨����
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_PLATFORM_PARAMETER,0,NULL,0);			

			//����ǩ��
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CHECKIN_REWARD,0,NULL,0);

			//��������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_TASK_LIST,0,NULL,0);

			//���صͱ�
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_BASEENSURE,0,NULL,0);

			//��Ա����
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_MEMBER_PARAMETER,0,NULL,0);		

			//�ɳ�����
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GROWLEVEL_CONFIG,0,NULL,0);	
			
			//��������
			m_GamePropertyListManager.DisusePropertyTypeItem();
			m_GamePropertyListManager.DisusePropertyItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_PROPERTY_LIST,0,NULL,0);

			//������֤
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_REAL_AUTH,0,NULL,0);

			//��ѯ������������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_SERVER_DUMMY_ONLINE,0,NULL,0);

			return true;
		}
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
	switch (dwTimerID)
	{
	case IDI_LOAD_GAME_LIST:		//�����б�
		{
			//�����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			return true;
		}
	case IDI_CONNECT_CORRESPOND:	//����Э��
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
	case IDI_COLLECT_ONLINE_INFO:	//ͳ������
		{
			//��������
			DBR_GP_OnLineCountInfo OnLineCountInfo;
			ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

			//�����������
			OnChangeDummyOnline();

			//��ȡ����
			OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo(false);
			OnLineCountInfo.dwAndroidCountSum=m_ServerListManager.CollectOnlineInfo(true);

			//��ȡ����
			POSITION KindPosition=NULL;
			do
			{
				//��ȡ����
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//���ñ���
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=OnLineCountInfo.wKindCount++;
					OnLineCountInfo.OnLineCountKind[wKindIndex].wKindID=pGameKindItem->m_GameKind.wKindID;

					//Ŀ¼����
					OnLineCountInfo.OnLineCountKind[wKindIndex].dwOnLineCount=pGameKindItem->m_GameKind.dwOnLineCount;
					OnLineCountInfo.OnLineCountKind[wKindIndex].dwAndroidCount=pGameKindItem->m_GameKind.dwAndroidCount;
				}

				//����ж�
				if (OnLineCountInfo.wKindCount>=CountArray(OnLineCountInfo.OnLineCountKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//��������
			WORD wHeadSize=sizeof(OnLineCountInfo)-sizeof(OnLineCountInfo.OnLineCountKind);
			WORD wSendSize=wHeadSize+OnLineCountInfo.wKindCount*sizeof(OnLineCountInfo.OnLineCountKind[0]);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,&OnLineCountInfo,wSendSize);
			return true;
		}
	case IDI_HTTP_EVENT_LOOP:
		{
			if (m_pHttpClientService)
				m_pHttpClientService->EventDispatch();

			
			return true;
		}
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

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//�����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:			//��¼����
		{
			return OnTCPNetworkMainPCLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_SERVER_LIST:	//�б�����
		{
			return OnTCPNetworkMainPCServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_USER_SERVICE:	//��������
		{
			return OnTCPNetworkMainPCUserService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_REMOTE_SERVICE:	//Զ�̷���
		{
			return OnTCPNetworkMainPCRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_ANDROID_SERVICE: //��������
		{
			return OnTCPNetworkMainAndroidService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_PROPERTY:		//���߷���
		{
			return OnTCPNetworkMainPCProperty(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_LOGON:			//��¼����
		{
			return OnTCPNetworkMainMBLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_SERVER_LIST:	//�б�����
		{
			return OnTCPNetworkMainMBServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_PERSONAL_SERVICE:	//˽�˷���
		{
			return OnTCPNetworkMainMBPersonalService(Command.wSubCmdID, pData, wDataSize,dwSocketID);
		}
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBPCLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBPCLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_VALIDATE_MBCARD:		//У���ܱ�
		{
			return OnDBPCLogonValidateMBCard(dwContextID,pData,wDataSize);
		}
	case DBO_GP_VALIDATE_PASSPORT:		//У��֤��
		{
			return OnDBPCLogonValidatePassPort(dwContextID,pData,wDataSize);
		}
	case DBO_GP_VERIFY_RESULT:			//��֤���
		{
			return OnDBPCLogonVerifyResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PLATFORM_PARAMETER:		//ƽ̨����
		{
			return OnDBPCPlatformParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_FACE_INFO:			//�û�ͷ��
		{
			return OnDBPCUserFaceInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INDIVIDUAL:		//�û���Ϣ
		{
			return OnDBPCUserIndividual(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_INFO:		//��������
		{
			return OnDBPCUserInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_SUCCESS:	//���гɹ�
		{
			return OnDBPCUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_FAILURE:	//����ʧ��
		{
			return OnDBPCUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_USER_INFO:  //�û���Ϣ
		{
			return OnDBPCUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_ENABLE_RESULT: //��ͨ���
		{
			return OnDBPCUserInsureEnableResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_TRANSFER_REBATE_RESULT: //�������
		{
			return OnDBPCQueryTransferRebateResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_USERDATA_RESULT:
		{
			return OnDBPCQueryUserDataResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_ANDROID_PARAMETER:		//��������
		{
			return OnDBAndroidParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GP_INDIVIDUAL_RESULT:
		{
			return OnDBIndividualResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_OPERATE_SUCCESS:		//�����ɹ�
		{
			return OnDBPCOperateSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_OPERATE_FAILURE:		//����ʧ��
		{
			return OnDBPCOperateFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_TYPE_ITEM:			//��������
		{
			return OnDBPCGameTypeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_KIND_ITEM:			//��������
		{
			return OnDBPCGameKindItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_NODE_ITEM:			//�ڵ�����
		{
			return OnDBPCGameNodeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PAGE_ITEM:			//��������
		{
			return OnDBPCGamePageItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_LIST_RESULT:		//���ؽ��
		{
			return OnDBPCGameListResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_SERVER_DUMMY_ONLINE_BEGIN:
		{
			return OnDBServerDummyOnLineBegin(dwContextID, pData, wDataSize);
		}
	case DBO_GP_SERVER_DUMMY_ONLINE:		//������������
		{
			return OnDBServerDummyOnLine(dwContextID, pData, wDataSize);
		}
	case DBO_GP_SERVER_DUMMY_ONLINE_END:
		{
			return OnDBServerDummyOnLineEnd(dwContextID, pData, wDataSize);
		}
	case DBO_GP_CHECKIN_INFO:			//ǩ����Ϣ
		{
			return OnDBPCUserCheckInInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_CHECKIN_RESULT:			//ǩ�����
		{
			return OnDBPCUserCheckInResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_CHECKIN_REWARD:			//ǩ������
		{
			return OnDBPCCheckInReward(dwContextID,pData,wDataSize);
		}
	case DBO_GP_TASK_LIST:				//�����б�
		{
			return OnDBPCTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_GP_TASK_LIST_END:			//�������
		{
			return OnDBPCTaskListEnd(dwContextID,pData,wDataSize);
		}
	case DBO_GP_TASK_INFO:				//������Ϣ
		{
			return OnDBPCTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_TASK_RESULT:			//������
		{
			return OnDBPCTaskResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_BASEENSURE_PARAMETER:	//�ͱ�����
		{
			return OnDBPCBaseEnsureParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GP_BASEENSURE_RESULT:		//�ͱ����
		{
			return OnDBPCBaseEnsureResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GROWLEVEL_CONFIG:		//�ȼ�����
		{
			return OnDBPCGrowLevelConfig(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GROWLEVEL_PARAMETER:	//�ȼ�����
		{
			return OnDBPCGrowLevelParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GROWLEVEL_UPGRADE:		//�ȼ�����
		{
			return OnDBPCGrowLevelUpgrade(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_TYPE_ITEM:  //�������� 
		{
			return OnDBGamePropertyTypeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_RELAT_ITEM: //���߹�ϵ
		{
			return OnDBGamePropertyRelatItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_ITEM:		 //���߽ڵ�
		{
			return OnDBGamePropertyItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_SUB_ITEM:		 //���߽ڵ�
		{
			return OnDBGamePropertySubItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_LIST_RESULT:		//���߽��
		{
			return OnDBGamePropertyListResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_BUY:
		{
			return OnDBGamePropertyBuy(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_FAILURE:
		{
			return OnDBGamePropertyFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_BACKPACK:			//������ѯ
		{
			return OnDBQueryUserBackpack(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PROPERTY_QUERY_SINGLE:			//������ѯ
		{
			return OnDBQueryPropertySingle(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_PROPERTY_BUFF:		//����Buff
		{
			return OnDBUserPropertyBuff(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PROPERTY_PRESENT:		//���͵���
		{
			return OnDBUserPropertyPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PROPERTY_USE:		//����ʹ��
		{
			return OnDBGamePropertyUse(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_SEND_PRESENT:		//��ѯ����
		{
			return OnDBQuerySendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GET_SEND_PRESENT:		//��ȡ����
		{
			return OnDBGetSendPresent(dwContextID,pData,wDataSize);
		}
	case DBO_GP_MEMBER_PARAMETER:		//��Ա����
		{
			return OnDBPCMemberParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GP_MEMBER_QUERY_INFO_RESULT://��Ա��ѯ
		{
			return OnDBPCMemberDayQueryInfoResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_MEMBER_DAY_PRESENT_RESULT://��Ա�ͽ�
		{
			return OnDBPCMemberDayPresentResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_MEMBER_DAY_GIFT_RESULT://��Ա���
		{
			return OnDBPCMemberDayGiftResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PURCHASE_RESULT:		//������
		{
			return OnDBPCPurchaseResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_EXCHANGE_RESULT:		//�һ����
		{
			return OnDBPCExChangeResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_ROOM_CARD_EXCHANGE_RESULT:		//�һ����
		{
			return OnDBPCExChangeRoomCardToScoreResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_SPREAD_INFO:			//�ƹ���Ϣ
		{
			return OnDBPCUserSpreadInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_REAL_AUTH_PARAMETER:				//ʵ����Ϣ
		{
			return OnDBPCRealAuthParameter(dwContextID,pData,wDataSize);
		}
	case DBO_MB_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBMBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_MB_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBMBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_LOTTERY_CONFIG:			//�齱����
		{
			return OnDBPCLotteryConfig(dwContextID,pData,wDataSize);
		}
	case DBO_GP_LOTTERY_USER_INFO:		//�齱��Ϣ
		{
			return OnDBPCLotteryUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_LOTTERY_RESULT:			//�齱���
		{
			return OnDBPCLotteryResult(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_USER_GAME_DATA:	//��Ϸ����
		{
			return OnDBPCQueryUserGameData(dwContextID,pData,wDataSize);
		}
	case DBO_GP_AGENT_GAME_KIND_ITEM:
		{
			return OnDBPCAgentGameList(dwContextID,pData,wDataSize);
		}
	case DBO_MB_AGENT_GAME_KIND_ITEM:
		{
			return OnDBMBAgentGameList(dwContextID,pData,wDataSize);
		}
	case DBO_MB_PERSONAL_PARAMETER: //˽�˷������
		{
			return OnDBMBPersonalParameter(dwContextID,pData,wDataSize);
		}
	case DBO_MB_PERSONAL_FEE_LIST: //˽�˷�������б�
		{
			return OnDBMBPersonalFeeList(dwContextID,pData,wDataSize);
		}
	case DBO_MB_PERSONAL_ROOM_LIST://˽�˷����б�
		{
			return OnDBMBPersonalRoomListInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_QUERY_USER_ROOM_SCORE://˽�˷��������Ϣ
		{
			return OnDBQueryUserRoomScore(dwContextID, pData, wDataSize);
		}
	case DBO_GR_QUERY_PERSONAL_ROOM_USER_INFO_RESULT://˽�˷��������Ϣ
		{
			return OnDBQueryPersonalRoomUersInfoResult(dwContextID, pData, wDataSize);
		}
	case DBO_GP_QUERY_BANKINFO_SUCCESS://��ѯ����Ϣ�ɹ�
		{
			return OnDBQueryBankInfoResult(dwContextID, pData, wDataSize);
		}
	case DBO_MB_QUERY_GAMELOCKINFO_RESULT://��ѯ������Ϣ
		{
			return OnDBQueryGameLockInfoResult(dwContextID, pData, wDataSize);
		}
	}


	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
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

			return true;
		}
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
		CTraceService::TraceString(TEXT("����ע����Ϸ��¼������..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterPlaza RegisterPlaza;
		ZeroMemory(&RegisterPlaza,sizeof(RegisterPlaza));

		//���ñ���
		lstrcpyn(RegisterPlaza.szServerName,m_pInitParameter->m_szServerName,CountArray(RegisterPlaza.szServerName));
		lstrcpyn(RegisterPlaza.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(RegisterPlaza.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_PLAZA,&RegisterPlaza,sizeof(RegisterPlaza));

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_REMOTE_SERVICE:	//Զ�̷���
			{
				return OnTCPSocketMainRemoteService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			LPCTSTR pszDescribeString=pRegisterFailure->szDescribeString;
			if (lstrlen(pszDescribeString)>0) CTraceService::TraceString(pszDescribeString,TraceLevel_Exception);

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//��������
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);
			if(pGameServerItem == NULL) return true;

			//��ȡ����
			tagGameServer * pGameServer=&pGameServerItem->m_GameServer; 

			//��������
			DWORD dwOldOnlineCount=0,dwOldAndroidCount=0,dwOldDummyCount=0;
			dwOldOnlineCount = pGameServer->dwOnLineCount;
			dwOldAndroidCount = pGameServer->dwAndroidCount;
			dwOldDummyCount = pGameServer->dwDummyCount;

			//���ҷ���������������
			tagServerDummyOnLine * pDummyOnLine= NULL;
			m_ServerDummyOnLineMap.Lookup(pServerOnLine->wServerID,pDummyOnLine);

			//��������
			pGameServer->dwAndroidCount=pServerOnLine->dwAndroidCount;
			pGameServer->dwOnLineCount=pServerOnLine->dwOnLineCount;

			if (pDummyOnLine != NULL)
				pGameServer->dwDummyCount = pDummyOnLine->dwCurDummyOnline;
			else
				pGameServer->dwDummyCount = 0;

			//Ŀ¼����
			CGameKindItem * pGameKindItem=m_ServerListManager.SearchGameKind(pGameServer->wKindID);
			if (pGameKindItem!=NULL)
			{
				//��������
				pGameKindItem->m_GameKind.dwOnLineCount -= dwOldOnlineCount;
				pGameKindItem->m_GameKind.dwOnLineCount += pGameServer->dwOnLineCount;

				//��������
				pGameKindItem->m_GameKind.dwAndroidCount -= dwOldAndroidCount;
				pGameKindItem->m_GameKind.dwAndroidCount += pGameServer->dwAndroidCount;

				//��������
				pGameKindItem->m_GameKind.dwDummyCount -= dwOldDummyCount;
				pGameKindItem->m_GameKind.dwDummyCount += pGameServer->dwDummyCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			ASSERT(m_ServerListManager.SearchGameServer(pServerModify->wServerID));
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
			if (pGameServerItem!=NULL)
			{
				//��������
				DWORD dwOldOnlineCount=0,dwOldAndroidCount=0,dwOldFullCount=0;
				dwOldAndroidCount = pGameServerItem->m_GameServer.dwAndroidCount;
				dwOldOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;				
				dwOldFullCount   = pGameServerItem->m_GameServer.dwFullCount;

				//�޸ķ�����Ϣ
				pGameServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;
				pGameServerItem->m_GameServer.dwAndroidCount=pServerModify->dwAndroidCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGameServerItem->m_GameServer.szServerAddr));

				//Ŀ¼����
				CGameKindItem * pGameKindItem=m_ServerListManager.SearchGameKind(pGameServerItem->m_GameServer.wKindID);
				if (pGameKindItem!=NULL)
				{
					tagGameServer * pGameServer=&pGameServerItem->m_GameServer;
					pGameKindItem->m_GameKind.dwOnLineCount -= dwOldOnlineCount;
					pGameKindItem->m_GameKind.dwOnLineCount += pGameServer->dwOnLineCount;

					pGameKindItem->m_GameKind.dwFullCount -= dwOldFullCount;
					pGameKindItem->m_GameKind.dwFullCount += pGameServer->dwFullCount;

					pGameKindItem->m_GameKind.dwAndroidCount -= dwOldAndroidCount;
					pGameKindItem->m_GameKind.dwAndroidCount += pServerModify->dwAndroidCount;
				}
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			DBR_GR_CLOSE_ROOM_SERVER_ID closeRoomServerID;
			closeRoomServerID.dwServerID = pServerRemove->wServerID;

			if (m_pIPersonalRoomServiceManager)
			{
				//�����ݿɷ��Ͳ�ѯÿ��������Ϣ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CLOSE_ROOM_WRITE_DISSUME_TIME,  0,  &closeRoomServerID, sizeof(DBR_GR_CLOSE_ROOM_SERVER_ID));	
			}
			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_MATCH_INSERT:		//��������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameMatch)==0);
			if (wDataSize%sizeof(tagGameMatch)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameMatch);
			tagGameMatch * pGameMatch=(tagGameMatch *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				CGameServerItem * pGameServerItem = m_ServerListManager.SearchGameServer(pGameMatch->wServerID);
				if(pGameServerItem!=NULL)
				{
					CopyMemory(&pGameServerItem->m_GameMatch,pGameMatch++,sizeof(pGameServerItem->m_GameMatch));
				}
			}

			return true;
		}
	case SUB_CS_S_QUERY_GAME_SERVER_RESULT:		//��ѯ���
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_S_QueryGameServerResult));
			if (wDataSize!=sizeof(CMD_CS_S_QueryGameServerResult)) return true;

			CMD_CS_S_QueryGameServerResult *pQueryGameServerResult = (CMD_CS_S_QueryGameServerResult*)pData;

			//�ж�����
			ASSERT(LOWORD(pQueryGameServerResult->dwSocketID)<m_pInitParameter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(pQueryGameServerResult->dwSocketID))->dwSocketID!=pQueryGameServerResult->dwSocketID) return true;

			//��������
			CMD_MB_QueryGameServerResult QueryGameServerResult;
			ZeroMemory(&QueryGameServerResult, sizeof(CMD_MB_QueryGameServerResult));

			QueryGameServerResult.dwServerID = pQueryGameServerResult->dwServerID;
			QueryGameServerResult.bCanCreateRoom = pQueryGameServerResult->bCanCreateRoom;
			lstrcpyn(QueryGameServerResult.szErrDescrybe,  pQueryGameServerResult->szErrDescrybe, sizeof(QueryGameServerResult.szErrDescrybe));

			//��������
			m_pITCPNetworkEngine->SendData(pQueryGameServerResult->dwSocketID, MDM_MB_PERSONAL_SERVICE, SUB_MB_QUERY_GAME_SERVER_RESULT, &QueryGameServerResult, sizeof(CMD_MB_QueryGameServerResult));

			return true;
		}
	case SUB_CS_S_SEARCH_TABLE_RESULT:
		{
			//У������
			ASSERT(wDataSize == sizeof(CMD_CS_S_SearchTableResult));
			if(wDataSize != sizeof(CMD_CS_S_SearchTableResult)) return true;

			CMD_CS_S_SearchTableResult* pSearTableResult = (CMD_CS_S_SearchTableResult*)pData;

			//�ж�����
			ASSERT(LOWORD(pSearTableResult->dwSocketID)<m_pInitParameter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(pSearTableResult->dwSocketID))->dwSocketID!=pSearTableResult->dwSocketID) return true;

			//��������
			CMD_MB_SearchResult SearchResult;
			ZeroMemory(&SearchResult, sizeof(CMD_MB_SearchResult));

			SearchResult.dwServerID = pSearTableResult->dwServerID;
			SearchResult.dwTableID = pSearTableResult->dwTableID;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pSearTableResult->dwSocketID, MDM_MB_PERSONAL_SERVICE, SUB_MB_SEARCH_RESULT, &SearchResult, sizeof(CMD_MB_SearchResult));

			return true;
		}
	case SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT:
		{
			//У������
			ASSERT(wDataSize == sizeof(CMD_CS_S_SearchTableResult));
			if(wDataSize != sizeof(CMD_CS_S_SearchTableResult)) return true;

			CMD_CS_S_SearchTableResult* pSearTableResult = (CMD_CS_S_SearchTableResult*)pData;

			//�ж�����
			ASSERT(LOWORD(pSearTableResult->dwSocketID)<m_pInitParameter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(pSearTableResult->dwSocketID))->dwSocketID!=pSearTableResult->dwSocketID) return true;

			//��������
			CMD_MB_DissumeSearchResult SearchResult;
			ZeroMemory(&SearchResult, sizeof(CMD_MB_DissumeSearchResult));

			SearchResult.dwServerID = pSearTableResult->dwServerID;
			SearchResult.dwTableID = pSearTableResult->dwTableID;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pSearTableResult->dwSocketID, MDM_MB_PERSONAL_SERVICE, SUB_MB_DISSUME_SEARCH_RESULT, &SearchResult, sizeof(CMD_MB_DissumeSearchResult));

			return true;
		}
	case SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT:
		{
			//У������
			ASSERT(wDataSize == sizeof(CMD_CS_C_HostCreatRoomInfo));
			if(wDataSize != sizeof(CMD_CS_C_HostCreatRoomInfo)) return true;

			CMD_CS_C_HostCreatRoomInfo* pHostCreatRoomInfo = (CMD_CS_C_HostCreatRoomInfo*)pData;

			//�����ݿɷ��Ͳ�ѯÿ��������Ϣ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_PERSONAL_ROOM_INFO,  pHostCreatRoomInfo->wSocketID,  pHostCreatRoomInfo, sizeof(CMD_CS_C_HostCreatRoomInfo));	

			return true;
		}
	case SUB_CS_S_PLAZA_CHANGE_DUMMY:
		{
			return m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_SERVER_DUMMY_ONLINE,0,NULL,0);
		}
	}

	return true;
}

//Զ�̷���
bool CAttemperEngineSink::OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SEARCH_CORRESPOND:	//Э������
		{
			//��������
			CMD_CS_S_SearchCorrespond * pSearchCorrespond=(CMD_CS_S_SearchCorrespond *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_CS_S_SearchCorrespond));
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)));
			ASSERT(wDataSize==(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)+pSearchCorrespond->wUserCount*sizeof(pSearchCorrespond->UserRemoteInfo[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_CS_S_SearchCorrespond)) return false;
			if (wDataSize<(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo))) return false;
			if (wDataSize!=(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)+pSearchCorrespond->wUserCount*sizeof(pSearchCorrespond->UserRemoteInfo[0]))) return false;

			//�ж�����
			ASSERT(LOWORD(pSearchCorrespond->dwSocketID)<m_pInitParameter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(pSearchCorrespond->dwSocketID))->dwSocketID!=pSearchCorrespond->dwSocketID) return true;

			//��������
			CMD_GP_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//���ñ���
			for (WORD i=0;i<pSearchCorrespond->wUserCount;i++)
			{
				//����Ч��
				ASSERT(SearchCorrespond.wUserCount<CountArray(SearchCorrespond.UserRemoteInfo));
				if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

				//���ñ���
				WORD wIndex=SearchCorrespond.wUserCount++;
				CopyMemory(&SearchCorrespond.UserRemoteInfo[wIndex],&pSearchCorrespond->UserRemoteInfo[i],sizeof(SearchCorrespond.UserRemoteInfo[wIndex]));
			}

			//��������
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(pSearchCorrespond->dwSocketID,MDM_GP_REMOTE_SERVICE,SUB_GP_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
	case SUB_CS_S_SEARCH_ALLCORRESPOND:
		{
			//��������
			CMD_CS_S_SearchAllCorrespond * pSearchAllCorrespond=(CMD_CS_S_SearchAllCorrespond *)pData;


			BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
			WORD cbDataSize = sizeof(CMD_GP_S_SearchAllCorrespond)-sizeof(tagUserRemoteInfo);
			CMD_GP_S_SearchAllCorrespond * pSearchCorrespond=(CMD_GP_S_SearchAllCorrespond *)cbDataBuffer;
			pSearchCorrespond->dwCount = pSearchAllCorrespond->dwCount;
			int nCount =(int)(pSearchAllCorrespond->dwCount);
			for(int i = 0; i < nCount; i++)
			{
				memcpy(&pSearchCorrespond->UserRemoteInfo[i], &pSearchAllCorrespond->UserRemoteInfo[i], sizeof(tagUserRemoteInfo));

				cbDataSize += sizeof(tagUserRemoteInfo);
			}

			if(pSearchAllCorrespond->dwCount == 0) cbDataSize = sizeof(CMD_GP_S_SearchAllCorrespond);
			m_pITCPNetworkEngine->SendData(pSearchAllCorrespond->dwSocketID,MDM_GP_REMOTE_SERVICE,SUB_GP_S_SEARCH_ALLCORRESPOND,pSearchCorrespond,cbDataSize);
			return true;
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_PLATFORM_PARAMETER:	//ƽ̨����
		{
			//�������
			m_wTaskCountBuffer = 0;
			ZeroMemory(m_TaskParameterBuffer, sizeof(m_TaskParameterBuffer));

			//ƽ̨����
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_PLATFORM_PARAMETER,0,NULL,0);		

			//��������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_TASK_LIST, 0, NULL, 0);

			//��Ա����
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_MEMBER_PARAMETER,0,NULL,0);	

			//ǩ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CHECKIN_REWARD, 0, NULL, 0);			

			//���صͱ�
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_BASEENSURE, 0, NULL, 0);

			//������֤
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_REAL_AUTH, 0, NULL, 0);

			//��������
			m_GamePropertyListManager.DisusePropertyTypeItem();
			m_GamePropertyListManager.DisusePropertyItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_PROPERTY_LIST,0,NULL,0);

			return true;
		}
	}

	return true;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_GAMEID:		//I D ��¼
		{
			return OnTCPNetworkSubPCLogonGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubPCLogonAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_LOGON_MANAGETOOL:	//���ߵ�¼
		{
			return OnTCPNetworkSubPCLogonManageTool(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_REGISTER_ACCOUNTS:	//�ʺ�ע��
		{
			return OnTCPNetworkSubPCRegisterAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_VERIFY_INDIVIDUAL:	//У������
		{
			return OnTCPNetworkSubPCVerifyIndividual(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_LOGON_VISITOR:      //�ο͵�¼
		{
			return OnTCPNetworkSubPCLogonVisitor(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_GET_LIST:			//��ȡ�б�
		{
			//�����б�
			SendGameTypeInfo(dwSocketID);
			SendGameKindInfo(dwSocketID);

			//�����б�
			if (m_pInitParameter->m_cbDelayList==FALSE)
			{
				//�����б�
				SendGamePageInfo(dwSocketID,INVALID_WORD);
				SendGameNodeInfo(dwSocketID,INVALID_WORD);
				//SendGameServerInfo(dwSocketID,INVALID_WORD);
				SendGameServerInfo(dwSocketID,INVALID_WORD,DEVICE_TYPE_PC);
			}
			else
			{
				//����ҳ��
				SendGamePageInfo(dwSocketID,0);
			}

			//�������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);

			return true;
		}
	case SUB_GP_GET_SERVER:			//��ȡ����
		{
			//Ч������
			ASSERT(wDataSize%sizeof(WORD)==0);
			if (wDataSize%sizeof(WORD)!=0) return false;

			//�����б�
			UINT nKindCount=wDataSize/sizeof(WORD);
			for (UINT i=0;i<nKindCount;i++)
			{
				SendGameNodeInfo(dwSocketID,((WORD *)pData)[i]);
				SendGamePageInfo(dwSocketID,((WORD *)pData)[i]);
				//SendGameServerInfo(dwSocketID,((WORD *)pData)[i]);
				SendGameServerInfo(dwSocketID,((WORD *)pData)[i],DEVICE_TYPE_PC);
			}

			//�������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_SERVER_FINISH,pData,wDataSize);

			return true;
		}
	case SUB_GP_GET_ONLINE:			//��ȡ����
		{
			//��������
			CMD_GP_GetOnline * pGetOnline=(CMD_GP_GetOnline *)pData;
			WORD wHeadSize=(sizeof(CMD_GP_GetOnline)-sizeof(pGetOnline->wOnLineServerID));

			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGetOnline->wServerCount*sizeof(WORD))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGetOnline->wServerCount*sizeof(WORD)))) return false;

			//��������
			CMD_GP_KindOnline KindOnline;
			CMD_GP_ServerOnline ServerOnline;
			ZeroMemory(&KindOnline,sizeof(KindOnline));
			ZeroMemory(&ServerOnline,sizeof(ServerOnline));

			//��ȡ����
			POSITION KindPosition=NULL;
			do
			{
				//��ȡ����
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//���ñ���
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=KindOnline.wKindCount++;
					KindOnline.OnLineInfoKind[wKindIndex].wKindID=pGameKindItem->m_GameKind.wKindID;
					KindOnline.OnLineInfoKind[wKindIndex].dwOnLineCount=pGameKindItem->m_GameKind.dwOnLineCount+pGameKindItem->m_GameKind.dwAndroidCount;
				}

				//����ж�
				if (KindOnline.wKindCount>=CountArray(KindOnline.OnLineInfoKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//��ȡ����
			for (WORD i=0;i<pGetOnline->wServerCount;i++)
			{
				//��ȡ����
				WORD wServerID=pGetOnline->wOnLineServerID[i];
				CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(wServerID);

				//���ñ���
				if (pGameServerItem!=NULL)
				{
					WORD wServerIndex=ServerOnline.wServerCount++;
					ServerOnline.OnLineInfoServer[wServerIndex].wServerID=wServerID;
					ServerOnline.OnLineInfoServer[wServerIndex].dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount+pGameServerItem->m_GameServer.dwAndroidCount;
				}

				//����ж�
				if (ServerOnline.wServerCount>=CountArray(ServerOnline.OnLineInfoServer))
				{
					ASSERT(FALSE);
					break;
				}
			}

			//��������
			if (KindOnline.wKindCount>0)
			{
				WORD wHeadSize=sizeof(KindOnline)-sizeof(KindOnline.OnLineInfoKind);
				WORD wSendSize=wHeadSize+KindOnline.wKindCount*sizeof(KindOnline.OnLineInfoKind[0]);
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_KINE_ONLINE,&KindOnline,wSendSize);
			}

			//��������
			if (ServerOnline.wServerCount>0)
			{
				WORD wHeadSize=sizeof(ServerOnline)-sizeof(ServerOnline.OnLineInfoServer);
				WORD wSendSize=wHeadSize+ServerOnline.wServerCount*sizeof(ServerOnline.OnLineInfoServer[0]);
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_SERVER_ONLINE,&ServerOnline,wSendSize);
			}

			return true;
		}
	case SUB_GP_GET_COLLECTION:		//��ȡ�ղ�
		{
			return true;
		}
	case SUB_GP_GET_PROPERTY:		//��ȡ����
		{
			//��������
			SendGamePropertyTypeInfo(dwSocketID);
			SendGamePropertyRelatInfo(dwSocketID);
			SendGamePropertyInfo(dwSocketID);
			SendGamePropertySubInfo(dwSocketID);
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_PROPERTY_FINISH);

			return true;
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_MACHINE:			  //�󶨻���
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyMachine));
			if (wDataSize!=sizeof(CMD_GP_ModifyMachine)) return false;

			//������Ϣ
			CMD_GP_ModifyMachine * pModifyMachine=(CMD_GP_ModifyMachine *)pData;
			pModifyMachine->szPassword[CountArray(pModifyMachine->szPassword)-1]=0;

			//��������
			DBR_GP_ModifyMachine ModifyMachine;
			ZeroMemory(&ModifyMachine,sizeof(ModifyMachine));

			//��������
			ModifyMachine.cbBind=pModifyMachine->cbBind;
			ModifyMachine.dwUserID=pModifyMachine->dwUserID;
			ModifyMachine.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyMachine.szPassword,pModifyMachine->szPassword,CountArray(ModifyMachine.szPassword));
			lstrcpyn(ModifyMachine.szMachineID,pModifyMachine->szMachineID,CountArray(ModifyMachine.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_MACHINE,dwSocketID,&ModifyMachine,sizeof(ModifyMachine));

			return true;
		}
	case SUB_GP_MODIFY_LOGON_PASS:		  //�޸�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyLogonPass));
			if (wDataSize!=sizeof(CMD_GP_ModifyLogonPass)) return false;

			//������Ϣ
			CMD_GP_ModifyLogonPass * pModifyLogonPass=(CMD_GP_ModifyLogonPass *)pData;
			pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword)-1]=0;
			pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword)-1]=0;

			//��������
			DBR_GP_ModifyLogonPass ModifyLogonPass;
			ZeroMemory(&ModifyLogonPass,sizeof(ModifyLogonPass));

			//��������
			ModifyLogonPass.dwUserID=pModifyLogonPass->dwUserID;
			ModifyLogonPass.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyLogonPass.szDesPassword,pModifyLogonPass->szDesPassword,CountArray(ModifyLogonPass.szDesPassword));
			lstrcpyn(ModifyLogonPass.szScrPassword,pModifyLogonPass->szScrPassword,CountArray(ModifyLogonPass.szScrPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_LOGON_PASS,dwSocketID,&ModifyLogonPass,sizeof(ModifyLogonPass));

			return true;
		}
	case SUB_GP_MODIFY_INSURE_PASS:		  //�޸�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyInsurePass));
			if (wDataSize!=sizeof(CMD_GP_ModifyInsurePass)) return false;

			//������Ϣ
			CMD_GP_ModifyInsurePass * pModifyInsurePass=(CMD_GP_ModifyInsurePass *)pData;
			pModifyInsurePass->szDesPassword[CountArray(pModifyInsurePass->szDesPassword)-1]=0;
			pModifyInsurePass->szScrPassword[CountArray(pModifyInsurePass->szScrPassword)-1]=0;

			//��������
			DBR_GP_ModifyInsurePass ModifyInsurePass;
			ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

			//��������
			ModifyInsurePass.dwUserID=pModifyInsurePass->dwUserID;
			ModifyInsurePass.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyInsurePass.szDesPassword,pModifyInsurePass->szDesPassword,CountArray(ModifyInsurePass.szDesPassword));
			lstrcpyn(ModifyInsurePass.szScrPassword,pModifyInsurePass->szScrPassword,CountArray(ModifyInsurePass.szScrPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INSURE_PASS,dwSocketID,&ModifyInsurePass,sizeof(ModifyInsurePass));

			return true;
		}
	case SUB_GP_MODIFY_UNDER_WRITE:		  //�޸�ǩ��
		{
			//��������
			CMD_GP_ModifyUnderWrite * pModifyUnderWrite=(CMD_GP_ModifyUnderWrite *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GP_ModifyUnderWrite));
			ASSERT(wDataSize>=(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)));
			ASSERT(wDataSize==(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)+CountStringBuffer(pModifyUnderWrite->szUnderWrite)));

			//Ч�����
			if (wDataSize>sizeof(CMD_GP_ModifyUnderWrite)) return false;
			if (wDataSize<(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite))) return false;
			if (wDataSize!=(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)+CountStringBuffer(pModifyUnderWrite->szUnderWrite))) return false;

			//������Ϣ
			pModifyUnderWrite->szPassword[CountArray(pModifyUnderWrite->szPassword)-1]=0;
			pModifyUnderWrite->szUnderWrite[CountArray(pModifyUnderWrite->szUnderWrite)-1]=0;

			//��������
			DBR_GP_ModifyUnderWrite ModifyUnderWrite;
			ZeroMemory(&ModifyUnderWrite,sizeof(ModifyUnderWrite));

			//��������
			ModifyUnderWrite.dwUserID=pModifyUnderWrite->dwUserID;
			ModifyUnderWrite.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyUnderWrite.szPassword,pModifyUnderWrite->szPassword,CountArray(ModifyUnderWrite.szPassword));
			lstrcpyn(ModifyUnderWrite.szUnderWrite,pModifyUnderWrite->szUnderWrite,CountArray(ModifyUnderWrite.szUnderWrite));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_UNDER_WRITE,dwSocketID,&ModifyUnderWrite,sizeof(ModifyUnderWrite));

			return true;
		}
	case SUB_GP_REAL_AUTH_QUERY:		  //ʵ����֤
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_RealAuth));

			//��������
			CMD_GP_RealAuth * pCmdData=(CMD_GP_RealAuth *)pData;

			//������Ϣ
			pCmdData->szPassword[CountArray(pCmdData->szPassword)-1]=0;
			pCmdData->szCompellation[CountArray(pCmdData->szCompellation)-1]=0;
			pCmdData->szPassPortID[CountArray(pCmdData->szPassPortID)-1]=0;

			//��������
			DBR_GP_RealAuth DataPara;
			ZeroMemory(&DataPara,sizeof(DataPara));

			//��������
			DataPara.dwUserID=pCmdData->dwUserID;
			DataPara.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(DataPara.szPassword,pCmdData->szPassword,CountArray(DataPara.szPassword));
			lstrcpyn(DataPara.szCompellation,pCmdData->szCompellation,CountArray(DataPara.szCompellation));
			lstrcpyn(DataPara.szPassPortID,pCmdData->szPassPortID,CountArray(DataPara.szPassPortID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_REAL_AUTH,dwSocketID,&DataPara,sizeof(DataPara));

			return true;
		}
	case SUB_GP_SYSTEM_FACE_INFO:		  //�޸�ͷ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_SystemFaceInfo));
			if (wDataSize!=sizeof(CMD_GP_SystemFaceInfo)) return false;

			//������Ϣ
			CMD_GP_SystemFaceInfo * pSystemFaceInfo=(CMD_GP_SystemFaceInfo *)pData;

			//��������
			DBR_GP_ModifySystemFace ModifySystemFace;
			ZeroMemory(&ModifySystemFace,sizeof(ModifySystemFace));

			//��������
			ModifySystemFace.wFaceID=pSystemFaceInfo->wFaceID;
			ModifySystemFace.dwUserID=pSystemFaceInfo->dwUserID;
			ModifySystemFace.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifySystemFace.szPassword,pSystemFaceInfo->szPassword,CountArray(ModifySystemFace.szPassword));
			lstrcpyn(ModifySystemFace.szMachineID,pSystemFaceInfo->szMachineID,CountArray(ModifySystemFace.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_SYSTEM_FACE,dwSocketID,&ModifySystemFace,sizeof(ModifySystemFace));

			return true;
		}
	case SUB_GP_CUSTOM_FACE_INFO:		  //�޸�ͷ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_CustomFaceInfo));
			if (wDataSize!=sizeof(CMD_GP_CustomFaceInfo)) return false;

			//������Ϣ
			CMD_GP_CustomFaceInfo * pCustomFaceInfo=(CMD_GP_CustomFaceInfo *)pData;

			//��������
			DBR_GP_ModifyCustomFace ModifyCustomFace;
			ZeroMemory(&ModifyCustomFace,sizeof(ModifyCustomFace));

			//��������
			ModifyCustomFace.dwUserID=pCustomFaceInfo->dwUserID;
			ModifyCustomFace.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyCustomFace.szPassword,pCustomFaceInfo->szPassword,CountArray(ModifyCustomFace.szPassword));
			lstrcpyn(ModifyCustomFace.szMachineID,pCustomFaceInfo->szMachineID,CountArray(ModifyCustomFace.szMachineID));
			CopyMemory(ModifyCustomFace.dwCustomFace,pCustomFaceInfo->dwCustomFace,sizeof(ModifyCustomFace.dwCustomFace));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_CUSTOM_FACE,dwSocketID,&ModifyCustomFace,sizeof(ModifyCustomFace));

			return true;
		}
	case SUB_GP_QUERY_INDIVIDUAL:		  //��ѯ��Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryIndividual));
			if (wDataSize!=sizeof(CMD_GP_QueryIndividual)) return false;

			//������Ϣ
			CMD_GP_QueryIndividual * pQueryIndividual=(CMD_GP_QueryIndividual *)pData;

			//��������
			DBR_GP_QueryIndividual QueryIndividual;
			ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

			//��������
			QueryIndividual.dwUserID=pQueryIndividual->dwUserID;
			QueryIndividual.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(QueryIndividual.szPassword,pQueryIndividual->szPassword,CountArray(QueryIndividual.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INDIVIDUAL,dwSocketID,&QueryIndividual,sizeof(QueryIndividual));

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL:		  //�޸�����
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GP_ModifyIndividual));
			if (wDataSize<sizeof(CMD_GP_ModifyIndividual)) return false;

			//������Ϣ
			CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)pData;
			pModifyIndividual->szPassword[CountArray(pModifyIndividual->szPassword)-1]=0;

			//��������
			DBR_GP_ModifyIndividual ModifyIndividual;
			ZeroMemory(&ModifyIndividual,sizeof(ModifyIndividual));

			//���ñ���
			ModifyIndividual.dwUserID=pModifyIndividual->dwUserID;
			ModifyIndividual.cbGender=pModifyIndividual->cbGender;
			ModifyIndividual.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyIndividual.szPassword,pModifyIndividual->szPassword,CountArray(ModifyIndividual.szPassword));

			//��������
			VOID * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pModifyIndividual+1,wDataSize-sizeof(CMD_GP_ModifyIndividual));

			//��չ��Ϣ
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_GP_UI_NICKNAME:		//�û��ǳ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szNickName));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szNickName))
						{
							CopyMemory(&ModifyIndividual.szNickName,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szNickName[CountArray(ModifyIndividual.szNickName)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_UNDER_WRITE:		//����ǩ��
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUnderWrite))
						{
							CopyMemory(&ModifyIndividual.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUnderWrite[CountArray(ModifyIndividual.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_USER_NOTE:		//�û���ע
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote))
						{
							CopyMemory(&ModifyIndividual.szUserNote,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUserNote[CountArray(ModifyIndividual.szUserNote)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_COMPELLATION:	//��ʵ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation))
						{
							CopyMemory(&ModifyIndividual.szCompellation,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szCompellation[CountArray(ModifyIndividual.szCompellation)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_PASSPORTID:	//���֤
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szPassPortID));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szPassPortID))
						{
							CopyMemory(&ModifyIndividual.szPassPortID,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szPassPortID[CountArray(ModifyIndividual.szPassPortID)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_SEAT_PHONE:		//�̶��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone))
						{
							CopyMemory(ModifyIndividual.szSeatPhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szSeatPhone[CountArray(ModifyIndividual.szSeatPhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_MOBILE_PHONE:	//�ƶ��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone))
						{
							CopyMemory(ModifyIndividual.szMobilePhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szMobilePhone[CountArray(ModifyIndividual.szMobilePhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_QQ:				//Q Q ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ))
						{
							CopyMemory(ModifyIndividual.szQQ,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szQQ[CountArray(ModifyIndividual.szQQ)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_EMAIL:			//�����ʼ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail))
						{
							CopyMemory(ModifyIndividual.szEMail,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szEMail[CountArray(ModifyIndividual.szEMail)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_DWELLING_PLACE:	//��ϵ��ַ
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace))
						{
							CopyMemory(ModifyIndividual.szDwellingPlace,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szDwellingPlace[CountArray(ModifyIndividual.szDwellingPlace)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_SPREADER:	//�ƹ��ʶ
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSpreader));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSpreader))
						{
							CopyMemory(ModifyIndividual.szSpreader,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szSpreader[CountArray(ModifyIndividual.szSpreader)-1]=0;
						}
						break;
					}
				}
			}

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INDIVIDUAL,dwSocketID,&ModifyIndividual,sizeof(ModifyIndividual));

			return true;
		}
	case SUB_GP_USER_ENABLE_INSURE:		  //��ͨ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_UserEnableInsure));
			if (wDataSize!=sizeof(CMD_GP_UserEnableInsure)) return false;

			//������Ϣ
			CMD_GP_UserEnableInsure * pUserEnableInsure=(CMD_GP_UserEnableInsure *)pData;
			pUserEnableInsure->szLogonPass[CountArray(pUserEnableInsure->szLogonPass)-1]=0;
			pUserEnableInsure->szInsurePass[CountArray(pUserEnableInsure->szInsurePass)-1]=0;
			pUserEnableInsure->szMachineID[CountArray(pUserEnableInsure->szMachineID)-1]=0;

			//��������
			DBR_GP_UserEnableInsure UserEnableInsure;
			ZeroMemory(&UserEnableInsure,sizeof(UserEnableInsure));

			//��������
			UserEnableInsure.dwUserID=pUserEnableInsure->dwUserID;			
			UserEnableInsure.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(UserEnableInsure.szLogonPass,pUserEnableInsure->szLogonPass,CountArray(UserEnableInsure.szLogonPass));
			lstrcpyn(UserEnableInsure.szInsurePass,pUserEnableInsure->szInsurePass,CountArray(UserEnableInsure.szInsurePass));
			lstrcpyn(UserEnableInsure.szMachineID,pUserEnableInsure->szMachineID,CountArray(UserEnableInsure.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_ENABLE_INSURE,dwSocketID,&UserEnableInsure,sizeof(UserEnableInsure));

			return true;
		}
	case SUB_GP_USER_SAVE_SCORE:		  //������Ϸ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_UserSaveScore));
			if (wDataSize!=sizeof(CMD_GP_UserSaveScore)) return false;

			//������Ϣ
			CMD_GP_UserSaveScore * pUserSaveScore=(CMD_GP_UserSaveScore *)pData;
			pUserSaveScore->szMachineID[CountArray(pUserSaveScore->szMachineID)-1]=0;

			//��������
			DBR_GP_UserSaveScore UserSaveScore;
			ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

			//��������
			UserSaveScore.dwUserID=pUserSaveScore->dwUserID;
			UserSaveScore.lSaveScore=pUserSaveScore->lSaveScore;
			UserSaveScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(UserSaveScore.szMachineID,pUserSaveScore->szMachineID,CountArray(UserSaveScore.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

			return true;
		}
	case SUB_GP_QUERY_BANK_INFO:		//��ѯ������Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryBankInfo));
			if (wDataSize!=sizeof(CMD_GP_QueryBankInfo)) return false;

			//������Ϣ
			CMD_GP_QueryBankInfo * pClientData=(CMD_GP_QueryBankInfo *)pData;

			//��������
			DBR_GP_QueryBankInfo DBRData;
			ZeroMemory(&DBRData,sizeof(DBRData));

			//��������
			DBRData.dwUserID=pClientData->dwUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_BANK_INFO,dwSocketID,&DBRData,sizeof(DBRData));

			return true;
		}
	case SUB_GP_BIND_BANK_INFO:			//���û����п���Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_BindBankInfo));
			if (wDataSize!=sizeof(CMD_GP_BindBankInfo)) return false;

			//������Ϣ
			CMD_GP_BindBankInfo * pClientData=(CMD_GP_BindBankInfo *)pData;
			pClientData->szBankAddress[CountArray(pClientData->szBankAddress)-1]=0;
			pClientData->szBankName[CountArray(pClientData->szBankName)-1]=0;
			pClientData->szBankNo[CountArray(pClientData->szBankNo)-1]=0;
			pClientData->szRealName[CountArray(pClientData->szRealName)-1]=0;
			pClientData->szAlipayID[CountArray(pClientData->szAlipayID)-1]=0;

			//��������
			DBR_GP_BindBankInfo DBRData;
			ZeroMemory(&DBRData,sizeof(DBRData));
			//��������
			DBRData.dwUserID=pClientData->dwUserID;
			lstrcpyn(DBRData.szBankAddress,pClientData->szBankAddress,CountArray(DBRData.szBankAddress));
			lstrcpyn(DBRData.szBankName,pClientData->szBankName,CountArray(DBRData.szBankName));
			lstrcpyn(DBRData.szBankNo,pClientData->szBankNo,CountArray(DBRData.szBankNo));
			lstrcpyn(DBRData.szRealName,pClientData->szRealName,CountArray(DBRData.szRealName));
			lstrcpyn(DBRData.szAlipayID,pClientData->szAlipayID,CountArray(DBRData.szAlipayID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_BIND_BANK_INFO,dwSocketID,&DBRData,sizeof(DBRData));
			return true;

		}
	case SUB_GP_EXCHANGE_BANK_SCORE:
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ExchangeBankScore));
			if (wDataSize!=sizeof(CMD_GP_ExchangeBankScore)) return false;

			//������Ϣ
			CMD_GP_ExchangeBankScore * pClientData=(CMD_GP_ExchangeBankScore *)pData;
			pClientData->szBankPass[CountArray(pClientData->szBankPass)-1]=0;
			pClientData->szOrderID[CountArray(pClientData->szOrderID)-1]=0;

			//��������
			DBR_GP_ExchangeBankScore DBRData;
			ZeroMemory(&DBRData,sizeof(DBRData));
			//��������
			DBRData.dwUserID=pClientData->dwUserID;
			DBRData.dwScore = pClientData->dwScore;
			lstrcpyn(DBRData.szBankPass,pClientData->szBankPass,CountArray(DBRData.szBankPass));
			lstrcpyn(DBRData.szOrderID,pClientData->szOrderID,CountArray(DBRData.szOrderID));

			DBRData.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			DBRData.dwType=pClientData->dwType;
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_EXCHANGE_BANK_SCORE,dwSocketID,&DBRData,sizeof(DBRData));
			return true;
		}
	case SUB_GP_USER_TAKE_SCORE:		  //ȡ����Ϸ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_UserTakeScore));
			if (wDataSize!=sizeof(CMD_GP_UserTakeScore)) return false;

			//������Ϣ
			CMD_GP_UserTakeScore * pUserTakeScore=(CMD_GP_UserTakeScore *)pData;
			pUserTakeScore->szPassword[CountArray(pUserTakeScore->szPassword)-1]=0;
			pUserTakeScore->szMachineID[CountArray(pUserTakeScore->szMachineID)-1]=0;

			//��������
			DBR_GP_UserTakeScore UserTakeScore;
			ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

			//��������
			UserTakeScore.dwUserID=pUserTakeScore->dwUserID;
			UserTakeScore.lTakeScore=pUserTakeScore->lTakeScore;
			UserTakeScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(UserTakeScore.szPassword,pUserTakeScore->szPassword,CountArray(UserTakeScore.szPassword));
			lstrcpyn(UserTakeScore.szMachineID,pUserTakeScore->szMachineID,CountArray(UserTakeScore.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

			return true;
		}
	case SUB_GP_USER_TRANSFER_SCORE:	  //ת����Ϸ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_UserTransferScore));
			if (wDataSize!=sizeof(CMD_GP_UserTransferScore)) return false;

			//������Ϣ
			CMD_GP_UserTransferScore * pUserTransferScore=(CMD_GP_UserTransferScore *)pData;
			pUserTransferScore->szAccounts[CountArray(pUserTransferScore->szAccounts)-1]=0;
			pUserTransferScore->szPassword[CountArray(pUserTransferScore->szPassword)-1]=0;
			pUserTransferScore->szMachineID[CountArray(pUserTransferScore->szMachineID)-1]=0;

			//��������
			DBR_GP_UserTransferScore UserTransferScore;
			ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

			//��������
			UserTransferScore.dwUserID=pUserTransferScore->dwUserID;
			UserTransferScore.lTransferScore=pUserTransferScore->lTransferScore;
			UserTransferScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(UserTransferScore.szAccounts,pUserTransferScore->szAccounts,CountArray(UserTransferScore.szAccounts));
			lstrcpyn(UserTransferScore.szPassword,pUserTransferScore->szPassword,CountArray(UserTransferScore.szPassword));
			lstrcpyn(UserTransferScore.szMachineID,pUserTransferScore->szMachineID,CountArray(UserTransferScore.szMachineID));
			lstrcpyn(UserTransferScore.szTransRemark,pUserTransferScore->szTransRemark,CountArray(UserTransferScore.szTransRemark));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

			return true;
		}
	case SUB_GP_QUERY_INSURE_INFO:		  //��ѯ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryInsureInfo));
			if (wDataSize!=sizeof(CMD_GP_QueryInsureInfo)) return false;

			//������Ϣ
			CMD_GP_QueryInsureInfo * pQueryInsureInfo=(CMD_GP_QueryInsureInfo *)pData;

			//��������
			DBR_GP_QueryInsureInfo QueryInsureInfo;
			ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

			//��������
			QueryInsureInfo.dwUserID=pQueryInsureInfo->dwUserID;
			QueryInsureInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(QueryInsureInfo.szPassword,pQueryInsureInfo->szPassword,CountArray(QueryInsureInfo.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

			return true;
		}
	case SUB_GP_QUERY_USER_INFO_REQUEST:  //��ѯ�û�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryUserInfoRequest));
			if (wDataSize!=sizeof(CMD_GP_QueryUserInfoRequest)) return false;

			//������Ϣ
			CMD_GP_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GP_QueryUserInfoRequest *)pData;

			//ID�ж�
			if(pQueryUserInfoRequest->cbByNickName==FALSE)
			{
				//�����ж�
				int nLen=lstrlen(pQueryUserInfoRequest->szAccounts);
				if(nLen>=8)
				{
					SendInsureFailure(dwSocketID,0,TEXT("������Ϸ������ID��"));
					return true;
				}

				//�Ϸ��ж�
				for(int i=0; i<nLen; i++)
				{
					if(pQueryUserInfoRequest->szAccounts[i] < TEXT('0') || pQueryUserInfoRequest->szAccounts[i] > TEXT('9'))
					{
						SendInsureFailure(dwSocketID,0,TEXT("������Ϸ������ID��"));
						return true;
					}
				}
			}

			//��������
			DBR_GP_QueryInsureUserInfo QueryInsureUserInfo;
			ZeroMemory(&QueryInsureUserInfo,sizeof(QueryInsureUserInfo));

			//��������
			QueryInsureUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
			lstrcpyn(QueryInsureUserInfo.szAccounts,pQueryUserInfoRequest->szAccounts,CountArray(QueryInsureUserInfo.szAccounts));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_USER_INFO,dwSocketID,&QueryInsureUserInfo,sizeof(QueryInsureUserInfo));

			return true;
		}
	case SUB_GP_QUERY_TRANSFER_REBATE:		  //��ѯ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_QueryTransferRebate));
			if(wDataSize!=sizeof(CMD_GP_QueryTransferRebate)) return false;

			//��ȡ����
			CMD_GP_QueryTransferRebate *pRebate = (CMD_GP_QueryTransferRebate *)pData;
			pRebate->szPassword[CountArray(pRebate->szPassword)-1]=0;

			//����ṹ
			DBR_GP_QueryTransferRebate Rebate;
			Rebate.dwUserID = pRebate->dwUserID;
			lstrcpyn(Rebate.szPassword,pRebate->szPassword,CountArray(Rebate.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_TRANSFER_REBATE,dwSocketID,&Rebate,sizeof(Rebate));

			return true;
		}
	case SUB_GP_CHECKIN_QUERY:		  //��ѯǩ��
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_CheckInQueryInfo));
			if(wDataSize!=sizeof(CMD_GP_CheckInQueryInfo)) return false;

			//��ȡ����
			CMD_GP_CheckInQueryInfo *pCheckInQueryInfo = (CMD_GP_CheckInQueryInfo *)pData;
			pCheckInQueryInfo->szPassword[CountArray(pCheckInQueryInfo->szPassword)-1]=0;

			//����ṹ
			DBR_GP_CheckInQueryInfo CheckInQueryInfo;
			CheckInQueryInfo.dwUserID = pCheckInQueryInfo->dwUserID;
			lstrcpyn(CheckInQueryInfo.szPassword,pCheckInQueryInfo->szPassword,CountArray(CheckInQueryInfo.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECKIN_QUERY_INFO,dwSocketID,&CheckInQueryInfo,sizeof(CheckInQueryInfo));

			return true;
		}
	case SUB_GP_CHECKIN_DONE:			  //ִ��ǩ��
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_CheckInDone));
			if(wDataSize!=sizeof(CMD_GP_CheckInDone)) return false;

			//��ȡ����
			CMD_GP_CheckInDone *pCheckInDone = (CMD_GP_CheckInDone *)pData;
			pCheckInDone->szPassword[CountArray(pCheckInDone->szPassword)-1]=0;
			pCheckInDone->szMachineID[CountArray(pCheckInDone->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_CheckInDone CheckInDone;
			CheckInDone.dwUserID = pCheckInDone->dwUserID;
			CheckInDone.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(CheckInDone.szPassword,pCheckInDone->szPassword,CountArray(CheckInDone.szPassword));
			lstrcpyn(CheckInDone.szMachineID,pCheckInDone->szMachineID,CountArray(CheckInDone.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECKIN_DONE,dwSocketID,&CheckInDone,sizeof(CheckInDone));

			return true;
		}
	case SUB_GP_TASK_LOAD:				//��������
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_TaskLoadInfo));
			if(wDataSize!=sizeof(CMD_GP_TaskLoadInfo)) return false;

			//��ȡ����
			CMD_GP_TaskLoadInfo * pTaskLoadInfo = (CMD_GP_TaskLoadInfo *)pData;
			pTaskLoadInfo->szPassword[CountArray(pTaskLoadInfo->szPassword)-1]=0;

			//����ṹ
			DBR_GP_TaskQueryInfo TaskQueryInfo;
			TaskQueryInfo.dwUserID = pTaskLoadInfo->dwUserID;
			lstrcpyn(TaskQueryInfo.szPassword,pTaskLoadInfo->szPassword,CountArray(TaskQueryInfo.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_TASK_QUERY_INFO,dwSocketID,&TaskQueryInfo,sizeof(TaskQueryInfo));

			return true;
		}
	case SUB_GP_TASK_GIVEUP:
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_TaskGiveUp));
			if(wDataSize!=sizeof(CMD_GP_TaskGiveUp)) return false;
			CMD_GP_TaskTake * pTask = (CMD_GP_TaskTake *)pData;
			pTask->szPassword[CountArray(pTask->szPassword)-1]=0;
			pTask->szMachineID[CountArray(pTask->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_TaskGiveUP task;
			task.dwUserID = pTask->dwUserID;
			task.wTaskID = pTask->wTaskID;
			task.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(task.szPassword,pTask->szPassword,CountArray(task.szPassword));
			lstrcpyn(task.szMachineID,pTask->szMachineID,CountArray(task.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_TASK_GIVEUP,dwSocketID,&task,sizeof(task));

			return true;
		}
	case SUB_GP_TASK_TAKE:			      //��ȡ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_TaskTake));
			if(wDataSize!=sizeof(CMD_GP_TaskTake)) return false;

			//��ȡ����
			CMD_GP_TaskTake * pTaskTake = (CMD_GP_TaskTake *)pData;
			pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
			pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_TaskTake TaskTake;
			TaskTake.dwUserID = pTaskTake->dwUserID;
			TaskTake.wTaskID = pTaskTake->wTaskID;
			TaskTake.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
			lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_TASK_TAKE,dwSocketID,&TaskTake,sizeof(TaskTake));

			return true;
		}
	case SUB_GP_TASK_REWARD:			  //��ȡ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_TaskReward));
			if(wDataSize!=sizeof(CMD_GP_TaskReward)) return false;

			//��ȡ����
			CMD_GP_TaskReward * pTaskReward = (CMD_GP_TaskReward *)pData;
			pTaskReward->szPassword[CountArray(pTaskReward->szPassword)-1]=0;
			pTaskReward->szMachineID[CountArray(pTaskReward->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_TaskReward TaskReward;
			TaskReward.dwUserID = pTaskReward->dwUserID;
			TaskReward.wTaskID = pTaskReward->wTaskID;
			TaskReward.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(TaskReward.szPassword,pTaskReward->szPassword,CountArray(TaskReward.szPassword));
			lstrcpyn(TaskReward.szMachineID,pTaskReward->szMachineID,CountArray(TaskReward.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_TASK_REWARD,dwSocketID,&TaskReward,sizeof(TaskReward));

			return true;
		}
	case SUB_GP_BASEENSURE_LOAD:		//���صͱ�
		{
			//����ṹ
			CMD_GP_BaseEnsureParamter BaseEnsureParameter;
			BaseEnsureParameter.cbTakeTimes=m_BaseEnsureParameter.cbTakeTimes;
			BaseEnsureParameter.lScoreAmount=m_BaseEnsureParameter.lScoreAmount;
			BaseEnsureParameter.lScoreCondition=m_BaseEnsureParameter.lScoreCondition;

			//Ͷ������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_USER_SERVICE,SUB_GP_BASEENSURE_PARAMETER,&BaseEnsureParameter,sizeof(BaseEnsureParameter));

			return true;
		}
	case SUB_GP_BASEENSURE_TAKE:		  //��ȡ�ͱ�
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_BaseEnsureTake));
			if(wDataSize!=sizeof(CMD_GP_BaseEnsureTake)) return false;

			//��ȡ����
			CMD_GP_BaseEnsureTake * pBaseEnsureTake = (CMD_GP_BaseEnsureTake *)pData;
			pBaseEnsureTake->szPassword[CountArray(pBaseEnsureTake->szPassword)-1]=0;
			pBaseEnsureTake->szMachineID[CountArray(pBaseEnsureTake->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_TakeBaseEnsure TakeBaseEnsure;
			TakeBaseEnsure.dwUserID = pBaseEnsureTake->dwUserID;
			TakeBaseEnsure.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(TakeBaseEnsure.szPassword,pBaseEnsureTake->szPassword,CountArray(TakeBaseEnsure.szPassword));
			lstrcpyn(TakeBaseEnsure.szMachineID,pBaseEnsureTake->szMachineID,CountArray(TakeBaseEnsure.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_BASEENSURE_TAKE,dwSocketID,&TakeBaseEnsure,sizeof(TakeBaseEnsure));

			return true;
		}
	case SUB_GP_SPREAD_QUERY:		//�ƹ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_UserSpreadQuery));
			if(wDataSize!=sizeof(CMD_GP_UserSpreadQuery)) return false;

			//��ȡ����
			CMD_GP_UserSpreadQuery * pUserSpreadQuery=(CMD_GP_UserSpreadQuery*)pData;

			//����ṹ
			DBR_GP_QuerySpreadInfo QuerySpreadInfo;
			QuerySpreadInfo.dwUserID=pUserSpreadQuery->dwUserID;

			//�ƹ㽱��
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_SPREAD_INFO,dwSocketID,&QuerySpreadInfo,sizeof(QuerySpreadInfo));

			return true;
		}
	case SUB_GP_GROWLEVEL_QUERY:		  //��ѯ�ȼ�
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_GrowLevelQueryInfo));
			if(wDataSize!=sizeof(CMD_GP_GrowLevelQueryInfo)) return false;

			//��ȡ����
			CMD_GP_GrowLevelQueryInfo * pGrowLevelQueryInfo= (CMD_GP_GrowLevelQueryInfo *)pData;
			pGrowLevelQueryInfo->szPassword[CountArray(pGrowLevelQueryInfo->szPassword)-1]=0;
			pGrowLevelQueryInfo->szMachineID[CountArray(pGrowLevelQueryInfo->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_GrowLevelQueryInfo GrowLevelQueryInfo;
			GrowLevelQueryInfo.dwUserID = pGrowLevelQueryInfo->dwUserID;
			GrowLevelQueryInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(GrowLevelQueryInfo.szPassword,pGrowLevelQueryInfo->szPassword,CountArray(GrowLevelQueryInfo.szPassword));
			lstrcpyn(GrowLevelQueryInfo.szMachineID,pGrowLevelQueryInfo->szMachineID,CountArray(GrowLevelQueryInfo.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_GROWLEVEL_QUERY_IFNO,dwSocketID,&GrowLevelQueryInfo,sizeof(GrowLevelQueryInfo));

			return true;
		}
	case SUB_GP_MEMBER_PARAMETER:		//���ػ�Ա
		{
			//����ṹ
			CMD_GP_MemberParameterResult MemberParameter;

			MemberParameter.wMemberCount = m_wMemberCount;
			CopyMemory(MemberParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameterNew)*m_wMemberCount);

			//Ͷ������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_USER_SERVICE,SUB_GP_MEMBER_PARAMETER_RESULT,&MemberParameter,sizeof(MemberParameter));

			return true;
		}
	case SUB_GP_MEMBER_QUERY_INFO:		//��Ա��ѯ
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_MemberQueryInfo));
			if(wDataSize!=sizeof(CMD_GP_MemberQueryInfo)) return false;

			//��ȡ����
			CMD_GP_MemberQueryInfo * pMemberInfo= (CMD_GP_MemberQueryInfo *)pData;
			pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
			pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_MemberQueryInfo MemberInfo;
			MemberInfo.dwUserID = pMemberInfo->dwUserID;
			MemberInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
			lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MEMBER_QUERY_INFO,dwSocketID,&MemberInfo,sizeof(MemberInfo));

			return true;
		}
	case SUB_GP_MEMBER_DAY_PRESENT:		//��Ա�ͽ�
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_MemberDayPresent));
			if(wDataSize!=sizeof(CMD_GP_MemberDayPresent)) return false;

			//��ȡ����
			CMD_GP_MemberDayPresent * pMemberInfo= (CMD_GP_MemberDayPresent *)pData;
			pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
			pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_MemberDayPresent MemberInfo;
			MemberInfo.dwUserID = pMemberInfo->dwUserID;
			MemberInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
			lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MEMBER_DAY_PRESENT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

			return true;
		}
	case SUB_GP_MEMBER_DAY_GIFT:		//��Ա���
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_MemberDayGift));
			if(wDataSize!=sizeof(CMD_GP_MemberDayGift)) return false;

			//��ȡ����
			CMD_GP_MemberDayGift * pMemberInfo= (CMD_GP_MemberDayGift *)pData;
			pMemberInfo->szPassword[CountArray(pMemberInfo->szPassword)-1]=0;
			pMemberInfo->szMachineID[CountArray(pMemberInfo->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_MemberDayGift MemberInfo;
			MemberInfo.dwUserID = pMemberInfo->dwUserID;
			MemberInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(MemberInfo.szPassword,pMemberInfo->szPassword,CountArray(MemberInfo.szPassword));
			lstrcpyn(MemberInfo.szMachineID,pMemberInfo->szMachineID,CountArray(MemberInfo.szMachineID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MEMBER_DAY_GIFT,dwSocketID,&MemberInfo,sizeof(MemberInfo));

			return true;
		}
	case SUB_GP_EXCHANGE_QUERY:		//�һ�����
		{
			//����ṹ
			CMD_GP_ExchangeParameter ExchangeParameter;
			ZeroMemory(&ExchangeParameter,sizeof(ExchangeParameter));

			//���ñ���
			ExchangeParameter.dwExchangeRate=m_PlatformParameter.dwExchangeRate;
			ExchangeParameter.wMemberCount=m_wMemberCount;
			ExchangeParameter.dwPresentExchangeRate=m_PlatformParameter.dwPresentExchangeRate;
			ExchangeParameter.dwRateGold=m_PlatformParameter.dwRateGold;
			CopyMemory(ExchangeParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameter)*m_wMemberCount);

			//�����С
			WORD wSendDataSize = sizeof(ExchangeParameter)-sizeof(ExchangeParameter.MemberParameter);
			wSendDataSize += sizeof(tagMemberParameter)*ExchangeParameter.wMemberCount;

			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_USER_SERVICE,SUB_GP_EXCHANGE_PARAMETER,&ExchangeParameter,wSendDataSize);

			return true;
		}
	case SUB_GP_PURCHASE_MEMBER:	//�����Ա
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_PurchaseMember));
			if(wDataSize!=sizeof(CMD_GP_PurchaseMember)) return false;

			//����ṹ
			CMD_GP_PurchaseMember * pPurchaseMember = (CMD_GP_PurchaseMember*)pData;
			pPurchaseMember->szMachineID[CountArray(pPurchaseMember->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_PurchaseMember PurchaseMember;
			ZeroMemory(&PurchaseMember,sizeof(PurchaseMember));

			//���ñ���
			PurchaseMember.dwUserID=pPurchaseMember->dwUserID;
			PurchaseMember.cbMemberOrder=pPurchaseMember->cbMemberOrder;
			PurchaseMember.wPurchaseTime=pPurchaseMember->wPurchaseTime;
			PurchaseMember.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(PurchaseMember.szMachineID,pPurchaseMember->szMachineID,CountArray(PurchaseMember.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_PURCHASE_MEMBER,dwSocketID,&PurchaseMember,sizeof(PurchaseMember));

			return true;
		}
	case SUB_GP_EXCHANGE_SCORE_BYINGOT:		//�һ���Ϸ��
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_ExchangeScoreByIngot));
			if(wDataSize!=sizeof(CMD_GP_ExchangeScoreByIngot)) return false;

			//����ṹ
			CMD_GP_ExchangeScoreByIngot * pExchangeScore = (CMD_GP_ExchangeScoreByIngot*)pData;
			pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_ExchangeScoreByIngot ExchangeScore;
			ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

			//���ñ���
			ExchangeScore.dwUserID=pExchangeScore->dwUserID;
			ExchangeScore.lExchangeIngot=pExchangeScore->lExchangeIngot;
			ExchangeScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_EXCHANGE_SCORE_INGOT,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

			return true;
		}
	case SUB_GP_EXCHANGE_SCORE_BYBEANS:		//�һ���Ϸ��
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_ExchangeScoreByBeans));
			if(wDataSize!=sizeof(CMD_GP_ExchangeScoreByBeans)) return false;

			//����ṹ
			CMD_GP_ExchangeScoreByBeans * pExchangeScore = (CMD_GP_ExchangeScoreByBeans*)pData;
			pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_ExchangeScoreByBeans ExchangeScore;
			ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

			//���ñ���
			ExchangeScore.dwUserID=pExchangeScore->dwUserID;
			ExchangeScore.dExchangeBeans=pExchangeScore->dExchangeBeans;
			ExchangeScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_EXCHANGE_SCORE_BEANS,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

			return true;
		}
	case SUB_GP_LOTTERY_CONFIG_REQ:			//��������
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_LotteryConfigReq));
			if(wDataSize!=sizeof(CMD_GP_LotteryConfigReq)) return false;

			//����ṹ
			CMD_GP_LotteryConfigReq * pLotteryConfigReq = (CMD_GP_LotteryConfigReq*)pData;
			pLotteryConfigReq->szLogonPass[CountArray(pLotteryConfigReq->szLogonPass)-1]=0;

			//����ṹ
			DBR_GP_LotteryConfigReq LotteryConfigReq;
			ZeroMemory(&LotteryConfigReq,sizeof(LotteryConfigReq));

			//���ñ���
			LotteryConfigReq.dwUserID=pLotteryConfigReq->dwUserID;
			LotteryConfigReq.wKindID=pLotteryConfigReq->wKindID;
			LotteryConfigReq.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(LotteryConfigReq.szLogonPass,pLotteryConfigReq->szLogonPass,CountArray(LotteryConfigReq.szLogonPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOTTERY_CONFIG_REQ,dwSocketID,&LotteryConfigReq,sizeof(LotteryConfigReq));

			return true;
		}
	case SUB_GP_LOTTERY_START:				//�齱��ʼ
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_LotteryStart));
			if(wDataSize!=sizeof(CMD_GP_LotteryStart)) return false;

			//����ṹ
			CMD_GP_LotteryStart * pLotteryStart = (CMD_GP_LotteryStart*)pData;
			pLotteryStart->szLogonPass[CountArray(pLotteryStart->szLogonPass)-1]=0;
			pLotteryStart->szMachineID[CountArray(pLotteryStart->szMachineID)-1]=0;

			//����ṹ
			DBR_GP_LotteryStart LotteryStart;
			ZeroMemory(&LotteryStart,sizeof(LotteryStart));

			//���ñ���
			LotteryStart.dwUserID=pLotteryStart->dwUserID;
			LotteryStart.wKindID=pLotteryStart->wKindID;
			LotteryStart.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(LotteryStart.szLogonPass,pLotteryStart->szLogonPass,CountArray(LotteryStart.szLogonPass));
			lstrcpyn(LotteryStart.szMachineID,pLotteryStart->szMachineID,CountArray(LotteryStart.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOTTERY_START,dwSocketID,&LotteryStart,sizeof(LotteryStart));

			return true;
		}
	case SUB_GP_QUERY_USER_GAME_DATA:		//��Ϸ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_QueryUserGameData));
			if(wDataSize!=sizeof(CMD_GP_QueryUserGameData)) return false;

			//����ṹ
			CMD_GP_QueryUserGameData * pQueryUserGameData = (CMD_GP_QueryUserGameData*)pData;
			pQueryUserGameData->szDynamicPass[CountArray(pQueryUserGameData->szDynamicPass)-1]=0;

			//����ṹ
			DBR_GP_QueryUserGameData QueryUserGameData;
			ZeroMemory(&QueryUserGameData,sizeof(QueryUserGameData));

			//���ñ���
			QueryUserGameData.dwUserID=pQueryUserGameData->dwUserID;
			QueryUserGameData.wKindID=pQueryUserGameData->wKindID;
			lstrcpyn(QueryUserGameData.szDynamicPass,pQueryUserGameData->szDynamicPass,CountArray(QueryUserGameData.szDynamicPass));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_USER_GAME_DATA,dwSocketID,&QueryUserGameData,sizeof(QueryUserGameData));

			return true;
		}
	case SUB_GP_ACCOUNT_BINDING:
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_AccountBind));
			if (wDataSize!=sizeof(CMD_GP_AccountBind)) return false;

			//������Ϣ
			CMD_GP_AccountBind * pAccountBind=(CMD_GP_AccountBind *)pData;
			pAccountBind->szPassword[CountArray(pAccountBind->szPassword)-1]=0;
			pAccountBind->szBindNewAccounts[CountArray(pAccountBind->szBindNewAccounts)-1]=0;
			pAccountBind->szBindNewPassword[CountArray(pAccountBind->szBindNewPassword)-1]=0;
			pAccountBind->szBindNewSpreader[CountArray(pAccountBind->szBindNewSpreader)-1]=0;
			

			//��������
			DBR_GP_AccountBind AccountBind;
			ZeroMemory(&AccountBind,sizeof(AccountBind));

			//��������
			AccountBind.dwUserID = pAccountBind->dwUserID;
			AccountBind.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(AccountBind.szPassword,pAccountBind->szPassword,CountArray(AccountBind.szPassword));
			lstrcpyn(AccountBind.szMachineID,pAccountBind->szMachineID,CountArray(AccountBind.szMachineID));
			
			lstrcpyn(AccountBind.szBindNewAccounts,pAccountBind->szBindNewAccounts,CountArray(AccountBind.szBindNewAccounts));
			lstrcpyn(AccountBind.szBindNewPassword,pAccountBind->szBindNewPassword,CountArray(AccountBind.szBindNewPassword));
			lstrcpyn(AccountBind.szBindNewSpreader,pAccountBind->szBindNewSpreader,CountArray(AccountBind.szBindNewSpreader));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ACCOUNT_BIND,dwSocketID,&AccountBind,sizeof(AccountBind));

			return true;
		}
	case SUB_GP_ACCOUNT_BINDING_EXISTS:
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_AccountBind_Exists));
			if (wDataSize!=sizeof(CMD_GP_AccountBind_Exists)) return false;

			//������Ϣ
			CMD_GP_AccountBind_Exists * pAccountBind=(CMD_GP_AccountBind_Exists *)pData;
			pAccountBind->szPassword[CountArray(pAccountBind->szPassword)-1]=0;
			pAccountBind->szBindExistsAccounts[CountArray(pAccountBind->szBindExistsAccounts)-1]=0;
			pAccountBind->szBindExistsPassword[CountArray(pAccountBind->szBindExistsPassword)-1]=0;


			//��������
			DBR_GP_AccountBind_Exists AccountBind;
			ZeroMemory(&AccountBind,sizeof(AccountBind));

			//��������
			AccountBind.dwUserID = pAccountBind->dwUserID;
			AccountBind.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(AccountBind.szPassword,pAccountBind->szPassword,CountArray(AccountBind.szPassword));
			lstrcpyn(AccountBind.szMachineID,pAccountBind->szMachineID,CountArray(AccountBind.szMachineID));

			lstrcpyn(AccountBind.szBindExistsAccounts,pAccountBind->szBindExistsAccounts,CountArray(AccountBind.szBindExistsAccounts));
			lstrcpyn(AccountBind.szBindExistsPassword,pAccountBind->szBindExistsPassword,CountArray(AccountBind.szBindExistsPassword));


			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ACCOUNT_BIND_EXISTS,dwSocketID,&AccountBind,sizeof(AccountBind));
			return true;
		}
	case SUB_GP_IP_LOCATION:
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_IpLocation));
			if (wDataSize!=sizeof(CMD_GP_IpLocation)) return false;
			//ip������
			CMD_GP_IpLocation * pIpAddr=(CMD_GP_IpLocation *)pData;

			//���Ƚض�
			pIpAddr->szLocation[CountArray(pIpAddr->szLocation)-1]=0;
			
			//��������
			DBR_GP_IpLocation DBRData;
			ZeroMemory(&DBRData,sizeof(DBRData));

			//��������
			DBRData.dwUserID = pIpAddr->dwUserID;
			lstrcpyn(DBRData.szLocation,pIpAddr->szLocation,CountArray(DBRData.szLocation));

			//Ͷ������
			return m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_IP_LOCATION,dwSocketID,&DBRData,sizeof(DBRData));
		}
	}

	return false;
}

//Զ�̴���
bool CAttemperEngineSink::OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_C_SEARCH_CORRESPOND:	//Э������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_GP_C_SearchCorrespond)) return false;

			//������Ϣ
			CMD_GP_C_SearchCorrespond * pSearchCorrespond=(CMD_GP_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//��������
			CMD_CS_C_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//���ӱ���
			SearchCorrespond.dwSocketID=dwSocketID;
			SearchCorrespond.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

			//���ұ���
			SearchCorrespond.dwGameID=pSearchCorrespond->dwGameID;
			lstrcpyn(SearchCorrespond.szNickName,pSearchCorrespond->szNickName,CountArray(SearchCorrespond.szNickName));

			//��������
			m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE,SUB_CS_C_SEARCH_CORRESPOND,&SearchCorrespond,sizeof(SearchCorrespond));

			return true;
		}
	case SUB_GP_C_SEARCH_ALLCORRESPOND:
		{
			//��������
			if(wDataSize >= sizeof(CMD_GP_C_SearchAllCorrespond)) 
			{
				CMD_GP_C_SearchAllCorrespond* pSearchAllCorrespond = (CMD_GP_C_SearchAllCorrespond*)pData;

				DWORD cBuffer[512]={0};
				CMD_CS_C_AllSearchCorrespond* pSearchCorrespond = (CMD_CS_C_AllSearchCorrespond*)cBuffer;

				//���ӱ���
				pSearchCorrespond->dwSocketID=dwSocketID;
				pSearchCorrespond->dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

				pSearchCorrespond->dwCount = pSearchAllCorrespond->dwCount;
				memcpy(pSearchCorrespond->dwGameID, pSearchAllCorrespond->dwGameID, sizeof(DWORD)*pSearchAllCorrespond->dwCount);

				m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE,SUB_CS_C_SEARCH_ALLCORRESPOND,pSearchCorrespond, sizeof(CMD_CS_C_AllSearchCorrespond)+sizeof(DWORD)*(WORD)(pSearchAllCorrespond->dwCount-1));
			}
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
	case SUB_GP_GET_PARAMETER:		//��ȡ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_GetParameter));
			if(wDataSize!=sizeof(CMD_GP_GetParameter)) return false;

			//��ȡ����
			CMD_GP_GetParameter * pGetParameter = (CMD_GP_GetParameter *)pData;
			ASSERT(pGetParameter!=NULL);

			//����ṹ
			DBR_GP_GetParameter GetParameter;
			GetParameter.wServerID = pGetParameter->wServerID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_GET_PARAMETER,dwSocketID,&GetParameter,sizeof(GetParameter));

			return true;
		}
	case SUB_GP_ADD_PARAMETER:		//��Ӳ���
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_AddParameter));
			if(wDataSize!=sizeof(CMD_GP_AddParameter)) return false;

			//��ȡ����
			CMD_GP_AddParameter * pAddParameter = (CMD_GP_AddParameter *)pData;
			ASSERT(pAddParameter!=NULL);

			//����ṹ
			DBR_GP_AddParameter AddParameter;
			AddParameter.wServerID = pAddParameter->wServerID;
			CopyMemory(&AddParameter.AndroidParameter,&pAddParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ADD_PARAMETER,dwSocketID,&AddParameter,sizeof(AddParameter));

			return true;
		}
	case SUB_GP_MODIFY_PARAMETER:	//�޸Ĳ���
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyParameter));
			if(wDataSize!=sizeof(CMD_GP_ModifyParameter)) return false;

			//��ȡ����
			CMD_GP_ModifyParameter * pModifyParameter = (CMD_GP_ModifyParameter *)pData;
			ASSERT(pModifyParameter!=NULL);

			//����ṹ
			DBR_GP_ModifyParameter ModifyParameter;
			ModifyParameter.wServerID=pModifyParameter->wServerID;
			CopyMemory(&ModifyParameter.AndroidParameter,&pModifyParameter->AndroidParameter,sizeof(tagAndroidParameter));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_PARAMETER,dwSocketID,&ModifyParameter,sizeof(ModifyParameter));
			
			return true;
		}
	case SUB_GP_DELETE_PARAMETER:	//ɾ������
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_DeleteParameter));
			if(wDataSize!=sizeof(CMD_GP_DeleteParameter)) return false;

			//��ȡ����
			CMD_GP_DeleteParameter * pDeleteParameter = (CMD_GP_DeleteParameter *)pData;
			ASSERT(pDeleteParameter!=NULL);

			//����ṹ
			DBR_GP_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID = pDeleteParameter->wServerID;
			DeleteParameter.dwBatchID = pDeleteParameter->dwBatchID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_PARAMETER,dwSocketID,&DeleteParameter,sizeof(DeleteParameter));
			
			return true;
		}
	}

	return false;
}

bool CAttemperEngineSink::OnTCPNetworkMainPCProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_PROPERTY:			//���ص���
		{

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_PROPERTY,SUB_GP_QUERY_PROPERTY_RESULT_FINISH);
			return true;
		}
	case SUB_GP_PROPERTY_BUY:	        //���߹���
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_PropertyBuy));
			if (wDataSize!=sizeof(CMD_GP_PropertyBuy)) return false;

			//��������
			CMD_GP_PropertyBuy * pPropertyBuy=(CMD_GP_PropertyBuy *)pData;

			//��������
			DBR_GP_PropertyBuy PropertyRequest;
			ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));

			//������Ϣ
			PropertyRequest.dwUserID=pPropertyBuy->dwUserID;
			PropertyRequest.dwPropertyID=pPropertyBuy->dwPropertyID;
			PropertyRequest.dwItemCount=pPropertyBuy->dwItemCount;
			PropertyRequest.cbConsumeType = pPropertyBuy->cbConsumeType;

			//ϵͳ��Ϣ
			PropertyRequest.dwClientAddr=(m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(PropertyRequest.szPassword,pPropertyBuy->szPassword,CountArray(PropertyRequest.szPassword));
			lstrcpyn(PropertyRequest.szMachineID,pPropertyBuy->szMachineID,CountArray(PropertyRequest.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_PROPERTY_BUY,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));

			return true;
		}
	case SUB_GP_PROPERTY_USE:			//����ʹ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_PropertyUse));
			if (wDataSize!=sizeof(CMD_GP_C_PropertyUse)) return false;

			//��������
			CMD_GP_C_PropertyUse * pPropertyUse = (CMD_GP_C_PropertyUse *)pData;

			//����Ч��
			ASSERT(pPropertyUse->wPropCount>0);
			if (pPropertyUse->wPropCount==0) return false;

			//��������
			DBR_GP_PropertyUse PropertyUseRequest;
			ZeroMemory(&PropertyUseRequest,sizeof(PropertyUseRequest));
			PropertyUseRequest.dwUserID = pPropertyUse->dwUserID;
			PropertyUseRequest.dwRecvUserID = pPropertyUse->dwRecvUserID;
			PropertyUseRequest.wPropCount = pPropertyUse->wPropCount;
			PropertyUseRequest.dwPropID = pPropertyUse->dwPropID;
			PropertyUseRequest.dwClientAddr=(m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr; //ϵͳ��Ϣ

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_PROPERTY_USE, dwSocketID, &PropertyUseRequest, sizeof(PropertyUseRequest));

			return true;
		}
	case SUB_GP_QUERY_SINGLE:
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_PropertyQuerySingle));
			if (wDataSize!=sizeof(CMD_GP_PropertyQuerySingle)) return false;

			CMD_GP_PropertyQuerySingle* pQuerySingle = (CMD_GP_PropertyQuerySingle*)pData;

			DBR_GP_PropertyQuerySingle QuerySingle;
			QuerySingle.dwUserID = pQuerySingle->dwUserID;
			QuerySingle.dwPropertyID = pQuerySingle->dwPropertyID;
			lstrcpyn(QuerySingle.szPassword,pQuerySingle->szPassword,CountArray(QuerySingle.szPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_SINGLE,dwSocketID,&QuerySingle,sizeof(QuerySingle));

			return true;
		}
	case SUB_GP_QUERY_BACKPACKET:  //��ѯ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_BackpackProperty));
			if (wDataSize!=sizeof(CMD_GP_C_BackpackProperty)) return false;

			CMD_GP_C_BackpackProperty* pBackpackProperty = (CMD_GP_C_BackpackProperty*)pData;

			DBR_GP_QueryBackpack QueryBackpack;
			QueryBackpack.dwUserID = pBackpackProperty->dwUserID;
			QueryBackpack.dwKindID = 0;
			QueryBackpack.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_BACKPACK,dwSocketID,&QueryBackpack,sizeof(QueryBackpack));
			return true;
		}
	case SUB_GP_PROPERTY_BUFF:			//����Buff
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_UserPropertyBuff));
			if (wDataSize!=sizeof(CMD_GP_C_UserPropertyBuff)) return false;

			//��������
			CMD_GP_C_UserPropertyBuff * pPropertyBuff = (CMD_GP_C_UserPropertyBuff *)pData;

			//��������
			DBR_GP_UserPropertyBuff PropertyBuffRequest;
			ZeroMemory(&PropertyBuffRequest,sizeof(PropertyBuffRequest));
			PropertyBuffRequest.dwUserID = pPropertyBuff->dwUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_PROPERTY_BUFF, dwSocketID, &PropertyBuffRequest, sizeof(PropertyBuffRequest));

			return true;
		}
	case SUB_GP_QUERY_SEND_PRESENT:		//��ѯ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QuerySendPresent));
			if (wDataSize!=sizeof(CMD_GP_QuerySendPresent)) return false;

			//��������
			CMD_GP_QuerySendPresent *pQuerySendPresent = (CMD_GP_QuerySendPresent *)pData;
			DBR_GP_QuerySendPresent QuerySendPresentRequest={0};
			QuerySendPresentRequest.dwUserID = pQuerySendPresent->dwUserID;
			QuerySendPresentRequest.dwClientAddr=(m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr; //ϵͳ��Ϣ

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_SEND_PRESENT, dwSocketID, &QuerySendPresentRequest, sizeof(QuerySendPresentRequest));
			return true;
		}
	case SUB_GP_PROPERTY_PRESENT:		//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_PropertyPresent));
			if (wDataSize!=sizeof(CMD_GP_C_PropertyPresent)) return false;

			//��������
			CMD_GP_C_PropertyPresent *PropertyPresent = (CMD_GP_C_PropertyPresent *)pData;
			DBR_GP_PropertyPresent PropertyPresentRequest={0};
			PropertyPresentRequest.dwUserID = PropertyPresent->dwUserID;
			PropertyPresentRequest.dwRecvGameID = PropertyPresent->dwRecvGameID;
			PropertyPresentRequest.dwPropID = PropertyPresent->dwPropID;
			PropertyPresentRequest.wPropCount = PropertyPresent->wPropCount;
			PropertyPresentRequest.wType = PropertyPresent->wType;
			lstrcpyn(PropertyPresentRequest.szRecvNickName,PropertyPresent->szRecvNickName,CountArray(PropertyPresentRequest.szRecvNickName));
			PropertyPresentRequest.dwClientAddr=(m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr; //ϵͳ��Ϣ

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_PROPERTY_PRESENT, dwSocketID, &PropertyPresentRequest, sizeof(PropertyPresentRequest));
			return true;
		}
	case SUB_GP_GET_SEND_PRESENT:		//�������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_GetSendPresent));
			if (wDataSize!=sizeof(CMD_GP_C_GetSendPresent)) return false;

			//��������
			CMD_GP_C_GetSendPresent *pGetSendPresent = (CMD_GP_C_GetSendPresent *)pData;
			DBR_GP_GetSendPresent GetSendPresentRequest={0};
			GetSendPresentRequest.dwUserID = pGetSendPresent->dwUserID;
			lstrcpyn(GetSendPresentRequest.szPassword,pGetSendPresent->szPassword,CountArray(GetSendPresentRequest.szPassword));
			GetSendPresentRequest.dwClientAddr=(m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr; //ϵͳ��Ϣ

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_GET_SEND_PRESENT, dwSocketID, &GetSendPresentRequest, sizeof(GetSendPresentRequest));
			return true;
		}
	default:
		ASSERT(false);
		break;
	}
	return false;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LOGON_GAMEID:		//I D ��¼
		{
			return OnTCPNetworkSubMBLogonGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubMBLogonAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_REGISTER_ACCOUNTS:	//�ʺ�ע��
		{
			return OnTCPNetworkSubMBRegisterAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_PHONE_REGISTER_VERIFYCODE:	//�ֻ���ע��
		{
			return OnTCPNetworkSubMBPhoneRegisterVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_PHONE_GET_REGISTER_VERIFYCODE://��ȡ��֤��
		{
			return OnTCPNetworkSubMBGetRegisterVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_GET_MODIFY_PASS_VERIFYCODE://��ȡ����������֤��
		{
			return OnTCPNetworkSubMBGetModifyPassVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_GET_MODIFY_BANKINFO_VERIFYCODE://��ȡ�޸�������Ϣ��֤��
		{
			return OnTCPNetworkSubMBGetModifyBankInfoVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_MODIFY_PASS_VERIFYCODE://�޸�����
		{
			return OnTCPNetworkSubMBModifyPassVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_MODIFY_BANKINFO_VERIFYCODE://�޸�������Ϣ
		{
			return OnTCPNetworkSubMBModifyBankInfoVerifyCode(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_QUERY_USERDATA://��ѯ�û���Ϣ
		{
			return OnTCPNetworkSubMBQueryUserData(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_LOGON_OTHERPLATFORM: //����ƽ̨
		{
			return OnTCPNetworkSubMBLogonOtherPlatform(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_LOGON_VISITOR:      //�ο͵�¼
		{
			return OnTCPNetworkSubMBLogonVisitor(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_TOUSU_AGENT:		//Ͷ�ߴ���
		{
			return OnTCPNetworkSubMBTouSuAgent(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_SHENGQING_AGENT_CHECK:		//�������ǰ����Ҫ����
		{
			return OnTCPNetworkSubMBShenQingAgentCheck(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_SHENGQING_AGENT:			//�������
		{
			return OnTCPNetworkSubMBShenQingAgent(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_QUERY_GAMELOCKINFO:
		{
			return OnTCPNetworkSubMBQueryGameLockInfo(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_MB_GET_ONLINE:
		{
			return OnTCPNetworkSubMBGetOnline(pData,wDataSize,dwSocketID);
		}
	case SUB_QUEREY_SERVER:
		{
			return OnTCPNetworkSubMBQueryServer(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainMBPersonalService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_MB_QUERY_GAME_SERVER:
		{
			return OnTCPNetworkSubMBQueryGameServer(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_SEARCH_SERVER_TABLE:
		{
			return OnTCPNetworkSubMBSearchServerTable(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_DISSUME_SEARCH_SERVER_TABLE:
		{
			return OnTCPNetworkSubMBDissumeSearchServerTable(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_GET_PERSONAL_PARAMETER:
		{
			return OnTCPNetworkSubMBPersonalParameter(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_QUERY_PERSONAL_ROOM_LIST:
		{
			return OnTCPNetworkSubMBQueryPersonalRoomList(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_QUERY_ROOM_SCORE: //���󷿼�ɼ�
		{
			return OnTCPNetworkSubQueryUserRoomScore(pData, wDataSize, dwSocketID);
		}
	case SUB_MB_QUERY_PERSONAL_ROOM_USER_INFO://������ҵ���Ϸ��
		{
			return OnTCPNetworkSubQueryPersonalRoomUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_MB_ROOM_CARD_EXCHANGE_TO_SCORE://������ҵ���Ϸ��
		{
			return OnTCPNetworkSubRoomCardExchangeToScore(pData, wDataSize, dwSocketID);
		}
	}
	return true;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonGameID));
	if (wDataSize<sizeof(CMD_GP_LogonGameID))
	{
		if (wDataSize<sizeof(CMD_GP_LogonGameID)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonGameID * pLogonGameID=(CMD_GP_LogonGameID *)pData;
	pLogonGameID->szPassword[CountArray(pLogonGameID->szPassword)-1]=0;
	pLogonGameID->szMachineID[CountArray(pLogonGameID->szMachineID)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonGameID->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonGameID->dwPlazaVersion,dwSocketID,((pLogonGameID->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonGameID LogonGameID;
	ZeroMemory(&LogonGameID,sizeof(LogonGameID));

	//������Ϣ
	LogonGameID.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonGameID.dwGameID=pLogonGameID->dwGameID;
	LogonGameID.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonGameID.szPassword,pLogonGameID->szPassword,CountArray(LogonGameID.szPassword));
	lstrcpyn(LogonGameID.szMachineID,pLogonGameID->szMachineID,CountArray(LogonGameID.szMachineID));
	LogonGameID.cbNeeValidateMBCard=(pLogonGameID->cbValidateFlags&MB_VALIDATE_FLAGS);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_GAMEID,dwSocketID,&LogonGameID,sizeof(LogonGameID));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GP_LogonAccounts));
	if (wDataSize>sizeof(CMD_GP_LogonAccounts)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szPassPortID[CountArray(pLogonAccounts->szPassPortID)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonAccounts->dwPlazaVersion,dwSocketID,((pLogonAccounts->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonAccounts.dwCheckUserRight=0;
	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	LogonAccounts.cbNeeValidateMBCard=(pLogonAccounts->cbValidateFlags&MB_VALIDATE_FLAGS);
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));	
	lstrcpyn(LogonAccounts.szPassPortID,pLogonAccounts->szPassPortID,CountArray(LogonAccounts.szPassPortID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//���ߵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonAccounts))
	{
		if (wDataSize<sizeof(CMD_GP_LogonAccounts)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonAccounts->dwPlazaVersion,dwSocketID,((pLogonAccounts->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonAccounts.dwCheckUserRight=UR_CAN_MANAGER_ANDROID;
	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));
	LogonAccounts.cbNeeValidateMBCard=(pLogonAccounts->cbValidateFlags&MB_VALIDATE_FLAGS);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts));
	if (wDataSize<sizeof(CMD_GP_RegisterAccounts))
	{
		if (wDataSize<sizeof(CMD_GP_RegisterAccounts)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;
	pRegisterAccounts->szPassPortID[CountArray(pRegisterAccounts->szPassPortID)-1]=0;
	pRegisterAccounts->szCompellation[CountArray(pRegisterAccounts->szCompellation)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pRegisterAccounts->dwPlazaVersion,dwSocketID,((pRegisterAccounts->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;
	RegisterAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(RegisterAccounts.szAgentID,pRegisterAccounts->szAgentID,CountArray(RegisterAccounts.szAgentID));
	lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,CountArray(RegisterAccounts.szAccounts));
	lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,CountArray(RegisterAccounts.szNickName));
	lstrcpyn(RegisterAccounts.szMachineID,pRegisterAccounts->szMachineID,CountArray(RegisterAccounts.szMachineID));
	lstrcpyn(RegisterAccounts.szLogonPass,pRegisterAccounts->szLogonPass,CountArray(RegisterAccounts.szLogonPass));
	lstrcpyn(RegisterAccounts.szPassPortID,pRegisterAccounts->szPassPortID,CountArray(RegisterAccounts.szPassPortID));
	lstrcpyn(RegisterAccounts.szCompellation,pRegisterAccounts->szCompellation,CountArray(RegisterAccounts.szCompellation));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//��֤����
bool CAttemperEngineSink::OnTCPNetworkSubPCVerifyIndividual(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_VerifyIndividual));
	if (wDataSize<sizeof(CMD_GP_VerifyIndividual)) return false;

	//������Ϣ
	CMD_GP_VerifyIndividual * pVerifyIndividual=(CMD_GP_VerifyIndividual *)pData;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pVerifyIndividual->dwPlazaVersion,dwSocketID,true)==false)
	{
		return true;
	}

	//��������
	DBR_GP_VerifyIndividual VerifyIndividual;
	ZeroMemory(&VerifyIndividual,sizeof(VerifyIndividual));

	//���ñ���
	VerifyIndividual.wVerifyMask=pVerifyIndividual->wVerifyMask;
	VerifyIndividual.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pVerifyIndividual+1,wDataSize-sizeof(CMD_GP_VerifyIndividual));

	//��չ��Ϣ
	while (true)
	{
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		if(DataDescribe.wDataDescribe==DTP_GP_UI_NICKNAME)	//�û��ǳ�
		{
			ASSERT(pDataBuffer!=NULL);
			ASSERT(DataDescribe.wDataSize<=sizeof(VerifyIndividual.szVerifyContent));
			if (DataDescribe.wDataSize<=sizeof(VerifyIndividual.szVerifyContent))
			{
				CopyMemory(&VerifyIndividual.szVerifyContent,pDataBuffer,DataDescribe.wDataSize);
				VerifyIndividual.szVerifyContent[CountArray(VerifyIndividual.szVerifyContent)-1]=0;
			}
			break;
		}

		if(DataDescribe.wDataDescribe==DTP_GP_UI_ACCOUNTS)	//�û��ʺ�
		{
			ASSERT(pDataBuffer!=NULL);
			ASSERT(DataDescribe.wDataSize<=sizeof(VerifyIndividual.szVerifyContent));
			if (DataDescribe.wDataSize<=sizeof(VerifyIndividual.szVerifyContent))
			{
				CopyMemory(&VerifyIndividual.szVerifyContent,pDataBuffer,DataDescribe.wDataSize);
				VerifyIndividual.szVerifyContent[CountArray(VerifyIndividual.szVerifyContent)-1]=0;
			}
			break;
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_VERIFY_INDIVIDUAL,dwSocketID,&VerifyIndividual,sizeof(VerifyIndividual));

	return true;
}

//�ο͵�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonVisitor));
	if (wDataSize<sizeof(CMD_GP_LogonVisitor))return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonVisitor * pLogonVisitor=(CMD_GP_LogonVisitor *)pData;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonVisitor->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonVisitor->dwPlazaVersion,dwSocketID,((pLogonVisitor->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor,sizeof(LogonVisitor));

	//������Ϣ
	LogonVisitor.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonVisitor.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	LogonVisitor.cbPlatformID=ULMByVisitor;
	lstrcpyn(LogonVisitor.szMachineID,pLogonVisitor->szMachineID,CountArray(LogonVisitor.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_VISITOR,dwSocketID,&LogonVisitor,sizeof(LogonVisitor));

	return true;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonGameID));
	if (wDataSize<sizeof(CMD_MB_LogonGameID)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonGameID * pLogonGameID=(CMD_MB_LogonGameID *)pData;
	pLogonGameID->szPassword[CountArray(pLogonGameID->szPassword)-1]=0;
	pLogonGameID->szMachineID[CountArray(pLogonGameID->szMachineID)-1]=0;
	pLogonGameID->szMobilePhone[CountArray(pLogonGameID->szMobilePhone)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonGameID->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonGameID->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pLogonGameID->cbDeviceType,pLogonGameID->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_LogonGameID LogonGameID;
	ZeroMemory(&LogonGameID,sizeof(LogonGameID));

	//������Ϣ
	LogonGameID.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonGameID.dwGameID=pLogonGameID->dwGameID;
	LogonGameID.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonGameID.szPassword,pLogonGameID->szPassword,CountArray(LogonGameID.szPassword));
	lstrcpyn(LogonGameID.szMachineID,pLogonGameID->szMachineID,CountArray(LogonGameID.szMachineID));
	lstrcpyn(LogonGameID.szMobilePhone,pLogonGameID->szMobilePhone,CountArray(LogonGameID.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_GAMEID,dwSocketID,&LogonGameID,sizeof(LogonGameID));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonAccounts));
	if (wDataSize<sizeof(CMD_MB_LogonAccounts)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonAccounts * pLogonAccounts=(CMD_MB_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szMobilePhone[CountArray(pLogonAccounts->szMobilePhone)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonAccounts->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonAccounts->cbDeviceType,pLogonAccounts->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));
	lstrcpyn(LogonAccounts.szMobilePhone,pLogonAccounts->szMobilePhone,CountArray(LogonAccounts.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//�ֻ��˺�ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBPhoneRegisterVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_PhoneRegisterAccounts));
	if (wDataSize<sizeof(CMD_MB_PhoneRegisterAccounts)) return false;


	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_PhoneRegisterAccounts * pClientData=(CMD_MB_PhoneRegisterAccounts *)pData;
	pClientData->szNickName[CountArray(pClientData->szNickName)-1]=0;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szLogonPass[CountArray(pClientData->szLogonPass)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;
	pClientData->szVerifyCode[CountArray(pClientData->szVerifyCode)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pClientData->wModuleID;
	pBindParameter->dwPlazaVersion=pClientData->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pClientData->cbDeviceType,pClientData->dwPlazaVersion,dwSocketID)==false) return true;

	//���Ҽ�У����֤��
	DWORD dwCurrentTime=(DWORD)time(NULL);
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (lstrcmp(pVerifyCode->szMobilePhone, pClientData->szMobilePhone) == 0 && lstrcmp(pVerifyCode->szValidateCode,pClientData->szVerifyCode)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (pVerifyCode->cbVerifyCodeType == VERIFY_CODE_TYPE_REGISTER)
			{
				if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
				{//����ʱ�� ��֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);

					CMD_GP_VerifyCodeResult VerifyCodeResult;
					ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

					VerifyCodeResult.cbResultCode=2;
					lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ѹ��ڣ������»�ȡ��"));

					//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					return true;
				}
				else
				{//��֤����Ч ����ע�ᣬ���Ƴ���֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);
					bFind = true;
					break;
				}
			}
			else
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������"));

				//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	if (!bFind)
	{
		//��������
		CMD_GP_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));
		DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

		//��������
		LogonFailure.lResultCode=pLogonFailure->lResultCode;
		lstrcpyn(LogonFailure.szDescribeString,_T("�ֻ���֤��ʧ�ܣ��������ԣ�"),CountArray(LogonFailure.szDescribeString));

		//��������
		WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_LOGON_FAILURE,&LogonFailure,wSendSize);

		//�ر�����
		m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
		return true;
	}

	//��������
	DBR_MB_PhoneRegisterAccounts PhoneRegisterAccounts;
	ZeroMemory(&PhoneRegisterAccounts,sizeof(PhoneRegisterAccounts));

	//������Ϣ
	PhoneRegisterAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	PhoneRegisterAccounts.cbDeviceType = pClientData->cbDeviceType;
	PhoneRegisterAccounts.wFaceID=pClientData->wFaceID;
	PhoneRegisterAccounts.cbGender=pClientData->cbGender;
	PhoneRegisterAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	PhoneRegisterAccounts.dwSpreaderGameID=pClientData->dwSpreaderGameID;
	lstrcpyn(PhoneRegisterAccounts.szAgentID,pClientData->szAgentID,CountArray(PhoneRegisterAccounts.szAgentID));
	lstrcpyn(PhoneRegisterAccounts.szNickName,pClientData->szNickName,CountArray(PhoneRegisterAccounts.szNickName));
	lstrcpyn(PhoneRegisterAccounts.szMachineID,pClientData->szMachineID,CountArray(PhoneRegisterAccounts.szMachineID));
	lstrcpyn(PhoneRegisterAccounts.szLogonPass,pClientData->szLogonPass,CountArray(PhoneRegisterAccounts.szLogonPass));
	lstrcpyn(PhoneRegisterAccounts.szMobilePhone,pClientData->szMobilePhone,CountArray(PhoneRegisterAccounts.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_PHONE_REGISTER_ACCOUNTS,dwSocketID,&PhoneRegisterAccounts,sizeof(PhoneRegisterAccounts));

	return true;
}

//�ֻ���ע�����ȡ
bool CAttemperEngineSink::OnTCPNetworkSubMBGetRegisterVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (strcmp(m_szHttpRegisterValidCode, "") == 0)
	{
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ��δ���Ź��ܣ�����ϵ�ͷ���"));

		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
		return true;
	}

	ASSERT(wDataSize==sizeof(CMD_MB_GetRegisterVerifyCode));
	if (wDataSize!=sizeof(CMD_MB_GetRegisterVerifyCode)) return false;

	CMD_MB_GetRegisterVerifyCode *pClientData=(CMD_MB_GetRegisterVerifyCode *)pData;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;

	DWORD dwCurrentTime=(DWORD)time(NULL);
	DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

	//�����Ƿ�������
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (pVerifyCode->dwClientAddr==dwClientAddr && lstrcmp(pVerifyCode->szMachineID,pClientData->szMachineID)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (dwCurrentTime-pVerifyCode->dwUpdateTime<=TIME_VALID_VERIFY_CODE)
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������Ƶ�������Ժ����ԣ�"));

				//֪ͨ�ͻ�����Ϣ �������Ƶ��
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}

			//��������֤�룬�������������µ���֤��
			char szMobile[LEN_MOBILE_PHONE] = "";

			//��ȡ����
			CString str(pClientData->szMobilePhone);
			int nLength = str.GetLength();
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

			char szUrlBuffer[MAX_PATH] = "";

			sprintf(szUrlBuffer, m_szHttpRegisterValidCode,  szMobile);

			m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetReBuildVerifyCodeCallBack, this, pVerifyCode);
			bFind = true;
			break;
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	//��������֤��
	if (bFind == false)
	{
		char szMobile[LEN_MOBILE_PHONE] = "";

		//��ȡ����
		CString str(pClientData->szMobilePhone);
		int nLength = str.GetLength();
		int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

		char szUrlBuffer[MAX_PATH] = "";

		sprintf(szUrlBuffer, m_szHttpRegisterValidCode,  szMobile);

		tagVerifyCode * pNewVerifyCode=new tagVerifyCode;
		//�ͻ���������
		pNewVerifyCode->dwSocketID = dwSocketID;
		//�ͻ��˵�ַ
		pNewVerifyCode->dwClientAddr=dwClientAddr;
		//��֤������
		pNewVerifyCode->cbVerifyCodeType = VERIFY_CODE_TYPE_REGISTER;
		//������
		lstrcpyn(pNewVerifyCode->szMachineID,pClientData->szMachineID,CountArray(pNewVerifyCode->szMachineID));
		//�ֻ���
		lstrcpyn(pNewVerifyCode->szMobilePhone,pClientData->szMobilePhone,CountArray(pNewVerifyCode->szMobilePhone));

		m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetVerifyCodeCallBack, this, pNewVerifyCode);
	}

	return true;
}

//�޸�������֤���ȡ
bool CAttemperEngineSink::OnTCPNetworkSubMBGetModifyPassVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (strcmp(m_szHttpModifyPassValidCode, "") == 0)
	{
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ��δ���Ź��ܣ�����ϵ�ͷ���"));

		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
		return true;
	}

	ASSERT(wDataSize==sizeof(CMD_MB_GetModifyPassVerifyCode));
	if (wDataSize!=sizeof(CMD_MB_GetModifyPassVerifyCode)) return false;

	CMD_MB_GetModifyPassVerifyCode *pClientData=(CMD_MB_GetModifyPassVerifyCode *)pData;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;

	DWORD dwCurrentTime=(DWORD)time(NULL);
	DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

	//�����Ƿ�������
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (pVerifyCode->dwClientAddr==dwClientAddr && lstrcmp(pVerifyCode->szMachineID,pClientData->szMachineID)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (dwCurrentTime-pVerifyCode->dwUpdateTime<=TIME_VALID_VERIFY_CODE)
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������Ƶ�������Ժ����ԣ�"));

				//֪ͨ�ͻ�����Ϣ �������Ƶ��
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}

			//��������֤�룬�������������µ���֤��
			char szMobile[LEN_MOBILE_PHONE] = "";

			//��ȡ����
			CString str(pClientData->szMobilePhone);
			int nLength = str.GetLength();
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

			char szUrlBuffer[MAX_PATH] = "";

			sprintf(szUrlBuffer, m_szHttpModifyPassValidCode,  szMobile);

			m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetReBuildVerifyCodeCallBack, this, pVerifyCode);
			bFind = true;
			break;
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	//��������֤��
	if (bFind == false)
	{
		char szMobile[LEN_MOBILE_PHONE] = "";

		//��ȡ����
		CString str(pClientData->szMobilePhone);
		int nLength = str.GetLength();
		int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

		char szUrlBuffer[MAX_PATH] = "";

		sprintf(szUrlBuffer, m_szHttpModifyPassValidCode,  szMobile);

		tagVerifyCode * pNewVerifyCode=new tagVerifyCode;
		//�ͻ���������
		pNewVerifyCode->dwSocketID = dwSocketID;
		//�ͻ��˵�ַ
		pNewVerifyCode->dwClientAddr=dwClientAddr;
		//��֤������
		pNewVerifyCode->cbVerifyCodeType = VERIFY_CODE_TYPE_MODIFYPASS;
		//������
		lstrcpyn(pNewVerifyCode->szMachineID,pClientData->szMachineID,CountArray(pNewVerifyCode->szMachineID));
		//�ֻ���
		lstrcpyn(pNewVerifyCode->szMobilePhone,pClientData->szMobilePhone,CountArray(pNewVerifyCode->szMobilePhone));

		m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetVerifyCodeCallBack, this, pNewVerifyCode);
	}

	return true;
}

//�޸�������Ϣ��֤���ȡ
bool CAttemperEngineSink::OnTCPNetworkSubMBGetModifyBankInfoVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (strcmp(m_szHttpModifyBankInfoValidCode, "") == 0)
	{
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ��δ���Ź��ܣ�����ϵ�ͷ���"));

		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
		return true;
	}

	ASSERT(wDataSize==sizeof(CMD_MB_GetModifyBankInfoVerifyCode));
	if (wDataSize!=sizeof(CMD_MB_GetModifyBankInfoVerifyCode)) return false;

	CMD_MB_GetModifyBankInfoVerifyCode *pClientData=(CMD_MB_GetModifyBankInfoVerifyCode *)pData;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;

	DWORD dwCurrentTime=(DWORD)time(NULL);
	DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

	//�����Ƿ�������
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (pVerifyCode->dwClientAddr==dwClientAddr && lstrcmp(pVerifyCode->szMachineID,pClientData->szMachineID)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (dwCurrentTime-pVerifyCode->dwUpdateTime<=TIME_VALID_VERIFY_CODE)
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������Ƶ�������Ժ����ԣ�"));

				//֪ͨ�ͻ�����Ϣ �������Ƶ��
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}

			//��������֤�룬�������������µ���֤��
			char szMobile[LEN_MOBILE_PHONE] = "";

			//��ȡ����
			CString str(pClientData->szMobilePhone);
			int nLength = str.GetLength();
			int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

			char szUrlBuffer[MAX_PATH] = "";

			sprintf(szUrlBuffer, m_szHttpModifyBankInfoValidCode,  szMobile);

			m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetReBuildVerifyCodeCallBack, this, pVerifyCode);
			bFind = true;
			break;
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	//��������֤��
	if (bFind == false)
	{
		char szMobile[LEN_MOBILE_PHONE] = "";

		//��ȡ����
		CString str(pClientData->szMobilePhone);
		int nLength = str.GetLength();
		int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_OEMCP, 0, str, nLength, szMobile, nBytes, NULL, NULL); 

		char szUrlBuffer[MAX_PATH] = "";

		sprintf(szUrlBuffer, m_szHttpModifyBankInfoValidCode,  szMobile);

		tagVerifyCode * pNewVerifyCode=new tagVerifyCode;
		//�ͻ���������
		pNewVerifyCode->dwSocketID = dwSocketID;
		//�ͻ��˵�ַ
		pNewVerifyCode->dwClientAddr=dwClientAddr;
		//��֤������
		pNewVerifyCode->cbVerifyCodeType = VERIFY_CODE_TYPE_MODIFYBANKINFO;
		//������
		lstrcpyn(pNewVerifyCode->szMachineID,pClientData->szMachineID,CountArray(pNewVerifyCode->szMachineID));
		//�ֻ���
		lstrcpyn(pNewVerifyCode->szMobilePhone,pClientData->szMobilePhone,CountArray(pNewVerifyCode->szMobilePhone));

		m_pHttpClientService->Get(szUrlBuffer, &CAttemperEngineSink::OnClientGetVerifyCodeCallBack, this, pNewVerifyCode);
	}

	return true;
}

//ͨ����֤���޸�����
bool CAttemperEngineSink::OnTCPNetworkSubMBModifyPassVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_ModifyPassVerifyCode));
	if (wDataSize<sizeof(CMD_MB_ModifyPassVerifyCode)) return false;


	//������Ϣ
	CMD_MB_ModifyPassVerifyCode * pClientData=(CMD_MB_ModifyPassVerifyCode *)pData;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szPassword[CountArray(pClientData->szPassword)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;
	pClientData->szVerifyCode[CountArray(pClientData->szVerifyCode)-1]=0;
	pClientData->szAccounts[CountArray(pClientData->szAccounts)-1]=0;

	//���Ҽ�У����֤��
	DWORD dwCurrentTime=(DWORD)time(NULL);
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (lstrcmp(pVerifyCode->szMobilePhone, pClientData->szMobilePhone) == 0 && lstrcmp(pVerifyCode->szValidateCode,pClientData->szVerifyCode)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (pVerifyCode->cbVerifyCodeType == VERIFY_CODE_TYPE_MODIFYPASS)
			{
				if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
				{//����ʱ�� ��֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);

					CMD_GP_VerifyCodeResult VerifyCodeResult;
					ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

					VerifyCodeResult.cbResultCode=2;
					lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ѹ��ڣ������»�ȡ��"));

					//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					return true;
				}
				else
				{//��֤����Ч ����ע�ᣬ���Ƴ���֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);
					bFind = true;
					break;
				}
			}
			else
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������"));

				//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	if (!bFind)
	{
		//��������
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ч��֤�룬�����ԣ�"));

		//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));

		//�ر�����
		m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
		return true;
	}

	//��������
	DBR_MB_ModifyPass DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	DBRData.cbMode = pClientData->cbMode;
	lstrcpyn(DBRData.szAccounts,pClientData->szAccounts,CountArray(DBRData.szAccounts));
	lstrcpyn(DBRData.szVerifyCode,pClientData->szVerifyCode,CountArray(DBRData.szVerifyCode));
	lstrcpyn(DBRData.szMachineID,pClientData->szMachineID,CountArray(DBRData.szMachineID));
	lstrcpyn(DBRData.szPassword,pClientData->szPassword,CountArray(DBRData.szPassword));
	lstrcpyn(DBRData.szMobilePhone,pClientData->szMobilePhone,CountArray(DBRData.szMobilePhone));

	//Ͷ������
	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_PASS,dwSocketID,&DBRData,sizeof(DBRData));
}

//ͨ����֤���޸�������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubMBModifyBankInfoVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_ModifyBankInfoVerifyCode));
	if (wDataSize<sizeof(CMD_MB_ModifyBankInfoVerifyCode)) return false;


	//������Ϣ
	CMD_MB_ModifyBankInfoVerifyCode * pClientData=(CMD_MB_ModifyBankInfoVerifyCode *)pData;
	pClientData->szMachineID[CountArray(pClientData->szMachineID)-1]=0;
	pClientData->szBankAddress[CountArray(pClientData->szBankAddress)-1]=0;
	pClientData->szBankName[CountArray(pClientData->szBankName)-1]=0;
	pClientData->szBankNo[CountArray(pClientData->szBankNo)-1]=0;
	pClientData->szMobilePhone[CountArray(pClientData->szMobilePhone)-1]=0;
	pClientData->szVerifyCode[CountArray(pClientData->szVerifyCode)-1]=0;

	//���Ҽ�У����֤��
	DWORD dwCurrentTime=(DWORD)time(NULL);
	bool bFind = false;
	for (INT_PTR i=0; i<m_VerifyCodeArray.GetCount();i++)
	{
		tagVerifyCode *pVerifyCode=m_VerifyCodeArray[i];
		if (pVerifyCode==NULL) continue;

		if (lstrcmp(pVerifyCode->szMobilePhone, pClientData->szMobilePhone) == 0 && lstrcmp(pVerifyCode->szValidateCode,pClientData->szVerifyCode)==0)
		{
			pVerifyCode->dwSocketID = dwSocketID;

			if (pVerifyCode->cbVerifyCodeType == VERIFY_CODE_TYPE_MODIFYBANKINFO)
			{
				if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
				{//����ʱ�� ��֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);

					CMD_GP_VerifyCodeResult VerifyCodeResult;
					ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

					VerifyCodeResult.cbResultCode=2;
					lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ����֤���ѹ��ڣ������»�ȡ��"));

					//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
					return true;
				}
				else
				{//��֤����Ч ����ע�ᣬ���Ƴ���֤��
					SafeDelete(pVerifyCode);
					m_VerifyCodeArray.RemoveAt(i);
					bFind = true;
					break;
				}
			}
			else
			{
				CMD_GP_VerifyCodeResult VerifyCodeResult;
				ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

				VerifyCodeResult.cbResultCode=2;
				lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ǹ������������"));

				//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));
				return true;
			}
		}
		else if (dwCurrentTime-pVerifyCode->dwUpdateTime>TIME_VALID_VERIFY_CODE)
		{//����������ʱ�� ��֤��
			SafeDelete(pVerifyCode);
			m_VerifyCodeArray.RemoveAt(i);
		}
	}

	if (!bFind)
	{
		//��������
		CMD_GP_VerifyCodeResult VerifyCodeResult;
		ZeroMemory(&VerifyCodeResult,sizeof(VerifyCodeResult));

		VerifyCodeResult.cbResultCode=2;
		lstrcpy(VerifyCodeResult.szDescString,TEXT("��Ч��֤�룬�����ԣ�"));

		//֪ͨ�ͻ�����Ϣ ��֤���ѹ���
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_VERIFY_CODE_RESULT,&VerifyCodeResult,sizeof(VerifyCodeResult));

		//�ر�����
		m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
		return true;
	}

	//��������
	DBR_MB_ModifyBankInfo DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	DBRData.dwUserID = pClientData->dwUserID;
	lstrcpyn(DBRData.szBankAddress,pClientData->szBankAddress,CountArray(DBRData.szBankAddress));
	lstrcpyn(DBRData.szBankName,pClientData->szBankName,CountArray(DBRData.szBankName));
	lstrcpyn(DBRData.szBankNo,pClientData->szBankNo,CountArray(DBRData.szBankNo));
	lstrcpyn(DBRData.szMachineID,pClientData->szMachineID,CountArray(DBRData.szMachineID));

	//Ͷ������
	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_BANKINFO,dwSocketID,&DBRData,sizeof(DBRData));
}

bool CAttemperEngineSink::OnTCPNetworkSubMBQueryUserData(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_QueryUserData));
	if (wDataSize<sizeof(CMD_MB_QueryUserData)) return false;


	//������Ϣ
	CMD_MB_QueryUserData * pClientData=(CMD_MB_QueryUserData *)pData;
	pClientData->szWhere[CountArray(pClientData->szWhere)-1]=0;

	//��������
	DBR_MB_QueryUserData DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.cbType = pClientData->cbType;
	lstrcpyn(DBRData.szWhere,pClientData->szWhere,CountArray(DBRData.szWhere));

	//Ͷ������
	return m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_USERDATA,dwSocketID,&DBRData,sizeof(DBRData));
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_RegisterAccounts));
	if (wDataSize<sizeof(CMD_MB_RegisterAccounts)) return false;


	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_RegisterAccounts * pRegisterAccounts=(CMD_MB_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;
	pRegisterAccounts->szMobilePhone[CountArray(pRegisterAccounts->szMobilePhone)-1]=0;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pRegisterAccounts->wModuleID;
	pBindParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pRegisterAccounts->cbDeviceType,pRegisterAccounts->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	RegisterAccounts.cbDeviceType = pRegisterAccounts->cbDeviceType;
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;
	RegisterAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	RegisterAccounts.dwSpreaderGameID=pRegisterAccounts->dwSpreaderGameID;
	lstrcpyn(RegisterAccounts.szAgentID,pRegisterAccounts->szAgentID,CountArray(RegisterAccounts.szAgentID));
	lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,CountArray(RegisterAccounts.szAccounts));
	lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,CountArray(RegisterAccounts.szNickName));
	lstrcpyn(RegisterAccounts.szMachineID,pRegisterAccounts->szMachineID,CountArray(RegisterAccounts.szMachineID));
	lstrcpyn(RegisterAccounts.szLogonPass,pRegisterAccounts->szLogonPass,CountArray(RegisterAccounts.szLogonPass));
	lstrcpyn(RegisterAccounts.szMobilePhone,pRegisterAccounts->szMobilePhone,CountArray(RegisterAccounts.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_ACCOUNTS,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//������¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonOtherPlatform(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonOtherPlatform));
	if (wDataSize<sizeof(CMD_MB_LogonOtherPlatform)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonOtherPlatform * pLogonOtherPlatform=(CMD_MB_LogonOtherPlatform *)pData;
	pLogonOtherPlatform->szUserUin[CountArray(pLogonOtherPlatform->szUserUin)-1]=0;
	pLogonOtherPlatform->szNickName[CountArray(pLogonOtherPlatform->szNickName)-1]=0;
	pLogonOtherPlatform->szMachineID[CountArray(pLogonOtherPlatform->szMachineID)-1]=0;
	pLogonOtherPlatform->szMobilePhone[CountArray(pLogonOtherPlatform->szMobilePhone)-1]=0;
	pLogonOtherPlatform->szCompellation[CountArray(pLogonOtherPlatform->szCompellation)-1]=0;

	//ƽ̨�ж�
	ASSERT(pLogonOtherPlatform->cbPlatformID==ULMBySina || pLogonOtherPlatform->cbPlatformID==ULMByTencent || pLogonOtherPlatform->cbPlatformID==ULMByRenRen);
	if (pLogonOtherPlatform->cbPlatformID!=ULMBySina && pLogonOtherPlatform->cbPlatformID!=ULMByTencent && pLogonOtherPlatform->cbPlatformID!=ULMByRenRen) return false;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonOtherPlatform->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonOtherPlatform->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonOtherPlatform->cbDeviceType,pLogonOtherPlatform->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_LogonOtherPlatform LogonOtherPlatform;
	ZeroMemory(&LogonOtherPlatform,sizeof(LogonOtherPlatform));

	//������Ϣ
	LogonOtherPlatform.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonOtherPlatform.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	LogonOtherPlatform.cbGender=pLogonOtherPlatform->cbGender;
	LogonOtherPlatform.cbPlatformID=pLogonOtherPlatform->cbPlatformID;
	lstrcpyn(LogonOtherPlatform.szAgentID,pLogonOtherPlatform->szAgentID,CountArray(LogonOtherPlatform.szAgentID));
	lstrcpyn(LogonOtherPlatform.szUserUin,pLogonOtherPlatform->szUserUin,CountArray(LogonOtherPlatform.szUserUin));
	lstrcpyn(LogonOtherPlatform.szNickName,pLogonOtherPlatform->szNickName,CountArray(LogonOtherPlatform.szNickName));
	lstrcpyn(LogonOtherPlatform.szMachineID,pLogonOtherPlatform->szMachineID,CountArray(LogonOtherPlatform.szMachineID));
	lstrcpyn(LogonOtherPlatform.szMobilePhone,pLogonOtherPlatform->szMobilePhone,CountArray(LogonOtherPlatform.szMobilePhone));
	lstrcpyn(LogonOtherPlatform.szCompellation,pLogonOtherPlatform->szCompellation,CountArray(LogonOtherPlatform.szCompellation));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_OTHERPLATFORM,dwSocketID,&LogonOtherPlatform,sizeof(LogonOtherPlatform));

	return true;
}

//�ο͵�¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonVisitor));
	if (wDataSize<sizeof(CMD_MB_LogonVisitor))return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonVisitor * pLogonVisitor=(CMD_MB_LogonVisitor *)pData;

	//��������
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonVisitor->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonVisitor->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonVisitor->cbDeviceType,pLogonVisitor->dwPlazaVersion,dwSocketID)==false)
	{
		return true;
	}

	//��������
	DBR_MB_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor,sizeof(LogonVisitor));

	//������Ϣ
	LogonVisitor.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonVisitor.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	LogonVisitor.cbPlatformID=ULMByVisitor;
	lstrcpyn(LogonVisitor.szAgentID,pLogonVisitor->szAgentID,CountArray(LogonVisitor.szAgentID));
	lstrcpyn(LogonVisitor.szMachineID,pLogonVisitor->szMachineID,CountArray(LogonVisitor.szMachineID));
	lstrcpyn(LogonVisitor.szMobilePhone,pLogonVisitor->szMobilePhone,CountArray(LogonVisitor.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_VISITOR,dwSocketID,&LogonVisitor,sizeof(LogonVisitor));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubMBTouSuAgent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_TouSuDaili));
	if (wDataSize<sizeof(CMD_MB_TouSuDaili))return false;

	//������Ϣ
	CMD_MB_TouSuDaili * pClientData=(CMD_MB_TouSuDaili *)pData;

	//��������
	DBR_MB_TouSuDaili DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwUserID = pClientData->dwUserID;
	lstrcpyn(DBRData.szUserWx,pClientData->szUserWx,CountArray(DBRData.szUserWx));
	lstrcpyn(DBRData.szAgentWx,pClientData->szAgentWx,CountArray(DBRData.szAgentWx));
	lstrcpyn(DBRData.szReason,pClientData->szReason,CountArray(DBRData.szReason));
	lstrcpyn(DBRData.szImageName,pClientData->szImageName,CountArray(DBRData.szImageName));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_TOUSU_AGENT,dwSocketID,&DBRData,sizeof(DBRData));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubMBShenQingAgentCheck(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_ShenQingDailiCheck));
	if (wDataSize<sizeof(CMD_MB_ShenQingDailiCheck))return false;

	//������Ϣ
	CMD_MB_ShenQingDailiCheck * pClientData=(CMD_MB_ShenQingDailiCheck *)pData;

	//��������
	DBR_MB_ShenQingDailiCheck DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwUserID = pClientData->dwUserID;
	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SHENQING_AGENT_CHECK,dwSocketID,&DBRData,sizeof(DBRData));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubMBShenQingAgent(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_ShenQingDaili));
	if (wDataSize<sizeof(CMD_MB_ShenQingDaili))return false;

	//������Ϣ
	CMD_MB_ShenQingDaili * pClientData=(CMD_MB_ShenQingDaili *)pData;

	//��������
	DBR_MB_ShenQingDaili DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwUserID = pClientData->dwUserID;
	lstrcpyn(DBRData.szAgentAcc,pClientData->szAgentAcc,CountArray(DBRData.szAgentAcc));
	lstrcpyn(DBRData.szAgentName,pClientData->szAgentName,CountArray(DBRData.szAgentName));
	lstrcpyn(DBRData.szAlipay,pClientData->szAlipay,CountArray(DBRData.szAlipay));
	lstrcpyn(DBRData.szWeChat,pClientData->szWeChat,CountArray(DBRData.szWeChat));
	lstrcpyn(DBRData.szQQ,pClientData->szQQ,CountArray(DBRData.szQQ));
	lstrcpyn(DBRData.szNote,pClientData->szNote,CountArray(DBRData.szNote));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SHENQING_AGENT,dwSocketID,&DBRData,sizeof(DBRData));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubMBQueryGameLockInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_QueryGameLockInfo));
	if (wDataSize<sizeof(CMD_MB_QueryGameLockInfo))return false;

	//������Ϣ
	CMD_MB_QueryGameLockInfo * pClientData=(CMD_MB_QueryGameLockInfo *)pData;

	//��������
	DBR_MB_QueryGameLockInfo DBRData;
	ZeroMemory(&DBRData,sizeof(DBRData));

	//��������
	DBRData.dwUserID = pClientData->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_GAMELOCKINFO,dwSocketID,&DBRData,sizeof(DBRData));

	return true;
}

//��ȡ��������
bool CAttemperEngineSink::OnTCPNetworkSubMBGetOnline(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_MB_GetOnline * pGetOnline=(CMD_MB_GetOnline *)pData;
	WORD wHeadSize=(sizeof(CMD_MB_GetOnline)-sizeof(pGetOnline->wOnLineServerID));

	//Ч������
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGetOnline->wServerCount*sizeof(WORD))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGetOnline->wServerCount*sizeof(WORD)))) return false;

	//��������
	CMD_MB_KindOnline KindOnline;
	CMD_MB_ServerOnline ServerOnline;
	ZeroMemory(&KindOnline,sizeof(KindOnline));
	ZeroMemory(&ServerOnline,sizeof(ServerOnline));

	//��ȡ����
	POSITION KindPosition=NULL;
	do
	{
		//��ȡ����
		CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

		//���ñ���
		if (pGameKindItem!=NULL)
		{
			WORD wKindIndex=KindOnline.wKindCount++;
			KindOnline.OnLineInfoKind[wKindIndex].wKindID=pGameKindItem->m_GameKind.wKindID;
			KindOnline.OnLineInfoKind[wKindIndex].dwOnLineCount=pGameKindItem->m_GameKind.dwOnLineCount+pGameKindItem->m_GameKind.dwAndroidCount;
		}

		//����ж�
		if (KindOnline.wKindCount>=CountArray(KindOnline.OnLineInfoKind))
		{
			ASSERT(FALSE);
			break;
		}

	} while (KindPosition!=NULL);

	//��ȡ����
	for (WORD i=0;i<pGetOnline->wServerCount;i++)
	{
		//��ȡ����
		WORD wServerID=pGetOnline->wOnLineServerID[i];
		CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(wServerID);

		//���ñ���
		if (pGameServerItem!=NULL)
		{
			WORD wServerIndex=ServerOnline.wServerCount++;
			ServerOnline.OnLineInfoServer[wServerIndex].wServerID=wServerID;
			ServerOnline.OnLineInfoServer[wServerIndex].dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount+pGameServerItem->m_GameServer.dwAndroidCount;
		}

		//����ж�
		if (ServerOnline.wServerCount>=CountArray(ServerOnline.OnLineInfoServer))
		{
			ASSERT(FALSE);
			break;
		}
	}

	//��������
	if (KindOnline.wKindCount>0)
	{
		WORD wHeadSize=sizeof(KindOnline)-sizeof(KindOnline.OnLineInfoKind);
		WORD wSendSize=wHeadSize+KindOnline.wKindCount*sizeof(KindOnline.OnLineInfoKind[0]);
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_KINE_ONLINE,&KindOnline,wSendSize);
	}

	//��������
	if (ServerOnline.wServerCount>0)
	{
		WORD wHeadSize=sizeof(ServerOnline)-sizeof(ServerOnline.OnLineInfoServer);
		WORD wSendSize=wHeadSize+ServerOnline.wServerCount*sizeof(ServerOnline.OnLineInfoServer[0]);
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_SERVER_ONLINE,&ServerOnline,wSendSize);
	}

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubMBQueryServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_MB_QueryServer));
	if (wDataSize!=sizeof(CMD_MB_QueryServer))return false;
	CMD_MB_QueryServer * pClientData = (CMD_MB_QueryServer*)pData;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER_START,0,0);
	SendMobileServerInfo(dwSocketID, pClientData->wKindID);
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER_END,0,0);
	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubMBQueryGameServer(pData, wDataSize, dwSocketID, m_pBindParameter, m_pITCPSocketService);
	}


	return true;
}

//������������
bool CAttemperEngineSink::OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubMBSearchServerTable(pData, wDataSize, dwSocketID, m_pBindParameter, m_pITCPSocketService);
	}

	////У������
	//ASSERT(wDataSize == sizeof(CMD_MB_SearchServerTable));
	//if(wDataSize != sizeof(CMD_MB_SearchServerTable)) return false;


	return true;
}

//��ɢ������������
bool CAttemperEngineSink::OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubMBDissumeSearchServerTable(pData, wDataSize, dwSocketID, m_pBindParameter, m_pITCPSocketService);
	}

	return true;
}

//˽�˷�������
bool CAttemperEngineSink::OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubMBPersonalParameter(pData, wDataSize, dwSocketID, m_pIDataBaseEngine);
	}

	////У������
	//ASSERT(wDataSize == sizeof(CMD_MB_GetPersonalParameter));
	//if(wDataSize != sizeof(CMD_MB_GetPersonalParameter)) return false;

	//CMD_MB_GetPersonalParameter* pGetPersonalParameter = (CMD_MB_GetPersonalParameter*)pData;



	////��������
	//DBR_MB_GetPersonalParameter GetPersonalParameter;
	//ZeroMemory(&GetPersonalParameter, sizeof(DBR_MB_GetPersonalParameter));
	//GetPersonalParameter.dwServerID = pGetPersonalParameter->dwServerID;

	////Ͷ������
	//m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_GET_PERSONAL_PARAMETER,dwSocketID, &GetPersonalParameter, sizeof(DBR_MB_GetPersonalParameter));

	return true;
}

//��ѯ˽�˷����б�
bool	CAttemperEngineSink::OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubMBQueryPersonalRoomList(pData, wDataSize, dwSocketID, m_pITCPSocketService);
	}

	return true;
}

//������󷿼�ɼ�
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (m_pIPersonalRoomServiceManager)
	{
		return m_pIPersonalRoomServiceManager->OnTCPNetworkSubQueryUserRoomScore(pData, wDataSize, dwSocketID, m_pIDataBaseEngine);
	}
	return true;
}

//������󷿼�ɼ�
bool CAttemperEngineSink::OnTCPNetworkSubQueryPersonalRoomUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	OutputDebugString(TEXT("ptdt *** OnTCPNetworkSubQueryPersonalRoomUserInfo  1"));
	//У������
	ASSERT(wDataSize == sizeof(CMD_MB_QueryPersonalRoomUserInfo));
	if(wDataSize != sizeof(CMD_MB_QueryPersonalRoomUserInfo)) return false;

	//��ȡ����
	CMD_MB_QueryPersonalRoomUserInfo * pQueryUserInfo = (CMD_MB_QueryPersonalRoomUserInfo*)pData;
	ASSERT(pQueryUserInfo!=NULL);

	//��������
	DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO USER_INFO;

	USER_INFO.dwUserID = pQueryUserInfo->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_PERSONAL_ROOM_USER_INFO,dwSocketID, &USER_INFO, sizeof(DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO));

	return true;
}



//������󷿼�ɼ�
bool CAttemperEngineSink::OnTCPNetworkSubRoomCardExchangeToScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_GP_ExchangeScoreByRoomCard));
	if(wDataSize != sizeof(CMD_GP_ExchangeScoreByRoomCard)) return false;


	//��������
	//����ṹ
	CMD_GP_ExchangeScoreByRoomCard * pExchangeScore = (CMD_GP_ExchangeScoreByRoomCard*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//����ṹ
	DBR_GP_ExchangeScoreByRoomCard ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//���ñ���
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.lExchangeRoomCard=pExchangeScore->lRoomCard;
	ExchangeScore.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_ROOM_CARD_EXCHANGE_TO_SCORE,dwSocketID, &ExchangeScore, sizeof(DBR_GP_ExchangeScoreByRoomCard));

	return true;
}



//��¼�ɹ�
bool CAttemperEngineSink::OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_GP_LogonSuccess * pDBOLogonSuccess=(DBO_GP_LogonSuccess *)pData;
	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)cbDataBuffer;

	//���Ͷ���
	WORD wHeadSize=sizeof(CMD_GP_LogonSuccess);
	CSendPacketHelper SendPacket(cbDataBuffer+wHeadSize,sizeof(cbDataBuffer)-wHeadSize);

	//���ñ���
	ZeroMemory(pCMDLogonSuccess,sizeof(CMD_GP_LogonSuccess));

	//��������
	pCMDLogonSuccess->wFaceID=pDBOLogonSuccess->wFaceID;
	pCMDLogonSuccess->cbGender=pDBOLogonSuccess->cbGender;
	pCMDLogonSuccess->dwGameID=pDBOLogonSuccess->dwGameID;
	pCMDLogonSuccess->dwUserID=pDBOLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwCustomID=pDBOLogonSuccess->dwCustomID;
	pCMDLogonSuccess->dwExperience=pDBOLogonSuccess->dwExperience;
	pCMDLogonSuccess->lLoveLiness=pDBOLogonSuccess->lLoveLiness;
	pCMDLogonSuccess->cbMoorMachine=pDBOLogonSuccess->cbMoorMachine;
	lstrcpyn(pCMDLogonSuccess->szAccounts,pDBOLogonSuccess->szAccounts,CountArray(pCMDLogonSuccess->szAccounts));
	lstrcpyn(pCMDLogonSuccess->szNickName,pDBOLogonSuccess->szNickName,CountArray(pCMDLogonSuccess->szNickName));
	lstrcpyn(pCMDLogonSuccess->szDynamicPass,pDBOLogonSuccess->szDynamicPass,CountArray(pCMDLogonSuccess->szDynamicPass));	

	//�û��ɼ�
	pCMDLogonSuccess->lUserScore=pDBOLogonSuccess->lUserScore;
	pCMDLogonSuccess->lUserIngot=pDBOLogonSuccess->lUserIngot;
	pCMDLogonSuccess->lUserInsure=pDBOLogonSuccess->lUserInsure;
	pCMDLogonSuccess->dUserBeans=pDBOLogonSuccess->dUserBeans;	

	//������Ϣ
	pCMDLogonSuccess->cbInsureEnabled=pDBOLogonSuccess->cbInsureEnabled;
	pCMDLogonSuccess->cbShowServerStatus=m_bShowServerStatus?1:0;
	pCMDLogonSuccess->cbIsAgent=pDBOLogonSuccess->cbIsAgent;

	//��Ա��Ϣ
	if (pDBOLogonSuccess->cbMemberOrder!=0 || pDBOLogonSuccess->dwCheckUserRight!=0)
	{
		DTP_GP_MemberInfo MemberInfo;
		ZeroMemory(&MemberInfo,sizeof(MemberInfo));
		MemberInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
		MemberInfo.MemberOverDate=pDBOLogonSuccess->MemberOverDate;
		SendPacket.AddPacket(&MemberInfo,sizeof(MemberInfo),DTP_GP_MEMBER_INFO);
	}

	//����ǩ��
	if (pDBOLogonSuccess->szUnderWrite[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite,CountStringBuffer(pDBOLogonSuccess->szUnderWrite),DTP_GP_UNDER_WRITE);
	}

	//��¼�ɹ�
	WORD wSendSize=SendPacket.GetDataSize()+sizeof(CMD_GP_LogonSuccess);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,cbDataBuffer,wSendSize);

	//�����б�
	if (m_pInitParameter->m_cbDelayList==FALSE || 
		pDBOLogonSuccess->dwCheckUserRight==UR_CAN_MANAGER_ANDROID)
	{
		//�����б�
		SendGameTypeInfo(dwContextID);
		SendGameKindInfo(dwContextID);
		SendGamePageInfo(dwContextID,INVALID_WORD);
		SendGameNodeInfo(dwContextID,INVALID_WORD);
		//SendGameServerInfo(dwContextID,INVALID_WORD);
		SendGameServerInfo(dwContextID,INVALID_WORD,DEVICE_TYPE_PC);
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
	}
	else
	{
		SendGameTypeInfo(dwContextID);
		SendGameKindInfo(dwContextID);
		SendGamePageInfo(dwContextID,0);
		//SendGameServerInfo(dwContextID,INVALID_WORD);
		SendGameServerInfo(dwContextID,INVALID_WORD,DEVICE_TYPE_PC);
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
	}
	
	//�ȼ�����
	SendGrowLevelConfig(dwContextID);

	//��������
	SendGamePropertyTypeInfo(dwContextID);
	SendGamePropertyRelatInfo(dwContextID);
	SendGamePropertyInfo(dwContextID);
	SendGamePropertySubInfo(dwContextID);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_PROPERTY_FINISH);

	//��Ա����
	SendMemberConfig(dwContextID);

	//��֤����
	SendRealAuthConfig(dwContextID);

	//��¼���
	CMD_GP_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish,sizeof(LogonFinish));
	LogonFinish.wIntermitTime=m_pInitParameter->m_wIntermitTime;
	LogonFinish.wOnLineCountTime=m_pInitParameter->m_wOnLineCountTime;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish));

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

	//��������
	LogonFailure.lResultCode=pLogonFailure->lResultCode;
	lstrcpyn(LogonFailure.szDescribeString,pLogonFailure->szDescribeString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FAILURE,&LogonFailure,wSendSize);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GP_ValidateMBCard));
	if (wDataSize!=sizeof(DBO_GP_ValidateMBCard)) return false;

	DBO_GP_ValidateMBCard * pValidateMBCard=(DBO_GP_ValidateMBCard *)pData;

	//����ṹ
	CMD_GP_ValidateMBCard ValidateMBCard;
	ValidateMBCard.uMBCardID=pValidateMBCard->uMBCardID;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_VALIDATE_MBCARD,&ValidateMBCard,sizeof(ValidateMBCard));

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonValidatePassPort(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_VALIDATE_PASSPORT,NULL,0);

	return true;
}

//��֤���
bool CAttemperEngineSink::OnDBPCLogonVerifyResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(DBO_GP_VerifyIndividualResult));
	if (wDataSize>sizeof(DBO_GP_VerifyIndividualResult)) return false;

	//��ȡ����
	DBO_GP_VerifyIndividualResult *pVerifyIndividualResult=(DBO_GP_VerifyIndividualResult *)pData;

	//����ṹ
	CMD_GP_VerifyIndividualResult VerifyIndividualResult;
	VerifyIndividualResult.wVerifyMask=pVerifyIndividualResult->wVerifyMask;
	VerifyIndividualResult.bVerifyPassage=pVerifyIndividualResult->bVerifyPassage;
	lstrcpyn(VerifyIndividualResult.szErrorMsg,pVerifyIndividualResult->szErrorMsg,CountArray(VerifyIndividualResult.szErrorMsg));

	//��������
	WORD wStringSize=CountStringBuffer(VerifyIndividualResult.szErrorMsg);
	WORD wSendSize=sizeof(VerifyIndividualResult)-sizeof(VerifyIndividualResult.szErrorMsg)+wStringSize;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_VERIFY_RESULT,&VerifyIndividualResult,wSendSize);

	return true;
}

//ƽ̨����
bool CAttemperEngineSink::OnDBPCPlatformParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GP_PlatformParameter * pPlatformParameter=(DBO_GP_PlatformParameter *)pData;

	//���ñ���
	m_PlatformParameter.dwExchangeRate = pPlatformParameter->dwExchangeRate;
	m_PlatformParameter.dwPresentExchangeRate = pPlatformParameter->dwPresentExchangeRate;
	m_PlatformParameter.dwRateGold = pPlatformParameter->dwRateGold;

	return true;
}

//�û�ͷ��
bool CAttemperEngineSink::OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_UserFaceInfo UserFaceInfo;
	ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));
	DBO_GP_UserFaceInfo * pUserFaceInfo=(DBO_GP_UserFaceInfo *)pData;

	//���ñ���
	UserFaceInfo.wFaceID=pUserFaceInfo->wFaceID;
	UserFaceInfo.dwCustomID=pUserFaceInfo->dwCustomID;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_FACE_INFO,&UserFaceInfo,sizeof(UserFaceInfo));

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_GP_UserIndividual * pDBOUserIndividual=(DBO_GP_UserIndividual *)pData;
	CMD_GP_UserIndividual * pCMDUserIndividual=(CMD_GP_UserIndividual *)cbDataBuffer;
	CSendPacketHelper SendPacket(cbDataBuffer+sizeof(CMD_GP_UserIndividual),sizeof(cbDataBuffer)-sizeof(CMD_GP_UserIndividual));

	//���ñ���
	ZeroMemory(pCMDUserIndividual,sizeof(CMD_GP_UserIndividual));

	//��������
	pCMDUserIndividual->dwUserID=pDBOUserIndividual->dwUserID;

	//�û���Ϣ
	if (pDBOUserIndividual->szUserNote[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szUserNote);
		SendPacket.AddPacket(pDBOUserIndividual->szUserNote,wBufferSize,DTP_GP_UI_USER_NOTE);
	}

	// ��ʵ
	if (pDBOUserIndividual->szCompellation[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szCompellation);
		SendPacket.AddPacket(pDBOUserIndividual->szCompellation,wBufferSize,DTP_GP_UI_COMPELLATION);
	}

	//���֤
	if (pDBOUserIndividual->szPassPortID[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szPassPortID);
		SendPacket.AddPacket(pDBOUserIndividual->szPassPortID,wBufferSize,DTP_GP_UI_PASSPORTID);
	}

	//�绰����
	if (pDBOUserIndividual->szSeatPhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szSeatPhone);
		SendPacket.AddPacket(pDBOUserIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEAT_PHONE);
	}

	//�ƶ��绰
	if (pDBOUserIndividual->szMobilePhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szMobilePhone);
		SendPacket.AddPacket(pDBOUserIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILE_PHONE);
	}

	//��ϵ����
	if (pDBOUserIndividual->szQQ[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szQQ);
		SendPacket.AddPacket(pDBOUserIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
	}

	//�����ʼ�
	if (pDBOUserIndividual->szEMail[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szEMail);
		SendPacket.AddPacket(pDBOUserIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
	}

	//��ϵ��ַ
	if (pDBOUserIndividual->szDwellingPlace[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szDwellingPlace);
		SendPacket.AddPacket(pDBOUserIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLING_PLACE);
	}

	//�ƹ���Ϣ
	if (pDBOUserIndividual->szSpreader[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szSpreader);
		SendPacket.AddPacket(pDBOUserIndividual->szSpreader,wBufferSize,DTP_GP_UI_SPREADER);
	}

	//������Ϣ
	WORD wSendSize=sizeof(CMD_GP_UserIndividual)+SendPacket.GetDataSize();
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INDIVIDUAL,cbDataBuffer,wSendSize);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserInsureInfo * pUserInsureInfo=(DBO_GP_UserInsureInfo *)pData;

	//��������
	CMD_GP_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//��������
	UserInsureInfo.cbEnjoinTransfer=pUserInsureInfo->cbEnjoinTransfer;
	UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
	UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
	UserInsureInfo.wRevenueTransferMember=pUserInsureInfo->wRevenueTransferMember;
	UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
	UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
	UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
	UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//���гɹ�
bool CAttemperEngineSink::OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserInsureSuccess * pUserInsureSuccess=(DBO_GP_UserInsureSuccess *)pData;

	//��������
	CMD_GP_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//���ñ���
	UserInsureSuccess.dwUserID=pUserInsureSuccess->dwUserID;
	UserInsureSuccess.lUserScore=pUserInsureSuccess->lSourceScore+pUserInsureSuccess->lVariationScore;
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//������Ϣ
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	DBO_GP_UserInsureFailure * pUserInsureFailure=(DBO_GP_UserInsureFailure *)pData;

	//��������
	UserInsureFailure.lResultCode=pUserInsureFailure->lResultCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pUserInsureFailure->szDescribeString,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDescribe);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserTransferUserInfo * pTransferUserInfo=(DBO_GP_UserTransferUserInfo *)pData;

	//��������
	CMD_GP_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//�������
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szAccounts,pTransferUserInfo->szAccounts,CountArray(UserTransferUserInfo.szAccounts));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_INFO_RESULT,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

//��ͨ���
bool CAttemperEngineSink::OnDBPCUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserInsureEnableResult * pUserInsureEnableResult=(DBO_GP_UserInsureEnableResult *)pData;

	//��������
	CMD_GP_UserInsureEnableResult UserInsureEnableResult;
	ZeroMemory(&UserInsureEnableResult,sizeof(UserInsureEnableResult));

	//�������
	UserInsureEnableResult.cbInsureEnabled=pUserInsureEnableResult->cbInsureEnabled;
	lstrcpyn(UserInsureEnableResult.szDescribeString,pUserInsureEnableResult->szDescribeString,CountArray(UserInsureEnableResult.szDescribeString));

	//��������
	WORD wHeadSize=CountStringBuffer(UserInsureEnableResult.szDescribeString);
	wHeadSize +=sizeof(UserInsureEnableResult)-sizeof(UserInsureEnableResult.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_ENABLE_RESULT,&UserInsureEnableResult,wHeadSize);

	return true;
}

bool CAttemperEngineSink::OnDBPCQueryTransferRebateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_QueryTransferRebateResult * pRebateResult=(DBO_GP_QueryTransferRebateResult *)pData;

	//��������
	CMD_GP_QueryTransferRebateResult RebateResult;
	ZeroMemory(&RebateResult,sizeof(RebateResult));

	RebateResult.dwUserID=pRebateResult->dwUserID;
	RebateResult.cbRebateEnabled = pRebateResult->cbRebateEnabled;
	RebateResult.lIngot = pRebateResult->lIngot;
	RebateResult.lLoveLiness = pRebateResult->lLoveLiness;

	return m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_QUERY_TRANSFER_REBATE_RESULT,&RebateResult,sizeof(RebateResult));
}

bool CAttemperEngineSink::OnDBPCQueryUserDataResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_QueryUserDataResult * pDBOData=(DBO_GP_QueryUserDataResult *)pData;

	//��������
	CMD_GP_QueryUserDataResult ClientData;
	ZeroMemory(&ClientData,sizeof(ClientData));

	ClientData.dwUserID=pDBOData->dwUserID;

	return m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_QUERY_USERDATA_RESULT,&ClientData,sizeof(ClientData));
}

//ǩ����Ϣ
bool CAttemperEngineSink::OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_CheckInInfo * pCheckInInfo=(DBO_GP_CheckInInfo *)pData;

	//��������
	CMD_GP_CheckInInfo CheckInInfo;
	ZeroMemory(&CheckInInfo,sizeof(CheckInInfo));

	//�������
	CheckInInfo.bTodayChecked=pCheckInInfo->bTodayChecked;
	CheckInInfo.wSeriesDate=pCheckInInfo->wSeriesDate;
	CopyMemory(CheckInInfo.lRewardGold,m_lCheckInReward,sizeof(CheckInInfo.lRewardGold));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_CHECKIN_INFO,&CheckInInfo,sizeof(CheckInInfo));

	return true;
}

//ǩ�����
bool CAttemperEngineSink::OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_CheckInResult * pCheckInResult=(DBO_GP_CheckInResult *)pData;

	//��������
	CMD_GP_CheckInResult CheckInResult;
	ZeroMemory(&CheckInResult,sizeof(CheckInResult));

	//�������
	CheckInResult.bSuccessed=pCheckInResult->bSuccessed;
	CheckInResult.lScore=pCheckInResult->lScore;
	lstrcpyn(CheckInResult.szNotifyContent,pCheckInResult->szNotifyContent,CountArray(CheckInResult.szNotifyContent));

	//��������
	WORD wSendSize = sizeof(CheckInResult)-sizeof(CheckInResult.szNotifyContent)+CountStringBuffer(CheckInResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_CHECKIN_RESULT,&CheckInResult,wSendSize);

	return true;
}

//ǩ������
bool CAttemperEngineSink::OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GP_CheckInReward));
	if(wDataSize!=sizeof(DBO_GP_CheckInReward)) return false;

	//��ȡ����
	DBO_GP_CheckInReward * pCheckInReward=(DBO_GP_CheckInReward *)pData;

	//��������
	CopyMemory(m_lCheckInReward,pCheckInReward->lRewardGold,sizeof(m_lCheckInReward));

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBPCTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GP_TaskListInfo * pTaskListInfo=(DBO_GP_TaskListInfo *)pData;

	//��������	
	WORD wTaskCount = __min(pTaskListInfo->wTaskCount,CountArray(m_TaskParameterBuffer)-m_wTaskCountBuffer);
	CopyMemory(m_TaskParameterBuffer+m_wTaskCountBuffer,++pTaskListInfo,sizeof(tagTaskParameter)*wTaskCount);

	//���ñ���
	m_wTaskCountBuffer += wTaskCount;

	return true;
}

//�������
bool CAttemperEngineSink::OnDBPCTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������	
	m_wTaskCount = m_wTaskCountBuffer;
	CopyMemory(m_TaskParameter, m_TaskParameterBuffer, sizeof(tagTaskParameter)*m_wTaskCount);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBPCTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_TaskInfo * pTaskInfo=(DBO_GP_TaskInfo *)pData;

	//��������
	WORD wTaskCount = m_wTaskCount;

	//��������
	WORD wSendSize=0,wParameterSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	LPBYTE pDataBuffer = cbDataBuffer;

	//��������
	pDataBuffer += sizeof(wTaskCount);
	wSendSize = wSendSize;

	//�ְ�����
	for(WORD i=0;i<wTaskCount;i++)
	{
		//Խ���ж�
		if(wSendSize+sizeof(tagTaskParameter)>SOCKET_TCP_PACKET)
		{
			//���ñ���
			pDataBuffer = cbDataBuffer;
			wTaskCount -= i;
			*(WORD*)pDataBuffer = i; 

			//��������
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_TASK_LIST,&cbDataBuffer,wSendSize);

			//���ñ���
			pDataBuffer += sizeof(wSendSize);
			wSendSize = wSendSize;
		}

		//�����С
		wParameterSize = sizeof(tagTaskParameter)-sizeof(m_TaskParameter[i].szTaskDescribe)+CountStringBuffer(m_TaskParameter[i].szTaskDescribe);
		*(WORD *)pDataBuffer = wParameterSize;
		pDataBuffer += sizeof(wParameterSize);
		wSendSize += sizeof(wParameterSize);

		//��������
		CopyMemory(pDataBuffer,&m_TaskParameter[i],wParameterSize);

		//�ƽ�ָ��
		pDataBuffer += wParameterSize;
		wSendSize += wParameterSize;
	}

	//ʣ�෢��
	if(wTaskCount>0 && wSendSize)
	{
		//���ñ���
		pDataBuffer = cbDataBuffer;
		*(WORD *)pDataBuffer = wTaskCount; 

		//��������
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_TASK_LIST,&cbDataBuffer,wSendSize);
	}

	//����ṹ
	CMD_GP_TaskInfo TaskInfo;
	TaskInfo.wTaskCount = pTaskInfo->wTaskCount;
	CopyMemory(TaskInfo.TaskStatus,pTaskInfo->TaskStatus,sizeof(TaskInfo.TaskStatus[0])*pTaskInfo->wTaskCount);

	//��������
	WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
	wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_TASK_INFO,&TaskInfo,wSendDataSize);

	return true;
}

//������
bool CAttemperEngineSink::OnDBPCTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_TaskResult * pTaskResult=(DBO_GP_TaskResult *)pData;

	//��������
	CMD_GP_TaskResult TaskResult;
	ZeroMemory(&TaskResult,sizeof(TaskResult));

	//�������
	TaskResult.bSuccessed=pTaskResult->bSuccessed;
	TaskResult.wCommandID=pTaskResult->wCommandID;
	TaskResult.lCurrScore=pTaskResult->lCurrScore;
	TaskResult.lCurrIngot=pTaskResult->lCurrIngot;	
	lstrcpyn(TaskResult.szNotifyContent,pTaskResult->szNotifyContent,CountArray(TaskResult.szNotifyContent));

	//��������
	WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_TASK_RESULT,&TaskResult,wSendSize);

	return true;
}

//�ͱ�����
bool CAttemperEngineSink::OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_BaseEnsureParameter * pEnsureParameter=(DBO_GP_BaseEnsureParameter *)pData;

	//���ñ���
	m_BaseEnsureParameter.cbTakeTimes = pEnsureParameter->cbTakeTimes;
	m_BaseEnsureParameter.lScoreAmount = pEnsureParameter->lScoreAmount;
	m_BaseEnsureParameter.lScoreCondition = pEnsureParameter->lScoreCondition;

	return true;
}

//�ͱ����
bool CAttemperEngineSink::OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_BaseEnsureResult * pBaseEnsureResult=(DBO_GP_BaseEnsureResult *)pData;

	//����ṹ
	CMD_GP_BaseEnsureResult BaseEnsureResult;
	BaseEnsureResult.bSuccessed=pBaseEnsureResult->bSuccessed;
	BaseEnsureResult.lGameScore=pBaseEnsureResult->lGameScore;
	lstrcpyn(BaseEnsureResult.szNotifyContent,pBaseEnsureResult->szNotifyContent,CountArray(BaseEnsureResult.szNotifyContent));

	//��������
	WORD wSendDataSize=sizeof(BaseEnsureResult)-sizeof(BaseEnsureResult.szNotifyContent);
	wSendDataSize+=CountStringBuffer(BaseEnsureResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_BASEENSURE_RESULT,&BaseEnsureResult,wSendDataSize);

	return true;
}

//�ƹ����
bool CAttemperEngineSink::OnDBPCUserSpreadInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserSpreadInfo * pSpreadParameter=(DBO_GP_UserSpreadInfo *)pData;

	//����ṹ
	CMD_GP_UserSpreadInfo UserSpreadInfo;
	UserSpreadInfo.dwSpreadCount=pSpreadParameter->dwSpreadCount;
	UserSpreadInfo.lSpreadReward=pSpreadParameter->lSpreadReward;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_SPREAD_INFO,&UserSpreadInfo,sizeof(UserSpreadInfo));

	return true;	
}

//ʵ������
bool CAttemperEngineSink::OnDBPCRealAuthParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_RealAuthParameter * pParameter=(DBO_GP_RealAuthParameter *)pData;

	m_AuthRealParameter.dwAuthRealAward = pParameter->dwAuthRealAward;
	m_AuthRealParameter.dwAuthentDisable= pParameter->dwAuthentDisable;

	return true;	

}


//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_GrowLevelConfig * pGrowLevelConfig=(DBO_GP_GrowLevelConfig *)pData;

	//��������	
	m_wLevelCount = pGrowLevelConfig->wLevelCount;
	CopyMemory(m_GrowLevelConfig,&pGrowLevelConfig->GrowLevelConfig,sizeof(tagGrowLevelConfig)*m_wLevelCount);

	return true;
}

//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_GrowLevelParameter * pGrowLevelParameter=(DBO_GP_GrowLevelParameter *)pData;

	//����ṹ
	CMD_GP_GrowLevelParameter GrowLevelParameter;
	GrowLevelParameter.wCurrLevelID=pGrowLevelParameter->wCurrLevelID;	
	GrowLevelParameter.dwExperience=pGrowLevelParameter->dwExperience;
	GrowLevelParameter.dwUpgradeExperience=pGrowLevelParameter->dwUpgradeExperience;
	GrowLevelParameter.lUpgradeRewardGold=pGrowLevelParameter->lUpgradeRewardGold;
	GrowLevelParameter.lUpgradeRewardIngot=pGrowLevelParameter->lUpgradeRewardIngot;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_GROWLEVEL_PARAMETER,&GrowLevelParameter,sizeof(GrowLevelParameter));

	return true;
}

//�ȼ�����
bool CAttemperEngineSink::OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_GrowLevelUpgrade * pGrowLevelUpgrade=(DBO_GP_GrowLevelUpgrade *)pData;

	//����ṹ
	CMD_GP_GrowLevelUpgrade GrowLevelUpgrade;
	GrowLevelUpgrade.lCurrScore=pGrowLevelUpgrade->lCurrScore;
	GrowLevelUpgrade.lCurrIngot=pGrowLevelUpgrade->lCurrIngot;
	lstrcpyn(GrowLevelUpgrade.szNotifyContent,pGrowLevelUpgrade->szNotifyContent,CountArray(GrowLevelUpgrade.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
	wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_GROWLEVEL_UPGRADE,&GrowLevelUpgrade,wSendDataSize);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertyRelatItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePropertyRelatItem)==0);
	if (wDataSize%sizeof(DBO_GP_GamePropertyRelatItem)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePropertyRelatItem);
	DBO_GP_GamePropertyRelatItem * pGamePropertyRelatItem=(DBO_GP_GamePropertyRelatItem *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagPropertyRelatItem GamePropertyRelat;
		ZeroMemory(&GamePropertyRelat,sizeof(GamePropertyRelat));

		//��������
		GamePropertyRelat.dwTypeID=(pGamePropertyRelatItem+i)->dwTypeID;
		GamePropertyRelat.dwPropertyID=(pGamePropertyRelatItem+i)->dwPropertyID;

		//�����б�
		m_GamePropertyListManager.InsertGamePropertyRelatItem(&GamePropertyRelat);
	}

	return true;

}

//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertyTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePropertyTypeItem)==0);
	if (wDataSize%sizeof(DBO_GP_GamePropertyTypeItem)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePropertyTypeItem);
	DBO_GP_GamePropertyTypeItem * pGamePropertyTypeItem=(DBO_GP_GamePropertyTypeItem *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagPropertyTypeItem GamePropertyType;
		ZeroMemory(&GamePropertyType,sizeof(GamePropertyType));

		//��������
		GamePropertyType.dwTypeID=(pGamePropertyTypeItem+i)->dwTypeID;
		GamePropertyType.dwSortID=(pGamePropertyTypeItem+i)->dwSortID;
		lstrcpyn(GamePropertyType.szTypeName,(pGamePropertyTypeItem+i)->szTypeName,CountArray(GamePropertyType.szTypeName));

		//�����б�
		m_GamePropertyListManager.InsertGamePropertyTypeItem(&GamePropertyType);
	}

	return true;

}
//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertyItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePropertyItem)==0);
	if (wDataSize%sizeof(DBO_GP_GamePropertyItem)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePropertyItem);
	DBO_GP_GamePropertyItem * pGamePropertyItem=(DBO_GP_GamePropertyItem *)pData;

	//��ѯ����
	for (WORD nNewIndex =0;nNewIndex<wItemCount;nNewIndex++)
	{
		DBO_GP_GamePropertyItem * pCurrGamePropertyItem = (DBO_GP_GamePropertyItem *)(pGamePropertyItem + nNewIndex);

		//��������
		tagPropertyItem GameProperty;
		ZeroMemory(&GameProperty,sizeof(GameProperty));

		GameProperty.dwPropertyID = pCurrGamePropertyItem->dwPropertyID;
		GameProperty.dwPropertyKind = pCurrGamePropertyItem->dwPropertyKind;
		GameProperty.cbUseArea = pCurrGamePropertyItem->cbUseArea;
		GameProperty.cbServiceArea = pCurrGamePropertyItem->cbServiceArea;
		GameProperty.cbRecommend = pCurrGamePropertyItem->cbRecommend;
		GameProperty.lPropertyGold = pCurrGamePropertyItem->lPropertyGold;
		GameProperty.dPropertyCash = pCurrGamePropertyItem->dPropertyCash;
		GameProperty.lPropertyUserMedal = pCurrGamePropertyItem->lPropertyUserMedal;
		GameProperty.lPropertyLoveLiness = pCurrGamePropertyItem->lPropertyLoveLiness;
		GameProperty.lSendLoveLiness = pCurrGamePropertyItem->lSendLoveLiness;
		GameProperty.lRecvLoveLiness = pCurrGamePropertyItem->lRecvLoveLiness;
		GameProperty.lUseResultsGold = pCurrGamePropertyItem->lUseResultsGold;
		GameProperty.dwUseResultsValidTime = pCurrGamePropertyItem->dwUseResultsValidTime;
		GameProperty.dwUseResultsValidTimeScoreMultiple = pCurrGamePropertyItem->dwUseResultsValidTimeScoreMultiple;
		GameProperty.dwUseResultsGiftPackage = pCurrGamePropertyItem->dwUseResultsGiftPackage;
		GameProperty.dwSortID = pCurrGamePropertyItem->dwSortID;
		lstrcpyn(GameProperty.szName,pCurrGamePropertyItem->szName,CountArray(GameProperty.szName));
		lstrcpyn(GameProperty.szRegulationsInfo,pCurrGamePropertyItem->szRegulationsInfo,CountArray(GameProperty.szRegulationsInfo));

		//�����б�
		m_GamePropertyListManager.InsertGamePropertyItem(&GameProperty);
	}



	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertySubItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePropertySubItem)==0);
	if (wDataSize%sizeof(DBO_GP_GamePropertySubItem)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePropertySubItem);
	DBO_GP_GamePropertySubItem * pGamePropertySubtem=(DBO_GP_GamePropertySubItem *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagPropertySubItem GamePropertySub;
		ZeroMemory(&GamePropertySub,sizeof(GamePropertySub));

		//��������
		GamePropertySub.dwPropertyID=(pGamePropertySubtem+i)->dwPropertyID;
		GamePropertySub.dwPropertyCount=(pGamePropertySubtem+i)->dwPropertyCount;
		GamePropertySub.dwOwnerPropertyID = (pGamePropertySubtem+i)->dwOwnerPropertyID;
		GamePropertySub.dwSortID=(pGamePropertySubtem+i)->dwSortID;

		//�����б�
		m_GamePropertyListManager.InsertGamePropertySubItem(&GamePropertySub);
	}

	return true;

}

//���߽��
bool CAttemperEngineSink::OnDBGamePropertyListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GP_GamePropertyListResult * pGamePropertyInfo=(DBO_GP_GamePropertyListResult *)pData;

	//���õ���
	if (pGamePropertyInfo->cbSuccess == TRUE)
	{

	}

	return true;
}
//���߹���
bool CAttemperEngineSink::OnDBGamePropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_PropertyFailure * pPropertyFailure=(DBO_GP_PropertyFailure *)pData;

	//����ṹ
	CMD_GP_PropertyFailure PropertyFailure;
	PropertyFailure.lErrorCode = pPropertyFailure->lErrorCode;
	lstrcpyn(PropertyFailure.szDescribeString,pPropertyFailure->szDescribeString,CountArray(PropertyFailure.szDescribeString));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_PROPERTY_FAILURE,&PropertyFailure,sizeof(PropertyFailure));


	return true;
}

bool CAttemperEngineSink::OnDBQueryPropertySingle(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_PropertyQuerySingle * pPropertyBuyResult=(DBO_GP_PropertyQuerySingle *)pData;

	//����ṹ
	CMD_GP_PropertyQuerySingleResult PropertyBuyResult;
	PropertyBuyResult.dwUserID = pPropertyBuyResult->dwUserID;
	PropertyBuyResult.dwPropertyID = pPropertyBuyResult->dwPropertyID;
	PropertyBuyResult.dwItemCount = pPropertyBuyResult->dwItemCount;
	return m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_QUERY_SINGLE_RESULT,&PropertyBuyResult,sizeof(PropertyBuyResult));
}

//���߹���
bool CAttemperEngineSink::OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_PropertyBuyResult * pPropertyBuyResult=(DBO_GP_PropertyBuyResult *)pData;

	//����ṹ
	CMD_GP_PropertyBuyResult PropertyBuyResult;
	PropertyBuyResult.dwUserID = pPropertyBuyResult->dwUserID;
	PropertyBuyResult.dwPropertyID = pPropertyBuyResult->dwPropertyID;
	PropertyBuyResult.dwItemCount = pPropertyBuyResult->dwItemCount;
	PropertyBuyResult.lInsureScore = pPropertyBuyResult->lInsureScore;
	PropertyBuyResult.lUserMedal = pPropertyBuyResult->lUserMedal;
	PropertyBuyResult.lLoveLiness = pPropertyBuyResult->lLoveLiness;
	PropertyBuyResult.dCash = pPropertyBuyResult->dCash;
	PropertyBuyResult.cbCurrMemberOrder = pPropertyBuyResult->cbCurrMemberOrder;
	lstrcpyn(PropertyBuyResult.szNotifyContent,pPropertyBuyResult->szNotifyContent,CountArray(PropertyBuyResult.szNotifyContent));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_PROPERTY_BUY_RESULT,&PropertyBuyResult,sizeof(PropertyBuyResult));

	return true;
}


bool CAttemperEngineSink::OnDBQueryUserBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GP_QueryBackpack* pQueryBackpackResult = (DBO_GP_QueryBackpack*)pData;


	//���췵��
	BYTE cbDataBuffer[SOCKET_TCP_PACKET]={0};
	CMD_GP_S_BackpackProperty* pBackpackPropertyResult = (CMD_GP_S_BackpackProperty*)cbDataBuffer;

	//��ʼ������
	WORD dwDataBufferSize = sizeof(CMD_GP_S_BackpackProperty);
	pBackpackPropertyResult->dwUserID = pQueryBackpackResult->dwUserID;
	pBackpackPropertyResult->dwCount = pQueryBackpackResult->dwCount;
	pBackpackPropertyResult->dwStatus = pQueryBackpackResult->dwStatus;
	if(pQueryBackpackResult->dwCount > 0)
	{
		memcpy(pBackpackPropertyResult->PropertyInfo, pQueryBackpackResult->PropertyInfo, sizeof(tagBackpackProperty)*(pQueryBackpackResult->dwCount));
		dwDataBufferSize += sizeof(tagBackpackProperty)*(WORD)(pQueryBackpackResult->dwCount-1);
	}
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_QUERY_BACKPACKET_RESULT,pBackpackPropertyResult,dwDataBufferSize);
	return true;
}

bool CAttemperEngineSink::OnDBUserPropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_UserPropertyBuff* pUserPropertyBuff = (DBO_GR_UserPropertyBuff*)pData;

	CMD_GP_S_UserPropertyBuff  UserPropertyBuff;
	UserPropertyBuff.dwUserID = pUserPropertyBuff->dwUserID;
	UserPropertyBuff.cbBuffCount = pUserPropertyBuff->cbBuffCount;
	memcpy(UserPropertyBuff.PropertyBuff, pUserPropertyBuff->PropertyBuff, sizeof(tagPropertyBuff)*pUserPropertyBuff->cbBuffCount);

	WORD dwHeadSize=sizeof(UserPropertyBuff)-sizeof(UserPropertyBuff.PropertyBuff);
	WORD dwDataSize=UserPropertyBuff.cbBuffCount*sizeof(UserPropertyBuff.PropertyBuff[0]);

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_PROPERTY_BUFF_RESULT,&UserPropertyBuff,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBGamePropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GP_PropertyUse* pPropertyUseResult = (DBO_GP_PropertyUse*)pData;

	//����ṹ
	CMD_GP_S_PropertyUse PropertyUseResult;

	PropertyUseResult.dwUserID = pPropertyUseResult->dwUserID;
	PropertyUseResult.dwRecvUserID = pPropertyUseResult->dwRecvUserID;
	PropertyUseResult.dwPropID = pPropertyUseResult->dwPropID;
	PropertyUseResult.dwPropKind = pPropertyUseResult->dwPropKind;
	PropertyUseResult.wPropCount = pPropertyUseResult->wPropCount;
	PropertyUseResult.Score = pPropertyUseResult->Score;
	PropertyUseResult.dwRemainderPropCount = pPropertyUseResult->dwRemainderPropCount;
	PropertyUseResult.dwScoreMultiple = pPropertyUseResult->dwScoreMultiple;
	PropertyUseResult.lRecvLoveLiness = pPropertyUseResult->lRecvLoveLiness;
	PropertyUseResult.lSendLoveLiness = pPropertyUseResult->lSendLoveLiness;
	PropertyUseResult.lUseResultsGold = pPropertyUseResult->lUseResultsGold;
	PropertyUseResult.UseResultsValidTime = pPropertyUseResult->UseResultsValidTime;
	PropertyUseResult.tUseTime = pPropertyUseResult->tUseTime;
	PropertyUseResult.dwHandleCode = pPropertyUseResult->dwHandleCode;
	PropertyUseResult.cbMemberOrder = pPropertyUseResult->cbMemberOrder;
	lstrcpyn(PropertyUseResult.szName, pPropertyUseResult->szName,CountArray(PropertyUseResult.szName));
	lstrcpyn(PropertyUseResult.szNotifyContent, pPropertyUseResult->szNotifyContent,CountArray(PropertyUseResult.szNotifyContent));
	WORD wSendSize=sizeof(PropertyUseResult)-sizeof(PropertyUseResult.szNotifyContent)+CountStringBuffer(PropertyUseResult.szNotifyContent);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_PROPERTY_USE_RESULT,&PropertyUseResult,wSendSize);
	
	return true;
}

bool CAttemperEngineSink::OnDBUserPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GP_PropertyPresent* pPropertyPresent = (DBO_GP_PropertyPresent*)pData;

	//����ṹ
	CMD_GP_S_PropertyPresent PropertyPresentResult={0};
	PropertyPresentResult.dwUserID = pPropertyPresent->dwUserID;
	PropertyPresentResult.dwRecvGameID = pPropertyPresent->dwRecvGameID;
	PropertyPresentResult.dwPropID = pPropertyPresent->dwPropID;
	PropertyPresentResult.wPropCount = pPropertyPresent->wPropCount;
	PropertyPresentResult.wType = pPropertyPresent->wType;
	PropertyPresentResult.nHandleCode = pPropertyPresent->nHandleCode;
	
	lstrcpyn(PropertyPresentResult.szRecvNickName, pPropertyPresent->szRecvNickName,CountArray(PropertyPresentResult.szRecvNickName));
	lstrcpyn(PropertyPresentResult.szNotifyContent, pPropertyPresent->szNotifyContent,CountArray(PropertyPresentResult.szNotifyContent));

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_PROPERTY_PRESENT_RESULT,&PropertyPresentResult,sizeof(PropertyPresentResult));
	return true;
}

bool CAttemperEngineSink::OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GP_QuerySendPresent* pQuerySendPresent = (DBO_GP_QuerySendPresent*)pData;
	CMD_GP_S_QuerySendPresent QuerySendPresentResult={0};
	QuerySendPresentResult.wPresentCount = pQuerySendPresent->wPresentCount;
	memcpy(QuerySendPresentResult.Present, pQuerySendPresent->Present, sizeof(pQuerySendPresent->wPresentCount*sizeof(pQuerySendPresent->Present[0])));
	
	WORD dwHeadSize=sizeof(QuerySendPresentResult)-sizeof(QuerySendPresentResult.Present);
	WORD dwDataSize=QuerySendPresentResult.wPresentCount*sizeof(QuerySendPresentResult.Present[0]);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_QUERY_SEND_PRESENT_RESULT,&QuerySendPresentResult,dwHeadSize+dwDataSize);
	return true;
}

bool CAttemperEngineSink::OnDBGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GP_GetSendPresent* pGetSendPresent = (DBO_GP_GetSendPresent*)pData;
	CMD_GP_S_GetSendPresent GetSendPresentResult={0};
	GetSendPresentResult.wPresentCount = pGetSendPresent->wPresentCount;
	memcpy(GetSendPresentResult.Present, pGetSendPresent->Present, pGetSendPresent->wPresentCount*sizeof(SendPresent));

	WORD dwHeadSize=sizeof(GetSendPresentResult)-sizeof(GetSendPresentResult.Present);
	WORD dwDataSize=GetSendPresentResult.wPresentCount*sizeof(GetSendPresentResult.Present[0]);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_PROPERTY,SUB_GP_GET_SEND_PRESENT_RESULT,&GetSendPresentResult,dwHeadSize+dwDataSize);
	return true;
}

//��Ա����
bool CAttemperEngineSink::OnDBPCMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_MemberParameter * pMemberParameter=(DBO_GP_MemberParameter *)pData;

	//��������	
	m_wMemberCount = pMemberParameter->wMemberCount;
	CopyMemory(m_MemberParameter,pMemberParameter->MemberParameter,sizeof(tagMemberParameterNew)*m_wMemberCount);

	return true;
}

//��Ա��ѯ
bool CAttemperEngineSink::OnDBPCMemberDayQueryInfoResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_MemberQueryInfoResult * pMemberResult=(DBO_GP_MemberQueryInfoResult *)pData;

	//����ṹ
	CMD_GP_MemberQueryInfoResult MemberResult;
	MemberResult.bPresent=pMemberResult->bPresent;
	MemberResult.bGift=pMemberResult->bGift;
	//��������	
	MemberResult.GiftSubCount = pMemberResult->GiftSubCount;
	CopyMemory(MemberResult.GiftSub,pMemberResult->GiftSub,sizeof(tagGiftPropertyInfo)*MemberResult.GiftSubCount);

	//��������
	//�����С
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.GiftSub);
	wSendDataSize += sizeof(tagGiftPropertyInfo)*(WORD)(MemberResult.GiftSubCount);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_MEMBER_QUERY_INFO_RESULT,&MemberResult,wSendDataSize);

	return true;
}

//��Ա�ͽ�
bool CAttemperEngineSink::OnDBPCMemberDayPresentResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_MemberDayPresentResult * pMemberResult=(DBO_GP_MemberDayPresentResult *)pData;

	//����ṹ
	CMD_GP_MemberDayPresentResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	MemberResult.lGameScore=pMemberResult->lGameScore;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_MEMBER_DAY_PRESENT_RESULT,&MemberResult,wSendDataSize);

	return true;

}

//��Ա���
bool CAttemperEngineSink::OnDBPCMemberDayGiftResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_MemberDayGiftResult * pMemberResult=(DBO_GP_MemberDayGiftResult *)pData;

	//����ṹ
	CMD_GP_MemberDayGiftResult MemberResult;
	MemberResult.bSuccessed=pMemberResult->bSuccessed;
	lstrcpyn(MemberResult.szNotifyContent,pMemberResult->szNotifyContent,CountArray(MemberResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(MemberResult)-sizeof(MemberResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(MemberResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_MEMBER_DAY_GIFT_RESULT,&MemberResult,wSendDataSize);
	return true;
}

//������
bool CAttemperEngineSink::OnDBPCPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��ȡ����
	DBO_GP_PurchaseResult * pPurchaseResult=(DBO_GP_PurchaseResult *)pData;

	//����ṹ
	CMD_GP_PurchaseResult PurchaseResult;
	ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

	//���ñ���
	PurchaseResult.bSuccessed=pPurchaseResult->bSuccessed;
	PurchaseResult.lCurrScore=pPurchaseResult->lCurrScore;
	PurchaseResult.dCurrBeans=pPurchaseResult->dCurrBeans;
	PurchaseResult.cbMemberOrder=pPurchaseResult->cbMemberOrder;
	lstrcpyn(PurchaseResult.szNotifyContent,pPurchaseResult->szNotifyContent,CountArray(PurchaseResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_PURCHASE_RESULT,&PurchaseResult,wSendDataSize);

	return true;
}

//�����һ����
bool CAttemperEngineSink::OnDBPCExChangeRoomCardToScoreResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��ȡ����
	DBO_GP_RoomCardExchangeResult * pExchangeResult=(DBO_GP_RoomCardExchangeResult *)pData;

	//����ṹ
	CMD_GP_ExchangeRoomCardResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//���ñ���
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lRoomCard=pExchangeResult->lCurrRoomCard;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_PERSONAL_SERVICE, SUB_GP_EXCHANGE_ROOM_CARD_RESULT,&ExchangeResult,wSendDataSize);

	return true;
}

//�һ����
bool CAttemperEngineSink::OnDBPCExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��ȡ����
	DBO_GP_ExchangeResult * pExchangeResult=(DBO_GP_ExchangeResult *)pData;

	//����ṹ
	CMD_GP_ExchangeResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//���ñ���
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lCurrIngot=pExchangeResult->lCurrIngot;
	ExchangeResult.dCurrBeans=pExchangeResult->dCurrBeans;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_EXCHANGE_RESULT,&ExchangeResult,wSendDataSize);

	return true;
}

//��������
bool CAttemperEngineSink::OnDBAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;
	
	//��������
	DBO_GP_AndroidParameter * pAndroidParameter=(DBO_GP_AndroidParameter *)pData;

	//��������
	CMD_GP_AndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter,sizeof(AndroidParameter));

	//�������
	AndroidParameter.wSubCommdID=pAndroidParameter->wSubCommdID;
	AndroidParameter.wParameterCount=pAndroidParameter->wParameterCount;
	CopyMemory(AndroidParameter.AndroidParameter,pAndroidParameter->AndroidParameter,sizeof(tagAndroidParameter)*AndroidParameter.wParameterCount);

	//�����С
	WORD wSendDataSize = sizeof(AndroidParameter)-sizeof(AndroidParameter.AndroidParameter);
	wSendDataSize += sizeof(tagAndroidParameter)*AndroidParameter.wParameterCount;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_ANDROID_SERVICE,SUB_GP_ANDROID_PARAMETER,&AndroidParameter,wSendDataSize);

	//�����ж�
	if(AndroidParameter.wParameterCount==1)
	{
		//Э������
		if(AndroidParameter.wSubCommdID==SUB_GP_ADD_PARAMETER)
		{
			//����ṹ
			CMD_CS_C_AddParameter AddParameter;
			AddParameter.wServerID=pAndroidParameter->wServerID;
			CopyMemory(&AddParameter.AndroidParameter,&AndroidParameter.AndroidParameter[0],sizeof(tagAndroidParameter));

			//��������
			m_pITCPSocketService->SendData(MDM_CS_ANDROID_SERVICE,SUB_CS_C_ADDPARAMETER,&AddParameter,sizeof(AddParameter));
		}

		//Э������
		if(AndroidParameter.wSubCommdID==SUB_GP_MODIFY_PARAMETER)
		{
			//����ṹ
			CMD_CS_C_ModifyParameter ModifyParameter;
			ModifyParameter.wServerID=pAndroidParameter->wServerID;
			CopyMemory(&ModifyParameter.AndroidParameter,&AndroidParameter.AndroidParameter[0],sizeof(tagAndroidParameter));

			//��������
			m_pITCPSocketService->SendData(MDM_CS_ANDROID_SERVICE,SUB_CS_C_MODIFYPARAMETER,&ModifyParameter,sizeof(ModifyParameter));
		}

		//Э������
		if(AndroidParameter.wSubCommdID==SUB_GP_DELETE_PARAMETER)
		{
			//����ṹ
			CMD_CS_C_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID=pAndroidParameter->wServerID;
			DeleteParameter.dwBatchID=AndroidParameter.AndroidParameter[0].dwBatchID;

			//��������
			m_pITCPSocketService->SendData(MDM_CS_ANDROID_SERVICE,SUB_CS_C_DELETEPARAMETER,&DeleteParameter,sizeof(DeleteParameter));
		}
	}

	return true;
}

bool CAttemperEngineSink::OnDBIndividualResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��ȡ����
	DBO_GP_IndividualResult * pResult=(DBO_GP_IndividualResult *)pData;

	//����ṹ
	CMD_GP_IndividuaResult currResult;
	ZeroMemory(&currResult,sizeof(currResult));

	//���ñ���
	currResult.bSuccessed=pResult->bSuccessed;
	currResult.lCurrScore=pResult->lScore;
	lstrcpyn(currResult.szNotifyContent,pResult->szDescribeString,CountArray(currResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(currResult)-sizeof(currResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(currResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_INDIVIDUAL_RESULT,&currResult,wSendDataSize);

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	//��������
	DBO_GP_OperateSuccess * pOperateSuccess=(DBO_GP_OperateSuccess *)pData;

	//��������
	OperateSuccess.lResultCode=pOperateSuccess->lResultCode;
	lstrcpyn(OperateSuccess.szDescribeString,pOperateSuccess->szDescribeString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wHeadSize+wDescribe);

	//�ر�����
	if(pOperateSuccess->bCloseSocket==true)
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_OperateFailure OperateFailure;
	ZeroMemory(&OperateFailure,sizeof(OperateFailure));

	//��������
	DBO_GP_OperateFailure * pOperateFailure=(DBO_GP_OperateFailure *)pData;

	//��������
	OperateFailure.lResultCode=pOperateFailure->lResultCode;
	lstrcpyn(OperateFailure.szDescribeString,pOperateFailure->szDescribeString,CountArray(OperateFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateFailure.szDescribeString);
	WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_FAILURE,&OperateFailure,wHeadSize+wDescribe);

	//�ر�����
	if(pOperateFailure->bCloseSocket==true)
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_MB_LogonSuccess * pDBOLogonSuccess=(DBO_MB_LogonSuccess *)pData;
	CMD_MB_LogonSuccess * pCMDLogonSuccess=(CMD_MB_LogonSuccess *)cbDataBuffer;

	//���Ͷ���
	WORD wHeadSize=sizeof(CMD_MB_LogonSuccess);
	CSendPacketHelper SendPacket(cbDataBuffer+wHeadSize,sizeof(cbDataBuffer)-wHeadSize);

	//���ñ���
	ZeroMemory(pCMDLogonSuccess,sizeof(CMD_MB_LogonSuccess));

	//��������
	pCMDLogonSuccess->wFaceID=pDBOLogonSuccess->wFaceID;
	pCMDLogonSuccess->cbGender=pDBOLogonSuccess->cbGender;
	pCMDLogonSuccess->dwCustomID=pDBOLogonSuccess->dwCustomID;
	pCMDLogonSuccess->dwGameID=pDBOLogonSuccess->dwGameID;
	pCMDLogonSuccess->dwUserID=pDBOLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwExperience=pDBOLogonSuccess->dwExperience;
	pCMDLogonSuccess->lLoveLiness=pDBOLogonSuccess->lLoveLiness;
	lstrcpyn(pCMDLogonSuccess->szAccounts,pDBOLogonSuccess->szAccounts,CountArray(pCMDLogonSuccess->szAccounts));
	lstrcpyn(pCMDLogonSuccess->szNickName,pDBOLogonSuccess->szNickName,CountArray(pCMDLogonSuccess->szNickName));
	lstrcpyn(pCMDLogonSuccess->szDynamicPass,pDBOLogonSuccess->szDynamicPass,CountArray(pCMDLogonSuccess->szDynamicPass));
	lstrcpyn(pCMDLogonSuccess->szChangeLogonIP,pDBOLogonSuccess->szChangeLogonIP,CountArray(pCMDLogonSuccess->szChangeLogonIP));
	lstrcpyn(pCMDLogonSuccess->szRegisterMobile,pDBOLogonSuccess->szRegisterMobile,CountArray(pCMDLogonSuccess->szRegisterMobile));
	//ip��ַ
	DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwContextID))->dwClientAddr;
	//ת����ַ
	TCHAR szClientAddr[LEN_IP]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&dwClientAddr;
	_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
	lstrcpyn(pCMDLogonSuccess->szIpAddress,szClientAddr,CountArray(pCMDLogonSuccess->szIpAddress));

	//�û��ɼ�
	pCMDLogonSuccess->lUserScore=pDBOLogonSuccess->lUserScore;
	pCMDLogonSuccess->lUserIngot=pDBOLogonSuccess->lUserIngot;
	pCMDLogonSuccess->lUserInsure=pDBOLogonSuccess->lUserInsure;
	pCMDLogonSuccess->dUserBeans=pDBOLogonSuccess->dUserBeans;

	//��չ��Ϣ
	pCMDLogonSuccess->cbInsureEnabled=pDBOLogonSuccess->cbInsureEnabled;
	pCMDLogonSuccess->cbIsAgent=pDBOLogonSuccess->cbIsAgent;
	pCMDLogonSuccess->cbMoorMachine=pDBOLogonSuccess->cbMoorMachine;
	pCMDLogonSuccess->lRoomCard = pDBOLogonSuccess->lRoomCard;
	pCMDLogonSuccess->dwLockServerID = pDBOLogonSuccess->dwLockServerID;
	pCMDLogonSuccess->dwKindID = pDBOLogonSuccess->dwKindID;
	pCMDLogonSuccess->RegisterData = pDBOLogonSuccess->RegisterData;

	//��Ա��Ϣ
	if (pDBOLogonSuccess->cbMemberOrder!=0 )
	{
		DTP_GP_MemberInfo MemberInfo;
		ZeroMemory(&MemberInfo,sizeof(MemberInfo));
		MemberInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
		MemberInfo.MemberOverDate=pDBOLogonSuccess->MemberOverDate;
		SendPacket.AddPacket(&MemberInfo,sizeof(MemberInfo),DTP_GP_MEMBER_INFO);
	}

	//����ǩ��
	if (pDBOLogonSuccess->szUnderWrite[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite,CountStringBuffer(pDBOLogonSuccess->szUnderWrite),DTP_GP_UNDER_WRITE);
	}

	//��¼�ɹ�
	WORD wSendSize=SendPacket.GetDataSize()+sizeof(CMD_MB_LogonSuccess);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_LOGON,SUB_MB_LOGON_SUCCESS,cbDataBuffer,wSendSize);

	//��Ա����
	SendMemberConfig(dwContextID);

	//���ͷ���
	WORD wIndex=LOWORD(dwContextID);
	SendMobileKindInfo(dwContextID);
	SendMobileServerInfo(dwContextID, INVALID_KIND);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_SERVER_LIST,SUB_MB_LIST_FINISH);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_MB_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_MB_LogonFailure * pLogonFailure=(DBO_MB_LogonFailure *)pData;

	//��������
	LogonFailure.lResultCode=pLogonFailure->lResultCode;
	lstrcpyn(LogonFailure.szDescribeString,pLogonFailure->szDescribeString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_LOGON,SUB_MB_LOGON_FAILURE,&LogonFailure,wSendSize);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//�齱����
bool CAttemperEngineSink::OnDBPCLotteryConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_LotteryConfig * pLotteryConfig=(DBO_GP_LotteryConfig *)pData;

	//��������
	CMD_GP_LotteryConfig LotteryConfig;
	ZeroMemory(&LotteryConfig,sizeof(LotteryConfig));

	//�������
	LotteryConfig.wLotteryCount=pLotteryConfig->wLotteryCount;
	CopyMemory(LotteryConfig.LotteryItem,pLotteryConfig->LotteryItem,sizeof(LotteryConfig.LotteryItem));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_LOTTERY_CONFIG,&LotteryConfig,sizeof(LotteryConfig));

	return true;
}

//�齱��Ϣ
bool CAttemperEngineSink::OnDBPCLotteryUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_LotteryUserInfo * pLotteryConfig=(DBO_GP_LotteryUserInfo *)pData;

	//��������
	CMD_GP_LotteryUserInfo LotteryUserInfo;
	ZeroMemory(&LotteryUserInfo,sizeof(LotteryUserInfo));

	//�������
	LotteryUserInfo.cbFreeCount=pLotteryConfig->cbFreeCount;
	LotteryUserInfo.cbAlreadyCount=pLotteryConfig->cbAlreadyCount;
	LotteryUserInfo.wKindID=pLotteryConfig->wKindID;
	LotteryUserInfo.dwUserID=pLotteryConfig->dwUserID;
	LotteryUserInfo.lChargeFee=pLotteryConfig->lChargeFee;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_LOTTERY_USER_INFO,&LotteryUserInfo,sizeof(LotteryUserInfo));

	return true;
}

//�齱���
bool CAttemperEngineSink::OnDBPCLotteryResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_LotteryResult * pLotteryResult=(DBO_GP_LotteryResult *)pData;

	//��������
	CMD_GP_LotteryResult LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(LotteryResult));

	//�������
	LotteryResult.bWined=pLotteryResult->bWined;
	LotteryResult.wKindID=pLotteryResult->wKindID;
	LotteryResult.dwUserID=pLotteryResult->dwUserID;
	LotteryResult.lUserScore=pLotteryResult->lUserScore;
	LotteryResult.dUserBeans=pLotteryResult->dUserBeans;
	CopyMemory(&LotteryResult.LotteryItem,&pLotteryResult->LotteryItem,sizeof(LotteryResult.LotteryItem));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_LOTTERY_RESULT,&LotteryResult,sizeof(LotteryResult));

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCQueryUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_GP_QueryUserGameData * pDBOQueryUserGameData=(DBO_GP_QueryUserGameData *)pData;
	CMD_GP_QueryUserGameData * pCMDQueryUserGameData=(CMD_GP_QueryUserGameData *)cbDataBuffer;
	CSendPacketHelper SendPacket(cbDataBuffer+sizeof(CMD_GP_QueryUserGameData),sizeof(cbDataBuffer)-sizeof(CMD_GP_QueryUserGameData));

	//���ñ���
	ZeroMemory(pCMDQueryUserGameData,sizeof(CMD_GP_QueryUserGameData));

	//�������
	pCMDQueryUserGameData->wKindID=pDBOQueryUserGameData->wKindID;
	pCMDQueryUserGameData->dwUserID=pDBOQueryUserGameData->dwUserID;

	//��ϵ��ַ
	if (pDBOQueryUserGameData->szUserGameData[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOQueryUserGameData->szUserGameData);
		SendPacket.AddPacket(pDBOQueryUserGameData->szUserGameData,wBufferSize,DTP_GP_UI_USER_GAME_DATA);
	}

	//������Ϣ
	WORD wSendSize=sizeof(CMD_GP_QueryUserGameData)+SendPacket.GetDataSize();
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_GAME_DATA,cbDataBuffer,wSendSize);

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBPCAgentGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagAgentGameKind)==0);
	if (wDataSize%sizeof(tagAgentGameKind)!=0) return false;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_AGENT_KIND,pData,wDataSize);

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBMBAgentGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagAgentGameKind)==0);
	if (wDataSize%sizeof(tagAgentGameKind)!=0) return false;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_SERVER_LIST,SUB_MB_AGENT_KIND,pData,wDataSize);

	return true;
}

//˽�˷�������
bool CAttemperEngineSink::OnDBMBPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagPersonalRoomOption)==0);
	if (wDataSize%sizeof(tagPersonalRoomOption)!=0) return false;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_MB_PERSONAL_PARAMETER, pData, wDataSize);

	return true;
}

//˽�˷�������
bool CAttemperEngineSink::OnDBMBPersonalFeeList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagPersonalTableFeeList)==0);
	if (wDataSize%sizeof(tagPersonalTableFeeList)!=0) return false;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_MB_PERSONAL_FEE_PARAMETER, pData, wDataSize);

	return true;
}

//����˽�˷����б�
bool CAttemperEngineSink::OnDBMBPersonalRoomListInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_MB_PersonalRoomInfoList)==0);
	if (wDataSize%sizeof(DBO_MB_PersonalRoomInfoList)!=0) return false;
	DBO_MB_PersonalRoomInfoList * pDBOPersonalRoomInfoList = (DBO_MB_PersonalRoomInfoList *)pData;
	CMD_MB_PersonalRoomInfoList  PersonalRoomInfoList;
	memcpy(&PersonalRoomInfoList, pDBOPersonalRoomInfoList,  sizeof(PersonalRoomInfoList));

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT, &PersonalRoomInfoList, sizeof(CMD_MB_PersonalRoomInfoList));

	return true;
}

//˽�˷���������󷿼���Ϣ
bool CAttemperEngineSink::OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	if (wDataSize == 0)
	{
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_GR_USER_QUERY_ROOM_SCORE_RESULT, NULL, 0);
		return true;
	}
	//Ч�����
	ASSERT(wDataSize%sizeof(tagQueryPersonalRoomUserScore)==0);
	if (wDataSize%sizeof(tagQueryPersonalRoomUserScore)!=0) return false;

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_GR_USER_QUERY_ROOM_SCORE_RESULT, pData, wDataSize);

	return true;

}

//˽�˷���������󷿼���Ϣ
bool CAttemperEngineSink::OnDBQueryPersonalRoomUersInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_MB_PersonalRoomUserInfo));
	if (wDataSize!=sizeof(DBO_MB_PersonalRoomUserInfo)) return false;
	DBO_MB_PersonalRoomUserInfo * pUserInfo = (DBO_MB_PersonalRoomUserInfo *)pData;
	CMD_MB_PersonalRoomUserInfo userInfo;
	userInfo.dBeans = pUserInfo->dBeans;
	userInfo.lRoomCard = pUserInfo->lRoomCard;


	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_PERSONAL_SERVICE, SUB_MB_QUERY_PERSONAL_ROOM_USER_INFO_RESULT, &userInfo, sizeof(userInfo));

	return true;

}

bool CAttemperEngineSink::OnDBQueryBankInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GP_QueryBankInfoResult));
	if (wDataSize!=sizeof(DBO_GP_QueryBankInfoResult)) return false;
	DBO_GP_QueryBankInfoResult * pDBOData = (DBO_GP_QueryBankInfoResult *)pData;
	CMD_GP_QueryBankInfoResult ClientData;
	lstrcpyn(ClientData.szRealName,  pDBOData->szRealName, CountArray(ClientData.szRealName));
	lstrcpyn(ClientData.szBankNo,  pDBOData->szBankNo, CountArray(ClientData.szBankNo));
	lstrcpyn(ClientData.szAlipayID,  pDBOData->szAlipayID, CountArray(ClientData.szAlipayID));
	ClientData.lDrawScore = pDBOData->lDrawScore;

	return m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_QUERY_BANK_INFO_RESULT, &ClientData, sizeof(ClientData));
}

bool CAttemperEngineSink::OnDBQueryGameLockInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_MB_QueryGameLockInfoResult));
	if (wDataSize!=sizeof(DBO_MB_QueryGameLockInfoResult)) return false;
	DBO_MB_QueryGameLockInfoResult * pDBOData = (DBO_MB_QueryGameLockInfoResult *)pData;

	CMD_MB_QueryGameLockInfoResult ClientData;
	ClientData.dwKindID = pDBOData->dwKindID;
	ClientData.dwServerID = pDBOData->dwServerID;

	return m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_QUERY_GAMELOCKINFO_RESULT, &ClientData, sizeof(ClientData));
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameType)==0);
	if (wDataSize%sizeof(DBO_GP_GameType)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameType);
	DBO_GP_GameType * pGameType=(DBO_GP_GameType *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//��������
		GameType.wTypeID=(pGameType+i)->wTypeID;
		GameType.wJoinID=(pGameType+i)->wJoinID;
		GameType.wSortID=(pGameType+i)->wSortID;
		lstrcpyn(GameType.szTypeName,(pGameType+i)->szTypeName,CountArray(GameType.szTypeName));

		//�����б�
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameKind)==0);
	if (wDataSize%sizeof(DBO_GP_GameKind)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameKind);
	DBO_GP_GameKind * pGameKind=(DBO_GP_GameKind *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameKind GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//��������
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		GameKind.wJoinID=(pGameKind+i)->wJoinID;
		GameKind.wSortID=(pGameKind+i)->wSortID;
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.wGameID=(pGameKind+i)->wGameID;
		GameKind.wRecommend=(pGameKind+i)->wRecommend;
		GameKind.wGameFlag=(pGameKind+i)->wGameFlag;
		GameKind.dwSuportType = (pGameKind+i)->dwSuportType;
		GameKind.dwOnLineCount=m_ServerListManager.CollectOnlineInfo((pGameKind+i)->wKindID,false);
		GameKind.dwAndroidCount=m_ServerListManager.CollectOnlineInfo((pGameKind+i)->wKindID,true);
		lstrcpyn(GameKind.szKindName,(pGameKind+i)->szKindName,CountArray(GameKind.szKindName));
		lstrcpyn(GameKind.szProcessName,(pGameKind+i)->szProcessName,CountArray(GameKind.szProcessName));

		//�����б�
		m_ServerListManager.InsertGameKind(&GameKind);
	}

	return true;
}

//��Ϸ�ڵ�
bool CAttemperEngineSink::OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameNode)==0);
	if (wDataSize%sizeof(DBO_GP_GameNode)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameNode);
	DBO_GP_GameNode * pGameNode=(DBO_GP_GameNode *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//��������
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wJoinID=(pGameNode+i)->wJoinID;
		GameNode.wSortID=(pGameNode+i)->wSortID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		lstrcpyn(GameNode.szNodeName,(pGameNode+i)->szNodeName,CountArray(GameNode.szNodeName));

		//�����б�
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePage)==0);
	if (wDataSize%sizeof(DBO_GP_GamePage)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePage);
	DBO_GP_GamePage * pGamePage=(DBO_GP_GamePage *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGamePage GamePage;
		ZeroMemory(&GamePage,sizeof(GamePage));

		//��������
		GamePage.wKindID=(pGamePage+i)->wKindID;
		GamePage.wNodeID=(pGamePage+i)->wNodeID;
		GamePage.wSortID=(pGamePage+i)->wSortID;
		GamePage.wPageID=(pGamePage+i)->wPageID;
		GamePage.wOperateType=(pGamePage+i)->wOperateType;
		lstrcpyn(GamePage.szDisplayName,(pGamePage+i)->szDisplayName,CountArray(GamePage.szDisplayName));

		//�����б�
		m_ServerListManager.InsertGamePage(&GamePage);
	}

	return true;
}

//��Ϸ�б�
bool CAttemperEngineSink::OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GP_GameListResult));
	if (wDataSize!=sizeof(DBO_GP_GameListResult)) return false;

	//��������
	DBO_GP_GameListResult * pGameListResult=(DBO_GP_GameListResult *)pData;

	//��Ϣ����
	if (pGameListResult->cbSuccess==TRUE)
	{
		//�����б�
		m_ServerListManager.CleanKernelItem();

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_LOAD_DB_LIST_RESULT,&ControlResult,sizeof(ControlResult));

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,m_pInitParameter->m_wLoadListTime*1000L,1,0);
	}
	else
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�������б����ʧ�ܣ�%ld ������¼���"),m_pInitParameter->m_wReLoadListTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,m_pInitParameter->m_wReLoadListTime*1000L,1,0);
	}

	return true;
}

//���������������Ϳ�ʼ
bool CAttemperEngineSink::OnDBServerDummyOnLineBegin(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ͷŵ��ɵ�����
	for (int i = 0; i < m_ServerDummyOnLineArray.GetCount(); ++i)
	{
		tagServerDummyOnLine * pCursor = m_ServerDummyOnLineArray.GetAt(i);
		free(pCursor);
	}
	m_ServerDummyOnLineArray.RemoveAll();
	m_ServerDummyOnLineMap.RemoveAll();
	return true;
}


//������������
bool CAttemperEngineSink::OnDBServerDummyOnLine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagServerDummyOnLine)==0);
	if (wDataSize%sizeof(tagServerDummyOnLine)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagServerDummyOnLine);
	tagServerDummyOnLine * pServerDummyOnLine=(tagServerDummyOnLine *)pData;

	tagServerDummyOnLine * pItemCursor = NULL;


	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		pItemCursor=(pServerDummyOnLine+i);

		tagServerDummyOnLine *pNewDummyOnline = (tagServerDummyOnLine*)malloc(sizeof(tagServerDummyOnLine));
		CopyMemory(pNewDummyOnline, pItemCursor, sizeof(tagServerDummyOnLine));
		m_ServerDummyOnLineArray.Add(pNewDummyOnline);
		//��������
	}

	return true;
}

//���������������ͽ���
bool CAttemperEngineSink::OnDBServerDummyOnLineEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	OnChangeDummyOnline();
	return true;
}

//�汾���
bool CAttemperEngineSink::CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer)
{
	//��������
	bool bMustUpdate=false;
	bool bAdviceUpdate=false;
	DWORD dwVersion=VERSION_PLAZA;

	//�ֻ��汾
	if(cbDeviceType >= DEVICE_TYPE_IPAD) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_IPHONE) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_ITOUCH) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_ANDROID) dwVersion=VERSION_MOBILE_ANDROID;
	else if(cbDeviceType == DEVICE_TYPE_PC) dwVersion=VERSION_PLAZA;

	//�汾�ж�
	if (bCheckLowVer && GetSubVer(dwPlazaVersion)<GetSubVer(dwVersion)) bAdviceUpdate=true;
	if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwVersion)) bMustUpdate=true;
	if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwVersion)) bMustUpdate=true;

	//�����ж�
	if ((bMustUpdate==true)||(bAdviceUpdate==true))
	{
		//��������
		CMD_GP_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdate=bMustUpdate;
		UpdateNotify.cbAdviceUpdate=bAdviceUpdate;
		UpdateNotify.dwCurrentVersion=dwVersion;

		//������Ϣ
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//�ж��ж�
		if (bMustUpdate==true) 
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return false;
		}
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

//��������
VOID CAttemperEngineSink::SendGameTypeInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameType))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,cbDataBuffer,wSendSize);

	return;
}

//��������
VOID CAttemperEngineSink::SendGameKindInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameKindItem->m_GameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,cbDataBuffer,wSendSize);

	return;
}

//���ͽڵ�
VOID CAttemperEngineSink::SendGameNodeInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameNode))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		//��������
		if ((wKindID==INVALID_WORD)||(pGameNodeItem->m_GameNode.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
			wSendSize+=sizeof(tagGameNode);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,cbDataBuffer,wSendSize);

	return;
}

//���Ͷ���
VOID CAttemperEngineSink::SendGamePageInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePageItem * pGamePageItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGamePageCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGamePage))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PAGE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePageItem=m_ServerListManager.EmunGamePageItem(Position);
		if (pGamePageItem==NULL) break;

		//��������
		if ((wKindID==INVALID_WORD)||(pGamePageItem->m_GamePage.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGamePageItem->m_GamePage,sizeof(tagGamePage));
			wSendSize+=sizeof(tagGamePage);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PAGE,cbDataBuffer,wSendSize);

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendGameServerInfo(DWORD dwSocketID, WORD wKindID, BYTE cbDeviceType)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;
		//֧������
		bool bSuportMobile = CServerRule::IsSuportMobile(pGameServerItem->m_GameServer.dwServerRule);
		bool bSurportPC = CServerRule::IsSuportPC(pGameServerItem->m_GameServer.dwServerRule);

		if(cbDeviceType == DEVICE_TYPE_PC)
		{
			if(bSurportPC == false) continue; 
		}
		else if(cbDeviceType != DEVICE_TYPE_PC)
		{
			if(bSuportMobile == false) continue;
		}

		//��������
		if ((wKindID==INVALID_WORD)||(pGameServerItem->m_GameServer.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameServer,sizeof(tagGameServer));
			wSendSize+=sizeof(tagGameServer);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,cbDataBuffer,wSendSize);
	//���ñ���
	wSendSize=0;
	ZeroMemory(cbDataBuffer,sizeof(cbDataBuffer));

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameMatch))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_MATCH,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;
		if (pGameServerItem->IsMatchServer()==false) continue;

		//��������
		if ((wKindID==INVALID_WORD)||(pGameServerItem->m_GameServer.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameMatch,sizeof(tagGameMatch));
			wSendSize+=sizeof(tagGameMatch);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_MATCH,cbDataBuffer,wSendSize);

	if(m_wAVServerPort!=0 && m_dwAVServerAddr!=0)
	{
		//��������
		tagAVServerOption AVServerOption;
		AVServerOption.wAVServerPort=m_wAVServerPort;
		AVServerOption.dwAVServerAddr=m_dwAVServerAddr;

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_VIDEO_OPTION,&AVServerOption,sizeof(AVServerOption));
	};

	return;
}

//�ȼ�����
VOID CAttemperEngineSink::SendGrowLevelConfig(DWORD dwSocketID)
{
	//����ṹ
	CMD_GP_GrowLevelConfig GrowLevelConfig;
	GrowLevelConfig.wLevelCount = m_wLevelCount;
	CopyMemory(GrowLevelConfig.GrowLevelItem,m_GrowLevelConfig,sizeof(tagGrowLevelConfig)*GrowLevelConfig.wLevelCount);

	//��������
	WORD wDataSize = sizeof(GrowLevelConfig)-sizeof(GrowLevelConfig.GrowLevelItem);
	wDataSize += sizeof(tagGrowLevelConfig)*GrowLevelConfig.wLevelCount;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_GROWLEVEL_CONFIG,&GrowLevelConfig,wDataSize);

	return;
}

//��������
VOID CAttemperEngineSink::SendGamePropertyTypeInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	DWORD dwCount = m_GamePropertyListManager.GetGamePropertyTypeCount();

	//ö������
	for (DWORD i=0;i<dwCount;i++)
	{
		//��������
		if ((wSendSize+sizeof(tagPropertyTypeItem))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePropertyTypeItem=m_GamePropertyListManager.EmunGamePropertyTypeItem(Position);
		if (pGamePropertyTypeItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGamePropertyTypeItem->m_PropertyTypeItem,sizeof(tagPropertyTypeItem));
		wSendSize+=sizeof(tagPropertyTypeItem);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_TYPE,cbDataBuffer,wSendSize);

	return;
}

//���߹�ϵ
VOID CAttemperEngineSink::SendGamePropertyRelatInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;
	DWORD dwCount = m_GamePropertyListManager.GetGamePropertyRelatCount();

	//ö������
	for (DWORD i=0;i<dwCount;i++)
	{
		//��������
		if ((wSendSize+sizeof(tagPropertyRelatItem))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_RELAT,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePropertyRelatItem=m_GamePropertyListManager.EmunGamePropertyRelatItem(Position);
		if (pGamePropertyRelatItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGamePropertyRelatItem->m_PropertyRelatItem,sizeof(tagPropertyRelatItem));
		wSendSize+=sizeof(tagPropertyRelatItem);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_RELAT,cbDataBuffer,wSendSize);


	return;
}

//���͵���
VOID CAttemperEngineSink::SendGamePropertyInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePropertyItem * pGamePropertyItem=NULL;
	DWORD dwCount = m_GamePropertyListManager.GetGamePropertyCount();

	//ö������
	for (DWORD i=0;i<dwCount;i++)
	{
		//��������
		if ((wSendSize+sizeof(tagPropertyItem))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePropertyItem=m_GamePropertyListManager.EmunGamePropertyItem(Position);
		if (pGamePropertyItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGamePropertyItem->m_PropertyItem,sizeof(tagPropertyItem));
		wSendSize+=sizeof(tagPropertyItem);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY,cbDataBuffer,wSendSize);


	return;
}


//���͵���
VOID CAttemperEngineSink::SendGamePropertySubInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePropertySubItem * pGamePropertySubItem=NULL;
	DWORD dwCount = m_GamePropertyListManager.GetGamePropertySubCount();

	//ö������
	for (DWORD i=0;i<dwCount;i++)
	{
		//��������
		if ((wSendSize+sizeof(CGamePropertySubItem))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_SUB,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePropertySubItem=m_GamePropertyListManager.EmunGamePropertySubItem(Position);
		if (pGamePropertySubItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGamePropertySubItem->m_PropertySubItem,sizeof(tagPropertySubItem));
		wSendSize+=sizeof(tagPropertySubItem);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PROPERTY_SUB,cbDataBuffer,wSendSize);


	return;
}

//��Ա����
VOID CAttemperEngineSink::SendMemberConfig(DWORD dwContextID)
{
	//��Ա����
	CMD_GP_MemberParameterResult MemberParameterResult;
	MemberParameterResult.wMemberCount = m_wMemberCount;
	CopyMemory(MemberParameterResult.MemberParameter,m_MemberParameter,sizeof(tagMemberParameterNew)*m_wMemberCount);

	WORD wConfigMemberHead=sizeof(MemberParameterResult)-sizeof(MemberParameterResult.MemberParameter);
	WORD wConfigMemberInfo=MemberParameterResult.wMemberCount*sizeof(MemberParameterResult.MemberParameter[0]);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_MEMBER_PARAMETER_RESULT,&MemberParameterResult,wConfigMemberHead+wConfigMemberInfo);

	return;
}

VOID CAttemperEngineSink::SendRealAuthConfig(DWORD dwContextID)
{
	//ʵ����֤
	CMD_GP_RealAuthParameter CmdParameter;
	CopyMemory(&CmdParameter,&m_AuthRealParameter,sizeof(CmdParameter));
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_REAL_AUTH_CONFIG,&CmdParameter,sizeof(CmdParameter));
	return;
}

//��������
VOID CAttemperEngineSink::SendMobileKindInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;
	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_KIND,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		CopyMemory(cbDataBuffer+wSendSize,&pGameKindItem->m_GameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}

	//����ʣ��
	if (wSendSize>0) 
	{
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_KIND,cbDataBuffer,wSendSize);
	}

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendMobileServerInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;
	CGameKindItem * pGameKindItem=NULL;
	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;



		//֧������
		bool bServerSuportMobile = CServerRule::IsSuportMobile(pGameServerItem->m_GameServer.dwServerRule);

		//��������
		if ((bServerSuportMobile==true)&&((wKindID==INVALID_WORD)||(pGameServerItem->m_GameServer.wKindID==wKindID)))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameServer,sizeof(tagGameServer));
			wSendSize+=sizeof(tagGameServer);
		}
	}

	//����ʣ��
	if (wSendSize>0) 
	{
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER,cbDataBuffer,wSendSize);
	}

	//���ñ���
	wSendSize=0;
	ZeroMemory(cbDataBuffer,sizeof(cbDataBuffer));

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameMatch))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_MATCH,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;
		if (pGameServerItem->IsMatchServer()==false) continue;

		//��������
		if ((wKindID==INVALID_WORD)||(pGameServerItem->m_GameServer.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameMatch,sizeof(tagGameMatch));
			wSendSize+=sizeof(tagGameMatch);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_MATCH,cbDataBuffer,wSendSize);

	return;
}

//����ʧ��
bool CAttemperEngineSink::SendInsureFailure(DWORD dwSocketID, LONG lResultCode, LPCTSTR pszDescribe)
{
	//��������
	CMD_GP_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	UserInsureFailure.lResultCode=lResultCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszDescribe,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDescribe);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

	return true;
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(DWORD dwContextID, LONG lResultCode, LPCTSTR pszDescribe)
{
	//Ч�����
	ASSERT(pszDescribe != NULL);
	if(pszDescribe == NULL) return;

	//��������
	CMD_GP_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	//��������
	OperateSuccess.lResultCode=lResultCode;
	lstrcpyn(OperateSuccess.szDescribeString,pszDescribe,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wHeadSize+wDescribe);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return;
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(DWORD dwContextID, LONG lResultCode, LPCTSTR pszDescribe)
{
	//Ч�����
	ASSERT(pszDescribe != NULL);
	if(pszDescribe == NULL) return;

	//��������
	CMD_GP_OperateFailure OperateFailure;
	ZeroMemory(&OperateFailure,sizeof(OperateFailure));

	//��������
	OperateFailure.lResultCode=lResultCode;
	lstrcpyn(OperateFailure.szDescribeString,pszDescribe,CountArray(OperateFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateFailure.szDescribeString);
	WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_FAILURE,&OperateFailure,wHeadSize+wDescribe);

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return;
}

VOID CAttemperEngineSink::OnChangeDummyOnline()
{
	////�������
	tagServerDummyOnLine * pItemCursor = NULL;


	SYSTEMTIME tempDate;
	GetSystemTime(&tempDate);

	//��ֵ���Ǳ���ʱ��
	tempDate.wYear = 1970;
	tempDate.wMonth = 1;
	tempDate.wDayOfWeek = 1;
	tempDate.wDay = 1;
	tempDate.wMinute;
	tempDate.wHour;

	time_t curTime = SystemTimeToTimet(tempDate);

	//��������
	srand(time(NULL));
	//iMaxAccCount ��ֵԽ�󣬲�������ԽС, iRetifyCount Խ�󣬲�������Խ�� iMaxAccCount > iRetifyCount
	int iMaxAccCount = 5;
	int iRetifyCount = 3;
	//printf("----------------------start update dummy onLine\n");

	for (int i=0;i<m_ServerDummyOnLineArray.GetCount();i++)
	{
		//��������
		pItemCursor=m_ServerDummyOnLineArray.GetAt(i);

		//��ʼ��ֵ
		pItemCursor->dwCurDummyOnline = 0;

		//ʱ����
		int timeSpecStart = (curTime - pItemCursor->lTimeStart);
		int timeSpecEnd = (curTime - pItemCursor->lTimeEnd);

		if (timeSpecStart > 0 && timeSpecEnd < 0)
		{//��ָ��ʱ���ʹ�ñ仯ֵ
			if (pItemCursor->dwDummyMinOnLine > pItemCursor->dwDummyMaxOnLine || (pItemCursor->dwDummyMaxOnLine - pItemCursor->dwDummyMinOnLine == 0))
			{
				if (pItemCursor->dwDummyMaxOnLine != 0)
				{
					pItemCursor->dwCurDummyOnline = pItemCursor->dwDummyMaxOnLine;
				}
				else
				{
					pItemCursor->dwCurDummyOnline = 0;
				}
			}
			else
			{
				//n+1�׵�FIR�˲��� ����ƽ������
				tagServerDummyOnLine * pItemLast = m_ServerDummyOnLineMap[pItemCursor->wServerID];
				if (pItemLast == NULL)
				{
					m_ServerDummyOnLineMap[pItemCursor->wServerID] = pItemCursor;
					//printf("��һ�λ�ȡ�����Map\n");
				}
				else
				{
					if (pItemLast != pItemCursor)
					{
						//��ȡ�ϴε��ۻ��������ۼ���
						pItemCursor->lAccumulationCount = pItemLast->lAccumulationCount;
						pItemCursor->lAccumulation = pItemLast->lAccumulation;
						m_ServerDummyOnLineMap[pItemCursor->wServerID] = pItemCursor;
					}
				}

				//printf(":ServerID��%d, ��ǰʱ���������Χ:%d-%d\n", pItemCursor->wServerID, pItemCursor->dwDummyMinOnLine, pItemCursor->dwDummyMaxOnLine);
				int nRand = pItemCursor->dwDummyMinOnLine + rand() % (pItemCursor->dwDummyMaxOnLine - pItemCursor->dwDummyMinOnLine);
				//printf(":ServerID��%d, ��ǰʱ��������:%d\n", pItemCursor->wServerID, nRand);

				//printf(":ServerID��%d, ��ǰʱ����ۻ���ǰ:%I64d���ۼ�����:%I64d\n", pItemCursor->wServerID, pItemCursor->lAccumulation, pItemCursor->lAccumulationCount);
				pItemCursor->lAccumulationCount = m_ServerDummyOnLineMap[pItemCursor->wServerID]->lAccumulationCount + 1;
				pItemCursor->lAccumulation = m_ServerDummyOnLineMap[pItemCursor->wServerID]->lAccumulation + nRand;
				//printf(":ServerID��%d, ��ǰʱ����ۻ�����:%I64d���ۼ�����:%I64d\n", pItemCursor->wServerID, pItemCursor->lAccumulation, pItemCursor->lAccumulationCount);
				//ƽ������ֵ
				int iAve = pItemCursor->lAccumulation / pItemCursor->lAccumulationCount;
				pItemCursor->dwCurDummyOnline = iAve;
				//printf(":ServerID��%d, ��ǰʱ��ξ�ֵ:%d\n", pItemCursor->wServerID, iAve);
			}

			if (pItemCursor->lAccumulationCount >= iMaxAccCount)
			{
				pItemCursor->lAccumulation -= pItemCursor->dwCurDummyOnline * iRetifyCount;
				pItemCursor->lAccumulationCount -= iRetifyCount;
				//printf(":ServerID��%d, �ﵽ����ۼ����������ۼ���:%I64d,�ۻ���:%I64d\n", pItemCursor->wServerID, pItemCursor->lAccumulation, pItemCursor->lAccumulationCount);
			}
		}
		//��������
	}


	//��Э��ͬ��������������
	WORD wPacketSize=0L;
	POSITION Position=m_ServerDummyOnLineMap.GetStartPosition();
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//������Ϣ
	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_DUMMY_INFO_START);

	//�ռ�����
	while (Position!=NULL)
	{
		//��������
		if ((wPacketSize+sizeof(CMD_CS_C_DummyOnLine))>sizeof(cbBuffer))
		{
			m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_DUMMY_INFO,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		CMD_CS_C_DummyOnLine * pDummyOnLine=(CMD_CS_C_DummyOnLine *)(cbBuffer+wPacketSize);


		tagServerDummyOnLine * pDummyOnline = NULL;
		WORD wServerID;
		//��ȡ����
		m_ServerDummyOnLineMap.GetNextAssoc(Position,wServerID,pDummyOnline);

		//��������
		pDummyOnLine->wServerID = wServerID;
		pDummyOnLine->dwDummyCount = pDummyOnline->dwCurDummyOnline;

		//�ۼ�����
		wPacketSize+=sizeof(CMD_CS_C_DummyOnLine);
	};

	//��������
	if (wPacketSize>0)
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_DUMMY_INFO,cbBuffer,wPacketSize);

	m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_S_SERVER_DUMMY_INFO_END);
}
//////////////////////////////////////////////////////////////////////////////////
