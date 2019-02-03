#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

///////////////////////////////////////////////////////////////////////////////////////////

//移除理由
#define REMOVE_BY_ENDMATCH			250											//比赛结束


//////////////////////////////////////////////////////////////////////////////////////////

//定时赛
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,public IServerUserItemSink
{
	//状态变量
protected:
	BYTE								m_MatchStatus;					//比赛状态	
	CTime								m_TimeLastMatch;				//比赛时间

	//变量定义
protected:
	CDistributeManager					m_DistributeManage;             //分组管理

	//比赛配置
protected:
	tagGameMatchOption *				m_pMatchOption;					//比赛配置	
	tagLockTimeMatch *					m_pLockTimeMatch;				//比赛规则
	tagGameServiceOption *				m_pGameServiceOption;			//服务配置
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//服务属性

	//内核接口
protected:
	ITableFrame	**						m_ppITableFrame;				//框架接口
	ITimerEngine *						m_pITimerEngine;				//时间引擎
	IDBCorrespondManager *				m_pIDataBaseEngine;				//数据引擎
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//网络引擎

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口
	IServerUserManager *				m_pIServerUserManager;			//用户管理
	IAndroidUserManager	*				m_pAndroidUserManager;			//机器管理
	IServerUserItemSink *				m_pIServerUserItemSink;			//用户回调

	//函数定义
public:
	//构造函数
	CLockTimeMatch();
	//析构函数
	virtual ~CLockTimeMatch(void);

	//基础接口
public:
 	//释放对象
 	virtual VOID Release(){ delete this; }
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动通知
	virtual void OnStartService();

	//管理接口
public:
	//绑定桌子
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//初始化接口
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID);

	//网络事件
public:
	//比赛事件
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//用户接口
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//用户数据
	virtual bool OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind);

	//事件接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//登录完成
	virtual bool OnEventUserLogonFinish(IServerUserItem * pIServerUserItem);
	//进入事件
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//用户参赛
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//用户退赛
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //功能函数
public:
	 //游戏开始
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //游戏结束
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	 //用户事件
public:
	 //玩家返赛
	 virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem);

	 //用户事件
public:
	 //用户坐下
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户起来
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //用户同意
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//清除玩家
	void ClearSameTableUser(DWORD dwUserID);		
	//插入用户
	void InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID);		
	//移除分组
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//插入用户
	bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//发送分数
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//发送分数
	bool SendMatchUserInitScore(IServerUserItem * pIServerUserItem);

	//提示函数
protected:
	//保险提示
	bool SendSafeCardNotify(IServerUserItem * pIServerUserItem);
	//扣费提示
	bool SendDeductFeeNotify(IServerUserItem * pIServerUserItem);	
	//淘汰提示
	bool SendEliminateNotify(IServerUserItem * pIServerUserItem);
	//报名成功
	bool SendSignupSuccessNotify(IServerUserItem * pIServerUserItem);	
	//复活提示
	bool SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard);

	//校验函数
private:
	//参赛校验
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//时间校验
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		

	//辅助函数
protected:	
	//计算场次
	LONGLONG CalcMatchNo();
	//分配用户
	bool PerformDistribute();
	//更新状态
	VOID UpdateMatchStatus();
	//切换时钟
	VOID SwitchMatchTimer();
	//截止报名
	bool IsMatchSignupEnd();
	//开始报名
	bool IsMatchSignupStart();	
	//开始间隔
	DWORD GetMatchStartInterval();	
	//執行起立
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);
	//发送比赛信息
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//写入奖励
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);
};

#endif