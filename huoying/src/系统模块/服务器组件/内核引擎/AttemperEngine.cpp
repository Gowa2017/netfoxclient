#include "StdAfx.h"
#include "AttemperEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngine::CAttemperEngine()
{
	//接口变量
	m_pITCPNetworkEngine=NULL;
	m_pIAttemperEngineSink=NULL;

	//辅助变量
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//析构函数
CAttemperEngine::~CAttemperEngine()
{
}

//接口查询
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

//启动服务
bool CAttemperEngine::StartService()
{
	//效验参数
	ASSERT((m_pITCPNetworkEngine!=NULL)&&(m_pIAttemperEngineSink!=NULL));
	if ((m_pITCPNetworkEngine==NULL)||(m_pIAttemperEngineSink==NULL)) return false;

	//注册对象
	IUnknownEx * pIAsynchronismEngineSink=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_AsynchronismEngine.SetAsynchronismSink(pIAsynchronismEngineSink)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//异步引擎
	if (m_AsynchronismEngine.StartService()==false) return false;

	return true;
}

//停止服务
bool CAttemperEngine::ConcludeService()
{
	//异步引擎
	m_AsynchronismEngine.ConcludeService();

	return true;
}

//网络接口
bool CAttemperEngine::SetNetworkEngine(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine)!=NULL);
		m_pITCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine);

		//成功判断
		if (m_pITCPNetworkEngine==NULL) return false;
	}
	else m_pITCPNetworkEngine=NULL;

	return true;
}

//回调接口
bool CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAttemperEngineSink)!=NULL);
	m_pIAttemperEngineSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAttemperEngineSink);

	//结果判断
	if (m_pIAttemperEngineSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//自定事件
bool CAttemperEngine::OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wRequestID&EVENT_MASK_CUSTOM)==wRequestID);
	if ((wRequestID&EVENT_MASK_CUSTOM)!=wRequestID) return false;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(wRequestID,pData,wDataSize);
}

//控制消息
bool CAttemperEngine::OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize+sizeof(NTY_ControlEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_ControlEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)m_cbBuffer;
	
	//构造数据
	pControlEvent->wControlID=wControlID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_ControlEvent),pData,wDataSize);
	}

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_CONTROL,m_cbBuffer,sizeof(NTY_ControlEvent)+wDataSize);
}

//定时器事件
bool CAttemperEngine::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)m_cbBuffer;
	
	//构造数据
	pTimerEvent->dwTimerID=dwTimerID;
	pTimerEvent->dwBindParameter=dwBindParameter;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TIMER,m_cbBuffer,sizeof(NTY_TimerEvent));
}

//数据库结果
bool CAttemperEngine::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize+sizeof(NTY_DataBaseEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_DataBaseEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)m_cbBuffer;
	
	//构造数据
	pDataBaseEvent->wRequestID=wRequestID;
	pDataBaseEvent->dwContextID=dwContextID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_DataBaseEvent),pData,wDataSize);
	}

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_DATABASE,m_cbBuffer,sizeof(NTY_DataBaseEvent)+wDataSize);
}

//连接事件
bool CAttemperEngine::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketLinkEvent * pConnectEvent=(NTY_TCPSocketLinkEvent *)m_cbBuffer;
	
	//构造数据
	pConnectEvent->wServiceID=wServiceID;
	pConnectEvent->nErrorCode=nErrorCode;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_LINK,m_cbBuffer,sizeof(NTY_TCPSocketLinkEvent));
}

//关闭事件
bool CAttemperEngine::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketShutEvent * pCloseEvent=(NTY_TCPSocketShutEvent *)m_cbBuffer;
	
	//构造数据
	pCloseEvent->wServiceID=wServiceID;
	pCloseEvent->cbShutReason=cbShutReason;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_SHUT,m_cbBuffer,sizeof(NTY_TCPSocketShutEvent));
}

//读取事件
bool CAttemperEngine::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize+sizeof(NTY_TCPSocketReadEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_TCPSocketReadEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)m_cbBuffer;
	
	//构造数据
	pReadEvent->Command=Command;
	pReadEvent->wDataSize=wDataSize;
	pReadEvent->wServiceID=wServiceID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_TCPSocketReadEvent),pData,wDataSize);
	}

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_READ,m_cbBuffer,sizeof(NTY_TCPSocketReadEvent)+wDataSize);
}

//应答事件
bool CAttemperEngine::OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)m_cbBuffer;
	
	//构造数据
	pAcceptEvent->dwSocketID=dwSocketID;
	pAcceptEvent->dwClientAddr=dwClientAddr;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_ACCEPT,m_cbBuffer,sizeof(NTY_TCPNetworkAcceptEvent));
}

//关闭事件
bool CAttemperEngine::OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkShutEvent * pCloseEvent=(NTY_TCPNetworkShutEvent *)m_cbBuffer;
	
	//构造数据
	pCloseEvent->dwSocketID=dwSocketID;
	pCloseEvent->dwClientAddr=dwClientAddr;
	pCloseEvent->dwActiveTime=dwActiveTime;

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_SHUT,m_cbBuffer,sizeof(NTY_TCPNetworkShutEvent));
}

//读取事件
bool CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize+sizeof(NTY_TCPNetworkReadEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_TCPNetworkReadEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)m_cbBuffer;
	
	//构造数据
	pReadEvent->Command=Command;
	pReadEvent->wDataSize=wDataSize;
	pReadEvent->dwSocketID=dwSocketID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_TCPNetworkReadEvent),pData,wDataSize);
	}

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_READ,m_cbBuffer,sizeof(NTY_TCPNetworkReadEvent)+wDataSize);
}

//启动事件
bool CAttemperEngine::OnAsynchronismEngineStart()
{
	//效验参数
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//启动通知
	if (m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//停止事件
bool CAttemperEngine::OnAsynchronismEngineConclude()
{
	//效验参数
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//停止通知
	if (m_pIAttemperEngineSink->OnAttemperEngineConclude(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//异步数据
bool CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);
	ASSERT(m_pIAttemperEngineSink!=NULL);

	//内核事件
	switch (wIdentifier)
	{
	case EVENT_TIMER:				//时间事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TimerEvent));
			if (wDataSize!=sizeof(NTY_TimerEvent)) return false;

			//处理消息
			NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)pData;
			m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID,pTimerEvent->dwBindParameter);

			return true;
		}
	case EVENT_CONTROL:				//控制消息
		{
			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_ControlEvent));
			if (wDataSize<sizeof(NTY_ControlEvent)) return false;

			//处理消息
			NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)pData;
			m_pIAttemperEngineSink->OnEventControl(pControlEvent->wControlID,pControlEvent+1,wDataSize-sizeof(NTY_ControlEvent));

			return true;
		}
	case EVENT_DATABASE:			//数据库事件
		{
			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_DataBaseEvent));
			if (wDataSize<sizeof(NTY_DataBaseEvent)) return false;

			//处理消息
			NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)pData;
			m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID,pDataBaseEvent->dwContextID,pDataBaseEvent+1,wDataSize-sizeof(NTY_DataBaseEvent));

			return true;
		}
	case EVENT_TCP_SOCKET_READ:		//读取事件
		{
			//变量定义
			NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)pData;

			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_TCPSocketReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize));

			//大小效验
			if (wDataSize<sizeof(NTY_TCPSocketReadEvent)) return false;
			if (wDataSize!=(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize)) return false;

			//处理消息
			m_pIAttemperEngineSink->OnEventTCPSocketRead(pReadEvent->wServiceID,pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize);

			return true;
		}
	case EVENT_TCP_SOCKET_SHUT:	//关闭事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPSocketShutEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketShutEvent)) return false;

			//处理消息
			NTY_TCPSocketShutEvent * pCloseEvent=(NTY_TCPSocketShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketShut(pCloseEvent->wServiceID,pCloseEvent->cbShutReason);

			return true;
		}
	case EVENT_TCP_SOCKET_LINK:	//连接事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPSocketLinkEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketLinkEvent)) return false;

			//处理消息
			NTY_TCPSocketLinkEvent * pConnectEvent=(NTY_TCPSocketLinkEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketLink(pConnectEvent->wServiceID,pConnectEvent->nErrorCode);

			return true;
		}
	case EVENT_TCP_NETWORK_ACCEPT:	//应答事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkAcceptEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkAcceptEvent)) return false;

			//变量定义
			bool bSuccess=false;
			NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)pData;

			//处理消息
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientAddr,pAcceptEvent->dwSocketID);
			}
			catch (...)	{ }

			//失败处理
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_READ:	//读取事件
		{
			//效验大小
			NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)pData;

			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_TCPNetworkReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize));

			//大小效验
			if (wDataSize<sizeof(NTY_TCPNetworkReadEvent))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//大小效验
			if (wDataSize!=(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//处理消息
			bool bSuccess=false;
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize,pReadEvent->dwSocketID);
			}
			catch (...)	{ }

			//失败处理
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_SHUT:	//关闭事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkShutEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkShutEvent)) return false;

			//处理消息
			NTY_TCPNetworkShutEvent * pCloseEvent=(NTY_TCPNetworkShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPNetworkShut(pCloseEvent->dwClientAddr,pCloseEvent->dwActiveTime,pCloseEvent->dwSocketID);

			return true;
		}
	}

	//其他事件
	return m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier,pData,wDataSize); 
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(AttemperEngine);

//////////////////////////////////////////////////////////////////////////
