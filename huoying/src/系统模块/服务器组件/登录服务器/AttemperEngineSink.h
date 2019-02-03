#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include "GamePropertyListManager.h"
#include "../约战服务/PersonalRoomServiceHead.h"
#include "CHttpKernel.h"

//////////////////////////////////////////////////////////////////////////////////

time_t SystemTimeToTimet(SYSTEMTIME st);

//////////////////////////////////////////////////////////////////////////////////
//数据定义

//连接类型
#define CLIENT_KIND_FALSH			1									//网页类型
#define CLIENT_KIND_MOBILE			2									//手机类型
#define CLIENT_KIND_COMPUTER		3									//电脑类型

//绑定参数
struct tagBindParameter
{
	//网络参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//连接信息
	WORD							wModuleID;							//模块标识
	BYTE							cbClientKind;						//连接类型
	DWORD							dwPlazaVersion;						//大厅版本
};

//平台配置
struct tagPlatformParameter
{
	DWORD							dwExchangeRate;						//兑换比率
	DWORD							dwPresentExchangeRate;				//魅力游戏币兑换率
	DWORD							dwRateGold;							//游戏豆游戏币兑换率
};


//验证码参数
struct tagVerifyCode
{
	DWORD							dwSocketID;							//网络句柄
	DWORD							dwClientAddr;						//客户端IP
	DWORD							dwUpdateTime;						//更新时间
	BYTE							cbVerifyCodeType;					//验证码类型
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//验证码绑定的手机号
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
	TCHAR							szValidateCode[LEN_VERIFY_CODE];	//验证码
};
//////////////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagVerifyCode *>	CVerifyCodeArray;					//验证码列表

typedef CMap<WORD,WORD,tagServerDummyOnLine *,tagServerDummyOnLine *> ServerDummyMap;		//当前有效虚拟人数
typedef CWHArray<tagServerDummyOnLine *>	ServerDummyArray;			//虚拟人数容器
//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bNeekCorrespond;					//协调标志
	bool                            m_bShowServerStatus;                //显示服务器状态
	char							m_szHttpRegisterValidCode[MAX_PATH];//注册申请验证码地址
	char							m_szHttpModifyPassValidCode[MAX_PATH];//修改密码申请验证码地址
	char							m_szHttpModifyBankInfoValidCode[MAX_PATH];//修改银行信息申请验证码地址

	//配置变量
protected:
	SCORE							m_lCheckInReward[LEN_WEEK];			//签到奖励
	tagPlatformParameter			m_PlatformParameter;				//平台参数
	tagBaseEnsureParameter			m_BaseEnsureParameter;				//低保参数	

	//任务参数
protected:
	WORD							m_wTaskCount;						//任务数目
	WORD							m_wTaskCountBuffer;					//任务数目
	tagTaskParameter				m_TaskParameter[TASK_MAX_COUNT];	//任务参数
	tagTaskParameter				m_TaskParameterBuffer[TASK_MAX_COUNT];//任务参数

	//会员参数
protected:
	WORD							m_wMemberCount;						//会员数目
	tagMemberParameterNew			m_MemberParameter[10];				//会员参数

	//等级配置
protected:
	WORD							m_wLevelCount;						//等级数目
	tagGrowLevelConfig				m_GrowLevelConfig[60];				//等级配置

	//认证参数
protected:
	tagAuthRealParameter			m_AuthRealParameter;				//认证参数

	//变量定义
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组
	ServerDummyMap					m_ServerDummyOnLineMap;				//当前时间段虚拟在线
	ServerDummyArray				m_ServerDummyOnLineArray;			//所有时间段虚拟人数
	//组件变量
protected:
	CServerListManager				m_ServerListManager;				//列表管理
	CVerifyCodeArray				m_VerifyCodeArray;					//验证数组
	CGamePropertyListManager		m_GamePropertyListManager;			//道具管理
	
	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketService;				//协调服务
	CHttpKernel<CAttemperEngineSink> *	m_pHttpClientService;			//HTTP服务
public:
	IPersonalRoomServiceManager * m_pIPersonalRoomServiceManager;	
	//视频配置
protected:
	WORD							m_wAVServerPort;					//视频端口
	DWORD							m_dwAVServerAddr;					//视频地址

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

	//HTTP事件接口
protected:
	//POST请求回调
	//void OnClientPostCallBack(struct evhttp_request *req, void * pArg);
	//生成注册验证码
	void OnClientGetVerifyCodeCallBack(struct evhttp_request *req, void * pArg);
	//重新生成注册验证码
	void OnClientGetReBuildVerifyCodeCallBack(struct evhttp_request *req, void * pArg);

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

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//远程服务
	bool OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录处理
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务处理
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//远程处理
	bool OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//机器服务
	bool OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//道具处理
	bool OnTCPNetworkMainPCProperty(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//登录处理
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);	
	//列表处理
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainMBPersonalService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//I D 登录
	bool OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//工具登录
	bool OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//验证资料
	bool OnTCPNetworkSubPCVerifyIndividual(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游客登录
	bool OnTCPNetworkSubPCLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//I D 登录
	bool OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//手机号验证码注册
	bool OnTCPNetworkSubMBPhoneRegisterVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//手机号注册验证码获取
	bool OnTCPNetworkSubMBGetRegisterVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//修改密码验证码获取
	bool OnTCPNetworkSubMBGetModifyPassVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//修改银行信息验证码获取
	bool OnTCPNetworkSubMBGetModifyBankInfoVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//验证码修改密码
	bool OnTCPNetworkSubMBModifyPassVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//验证码修改银行信息
	bool OnTCPNetworkSubMBModifyBankInfoVerifyCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询用户信息
	bool OnTCPNetworkSubMBQueryUserData(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//其他登录
	bool OnTCPNetworkSubMBLogonOtherPlatform(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游客登录
	bool OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//投诉代理
	bool OnTCPNetworkSubMBTouSuAgent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请代理前检查
	bool OnTCPNetworkSubMBShenQingAgentCheck(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请代理
	bool OnTCPNetworkSubMBShenQingAgent(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询游戏锁表信息
	bool OnTCPNetworkSubMBQueryGameLockInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取在线人数
	bool OnTCPNetworkSubMBGetOnline(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询房间信息
	bool OnTCPNetworkSubMBQueryServer(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//创建房间
	bool OnTCPNetworkSubMBCreateTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询房间
	bool OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//搜索房间桌号
	bool OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//强制解散搜索房间桌号
	bool OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//私人房间配置
	bool OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询私人房间列表
	bool	OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//玩家请求房间成绩
	bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//玩家请求房卡和游戏豆信息
	bool OnTCPNetworkSubQueryPersonalRoomUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//房卡兑换游戏币
	bool OnTCPNetworkSubRoomCardExchangeToScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游戏事件
protected:
	//登录成功
	bool OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonValidatePassPort(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//验证结果
	bool OnDBPCLogonVerifyResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
    //平台配置
	bool OnDBPCPlatformParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);	  
	//用户头像
	bool OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行信息
	bool OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行成功
	bool OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行失败
	bool OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//开通结果
	bool OnDBPCUserInsureEnableResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//开通结果
	bool OnDBPCQueryTransferRebateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询UserData结果
	bool OnDBPCQueryUserDataResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到信息
	bool OnDBPCUserCheckInInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到结果
	bool OnDBPCUserCheckInResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到奖励
	bool OnDBPCCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务列表
	bool OnDBPCTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务结束
	bool OnDBPCTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务信息
	bool OnDBPCTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//任务结果
	bool OnDBPCTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//低保参数
	bool OnDBPCBaseEnsureParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//低保结果
	bool OnDBPCBaseEnsureResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//推广参数
	bool OnDBPCUserSpreadInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//实名奖励
	bool OnDBPCRealAuthParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级配置
	bool OnDBPCGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级参数
	bool OnDBPCGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//等级升级
	bool OnDBPCGrowLevelUpgrade(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具信息
	bool OnDBGamePropertyTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具信息
	bool OnDBGamePropertyRelatItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具信息
	bool OnDBGamePropertyItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具信息
	bool OnDBGamePropertySubItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具信息
	bool OnDBGamePropertyListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具购买
	bool OnDBGamePropertyBuy(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具失败
	bool OnDBGamePropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询单个
	bool OnDBQueryPropertySingle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//背包查询
	bool OnDBQueryUserBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家道具Buff
	bool OnDBUserPropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具使用
	bool OnDBGamePropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具赠送
	bool OnDBUserPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询赠送
	bool OnDBQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//获取赠送
	bool OnDBGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员参数
	bool OnDBPCMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员查询
	bool OnDBPCMemberDayQueryInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员送金
	bool OnDBPCMemberDayPresentResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//会员礼包
	bool OnDBPCMemberDayGiftResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//购买结果
	bool OnDBPCPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换结果
	bool OnDBPCExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//房卡游戏兑换游戏币结果
	bool OnDBPCExChangeRoomCardToScoreResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//机器参数
	bool OnDBAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//资料结果
	bool OnDBIndividualResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作成功
	bool OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖配置
	bool OnDBPCLotteryConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖信息
	bool OnDBPCLotteryUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖结果
	bool OnDBPCLotteryResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏数据
	bool OnDBPCQueryUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//代理列表
	bool OnDBPCAgentGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏数据
	bool OnDBMBPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏数据
	bool OnDBMBPersonalFeeList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏数据
	bool OnDBMBPersonalRoomListInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//私人房间玩家请求房间信息
	bool OnDBQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//私人房间玩家请求房卡和游戏豆信息
	bool OnDBQueryPersonalRoomUersInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询绑定银行卡信息结果
	bool OnDBQueryBankInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询游戏锁表信息
	bool OnDBQueryGameLockInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//手机事件
protected:
	//登录成功
	bool OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//代理列表
	bool OnDBMBAgentGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//列表事件
protected:
	//游戏种类
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏类型
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏节点
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏定制
	bool OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏列表
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//房间虚拟人数
	bool OnDBServerDummyOnLine(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//房间虚拟人数发送开始
	bool OnDBServerDummyOnLineBegin(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//房间虚拟人数发送结束
	bool OnDBServerDummyOnLineEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//辅助函数
protected:
	//版本检测
	bool CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer=true);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//发送列表
protected:
	//发送类型
	VOID SendGameTypeInfo(DWORD dwSocketID);
	//发送种类
	VOID SendGameKindInfo(DWORD dwSocketID);
	//发送节点
	VOID SendGameNodeInfo(DWORD dwSocketID, WORD wKindID);
	//发送定制
	VOID SendGamePageInfo(DWORD dwSocketID, WORD wKindID);
	////发送房间
	//VOID SendGameServerInfo(DWORD dwSocketID, WORD wKindID);
	//发送房间
	VOID SendGameServerInfo(DWORD dwSocketID, WORD wKindID, BYTE cbDeviceType);
	//等级配置
	VOID SendGrowLevelConfig(DWORD dwSocketID);
	//发送类型
	VOID SendGamePropertyTypeInfo(DWORD dwSocketID);
	//发送关系
	VOID SendGamePropertyRelatInfo(DWORD dwSocketID);
	//发送道具
	VOID SendGamePropertyInfo(DWORD dwSocketID);
	//发送道具
	VOID SendGamePropertySubInfo(DWORD dwSocketID);
	//会员配置
	VOID SendMemberConfig(DWORD dwContextID);
	//认证配置
	VOID SendRealAuthConfig(DWORD dwContextID);

	//手机列表
protected:
	//发送类型
	VOID SendMobileKindInfo(DWORD dwSocketID);
	//发送房间
	VOID SendMobileServerInfo(DWORD dwSocketID, WORD wKindID);

	//辅助函数
protected:
	//银行失败
	bool SendInsureFailure(DWORD dwSocketID, LONG lResultCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(DWORD dwContextID,LONG lResultCode, LPCTSTR pszDescribe);
	//操作失败
	VOID SendOperateFailure(DWORD dwContextID,LONG lResultCode, LPCTSTR pszDescribe);

	//功能更函数
protected:
	VOID OnChangeDummyOnline();
};

//////////////////////////////////////////////////////////////////////////////////

#endif