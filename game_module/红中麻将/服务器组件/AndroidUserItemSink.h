#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////



//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
  //��Ϸ����
protected:
	BYTE						m_cbHandCardData[MAX_COUNT];		//�û��˿�
	BYTE						m_cbHandCardCount;					//�û��˿�����
  //�ؼ�����
public:
	CGameLogic					m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *			m_pIAndroidUserItem;				//�û��ӿ�

  //��������
public:
  //���캯��
  CAndroidUserItemSink();
  //��������
  virtual ~CAndroidUserItemSink();

  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID Release()
  {
    delete this;
  }
  //�ӿڲ�ѯ
  virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

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
  virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
  //��Ϸ��Ϣ
  virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
  //������Ϣ
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

  //�û��¼�
public:
  //�û�����
  virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û��뿪
  virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�����
  virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�״̬
  virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û���λ
  virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);


  //��Ϸ��ʼ
  bool OnSubGameStart(const VOID * pBuffer, WORD wDataSize);
  //��ҳ���
  bool OnSubOutCard(const VOID * pBuffer, WORD wDataSize);
  //������
  bool OnSubSendCard(const VOID * pBuffer, WORD wDataSize);
  //������ʾ
  bool OnSubOperateNotify(const VOID * pBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
