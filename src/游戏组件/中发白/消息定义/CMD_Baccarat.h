#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//1字节对其
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						105									//游戏 I D
#define GAME_NAME					TEXT("中发白")						//游戏名字

#define GAME_PLAYER					MAX_CHAIR							//游戏人数

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//下注状态
#define	GAME_SCENE_END				GAME_STATUS_PLAY+1					//结束状态

//玩家索引
#define AREA_ZHONG					0									//中家索引
#define AREA_PING					1									//平家索引
#define AREA_FA						2									//发家索引
#define AREA_ZERO					3									//0
#define AREA_ONE					4									//1
#define AREA_TWO					5									//2
#define AREA_THREE					6									//3
#define AREA_FOUR					7									//4
#define AREA_FIVE					8									//5
#define AREA_SIX					9									//6
#define AREA_SEVEN					10									//7
#define AREA_EIGHT					11									//8
#define AREA_NINE					12									//9
#define AREA_LEOPARD				13									//豹子
#define AREA_TIANGANG				14									//天杠



#define AREA_MAX					15									//最大区域

//区域倍数multiple
#define MULTIPLE_ZHONG					2									//中家索引
#define MULTIPLE_PING					10									//平家索引
#define MULTIPLE_FA						2									//发家索引
#define MULTIPLE_ZERO					5									//0
#define MULTIPLE_ONE					5									//1
#define MULTIPLE_TWO					5									//2
#define MULTIPLE_THREE					5									//3
#define MULTIPLE_FOUR					5									//4
#define MULTIPLE_FIVE					5									//5
#define MULTIPLE_SIX					5									//6
#define MULTIPLE_SEVEN					5									//7
#define MULTIPLE_EIGHT					5									//8
#define MULTIPLE_NINE					5									//9
#define MULTIPLE_LEOPARD				10									//豹子
#define MULTIPLE_TIANGANG				30									//天杠


//占位配置
#define OCCUPYSEAT_VIPTYPE  0          //会员占位
#define OCCUPYSEAT_CONSUMETYPE  1      //消耗金币占位
#define OCCUPYSEAT_FREETYPE  2         //免费占位

//赔率定义
#define RATE_TWO_PAIR				12									//对子赔率
#define SERVER_LEN					32									//房间长度


#define MAX_OCCUPY_SEAT_COUNT       8                                 //最大占位个数

#define IDM_UPDATE_STORAGE			WM_USER+1001

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif
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
	SCORE lOccupySeatConsume;
	//免费占位金币上限
	SCORE lOccupySeatFree;
	//强制站立条件
	SCORE lForceStandUpCondition;
};
//记录信息
struct tagServerGameRecord
{
	bool							bPlayerTianGang;					//天杠标识
	bool							bBankerTianGang;					//天杠标识
	bool							bPlayerTwoPair;						//对子标识
	bool							bBankerTwoPair;						//对子标识
	BYTE							cbPlayerCount;						//闲家点数
	BYTE							cbBankerCount;						//庄家点数
	BYTE							cbGameResult;						//游戏结果
};

struct tagCustomAndroid
{
	//坐庄
	BOOL							nEnableRobotBanker;				//是否做庄
	LONGLONG						lRobotBankerCountMin;			//坐庄次数
	LONGLONG						lRobotBankerCountMax;			//坐庄次数
	LONGLONG						lRobotListMinCount;				//列表人数
	LONGLONG						lRobotListMaxCount;				//列表人数
	LONGLONG						lRobotApplyBanker;				//最多申请个数
	LONGLONG						lRobotWaitBanker;				//空盘重申

	//下注
	LONGLONG						lRobotMinBetTime;				//下注筹码个数
	LONGLONG						lRobotMaxBetTime;				//下注筹码个数
	SCORE						lRobotMinJetton;				//下注筹码金额
	SCORE						lRobotMaxJetton;				//下注筹码金额
	LONGLONG						lRobotBetMinCount;				//下注机器人数
	LONGLONG						lRobotBetMaxCount;				//下注机器人数
	SCORE						lRobotAreaLimit;				//区域限制

	//存取款
	SCORE						lRobotScoreMin;					//金币下限
	SCORE						lRobotScoreMax;					//金币上限
	SCORE						lRobotBankGetMin;				//取款最小值(非庄)
	SCORE						lRobotBankGetMax;				//取款最大值(非庄)
	SCORE						lRobotBankGetBankerMin;			//取款最小值(坐庄)
	SCORE						lRobotBankGetBankerMax;			//取款最大值(坐庄)
	LONGLONG						lRobotBankStoMul;				//存款百分比
	
	//区域几率
	int								nAreaChance[15];		//区域几率
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
		lRobotMaxJetton = 500;
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

		int nTmpAreaChance[15] = {30, 3, 30, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1};
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));

	}
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	SCORE						lUserStartScore;					//用户金币
	SCORE						lUserWinLost;						//用户金币
	SCORE						lUserBet[AREA_MAX];				//用户下注
};

//下注信息数组
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//库存控制
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请
#define SUB_S_AMDIN_COMMAND			109									//管理员命令
#define SUB_S_UPDATE_STORAGE        110									//更新库存
#define SUB_S_SEND_USER_BET_INFO    111									//发送下注
#define SUB_S_USER_SCORE_NOTIFY		112									//发送下注
//超级抢庄
#define SUB_S_SUPERROB_BANKER       113
//超级抢庄玩家离开
#define SUB_S_CURSUPERROB_LEAVE     114
//占位
#define SUB_S_OCCUPYSEAT            115
//占位失败
#define SUB_S_OCCUPYSEAT_FAIL       116
//更新占位
#define SUB_S_UPDATE_OCCUPYSEAT     117

#define SUB_S_PEIZHI_USER			119								    //配置玩家
#define SUB_S_DelPeizhi				120									//删除配置
#define SUB_S_UPALLLOSEWIN			121									//更新玩家输赢
#define SUB_S_BET_INFO				122									//下注信息

struct CMD_S_DelPeizhi
{
	DWORD						dwGameID;
};

struct CMD_S_peizhiVec
{
	//TCHAR						szNickName[32];
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
//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
		 #define ACK_SET_WIN_AREA  1
		 #define ACK_PRINT_SYN     2
		 #define ACK_RESET_CONTROL 3
	BYTE cbResult;
	#define CR_ACCEPT  2			//接受
	#define CR_REFUSAL 3			//拒绝
	//BYTE cbExtendData[20];			//附加数据
	int  nControlTimes;				//控制的次数
	BYTE bWinArea[15];				//控制赢的区域
	
};


//更新库存
struct CMD_S_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	SCORE						lStorageStart;				//起始库存
	SCORE						lStorageDeduct;				//库存衰减
	SCORE						lStorageCurrent;				//当前库存
	SCORE						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	SCORE						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};

//发送下注
struct CMD_S_SendUserBetInfo
{
	SCORE						lUserStartScore[GAME_PLAYER];				//起始分数
	SCORE						lUserJettonScore[GAME_PLAYER][AREA_MAX];//个人总注
};

//失败结构
struct CMD_S_PlaceBetFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lBetArea;							//下注区域
	SCORE						lPlaceScore;						//当前下注
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;						//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	SCORE						lBankerScore;						//庄家分数
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	SCORE						lPlayFreeSocre;						//玩家自由金币

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	SCORE						lBankerScore;						//庄家分数
	SCORE						lBankerWinScore;					//庄家赢分
	WORD							wBankerTime;						//庄家局数

	//是否系统坐庄
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	SCORE						lApplyBankerCondition;				//申请条件
	SCORE						lAreaLimitScore;					//区域限制

	BYTE							cbArea[15];
	BYTE							cbControlTimes;
	bool							bIsGameCheatUser;				//是否有管理权限

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	bool							bGenreEducate;						//是否练习场

	SUPERBANKERCONFIG				superbankerConfig; //抢庄配置
	WORD							wCurSuperRobBankerUser;
	int								typeCurrentBanker;
	tagOccUpYesAtconfig   			occupyseatConfig;
	WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	SCORE							lBottomPourImpose;    //下注限制
	tagCustomAndroid				CustomAndroid;						//机器人配置


};
//下注信息
struct CMD_S_StatusBetInfo
{
	SCORE							lPlayerBet[AREA_MAX];					//真人每个区域总下注
	SCORE							lAndroidBet[AREA_MAX];					//机器人每个区域总下注
	SCORE							lPlayerAreaBet[AREA_MAX];				//真人每个人每个区域下注
	DWORD							dwGameID;								//玩家id
	SCORE							lPlayerTotleBet;						//每个玩家总下注

};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态

	//下注数
	SCORE							lAllBet[AREA_MAX];					//总下注
	SCORE							lPlayBet[AREA_MAX];					//玩家下注

	//玩家积分
	SCORE						lPlayBetScore;						//玩家最大下注	
	SCORE						lPlayFreeSocre;						//玩家自由金币

	//玩家输赢
	SCORE						lPlayScore[AREA_MAX];				//玩家输赢
	SCORE						lPlayAllScore;						//玩家成绩
	SCORE						lRevenue;							//税收
	SCORE						lAllPlayerScore[GAME_PLAYER];		//所有玩家成绩

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	SCORE						lBankerScore;						//庄家分数
	SCORE						lBankerWinScore;					//庄家赢分
	WORD							wBankerTime;						//庄家局数

	//是否系统坐庄
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	SCORE						lApplyBankerCondition;				//申请条件
	SCORE						lAreaLimitScore;					//区域限制

	BYTE							cbArea[15];
	BYTE							cbControlTimes;
	bool							bIsGameCheatUser;				//是否有管理权限

	//扑克信息
 	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][2];				//桌面扑克

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	bool							bGenreEducate;						//是否练习场


	SUPERBANKERCONFIG				superbankerConfig; //抢庄配置
	WORD							wCurSuperRobBankerUser;
	int								typeCurrentBanker;
	tagOccUpYesAtconfig   			occupyseatConfig;
	WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	//占位玩家成绩
	SCORE						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];

	BYTE							cbWinArea[AREA_MAX];					//输赢区域
	SCORE							lBottomPourImpose;    //下注限制	
	tagCustomAndroid				CustomAndroid;						//机器人配置	

};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
	INT64							nListUserCount;						//列表人数
	SCORE						lStorageStart;						//
	BYTE							cbControl;							//控制是否还有效
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//剩余时间

	WORD							wBankerUser;						//庄家位置
	SCORE						lBankerScore;						//庄家金币

	SCORE						lPlayBetScore;						//玩家最大下注	
	SCORE						lPlayFreeSocre;						//玩家自由金币
	SCORE							lBottomPourImpose;    //下注限制
	int								nChipRobotCount;					//人数上限 (下注机器人)
	__int64                         nListUserCount;						//列表人数
	int								nAndriodCount;						//机器人列表人数
};

//用户下注
struct CMD_S_PlaceBet
{
	WORD							wChairID;							//用户位置
	BYTE							cbBetArea;							//筹码区域
	SCORE						lBetScore;							//加注数目
	bool							cbAndroidUser;						//机器标识
	bool							cbAndroidUserT;						//机器标识
	SCORE						lPlayerAreaBet;						//个人区域总下注
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][2];				//桌面扑克
 
	//庄家信息
	SCORE						lBankerScore;						//庄家成绩
	SCORE						lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	SCORE						lPlayScore[AREA_MAX];				//玩家成绩
	SCORE						lPlayAllScore;						//玩家成绩
	SCORE						lAllPlayerScore[GAME_PLAYER];		//所有玩家成绩

	//全局信息
	SCORE						lRevenue;							//游戏税收

	//占位玩家成绩
	SCORE lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];
	BYTE							cbWinArea[AREA_MAX];					//输赢区域
};

struct CMD_S_UserScoreNotify
{
	WORD							wChairID;							//玩家ID
	//玩家积分
	SCORE						lPlayBetScore;						//玩家最大下注
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

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家
#define SUB_C_CANCEL_BANKER			3									//取消申请
#define SUB_C_AMDIN_COMMAND			4									//管理员命令
#define SUB_C_UPDATE_STORAGE        5									//更新库存
#define SUB_C_SUPERROB_BANKER       6									//超级抢庄
#define SUB_C_OCCUPYSEAT            7									//占位
#define SUB_C_QUIT_OCCUPYSEAT       8									//退出占位   

#define SUB_C_PEIZHI_USER			10								    //配置玩家
#define SUB_C_DelPeizhi			    11									//删除配置

#define RQ_SET_WIN_AREA	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
struct CMD_C_AdminReq
{
	BYTE cbReqType;
	int  nControlTimes;				//控制的次数
	BYTE bWinArea[15];				//控制赢的区域

};
//占位
struct CMD_C_OccupySeat
{
	//占位玩家
	WORD wOccupySeatChairID;
	//占位索引
	BYTE cbOccupySeatIndex;
};
//用户下注
struct CMD_C_PlaceBet
{
	
	BYTE							cbBetArea;						//筹码区域
	SCORE						lBetScore;						//加注数目
};
struct CMD_C_peizhiVec
{
	DWORD						dwGameID;
};

struct CMD_C_DelPeizhi
{
	DWORD						dwGameID;
};

//更新库存
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	SCORE						lStorageDeduct;					//库存衰减
	SCORE						lStorageCurrent;				//当前库存
	SCORE						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	SCORE						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};

//还原对其数
#pragma pack()
//////////////////////////////////////////////////////////////////////////

#endif
