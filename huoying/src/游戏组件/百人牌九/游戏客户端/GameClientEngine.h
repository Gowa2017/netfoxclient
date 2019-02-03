#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "DirectSound.h"
#include "afxtempl.h"
#include "GameOption.h"
//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CGameClientEngine : public CGameFrameEngine
{
  //��Ԫ����
  friend class CGameClientView;

  //������Ϣ
protected:
  LONGLONG            m_lMeMaxScore;            //�����ע
  LONGLONG            m_lAreaLimitScore;          //��������
  LONGLONG            m_lApplyBankerCondition;      //��������
  LONGLONG            m_lInitUserScore[GAME_PLAYER];    //ԭʼ����
  INT               m_nEndGameMul;            //��ǰ���ưٷֱ�

  //������ע
protected:
  LONGLONG            m_lUserJettonScore[AREA_COUNT+1]; //������ע

  //ׯ����Ϣ
protected:
  LONGLONG            m_lBankerScore;           //ׯ�һ���
  WORD              m_wCurrentBanker;         //��ǰׯ��
  BYTE              m_cbLeftCardCount;          //�˿���Ŀ
  bool              m_bEnableSysBanker;         //ϵͳ��ׯ

  //״̬����
protected:
  bool              m_bMeApplyBanker;         //�����ʶ
  bool              m_bPlaySound;           //

  //��������
protected:
  CList<tagAndroidBet,tagAndroidBet>    m_ListAndroid;        //��������ע����

  //�ؼ�����
protected:
  CGameLogic            m_GameLogic;            //��Ϸ�߼�
  CGameClientView         m_GameClientView;         //��Ϸ��ͼ

  //��������
public:
  //���캯��
  CGameClientEngine();
  //��������
  virtual ~CGameClientEngine();

  //����̳�
private:
  //��ʼ����
  virtual bool OnInitGameEngine();
  //���ÿ��
  virtual bool OnResetGameEngine();
  //��Ϸ����
  virtual void OnGameOptionSet();
  //��������
  virtual bool AllowBackGroundSound(bool bAllowSound);

  //�û��¼�
public:
  //�û�����
  virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û��뿪
  virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�����
  virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser) {};
  //�û�״̬
  virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û���λ
  virtual VOID OnEventUserSegment(IClientUserItem * pIClientUserItem, bool bLookonUser) {};
  //�û�ͷ��
  virtual VOID OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser) {};

  //ʱ���¼�
public:
  //ʱ��ɾ��
  virtual bool OnEventGameClockKill(WORD wChairID);
  //ʱ����Ϣ
  virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

  //��Ϸ�¼�
public:
  //�Թ���Ϣ
  virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
  //��Ϸ��Ϣ
  virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //������Ϣ
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

  //��Ϣ����
protected:
  //��Ϸ��ʼ
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //��Ϸ����
  bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
  //�û���ע
  bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
  //��Ϸ����
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
  //������ׯ
  bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
  //ȡ����ׯ
  bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
  //�л�ׯ��
  bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
  //��Ϸ��¼
  bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);
  //��עʧ��
  bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
  //�������
  bool OnSubCheat(const void * pBuffer, WORD wDataSize);
  //������
  bool OnSubReqResult(const void * pBuffer, WORD wDataSize);
  //���¿��
  bool OnSubUpdateStorage(const void * pBuffer, WORD wDataSize);
  //���¿��
  bool OnSubSendUserBetInfo(const void * pBuffer, WORD wDataSize);

  //��ֵ����
protected:
  //����ׯ��
  void SetBankerInfo(WORD wBanker,LONGLONG lScore);
  //������ע
  void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);

  //���ܺ���
protected:
  //���¿���
  void UpdateButtonContron();
  //�������пؼ�
  void ReSetGameCtr();
  //�����û��б�
  void UpdateUserList();

  //��Ϣӳ��
protected:
  //��ע��Ϣ
  LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
  //������Ϣ
  LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
  //��������
  LRESULT OnContinueCard(WPARAM wParam, LPARAM lParam);
  //�ֹ�����
  LRESULT  OnOpenCard(WPARAM wParam, LPARAM lParam);
  //�Զ�����
  LRESULT  OnAutoOpenCard(WPARAM wParam, LPARAM lParam);
  //����Ա����
  LRESULT OnAdminCommand(WPARAM wParam, LPARAM lParam);
  //���¿��
  LRESULT OnUpdateStorage(WPARAM wParam, LPARAM lParam);
  //����
  LRESULT OnGameSetting(WPARAM wParam, LPARAM lParam);
  //����
  LRESULT OnChatMessage(WPARAM wParam, LPARAM lParam);
  //��Ϸ����
  LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
