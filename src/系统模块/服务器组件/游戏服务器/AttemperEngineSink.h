#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrame.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "DistributeManager.h"
#include "SensitiveWordsFilter.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define CLIENT_KIND_FALSH			1									//��ҳ����
#define CLIENT_KIND_MOBILE			2									//�ֻ�����
#define CLIENT_KIND_COMPUTER		3									//��������

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//�û�����
	BYTE							cbClientKind;						//��������
	IServerUserItem *				pIServerUserItem;					//�û��ӿ�
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	DBR_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};

//����˵��
typedef CWHArray<CTableFrame *>		CTableFrameArray;					//��������
typedef CWHArray<tagSystemMessage *>   CSystemMessageArray;             //ϵͳ��Ϣ
typedef CWHArray<tagTaskParameter *>   CTaskParameterArray;             //��������
typedef CMap<DWORD,DWORD,DWORD,DWORD &>  CKickUserItemMap;              //����ӳ�� 
typedef CMap<WORD,WORD,tagTaskParameter *,tagTaskParameter *>  CTaskParameterMap;

typedef CWHArray<tagPersonalTableParameter *> CPersonalTableParameterArray;	//˽������


//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink, public IMainServiceFrame,public IServerUserItemSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeekCorrespond;					//Э����־

	//���Ʊ���
protected:
	DWORD							m_dwIntervalTime;					//���ʱ��
	DWORD							m_dwLastDisposeTime;				//����ʱ��

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pAndroidParameter;				//����Ϣ

	//������Ϣ
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameMatchOption *			m_pGameMatchOption;					//��������	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	CPersonalTableParameterArray	m_PersonalTableParameterArray;		//˽������
	tagPersonalRoomOption			m_PersonalRoomOption;				//˽�˷�������
	bool		m_bHasPersonalRoomService;						//�Ƿ���˽�˷�����

	//��������
protected:
	CMD_GR_ConfigColumn				m_DataConfigColumn;					//�б�����

	//�ͱ�����
protected:
	tagBaseEnsureParameter			m_BaseEnsureParameter;				//�ͱ�����	

	//ǩ������
protected:
	SCORE							m_lCheckInReward[LEN_WEEK];			//ǩ������

	//��Ա����
protected:
	WORD							m_wMemberCount;						//��Ա��Ŀ
	tagMemberParameterNew			m_MemberParameter[10];				//��Ա����

	//�������
protected:
	WORD							m_wTaskCount;						//������Ŀ
	tagTaskParameter				m_TaskParameter[TASK_MAX_COUNT];	//�������

	//�ȼ�����
protected:
	WORD							m_wLevelCount;						//�ȼ���Ŀ
	tagGrowLevelConfig				m_GrowLevelConfig[60];				//�ȼ�����

	//�������
protected:
	CTableFrameArray				m_TableFrameArray;					//��������
	CServerListManager				m_ServerListManager;				//�б����
	CServerUserManager				m_ServerUserManager;				//�û�����
	CAndroidUserManager				m_AndroidUserManager;				//��������
	CUserTaskManager				m_UserTaskManager;					//�������
	CGamePropertyManager			m_GamePropertyManager;				//���߹���
	CDistributeManager              m_DistributeManage;                 //�������
	CKickUserItemMap                m_KickUserItemMap;                  //���˹���
	CSystemMessageArray             m_SystemMessageActive;              //ϵͳ��Ϣ
	CSystemMessageArray				m_SystemMessageBuffer;				//��Ϣ����	
	CSensitiveWordsFilter			m_WordsFilter;						//���ֹ���

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IAttemperEngine *				m_pIAttemperEngine;					//��������
	ITCPSocketService *				m_pITCPSocketService;				//�������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//ʱ������

	//��������
public:	
	IMatchServiceManager			* m_pIMatchServiceManager;			//��������

	//��������
public:
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��������
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//��������
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��
protected:
	CRITICAL_SECTION				m_cs_test;							///< ����һ���ٽ�������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendHallMessage(LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);

	//����ӿ�
public:
	//��������
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Ⱥ������
	virtual bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ܽӿ�
public:
	//�Ͽ�Э��
	virtual bool DisconnectCorrespond();	
	//�����û�
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//ɾ���û�
	virtual bool DeleteDistribute(IServerUserItem * pIServerUserItem);
	//���дʹ���
	virtual void SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen);
	//����������
	virtual VOID UnLockAndroidUser(WORD wServerID, WORD wBatchID);
	//��ɢ˽������
	virtual VOID DismissPersonalTable(WORD wServerID, WORD wTableID);
	//ȡ������
	virtual VOID CancelCreateTable(DWORD dwUserID, DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, DWORD dwReason, WORD wTableID, TCHAR * szRoomID);
	//��ʼ��Ϸд�������Ϣ
	virtual VOID PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, TCHAR * szRoomID);
	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind);
	//�û�����
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);

	//�����¼�
protected:
	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�б���Ϣ
	bool OnDBGameColumnInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����BUFF
	bool OnDBGamePropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGameLoadTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBGameSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա����
	bool OnDBGameMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա��ѯ
	bool OnDBPCMemberDayQueryInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա�ͽ�
	bool OnDBPCMemberDayPresentResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա���
	bool OnDBPCMemberDayGiftResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���гɹ�
	bool OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�û���Ϣ
	bool OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ͨ���
	bool OnDBUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ǩ����Ϣ
	bool OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ǩ�����
	bool OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ǩ������
	bool OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ͱ�����
	bool OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ͱ����
	bool OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����б�
	bool OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//���߳ɹ�
	bool OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���߹�����
	bool OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBQueryPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ʹ�ý��
	bool OnDBQueryPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBQueryPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ����
	bool OnDBQueryGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ����
	bool OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������д�
	bool OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ȼ�����
	bool OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ȼ�����
	bool OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ȼ�����
	bool OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ȡ������
	bool OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//˽������
	bool OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ɢ����
	bool OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//˽�˷���������󷿼���Ϣ
	bool OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ǰ�û��ķ�������Ϸ��
	bool OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����¼�
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	bool OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//�û�����
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��¼����
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϸ����
	bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ܴ���
	bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���д���
	bool OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������
	bool OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ͱ�����
	bool OnTCPNetworkMainBaseEnsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ�����
	bool OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ա����
	bool OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���ߴ���
	bool OnTCPNetworkMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������
	bool OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ǩ������
	bool OnTCPNetworkMainCheckIn(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//˽�˷���
	bool OnTCPNetworkMainPersonalTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�����¼�
protected:
	//I D ��¼
	bool OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ֻ���¼
	bool OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ʺŵ�¼
	bool OnTCPNetworkSubLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�û�����
protected:
	//�û�����
	bool OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û��Թ�
	bool OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�˽��
	bool OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetworkSubGamePropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubPropertyBackpack(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����ʹ��
	bool OnTCPNetworkSubPropertyUse(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ����
	bool OnTCPNetworkSubQuerySendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���͵���
	bool OnTCPNetworkSubPropertyPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����
	bool OnTCPNetworkSubGetSendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ʹ�õ���
	bool OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	bool OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ܾ�����
	bool OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳�����
	bool OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û���Ϣ
	bool OnTCPNetworkSubUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������λ��
	bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���������û���Ϣ
	bool OnTCPNetworkSubChairUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);	
	//�ȼ���ѯ
	bool OnTCPNetworkSubGrowLevelQuery(VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//��������
protected:	
	//��ѯ����
	bool OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ͨ����
	bool OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ȡ������
	bool OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ת������
	bool OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ�û�����
	bool OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//ǩ������
protected:
	//��ѯǩ��
	bool OnTCPNetworkSubCheckInQueryRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����
	bool OnTCPNetworkSubCheckInDoneRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�ͱ�����
protected:
	//��ѯ�ͱ�
	bool OnTCPNetworkSubBaseEnsureQueryRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�ͱ�
	bool OnTCPNetworkSubBaseEnsureTakeRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:
	//��ȡ����
	bool OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����
	bool OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubGiveUpTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//��Ա����
protected:
	//��Ա��ѯ
	bool OnTCPNetworkSubMemberQueryInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ա�ͽ�
	bool OnTCPNetworkSubMemberDayPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ա���
	bool OnTCPNetworkSubMemberDayGift(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�һ�����
protected:
	//��ѯ����
	bool OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����Ա
	bool OnTCPNetworkSubPurchaseMember(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ���Ϸ��
	bool OnTCPNetworkSubExchangeScoreByIngot(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ���Ϸ��
	bool OnTCPNetworkSubExchangeScoreByBeans(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:
	//��ѯ����
	bool OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳��û�
	bool OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳������û�
	bool OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ϣ
	bool OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ��Ϸ
	bool OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//˽�˷���
protected:
	//��������
	bool OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ȡ������
	bool OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ȡ����
	bool OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����ǿ����������
	bool OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������󷿼�ɼ�
	bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�ڲ��¼�
protected:
	//�û���¼
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine);
	//�û��ǳ�
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason);

	//ִ�й���
protected:
	//�����û�
	bool PerformDistribute();	
	//������Ϸ��
	bool PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason);
	//�汾���
	bool PerformCheckVersion(DWORD dwPlazaVersion, DWORD dwFrameVersion, DWORD dwClientVersion, DWORD dwSocketID);
	//�ƽ�����
	bool PerformTaskProgress(IServerUserItem * pIServerUserItem,tagUserTaskEntry * pUserTaskEntry,DWORD dwWinCount,DWORD dwLostCount,DWORD dwDrawCount);
	//�л�����
	bool SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,BYTE cbDeviceType=DEVICE_TYPE_PC,WORD wBehaviorFlags=0,WORD wPageTableCount=0);

	//���ͺ���
protected:
	//�û���Ϣ
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem,DWORD dwSocketID,bool bSendAndroidFalg=false);

	//��������
protected:
	//��ǰ�¼�
	bool OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem);
	//������Ϣ
	bool SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount);
	//����ЧӦ
	bool SendPropertyEffect(IServerUserItem * pIServerUserItem, WORD wPropKind);

	//��������
protected:
	//��¼ʧ��
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID);
	//����ʧ��
	bool SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,BYTE cbActivityGame);
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);
	//����ʧ��
	bool SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea);

	//��������
public:
	//���û�
	IServerUserItem * GetBindUserItem(WORD wBindIndex);
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//��������
	WORD GetPropertyType(WORD wPropertyIndex);

	//��������
protected:
	//���û���
	bool InitAndroidUser();
	//��������
	bool InitTableFrameArray();
	//���ñ���
	bool InitMatchServiceManager();
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//���ò���
	void SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount);	
	//Ⱥ���û���Ϣ
	bool SendUserInfoPacketBatchToMobileUser(IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//�������û���Ϣ
	bool SendVisibleTableUserInfoToMobileUser(IServerUserItem * pIServerUserItem,WORD wTablePos);
	//���͸�����Ϣ��ĳһҳ
	bool SendUserInfoPacketBatchToPage(IServerUserItem * pIServerUserItem);
	//�ֻ�������¼
	bool MobileUserImmediately(IServerUserItem * pIServerUserItem);
	//����ϵͳ��Ϣ
	bool SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize);
	//���¿���
	bool OnDBUpdateControlInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
protected:
	//�Ƴ���Ϣ
	void RemoveSystemMessage();
};

//////////////////////////////////////////////////////////////////////////////////

#endif