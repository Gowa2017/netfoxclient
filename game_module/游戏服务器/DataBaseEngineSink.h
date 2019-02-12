#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//友元定义
	friend class CServiceUnits;

	//辅助变量
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//登录失败
	DBO_GR_LogonSuccess				m_LogonSuccess;						//登录成功

	//游戏数据库
protected:
	CDataBaseAide					m_GameDBAide;						//游戏数据
	CDataBaseHelper					m_GameDBModule;						//游戏数据

	//游戏币数据库
protected:
	CDataBaseAide					m_TreasureDBAide;					//游戏币数据库
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//配置变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagDataBaseParameter *			m_pDataBaseParameter;				//连接信息
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//组件变量
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//引擎接口
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//查询接口
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//数据接口

	//组件变量
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //数据协调

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置参数
public:
	//自定配置
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//服务属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//服务配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//获取对象
public:
	//获取对象
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//获取对象
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//功能接口
public:
	//投递结果
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录函数
protected:
	//I D 登录
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch=false);
	//I D 登录
	bool OnRequestLogonMobile(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//帐号登录
	bool OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//系统处理
protected:
	//游戏写分
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//离开房间
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//游戏记录
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载参数
	bool OnRequestLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载列表
	bool OnRequestLoadGameColumn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载机器
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载机器
	bool OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//等级配置
	bool OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//购买道具
	bool OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//背包道具
	bool OnRequestPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//使用道具
	bool OnRequestPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询赠送
	bool OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//赠送道具
	bool OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//获取赠送
	bool OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载Buff
	bool OnRequestLoadGameBuff(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载喇叭
	bool OnRequestLoadGameTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//发送喇叭
	bool OnRequestSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//用户游戏数据
	bool OnRequestWriteUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//用户权限
	bool OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛权限
	bool OnRequestManageMatchRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//系统消息
	bool OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载敏感词
	bool OnRequestLoadSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//解锁机器人
	bool OnRequestUnlockAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//比赛函数
protected:
	//比赛报名
	bool OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//退出比赛
	bool OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//报名开始
	bool OnRequestMatchSignupStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询资格
	bool OnRequestMatchQueryQualify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//购买保险
	bool OnRequestMatchBuySafeCard(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛开始
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛结束
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛取消
	bool OnRequestMatchCancel(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛奖励
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//比赛淘汰
	bool OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//记录成绩
	bool OnRequestMatchRecordGrades(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//玩家复活
	bool OnRequestMatchUserRevive(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询复活
	bool OnRequestMatchQueryReviveInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//等级服务
protected:
	//查询等级
	bool OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//银行服务
protected:
	//开通银行
	bool OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//存入游戏币
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//提取游戏币
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//转帐游戏币
	bool OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询银行
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询用户
	bool OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//任务服务
protected:
	//放弃任务
	bool OnRequestTaskGiveUp(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//领取任务
	bool OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//领取奖励
	bool OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载任务
	bool OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//查询任务
	bool OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//推进任务
	bool OnRequestRoomTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//会员服务
protected:
	//会员参数
	bool OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//会员查询
	bool OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//会员送金
	bool OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//会员礼包
	bool OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//兑换服务
protected:
	//购买会员
	bool OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//兑换游戏币
	bool OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//兑换游戏币
	bool OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//私人房间
protected:
	//创建桌子
	bool OnRequestCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//插入创建桌子记录
	bool OnRequsetInsertCreateRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//解散房间
	bool OnRequsetDissumeRoom(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//取消创建
	bool OnRequestCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//玩家强制取消
	bool OnRequestHostCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//私人配置
	bool OnRequestLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//玩家获取房间成绩
	bool OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//游戏写分
	bool OnRequestWritePersonalGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//写入参与信息
	bool OnRequestWriteJoinInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//签到服务
protected:
	//加载签到
	bool OnRequestLoadCheckIn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询签到
	bool OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//执行签到
	bool OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//低保服务
protected:
	//加载低保
	bool OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//领取低保
	bool OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//辅助函数
private:
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbDeviceType=DEVICE_TYPE_PC,WORD wBehaviorFlags=0,WORD wPageTableCount=0);
	//银行结果
	VOID OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbActivityGame=FALSE);
};

//////////////////////////////////////////////////////////////////////////////////

#endif