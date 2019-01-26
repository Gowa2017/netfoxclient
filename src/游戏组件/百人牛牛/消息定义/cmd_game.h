#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

#define UR_GAME_CONTROL         0x20000000L       //游戏特殊控制
//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID           104                 //游戏 I D
#define GAME_NAME         TEXT("百人牛牛")          //游戏名字

//组件属性
#define GAME_PLAYER         MAX_CHAIR                 //游戏人数
#define VERSION_SERVER        PROCESS_VERSION(7,0,1)        //程序版本
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE       GAME_STATUS_FREE          //等待开始
#define GAME_SCENE_PLACE_JETTON   GAME_STATUS_PLAY          //下注状态
#define GAME_SCENE_GAME_END     GAME_STATUS_PLAY+1          //结束状态


//区域索引
#define ID_TIAN_MEN         1                 //天
#define ID_DI_MEN         2                 //地
#define ID_XUAN_MEN         3                 //玄
#define ID_HUANG_MEN        4                 //黄

//玩家索引
#define BANKER_INDEX        0                 //庄家索引
#define SHUN_MEN_INDEX        1                 //顺门索引
#define DUI_MEN_INDEX       2                 //对门索引
#define DAO_MEN_INDEX       3                 //倒门索引
#define HUAN_MEN_INDEX        4                 //倒门索引
#define MAX_INDEX         3                 //最大索引

#define AREA_COUNT          4                 //区域数目
#define CONTROL_AREA        4

//赔率定义
#define RATE_TWO_PAIR       12                  //对子赔率
#define SERVER_LEN          32                  //房间长度
#define MAX_OCCUPY_SEAT_COUNT       6                                 //最大占位个数
////////////////////////////////////////////////////////////////////////////////////
//宏定义
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#endif
//占位配置
#define OCCUPYSEAT_VIPTYPE  0          //会员占位
#define OCCUPYSEAT_CONSUMETYPE  1      //消耗金币占位
#define OCCUPYSEAT_FREETYPE  2         //免费占位
//配置结构
struct SUPERBANKERCONFIG
{
	int superbankerType;  //抢庄类型
	int enVipIndex;			//vip索引
	SCORE lSuperBankerConsume;//抢庄消耗
};
//占位配置结构
struct tagOccUpYesAtconfig 
{
	//占位类型
	int occupyseatType;
	//vip索引
	int enVipIndex;
	//占位消耗
	LONGLONG lOccupySeatConsume;
	//免费占位金币上限
	SCORE lOccupySeatFree;
	//强制站立条件
	SCORE lForceStandUpCondition;
};
struct tagCustomAndroid
{
  //坐庄
  BOOL              nEnableRobotBanker;       //是否做庄
  LONGLONG            lRobotBankerCountMin;     //坐庄次数
  LONGLONG            lRobotBankerCountMax;     //坐庄次数
  LONGLONG            lRobotListMinCount;       //列表人数
  LONGLONG            lRobotListMaxCount;       //列表人数
  LONGLONG            lRobotApplyBanker;        //最多申请个数
  LONGLONG            lRobotWaitBanker;       //空盘重申

  //下注
  LONGLONG            lRobotMinBetTime;       //下注筹码个数
  LONGLONG            lRobotMaxBetTime;       //下注筹码个数
  SCORE				  lRobotMinJetton;        //下注筹码金额
  SCORE				  lRobotMaxJetton;        //下注筹码金额
  LONGLONG            lRobotBetMinCount;        //下注机器人数
  LONGLONG            lRobotBetMaxCount;        //下注机器人数
  SCORE				  lRobotAreaLimit;        //区域限制

  //存取款
  SCORE		         lRobotScoreMin;         //金币下限
  SCORE				 lRobotScoreMax;         //金币上限
  SCORE              lRobotBankGetMin;       //取款最小值(非庄)
  SCORE              lRobotBankGetMax;       //取款最大值(非庄)
  SCORE              lRobotBankGetBankerMin;     //取款最小值(坐庄)
  SCORE              lRobotBankGetBankerMax;     //取款最大值(坐庄)
  LONGLONG           lRobotBankStoMul;       //存款百分比

  //构造函数
  tagCustomAndroid()
  {
    DefaultCustomRule();
  }

  void DefaultCustomRule()
  {
    nEnableRobotBanker = TRUE;
    lRobotBankerCountMin = 5;
    lRobotBankerCountMax = 10;
    lRobotListMinCount = 2;
    lRobotListMaxCount = 5;
    lRobotApplyBanker = 5;
    lRobotWaitBanker = 3;

    lRobotMinBetTime = 6;
    lRobotMaxBetTime = 8;
    lRobotMinJetton = 1;
    lRobotMaxJetton = 1000;
    lRobotBetMinCount = 4;
    lRobotBetMaxCount = 8;
    lRobotAreaLimit = 10000000;

    lRobotScoreMin = 1000000;
    lRobotScoreMax = 100000000;
    lRobotBankGetMin = 100;
    lRobotBankGetMax = 30000000;
    lRobotBankGetBankerMin = 10000000;
    lRobotBankGetBankerMax = 50000000;
    lRobotBankStoMul = 50;
  }
};

//机器人信息
struct tagRobotInfo
{
  int nChip[7];                           //筹码定义
  int nAreaChance[AREA_COUNT];                    //区域几率
  TCHAR szCfgFileName[MAX_PATH];                    //配置文件
  int nMaxTime;                           //最大赔率

  tagRobotInfo()
  {
    int nTmpChip[7] = {1, 5, 10, 50, 100, 500, 1000};
    int nTmpAreaChance[AREA_COUNT] = {1, 1, 1, 1};

    nMaxTime = 10;
    memcpy(nChip, nTmpChip, sizeof(nChip));
    memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
  }
};

//记录信息
struct tagServerGameRecord
{
  bool              bWinShunMen;            //顺门胜利
  bool              bWinDuiMen;             //对门胜利
  bool              bWinDaoMen;             //倒门胜利
  bool              bWinHuang;              //倒门胜利
};

//下注信息
struct tagUserBet
{
  TCHAR             szNickName[32];           //用户昵称
  DWORD             dwUserGameID;           //用户ID
  SCORE            lUserStartScore;          //用户金币
  SCORE            lUserWinLost;           //用户金币
  SCORE            lUserBet[AREA_COUNT+1];       //用户下注
};

//下注信息数组
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//库存控制
#define RQ_REFRESH_STORAGE    1
#define RQ_SET_STORAGE      2
//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE       99                  //游戏空闲
#define SUB_S_GAME_START      100                 //游戏开始
#define SUB_S_PLACE_JETTON      101                 //用户下注
#define SUB_S_GAME_END        102                 //游戏结束
#define SUB_S_APPLY_BANKER      103                 //申请庄家
#define SUB_S_CHANGE_BANKER     104                 //切换庄家
#define SUB_S_CHANGE_USER_SCORE   105                 //更新积分
#define SUB_S_SEND_RECORD     106                 //游戏记录
#define SUB_S_PLACE_JETTON_FAIL   107                 //下注失败
#define SUB_S_CANCEL_BANKER     108                 //取消申请


#define SUB_S_AMDIN_COMMAND     110                 //管理员命令
#define SUB_S_UPDATE_STORAGE        111                 //更新库存
#define SUB_S_SEND_USER_BET_INFO    112                 //发送下注

#define SUB_S_ADVANCE_OPENCARD		113								//提前开牌
#define SUB_S_SUPERROB_BANKER		114								//超级抢庄
#define SUB_S_CURSUPERROB_LEAVE		115								//超级抢庄玩家离开

#define SUB_S_OCCUPYSEAT            116								//占位
#define SUB_S_OCCUPYSEAT_FAIL       117								//占位失败
#define SUB_S_UPDATE_OCCUPYSEAT     118								//更新占位
#define SUB_S_PEIZHI_USER			119								    //配置玩家
#define SUB_S_DelPeizhi				120									//机器人聊天
#define SUB_S_UPALLLOSEWIN			121									//更新玩家输赢
struct CMD_S_DelPeizhi
	{
	DWORD						dwGameID;
};

struct CMD_S_peizhiVec
{
	DWORD						dwGameID;
	SCORE						lScore;
};
struct CMD_S_UpAlllosewin
{
	DWORD							dwGameID;						//玩家GameID
	SCORE							lTotalScore;					//总输赢分
	SCORE							lTdTotalScore;					//今日输赢分
	SCORE							lScore;							//玩家金币
};
//占位
struct CMD_S_OccupySeat
{
	//申请占位玩家id
	WORD			wOccupySeatChairID;
	//占位索引
	BYTE			cbOccupySeatIndex;
	//占位椅子id
	WORD			tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
};
//更新占位
struct CMD_S_UpdateOccupySeat
{
	//占位椅子id
	WORD tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	//申请退出占位玩家
	WORD wQuitOccupySeatChairID;
};
//占位失败
struct CMD_S_OccupySeat_Fail
{
	//已申请占位玩家ID
	WORD wAlreadyOccupySeatChairID;
	//已占位索引
	BYTE cbAlreadyOccupySeatIndex;
	//占位椅子id
	WORD tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
};

//更新库存
struct CMD_S_UpdateStorage
{
  BYTE                            cbReqType;            //请求类型
  SCORE            lStorageStart;        //起始库存
  SCORE            lStorageDeduct;       //库存衰减
  SCORE            lStorageCurrent;        //当前库存
  SCORE            lStorageMax1;         //库存上限1
  LONGLONG            lStorageMul1;         //系统输分概率1
  SCORE            lStorageMax2;         //库存上限2
  LONGLONG            lStorageMul2;         //系统输分概率2
};

//发送下注
struct CMD_S_SendUserBetInfo
{
  SCORE            lUserStartScore[GAME_PLAYER];       //起始分数
  SCORE            lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//个人总注
};
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3

#define CR_ACCEPT  2      //接受
#define CR_REFUSAL 3      //拒绝
//请求回复
struct CMD_S_CommandResult
{
  BYTE cbAckType;         //回复类型
  BYTE cbResult;
  BYTE cbExtendData[20];      //附加数据
};
//失败结构
struct CMD_S_PlaceJettonFail
{
  WORD              wPlaceUser;             //下注玩家
  BYTE              lJettonArea;            //下注区域
  LONGLONG            lPlaceScore;            //当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
  WORD              wChairID;             //椅子号码
  SCORE              lScore;               //玩家积分

  //庄家信息
  WORD              wCurrentBankerChairID;        //当前庄家
  BYTE              cbBankerTime;           //庄家局数
  SCORE              lCurrentBankerScore;        //庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
  WORD              wApplyUser;             //申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
  WORD              wCancelUser;          //取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
  WORD              wBankerUser;            //当庄玩家
  SCORE            lBankerScore;           //庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
  //全局信息
  BYTE							cbTimeLeave;				//剩余时间

  //玩家信息
  SCORE						lUserMaxScore;				//玩家金币

  //庄家信息
  WORD							wBankerUser;				//当前庄家
  WORD							cbBankerTime;				//庄家局数
  SCORE						lBankerWinScore;			//庄家成绩
  SCORE						lBankerScore;				//庄家分数
  INT							nEndGameMul;				//提前开牌百分比
  bool							bEnableSysBanker;			//系统做庄

  BYTE							cbArea[4];
  BYTE							cbControlStyle;
  bool						    bControlLimits;				//控制权限	
  //控制信息
  SCORE						lApplyBankerCondition;       //申请条件
  SCORE						lAreaLimitScore;			 //区域限制

  //房间信息
  TCHAR							szGameRoomName[SERVER_LEN];     //房间名称
  bool							bGenreEducate;				//练习模式

  SUPERBANKERCONFIG				superbankerConfig; //抢庄配置
  WORD							wCurSuperRobBankerUser;
  int							typeCurrentBanker;
  tagOccUpYesAtconfig   		occupyseatConfig;
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
  SCORE							lBottomPourImpose;    //下注限制
  tagCustomAndroid      		CustomAndroid;            //机器人配置

};

//游戏状态
struct CMD_S_StatusPlay
{
  //全局下注
  SCORE            lAllJettonScore[AREA_COUNT+1];    //全体总注

  //玩家下注
  SCORE            lUserJettonScore[AREA_COUNT+1];   //个人总注

  //玩家积分
  SCORE            lUserMaxScore;            //最大下注

  //控制信息
  SCORE            lApplyBankerCondition;        //申请条件
  SCORE            lAreaLimitScore;          //区域限制
  BYTE							cbArea[4];
  BYTE							cbControlStyle;
  bool						    bControlLimits;				//控制权限	
  //扑克信息
  BYTE              cbTableCardArray[5][5];       //桌面扑克

  //庄家信息
  WORD              wBankerUser;            //当前庄家
  WORD              cbBankerTime;           //庄家局数
  SCORE            lBankerWinScore;          //庄家赢分
  SCORE            lBankerScore;           //庄家分数
  INT               nEndGameMul;            //提前开牌百分比
  bool              bEnableSysBanker;         //系统做庄

  //结束信息
  SCORE            lEndBankerScore;          //庄家成绩
  SCORE            lEndUserScore;            //玩家成绩
  SCORE            lEndUserReturnScore;        //返回积分
  SCORE            lEndRevenue;            //游戏税收

  //全局信息
  BYTE              cbTimeLeave;            //剩余时间
  BYTE              cbGameStatus;           //游戏状态

  //房间信息
  TCHAR             szGameRoomName[SERVER_LEN];     //房间名称
  bool              bGenreEducate;            //练习模式

  SUPERBANKERCONFIG				superbankerConfig; //抢庄配置
  WORD							wCurSuperRobBankerUser;
  int							typeCurrentBanker;
  tagOccUpYesAtconfig   		occupyseatConfig;
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
  //占位玩家成绩
  SCORE						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];
  SCORE							lBottomPourImpose;    //下注限制
  tagCustomAndroid      		CustomAndroid;            //机器人配置

};

//游戏空闲
struct CMD_S_GameFree
{
  BYTE							cbTimeLeave;				//剩余时间
  INT64                         nListUserCount;				//列表人数
  SCORE						lStorageStart;				//
  BYTE							cbControl;					//控制是否还有效
};

//游戏开始
struct CMD_S_GameStart
{
  WORD              wBankerUser;            //庄家位置
  SCORE            lBankerScore;           //庄家金币
  SCORE            lUserMaxScore;            //我的金币
  BYTE              cbTimeLeave;            //剩余时间
  bool              bContiueCard;           //继续发牌
  int               nChipRobotCount;          //人数上限 (下注机器人)
  int               nAndriodApplyCount;         //机器人列表人数
  SCORE				lBottomPourImpose;    //下注限制
};

//用户下注
struct CMD_S_PlaceJetton
{
  WORD              wChairID;             //用户位置
  BYTE              cbJettonArea;           //筹码区域
  SCORE            lJettonScore;           //加注数目
  bool              bIsAndroid;             //是否机器人
  bool              bAndroid;           //机器标识
  SCORE				lAreaAllJetton;		//区域总下注
   SCORE			lAllJetton;			//总下注
};

//游戏结束
struct CMD_S_GameEnd
{
  //下局信息
  BYTE              cbTimeLeave;            //剩余时间

  //扑克信息
  BYTE              cbTableCardArray[5][5];       //桌面扑克
  BYTE              cbLeftCardCount;          //扑克数目

  BYTE              bcFirstCard;

  //庄家信息
  SCORE            lBankerScore;           //庄家成绩
  SCORE            lBankerTotallScore;         //庄家成绩
  INT               nBankerTime;            //做庄次数

  //玩家成绩
  SCORE            lUserScore;             //玩家成绩
  SCORE            lUserReturnScore;         //返回积分

  //全局信息
  SCORE            lRevenue;             //游戏税收
  SCORE			  lOccupySeatUserWinScore[6];
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON      1                 //用户下注
#define SUB_C_APPLY_BANKER      2                 //申请庄家
#define SUB_C_CANCEL_BANKER     3                 //取消申请
#define SUB_C_CONTINUE_CARD     4                 //继续发牌
#define SUB_C_AMDIN_COMMAND     5                 //管理员命令
#define SUB_C_UPDATE_STORAGE        6                 //更新库存

#define SUB_C_SUPERROB_BANKER		7						//超级抢庄
#define SUB_C_OCCUPYSEAT			8						//占位
#define SUB_C_QUIT_OCCUPYSEAT		9						//退出占位

#define SUB_C_PEIZHI_USER			10								    //配置玩家
#define SUB_C_DelPeizhi			    11									//散删除配置
#define SUB_C_ANDROIDXIAZHUANG	    12					//机器人下庄
//客户端消息
#define IDM_ADMIN_COMMDN      WM_USER+1000
#define IDM_UPDATE_STORAGE      WM_USER+1001

//控制区域信息
struct tagControlInfo
{
  BYTE cbControlArea[MAX_INDEX];      //控制区域
};
#define   CS_BANKER_LOSE    1
#define   CS_BANKER_WIN   2
#define   CS_BET_AREA     3

struct tagAdminReq
{
  BYTE              m_cbExcuteTimes;          //执行次数
  BYTE              m_cbControlStyle;         //控制方式
  bool              m_bWinArea[4];            //赢家区域
};
#define RQ_SET_WIN_AREA   1
#define RQ_RESET_CONTROL  2
#define RQ_PRINT_SYN    3

struct CMD_C_AdminReq
{
  BYTE cbReqType;
  BYTE cbExtendData[20];      //附加数据
};
//用户下注
struct CMD_C_PlaceJetton
{
  BYTE              cbJettonArea;           //筹码区域
  SCORE            lJettonScore;           //加注数目
};

//更新库存
struct CMD_C_UpdateStorage
{
  BYTE                            cbReqType;            //请求类型
  SCORE            lStorageDeduct;         //库存衰减
  SCORE            lStorageCurrent;        //当前库存
  SCORE            lStorageMax1;         //库存上限1
  LONGLONG            lStorageMul1;         //系统输分概率1
  SCORE            lStorageMax2;         //库存上限2
  LONGLONG            lStorageMul2;         //系统输分概率2
};
//占位
struct CMD_C_OccupySeat
{
	//占位玩家
	WORD wOccupySeatChairID;
	//占位索引
	BYTE cbOccupySeatIndex;
};
struct CMD_C_peizhiVec
{
	DWORD						dwGameID;
};
struct CMD_C_DelPeizhi
{
	DWORD						dwGameID;
};
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif