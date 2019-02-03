#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ChatServiceHead.h"
#include "ServerUserManager.h"
#include <list>


//绑定参数
struct tagBindParameter
{
	//连接属性
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//版本信息
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//进程版本

	//用户属性
	BYTE							cbClientKind;						//连接类型
	CServerUserItem *				pServerUserItem;					//用户接口
};

//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bCollectUser;						//汇总标志
	bool							m_bNeekCorrespond;					//协调标志

	//控制变量
protected:
	DWORD							m_dwIntervalTime;					//间隔时间
	DWORD							m_dwLastDisposeTime;				//处理时间
	std::list<DWORD>				m_LogonLockList;					//登录锁数组

	//绑定信息
protected:
	tagBindParameter *				m_pNormalParameter;					//绑定信息
	tagBindParameter *				m_pAndroidParameter;				//绑定信息

	//配置信息
protected:
	CInitParameter *				m_pInitParameter;					//配置参数

	//组件变量
protected:
	CServerUserManager				m_ServerUserManager;				//用户管理
	CFriendGroupManager				m_FriendGroupManager;				//好友管理

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IAttemperEngine *				m_pIAttemperEngine;					//调度引擎
	ITCPSocketService *				m_pITCPSocketService;				//网络服务
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);
	//连接处理
protected:
	//列表事件
	bool OnEventTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//用户事件
	bool OnEventTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//用户状态
	bool OnEventTCPSocketSubUserStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//用户强制离线（针对少概率卡线状态）
	bool OnEventTCPSocketSubForceOffline(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
public:
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

//////////////////////////////////////////////////////////////////////////

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//用户处理
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//登录处理
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID登录
	bool OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//账号登录
	bool OnTCPNetworkSubMBLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//好友申请
	bool OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//好友回应
	bool OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//好友邀请
	bool OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//私人邀请
	bool OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户聊天
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查找用户
	bool OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//发送喇叭
	bool OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//删除好友
	bool OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//更新坐标
	bool OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//附近结果
	bool OnTCPNetworkGetNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询结果
	bool OnTCPNetworkQueryNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//分享消息
	bool OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//辅助函数
protected:
	//发送数据
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData = NULL, WORD wDataSize = 0);
	//发送配置
	bool SendChatServerConfig(DWORD dwUserID);
	//发送数据
	bool SendSystemMessage(BYTE cbSendMask, LPCTSTR lpszMessage, WORD wType);
	//发送数据
	bool SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType);
	//登录失败
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID);
	//操作结果
	bool SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID);
	//操作结果
	bool SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID);
	//发送数据
	bool SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize);
	//存储离线消息
	bool SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID);
	//绑定用户
	CServerUserItem * GetBindUserItem(WORD wBindIndex);
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);

//////////////////////////////////////////////////////////////////////////

protected:
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//数据事件

	//登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBAccountLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//配置信息
	bool OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载分组
	bool OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载好友
	bool OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//好友备注
	bool OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//好友资料
	bool OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//离线消息
	bool OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//离线消息
	bool OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改密码
	bool OnDBModifyUserPassword(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改签名
	bool OnDBModifyUserUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改资料
	bool OnDBModifyUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改分组
	bool OnDBModifyUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改好友
	bool OnDBModifyUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//修改备注
	bool OnDBModifyUserRemarks(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加好友
	bool OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作结果
	bool OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查找用户
	bool OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//喇叭消息
	bool OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

};

//////////////////////////////////////////////////////////////////////////////////

#endif