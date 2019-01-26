#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

///////////////////////////////////////////////////////////////////////////////////////////

//�Ƴ�����
#define REMOVE_BY_ENDMATCH			250											//��������


//////////////////////////////////////////////////////////////////////////////////////////

//��ʱ��
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,public IServerUserItemSink
{
	//״̬����
protected:
	BYTE								m_MatchStatus;					//����״̬	
	CTime								m_TimeLastMatch;				//����ʱ��

	//��������
protected:
	CDistributeManager					m_DistributeManage;             //�������

	//��������
protected:
	tagGameMatchOption *				m_pMatchOption;					//��������	
	tagLockTimeMatch *					m_pLockTimeMatch;				//��������
	tagGameServiceOption *				m_pGameServiceOption;			//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//��������

	//�ں˽ӿ�
protected:
	ITableFrame	**						m_ppITableFrame;				//��ܽӿ�
	ITimerEngine *						m_pITimerEngine;				//ʱ������
	IDBCorrespondManager *				m_pIDataBaseEngine;				//��������
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//��������

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�
	IServerUserManager *				m_pIServerUserManager;			//�û�����
	IAndroidUserManager	*				m_pAndroidUserManager;			//��������
	IServerUserItemSink *				m_pIServerUserItemSink;			//�û��ص�

	//��������
public:
	//���캯��
	CLockTimeMatch();
	//��������
	virtual ~CLockTimeMatch(void);

	//�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){ delete this; }
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//����֪ͨ
	virtual void OnStartService();

	//����ӿ�
public:
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//��ʼ���ӿ�
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�����
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind);

	//�¼��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //���ܺ���
public:
	 //��Ϸ��ʼ
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	 //�û��¼�
public:
	 //��ҷ���
	 virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem);

	 //�û��¼�
public:
	 //�û�����
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�����
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�ͬ��
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//��������
protected:
	//������
	void ClearSameTableUser(DWORD dwUserID);		
	//�����û�
	void InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID);		
	//�Ƴ�����
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//�����û�
	bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//���ͷ���
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//���ͷ���
	bool SendMatchUserInitScore(IServerUserItem * pIServerUserItem);

	//��ʾ����
protected:
	//������ʾ
	bool SendSafeCardNotify(IServerUserItem * pIServerUserItem);
	//�۷���ʾ
	bool SendDeductFeeNotify(IServerUserItem * pIServerUserItem);	
	//��̭��ʾ
	bool SendEliminateNotify(IServerUserItem * pIServerUserItem);
	//�����ɹ�
	bool SendSignupSuccessNotify(IServerUserItem * pIServerUserItem);	
	//������ʾ
	bool SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard);

	//У�麯��
private:
	//����У��
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//ʱ��У��
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		

	//��������
protected:	
	//���㳡��
	LONGLONG CalcMatchNo();
	//�����û�
	bool PerformDistribute();
	//����״̬
	VOID UpdateMatchStatus();
	//�л�ʱ��
	VOID SwitchMatchTimer();
	//��ֹ����
	bool IsMatchSignupEnd();
	//��ʼ����
	bool IsMatchSignupStart();	
	//��ʼ���
	DWORD GetMatchStartInterval();	
	//��������
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//���ͱ�����Ϣ
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//д�뽱��
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
};

#endif