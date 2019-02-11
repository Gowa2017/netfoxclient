#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include <vector>

class ICGame;

using namespace std;
//////////////////////////////////////////////////////////////////////////////////

//效验类型
enum enEstimatKind
{
	EstimatKind_OutCard,			//出牌效验
	EstimatKind_GangCard,			//杠牌效验
};

//发牌状态
enum enSendStatus
{
	Not_Send = 0,					//无
	OutCard_Send,					//出牌后发牌
	Gang_Send,						//杠牌后发牌
	BuHua_Send,						//补花后发牌
};


//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//模式变量
protected:
	int								diFenUnit;							//私人场底分
	BYTE							m_cbMaCount;						//码数 1:一码全中;2-6:对应的码数
	BYTE							m_cbPlayerCount;					//指定游戏人数，2-4
	BYTE							m_cbJiePao;							//可接炮
	BYTE							m_cbQiangGang;						//可抢杠
	BYTE							m_cbQiXiaoDui;						//可胡七小对
	CMD_S_RECORD		m_stRecord;

	bool							m_TableEnded;
	int								m_PlayCount;

	CString strInfo;
	//游戏变量
protected:
	WORD						m_wSiceCount;									//骰子点数
	WORD						m_wBankerUser;									//庄家用户
	bool							m_bTing[GAME_PLAYER];							//是否听牌
	bool							m_bTrustee[GAME_PLAYER];						//是否托管
	BYTE                        m_cbMagicIndex;									 //财神索引
	bool							m_bPlayStatus[GAME_PLAYER];				//是否参与游戏
	//属性变量
public:
	WORD						m_wPlayerCount;									//游戏人数
	BYTE							m_cbWaitTime;									//等待时间

	//堆立变量
protected:
	WORD						m_wHeapHead;									//堆立头部
	WORD						m_wHeapTail;									//堆立尾部
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];				//堆牌信息

	//运行变量
protected:
	WORD						m_wResumeUser;									//还原用户
	WORD						m_wCurrentUser;									//当前用户
	WORD						m_wProvideUser;									//供应用户
	BYTE							m_cbProvideCard;								//供应扑克
	WORD						m_wLastCatchCardUser;						//最后一个摸牌的用户
	bool							m_bUserActionDone;
	//状态变量
protected:
	bool							m_bGangOutCard;									//杠后出牌
	enSendStatus				m_enSendStatus;									//发牌状态
	BYTE							m_cbGangStatus;									//杠牌状态
	WORD						m_wProvideGangUser;								//供杠用户
	bool							m_bEnjoinChiHu[GAME_PLAYER];					//禁止吃胡
	bool							m_bEnjoinChiPeng[GAME_PLAYER];					//禁止吃碰
	bool							m_bEnjoinGang[GAME_PLAYER];						//禁止杠牌
	bool							m_bGangCard[GAME_PLAYER];						//杠牌状态
	WORD						m_wLianZhuang;									//连庄次数
	BYTE							m_cbGangCount[GAME_PLAYER];						//杠牌次数	
	BYTE							m_cbChiPengCount[GAME_PLAYER];		//吃碰杠次数	
	vector<BYTE>			m_vecEnjoinChiHu[GAME_PLAYER];					//禁止吃胡
	vector<BYTE>			m_vecEnjoinChiPeng[GAME_PLAYER];				//禁止吃碰
	BYTE							m_cbUserCatchCardCount[GAME_PLAYER];		//玩家抓牌数量
	BYTE							m_cbHuCardCount[GAME_PLAYER];//胡牌个数
	BYTE							m_cbHuCardData[GAME_PLAYER][MAX_INDEX];//胡牌数据
	BYTE							m_cbUserMaCount[GAME_PLAYER];					
	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];						//响应标志
	BYTE							m_cbUserAction[GAME_PLAYER];					//用户动作
	BYTE							m_cbOperateCard[GAME_PLAYER][3];				//操作扑克
	BYTE							m_cbPerformAction[GAME_PLAYER];					//执行动作
	SCORE						m_lUserGangScore[GAME_PLAYER];					//游戏中杠的输赢
	//结束信息
protected:
	BYTE							m_cbChiHuCard;									//吃胡扑克
	DWORD						m_dwChiHuKind[GAME_PLAYER];						//吃胡结果
	CChiHuRight				m_ChiHuRight[GAME_PLAYER];						//吃胡权位

	//出牌信息
protected:
	WORD						m_wOutCardUser;									//出牌用户
	BYTE							m_cbOutCardData;								//出牌扑克
	BYTE							m_cbOutCardCount;								//出牌数目
	BYTE							m_cbDiscardCount[GAME_PLAYER];					//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];				//丢弃记录

	//发牌信息
protected:
	BYTE							m_cbSendCardData;								//发牌扑克
	BYTE							m_cbSendCardCount;								//发牌数目	
public:
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];				//库存扑克
protected:
	BYTE							m_cbEndLeftCount;								//荒庄牌数
	BYTE							m_cbMinusHeadCount;								//头部空缺
	BYTE							m_cbMinusLastCount;								//尾部空缺
	BYTE							m_cbLeftCardCount;								//剩余数目

	//用户扑克
protected:
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];			//用户扑克
	BYTE							m_cbHandCardCount[GAME_PLAYER];					//扑克数目
	
	//组合扑克
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];				//组合数目
	tagWeaveItem			m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合扑克

	//组件变量
protected:
	CGameLogic				m_GameLogic;									//游戏逻辑
	CHistoryScore				m_HistoryScore;									//历史成绩

	HINSTANCE           m_hC;
	ICGame*         m_pC;

#ifdef  CARD_DISPATCHER_CONTROL

	//控制变量
protected:
	BYTE                        m_cbControlGameCount;							//控制局数
	WORD						m_wControBankerUser;							//控制庄家
	BYTE							m_cbControlRepertoryCard[MAX_REPERTORY];		//控制库存

#endif
	//组件接口
protected:
	ITableFrame	*						m_pITableFrame;									//框架接口
	tagCustomRule *					m_pGameCustomRule;							//自定规则
	tagGameServiceOption *			m_pGameServiceOption;							//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;							//游戏属性

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
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//查询是否动态加入玩家
	virtual bool GetIsDynamicJoinUser(IServerUserItem * pIServerUserItem){ return false; }
	//游戏事件
public:
	void  PrintCards();
	//游戏开始
	virtual bool OnEventGameStart();
	virtual void OnPrivateTableCreated();
	virtual void OnPrivateTableEnded(int flag);
	//游戏结束
	void SetLeftCards(CMD_S_GameConclude &conclude);
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
	virtual void SetGameBaseScore(LONG lBaseScore);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut=false);
	//用户操作
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3]);
	//用户听牌
	bool OnUserListenCard(WORD wChairID, bool bListenCard);
	//用户托管
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//用户补牌
	bool OnUserReplaceCard(WORD wChairID, BYTE cbCardData);
	//发送扑克
	bool OnUserSendCard(BYTE cbCardCount, WORD wBankerUser, BYTE cbCardData[], BYTE cbControlGameCount);

	//辅助函数
protected:
	//辅助函数
protected:
	//发送操作
	bool SendOperateNotify();
	//取得扑克
	BYTE GetSendCard(bool bTail = false);
	//派发扑克
	bool DispatchCardData(WORD wSendCardUser,bool bTail = false);
	//响应判断
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//算胡牌分
	void CalHuPaiScore(LONGLONG lEndScore[GAME_PLAYER]);
	//算杠分
	void CalGangScore();
	//权位过滤
	void FiltrateRight(WORD wWinner, CChiHuRight &chr);
	//计算倍数
	BYTE GetTimes(WORD wChairId);

	BYTE GetRemainingCount(WORD wChairID,BYTE cbCardData);
	
	void addGameRecord(WORD wChairId, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

#endif