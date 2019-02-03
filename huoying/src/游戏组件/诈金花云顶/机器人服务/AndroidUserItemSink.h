#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\游戏服务器\GameLogic.h"

//////////////////////////////////////////////////////////////////////////



//游戏对话框
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//游戏变量
protected:
	SCORE						m_lStockScore;							//当前库存
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_cbCardType;							//用户牌型
	int								m_nCurrentRounds;						//当前轮次
	int								m_nTotalRounds;							//总轮数

	//加注信息
protected:
	bool							m_bMingZhu;								//看牌动作
	SCORE						m_lCellScore;							//单元下注
	SCORE						m_lMaxCellScore;						//最大下注
	SCORE						m_lUserMaxScore;						//最大分数
	SCORE						m_lCurrentTimes;						//当前倍数
	SCORE						m_lTableScore[GAME_PLAYER];				//下注数目
	unsigned char					m_chUserAESKey[AESKEY_TOTALCOUNT];		//初始密钥
	TCHAR							m_szServerName[LEN_SERVER];				//房间名称

	//用户状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//真人玩家
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//机器玩家

	//用户扑克
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];			//用户数据
	BYTE							m_cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克

	//控件变量
public:
	CGameLogic						m_GameLogic;							//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口

	//机器人存取款
	SCORE						m_lRobotScoreRange[2];					//最大范围
	SCORE						m_lRobotBankGetScore;					//提款数额
	SCORE						m_lRobotBankGetScoreBanker;				//提款数额 (庄家)
	int								m_nRobotBankStorageMul;					//存款倍数
	bool							m_bAndroidControl;						//控制机器人输赢
	bool                            m_bGiveUp;                              //机器人弃牌控制
	WORD							m_wAndroidPour;							//机器人已下注轮数
	WORD							m_wAndroidPourNumber;					//机器人下注轮数
	WORD							m_dwCompareState;						//比牌状态

	TCHAR							m_szConfigFileName[MAX_PATH];		//配置文件

	int								m_nShunZiBetMin;		//顺子最小下注开牌
	int								m_nShunZiBetMax;		//顺子最大下注开牌
	int								m_nJInHuaBetMin;		//金花最小下注开牌
	int								m_nJInHuaBetMax;		//金花最大下注开牌
	int								m_nShunJinBetMin ;		//顺金最小下注开牌
	int								m_nShunJinBetMax;		//顺金最大下注开牌
	int								m_nZhaDanBetMin;		//炸弹最小下注开牌
	int								m_nZhaDanBetMax;		//炸弹最大下注开牌

	int								m_nSanPaiProbability;	//顺子开牌概率
	int								m_nDuiziProbability;	//金花开牌概率
	int								m_nShunZiProbability;	//顺子开牌概率
	int								m_nJInHuaProbability;	//金花开牌概率
	int								m_nShunJinProbability;	//顺金开牌概率
	int								m_nZhaDanProbability;	//炸弹开牌概率

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//机器人消息
	bool OnSubAndroidCard(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户看牌
	bool OnSubLookCard(const void * pBuffer, WORD wDataSize);
	//用户比牌
	bool OnSubCompareCard(const void * pBuffer, WORD wDataSize);
	//用户开牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//更新密钥
	bool OnSubUpdateAESKey(const void * pBuffer, WORD wDataSize);

	//库存操作
private:
	//读取配置
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//读取配置
	void ReadConfigInformation2();
	//银行操作
	void BankOperate(BYTE cbType);

	//写日志文件
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

	//逻辑辅助
protected:
	//推断胜者
	WORD EstimateWinner();
	//加注倍数
	LONGLONG GetAddScoreTimes();
	bool ControlBottompour();
	//是否开牌
	BYTE IsOpenBrand();
};

//////////////////////////////////////////////////////////////////////////

#endif
