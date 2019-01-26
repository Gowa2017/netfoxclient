#include "StdAfx.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CTraceServiceManager::CTraceServiceManager()
{
	//设置标志
	m_bTraceFlag[TraceLevel_Info]=true;
	m_bTraceFlag[TraceLevel_Debug]=true;
	m_bTraceFlag[TraceLevel_Normal]=true;
	m_bTraceFlag[TraceLevel_Warning]=true;
	m_bTraceFlag[TraceLevel_Exception]=true;

	//设置接口
	m_pITraceService=NULL;

	return;
}

//析构函数
CTraceServiceManager::~CTraceServiceManager()
{
}

//接口查询
VOID * CTraceServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceServiceManager,Guid,dwQueryVer);
	return NULL;
}

//输出状态
bool CTraceServiceManager::IsEnableTrace(enTraceLevel TraceLevel)
{
	//效验参数
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	return m_bTraceFlag[TraceLevel];
}

//输出控制
bool CTraceServiceManager::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	//效验参数
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	//设置参数
	m_bTraceFlag[TraceLevel]=bEnableTrace;

	return true;
}

//设置接口
bool CTraceServiceManager::SetTraceService(IUnknownEx * pIUnknownEx)
{
	//同步锁定
	CWHDataLocker ThreadLock(m_CriticalSection);

	//注销接口
	if (pIUnknownEx==NULL)
	{
		m_pITraceService=NULL;
		return true;
	}

	//设置接口
	m_pITraceService=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITraceService);
	ASSERT(m_pITraceService!=NULL);

	return (m_pITraceService!=NULL);
}

//获取接口
VOID * CTraceServiceManager::GetTraceService(REFGUID Guid, DWORD dwQueryVer)
{
	//同步锁定
	CWHDataLocker ThreadLock(m_CriticalSection);

	//获取接口
	if (m_pITraceService==NULL) return NULL;
	return m_pITraceService->QueryInterface(Guid,dwQueryVer);
}

//字串输出
bool CTraceServiceManager::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//效验参数
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	//同步锁定
	CWHDataLocker ThreadLock(m_CriticalSection);

	//状态判断
	if ((m_pITraceService==NULL)||(m_bTraceFlag[TraceLevel]==false)) return false;

	//输出消息
	return m_pITraceService->TraceString(pszString,TraceLevel);
}

//////////////////////////////////////////////////////////////////////////

//输出管理
CTraceServiceManager g_TraceServiceManager;

//组件创建函数
extern "C" __declspec(dllexport) VOID * CreateTraceServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_TraceServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
