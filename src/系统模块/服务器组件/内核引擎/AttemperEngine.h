#ifndef ATTEMPER_ENGINE_HEAD_FILE
#define ATTEMPER_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////

//��������
class CAttemperEngine : public IAttemperEngine, public IAsynchronismEngineSink,
	public IDataBaseEngineEvent, public ITimerEngineEvent, public ITCPSocketEvent,
	public ITCPNetworkEngineEvent
{
	//�������
protected:
	CCriticalSection				m_CriticalLocker;					//��������
	CAsynchronismEngine				m_AsynchronismEngine;				//�첽����

	//�ӿڱ���
protected:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//����ӿ�
	IAttemperEngineSink	*			m_pIAttemperEngineSink;				//���ӽӿ�

	//��������
protected:
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//��ʱ����

	//��������
public:
	//���캯��
	CAttemperEngine();
	//��������
	virtual ~CAttemperEngine();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���ýӿ�
public:
	//����ӿ�
	virtual bool SetNetworkEngine(IUnknownEx * pIUnknownEx);
	//�ص��ӿ�
	virtual bool SetAttemperEngineSink(IUnknownEx * pIUnknownEx);

	//�����¼�
public:
	//�Զ��¼�
	virtual bool OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//��ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ���
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAsynchronismEngineStart();
	//ֹͣ�¼�
	virtual bool OnAsynchronismEngineConclude();
	//�첽����
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif