#ifndef TIMER_ENGINE_HEAD_FILE
#define TIMER_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//��˵��
class CTimerEngine;

//////////////////////////////////////////////////////////////////////////

//��ʱ���߳�
class CTimerThread : public CWHThread
{
	//��������
protected:
	DWORD							m_dwTimerSpace;						//ʱ����
	DWORD							m_dwLastTickCount;					//����ʱ��

	//�ӿڱ���
protected:
	CTimerEngine *					m_pTimerEngine;						//��ʱ������

	//��������
public:
	//���캯��
	CTimerThread();
	//��������
	virtual ~CTimerThread();

	//���ܺ���
public:
	//���ú���
	bool InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//��ʱ������
struct tagTimerItem
{
	//ʱ�����
	DWORD							dwElapse;							//����ʱ��
	DWORD							dwTimerID;							//ʱ���ʶ
	DWORD							dwTimeLeave;						//ʣ��ʱ��
	DWORD							dwRepeatTimes;						//�ظ�����
	WPARAM							dwBindParameter;					//�󶨲���
};

//////////////////////////////////////////////////////////////////////////

//��˵��
typedef CWHArray<tagTimerItem *> CTimerItemArray;					//��������

//��ʱ������
class CTimerEngine : public ITimerEngine
{
	friend class CTimerThread;

	//״̬����
protected:
	bool							m_bService;							//���б�־
	DWORD							m_dwTimePass;						//����ʱ��
	DWORD							m_dwTimeLeave;						//����ʱ��

	//���ö���
protected:
	DWORD							m_dwTimerSpace;						//ʱ����
	ITimerEngineEvent *				m_pITimerEngineEvent;				//�¼��ӿ�

	//�������
protected:
	CTimerThread					m_TimerThread;						//�̶߳���
	CTimerItemArray					m_TimerItemFree;					//��������
	CTimerItemArray					m_TimerItemActive;					//�����
	CCriticalSection				m_CriticalSection;					//��������

	//��������
public:
	//���캯��
	CTimerEngine();
	//��������
	virtual ~CTimerEngine();

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
	//���ýӿ�
	virtual bool SetTimerEngineEvent(IUnknownEx * pIUnknownEx);

	//�ӿں���
public:
	//���ö�ʱ��
	virtual bool SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//���ö�ʱ��
	virtual bool SetTimeCell(DWORD dwTimerID);
	//ɾ����ʱ��
	virtual bool KillTimer(DWORD dwTimerID);
	//ɾ����ʱ��
	virtual bool KillAllTimer();

	//�ڲ�����
private:
	//��ʱ��֪ͨ
	VOID OnTimerThreadSink();
};

//////////////////////////////////////////////////////////////////////////

#endif