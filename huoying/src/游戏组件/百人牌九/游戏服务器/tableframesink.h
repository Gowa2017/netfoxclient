#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////
//历史记录
#define MAX_SCORE_HISTORY		   16             //历史个数
#define MAX_CARDRECORD_HISTORY     3             //历史开牌个数
//////////////////////////////////////////////////////////////////////////


//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
  //用户信息
protected:
  SCORE            m_lUserStartScore[GAME_PLAYER];   //起始分数

  //总下注数
protected:
  SCORE            m_lAllJettonScore[AREA_COUNT+1];  //全体总注

  //个人下注
protected:
  SCORE            m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//个人总注

  //控制变量
protected:
  bool              m_bRefreshCfg;					//每盘刷新
  TCHAR             m_szRoomName[32];				//配置房间
  TCHAR             m_szConfigFileName[MAX_PATH];   //配置文件
  SCORE          m_lAreaLimitScore;              //区域限制
  SCORE          m_lUserLimitScore;              //区域限制
  INT               m_nEndGameMul;					//提前开牌百分比
  
  TCHAR             m_szControlRoomName[32];				 //配置房间
  TCHAR             m_szControlConfigFileName[MAX_PATH];     //配置文件

  //坐庄控制
  SCORE           m_lApplyBankerCondition;        //申请条件
  int                m_nBankerTimeLimit;             //次数限制
  int                m_nBankerTimeAdd;               //增加次数 (金币大于其他玩家时)
  SCORE           m_lExtraBankerScore;            //额外条件 (大于此值时可以无视条件额外坐庄)
  int                m_nExtraBankerTime;             //额外次数

  //库存控制
  SCORE            m_lStorageStart;            //库存数值
  SCORE            m_lStorageCurrent;          //当前库存数值
  int				  m_nStorageDeduct;           //扣取比例
 
  SCORE            m_lStorageMax1;             //库存封顶1
  LONGLONG            m_lStorageMul1;             //系统输钱概率
  SCORE            m_lStorageMax2;             //库存封顶2
  LONGLONG            m_lStorageMul2;             //系统输钱概率

  bool                m_bControl;                 //是否控制
  TCHAR               m_szControlName[LEN_NICKNAME];      //房间名称
  
  int				 m_IsOpen;					//--MXM
  SCORE				 m_lAttenuationScore;		//衰减值--MXM
  SCORE				 m_lBottomPourImpose;		//下注限制--MXM

  //时间控制
  int               m_nFreeTime;				//空闲时间
  int               m_nPlaceJettonTime;         //下注时间
  int               m_nGameEndTime;             //结束时间

  //机器人控制
  int               m_nMaxChipRobot;				//最大数目 (下注机器人)
  int               m_nChipRobotCount;				//人数统计 (下注机器人)
  SCORE            m_lRobotAreaLimit;            //区域统计 (机器人)
  SCORE            m_lRobotAreaScore[AREA_COUNT+1];    //区域统计 (机器人)
  int               m_nRobotListMaxCount;         //最多人数

 

  //玩家成绩
protected:
  SCORE            m_lUserWinScore[GAME_PLAYER];     //玩家成绩
  SCORE            m_lUserReturnScore[GAME_PLAYER];    //返回下注
  SCORE            m_lUserRevenue[GAME_PLAYER];      //玩家税收
  BYTE              m_cbLeftCardCount;            //扑克数目
  bool              m_bContiueCard;             //继续发牌
  BYTE              m_bcFirstPostCard;            //最新發牌點數
  BYTE				m_bcNextPostCard;

  //扑克信息
protected:
  BYTE              m_cbCardCount[4];           //扑克数目
  BYTE              m_cbTableCardArray[4][2];       //桌面扑克
  BYTE              m_cbTableCard[CARD_COUNT];        //桌面扑克

  //状态变量
protected:
  DWORD             m_dwJettonTime;             //开始时间
  bool              m_bExchangeBanker;            //变换庄家
  WORD              m_wAddTime;               //额外坐庄

  //庄家信息
protected:
  CWHArray<WORD>          m_ApplyUserArray;           //申请玩家
  WORD              m_wCurrentBanker;           //当前庄家
  WORD              m_wBankerTime;              //做庄次数
  SCORE            m_lBankerScore;
  SCORE            m_lBankerWinScore;            //累计成绩
  SCORE            m_lBankerCurGameScore;          //当前成绩
  bool              m_bEnableSysBanker;           //系统做庄
  WORD              m_wCardRecordCount;           //开牌轮数

  //控制变量
protected:
  BYTE              m_cbWinSideControl;           //控制输赢
  int               m_nSendCardCount;           //发送次数

  //记录变量
protected:
  tagServerGameRecord       m_GameRecordArrary[MAX_SCORE_HISTORY];		 //游戏记录
  tagServerCardRecord       m_CardRecordArrary[MAX_CARDRECORD_HISTORY];  //开牌记录
  int               m_nRecordFirst;             //开始记录
  int               m_nRecordLast;              //最后记录
  CFile             m_FileRecord;               //记录结果
  DWORD             m_dwRecordCount;            //记录数目

  int               m_nCardRecordFirst;             //开始记录
  int               m_nCardRecordLast;              //最后记录
  CFile             m_FileCardRecord;               //记录结果
  DWORD             m_dwCardRecordCount;			//记录数目

  //组件变量
protected:
  CGameLogic            m_GameLogic;              //游戏逻辑
  ITableFrame           * m_pITableFrame;           //框架接口
//  ITableFrameControl        * m_pITableFrameControl;        //框架接口
  const tagGameServiceOption    * m_pGameServiceOption;         //配置参数

  //服务控制
protected:
  HINSTANCE           m_hInst;
  IServerControl*         m_pServerContro;

  //属性变量
protected:
  static const WORD       m_wPlayerCount;             //游戏人数

//--MXM
protected:											//占位配置
	WORD				  m_wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	tagOccUpYesAtconfig   m_OccUpYesAtconfig;

	CWHArray<DWORD>					m_PeizhiUserArray;						//配置玩家
	bool							m_bAndroidXiahzuang;					//机器人下庄

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
  //接口查询
  virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

  //管理接口
public:
  //初始化
  virtual bool Initialization(IUnknownEx * pIUnknownEx);
  //复位桌子
  virtual VOID RepositionSink();

  //查询接口
public:
  //查询限额
  virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
  //最少积分
  virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
  {
    return 0;
  }
  //数据事件
  virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
  {
    return false;
  }
  //积分事件
  virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
  //信息接口
public:
  //游戏状态
  virtual bool IsUserPlaying(WORD wChairID);

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
  //定时器事件
  virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
  //游戏消息处理
  virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
  //框架消息处理
  virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
  //数据事件
  virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);
  //查询是否扣服务费
  virtual bool QueryBuckleServiceCharge(WORD wChairID);
  //设置基数
  virtual void SetGameBaseScore(SCORE lBaseScore) {};

  //动作事件
public:
  //用户断线
  virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
  //用户重入
  virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem)
  {
    return true;
  }
  //用户坐下
  virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户起来
  virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户同意
  virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
  {
    return true;
  }

  //游戏事件
protected:
  //加注事件
  bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore);
  //申请庄家
  bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
  //取消申请
  bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

  //辅助函数
private:
  //发送扑克
  bool DispatchTableCard();
  //发送庄家
  void SendApplyUser(IServerUserItem *pServerUserItem);
  //更换庄家
  bool ChangeBanker(bool bCancelCurrentBanker);
  //轮换判断
  void TakeTurns();
  //发送记录
  void SendGameRecord(IServerUserItem *pIServerUserItem);
  //发送开牌记录
  void SendCardRecord(IServerUserItem *pIServerUserItem);
  //发送消息
  void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
  //管理员命令
  bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);
  //读取配置
  void ReadConfigInformation();
  //混乱扑克--MXM
  void RandCount(BYTE bcSaizi[2],BYTE cbCardBuffer[], BYTE cbBufferCount,BYTE bcSaiziCount);
  //是否衰减
  bool NeedDeductStorage();
  //发送下注信息
  void SendUserBetInfo(IServerUserItem *pIServerUserItem);
  //发送配置玩家--MXM
  void  SendPeizhiUser( IServerUserItem *pRcvServerUserItem );

  //输赢处理
protected:
  // 库存优化
  VOID StorageOptimize();
  // 判断输赢
  void JudgeSystemScore(BYTE bCardData[4][2], SCORE& lSystemScore, bool& bAllEat);

  //下注计算
private:
  //最大下注
  SCORE GetUserMaxJetton(WORD wChairID, BYTE cbArea);

  //游戏统计
private:
  //计算得分
  SCORE CalculateScore();
  //预先计算得分
  SCORE CalculateScoreEx();
  //推断赢家
  void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);
  //开牌记录轮数跟牌值
  void OpenCardRecordValue(BYTE &m_card1, BYTE &m_card2, BYTE &m_card3, BYTE &m_card4, BYTE &m_card5, BYTE &m_card6, BYTE &m_card7, BYTE &m_card8);
  // 记录函数
  void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////

#endif
