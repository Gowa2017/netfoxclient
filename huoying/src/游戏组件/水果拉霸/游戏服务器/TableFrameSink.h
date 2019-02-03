#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "ServerControl.h"
#include "ServerControlItemSink.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	bool							m_bGameStart;						//开始游戏
	BYTE							m_cbGameMode;						//游戏模式
	BYTE							m_cbTwoMode;						//下注模式
	LONGLONG						m_cbLine;							//押线数量
	LONGLONG						m_lBet;								//押线下注
	LONGLONG						m_lBetScore;						//押线下注
	LONGLONG						m_lOneGameScore;					//押线得分
	LONGLONG						m_lTwoGameScore;					//比倍得分	
	LONGLONG						m_lThreeGameScore;					//玛丽得分
	BYTE							m_cbCurBounsGameCount;				//小玛丽次数
	BYTE							m_cbBounsGameCount;					//小玛丽次数
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//开奖信息
	LONGLONG						m_cbALLSmallSorce;							//小游戏得分
	BYTE							m_cbCustoms;								//小游戏关卡
	DWORD							SamllGameFruit[4][5];						//小游戏数据
	LONGLONG						m_Bonus;									//彩金池
	CTime							m_tCreateTime;								//小游戏开始时间
	BYTE							m_BonusCount;								//中奖池次数
	LONGLONG						m_BonusGetSorce;							//中奖得分
	LONGLONG						m_BonusSorce[10];							//中奖池记录
	SYSTEMTIME						m_BonusSorceDate[10];						//中奖池时间
	TCHAR							m_NickName[10][LEN_NICKNAME];				//玩家昵称

	//小游戏每关分数
	DWORD  FirstSorce[5];
	DWORD  SecondSorce[5];
	DWORD  ThirdSorce[5];
	DWORD  FourthSorce[5];

	//库存变量
protected:
	static CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> m_MapRoomUserInfo;	//玩家USERID映射玩家信息
	static CList<CString, CString&> m_ListOperationRecord;						//操作控制记录

	//全局库存控制变量
	static int						m_IsOpen;								//起始桌子库存
	static LONGLONG					m_lStorageStartTable;					//起始桌子库存
	static LONGLONG					m_lStorageInputTable;					//总投入库存
	static LONGLONG					m_lCurStorage;							//当前库存
	static LONGLONG					m_lRoomStockRecoverScore;				//回收赢分
	static LONGLONG					m_lStorageDeductRoom;					//全局库存衰减
	static LONGLONG					m_lStorageMax1Room;						//全局库存封顶
	static LONGLONG					m_lStorageMul1Room;						//全局系统输钱比例
	static LONGLONG					m_lStorageMax2Room;						//全局库存封顶
	static LONGLONG					m_lStorageMul2Room;						//全局系统输钱比例
	static WORD						m_wGameTwo;								//比倍概率
	static WORD						m_wGameTwoDeduct;						//比倍概率
	static WORD						m_wGameThree;							//小玛丽概率
	static WORD						m_wGameThreeDeduct;						//小玛丽概率
	LONGLONG						m_lStorageMulRoom;						//系统输钱比例
	WORD							m_wSingleGameTwo;						//比倍概率
	WORD							m_wSingleGameThree;						//小玛丽概率
//控制变量
	
	TCHAR							m_szControlRoomName[32];				//配置房间
	TCHAR							m_szControlConfigFileName[MAX_PATH];	//配置文件

	//服务控制
protected:
	HINSTANCE						m_hInst;								//控制句柄
	CServerControlItemSink*			m_pServerControl;						//控制组件

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CHistoryScore					m_HistoryScore;						//历史成绩


	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
	tagCustomRule *					m_pGameCustomRule;					//自定规则
	tagGameServiceOption *			m_pGameServiceOption;				//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//游戏属性

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//查询接口
public:
	//查询限额
	virtual DOUBLE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual DOUBLE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(DOUBLE lBaseScore);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);


	//游戏事件
protected:
	//押线开始
	bool OnUserOneStart(IServerUserItem * pIServerUserItem, LONGLONG lBet, LONGLONG lBetCount);
	//是否进入小游戏
	bool EntranceSmallGame(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT]);
	///三个bonus触发小游戏
	bool  CTableFrameSink::OnUserSmallGameStartBefore();
	//小游戏
	bool OnUserSmallGameStart(IServerUserItem * pIServerUserItem, BYTE cbCustoms,LONGLONG	SmallSorce);
	//游戏结束
	bool OnUserGameEnd(IServerUserItem * pIServerUserItem);
	//判断是否中奖
	bool DetermineTheWinning(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT]);
	//中奖得分
	LONGLONG WinningSocre(IServerUserItem * pIServerUserItem);
	//返回奖池记录
	bool OnUserWinningRecord(IServerUserItem * pIServerUserItem);
	
	//功能函数
public:
	//写日志文件
	void WriteInfo(LPCTSTR pszString);
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore = 0LL);	
	//读取配置
	void ReadConfigInformation();
	//更新控制
	bool UpdateControl(ROOMUSERINFO &roomUserInfo);
	//获取控制类型
	void GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr);
	//获取控制类型
	CString GetControlDataString(USERCONTROL &UserControl);
	//获取控制类型
	CString GetControlDataString(BYTE cbGameMode);
};

//////////////////////////////////////////////////////////////////////////////////

#endif