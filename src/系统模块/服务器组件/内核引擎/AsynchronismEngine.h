#ifndef ASYNCHRONISM_ENGINE_HEAD_FILE
#define ASYNCHRONISM_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CAsynchronismEngine;

//�����߳�
class CAsynchronismThread : public CWHThread
{
	//���б���
protected:
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	IAsynchronismEngineSink	*		m_pIAsynchronismEngineSink;			//�ص��ӿ�

	//��������
private:
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//���ջ���

	//��������
public:
	//���캯��
	CAsynchronismThread();
	//��������
	virtual ~CAsynchronismThread();

	//���ܺ���
public:
	//���ú���
	VOID SetCompletionPort(HANDLE hCompletionPort);
	//���ýӿ�
	VOID SetAsynchronismEngineSink(IAsynchronismEngineSink * pIAsynchronismEngineSink);

	//���غ���
public:
	//���к���
	virtual bool OnEventThreadRun();
	//��ʼ�¼�
	virtual bool OnEventThreadStrat();
	//��ֹ�¼�
	virtual bool OnEventThreadConclude();
};

//////////////////////////////////////////////////////////////////////////

//�첽����
class CAsynchronismEngine : public IAsynchronismEngine
{
	//��Ԫ����
	friend class CAsynchronismThread;

	//�ں˱���
protected:
	bool							m_bService;							//�����־
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	IAsynchronismEngineSink *		m_pIAsynchronismEngineSink;			//�ص��ӿ�

	//�������
protected:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	CCriticalSection				m_CriticalSection;					//��������
	CAsynchronismThread				m_AsynchronismThread;				//�̶߳���

	//��������
public:
	//���캯��
	CAsynchronismEngine();
	//��������
	virtual ~CAsynchronismEngine();

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

	//�첽�ӿ�
public:
	//���и���
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);
	//����ģ��
	virtual bool SetAsynchronismSink(IUnknownEx * pIUnknownEx);
	//�첽����
	virtual bool PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//Ͷ������
	virtual bool PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount);
};

//////////////////////////////////////////////////////////////////////////

#endif