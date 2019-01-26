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

//连接类型
#define CLIENT_KIND_FALSH			1									//网页类型
#define CLIENT_KIND_MOBILE			2									//手机类型
#define CLIENT_KIND_COMPUTER		3									//电脑类型

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
	IServerUserItem *				pIServerUserItem;					//用户接口
};

//系统消息
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //发送时间
	DBR_GR_SystemMessage            SystemMessage;                     //系统消息
};

//数组说明
typedef CWHArray<CTableFrame *>		CTableFrameArray;					//桌子数组
typedef CWHArray<tagSystemMessage *>   CSystemMessageArray;             //系统消息
typedef CWHArray<tagTaskParameter *>   CTaskParameterArray;             //任务数组
typedef CMap<DWORD,DWORD,DWORD,DWORD &>  CKickUserItemMap;              //踢人映射 
typedef CMap<WORD,WORD,tagTaskParameter *,tagTaskParameter *>  CTaskParameterMap;

typedef CWHArray<tagPersonalTableParameter *> CPersonalTableParameterArray;	//私人配置


//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink, public IMainServiceFrame,public IServerUserItemSink
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

	//绑定信息
protected:
	tagBindParameter *				m_pNormalParameter;					//绑定信息
	tagBindParameter *				m_pAndroidParameter;				//绑定信息

	//配置信息
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagGameMatchOption *			m_pGameMatchOption;					//比赛配置	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置
	CPersonalTableParameterArray	m_PersonalTableParameterArray;		//私人配置
	tagPersonalRoomOption			m_PersonalRoomOption;				//私人房间配置
	bool		m_bHasPersonalRoomService;						//是否有私人房服务

	//配置数据
protected:
	CMD_GR_ConfigColumn				m_DataConfigColumn;					//列表配置

	//低保参数
protected:
	tagBaseEnsureParameter			m_BaseEnsureParameter;				//低保参数	

	//签到参数
protected:
	SCORE							m_lCheckInReward[LEN_WEEK];			//签到奖励

	//会员参数
protected:
	WORD							m_wMemberCount;						//会员数目
	tagMemberParameterNew			m_MemberParameter[10];				//会员参数

	//任务参数
protected:
	WORD							m_wTaskCount;						//任务数目
	tagTaskParameter				m_TaskParameter[TASK_MAX_COUNT];	//任务参数

	//等级配置
protected:
	WORD							m_wLevelCount;						//等级数目
	tagGrowLevelConfig				m_GrowLevelConfig[60];				//等级配置

	//组件变量
protected:
	CTableFrameArray				m_TableFrameArray;					//桌子数组
	CServerListManager				m_ServerListManager;				//列表管理
	CServerUserManager				m_ServerUserManager;				//用户管理
	CAndroidUserManager				m_AndroidUserManager;				//机器管理
	CUserTaskManager				m_UserTaskManager;					//任务管理
	CGamePropertyManager			m_GamePropertyManager;				//道具管理
	CDistributeManager              m_DistributeManage;                 //分组管理
	CKickUserItemMap                m_KickUserItemMap;                  //踢人管理
	CSystemMessageArray             m_SystemMessageActive;              //系统消息
	CSystemMessageArray				m_SystemMessageBuffer;				//消息缓冲	
	CSensitiveWordsFilter			m_WordsFilter;						//脏字过滤

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IAttemperEngine *				m_pIAttemperEngine;					//调度引擎
	ITCPSocketService *				m_pITCPSocketService;				//网络服务
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//时间配置

	//比赛服务
public:	
	IMatchServiceManager			* m_pIMatchServiceManager;			//比赛管理

	//数据引擎
public:
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//数据引擎
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//数据引擎
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //数据协调
protected:
	CRITICAL_SECTION				m_cs_test;							///< 定义一个临界区对象
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
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//消息接口
public:
	//房间消息
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//大厅消息
	virtual bool SendHallMessage(LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);

	//网络接口
public:
	//发送数据
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//群发数据
	virtual bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//功能接口
public:
	//断开协调
	virtual bool DisconnectCorrespond();	
	//插入用户
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//删除用户
	virtual bool DeleteDistribute(IServerUserItem * pIServerUserItem);
	//敏感词过滤
	virtual void SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen);
	//解锁机器人
	virtual VOID UnLockAndroidUser(WORD wServerID, WORD wBatchID);
	//解散私人桌子
	virtual VOID DismissPersonalTable(WORD wServerID, WORD wTableID);
	//取消创建
	virtual VOID CancelCreateTable(DWORD dwUserID, DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, DWORD dwReason, WORD wTableID, TCHAR * szRoomID);
	//开始游戏写入参与信息
	virtual VOID PersonalRoomWriteJoinInfo(DWORD dwUserID, WORD wTableID, TCHAR * szRoomID);
	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind);
	//用户数据
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);

	//数据事件
protected:
	//登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//配置信息
	bool OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//列表信息
	bool OnDBGameColumnInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//机器信息
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//机器参数
	bool OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具BUFF
	bool OnDBGamePropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//喇叭信息
	bool OnDBGameLoadTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//发送喇叭
	bool OnDBGameSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员参数
	bool OnDBGameMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员查询
	bool OnDBPCMemberDayQueryInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员送金
	bool OnDBPCMemberDayPresentResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员礼包
	bool OnDBPCMemberDayGiftResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行信息
	bool OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行成功
	bool OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行失败
	bool OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//开通结果
	bool OnDBUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到信息
	bool OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到结果
	bool OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到奖励
	bool OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//低保参数
	bool OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//低保结果
	bool OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务列表
	bool OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务结束
	bool OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务信息
	bool OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务结果
	bool OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//道具成功
	bool OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具失败
	bool OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具购买结果
	bool OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//背包结果
	bool OnDBQueryPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//使用结果
	bool OnDBQueryPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询赠送
	bool OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具赠送
	bool OnDBQueryPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//获取赠送
	bool OnDBQueryGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//购买结果
	bool OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换结果
	bool OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//系统消息
	bool OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载敏感词
	bool OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载完成
	bool OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级配置
	bool OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级参数
	bool OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级升级
	bool OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建成功
	bool OnDBCreateSucess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//创建失败
	bool OnDBCreateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//取消创建
	bool OnDBCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//私人配置
	bool OnDBLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//解散房间
	bool OnDBDissumeTableResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//私人房间玩家请求房间信息
	bool OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//当前用户的房卡和游戏豆
	bool OnDBCurrenceRoomCardAndBeant(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//汇总事件
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//机器服务
	bool OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//用户处理
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//登录处理
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游戏处理
	bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//框架处理
	bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//银行处理
	bool OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//任务处理
	bool OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//低保处理
	bool OnTCPNetworkMainBaseEnsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//兑换处理
	bool OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//会员处理
	bool OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//道具处理
	bool OnTCPNetworkMainProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//管理处理
	bool OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//比赛命令
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//签到命令
	bool OnTCPNetworkMainCheckIn(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//私人房间
	bool OnTCPNetworkMainPersonalTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//I D 登录
	bool OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//手机登录
	bool OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//用户命令
protected:
	//用户规则
	bool OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户旁观
	bool OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户坐下
	bool OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户起立
	bool OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户聊天
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户私聊
	bool OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户表情
	bool OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户表情
	bool OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//购买道具
	bool OnTCPNetworkSubGamePropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//背包道具
	bool OnTCPNetworkSubPropertyBackpack(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//道具使用
	bool OnTCPNetworkSubPropertyUse(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询赠送
	bool OnTCPNetworkSubQuerySendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//赠送道具
	bool OnTCPNetworkSubPropertyPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取赠送
	bool OnTCPNetworkSubGetSendPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//使用道具
	bool OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//邀请用户
	bool OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//拒绝厌友
	bool OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出命令
	bool OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求用户信息
	bool OnTCPNetworkSubUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求更换位置
	bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求椅子用户信息
	bool OnTCPNetworkSubChairUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);	
	//等级查询
	bool OnTCPNetworkSubGrowLevelQuery(VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//银行命令
protected:	
	//查询银行
	bool OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//开通银行
	bool OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//存款请求
	bool OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//取款请求
	bool OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//转帐请求
	bool OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询用户请求
	bool OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//签到命令
protected:
	//查询签到
	bool OnTCPNetworkSubCheckInQueryRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取奖励
	bool OnTCPNetworkSubCheckInDoneRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//低保命令
protected:
	//查询低保
	bool OnTCPNetworkSubBaseEnsureQueryRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取低保
	bool OnTCPNetworkSubBaseEnsureTakeRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//任务命令
protected:
	//领取任务
	bool OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取奖励
	bool OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//加载任务
	bool OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//放弃任务
	bool OnTCPNetworkSubGiveUpTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//会员命令
protected:
	//会员查询
	bool OnTCPNetworkSubMemberQueryInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//会员送金
	bool OnTCPNetworkSubMemberDayPresent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//会员礼包
	bool OnTCPNetworkSubMemberDayGift(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//兑换命令
protected:
	//查询参数
	bool OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//购买会员
	bool OnTCPNetworkSubPurchaseMember(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//兑换游戏币
	bool OnTCPNetworkSubExchangeScoreByIngot(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//兑换游戏币
	bool OnTCPNetworkSubExchangeScoreByBeans(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//管理命令
protected:
	//查询设置
	bool OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间设置
	bool OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出用户
	bool OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//限制聊天
	bool OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出所有用户
	bool OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//发布消息
	bool OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散游戏
	bool OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//警告命令
	bool OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//私人房间
protected:
	//创建桌子
	bool OnTCPNetworkSubCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//取消请求
	bool OnTCPNetworkSubCancelRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//取消答复
	bool OnTCPNetworkSubRequestReply(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房主强制请求桌子
	bool OnTCPNetworkSubHostDissumeTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//玩家请求房间成绩
	bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//内部事件
protected:
	//用户登录
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine);
	//用户登出
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason);

	//执行功能
protected:
	//分配用户
	bool PerformDistribute();	
	//解锁游戏币
	bool PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason);
	//版本检查
	bool PerformCheckVersion(DWORD dwPlazaVersion, DWORD dwFrameVersion, DWORD dwClientVersion, DWORD dwSocketID);
	//推进任务
	bool PerformTaskProgress(IServerUserItem * pIServerUserItem,tagUserTaskEntry * pUserTaskEntry,DWORD dwWinCount,DWORD dwLostCount,DWORD dwDrawCount);
	//切换连接
	bool SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,BYTE cbDeviceType=DEVICE_TYPE_PC,WORD wBehaviorFlags=0,WORD wPageTableCount=0);

	//发送函数
protected:
	//用户信息
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem,DWORD dwSocketID,bool bSendAndroidFalg=false);

	//辅助函数
protected:
	//购前事件
	bool OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem);
	//道具消息
	bool SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount);
	//道具效应
	bool SendPropertyEffect(IServerUserItem * pIServerUserItem, WORD wPropKind);

	//辅助函数
protected:
	//登录失败
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID);
	//银行失败
	bool SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,BYTE cbActivityGame);
	//请求失败
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);
	//道具失败
	bool SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea);

	//辅助函数
public:
	//绑定用户
	IServerUserItem * GetBindUserItem(WORD wBindIndex);
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//道具类型
	WORD GetPropertyType(WORD wPropertyIndex);

	//辅助函数
protected:
	//配置机器
	bool InitAndroidUser();
	//配置桌子
	bool InitTableFrameArray();
	//配置比赛
	bool InitMatchServiceManager();
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//设置参数
	void SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount);	
	//群发用户信息
	bool SendUserInfoPacketBatchToMobileUser(IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//发可视用户信息
	bool SendVisibleTableUserInfoToMobileUser(IServerUserItem * pIServerUserItem,WORD wTablePos);
	//发送个人信息到某一页
	bool SendUserInfoPacketBatchToPage(IServerUserItem * pIServerUserItem);
	//手机立即登录
	bool MobileUserImmediately(IServerUserItem * pIServerUserItem);
	//发送系统消息
	bool SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize);
	//更新控制
	bool OnDBUpdateControlInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//辅助函数
protected:
	//移除消息
	void RemoveSystemMessage();
};

//////////////////////////////////////////////////////////////////////////////////

#endif