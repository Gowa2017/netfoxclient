#ifndef IMMEDIATE_GROUP_HEAD_FILE
#define IMMEDIATE_GROUP_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//����״̬
enum emMatchStatus
{
	MatchStatus_Null=0,										//��״̬
	MatchStatus_Signup,										//������
	MatchStatus_Wait,										//�ȴ�����
	MatchStatus_Playing										//����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���
#define IDI_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+1)					//ɾ�������ı�����
#define IDI_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+2)					//���һ������ 
#define IDI_GROUP_TIMER_START		(IDI_MATCH_MODULE_START+10)					//�������ڵĶ�ʱ��

//ʱ�Ӷ���
#define IDI_CHECK_MATCH				(IDI_GROUP_TIMER_START+1)					//Ѳ����� 
#define IDI_WAIT_TABLE				(IDI_GROUP_TIMER_START+2)					//�ȴ����� 
#define IDI_SWITCH_TABLE			(IDI_GROUP_TIMER_START+4)					//�����ȴ� 
#define IDI_MATCH_FINISH			(IDI_GROUP_TIMER_START+6)					//�������

//ʱ�Ӷ���
#define IDI_CONTINUE_GAME			1											//������Ϸ
#define IDI_LEAVE_TABLE				2											//�뿪����
#define IDI_CHECK_TABLE_START		3											//�����������Ƿ�ʼ(����п���׼���󲻿�ʼ���)

///////////////////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//������ʱ��
struct tagMatchTimer
{
	DWORD					dwTimerID;						//��ʱ��ID
	int						iElapse;						//ִ�м����	
	WPARAM					wParam;							//���Ӳ���
	LPARAM					lParam;							//���Ӳ���
};

//��������
struct tagMatchScore
{
	DWORD					dwUserID;						//�û�I D
	LONGLONG				lScore;							//�û��ɼ�	
	IServerUserItem *		pUserItem;						//�û��ӿ�
};

//��������
struct tagMatchRanking
{
	DWORD					dwUserID;						//�û�I D
	LONGLONG				lScore;							//�û�����
	LONG					lExpand;						//��չֵ
	IServerUserItem *		pUserItem;						//�û��ӿ�
};

//����������
struct tagTableFrameInfo 
{
	ITableFrame		*		pTableFrame;					//���ӽӿ�
	WORD					wTableID;						//���ӱ��
	LONGLONG				lBaseScore;						//��������	
	WORD					wPlayCount;						//��Ϸ����
	bool					bMatchTaskFinish;				//��ɱ�ʶ
	bool					bSwtichTaskFinish;				//������ʶ
	bool					bWaitForNextInnings;			//�ȴ��¾�
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���

//���鶨��
typedef CWHArray<tagMatchTimer *>	CUserMatchTimerArray;
typedef CWHArray<tagMatchScore *>	CUserMatchScoreArray;
typedef CWHArray<IServerUserItem *>	CMatchUserItemArray;
typedef CWHArray<tagTableFrameInfo *>	CTableFrameMananerArray;

//�ֵ䶨��
typedef CMap<DWORD,DWORD,IServerUserItem *, IServerUserItem *> CUserItemMap;
typedef CMap<IServerUserItem *, IServerUserItem *, DWORD, DWORD> CUserSeatMap;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���

//��������
class CImmediateGroup;

//����ص�
interface IImmediateGroupSink
{
	//�����¼�
public:
	//��ʼ����
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch)=NULL;
	//��������
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch)=NULL;

	//��ʱ���ӿ�
public:
	//ɾ����ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)=NULL;
	//���ö�ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)=NULL;	

	//���ͺ���
public:
	//��������
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem)=NULL;
	//������Ϣ
	virtual bool SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch)=NULL;
	//���ͷ���
	virtual bool SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)=NULL;
	//����״̬
	virtual bool SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)=NULL;	
	//������Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//��������
	virtual bool SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch)=NULL;
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	

	//���ܺ���
public:
	//����յ�����
	virtual void InsertNullTable()=NULL;
	//��ȡ������
	virtual IAndroidUserItem * GetFreeAndroidUserItem()=NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

//��������
class CImmediateGroup: public IMatchEventSink
{
	friend class CImmediateMatch;

	//״̬��Ϣ
protected:	
	SCORE							m_lCurBase;						//��ǰ����
	LONGLONG						m_lMatchNo;						//�������
	BYTE							m_cbMatchStatus;				//�����׶�
	BYTE							m_LoopTimer;					//ѭ����ʱ��
	WORD							m_wAndroidUserCount;			//�����˸���
	SYSTEMTIME						m_MatchStartTime;				//������ʼ

    //ָ�����
protected:
	IImmediateGroupSink *			m_pMatchSink;					//����ص�
	tagGameMatchOption *			m_pMatchOption;					//��������
	tagImmediateMatch *				m_pImmediateMatch;				//��������	

	//�洢����
protected:
	CUserItemMap					m_OnMatchUserMap;				//�����û�
	CMatchUserItemArray				m_FreeUserArray;				//�����û�
	CTableFrameMananerArray			m_MatchTableArray;				//Ԥ����������
	CUserMatchTimerArray			m_MatchTimerArray;				//����ר�ö�ʱ��	

	//��̬����
protected:
	static WORD						m_wChairCount;					//������Ŀ
	
	//��������
public:
	//���캯��
	CImmediateGroup(LONGLONG lMatchNo,tagGameMatchOption * pGameMatchOption,IImmediateGroupSink *pIImmediateGroupSink);
	//��������
	virtual ~CImmediateGroup();

	 //�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){};
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

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

 	//�û��¼�
public:
	//�û�����
	bool OnUserSignUpMatch(IServerUserItem *pUserItem);
	//�û�����
	bool OnUserQuitMatch(IServerUserItem *pUserItem);

	//��������
protected:
	//�������
	VOID InsertFreeGroup(IServerUserItem * pUserServerItem);
	//ʱ����Ϣ
	bool OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter);	
	//�����������
	VOID AddMatchTable(tagTableFrameInfo* pTableFrameInfo){ m_MatchTableArray.Add(pTableFrameInfo);} 		

	//��������
protected:
	//��������
	VOID MatchOver();
	//Ч�������ʼ
	VOID EfficacyStartMatch();
	//������Ϸ
	VOID ContinueGame(ITableFrame * pITableFrame);
	//�������
	WORD SortMapUser(tagMatchScore dwDrawCount[]);	
	//��ȡ�ӿ�
	tagTableFrameInfo * GetTableInterface(ITableFrame * pITableFrame);
	//�û�����
	WORD GetUserRank(IServerUserItem *pUserServerItem,ITableFrame * pITableFrame=NULL);	

	//��Ϣ����
protected:
	//������Ϣ
	VOID SendGroupMessage(LPCTSTR pStrMessage);		
	//������Ϣ
	VOID SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//��ʾ��Ϣ
	VOID SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID=INVALID_CHAIR);

	//��������
protected:
	//��������
	VOID PerformDistributeTable();
	//�û�����
	VOID PerformAllUserStandUp();
	//�û�����
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);

	//��⺯��
protected:	
	//�������
	bool CheckMatchUser();	
	//������
	bool CheckMatchTaskFinish();
	//�����
	bool CheckSwitchTaskFinish();
	//�������
	VOID CheckTableStart(ITableFrame * pITableFrame);

	//��ʱ������
protected:
	//����ʱ��
	VOID CaptureMatchTimer();
	//ɱ����ʱ��
	VOID AllKillMatchTimer();
	//ɱ����ʱ��
	bool KillMatchTimer(INT_PTR dwIndexID);
	//ɱ����ʱ��
	VOID KillMatchGameTimer(DWORD dwTimerID);	
	//ɱ����ʱ��
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//Ͷ�ݶ�ʱ��
	VOID PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam=0, LPARAM lParam=0);		
	//�趨��ʱ��
	VOID SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);		
};

#endif