#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//�Թ�����
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//��¼����
typedef CMap<DWORD, DWORD, LONGLONG, LONGLONG&> CMapPersonalTabelScoreInfo;

interface IMatchTableFrameNotify;

//ȡ������ԭ��
#define CANCELTABLE_REASON_PLAYER		0								//���ȡ��
#define CANCELTABLE_REASON_SYSTEM 		1								//ϵͳȡ��
#define CANCELTABLE_REASON_ENFOCE 		2							//ǿ�ƽ�ɢ����
#define CANCELTABLE_REASON_ERROR 		3								//����ȡ��
#define CANCELTABLE_REASON_NOT_START	4
#define CANCELTABLE_REASON_OVER_TIME	5

//////////////////////////////////////////////////////////////////////////////////

//���ӿ��
class CTableFrame : public ITableFrame, public ICompilationSink
{
	//��Ϸ����
protected:
	WORD							m_wTableID;							//���Ӻ���
	WORD							m_wChairCount;						//������Ŀ
	BYTE							m_cbStartMode;						//��ʼģʽ
	WORD							m_wUserCount;						//�û���Ŀ

	//״̬����
protected:
	bool							m_bGameStarted;						//��Ϸ��־
	bool							m_bDrawStarted;						//��Ϸ��־
	bool							m_bTableStarted;					//��Ϸ��־
	bool							m_bTableInitFinish;					//��ʼ��ʶ
	//bool							m_lWriteuser[MAX_CHAIR];			//���д��
	bool							m_bFirstRegister;					//��һ�ε�½
	//״̬����
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//�Թ۱�־
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//�������

	//��Ϸ����
protected:
	SCORE							m_lCellScore;						//��Ԫ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	//ʱ�����
protected:
	DWORD							m_dwDrawStartTime;					//��ʼʱ��
	SYSTEMTIME						m_SystemTimeStart;					//��ʼʱ��
	WORD                            m_wDrawCount;                       //��Ϸ����

	//��̬����
protected:
	DWORD							m_dwTableOwnerID;					//�����û�
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//��������

	//���߱���
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//���ߴ���
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//����ʱ��

	//�û�����
protected:
	CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	CLookonUserItemArray			m_LookonUserItemArray;				//�Թ��û�

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//����ӿ�
	IAndroidUserManager *			m_pIAndroidUserManager;				//�����ӿ�

	//��չ�ӿ�
protected:
	ITableUserAction *				m_pITableUserAction;				//�����ӿ�
	ITableUserRequest *				m_pITableUserRequest;				//����ӿ�	

	//���ݽӿ�
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//�ں�����
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��¼����

	//������Ϣ
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameMatchOption *			m_pGameMatchOption;					//��������	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��Ϸ��¼
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//��Ϸ��¼
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//��Ϸ��¼

	//�����ӿ�
protected:
	ITableFrameHook	*				m_pITableFrameHook;					//�����ӿ�
	ITableUserAction *				m_pIMatchTableAction;				//�����ӿ�

	//˽������
protected:
	IServerUserItem*			m_pTableOwnerUser;					//������Ϣ
	DWORD							m_dwDrawCountLimit;					//��������
	DWORD							m_dwDrawTimeLimit;					//ʱ������
	DWORD							m_dwPersonalPlayCount;				//Լս����
	LONGLONG						m_lIniScore;						//��ʼ����
	DWORD							m_dwTimeBegin;						//��ʼʱ��
	DWORD							m_PlayerWaitTime;					//�ȴ����ʱ��
	DWORD							m_TableOwnerWaitTime;				//�ȴ�����ʱ��
	DWORD							m_GameStartOverTime;				//�ȴ���ʼʱ��
	bool									m_bWaitConclude;					//�ȴ�����
	DWORD							m_dwReplyCount;						//�ظ�����
	bool									m_bRequestReply[MAX_CHAIR];			//�����
	CMapPersonalTabelScoreInfo		m_MapPersonalTableScoreInfo;		//��ҷ���
	TCHAR								m_szPersonalTableID[7];
	bool									m_bIsDissumGame;					//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	byte									m_cbGanmeRule[RULE_LEN];
	bool									m_bIsAllRequstCancel;
	bool							m_bPersonalLock;					//˽������
	bool									m_bIsEnfoceDissum;		//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	//˽�˷���ս����¼
public:
	tagPersonalUserScoreInfo	m_PersonalUserScoreInfo[MAX_CHAIR];		//˽�˷���ս����¼
	byte							m_cbSpecialInfo[SPECIAL_INFO_LEN];//˽�˷����ɢͳ����Ϣ
	int							m_nSpecialInfoLen;								//˽�˷���ͳ����Ϣ����
	tagPersonalTableParameter	m_PersonalTableParameter;		//˽�˷�������Ϣ
	tagPersonalRoomOption			m_PersonalRoomOption;				//˽�˷�������
	DWORD					m_dwRecordTableID;
	bool							m_bBeOut[MAX_CHAIR];			//�Ƿ��߳�			

	//״̬

	//��������
public:
	//���캯��
	CTableFrame();
	//��������
	virtual ~CTableFrame();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_wTableID; }
	//��Ϸ����
	virtual WORD GetChairCount() { return m_wChairCount; }
	//��λ����Ŀ
	virtual WORD GetNullChairCount(){return m_wChairCount - m_wUserCount;}

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameMatchOption * GetGameMatchOption() { return m_pGameMatchOption; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//���ýӿ�
public:
	//��ʼģʽ
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//��ʼģʽ
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode=cbStartMode; }

	//��Ԫ����
public:
	//��Ԫ����
	virtual SCORE GetCellScore() { return m_lCellScore; }
	//��Ԫ����
	virtual VOID SetCellScore(SCORE lCellScore);

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//��Ϸ״̬
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//��Ϸ״̬
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//����״̬
	virtual bool IsTableLocked() { return (m_szEnterPassword[0]!=0); }

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();
	//��ɢ��Ϸ
	virtual bool DismissGame();
	//������Ϸ
	virtual bool ConcludeGame(BYTE cbGameStatus);
	//��������
	virtual bool ConcludeTable();

	//�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);
	//�Թ��û�
	virtual IServerUserItem * SearchUserItemGameID(DWORD dwGameID);
	//д�ֽӿ�
public:
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal=INVALID_DWORD, DWORD dwPlayGameTime=INVALID_DWORD);
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//����ӿ�
public:
	//����˰��
	virtual SCORE CalculateRevenue(WORD wChairID, SCORE lScore);
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID);

	//��Ϸ�û�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//�Թ��û�
public:
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����û�
public:
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ϵͳ��Ϣ
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative=false);
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword=NULL);

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//���ýӿ�
	virtual bool SetTableFrameHook(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual IUnknownEx * GetTableFrameHook(){ return m_pITableFrameHook;}
	//α������
	virtual bool ImitateGameOption(IServerUserItem * pIServerUserItem);
	
	//����ӿ�
public:
	virtual bool SendChatMessage(IServerUserItem * pIServerUserItem,IServerUserItem * pITargetServerUserItem,DWORD dwChatColor, LPCTSTR lpszChatString,LPTSTR lpszDescribeString);
	//��ȡ��Ϣ
public:
	//��ȡ��Ϣ
	virtual LPCTSTR GetCompilation(){return szCompilation;}

	//���ܺ���
public:
	//��Ϸ����
	WORD GetDrawCount();
	//��ȡ��λ
	WORD GetNullChairID();
	//�����λ
	WORD GetRandNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();
	//�Թ���Ŀ
	WORD GetLookonUserCount();
	//������Ŀ
	WORD GetOffLineUserCount();
	//���ӻ���������
	WORD GetTableAndroidConut();
	//���ܺ���
public:
	//�û�״��
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//��������
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//�û��¼�
public:
	//�����¼�
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//�����¼�
	bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ�¼�
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//����¼�
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��������
public:
	//����״̬
	bool SendTableStatus();
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem);

public:
	//������
	bool CheckDistribute();
	//��Ϸ��¼
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime=INVALID_DWORD);

	//˽������
	virtual bool IsPersonalTableLocked() { return m_bPersonalLock; }
	//��������
	virtual VOID SetPersonalTableLlocked(bool bLocked) { m_bPersonalLock = bLocked; }
	//��ȡ����
	virtual bool GetPersonalScore(DWORD dwUserID, LONGLONG& lScore);
	//������Ϣ
	virtual tagPersonalTableParameter GetPersonalTableParameter();

	//˽������
public:
	//��������
	virtual VOID SetTableOwner(DWORD dwUserID) { m_dwTableOwnerID = dwUserID; };
	//��ȡ����
	virtual DWORD GetTableOwner() { return m_dwTableOwnerID; };
	//α������
	virtual DWORD GetRecordTableOwner() ;
	//��������
	virtual VOID SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore);
	//�������Ӳ���
	virtual VOID SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter ,  tagPersonalRoomOption PersonalRoomOption);
	//���Ӵ�����೤ʱ��δ��ʼ��Ϸ ��ɢ����
	virtual VOID SetTimerNotBeginAfterCreate();

	//�����ɢ
	bool CancelTableRequest(DWORD dwUserID, WORD dwChairID);
	//��ɢ��
	bool CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree);
	//˽�˱��
	void SetPersonalTableID(LPCTSTR szTableID);
	//����ǿ�ƽ�ɢ����
	void HostDissumeGame(BOOL bIsEnforce = false);

	//��ȡ˽�˷���
	TCHAR * GetPersonalTableID();

	//������Ϸ����
	virtual VOID SetGameRule(byte * cbRule,  int nLen);

	//��ȡ��Ϸ����
	virtual byte * GetGameRule();

	//��ȡ����ʱ��������Ϣ
	virtual void GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen);

	//�������������ӵĸ���
	virtual VOID SetTableChairCount(WORD wChairCount);

public:
	byte m_cbDataBaseMode;			//ʹ�õ����ݿ�ģʽ  0 Ϊ��ҿ� 1 Ϊ ���ֿ�

	//˽�˷����Ƿ��ɢ
	virtual bool IsPersonalRoomDisumme();

	//�����ǽ�����ݿ⻹�ǻ������ݿ�,  0 Ϊ��ҿ� 1 Ϊ ���ֿ�
	virtual void SetDataBaseMode(byte cbDataBaseMode);

	//��ȡ���ݿ�ģʽ,  0 Ϊ��ҿ� 1 Ϊ ���ֿ�
	virtual byte GetDataBaseMode();


};

//////////////////////////////////////////////////////////////////////////////////

#endif