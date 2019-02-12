#ifndef PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE
#define PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE

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
#ifndef PERSONAL_ROOM_SERVICE_CLASS
	#ifdef  PERSONAL_ROOM_SERVICE_DLL
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PERSONAL_ROOM_SERVICE_DLL_NAME		TEXT("PersonalRoomService.dll")			//�������
#else
	#define PERSONAL_ROOM_SERVICE_DLL_NAME		TEXT("PersonalRoomServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//ȡ������ԭ��
#define UNSIGNUP_REASON_PLAYER		0									//���ȡ��
#define UNSIGNUP_REASON_SYSTEM 		1									//ϵͳȡ��

//////////////////////////////////////////////////////////////////////////////////
//��������
class CLockTimeMatch;

//////////////////////////////////////////////////////////////////////////////////
//Լս������
struct tagPersonalRoomManagerParameter
{
	//���ò���
	tagPersonalRoomOption *			pPersonalRoomOption;					//Լս������	
	tagGameServiceOption *			pGameServiceOption;					//��������
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������

	//�ں����
	ITimerEngine *					pITimerEngine;						//ʱ������
	IDBCorrespondManager *			pICorrespondManager;				//��������
	ITCPNetworkEngineEvent *		pTCPNetworkEngine;					//��������
	ITCPNetworkEngine *				pITCPNetworkEngine;				//��������
	ITCPSocketService *				pITCPSocketService;				//�������

	//�������
	IAndroidUserManager *			pIAndroidUserManager;				//��������
	IServerUserManager *			pIServerUserManager;				//�û�����
	IMainServiceFrame *				pIMainServiceFrame;					//������
	IServerUserItemSink *			pIServerUserItemSink;				//�û��ӿ�
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPersonalRoomServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0xaf116139, 0xa0, 0x40e3, 0xaa, 0x7b, 0x2f, 0x41, 0xf2, 0x3d, 0xb1, 0x89};
#else
#define VER_IPersonalRoomServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0xc59d784b, 0x8875, 0x41f5, 0xa3, 0xd0, 0x23, 0x94, 0x96, 0xe0, 0x28, 0x3c};
#endif

//Լս������������ӿ�
interface IPersonalRoomServiceManager : public IUnknownEx
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
	virtual bool CreatePersonalRoom(BYTE cbPersonalRoomType)=NULL;
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wChairID)=NULL;
	//��ʼ���ӿ�
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalRoomManagerParameter)=NULL;	
	
	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//�����¼�
public:
	//Լս�����¼�
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	
	//Լս�������¼�
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//�û��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)=NULL;

	//�ӿ���Ϣ
public:
	//�û��ӿ�
	virtual IUnknownEx * GetServerUserItemSink()=NULL;
	//�û��ӿ�
	//virtual IUnknownEx * GetMatchUserItemSink()=NULL;
};


//////////////////////////////////////////////////////////////////////////////////


#ifdef _UNICODE
#define VER_IPersonalRoomItem	 INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomItem = {0x758b6167, 0x248f, 0x4138, 0xac, 0xcb, 0x1f, 0x72, 0x70, 0x8, 0x25, 0xc9};
#else
#define VER_IPersonalRoomItem	 INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomItem = {0xdb849912, 0x7834, 0x4429, 0xae, 0xa, 0x4c, 0x1a, 0x96, 0x87, 0x38, 0xa9};
#endif

//��Ϸ�����ӿ�
interface IPersonalRoomItem : public IUnknownEx
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
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & MatchManagerParameter)=NULL;	
	
	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//�����¼�
public:
	//Լս�����¼�
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	
	//Լս�������¼�
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��Ϣ�ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//��¼���
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////


#ifdef _UNICODE
#define VER_IPersonalRoomEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomEventSink={0x8d288098, 0x818c, 0x40a8, 0x9b, 0x8d, 0xb5, 0x19, 0xbb, 0x94, 0x40, 0x7d};
#else
#define VER_IPersonalRoomEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomEventSink={0x2650c8a7, 0x313d, 0x4635, 0xbf, 0x27, 0x4b, 0x3a, 0xd0, 0x5e, 0x0, 0x4};
#endif

//��Ϸ�¼�
interface IPersonalRoomEventSink :public IUnknownEx
{
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
	virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID,  DWORD dwKindID, TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)=NULL;

	//��Ϸ����
	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	virtual bool OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo,SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo)=NULL;

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
#define VER_IPersonalTableFrameHook	 INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalTableFrameHook={0x958d9add, 0xe98c, 0x4067, 0x8a, 0xca, 0x1c, 0x32, 0x6c, 0xb8, 0x1e, 0x72};
#else
#define VER_IPersonalTableFrameHook	 INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalTableFrameHook={0x10ac366e, 0x9d0c, 0x41b6, 0x9b, 0x2e, 0x39, 0x9a, 0x10, 0x13, 0x17, 0x81};
#endif

//���ӹ��ӽӿ�
interface IPersonalTableFrameHook : public IUnknownEx
{
	//����ӿ�
public:
	//���ýӿ�
	virtual bool  SetPersonalRoomEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//��ʼ��
	virtual bool  InitTableFrameHook(IUnknownEx * pIUnknownEx)=NULL;	

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart(WORD wChairCount)=NULL;
	//Լս��д������Ϣ
	virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID,  DWORD dwKindID, TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)=NULL;

	//��Ϸ����
	virtual bool  OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//��Ϸ����
	virtual bool  OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo)=NULL;

};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
DECLARE_MODULE_HELPER(PersonalRoomServiceManager,PERSONAL_ROOM_SERVICE_DLL_NAME,"CreatePersonalRoomServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif