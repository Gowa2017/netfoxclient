#pragma once
#include "../��Ϸ������/ServerControl.h"
#include "../��Ϸ������/GameLogic.h"

#define INDEX_BANKER  1
//#define UR_GAME_CONTROL 0x20000000L

#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define mymemcpy  memcpy
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define mymemcpy  memcpy
#endif

class CServerControlItemSink : public IServerControl
{
  //��ҿ���
protected:
  BYTE              m_cbExcuteTimes;        //ִ�д���
  BYTE              m_cbControlStyle;       //���Ʒ�ʽ
  bool              m_bWinArea[3];          //Ӯ������
  BYTE              m_cbWinAreaCount;
  int               m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP]; //�ԱȾ���

  //������Ϣ
protected:
  BYTE              m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];  //������Ϣ
  BYTE              m_cbTableCard[CARD_COUNT];    //�����˿�
  LONGLONG            m_lAllJettonScore[AREA_COUNT+1];//ȫ����ע
  CGameLogic            m_GameLogic;          //��Ϸ�߼�

public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);

public:
  //�����Ƿ��ܳɹ�
  bool IsSettingSuccess(BYTE cbControlArea[MAX_INDEX]);

public:
  //����������
  virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
                                     IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

  //��Ҫ����
  virtual bool __cdecl NeedControl();

  //�������
  virtual bool __cdecl MeetControl(tagControlInfo ControlInfo);

  //��ɿ���
  virtual bool __cdecl CompleteControl();

  //���ؿ�������
  virtual bool __cdecl ReturnControlArea(tagControlInfo& ControlInfo);

  //��ʼ����
  virtual void __cdecl GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[]);

  //��Ӯ����
protected:

  //��������
  void BuildCardGroup();

  //��������
  bool AreaWinCard(BYTE cbStack[]);

  //�������
  bool GetSuitCardCombine(BYTE cbStack[]);

  //ׯ����Ӯ
  LONGLONG GetBankerWinScore(bool bWinArea[]);

  //ׯ��Ӯ��
  void BankerWinCard(bool bIsWin,BYTE cbStack[]);

  //������Ϣ
  VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

  // ��¼����
  VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
};
