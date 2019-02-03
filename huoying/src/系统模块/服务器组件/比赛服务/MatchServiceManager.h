#ifndef GAME_MATCH_SERVICE_MANAGER_HEAD_FILE
#define GAME_MATCH_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "MatchServiceHead.h"

////////////////////////////////////////////////////////////////////////

//�������������
class MATCH_SERVICE_CLASS CMatchServiceManager : public IMatchServiceManager
{
	//״̬����
protected:
	bool								m_bIsService;					//�����ʶ	
	
	//�ӿڱ���
protected:	
	IGameMatchItem *					m_pIGameMatchItem;				//��������

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�

	//��������
public:
	//���캯��
	CMatchServiceManager(void);
	//��������
	virtual ~CMatchServiceManager(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService();
	//��������
	virtual bool StartService();
	
	//����ӿ�
public:
	//��������
	virtual bool CreateGameMatch(BYTE cbMatchType);
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//��ʼ���ӿ�
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//�û��¼�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);
	//�������
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);		

	//�ӿ���Ϣ
public:
	//�û��ӿ�
	virtual IUnknownEx * GetServerUserItemSink();
};

//////////////////////////////////////////////////////////////////////////
#endif