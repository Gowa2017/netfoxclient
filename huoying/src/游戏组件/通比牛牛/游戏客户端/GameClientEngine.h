#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "HistoryRecord.h"

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<UDP_Buffer,UDP_Buffer &> CSocketPacketArray;

//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CGameClientEngine : public CGameFrameEngine
{
  //���ñ���
protected:
  DWORD             m_dwCardHSpace;             //�˿�����
  WORD              m_wViewChairID[GAME_PLAYER];      //��ͼλ��

  //��ע��Ϣ
protected:
  SCORE            m_lTurnMaxScore;            //�����ע
  SCORE            m_lTableScore[GAME_PLAYER];       //��ע��Ŀ

  //״̬����
protected:
  BYTE                            m_bDynamicJoin;                         //��̬����
  WORD              m_wBankerUser;              //ׯ���û�
  BYTE              m_cbPlayStatus[GAME_PLAYER];      //��Ϸ״̬
  TCHAR             m_szAccounts[GAME_PLAYER][LEN_ACCOUNTS];  //�������

  //�û��˿�
protected:
  BYTE              m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�û�����
  BYTE              m_bUserOxCard[GAME_PLAYER];       //ţţ����

  //�������
protected:
  CSocketPacketArray        m_SocketPacketArray;          //���ݻ���

  //�������
protected:
  CHistoryScore         m_HistoryScore;           //��ʷ����

  //�ؼ�����
public:
  CGameLogic            m_GameLogic;              //��Ϸ�߼�
  CGameClientView         m_GameClientView;           //��Ϸ��ͼ

  //��������
public:
  //���캯��
  CGameClientEngine();
  //��������
  virtual ~CGameClientEngine();

  //���ƽӿ�
public:
  //��ʼ����
  virtual bool OnInitGameEngine();
  //���ÿ��
  virtual bool OnResetGameEngine();

  //ʱ���¼�
public:
  //ʱ��ɾ��
  virtual bool OnEventGameClockKill(WORD wChairID);
  //ʱ����Ϣ
  virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

  //�¼��ӿ�
public:
  //�Թ�״̬
  virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
  //������Ϣ
  virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //��Ϸ����
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

  //�û��¼�
public:
  //�û�����
  virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û��뿪
  virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�״̬
  virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);

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

  //��������
protected:
  //�Ƿ�ǰ�����û�
  bool IsCurrentUser(WORD wCurrentUser)
  {
    return (!IsLookonMode() && wCurrentUser==GetMeChairID());
  }
  //���¿���
  void UpdateScoreControl(SCORE lScore[],BOOL bShow);
  //ת����Ϣ
  void ChangeUserInfo(BYTE bCardData[],BYTE bCardCount,CString &CardInfo);

  //��Ϣӳ��
protected:
  //��ʾ��ť
  LRESULT OnHintOx(WPARAM wParam, LPARAM lParam);
  //��ʼ��ť
  LRESULT OnStart(WPARAM wParam, LPARAM lParam);
  //��ע��ť
  LRESULT OnAddScore(WPARAM wParam, LPARAM lParam);
  //ţţ��ť
  LRESULT OnOx(WPARAM wParam, LPARAM lParam);
  //��ݼ���ť
  LRESULT OnShortcut(WPARAM wParam, LPARAM lParam);
  //̯�ư�ť
  LRESULT OnOpenCard(WPARAM wParam, LPARAM lParam);
  //�������
  LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
  //��ʾ����
  LRESULT OnSetUserOxValue(WPARAM wParam, LPARAM lParam);
  //��ʾ��Ϣ
  LRESULT OnShowInfo(WPARAM wParam, LPARAM lParam);
  //ţBTΪ��
  LRESULT OnOxEnable(WPARAM wParam, LPARAM lParam);
  //�˿˷���
  LRESULT OnSortCard(WPARAM wParam, LPARAM lParam);
  //�˿˷���
  LRESULT OnReSortCard(WPARAM wParam, LPARAM lParam);
  //��ׯ��Ϣ
  LRESULT OnBanker(WPARAM wParam, LPARAM lParam);
  //�����Ϣ
  LRESULT OnClickCard(WPARAM wParam, LPARAM lParam);
  //������Ϣ
  LRESULT OnMessageYuYin(WPARAM wParam, LPARAM lParam);
  //�����Ϣ
  LRESULT OnStorageInfo(WPARAM wParam,LPARAM lParam);
  //��������
  LRESULT OnAddUserRoster(WPARAM wParam,LPARAM lParam);
  //ɾ������
  LRESULT OnDeleteUserRoster(WPARAM wParam,LPARAM lParam);
  //�����������
  LRESULT OnRequestUpdateUserRoster(WPARAM wParam,LPARAM lParam);
  //�������
  LRESULT OnSpeCommdControl(WPARAM wParam, LPARAM lParam);

public:
  afx_msg void OnTimer(UINT nIDEvent);
  //������Ϣ
  void InsertMessageToDlg(CString strInfo);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
