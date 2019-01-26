#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#endif

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
  virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û��뿪
  virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�����
  virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //�û�״̬
  virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

  //��Ϣ����
public:
  //��Ϸ����
  bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
  //��Ϸ��ʼ
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //�û���ע
  bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
  //��עʧ��
  bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
  //��Ϸ����
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
  //������ׯ
  bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
  //ȡ����ׯ
  bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
  //�л�ׯ��
  bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);

  //���ܺ���
public:
  //��ȡ����
  void ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh);
  //���㷶Χ
  bool CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[]);
  //��ȡ����
  LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);
};

//////////////////////////////////////////////////////////////////////////

#endif
