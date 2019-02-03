#include "StdAfx.h"
#include "AttemperEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngine::CAttemperEngine()
{
	//�ӿڱ���
	m_pITCPNetworkEngine=NULL;
	m_pIAttemperEngineSink=NULL;

	//��������
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//��������
CAttemperEngine::~CAttemperEngine()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketEvent,Guid,dwQueryVer);
	QUERYINTERFACE(ITimerEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(IAsynchronismEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngine,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CAttemperEngine::StartService()
{
	//Ч�����
	ASSERT((m_pITCPNetworkEngine!=NULL)&&(m_pIAttemperEngineSink!=NULL));
	if ((m_pITCPNetworkEngine==NULL)||(m_pIAttemperEngineSink==NULL)) return false;

	//ע�����
	IUnknownEx * pIAsynchronismEngineSink=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_AsynchronismEngine.SetAsynchronismSink(pIAsynchronismEngineSink)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//�첽����
	if (m_AsynchronismEngine.StartService()==false) return false;

	return true;
}

//ֹͣ����
bool CAttemperEngine::ConcludeService()
{
	//�첽����
	m_AsynchronismEngine.ConcludeService();

	return true;
}

//����ӿ�
bool CAttemperEngine::SetNetworkEngine(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine)!=NULL);
		m_pITCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine);

		//�ɹ��ж�
		if (m_pITCPNetworkEngine==NULL) return false;
	}
	else m_pITCPNetworkEngine=NULL;

	return true;
}

//�ص��ӿ�
bool CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAttemperEngineSink)!=NULL);
	m_pIAttemperEngineSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAttemperEngineSink);

	//����ж�
	if (m_pIAttemperEngineSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�Զ��¼�
bool CAttemperEngine::OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wRequestID&EVENT_MASK_CUSTOM)==wRequestID);
	if ((wRequestID&EVENT_MASK_CUSTOM)!=wRequestID) return false;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(wRequestID,pData,wDataSize);
}

//������Ϣ
bool CAttemperEngine::OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_ControlEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_ControlEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)m_cbBuffer;
	
	//��������
	pControlEvent->wControlID=wControlID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_ControlEvent),pData,wDataSize);
	}

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_CONTROL,m_cbBuffer,sizeof(NTY_ControlEvent)+wDataSize);
}

//��ʱ���¼�
bool CAttemperEngine::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)m_cbBuffer;
	
	//��������
	pTimerEvent->dwTimerID=dwTimerID;
	pTimerEvent->dwBindParameter=dwBindParameter;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TIMER,m_cbBuffer,sizeof(NTY_TimerEvent));
}

//���ݿ���
bool CAttemperEngine::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_DataBaseEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_DataBaseEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)m_cbBuffer;
	
	//��������
	pDataBaseEvent->wRequestID=wRequestID;
	pDataBaseEvent->dwContextID=dwContextID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_DataBaseEvent),pData,wDataSize);
	}

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_DATABASE,m_cbBuffer,sizeof(NTY_DataBaseEvent)+wDataSize);
}

//�����¼�
bool CAttemperEngine::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketLinkEvent * pConnectEvent=(NTY_TCPSocketLinkEvent *)m_cbBuffer;
	
	//��������
	pConnectEvent->wServiceID=wServiceID;
	pConnectEvent->nErrorCode=nErrorCode;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_LINK,m_cbBuffer,sizeof(NTY_TCPSocketLinkEvent));
}

//�ر��¼�
bool CAttemperEngine::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketShutEvent * pCloseEvent=(NTY_TCPSocketShutEvent *)m_cbBuffer;
	
	//��������
	pCloseEvent->wServiceID=wServiceID;
	pCloseEvent->cbShutReason=cbShutReason;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_SHUT,m_cbBuffer,sizeof(NTY_TCPSocketShutEvent));
}

//��ȡ�¼�
bool CAttemperEngine::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_TCPSocketReadEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_TCPSocketReadEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)m_cbBuffer;
	
	//��������
	pReadEvent->Command=Command;
	pReadEvent->wDataSize=wDataSize;
	pReadEvent->wServiceID=wServiceID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_TCPSocketReadEvent),pData,wDataSize);
	}

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_READ,m_cbBuffer,sizeof(NTY_TCPSocketReadEvent)+wDataSize);
}

//Ӧ���¼�
bool CAttemperEngine::OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)m_cbBuffer;
	
	//��������
	pAcceptEvent->dwSocketID=dwSocketID;
	pAcceptEvent->dwClientAddr=dwClientAddr;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_ACCEPT,m_cbBuffer,sizeof(NTY_TCPNetworkAcceptEvent));
}

//�ر��¼�
bool CAttemperEngine::OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkShutEvent * pCloseEvent=(NTY_TCPNetworkShutEvent *)m_cbBuffer;
	
	//��������
	pCloseEvent->dwSocketID=dwSocketID;
	pCloseEvent->dwClientAddr=dwClientAddr;
	pCloseEvent->dwActiveTime=dwActiveTime;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_SHUT,m_cbBuffer,sizeof(NTY_TCPNetworkShutEvent));
}

//��ȡ�¼�
bool CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_TCPNetworkReadEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_TCPNetworkReadEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//��������
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)m_cbBuffer;
	
	//��������
	pReadEvent->Command=Command;
	pReadEvent->wDataSize=wDataSize;
	pReadEvent->dwSocketID=dwSocketID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_TCPNetworkReadEvent),pData,wDataSize);
	}

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_READ,m_cbBuffer,sizeof(NTY_TCPNetworkReadEvent)+wDataSize);
}

//�����¼�
bool CAttemperEngine::OnAsynchronismEngineStart()
{
	//Ч�����
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//����֪ͨ
	if (m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CAttemperEngine::OnAsynchronismEngineConclude()
{
	//Ч�����
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//ֹ֪ͣͨ
	if (m_pIAttemperEngineSink->OnAttemperEngineConclude(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�첽����
bool CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(m_pITCPNetworkEngine!=NULL);
	ASSERT(m_pIAttemperEngineSink!=NULL);

	//�ں��¼�
	switch (wIdentifier)
	{
	case EVENT_TIMER:				//ʱ���¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TimerEvent));
			if (wDataSize!=sizeof(NTY_TimerEvent)) return false;

			//������Ϣ
			NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)pData;
			m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID,pTimerEvent->dwBindParameter);

			return true;
		}
	case EVENT_CONTROL:				//������Ϣ
		{
			//��С����
			ASSERT(wDataSize>=sizeof(NTY_ControlEvent));
			if (wDataSize<sizeof(NTY_ControlEvent)) return false;

			//������Ϣ
			NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)pData;
			m_pIAttemperEngineSink->OnEventControl(pControlEvent->wControlID,pControlEvent+1,wDataSize-sizeof(NTY_ControlEvent));

			return true;
		}
	case EVENT_DATABASE:			//���ݿ��¼�
		{
			//��С����
			ASSERT(wDataSize>=sizeof(NTY_DataBaseEvent));
			if (wDataSize<sizeof(NTY_DataBaseEvent)) return false;

			//������Ϣ
			NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)pData;
			m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID,pDataBaseEvent->dwContextID,pDataBaseEvent+1,wDataSize-sizeof(NTY_DataBaseEvent));

			return true;
		}
	case EVENT_TCP_SOCKET_READ:		//��ȡ�¼�
		{
			//��������
			NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)pData;

			//��С����
			ASSERT(wDataSize>=sizeof(NTY_TCPSocketReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize));

			//��СЧ��
			if (wDataSize<sizeof(NTY_TCPSocketReadEvent)) return false;
			if (wDataSize!=(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize)) return false;

			//������Ϣ
			m_pIAttemperEngineSink->OnEventTCPSocketRead(pReadEvent->wServiceID,pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize);

			return true;
		}
	case EVENT_TCP_SOCKET_SHUT:	//�ر��¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPSocketShutEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketShutEvent)) return false;

			//������Ϣ
			NTY_TCPSocketShutEvent * pCloseEvent=(NTY_TCPSocketShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketShut(pCloseEvent->wServiceID,pCloseEvent->cbShutReason);

			return true;
		}
	case EVENT_TCP_SOCKET_LINK:	//�����¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPSocketLinkEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketLinkEvent)) return false;

			//������Ϣ
			NTY_TCPSocketLinkEvent * pConnectEvent=(NTY_TCPSocketLinkEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketLink(pConnectEvent->wServiceID,pConnectEvent->nErrorCode);

			return true;
		}
	case EVENT_TCP_NETWORK_ACCEPT:	//Ӧ���¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkAcceptEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkAcceptEvent)) return false;

			//��������
			bool bSuccess=false;
			NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)pData;

			//������Ϣ
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientAddr,pAcceptEvent->dwSocketID);
			}
			catch (...)	{ }

			//ʧ�ܴ���
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_READ:	//��ȡ�¼�
		{
			//Ч���С
			NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)pData;

			//��С����
			ASSERT(wDataSize>=sizeof(NTY_TCPNetworkReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize));

			//��СЧ��
			if (wDataSize<sizeof(NTY_TCPNetworkReadEvent))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//��СЧ��
			if (wDataSize!=(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//������Ϣ
			bool bSuccess=false;
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize,pReadEvent->dwSocketID);
			}
			catch (...)	{ }

			//ʧ�ܴ���
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_SHUT:	//�ر��¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkShutEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkShutEvent)) return false;

			//������Ϣ
			NTY_TCPNetworkShutEvent * pCloseEvent=(NTY_TCPNetworkShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPNetworkShut(pCloseEvent->dwClientAddr,pCloseEvent->dwActiveTime,pCloseEvent->dwSocketID);

			return true;
		}
	}

	//�����¼�
	return m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier,pData,wDataSize); 
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(AttemperEngine);

//////////////////////////////////////////////////////////////////////////
