#include "StdAfx.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CTraceServiceManager::CTraceServiceManager()
{
	//���ñ�־
	m_bTraceFlag[TraceLevel_Info]=true;
	m_bTraceFlag[TraceLevel_Debug]=true;
	m_bTraceFlag[TraceLevel_Normal]=true;
	m_bTraceFlag[TraceLevel_Warning]=true;
	m_bTraceFlag[TraceLevel_Exception]=true;

	//���ýӿ�
	m_pITraceService=NULL;

	return;
}

//��������
CTraceServiceManager::~CTraceServiceManager()
{
}

//�ӿڲ�ѯ
VOID * CTraceServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceServiceManager,Guid,dwQueryVer);
	return NULL;
}

//���״̬
bool CTraceServiceManager::IsEnableTrace(enTraceLevel TraceLevel)
{
	//Ч�����
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	return m_bTraceFlag[TraceLevel];
}

//�������
bool CTraceServiceManager::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	//Ч�����
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	//���ò���
	m_bTraceFlag[TraceLevel]=bEnableTrace;

	return true;
}

//���ýӿ�
bool CTraceServiceManager::SetTraceService(IUnknownEx * pIUnknownEx)
{
	//ͬ������
	CWHDataLocker ThreadLock(m_CriticalSection);

	//ע���ӿ�
	if (pIUnknownEx==NULL)
	{
		m_pITraceService=NULL;
		return true;
	}

	//���ýӿ�
	m_pITraceService=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITraceService);
	ASSERT(m_pITraceService!=NULL);

	return (m_pITraceService!=NULL);
}

//��ȡ�ӿ�
VOID * CTraceServiceManager::GetTraceService(REFGUID Guid, DWORD dwQueryVer)
{
	//ͬ������
	CWHDataLocker ThreadLock(m_CriticalSection);

	//��ȡ�ӿ�
	if (m_pITraceService==NULL) return NULL;
	return m_pITraceService->QueryInterface(Guid,dwQueryVer);
}

//�ִ����
bool CTraceServiceManager::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//Ч�����
	ASSERT(TraceLevel<=TraceLevel_Debug);
	if (TraceLevel>TraceLevel_Debug) return false;

	//ͬ������
	CWHDataLocker ThreadLock(m_CriticalSection);

	//״̬�ж�
	if ((m_pITraceService==NULL)||(m_bTraceFlag[TraceLevel]==false)) return false;

	//�����Ϣ
	return m_pITraceService->TraceString(pszString,TraceLevel);
}

//////////////////////////////////////////////////////////////////////////

//�������
CTraceServiceManager g_TraceServiceManager;

//�����������
extern "C" __declspec(dllexport) VOID * CreateTraceServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_TraceServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
