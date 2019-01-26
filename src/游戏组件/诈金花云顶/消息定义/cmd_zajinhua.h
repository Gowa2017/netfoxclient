#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE


#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							6									//游戏 I D
#define GAME_PLAYER						7									//游戏人数
#define GAME_NAME						TEXT("诈金花")						//游戏名字
#define MAX_COUNT						3									//扑克数目
#define VERSION_SERVER			    	PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				    PROCESS_VERSION(7,0,1)				//程序版本

//游戏概率
#define GIVE_UP							2									//放弃概率
#define LOOK_CARD						8									//看牌概率
#define COMPARE_CARD					4									//比牌概率
#define RAISE_SCORE						2									//首轮加注概率
#define MAX_LOOK_INDEX					16									//看牌索引
#define MAX_OPEN_INDEX					5									//开牌索引

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家
#define GER_COMPARECARD					0x20								//比牌结束
#define GER_OPENCARD					0x30								//开牌结束

#define LEN_NICKNAME				32									//昵称长度
#define    CHEAT_TYPE_LOST		     0		                      //输
#define    CHEAT_TYPE_WIN		     1		                      //赢

#define    DOUBLE_DOUBLE_X			 0.001						  //两个相同double的值相减，值不一定为0
#define    DIFEN_TIMES			     2						      //底分相对底注的倍数
//游戏状态
#define GS_T_FREE					GAME_STATUS_FREE						//等待开始
#define GS_T_SCORE					GAME_STATUS_PLAY						//叫分状态
#define GS_T_PLAYING				GAME_STATUS_PLAY+1						//游戏进行

//命令消息
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_CONTROL				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//操作记录
#define MAX_OPERATION_RECORD			20									//操作记录条数
#define RECORD_LENGTH					128									//每条记录字长

//密钥总个数（前4个位取用户USERID的四位，后12位可变）
#define AESKEY_TOTALCOUNT					16

//可变密钥个数
#define AESKEY_VARIABLECOUNT				12

//公共加密原文长度
#define AESENCRYPTION_LENGTH				16


//扑克类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define	CT_SHUN_ZI					3									//顺子类型
#define CT_JIN_HUA					4									//金花类型
#define	CT_SHUN_JIN					5									//顺金类型
#define	CT_BAO_ZI					6									//豹子类型
#define CT_SPECIAL					7									//特殊类型

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_SCORE					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_COMPARE_CARD				105									//比牌跟注
#define SUB_S_LOOK_CARD					106									//看牌跟注
#define SUB_S_PLAYER_EXIT				107									//用户强退
#define SUB_S_OPEN_CARD					108									//开牌消息
#define SUB_S_WAIT_COMPARE				109									//等待比牌
#define SUB_S_ANDROID_CARD				110									//智能消息
#define SUB_S_CHEAT_CARD				111									//看牌消息

#define SUB_S_ADMIN_STORAGE_INFO		112									//刷新控制服务端
#define SUB_S_REQUEST_QUERY_RESULT		113									//查询用户结果
#define SUB_S_USER_CONTROL				114									//用户控制
#define SUB_S_USER_CONTROL_COMPLETE		115									//用户控制完成
#define SUB_S_OPERATION_RECORD		    116									//操作记录
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 117
#define SUB_S_ANDROID_BANKOPERATOR		119	
#define SUB_S_UPDATEAESKEY				120									//更新密钥

#define SUB_S_RC_TREASEURE_DEFICIENCY	121									//房卡金币房间金币不足

#define SUB_S_SHOW_CARD					122									//亮牌消息

#define SUB_S_AUTO_COMPARE				123									//自动比牌消息

#define SUB_S_ALL_CARD					124									//超控发牌消息

#define SUB_S_ADMIN_COLTERCARD			125		


//机器人存款取款
struct tagCustomAndroid
{
	double									lRobotScoreMin;	
	double									lRobotScoreMax;
	double	                                lRobotBankGet; 
	double									lRobotBankGetBanker; 
	double									lRobotBankStoMul; 
};

//控制类型
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//控制结果      控制成功 、控制失败 、控制取消成功 、控制取消无效
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//用户行为
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//控制信息
typedef struct
{
	CONTROL_TYPE						control_type;					  //控制类型
	BYTE								cbControlCount;					  //控制局数
	bool							    bCancelControl;					  //取消标识
}USERCONTROL;

//房间用户信息
typedef struct
{
	WORD								wChairID;							//椅子ID
	WORD								wTableID;							//桌子ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//机器人标识
	TCHAR								szNickName[LEN_NICKNAME];			//用户昵称
	BYTE								cbUserStatus;						//用户状态
	BYTE								cbGameStatus;						//游戏状态
}ROOMUSERINFO;

//房间用户控制
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//房间用户信息
	USERCONTROL							userControl;						//用户控制
}ROOMUSERCONTROL;

//游戏状态
struct CMD_S_StatusFree
{
	double							lCellScore;							//基础积分
	//double							lRoomStorageStart;					//房间起始库存
	//double							lRoomStorageCurrent;				//房间当前库存
	tagCustomAndroid					CustomAndroid;						//机器人配置
	unsigned char						chUserAESKey[AESKEY_TOTALCOUNT];	//初始密钥
	TCHAR								szServerName[32];					//房间名称
};

//游戏状态
struct CMD_S_StatusPlay
{
	//加注信息
	double							lMaxCellScore;						//单元上限
	double							lCellScore;							//单元下注
	double							lCurrentTimes;						//当前倍数
	double							lUserMaxScore;						//用户分数上限

	//状态信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool								bMingZhu[GAME_PLAYER];				//看牌状态
	double							lTableScore[GAME_PLAYER];			//下注数目
	//double							lRoomStorageStart;					//房间起始库存
	//double							lRoomStorageCurrent;				//房间当前库存
	tagCustomAndroid					CustomAndroid;						//机器人配置

	//扑克信息
	BYTE								cbHandCardData[MAX_COUNT];			//扑克数据

	//状态信息
	bool								bCompareState;						//比牌状态		
	unsigned char						chUserAESKey[AESKEY_TOTALCOUNT];	//初始密钥
	TCHAR								szServerName[32];					//房间名称

	int									nCurrentRounds;						//当前轮次
	int									nTotalRounds;						//总轮数

	double								dElapse;							//流逝时间
};

//机器人扑克
struct CMD_S_AndroidCard
{
	BYTE								cbRealPlayer[GAME_PLAYER];				//真人玩家
	BYTE								cbAndroidStatus[GAME_PLAYER];			//机器数目
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克
	double								lStockScore;							//当前库存
	bool								bAndroidControl;
};


//机器人扑克
struct CMD_S_CheatCardInfo
{
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克
};



//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	double							lMaxScore;							//最大下注
	double							lCellScore;							//单元下注
	double							lCurrentTimes;						//当前倍数
	double							lUserMaxScore;						//分数上限

	//用户信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//扑克数据
	BYTE								cbPlayStatus[GAME_PLAYER];			//用户状态

	int									nCurrentRounds;						//当前轮次
	int									nTotalRounds;						//总轮数
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	WORD								wCompareState;						//比牌状态
	double								lAddScoreCount;						//加注数目
	double								lCurrentTimes;						//当前倍数
	int									nCurrentRounds;						//当前轮次
	int									nNetwaititem;						//强制开牌
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
};

//自动比牌
struct CMD_S_AutoCompare
{
	WORD								wAutoCompareUser;					//自动比牌发起用户，当前用户current
};

//比牌数据包
struct CMD_S_CompareCard
{
	WORD								wCurrentUser;						//当前用户
	WORD								wCompareUser[2];					//比牌用户
	WORD								wLostUser;							//输牌用户
};

//看牌数据包
struct CMD_S_LookCard
{
	WORD								wLookCardUser;						//看牌用户
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
};

//亮牌数据包
struct CMD_S_ShowCard
{
	WORD								wShowCardUser;						//亮牌用户
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
};

//开牌数据包
struct CMD_S_OpenCard
{
	WORD								wWinner;							//胜利用户
};

//游戏结束
struct CMD_S_GameEnd
{
	double							lGameTax;							//游戏税收
	double							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER][3];			//用户扑克
	WORD								wCompareUser[GAME_PLAYER][6];		//比牌用户
	WORD								wEndState;							//结束状态
	bool								bDelayOverGame;						//延迟开始
	WORD								wServerType;						//房间类型
};

//用户退出
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//退出用户
};

//等待比牌
struct CMD_S_WaitCompare
{
	WORD								wCompareUser;						//比牌用户
};

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//用户信息
	bool								bFind;								//找到标识
};

//用户控制
struct CMD_S_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//用户昵称
	CONTROL_RESULT							controlResult;						//控制结果
	CONTROL_TYPE							controlType;						//控制类型
	BYTE									cbControlCount;						//控制局数
};

//用户控制
struct CMD_S_UserControlComplete
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//用户昵称
	CONTROL_TYPE							controlType;						//控制类型
	BYTE									cbRemainControlCount;				//剩余控制局数
};

//控制服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
	double	lRoomStorageStart;						//房间起始库存
	double	lRoomStorageCurrent;
	double	lRoomStorageDeduct;
	double	lMaxRoomStorage[2];
	WORD		wRoomStorageMul[2];
};

//操作记录
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//记录最新操作的20条记录
};

//请求更新结果
struct CMD_S_RequestUpdateRoomInfo_Result
{
	double							lRoomStorageCurrent;				//房间当前库存
	ROOMUSERINFO						currentqueryuserinfo;				//当前查询用户信息
	bool								bExistControl;						//查询用户存在控制标识
	USERCONTROL							currentusercontrol;
};

//更新密钥
struct CMD_S_UpdateAESKey
{
	unsigned char					chUserUpdateAESKey[AESKEY_TOTALCOUNT];	//密钥
};


//发牌数据包
struct CMD_S_AllCard
{
	bool                bAICount[GAME_PLAYER];
	BYTE                cbCardData[GAME_PLAYER][MAX_COUNT]; //用户扑克
	BYTE                cbSurplusCardCount;    //剩余数量
	BYTE                cbSurplusCardData[52]; //剩余扑克

};

struct CMD_S_Admin_ChangeCard
{
	WORD dwCharID;
	BYTE cbCardData[MAX_COUNT];	//用户扑克

};
//////////////////////////////////////////////////////////////////////////

//客户端命令结构
#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃消息
#define SUB_C_COMPARE_CARD				3									//比牌消息
#define SUB_C_LOOK_CARD					4									//看牌消息
#define SUB_C_OPEN_CARD					5									//开牌消息
#define SUB_C_WAIT_COMPARE				6									//等待比牌
#define SUB_C_FINISH_FLASH				7									//完成动画
#define SUB_C_ADD_SCORE_TIME			8									//完成动画
#define SUB_C_SHOW_CARD					9									//亮牌消息

#define SUB_C_STORAGE					10									//更新库存
#define	SUB_C_STORAGEMAXMUL				11									//设置上限
#define SUB_C_REQUEST_QUERY_USER		12									//请求查询用户
#define SUB_C_USER_CONTROL				13									//用户控制

//请求更新命令
#define SUB_C_REQUEST_UDPATE_ROOMINFO	14									//请求更新房间信息
#define SUB_C_CLEAR_CURRENT_QUERYUSER	15

#define SUB_C_AMDIN_CHANGE_CARD			16									//换牌


//用户加注
struct CMD_C_AddScore
{
	double							lScore;								//加注数目
	WORD								wState;								//当前状态
	unsigned char						chciphertext[AESENCRYPTION_LENGTH];	//密文
};

//比牌数据包
struct CMD_C_CompareCard
{	
	WORD								wCompareUser;						//比牌用户
};

struct CMD_C_UpdateStorage
{
	double						lRoomStorageCurrent;					//库存数值
	double						lRoomStorageDeduct;						//库存数值
};

struct CMD_C_ModifyStorage
{
	double						lMaxRoomStorage[2];							//库存上限
	WORD							wRoomStorageMul[2];							//赢分概率
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//查询用户GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //查询用户昵称
};

//用户控制
struct CMD_C_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//用户昵称
	USERCONTROL								userControlInfo;					//
};
//换牌
struct CMD_C_Admin_ChangeCard
{
	BYTE							cbStart;
	BYTE							cbEnd;
	DWORD							dwChairID;								//查询用户GAMEID
};

//////////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif
