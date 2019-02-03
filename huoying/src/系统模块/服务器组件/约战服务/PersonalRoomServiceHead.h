#ifndef PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE
#define PERSONAL_ROOM_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_Correspond.h"
//#include "..\..\消息定义\CMD_GameServer.h"

//平台文件
//#include "..\..\公共组件\服务核心\ServiceCoreHead.h"
//#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
//#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MATCH_SERVICE_CLASS
	#ifdef  PERSONAL_ROOM_SERVICE_DLL
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PERSONAL_ROOM_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG																 
	#define PERSONAL_ROOM_SERVICE_DLL_NAME	TEXT("PersonalRoomService.dll")			//组件名字
#else
	#define PERSONAL_ROOM_SERVICE_DLL_NAME	TEXT("PersonalRoomServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//取消报名原因
#define UNSIGNUP_REASON_PLAYER		0									//玩家取消
#define UNSIGNUP_REASON_SYSTEM 		1									//系统取消

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPersonalRoomServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IPersonalRoomServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomServiceManager={0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
#endif

class CTableFrame;
typedef CWHArray<CTableFrame *>		CTableFrameArray;					//桌子数组
struct tagBindParameter;
//比赛服务器管理接口
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

	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink()=NULL;
	//用户接口
	//virtual IUnknownEx * GetMatchUserItemSink()=NULL;
	//测试
public:
	virtual void TestPersonal() = NULL;
	//查询房间
	virtual bool OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService) = NULL;

	//搜索房间桌号
	virtual bool OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService) = NULL;

	//强制解散搜索房间桌号
	virtual bool OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService) = NULL;

	//私人房间配置
	virtual bool OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID, IDataBaseEngine *	 pIDataBaseEngine) = NULL;

	//查询私人房间列表
	virtual bool OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID,  ITCPSocketService * pITCPSocketService) = NULL;

	///玩家请求房间成绩
	virtual bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID,  IDataBaseEngine *	 pIDataBaseEngine) = NULL;

};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPersonalRoomItem INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomItem={0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b};
#else
#define VER_IPersonalRoomItem INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomItem={0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53};
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

};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IPersonalRoomEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomEventSink={0x9d49ab20,0x472c,0x4b3a,0x00bc,0xb4,0x92,0xfe,0x8c,0x41,0xcd,0xaa};
#else
#define VER_IPersonalRoomEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IPersonalRoomEventSink={0x91cf29a0,0x04d3,0x48da,0x0083,0x36,0x64,0xb2,0xda,0x6a,0x21,0xdb};
#endif

//游戏事件
interface IPersonalRoomEventSink :public IUnknownEx
{
//public:
//	//游戏开始
//	virtual bool  OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)=NULL;
//	//游戏结束
//	virtual bool  OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
//
//	//用户事件
//public:
//	//玩家返赛
//	virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)=NULL;
//
//	//玩家动作
//public:
//	//用户坐下
//	virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
//	//用户起来
//	virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
//	//用户同意
//	virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

///////////////////////////////////////////////////////////////////////////
//#ifdef _UNICODE
//#define VER_ITableFrameHook INTERFACE_VERSION(1,1)
//static const GUID IID_ITableFrameHook={0xe9f19de8,0xfccb,0x42bd,0x0099,0x85,0xac,0xe9,0x26,0xf3,0xc4,0x2b};
//#else
//#define VER_ITableFrameHook INTERFACE_VERSION(1,1)
//static const GUID IID_ITableFrameHook={0x94a90d45,0x570e,0x41d6,0x009f,0x20,0x01,0x8d,0x68,0x16,0xd5,0x0f};
//#endif

////桌子钩子接口
//interface ITableFrameHook : public IUnknownEx
//{
//	//管理接口
//public:
//	//设置接口
//	virtual bool  SetMatchEventSink(IUnknownEx * pIUnknownEx)=NULL;
//	//初始化
//	virtual bool  InitTableFrameHook(IUnknownEx * pIUnknownEx)=NULL;	
//
//	//游戏事件
//public:
//	//游戏开始
//	virtual bool  OnEventGameStart(WORD wChairCount)=NULL;
//	//游戏结束
//	virtual bool  OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
//
//	//用户事件
//public:
//	//玩家返赛
//	virtual bool OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)=NULL;
//};

//////////////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_HELPER(PersonalRoomServiceManager,PERSONAL_ROOM_SERVICE_DLL_NAME,"CreatePersonalRoomServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif