#ifndef PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE
#define PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef PERSONAL_ROOM_SERVICE_CLASS
	#ifdef  PERSONAL_ROOM_SERVICE_DLL
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PERSONAL_ROOM_SERVICE_DLL_NAME		TEXT("PersonalRoomService.dll")			//组件名字
#else
	#define PERSONAL_ROOM_SERVICE_DLL_NAME		TEXT("PersonalRoomServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//取消报名原因
#define UNSIGNUP_REASON_PLAYER		0									//玩家取消
#define UNSIGNUP_REASON_SYSTEM 		1									//系统取消

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CLockTimeMatch;

//////////////////////////////////////////////////////////////////////////////////
//约战房参数
struct tagPersonalRoomManagerParameter
{
	//配置参数
	tagPersonalRoomOption *			pPersonalRoomOption;					//约战房配置	
	tagGameServiceOption *			pGameServiceOption;					//服务配置
	tagGameServiceAttrib *			pGameServiceAttrib;					//服务属性

	//内核组件
	ITimerEngine *					pITimerEngine;						//时间引擎
	IDBCorrespondManager *			pICorrespondManager;				//数据引擎
	ITCPNetworkEngineEvent *		pTCPNetworkEngine;					//网络引擎
	ITCPNetworkEngine *				pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				pITCPSocketService;				//网络服务

	//服务组件
	IAndroidUserManager *			pIAndroidUserManager;				//机器管理
	IServerUserManager *			pIServerUserManager;				//用户管理
	IMainServiceFrame *				pIMainServiceFrame;					//服务框架
	IServerUserItemSink *			pIServerUserItemSink;				//用户接口
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPersonalRoomServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0xaf116139, 0xa0, 0x40e3, 0xaa, 0x7b, 0x2f, 0x41, 0xf2, 0x3d, 0xb1, 0x89};
#else
#define VER_IPersonalRoomServiceManager	INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0xc59d784b, 0x8875, 0x41f5, 0xa3, 0xd0, 0x23, 0x94, 0x96, 0xe0, 0x28, 0x3c};
#endif

//约战房服务器管理接口
interface IPersonalRoomServiceManager : public IUnknownEx
{
	//控制接口
public:
	//停止服务
	virtual bool StopService()=NULL;
	//启动服务
	virtual bool StartService()=NULL;

	//管理接口
public:
	//创建比赛
	virtual bool CreatePersonalRoom(BYTE cbPersonalRoomType)=NULL;
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wChairID)=NULL;
	//初始化接口
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & PersonalRoomManagerParameter)=NULL;	
	
	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//网络事件
public:
	//约战服务事件
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	
	//约战服务器事件
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//用户接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//登录完成
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)=NULL;

	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink()=NULL;
	//用户接口
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

//游戏比赛接口
interface IPersonalRoomItem : public IUnknownEx
{
	//控制接口
public:
	//启动通知
	virtual void OnStartService()=NULL;

	//管理接口
public:	
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)=NULL;
	//初始化接口
	virtual bool InitPersonalRooomInterface(tagPersonalRoomManagerParameter & MatchManagerParameter)=NULL;	
	
	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)=NULL;

	//网络事件
public:
	//约战服务事件
	virtual bool OnEventSocketPersonalRoom(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)=NULL;	
	//约战服务器事件
	virtual bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//信息接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem)=NULL;
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem)=NULL;
	//登录完成
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

//游戏事件
interface IPersonalRoomEventSink :public IUnknownEx
{
public:
	//游戏开始
	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
	virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID,  DWORD dwKindID, TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)=NULL;

	//游戏结束
	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	virtual bool OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo,SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo)=NULL;

	//玩家动作
public:
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户同意
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

//桌子钩子接口
interface IPersonalTableFrameHook : public IUnknownEx
{
	//管理接口
public:
	//设置接口
	virtual bool  SetPersonalRoomEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//初始化
	virtual bool  InitTableFrameHook(IUnknownEx * pIUnknownEx)=NULL;	

	//游戏事件
public:
	//游戏开始
	virtual bool  OnEventGameStart(WORD wChairCount)=NULL;
	//约战房写参与信息
	virtual void	PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID,  WORD wChairID,  DWORD dwKindID, TCHAR * szRoomID,  TCHAR * szPersonalRoomGUID)=NULL;

	//游戏结束
	virtual bool  OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//游戏结束
	virtual bool  OnEventGameEnd(WORD wTableID,  WORD wChairCount, DWORD dwDrawCountLimit, DWORD & dwPersonalPlayCount, int nSpecialInfoLen, byte * cbSpecialInfo, SYSTEMTIME sysStartTime, tagPersonalUserScoreInfo * PersonalUserScoreInfo)=NULL;

};

//////////////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_HELPER(PersonalRoomServiceManager,PERSONAL_ROOM_SERVICE_DLL_NAME,"CreatePersonalRoomServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif