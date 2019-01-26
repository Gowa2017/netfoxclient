#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//�����ע����
#define MAX_CHIP_TIME               50
//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
  //��Ϸ����
protected:
  LONGLONG            m_lMaxChipBanker;         //�����ע (ׯ��)
  LONGLONG            m_lMaxChipUser;           //�����ע (����)
  LONGLONG            m_lAreaChip[AREA_COUNT];      //������ע
  WORD              m_wCurrentBanker;         //ׯ��λ��
  BYTE              m_cbTimeLeave;            //ʣ��ʱ��

  int               m_nChipLimit[2];          //��ע��Χ (0-AREA_COUNT)
  int               m_nChipTime;            //��ע���� (����)
  int               m_nChipTimeCount;         //���´��� (����)

  //��ׯ����
protected:
  bool              m_bMeApplyBanker;         //�����ʶ
  int               m_nBankerCount;           //�������˵���ׯ����
  int               m_nWaitBanker;            //�ռ���
  static int            m_stlApplyBanker;         //������
  int               m_nMinRobotBankerCount;       //��С����
  INT64                             m_nListUserCount;         //�б�����

  //���ñ���  (ȫ������)
protected:
  tagRobotInfo          m_RobotInfo;            //ȫ������
  TCHAR             m_szRoomName[32];         //���÷���

  //���ñ���  (��Ϸ����)
protected:
  bool              m_bRefreshCfg;            //ÿ��ˢ��
  LONGLONG            m_lAreaLimitScore;          //��������
  LONGLONG            m_lUserLimitScore;          //��ע����
  LONGLONG            m_lBankerCondition;         //��ׯ����

  //���ñ���  (����������)
protected:
  LONGLONG            m_lRobotJettonLimit[2];       //��������
  int               m_nRobotBetTimeLimit[2];      //��������
  bool              m_bRobotBanker;           //�Ƿ���ׯ
  int               m_nRobotBankerCount;        //��ׯ����
  int               m_nRobotWaitBanker;         //��������
  int               m_nRobotApplyBanker;        //��ׯ����
  bool              m_bReduceJettonLimit;       //��������
  tagCustomAndroid        m_CustomAndroidConfig;        //����������
  //�����˴�ȡ��
  LONGLONG            m_lRobotScoreRange[2];        //���Χ
  LONGLONG            m_lRobotBankGetScore;       //�������
  LONGLONG            m_lRobotBankGetScoreBanker;     //������� (ׯ��)
  int               m_nRobotBankStorageMul;       //����

  //�ؼ�����
protected:
  CGameLogic            m_GameLogic;            //��Ϸ�߼�
  IAndroidUserItem *        m_pIAndroidUserItem;        //�û��ӿ�

  //��������
public:
  //���캯��
  CAndroidUserItemSink();
  //��������
  virtual ~CAndroidUserItemSink();


  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID  Release()
  {
    delete this;
  }

  //�ӿڲ�ѯ
  virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);


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
  virtual VOID  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û��뿪
  virtual VOID  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�����
  virtual VOID  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�״̬
  virtual VOID  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

  //��Ϣ����
public:
  //��Ϸ����
  bool OnSubGameFree(const VOID * pBuffer, WORD wDataSize);
  //��Ϸ��ʼ
  bool OnSubGameStart(const VOID * pBuffer, WORD wDataSize);
  //�û���ע
  bool OnSubPlaceJetton(const VOID * pBuffer, WORD wDataSize);
  //��עʧ��
  bool OnSubPlaceJettonFail(const VOID * pBuffer, WORD wDataSize);
  //��Ϸ����
  bool OnSubGameEnd(const VOID * pBuffer, WORD wDataSize);
  //������ׯ
  bool OnSubUserApplyBanker(const VOID * pBuffer, WORD wDataSize);
  //ȡ����ׯ
  bool OnSubUserCancelBanker(const VOID * pBuffer, WORD wDataSize);
  //�л�ׯ��
  bool OnSubChangeBanker(const VOID * pBuffer, WORD wDataSize);

  //���ܺ���
public:
  //��ȡ����
  VOID ReadConfigInformation(tagCustomAndroid& customAndroidConfig);
  //���㷶Χ
  bool CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[]);
};

//////////////////////////////////////////////////////////////////////////

#endif
