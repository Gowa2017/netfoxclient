#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\��Ϸ������\GameLogic.h"

//////////////////////////////////////////////////////////////////////////



//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
  //��ע��Ϣ
protected:
  SCORE            m_lTurnMaxScore;            //�����ע

  //�û��˿�
protected:
  BYTE              m_HandCardData[MAX_COUNT];        //�û�����

  //�ؼ�����
public:
  CGameLogic            m_GameLogic;              //��Ϸ�߼�
  IAndroidUserItem *        m_pIAndroidUserItem;          //�û��ӿ�
  TCHAR             m_szRoomName[32];           //���÷���
  //�����˴�ȡ��
  LONGLONG            m_lRobotScoreRange[2];          //���Χ
  LONGLONG            m_lRobotBankGetScore;         //�������
  LONGLONG            m_lRobotBankGetScoreBanker;       //������� (ׯ��)
  int               m_nRobotBankStorageMul;         //����

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

  //��Ϣ����
protected:
  //�û���ׯ
  bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
  //��Ϸ��ʼ
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //�û���ע
  bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
  //������Ϣ
  bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
  //�û�̯��
  bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
  //�û�ǿ��
  bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
  //��Ϸ����
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

private:
  //��ȡ����
  void ReadConfigInformation(tagRobotConfig *pConfig);
  //���в���
  void BankOperate(BYTE cbType);
};

//////////////////////////////////////////////////////////////////////////

#endif
