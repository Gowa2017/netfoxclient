#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						202									//游戏 I D
#define GAME_NAME					TEXT("不洗牌斗地主")						//游戏名字

//组件属性
#define GAME_PLAYER					3									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//数目定义
#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目

//逻辑数目
#define NORMAL_COUNT				17									//常规数目
#define DISPATCH_COUNT				51									//派发数目
#define GOOD_CARD_COUTN				38									//好牌数目

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//逻辑类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_BOMB_CARD				11									//炸弹类型
#define CT_MISSILE_CARD				12									//火箭类型

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define	GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//叫分状态
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//游戏进行
#define GAME_SCENE_MULTIPLE			GAME_STATUS_PLAY+2					//加倍

//空闲状态
struct CMD_S_StatusFree
{
	//游戏属性
	SCORE							lCellScore;							//基础积分

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
};

//叫分状态
struct CMD_S_StatusCall
{
	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//游戏信息
	SCORE							lCellScore;							//单元积分
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分
	BYTE							cbScoreInfo[GAME_PLAYER];			//叫分信息
	BYTE							cbHandCardData[NORMAL_COUNT];		//手上扑克
	bool							bTrustee[GAME_PLAYER];				//托管标志
	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
};

//加倍状态
struct CMD_S_StatusMultiple
{
	//时间信息
	BYTE							cbMultiple;							//加倍时间

	//游戏变量
	SCORE							lCellScore;							//单元积分
	WORD							wBankerUser;						//庄家用户
	BYTE							cbBankerScore;						//庄家叫分

	//扑克信息
	BYTE							cbBankerCard[3];					//游戏底牌
	BYTE							cbHandCardData[MAX_COUNT];			//手上扑克
	BYTE							cbHandCardCount[GAME_PLAYER];		//扑克数目

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
	bool							bTrustee[GAME_PLAYER];				//是否托管
	SCORE							lEnterScore;						//进入积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

	//游戏变量
	SCORE							lCellScore;							//单元积分
	BYTE							cbBombCount;						//炸弹次数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分

	//出牌信息
	WORD							wTurnWiner;							//胜利玩家
	BYTE							cbTurnCardCount;					//出牌数目
	BYTE							cbTurnCardData[MAX_COUNT];			//出牌数据

	//扑克信息
	BYTE							cbBankerCard[3];					//游戏底牌
	BYTE							cbHandCardData[MAX_COUNT];			//手上扑克
	BYTE							cbHandCardCount[GAME_PLAYER];		//扑克数目

	//历史积分
	SCORE							lTurnScore[GAME_PLAYER];			//积分信息
	SCORE							lCollectScore[GAME_PLAYER];			//积分信息
	bool							bTrustee[GAME_PLAYER];				//是否托管
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define	SUB_S_GAME_START			100									//游戏开始
#define SUB_S_CALL_SCORE			101									//用户叫分
#define SUB_S_BANKER_INFO			102									//庄家信息
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//用户放弃
#define SUB_S_GAME_CONCLUDE			105									//游戏结束
#define SUB_S_TRUSTEE				106									//设置基数
#define SUB_S_CHEAT_CARD			107									//作弊扑克
#define SUB_S_SET_BASESCORE			108									//托管
#define SUB_S_OTHER_CARDS			109									//看其它人牌
#define SUB_S_MULTIPLE				110									//加倍
#define SUB_S_START_OUTCARD			111									//开始出牌
#define SUB_S_ANDROID_BANKOPERATOR    112                 //机器人银行操作

//发送扑克
struct CMD_S_GameStart
{
	WORD							wStartUser;							//开始玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbValidCardData;					//明牌扑克
	BYTE							cbValidCardIndex;					//明牌位置
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbCardData[NORMAL_COUNT];			//扑克列表
};

//机器人扑克
struct CMD_S_AndroidCard
{
	BYTE							cbHandCard[GAME_PLAYER][NORMAL_COUNT];//手上扑克
	BYTE							cbBankerCard[3];
	WORD							wCurrentUser ;						//当前玩家
};

//作弊扑克
struct CMD_S_CheatCard
{
	WORD							wCardUser[GAME_PLAYER ];				//作弊玩家
	BYTE							cbUserCount;							//作弊数量
	BYTE							cbCardData[GAME_PLAYER ][MAX_COUNT];	//扑克列表
	BYTE							cbCardCount[GAME_PLAYER ];				//扑克数量

};
//用户叫分
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCallScoreUser;						//叫分玩家
	BYTE							cbCurrentScore;						//当前叫分
	BYTE							cbUserCallScore;					//上次叫分
	BYTE							cbTimeCallScore;					//叫分时间
};

//庄家信息
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//庄家玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分
	BYTE							cbTimeHeadOutCard;					//首出时间
	BYTE							cbBankerCard[3];					//庄家扑克
	BYTE							cbMultiple;							//加倍时间
	SCORE							lEnterScore;						//进入积分
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	int								lMultiple;							//倍数
	bool							bSysOut;							//托管系统出牌
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//一轮结束
	WORD				 			wCurrentUser;						//当前玩家
	WORD				 			wPassCardUser;						//放弃玩家
	bool							bSysOut;							//托管系统出牌
	BYTE							cbTimeOutCard;						//出牌时间
};

//游戏结束
struct CMD_S_GameConclude
{
	//积分变量
	SCORE							lCellScore;							//单元积分
	SCORE							lGameScore[GAME_PLAYER];			//游戏积分

	//春天标志
	BYTE							bChunTian;							//春天标志
	BYTE							bFanChunTian;						//春天标志

	//炸弹信息
	BYTE							cbBombCount;						//炸弹个数
	BYTE							cbEachBombCount[GAME_PLAYER];		//炸弹个数

	//游戏信息
	BYTE							cbBankerScore;						//叫分数目
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbHandCardData[FULL_COUNT];			//扑克列表

	LONG							lAllMultiple;						//总倍数

	LONG							lChunTianMultiple;					//春天倍数
	LONG							lFanChunTianMultiple;				//反春天倍数
	LONG							lBombMultiple;						//炸弹倍数

	// 火箭信息
	LONG							lRocketMultiple;					//火箭倍数
	BYTE							cbTimeStartGame;					//开始时间

	//特殊提示
	bool							bSpecialTip;						// 特殊提示
	BYTE							bMultiple[GAME_PLAYER];			//是否加倍
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};
//发送其他人的扑克数据
struct CMD_S_Other_CardData
{
	BYTE							cbCardDataList[GAME_PLAYER][MAX_COUNT];			//扑克列表
};

//用户加倍
struct CMD_S_Multiple
{
	bool							bIsMultiple;							//是否加倍
	WORD							wChairID;							//加倍/不加倍用户
	BYTE							cbBankerScore;						//庄家叫分
};

// 开始出牌
struct CMD_S_START_OUTCARD
{
	BYTE							cbTimeHeadOutCard;					//首出时间
	WORD				 			wBankerUser;						//庄家玩家
};
//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_CALL_SCORE			1									//用户叫分
#define SUB_C_OUT_CARD				2									//用户出牌
#define SUB_C_PASS_CARD				3									//用户放弃
#define SUB_C_TRUSTEE				4									//用户托管
#define SUB_C_MULTIPLE				5									//加倍
#define SUB_S_ANDROIDUP				11									//机器人起立
//用户叫分
struct CMD_C_CallScore
{
	BYTE							cbCallScore;						//叫分数目
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[MAX_COUNT];				//扑克数据
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//用户加倍
struct CMD_C_Multiple
{
	bool							bIsMultiple;						//是否加倍
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif