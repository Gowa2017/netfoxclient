#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	BYTE							m_cbGameMode;						//��Ϸģʽ

	//�ؼ�����
protected:
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
	virtual VOID  Release() {delete this; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool  RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool  OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool  OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	
	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubTwoStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeStart(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeKaiJian(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubThreeEnd(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
