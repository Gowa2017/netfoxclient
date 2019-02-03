#pragma once
#include "../���������/ServerControl.h"
#include "../���������/GameLogic.h"

#define INDEX_BANKER        1
#define CONTROL_AREA        4
//#define MAX_CARD          5
#define MAX_CARDGROUP       5

class CServerControlItemSink : public IServerControl
{
  //��ҿ���
protected:
  BYTE              m_cbExcuteTimes;        //ִ�д���
  BYTE              m_cbControlStyle;       //���Ʒ�ʽ
  bool              m_bWinArea[CONTROL_AREA];   //Ӯ������
  BYTE              m_cbWinAreaCount;
  int               m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP]; //�ԱȾ���
  bool              m_bBakWinArea[CONTROL_AREA];   //Ӯ������
  //������Ϣ
protected:
  BYTE              m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];  //������Ϣ
  BYTE              m_cbTableCard[CARD_COUNT];    //�����˿�
  SCORE            m_lAllJettonScore[AREA_COUNT+1];//ȫ����ע
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
  virtual bool  NeedControl();

  //�������
  virtual bool  MeetControl(tagControlInfo ControlInfo);

  //��ɿ���
  virtual bool  CompleteControl();

  //���ؿ�������
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo);

  //��ʼ����
  virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[]);

  //��ʼ����
  virtual void  GetControlArea(BYTE Area[], BYTE &cbControlStyle);
  //��ʼ����
  virtual BYTE  GetControlTybe();
  //��ʼ����
  virtual void  SetControlArea(BYTE Area[]);
  //��Ӯ����
protected:

  //��������
  bool AreaWinCard(BYTE cbStack[]);

  //�������
  bool GetSuitCardCombine(BYTE cbStack[]);

  //ׯ����Ӯ
  SCORE GetBankerWinScore(int nWinMultiple[]);

  //ׯ��Ӯ��
  void BankerWinCard(bool bIsWin,BYTE cbStack[]);

  //������Ϣ
  VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

  // ��¼����
  VOID WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
};
