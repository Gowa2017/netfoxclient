#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			65									//��ʷ����
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//�û���Ϣ
protected:
	SCORE							m_lUserStartScore[GAME_PLAYER];		//��ʼ����

	//��ע��
protected:
	SCORE							m_lAllBet[AREA_MAX];					//����ע
	SCORE							m_lPlayBet[GAME_PLAYER][AREA_MAX];		//�����ע
	

	//����
protected:
	SCORE							m_lBankerScore;							//ׯ�һ���
	SCORE							m_lPlayScore[GAME_PLAYER][AREA_MAX];	//�����Ӯ
	SCORE							m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	SCORE							m_lUserRevenue[GAME_PLAYER];			//���˰��

	//���Ʊ���
protected:
	SCORE							m_lAreaLimitScore;						//��������
	SCORE							m_lUserLimitScore;						//��������

	SCORE							m_lApplyBankerCondition;				//��������
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
	int								m_nServiceCharge;						//�����

	//�˿���Ϣ
protected:
	BYTE							m_cbCardCount[2];						//�˿���Ŀ
    BYTE							m_cbTableCardArray[2][3];				//�����˿�

	//״̬����
protected:
	DWORD							m_dwBetTime;							//��ʼʱ��
	bool							m_bControl;								//�Ƿ����
	SCORE							m_lControlStorage;						//���ƺ�����д��
	TCHAR							m_szControlName[LEN_NICKNAME];			//��������

	//ׯ����Ϣ
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//�������
	WORD							m_wCurrentBanker;						//��ǰׯ��
	WORD							m_wBankerTime;							//��ׯ����
	SCORE							m_lBankerWinScore;						//�ۼƳɼ�
	SCORE							m_lBankerCurGameScore;					//��ǰ�ɼ�
	bool							m_bEnableSysBanker;						//ϵͳ��ׯ
	SCORE							m_lBottomPourImpose;					//��ע����
	//��¼����
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	DWORD							m_dwRecordCount;						//��¼��Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���

	//�������
protected:
	HINSTANCE						m_hControlInst;
	IServerControl*					m_pServerContro;

	//���Ʊ���
protected:
	bool							m_bRefreshCfg;							//ÿ��ˢ��
	TCHAR							m_szGameRoomName[SERVER_LEN];			//��������

	SCORE							m_lStorageStart;						//�����ֵ
	SCORE							m_lStorageCurrent;						//��������ÿ���ӵĿ����ֵ����ȡʧ�ܰ� 0 ����
	SCORE							m_lStorageDeduct;						//ÿ����Ϸ������۳��Ŀ���������ȡʧ�ܰ� 1.00 ����
	SCORE							m_lStorageMax1;							//���ⶥ1
	LONGLONG						m_lStorageMul1;							//ϵͳ��Ǯ����
	SCORE							m_lStorageMax2;							//���ⶥ2
	LONGLONG						m_lStorageMul2;							//ϵͳ��Ǯ����
	int								m_nStorageCount;						//ѭ������


	int								m_IsOpen;
	SCORE							m_lAttenuationScore;					//˥��ֵ
	TCHAR							m_szControlRoomName[32];           //���÷���
    TCHAR							m_szControlConfigFileName[MAX_PATH];     //�����ļ�
	//ׯ������
protected:
	//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_nBankerTimeLimit)��
	//�������ֵ��������������ׯ���б�����������ҽ��ʱ��
	//�����ټ���ׯm_nBankerTimeAdd�֣���ׯ���������á�
	LONGLONG						m_nBankerTimeLimit;							//���ׯ����
	LONGLONG						m_nBankerTimeAdd;							//ׯ��������

	//��ҳ���m_lExtraBankerScore֮�󣬾�����������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_nExtraBankerTime�֡�
	SCORE							m_lExtraBankerScore;						//ׯ��Ǯ
	LONGLONG						m_nExtraBankerTime;						//ׯ��Ǯ��ʱ,��ׯ������

	//���ׯ����
	LONGLONG						m_lPlayerBankerMAX;						//������ׯ����

	//��ׯ
	bool							m_bExchangeBanker;						//����ׯ��

	//ʱ������
protected:
	BYTE							m_cbFreeTime;							//����ʱ��
	BYTE							m_cbBetTime;							//��עʱ��
	BYTE							m_cbEndTime;							//����ʱ��

	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	SCORE							m_lRobotAreaLimit;						//����ͳ�� (������)
	SCORE							m_lRobotAreaScore[AREA_MAX];		//����ͳ�� (������)
	int								m_nRobotListMaxCount;					//�������
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
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(SCORE lBaseScore){};

public:
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }
	//�û���ע
	virtual bool OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem);
	//����¼�
public:
	//��ѯ���
	virtual bool OnEventQueryChargeable(IServerUserItem *pIServerUserItem, bool bLookonUser);
	//��ѯ�����
	virtual SCORE OnEventQueryCharge(IServerUserItem *pIServerUserItem);


	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlayBet(WORD wChairID, BYTE cbBetArea, SCORE lBetScore);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//�ֻ��ж�
	void TakeTurns();
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//������Ϣ
	void SendPlaceBetFail(WORD wChairID, BYTE cbBetArea, SCORE lBetScore);
	//����Ա����
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);
	//��ȡ����
	void ReadConfigInformation();
	// ��Ӷ���
	CString AddComma( SCORE lScore );
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//������ע��Ϣ
	void SendUserBetInfo( IServerUserItem *pIServerUserItem );

	//��ע����
private:
	//�����ע
	SCORE GetMaxPlayerScore( BYTE cbBetArea, WORD wChairID );

	//��Ϸͳ��
private:
	//��Ϸ��������
	SCORE GameOver();
	//����÷�
    bool CalculateScore(OUT SCORE& lBankerWinScore, OUT tagServerGameRecord& GameRecord, BYTE cbDispatchCount);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE* pWinArea);
	// ��¼����
	void WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
	//�����������
	void  SendPeizhiUser( IServerUserItem *pRcvServerUserItem );
};

//////////////////////////////////////////////////////////////////////////

#endif
