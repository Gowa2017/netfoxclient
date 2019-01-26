#ifndef IMMEDIATE_GROUP_HEAD_FILE
#define IMMEDIATE_GROUP_HEAD_FILE

#pragma once

//包含文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//比赛状态
enum emMatchStatus
{
	MatchStatus_Null=0,										//无状态
	MatchStatus_Signup,										//报名中
	MatchStatus_Wait,										//等待桌子
	MatchStatus_Playing										//比赛进行中
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义
#define IDI_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+1)					//删除结束的比赛组
#define IDI_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+2)					//检查一场比赛 
#define IDI_GROUP_TIMER_START		(IDI_MATCH_MODULE_START+10)					//比赛组内的定时器

//时钟定义
#define IDI_CHECK_MATCH				(IDI_GROUP_TIMER_START+1)					//巡查比赛 
#define IDI_WAIT_TABLE				(IDI_GROUP_TIMER_START+2)					//等待配桌 
#define IDI_SWITCH_TABLE			(IDI_GROUP_TIMER_START+4)					//换桌等待 
#define IDI_MATCH_FINISH			(IDI_GROUP_TIMER_START+6)					//比赛完成

//时钟定义
#define IDI_CONTINUE_GAME			1											//继续游戏
#define IDI_LEAVE_TABLE				2											//离开桌子
#define IDI_CHECK_TABLE_START		3											//检查分配桌子是否开始(解决有可能准备后不开始情况)

///////////////////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//比赛定时器
struct tagMatchTimer
{
	DWORD					dwTimerID;						//定时器ID
	int						iElapse;						//执行间隔秒	
	WPARAM					wParam;							//附加参数
	LPARAM					lParam;							//附加参数
};

//分数单项
struct tagMatchScore
{
	DWORD					dwUserID;						//用户I D
	LONGLONG				lScore;							//用户成绩	
	IServerUserItem *		pUserItem;						//用户接口
};

//排名单项
struct tagMatchRanking
{
	DWORD					dwUserID;						//用户I D
	LONGLONG				lScore;							//用户分数
	LONG					lExpand;						//扩展值
	IServerUserItem *		pUserItem;						//用户接口
};

//比赛桌详情
struct tagTableFrameInfo 
{
	ITableFrame		*		pTableFrame;					//桌子接口
	WORD					wTableID;						//桌子编号
	LONGLONG				lBaseScore;						//基础分数	
	WORD					wPlayCount;						//游戏局数
	bool					bMatchTaskFinish;				//完成标识
	bool					bSwtichTaskFinish;				//换桌标识
	bool					bWaitForNextInnings;			//等待下局
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//类型定义

//数组定义
typedef CWHArray<tagMatchTimer *>	CUserMatchTimerArray;
typedef CWHArray<tagMatchScore *>	CUserMatchScoreArray;
typedef CWHArray<IServerUserItem *>	CMatchUserItemArray;
typedef CWHArray<tagTableFrameInfo *>	CTableFrameMananerArray;

//字典定义
typedef CMap<DWORD,DWORD,IServerUserItem *, IServerUserItem *> CUserItemMap;
typedef CMap<IServerUserItem *, IServerUserItem *, DWORD, DWORD> CUserSeatMap;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//接口定义

//类型声明
class CImmediateGroup;

//分组回调
interface IImmediateGroupSink
{
	//比赛事件
public:
	//开始比赛
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch)=NULL;
	//比赛结束
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch)=NULL;

	//定时器接口
public:
	//删除定时器
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)=NULL;
	//设置定时器
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)=NULL;	

	//发送函数
public:
	//发送数据
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem)=NULL;
	//发送信息
	virtual bool SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch)=NULL;
	//发送分数
	virtual bool SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)=NULL;
	//发送状态
	virtual bool SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)=NULL;	
	//发送消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送数据
	virtual bool SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch)=NULL;
	//发送数据
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	

	//功能函数
public:
	//插入空的桌子
	virtual void InsertNullTable()=NULL;
	//获取机器人
	virtual IAndroidUserItem * GetFreeAndroidUserItem()=NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

//比赛分组
class CImmediateGroup: public IMatchEventSink
{
	friend class CImmediateMatch;

	//状态信息
protected:	
	SCORE							m_lCurBase;						//当前基数
	LONGLONG						m_lMatchNo;						//比赛编号
	BYTE							m_cbMatchStatus;				//比赛阶段
	BYTE							m_LoopTimer;					//循环定时器
	WORD							m_wAndroidUserCount;			//机器人个数
	SYSTEMTIME						m_MatchStartTime;				//比赛开始

    //指针变量
protected:
	IImmediateGroupSink *			m_pMatchSink;					//管理回调
	tagGameMatchOption *			m_pMatchOption;					//比赛配置
	tagImmediateMatch *				m_pImmediateMatch;				//比赛规则	

	//存储变量
protected:
	CUserItemMap					m_OnMatchUserMap;				//参赛用户
	CMatchUserItemArray				m_FreeUserArray;				//空闲用户
	CTableFrameMananerArray			m_MatchTableArray;				//预定比赛桌子
	CUserMatchTimerArray			m_MatchTimerArray;				//比赛专用定时器	

	//静态变量
protected:
	static WORD						m_wChairCount;					//椅子数目
	
	//函数定义
public:
	//构造函数
	CImmediateGroup(LONGLONG lMatchNo,tagGameMatchOption * pGameMatchOption,IImmediateGroupSink *pIImmediateGroupSink);
	//析构函数
	virtual ~CImmediateGroup();

	 //基础接口
public:
 	//释放对象
 	virtual VOID Release(){};
 	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

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

 	//用户事件
public:
	//用户报名
	bool OnUserSignUpMatch(IServerUserItem *pUserItem);
	//用户退赛
	bool OnUserQuitMatch(IServerUserItem *pUserItem);

	//辅助函数
protected:
	//插入分组
	VOID InsertFreeGroup(IServerUserItem * pUserServerItem);
	//时钟消息
	bool OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter);	
	//加入比赛桌子
	VOID AddMatchTable(tagTableFrameInfo* pTableFrameInfo){ m_MatchTableArray.Add(pTableFrameInfo);} 		

	//辅助函数
protected:
	//比赛结束
	VOID MatchOver();
	//效验比赛开始
	VOID EfficacyStartMatch();
	//继续游戏
	VOID ContinueGame(ITableFrame * pITableFrame);
	//玩家排序
	WORD SortMapUser(tagMatchScore dwDrawCount[]);	
	//获取接口
	tagTableFrameInfo * GetTableInterface(ITableFrame * pITableFrame);
	//用户名次
	WORD GetUserRank(IServerUserItem *pUserServerItem,ITableFrame * pITableFrame=NULL);	

	//消息函数
protected:
	//分组消息
	VOID SendGroupMessage(LPCTSTR pStrMessage);		
	//发送信息
	VOID SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//提示消息
	VOID SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID=INVALID_CHAIR);

	//辅助函数
protected:
	//分配桌子
	VOID PerformDistributeTable();
	//用户起立
	VOID PerformAllUserStandUp();
	//用户起立
	VOID PerformAllUserStandUp(ITableFrame *pITableFrame);

	//检测函数
protected:	
	//检测人数
	bool CheckMatchUser();	
	//检测结束
	bool CheckMatchTaskFinish();
	//检测标记
	bool CheckSwitchTaskFinish();
	//检测桌子
	VOID CheckTableStart(ITableFrame * pITableFrame);

	//定时器函数
protected:
	//捕获定时器
	VOID CaptureMatchTimer();
	//杀死定时器
	VOID AllKillMatchTimer();
	//杀死定时器
	bool KillMatchTimer(INT_PTR dwIndexID);
	//杀死定时器
	VOID KillMatchGameTimer(DWORD dwTimerID);	
	//杀死定时器
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//投递定时器
	VOID PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam=0, LPARAM lParam=0);		
	//设定定时器
	VOID SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);		
};

#endif