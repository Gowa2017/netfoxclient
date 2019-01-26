#include "StdAfx.h"
#include "AsynchronismEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAsynchronismThread::CAsynchronismThread()
{
	//运行变量
	m_hCompletionPort=NULL;
	m_pIAsynchronismEngineSink=NULL;

	//辅助变量
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//析构函数
CAsynchronismThread::~CAsynchronismThread()
{
}

//配置函数
VOID CAsynchronismThread::SetCompletionPort(HANDLE hCompletionPort) 
{ 
	//设置变量
	m_hCompletionPort=hCompletionPort; 

	return;
}

//设置接口
VOID CAsynchronismThread::SetAsynchronismEngineSink(IAsynchronismEngineSink * pIAsynchronismEngineSink)
{
	//设置变量
	m_pIAsynchronismEngineSink=pIAsynchronismEngineSink;

	return;
}

//运行函数
bool CAsynchronismThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);
	ASSERT(m_pIAsynchronismEngineSink!=NULL);

	//变量定义
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	CAsynchronismEngine * pAsynchronismEngine=NULL;

	//完成端口
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pAsynchronismEngine,&pOverLapped,INFINITE))
	{
		//退出判断
		if (pAsynchronismEngine==NULL) return false;

		//队列锁定
		CWHDataLocker ThreadLock(pAsynchronismEngine->m_CriticalSection);

		//提取数据
		tagDataHead DataHead;
		pAsynchronismEngine->m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));

		//队列解锁
		ThreadLock.UnLock();

		//数据处理
		try
		{
			m_pIAsynchronismEngineSink->OnAsynchronismEngineData(DataHead.wIdentifier,m_cbBuffer,DataHead.wDataSize);
		}
		catch (...)
		{
			//构造信息
			TCHAR szDescribe[256]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("CAsynchronismEngine::OnAsynchronismEngineData [ wIdentifier=%d wDataSize=%ld ]"),
				DataHead.wIdentifier,DataHead.wDataSize);

			//输出信息
			g_TraceServiceManager.TraceString(szDescribe,TraceLevel_Exception);
		}

		return true;
	}

	return false;
}

//开始事件
bool CAsynchronismThread::OnEventThreadStrat()
{
	//事件通知
	ASSERT(m_pIAsynchronismEngineSink!=NULL);
	bool bSuccess=m_pIAsynchronismEngineSink->OnAsynchronismEngineStart();

	//设置变量
	CAsynchronismEngine * pAsynchronismEngine=CONTAINING_RECORD(this,CAsynchronismEngine,m_AsynchronismThread);
	pAsynchronismEngine->m_bService=true;

	return bSuccess;
}

//停止事件
bool CAsynchronismThread::OnEventThreadConclude()
{
	//设置变量
	CAsynchronismEngine * pAsynchronismEngine=CONTAINING_RECORD(this,CAsynchronismEngine,m_AsynchronismThread);
	pAsynchronismEngine->m_bService=false;

	//事件通知
	ASSERT(m_pIAsynchronismEngineSink!=NULL);
	bool bSuccess=m_pIAsynchronismEngineSink->OnAsynchronismEngineConclude();

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CAsynchronismEngine::CAsynchronismEngine()
{
	//设置变量
	m_bService=false;
	m_hCompletionPort=NULL;
	m_pIAsynchronismEngineSink=NULL;

	return;
}

//析构函数
CAsynchronismEngine::~CAsynchronismEngine()
{
	//终止服务
	ConcludeService();

	return;
}

//接口查询
VOID * CAsynchronismEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IAsynchronismEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsynchronismEngine,Guid,dwQueryVer);
	return NULL;
}

//队列负荷
bool CAsynchronismEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CWHDataLocker lock(m_CriticalSection);//
	m_DataQueue.GetBurthenInfo(BurthenInfo);

	return true;
}


//设置模块
bool CAsynchronismEngine::SetAsynchronismSink(IUnknownEx * pIUnknownEx)
{
	//运行判断
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//查询接口
	m_pIAsynchronismEngineSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAsynchronismEngineSink);

	//错误判断
	if (m_pIAsynchronismEngineSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//异步数据
bool CAsynchronismEngine::PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//运行判断
	ASSERT((m_bService==true)&&(m_hCompletionPort!=NULL));
	if ((m_hCompletionPort==NULL)||(m_bService==false)) return false;

	//插入数据
	CWHDataLocker ThreadLock(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//投递通知
	PostQueuedCompletionStatus(m_hCompletionPort,wDataSize,(ULONG_PTR)this,NULL);

	return true;
}

//投递数据
bool CAsynchronismEngine::PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	ASSERT(wDataCount > 0);
	if (wDataCount == 0) return false;

	//插入数据
	CWHDataLocker lcok(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount) == false) return false;

	for (WORD i = 0; i < wDataCount; i++)
	{
		PostQueuedCompletionStatus(m_hCompletionPort,0,(ULONG_PTR)this,NULL);
	}

	return true;
}

//启动服务
bool CAsynchronismEngine::StartService()
{
	//运行判断
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//完成端口
	m_hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);
	if (m_hCompletionPort==NULL) return false;

	//设置对象
	m_AsynchronismThread.SetCompletionPort(m_hCompletionPort);
	m_AsynchronismThread.SetAsynchronismEngineSink(m_pIAsynchronismEngineSink);

	//启动线程
	if (m_AsynchronismThread.StartThread()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//停止服务
bool CAsynchronismEngine::ConcludeService()
{
	//设置变量
	m_bService=false;

	//投递通知
	if (m_hCompletionPort!=NULL) 
	{
		PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,NULL);
	}

	//停止线程
	m_AsynchronismThread.ConcludeThread(INFINITE);

	//关闭对象
	if (m_hCompletionPort!=NULL) CloseHandle(m_hCompletionPort);

	//设置变量
	m_hCompletionPort=NULL;
	m_pIAsynchronismEngineSink=NULL;

	//设置对象
	m_AsynchronismThread.SetCompletionPort(NULL);
	m_AsynchronismThread.SetAsynchronismEngineSink(NULL);

	//设置数据
	m_DataQueue.RemoveData(false);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(AsynchronismEngine);

//////////////////////////////////////////////////////////////////////////
