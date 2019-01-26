#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\�������\�������\ServiceCoreHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//�������
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//ȡ������ԭ��
#define UNSIGNUP_REASON_PLAYER		0									//���ȡ��
#define UNSIGNUP_REASON_SYSTEM 		1									//ϵͳȡ��

//////////////////////////////////////////////////////////////////////////////////
//��������
class CLockTimeMatch;

//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagMatchManagerParameter
{
	//���ò���
	tagGameMatchOption *			pGameMatchOption;					//��������	
	tagGameServiceOption *			pGameServiceOption;					//��������
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������

	//�ں����
	ITimerEngine *					pITimerEngine;						//ʱ������
	IDBCorrespondManager *			pICorrespondManager;				//��������
	ITCPNetworkEngineEvent *		pTCPNetworkEngine;					//��������

	//�������
	IAndroidUserManager *			pIAndroidUserManager;				//��������
	IServerUserManager *			pIServerUserManager;				//�û�����
	IMainServiceFrame *				pIMainServiceFrame;					//������
	IServerUserItemSink *			pIServerUserItemSink;				//�û��ӿ�
};

//������Ϣ
struct tagMatchRankInfo
{
	WORD							wRankID;							//��������
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lMatchScore;						//��������
	SCORE							lRewardGold;						//��ҽ���
	SCORE							lRewardIngot;						//Ԫ������
	DWORD							dwRewardExperience;					//���齱��
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IMatchServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IMatchServiceManager={0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IMatchServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IMatchServiceManager={0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
#endif

//��������������ӿ�
interface IMatchServiceManager : public IUnknownEx
{
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService()=NULL;
	//��������
	virtual bool StartService()=NULL;

	//����ӿ�
public:
	//��������
	virtual bool CreateGameMatch(BYTE cbMatchType)=NULL;
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wChairID)=NULL;
	//��ʼ���ӿ�
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)=NULL;	
	
	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	
	//�û��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)=NULL;
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)=NULL;	
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)=NULL;
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwSocketID=INVALID_WORD)=NULL;

	//�ӿ���Ϣ
public:
	//�û��ӿ�
	virtual IUnknownEx * GetServerUserItemSink()=NULL;
	//�û��ӿ�
	//virtual IUnknownEx * GetMatchUserItemSink()=NULL;
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameMatchItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameMatchItem={0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IGameMatchItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameMatchItem={0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
#endif

//��Ϸ�����ӿ�
interface IGameMatchItem : public IUnknownEx
{
	//���ƽӿ�
public:
	//����֪ͨ
	virtual void OnStartService()=NULL;

	//����ӿ�
public:	
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)=NULL;
	//��ʼ���ӿ�
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)=NULL;	
	
	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	

	//��Ϣ�ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)=NULL;
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)=NULL;	
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)=NULL;
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwSocketID=INVALID_WORD)=NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IMatchEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IMatchEventSink={0x9d49ab20,0x472c,0x4b3a,0x00bc,0xb4,0x92,0xfe,0x8c,0x41,0xcd,0xaa};
#else
#define VER_IMatchEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IMatchEventSink={0x91cf29a0,0x04d3,0x48da,0x0083,0x36,0x64,0xb2,0xda,0x6a,0x21,0xdb};
#endif

//��Ϸ�¼�
interface IMatchEventSink :public IUnknownEx
{
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
	//��Ϸ����
	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;

	//�û��¼�
public:
	//��ҷ���
	virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)=NULL;

	//��Ҷ���
public:
	//�û�����
	virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

///////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_ITableFrameHook INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameHook={0xe9f19de8,0xfccb,0x42bd,0x0099,0x85,0xac,0xe9,0x26,0xf3,0xc4,0x2b};
#else
#define VER_ITableFrameHook INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameHook={0x94a90d45,0x570e,0x41d6,0x009f,0x20,0x01,0x8d,0x68,0x16,0xd5,0x0f};
#endif

//���ӹ��ӽӿ�
interface ITableFrameHook : public IUnknownEx
{
	//����ӿ�
public:
	//���ýӿ�
	virtual bool  SetMatchEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//��ʼ��
	virtual bool  InitTableFrameHook(IUnknownEx * pIUnknownEx)=NULL;	

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart(WORD wChairCount)=NULL;
	//��Ϸ����
	virtual bool  OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;

	//�û��¼�
public:
	//��ҷ���
	virtual bool OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
DECLARE_MODULE_HELPER(MatchServiceManager,MATCH_SERVICE_DLL_NAME,"CreateMatchServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif