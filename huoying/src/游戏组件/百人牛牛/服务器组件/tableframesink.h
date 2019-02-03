#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"

#include "ServerControl.h"
#define MAX_CARD          5
#define MAX_CARDGROUP       5
#define CONTROL_AREA        4
//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY     16                  //��ʷ����
//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
  //�û���Ϣ
protected:
  SCORE		            m_lUserStartScore[GAME_PLAYER];   //��ʼ����

  //����ע��
protected:
  SCORE		            m_lAllJettonScore[AREA_COUNT+1];    //ȫ����ע
  SCORE		            m_lbakAllJettonScore[AREA_COUNT+1];    //ȫ����ע
  //������ע
protected:
  SCORE		            m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];  //������ע

  //���Ʊ���
protected:
  bool              m_bRefreshCfg;              //ÿ��ˢ��
  TCHAR             m_szRoomName[32];           //���÷���
  TCHAR             m_szConfigFileName[MAX_PATH];     //�����ļ�
  SCORE	            m_lAreaLimitScore;            //��������
  SCORE	            m_lUserLimitScore;            //��������
  INT               m_nEndGameMul;              //��ǰ���ưٷֱ�

  TCHAR             m_szControlRoomName[32];           //���÷���
  TCHAR             m_szControlConfigFileName[MAX_PATH];     //�����ļ�

  //��ׯ����
  SCORE	            m_lApplyBankerCondition;        //��������
  int               m_nBankerTimeLimit;           //��������
  int               m_nBankerTimeAdd;           //���Ӵ��� (��Ҵ����������ʱ)
  SCORE	            m_lExtraBankerScore;          //�������� (���ڴ�ֵʱ������������������ׯ)
  int               m_nExtraBankerTime;           //�������

  //������
  SCORE            m_lStorageStart;            //�����ֵ
  SCORE            m_lStorageCurrent;            //�����ֵ
  int               m_nStorageDeduct;           //��ȡ����
  SCORE            m_lStorageMax1;             //���ⶥ1
  LONGLONG            m_lStorageMul1;             //ϵͳ��Ǯ����
  SCORE            m_lStorageMax2;             //���ⶥ2
  LONGLONG            m_lStorageMul2;             //ϵͳ��Ǯ����
  bool              m_bControl;               //�Ƿ����
  TCHAR             m_szControlName[LEN_NICKNAME];      //��������
	int					m_IsOpen;
	SCORE							m_lAttenuationScore;					//˥��ֵ
	SCORE		   m_lBottomPourImpose;		//��ע����
  //ʱ�����
  int               m_nFreeTime;              //����ʱ��
  int               m_nPlaceJettonTime;           //��עʱ��
  int               m_nGameEndTime;             //����ʱ��

  //�����˿���
  int               m_nMaxChipRobot;            //�����Ŀ (��ע������)
  int               m_nChipRobotCount;            //����ͳ�� (��ע������)
  SCORE            m_lRobotAreaLimit;            //����ͳ�� (������)
  SCORE            m_lRobotAreaScore[AREA_COUNT+1];    //����ͳ�� (������)
  int               m_nRobotListMaxCount;         //�������

  //��ҳɼ�
protected:
  SCORE            m_lUserWinScore[GAME_PLAYER];     //��ҳɼ�
  SCORE            m_lUserReturnScore[GAME_PLAYER];    //������ע
  SCORE            m_lUserRevenue[GAME_PLAYER];      //���˰��
  BYTE              m_cbLeftCardCount;            //�˿���Ŀ
  bool              m_bContiueCard;             //��������
  BYTE              m_bcFirstPostCard;            //���°l���c��

  //�˿���Ϣ
protected:
  BYTE              m_cbCardCount[4];           //�˿���Ŀ
  BYTE              m_cbTableCardArray[5][5];       //�����˿�
  BYTE              m_cbTableCard[CARD_COUNT];        //�����˿�

  //״̬����
protected:
  DWORD             m_dwJettonTime;             //��ʼʱ��
  bool              m_bExchangeBanker;            //�任ׯ��
  WORD              m_wAddTime;               //������ׯ

  //ׯ����Ϣ
protected:
  CWHArray<WORD>          m_ApplyUserArray;           //�������
  WORD              m_wCurrentBanker;           //��ǰׯ��
  WORD              m_wBankerTime;              //��ׯ����
  SCORE            m_lBankerScore;             //
  SCORE            m_lBankerWinScore;            //�ۼƳɼ�
  SCORE            m_lBankerCurGameScore;          //��ǰ�ɼ�
  bool              m_bEnableSysBanker;           //ϵͳ��ׯ

  //���Ʊ���
protected:
  BYTE              m_cbWinSideControl;           //������Ӯ
  int               m_nSendCardCount;           //���ʹ���

  //��¼����
protected:
  tagServerGameRecord       m_GameRecordArrary[MAX_SCORE_HISTORY];  //��Ϸ��¼
  int               m_nRecordFirst;             //��ʼ��¼
  int               m_nRecordLast;              //����¼
  CFile             m_FileRecord;             //��¼���
  DWORD             m_dwRecordCount;            //��¼��Ŀ

  //�������
protected:
  CGameLogic            m_GameLogic;              //��Ϸ�߼�
  ITableFrame           * m_pITableFrame;           //��ܽӿ�
  tagGameServiceOption      * m_pGameServiceOption;         //���ò���
  tagGameServiceAttrib *      m_pGameServiceAttrib;         //��Ϸ����

  //�������
protected:
  HINSTANCE           m_hInst;
  IServerControl*         m_pServerContro;

  //���Ա���
protected:
  static const WORD       m_wPlayerCount;             //��Ϸ����
protected:											//ռλ����
	WORD				  m_wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	tagOccUpYesAtconfig   m_OccUpYesAtconfig;
	CWHArray<DWORD>					m_PeizhiUserArray;						//�������
	bool							m_bAndroidXiahzuang;					//��������ׯ
  //��������
public:
  //���캯��
  CTableFrameSink();
  //��������
  virtual ~CTableFrameSink();

  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID Release();
  //�ӿڲ�ѯ
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //����ӿ�
public:
  //��λ�ӿ�
  virtual VOID RepositionSink();
  //���ýӿ�
  virtual bool Initialization(IUnknownEx * pIUnknownEx);
  //�����ӿ�
public:
  //���û���
  virtual void SetGameBaseScore(SCORE lBaseScore) {};
  //��Ϸ�¼�
public:
  //��Ϸ��ʼ
  virtual bool OnEventGameStart();
  //��Ϸ����
  virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
  //���ͳ���
  virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

  //�¼��ӿ�
public:
  //ʱ���¼�
  virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
  //��Ϸ��Ϣ
  virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
  //�����Ϣ
  virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

  //�û��¼�
public:
  //�û�����
  virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
  //�û�����
  virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem)
  {
    return true;
  }
  //�û�����
  virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //�û�����
  virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //�û�ͬ��
  virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
  {
    return true;
  }
  //�û���ע
  virtual bool OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem);
  //��ѯ�ӿ�
public:
  //��ѯ�޶�
  virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
  //���ٻ���
  virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
  {
    return 0;
  }
  //�����¼�
  virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
  {
    return false;
  }
  //�����¼�
  virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
  //��ѯ�Ƿ�۷����
  virtual bool QueryBuckleServiceCharge(WORD wChairID);

  //��Ϸ�¼�
protected:
  //��ע�¼�
  bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore);
  //����ׯ��
  bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
  //ȡ������
  bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

  //��������
private:
  //�����˿�
  bool DispatchTableCard();
  //����ׯ��
  void SendApplyUser(IServerUserItem *pServerUserItem);
  //����ׯ��
  bool ChangeBanker(bool bCancelCurrentBanker);
  //�ֻ��ж�
  void TakeTurns();
  //���ͼ�¼
  void SendGameRecord(IServerUserItem *pIServerUserItem);
  //������Ϣ
  void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
  //��ȡ����
  void ReadConfigInformation();
  //�Ƿ�˥��
  bool NeedDeductStorage();
  //������ע��Ϣ
  void SendUserBetInfo(IServerUserItem *pIServerUserItem);
  //�����������
  void  SendPeizhiUser( IServerUserItem *pRcvServerUserItem );
  //��Ӯ����
protected:
  // ����Ż�
  VOID StorageOptimize();
  // �ж���Ӯ
  void JudgeSystemScore(BYTE bCardData[5][5], SCORE& lSystemScore, bool& bAllEat);
  // ��¼����
  void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

  //��ע����
private:
  //�����ע
  SCORE GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea);

  //��Ϸͳ��
private:
  //����÷�
  SCORE CalculateScore();
  //�ƶ�Ӯ��
  void DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan,bool &bWinHuan,BYTE &TianMultiple,BYTE &diMultiple,BYTE &TianXuanltiple,BYTE &HuangMultiple);
};


//////////////////////////////////////////////////////////////////////////////////

#endif