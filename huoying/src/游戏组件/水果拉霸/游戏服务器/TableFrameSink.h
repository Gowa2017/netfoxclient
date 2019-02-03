#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "ServerControl.h"
#include "ServerControlItemSink.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	bool							m_bGameStart;						//��ʼ��Ϸ
	BYTE							m_cbGameMode;						//��Ϸģʽ
	BYTE							m_cbTwoMode;						//��עģʽ
	LONGLONG						m_cbLine;							//Ѻ������
	LONGLONG						m_lBet;								//Ѻ����ע
	LONGLONG						m_lBetScore;						//Ѻ����ע
	LONGLONG						m_lOneGameScore;					//Ѻ�ߵ÷�
	LONGLONG						m_lTwoGameScore;					//�ȱ��÷�	
	LONGLONG						m_lThreeGameScore;					//�����÷�
	BYTE							m_cbCurBounsGameCount;				//С��������
	BYTE							m_cbBounsGameCount;					//С��������
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	LONGLONG						m_cbALLSmallSorce;							//С��Ϸ�÷�
	BYTE							m_cbCustoms;								//С��Ϸ�ؿ�
	DWORD							SamllGameFruit[4][5];						//С��Ϸ����
	LONGLONG						m_Bonus;									//�ʽ��
	CTime							m_tCreateTime;								//С��Ϸ��ʼʱ��
	BYTE							m_BonusCount;								//�н��ش���
	LONGLONG						m_BonusGetSorce;							//�н��÷�
	LONGLONG						m_BonusSorce[10];							//�н��ؼ�¼
	SYSTEMTIME						m_BonusSorceDate[10];						//�н���ʱ��
	TCHAR							m_NickName[10][LEN_NICKNAME];				//����ǳ�

	//С��Ϸÿ�ط���
	DWORD  FirstSorce[5];
	DWORD  SecondSorce[5];
	DWORD  ThirdSorce[5];
	DWORD  FourthSorce[5];

	//������
protected:
	static CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> m_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
	static CList<CString, CString&> m_ListOperationRecord;						//�������Ƽ�¼

	//ȫ�ֿ����Ʊ���
	static int						m_IsOpen;								//��ʼ���ӿ��
	static LONGLONG					m_lStorageStartTable;					//��ʼ���ӿ��
	static LONGLONG					m_lStorageInputTable;					//��Ͷ����
	static LONGLONG					m_lCurStorage;							//��ǰ���
	static LONGLONG					m_lRoomStockRecoverScore;				//����Ӯ��
	static LONGLONG					m_lStorageDeductRoom;					//ȫ�ֿ��˥��
	static LONGLONG					m_lStorageMax1Room;						//ȫ�ֿ��ⶥ
	static LONGLONG					m_lStorageMul1Room;						//ȫ��ϵͳ��Ǯ����
	static LONGLONG					m_lStorageMax2Room;						//ȫ�ֿ��ⶥ
	static LONGLONG					m_lStorageMul2Room;						//ȫ��ϵͳ��Ǯ����
	static WORD						m_wGameTwo;								//�ȱ�����
	static WORD						m_wGameTwoDeduct;						//�ȱ�����
	static WORD						m_wGameThree;							//С��������
	static WORD						m_wGameThreeDeduct;						//С��������
	LONGLONG						m_lStorageMulRoom;						//ϵͳ��Ǯ����
	WORD							m_wSingleGameTwo;						//�ȱ�����
	WORD							m_wSingleGameThree;						//С��������
//���Ʊ���
	
	TCHAR							m_szControlRoomName[32];				//���÷���
	TCHAR							m_szControlConfigFileName[MAX_PATH];	//�����ļ�

	//�������
protected:
	HINSTANCE						m_hInst;								//���ƾ��
	CServerControlItemSink*			m_pServerControl;						//�������

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;						//��ʷ�ɼ�


	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;						//��ܽӿ�
	tagCustomRule *					m_pGameCustomRule;					//�Զ�����
	tagGameServiceOption *			m_pGameServiceOption;				//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ����
	virtual VOID RepositionSink();
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual DOUBLE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual DOUBLE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

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
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(DOUBLE lBaseScore);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//��Ϸ�¼�
protected:
	//Ѻ�߿�ʼ
	bool OnUserOneStart(IServerUserItem * pIServerUserItem, LONGLONG lBet, LONGLONG lBetCount);
	//�Ƿ����С��Ϸ
	bool EntranceSmallGame(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT]);
	///����bonus����С��Ϸ
	bool  CTableFrameSink::OnUserSmallGameStartBefore();
	//С��Ϸ
	bool OnUserSmallGameStart(IServerUserItem * pIServerUserItem, BYTE cbCustoms,LONGLONG	SmallSorce);
	//��Ϸ����
	bool OnUserGameEnd(IServerUserItem * pIServerUserItem);
	//�ж��Ƿ��н�
	bool DetermineTheWinning(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT]);
	//�н��÷�
	LONGLONG WinningSocre(IServerUserItem * pIServerUserItem);
	//���ؽ��ؼ�¼
	bool OnUserWinningRecord(IServerUserItem * pIServerUserItem);
	
	//���ܺ���
public:
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore = 0LL);	
	//��ȡ����
	void ReadConfigInformation();
	//���¿���
	bool UpdateControl(ROOMUSERINFO &roomUserInfo);
	//��ȡ��������
	void GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr);
	//��ȡ��������
	CString GetControlDataString(USERCONTROL &UserControl);
	//��ȡ��������
	CString GetControlDataString(BYTE cbGameMode);
};

//////////////////////////////////////////////////////////////////////////////////

#endif