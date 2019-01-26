#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	BYTE							m_cbCurrentLandScore;				//�ѽз���
	WORD							m_wOutCardUser;						//�������

	//�˿˱���
protected:
	BYTE							m_cbTurnCardCount;					//������Ŀ
	BYTE							m_cbTurnCardData[MAX_COUNT];		//�����б�

	//�����˿�
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];		//�����˿�
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//��ʷ�˿�
protected:

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//�û��з�
	bool OnSubCallScore(VOID * pData, WORD wDataSize);
	//ׯ����Ϣ
	bool OnSubBankerInfo(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubPassCard(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(VOID * pData, WORD wDataSize);
	//��Ϸ��ʼ����
	bool OnSubStartOutCard(VOID * pData, WORD wDataSize);
	//���в���
	void BankOperate(BYTE cbType);
};

//////////////////////////////////////////////////////////////////////////

#endif