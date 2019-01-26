#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						406									//游戏 I D
#define GAME_NAME					TEXT("水果拉霸")					//游戏名字

//组件属性
#define GAME_PLAYER					1									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

// 屏幕限制	
#define Screen_Width				(1280)		// 最小宽度
#define Screen_Height				(798)		// 最小高度

//数目定义
#define ITEM_COUNT					9										//图标数量
#define ITEM_X_COUNT				5										//图标数量
#define ITEM_Y_COUNT				3										//图标数量

//逻辑类型
#define CT_FUTOU					0					//香蕉				//斧头
#define CT_YINGQIANG				1					//西瓜				//银枪
#define CT_DADAO					2					//芒果				//大刀
#define CT_LU						3					//葡萄				//鲁
#define CT_LIN						4					//菠萝				//林
#define CT_SONG						5					//铃铛				//宋
#define CT_TITIANXINGDAO			6					//樱桃				//替天行道
#define CT_ZHONGYITANG				7					//酒吧				//忠义堂
#define CT_SHUIHUZHUAN				8					//梨子				//水浒传
#define CT_SHUIHUZHUAN2				9					//7字				
#define CT_SHENBEN					10					//扇贝				

//进入模式
#define GM_NULL						0									//结束
#define GM_SHUO_MING				1									//说明
#define GM_ONE						2									//开奖
#define GM_TWO						3									//比倍
#define GM_TWO_WAIT					4									//等待比倍
//#define GM_THREE					5									//小玛丽
#define GM_SMALL					5									//小游戏

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_ONE				GAME_STATUS_PLAY+1					//水浒传
#define GAME_SCENE_TWO				GAME_STATUS_PLAY+2					//骰子
#define GAME_SCENE_THREE			GAME_STATUS_PLAY+3					//水果机
#define GAME_SCENE_WIAT				GAME_STATUS_PLAY+4					//等待
#define GAME_SMALL_GAME				GAME_STATUS_PLAY+5					//小游戏状态
//空闲状态
struct CMD_S_StatusFree
{
	//游戏属性
	LONG							lCellScore;							//基础积分

	//下注值
	BYTE							cbBetCount;							//下注数量
	LONGLONG						lBetScore[9];						//下注大小
	BYTE							cbBetLine;							//下注数量
	LONGLONG 						ALLbonus;							 //奖金
};

//小游戏断线重连
struct CMD_S_SmallStatus
{
	//游戏属性
	LONG							lCellScore;								//基础积分
	BYTE							cbGameMode;								//游戏模式
	LONGLONG						lBet;									//押线下注
	LONGLONG						lBetCount;								//压线数量
	LONGLONG						lOneGameScore;							//押线得分
	LONGLONG						lSmallGameScore;						//小游戏得分	
	BYTE							cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//开奖信息
	LONGLONG						lBetScore[9];							//下注大小
	BYTE							cbCustoms;								//小游戏关卡
	WORD							wUseTime;								//小游戏剩时间
	LONGLONG                        cbBonus;									//奖金
};

//游戏状态
struct CMD_S_StatusPlay
{
	//下注值
	BYTE							cbBetCount;							//下注数量
	LONGLONG						lBetScore[9];						//下注大小
	BYTE							cbBetCount1;						//下注数量

	LONGLONG                        ALLbonus;                 //奖金
};

//操作记录
#define MAX_OPERATION_RECORD		20									 //操作记录条数
#define RECORD_LENGTH				200									 //每条记录字长

//控制类型
typedef enum{CONTINUE_3, CONTINUE_4, CONTINUE_5, CONTINUE_ALL, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//控制结果      控制成功 、控制失败 、控制取消成功 、控制取消无效
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//用户行为
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//控制信息
typedef struct
{
	CONTROL_TYPE					controlType;					   //控制类型
	BYTE							cbControlData;					   //控制数据
	BYTE							cbControlCount;					   //控制局数
	BYTE							cbZhongJiang;					   //中奖概率
	BYTE							cbZhongJiangJian;				   //衰减比例
}USERCONTROL;

//房间用户信息
typedef struct
{
	WORD							wChairID;							//椅子ID
	WORD							wTableID;							//桌子ID
	DWORD							dwGameID;							//GAMEID
	bool							bAndroid;							//机器人标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbUserStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	LONGLONG						lGameScore;							//游戏输赢分
	USERCONTROL						UserContorl;						//控制消息
}ROOMUSERINFO;

//桌子库存投入信息
typedef struct
{
	WORD							wTableID;						
	LONGLONG						lTableStorageInput;
}TABLESTORAGEINPUT;

//查询用户结果
struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO					userinfo;							//用户信息
	bool							bFind;								//找到标识
};

//用户控制
struct CMD_S_UserControl
{
	DWORD							dwGameID;							//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	CONTROL_RESULT					controlResult;						//控制结果
	USERCONTROL						UserControl;						//控制类型
	BYTE							cbControlCount;						//控制局数
};

//用户控制
struct CMD_S_UserControlComplete
{
	DWORD							dwGameID;							//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	USERCONTROL						UserControl;						//控制类型
	BYTE							cbRemainControlCount;				//剩余控制局数
};

//控制服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG						lRoomStorageStart;					//房间起始库存
	LONGLONG						lRoomStorageCurrent;				//房间当前库存
	LONGLONG						lRoomStorageInput;					//房间库存投入
	LONGLONG						lCurrentStorageTable;				//当前桌子库存
	LONGLONG						lRoomStockRecoverScore;				//房间库存输赢
	LONGLONG						lCurrentDeductRoom;					//全局库存衰减
	LONGLONG						lMaxStorageRoom[2];					//全局库存上限
	WORD							wStorageMulRoom[2];					//全局库存赢分概率
	WORD							wGameTwo;							//比倍概率
	WORD							wGameTwoDeduct;						//比倍概率
	WORD							wGameThree;							//比倍概率
	WORD							wGameThreeDeduct;					//小玛丽概率
};

//操作记录
struct CMD_S_Operation_Record
{
	TCHAR							szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];		//记录最新操作的20条记录
};

//更新房间库存
struct CMD_S_UpdateRoomStorage
{
	LONGLONG						lRoomStorageStart;					//房间起始库存
	LONGLONG						lRoomStorageCurrent;				//房间当前库存
	LONGLONG						lRoomStorageInput;					//房间库存投入
	LONGLONG						lRoomStockRecoverScore;				//房间库存输赢
};

//更新玩家列表
struct CMD_S_UpdateRoomUserList
{
	DWORD							dwUserID;							//用户ID
	ROOMUSERINFO					roomUserInfo;
};


//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START				100									//押线结束
#define SUB_S_GAME_CONCLUDE				101									//小玛丽结束
#define SUB_S_SMALL_START				105									//小游戏开始
#define SUB_S_TWO_GAME_CONCLUDE			106									//小游戏结束结束
#define SUB_S_THREE_KAI_JIAN			104									//中奖记录返回


#define SUB_S_TWO_START					103									//押线结束
#define SUB_S_THREE_END					107									//小玛丽结束
#define SUB_S_SET_BASESCORE				108									//设置基数
#define SUB_S_AMDIN_COMMAND				109									//系统控制
#define SUB_S_ADMIN_STORAGE_INFO		110									//刷新控制服务端
#define SUB_S_REQUEST_QUERY_RESULT		111									//查询用户结果
#define SUB_S_USER_CONTROL				112									//用户控制
#define SUB_S_USER_CONTROL_COMPLETE		113									//用户控制完成
#define SUB_S_OPERATION_RECORD			114									//操作记录
#define SUB_S_UPDATE_ROOM_STORAGE		115									//更新房间库存
#define SUB_S_UPDATE_ROOMUSERLIST		116									//更新房间用户列表

//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	LONGLONG							lScore;										//游戏积分	
	BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];		//开奖信息
	BYTE								cbGameMode;									//游戏模式
	LONGLONG							cbBouns;									//小玛丽次数
};

struct CMD_S_WinningRecord
{
	BYTE								BonusCount;								//中奖池次数
	LONGLONG							BonusSorce[10];							//中奖池记录
	SYSTEMTIME							BonusSorceDate[10];						//中奖池时间
	TCHAR								NickName[10][LEN_NICKNAME];				//玩家
};

//比倍开始
struct CMD_S_GameTwoStart
{
	//下注信息
	BYTE								cbOpenSize[2];								//骰子
	LONGLONG							lScore;										//游戏积分	
};

//小玛丽开始
struct CMD_S_GameThreeStart
{
	//下注信息
	BYTE								cbItem[4];									//骰子
	BYTE								cbBounsGameCount;							//游戏积分	
	LONGLONG							lScore;										//游戏积分	
};

//小玛丽开始
struct CMD_S_GameThreeKaiJiang
{
	//下注信息
	BYTE								cbIndex;									//骰子
	int									nTime;										//骰子
	LONGLONG							lScore;										//游戏积分	
};

//游戏结束
struct CMD_S_OneGameConclude
{
	//积分变量
	LONGLONG							lCellScore;									//单元积分
	LONGLONG							lGameScore;									//游戏积分
	BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];		//开奖信息
	BYTE								cbGameMode;									//游戏模式
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define SUB_C_SMALL_START			1									//开始小游戏
#define SUB_C_SMALL_END				2									//结束小游戏
#define SUB_C_THREE_START			3									//比倍开始
#define SUB_C_LOOK_RECORD			4									//中奖纪录
#define SUB_C_THREE_END				5									//结束小游戏
#define SUB_C_ONE_START				6									//押线开始
#define SUB_C_ONE_END				7									//结束押线

#define SUB_C_UPDATE_TABLE_STORAGE	28									//更新桌子库存
#define	SUB_C_MODIFY_ROOM_CONFIG	29									//修改房间配置
#define SUB_C_REQUEST_QUERY_USER	30									//请求查询用户
#define SUB_C_USER_CONTROL			31									//用户控制

//请求更新命令
#define SUB_C_REQUEST_UDPATE_ROOMUSERLIST			32					//请求更新房间用户列表
#define SUB_C_REQUEST_UDPATE_ROOMSTORAGE			33					//请求更新房间库存

//单桌模式
#define SUB_C_SINGLEMODE_CONFIRM					35					//
//批量模式
#define SUB_C_BATCHMODE_CONFIRM						36					//

struct CMD_C_SmallStart
{
	BYTE				cbGameMode;									//游戏模式
	DWORD				SamllGameFruitData[4][5];					//小游戏数据
};

struct CMD_C_SmallStatus
{
	bool                Isover;						//是否结束
	BYTE				cbCustoms;					//当前关卡
	LONGLONG			SmallSorce;					//水果分数
};

//用户叫分
struct CMD_C_OneStart
{		
	LONGLONG							lBetCount;							//压线数量
	LONGLONG							lBet;								//押线分数
};

//用户叫分
struct CMD_C_TwoMode
{
	BYTE							cbOpenMode;							//比倍模式 0：半比倍  1：全比倍   2：双比倍
};

//用户叫分
struct CMD_C_TwoStart
{
	BYTE							cbOpenArea;							//下注区域 0：左  1：中   2：右
};

//更新本桌库存
struct CMD_C_UpdateStorageTable
{
	LONGLONG						lStorageTable;							//桌子库存
};

struct CMD_C_ModifyRoomConfig
{
	LONGLONG						lStorageDeductRoom;						//全局库存衰减
	LONGLONG						lMaxStorageRoom[2];						//全局库存上限
	WORD							wStorageMulRoom[2];						//全局赢分概率
	WORD							wGameTwo;								//比倍概率
	WORD							wGameTwoDeduct;							//比倍概率
	WORD							wGameThree;								//小玛丽概率
	WORD							wGameThreeDeduct;						//小玛丽概率
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//查询用户GAMEID
};

//用户控制
struct CMD_C_UserControl
{
	DWORD							dwGameID;								//GAMEID
	USERCONTROL						userControlInfo;						//用户控制信息
};

//单桌修改
struct CMD_C_SingleMode
{
	WORD							wTableID;
	LONGLONG						lTableStorage;
};

//批量修改
struct CMD_C_BatchMode
{
	LONGLONG						lBatchModifyStorage;
	WORD							wBatchTableCount;						//批量修改的桌子张数
	bool							bBatchTableFlag[MAX_TABLE];				//TRUE为修改的标志	
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif