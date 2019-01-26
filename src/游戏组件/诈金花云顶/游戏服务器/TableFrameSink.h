#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"
#include "AESEncrypt.h"
#include <map>
using namespace std;
typedef CMap<WORD,WORD,int,int> CMapControlID;
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink :public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	BYTE							m_bOperaCount;							//操作次数
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wFlashUser[GAME_PLAYER];				//动画用户

	//用户状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbGiveUpUser[GAME_PLAYER];			//游戏状态
	SCORE						m_lCompareCount;						//比牌状态
	bool							m_bGameEnd;								//结束状态
	CMD_S_GameEnd					m_StGameEnd;							//结束数据
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//真人玩家
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//机器状态

	//扑克变量
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克
	CWHArray<WORD>					m_wCompardUser[GAME_PLAYER];			//扑克数组
	BYTE							m_cbCardDataForShow[GAME_PLAYER][MAX_COUNT];//桌面扑克

	//下注信息
protected:
	SCORE						m_lTableScore[GAME_PLAYER];				//下注数目
	SCORE						m_lUserMaxScore[GAME_PLAYER];			//最大下注
	SCORE						m_lMaxCellScore;						//单元上限
	SCORE						m_lCellScore;							//单元下注
	SCORE						m_lCurrentTimes;						//当前倍数
	bool							m_bMingZhu[GAME_PLAYER];				//看明下注

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//服务控制
protected:
	HINSTANCE						m_hControlInst;
	IServerControl*					m_pServerControl;

	//AI变量
protected:
	bool							m_bUpdataStorage;						//更新库存


	//属性变量
protected:
	WORD							m_wPlayerCount;							//游戏人数
	static const BYTE				m_GameStartMode;						//开始模式

	//密钥
	unsigned char					m_chUserAESKey[GAME_PLAYER][AESKEY_TOTALCOUNT];	//初始密钥

	//玩家轮次
	map<DWORD,int>					m_RoundsMap;						//玩家椅子，当前轮次
	TCHAR							m_szConfigFileName[MAX_PATH];		//配置文件
	int								m_nCurrentRounds;					//当前轮次
	int								m_nTotalRounds;						//总轮次
	TCHAR							m_szRoomName [32];

	DOUBLE							m_dElapse;							//流逝时间
	CMapControlID					m_MapControlID;						//控制玩家输赢map

	BYTE							m_bTempArray[52];
	bool							m_bAndroidControl;						//控制机器人输赢


	int								m_nZhaDan;					//炸弹
	int								m_nShunJin ;				//顺金
	int								m_nJinHua  ;				///金华
	int								m_nShunZI  ;				//顺子
	int								m_nDuiZi;					//对子
	int								m_nSanPai;					//散牌
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release();
	//是否有效
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void  RepositionSink();
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
		//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(SCORE lBaseScore){};

	//信息接口
public:
	//开始模式
	virtual BYTE GetGameStartMode();
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);


	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//用户下注
	virtual bool OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem){return true;}
	//游戏事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID,bool bExit=false);
	//看牌事件
	bool OnUserLookCard(WORD wChairID);
	//比牌事件
	bool OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID);
	//开牌事件
	bool OnUserOpenCard(WORD wUserID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp, bool bCompareUser, unsigned char	chciphertext[AESENCRYPTION_LENGTH]);
	//亮牌事件
	bool OnUserShowCard(WORD wChairID);
	//自动比牌事件
	bool OnUserAutoCompareCard(WORD wChairID);

	//功能函数
protected:
	//扑克分析
	void AnalyseStartCard();
	//是否衰减
	bool NeedDeductStorage();
	//读取配置
	void ReadConfigInformation();
	//更新房间用户信息
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//更新同桌用户控制
	void UpdateUserControl(IServerUserItem *pIServerUserItem);
	//除重用户控制
	void TravelControlList(ROOMUSERCONTROL keyroomusercontrol);
	//是否满足控制条件
	void IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl);
	//分析房间用户控制
	bool AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList);
	//获取控制类型
	void GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr);
	//写日志文件
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
	//修改库存
	bool UpdateStorage(SCORE lScore);
	//乱序密钥
	void RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount);
	//判断积分约占房间
	bool IsRoomCardScoreType();
	//判断金币约占房间
	bool IsRoomCardTreasureType();
	//试探性判断, 确保Chair的用户可以赢
	DWORD ProbeJudge(WORD wChairIDCtrl,BYTE cbCheatType);
	//推断胜者
	WORD EstimateWinner();
	//获取牌
	bool GetControlCard(BYTE cbControlCardByte[],BYTE cbCardCount);
	//随机牌
	BYTE GetRandCard(BYTE cbRandCard[],DWORD dwChairID,int &cbIndex,int nRandMin,int nRandMax,bool bFistGetCrad);
	bool getSingBomb(BYTE cbCardData[52],int chairID,BYTE cbSingBomb[]);  //单个炸弹
	bool getSingSameStraight(BYTE cbCardData[52],int chairID,BYTE SingSameStraight[]);  //单个同花顺
};

//////////////////////////////////////////////////////////////////////////

#endif
