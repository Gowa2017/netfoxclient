#ifndef TRACE_SERVICE_MANAGER_HEAD_FILE
#define TRACE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//�¼��������
class CTraceServiceManager : public ITraceServiceManager
{
	//���ñ���
protected:
	bool							m_bTraceFlag[5];					//�����־

	//�ں˱���
protected:
	ITraceService *					m_pITraceService;					//�¼�����
	CCriticalSection				m_CriticalSection;					//��������

	//��������
public:
	//���캯��
	CTraceServiceManager();
	//��������
	virtual ~CTraceServiceManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//״̬����
public:
	//���״̬
	virtual bool IsEnableTrace(enTraceLevel TraceLevel);
	//�������
	virtual bool EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace);

	//��������
public:
	//���ýӿ�
	virtual bool SetTraceService(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual VOID * GetTraceService(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//�ִ����
	virtual bool TraceString(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

//ȫ�ֱ���
extern CTraceServiceManager g_TraceServiceManager;

//////////////////////////////////////////////////////////////////////////

#endif