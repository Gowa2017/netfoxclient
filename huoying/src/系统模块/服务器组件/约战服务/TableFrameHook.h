#ifndef GAME_MATCH_SINK_HEAD_FILE
#define GAME_MATCH_SINK_HEAD_FILE

#pragma once

//�����ļ�
#include "PersonalRoomServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//���ӹ�����
class CTableFrameHook : public ITableFrameHook, ITableUserAction
{
	//��Ԫ����
	friend class CGameServiceManager;
	
	//�ӿڱ���
public:
	ITableFrame						* m_pITableFrame;					//��ܽӿ�
	
	//�¼��ӿ�
protected:
	IPersonalRoomEventSink					* m_pPersonalRoomEventSink;				//�����¼�

	//���ñ���
protected:
	const tagGameServiceOption		* m_pGameServiceOption;				//���ò���
	
	//���Ա���
protected:
	static const WORD				m_wPlayerCount;						//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameHook();
	//��������
	virtual ~CTableFrameHook();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//���������¼��ӿ�
	virtual bool SetMatchEventSink(IUnknownEx * pIUnknownEx);
	//��ʼ��
	virtual bool InitTableFrameHook(IUnknownEx * pIUnknownEx);		

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart(WORD wChairCount);
	//��Ϸ����
	virtual bool OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//�û��¼�
public:
	//��ҷ���
	virtual bool OnEventUserReturnMatch(IServerUserItem * pIServerUserItem);

	//�û�����
public:	
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);	
};

//////////////////////////////////////////////////////////////////////////

#endif