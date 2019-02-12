#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//用户事件
#define	DBR_GR_LOGON_USERID			100									//I D 登录
#define	DBR_GR_LOGON_MOBILE			101									//手机登录
#define	DBR_GR_LOGON_ACCOUNTS		102									//帐号登录
#define DBR_GR_LOGON_MATCH			103									//比赛登录

//系统事件
#define DBR_GR_WRITE_GAME_SCORE		200									//游戏积分
#define DBR_GR_LEAVE_GAME_SERVER	201									//离开房间
#define DBR_GR_GAME_SCORE_RECORD	202									//积分记录
#define DBR_GR_MANAGE_USER_RIGHT	203									//权限管理
#define DBR_GR_LOAD_SYSTEM_MESSAGE	204									//系统消息
#define DBR_GR_LOAD_SENSITIVE_WORDS	205									//敏感词	
#define DBR_GR_UNLOCK_ANDROID_USER	206									//解锁机器人

//配置事件
#define DBR_GR_LOAD_PARAMETER		300									//加载参数
#define DBR_GR_LOAD_GAME_COLUMN		301									//加载列表
#define DBR_GR_LOAD_ANDROID_PARAMETER 302								//加载参数
#define DBR_GR_LOAD_ANDROID_USER	303									//加载机器

#define DBR_GR_LOAD_GROWLEVEL_CONFIG 305								//等级配置
#define DBR_GR_LOAD_GAME_BUFF		306									//加载Buff （如：1小时双倍积分）
#define DBR_GR_LOAD_CHECKIN_REWARD	307 								//加载签到
#define DBR_GR_LOAD_BASEENSURE		308 								//加载低保 
#define DBR_GR_LOAD_TRUMPET			309									//加载喇叭
#define DBR_GR_LOAD_MEMBER_PARAMETER 310								//会员参数

//银行事件
#define DBR_GR_USER_ENABLE_INSURE	400									//开通银行
#define DBR_GR_USER_SAVE_SCORE		401									//存入游戏币
#define DBR_GR_USER_TAKE_SCORE		402									//提取游戏币
#define DBR_GR_USER_TRANSFER_SCORE	403									//转帐游戏币
#define DBR_GR_QUERY_INSURE_INFO	404									//查询银行
#define DBR_GR_QUERY_TRANSFER_USER_INFO	    405							//查询用户

//游戏事件
#define DBR_GR_GAME_FRAME_REQUEST	502									//游戏请求
#define DBR_GR_WRITE_USER_GAME_DATA	503									//用户游戏数据

//比赛事件
#define DBR_GR_MATCH_SIGNUP			600									//报名比赛
#define DBR_GR_MATCH_UNSIGNUP		601									//退出比赛
#define DBR_GR_MATCH_SIGNUP_START	602									//开始报名
#define DBR_GR_MATCH_QUERY_QUALIFY	603									//查询资格
#define DBR_GR_MATCH_BUY_SAFECARD	604									//购买保险卡	
#define DBR_GR_MATCH_START			605									//比赛开始
#define DBR_GR_MATCH_OVER			606									//比赛结束
#define DBR_GR_MATCH_CANCEL			607									//比赛取消
#define DBR_GR_MATCH_REWARD			608									//比赛奖励
#define DBR_GR_MATCH_ELIMINATE      609									//比赛淘汰
#define DBR_GR_MATCH_QUERY_REVIVE	610									//查询复活
#define DBR_GR_MATCH_USER_REVIVE	611									//用户复活
#define DBR_GR_MATCH_RECORD_GRADES	612									//记录成绩

//任务事件
#define DBR_GR_TASK_TAKE			700									//领取任务
#define DBR_GR_TASK_REWARD			701									//领取奖励
#define DBR_GR_TASK_LOAD_LIST		702									//加载任务
#define DBR_GR_TASK_QUERY_INFO		703									//查询任务
#define DBR_GR_TASK_GIVEUP			704									//放弃任务

//兑换事件
#define DBR_GR_PURCHASE_MEMBER		801									//购买会员
#define DBR_GR_EXCHANGE_SCORE_INGOT	802									//兑换游戏币
#define DBR_GR_EXCHANGE_SCORE_BEANS	803									//兑换游戏币

//等级命令
#define DBR_GR_GROWLEVEL_QUERY_IFNO	 820								//查询等级

//道具事件
#define DBR_GR_LOAD_PROPERTY		900									//加载道具
#define DBR_GR_GAME_PROPERTY_BUY	901									//购买道具
#define DBR_GR_QUERY_BACKPACK		902									//背包道具
#define DBR_GR_PROPERTY_USE			903									//使用道具
#define DBR_GR_QUERY_SEND_PRESENT	904									//查询赠送
#define DBR_GR_PROPERTY_PRESENT		905									//赠送道具
#define DBR_GR_GET_SEND_PRESENT		906									//获取赠送
#define DBR_GR_SEND_TRUMPET			907									//使用喇叭

//签到事件
#define DBR_GR_CHECKIN_DONE			1000								//执行签到
#define DBR_GR_CHECKIN_QUERY_INFO	1001								//查询信息

//低保事件
#define DBR_GR_BASEENSURE_TAKE		1100								//领取低保

//会员事件
#define DBR_GR_MEMBER_QUERY_INFO	1200								//会员查询
#define DBR_GR_MEMBER_DAY_PRESENT	1201								//会员送金
#define DBR_GR_MEMBER_DAY_GIFT		1202								//会员礼包

//私人房间
#define DBR_GR_WRITE_PERSONAL_GAME_SCORE	1300					//写私人房游戏积分
//私人房间
#define DBR_GR_CREATE_TABLE			1301									//创建桌子
#define DBR_GR_CANCEL_CREATE_TABLE	1302									//取消创建
#define DBR_GR_LOAD_PERSONAL_PARAMETER	1303								//私人配置

#define DBR_GR_INSERT_CREATE_RECORD	1304									//插入创建记录
#define DBR_GR_DISSUME_ROOM		1305									//解散房间
#define DBR_GR_QUERY_USER_ROOM_SCORE	1306								//玩家请求房间成绩
#define DBO_GR_LOAD_PERSONAL_ROOM_OPTION		1307				//私人配置
#define DBR_GR_HOST_CANCEL_CREATE_TABLE	1308									//取消创建

#define DBR_GR_WRITE_JOIN_INFO	1309									//取消创建


#define DBR_GR_WRITE_ROOM_TASK_PROGRESS		1500					//推荐约战任务
//////////////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_LOGON_SUCCESS		100									//登录成功
#define DBO_GR_LOGON_FAILURE		101									//登录失败

//配置事件
#define DBO_GR_GAME_PARAMETER		200									//配置信息
#define DBO_GR_GAME_COLUMN_INFO		201									//列表信息
#define DBO_GR_GAME_ANDROID_PARAMETER 202								//参数信息
#define DBO_GR_GAME_ANDROID_INFO	203									//机器信息
#define DBO_GR_GAME_UNLOCK_ANDROID	204									//解锁机器

#define DBO_GR_GAME_MEMBER_PAREMETER 206								//会员参数
#define DBO_GR_GAME_PROPERTY_BUFF	207									//道具BUFF
#define DBO_GR_CHECKIN_REWARD		208									//签到奖励
#define DBO_GR_BASEENSURE_PARAMETER	209									//低保信息
#define DBO_GR_LOAD_TRUMPET			210									//喇叭数量
#define DBO_GR_SEND_TRUMPET			211									//发送喇叭
#define DBO_GR_GAME_PROPERTY_ITEM	220									//道具信息

//银行命令
#define DBO_GR_USER_INSURE_INFO		300									//银行资料
#define DBO_GR_USER_INSURE_SUCCESS	301									//银行成功
#define DBO_GR_USER_INSURE_FAILURE	302									//银行失败
#define DBO_GR_USER_INSURE_USER_INFO   303								//用户资料
#define DBO_GR_USER_INSURE_ENABLE_RESULT 304							//开通结果

//游戏事件
#define DBO_GR_PROPERTY_SUCCESS		400									//道具成功
#define DBO_GR_PROPERTY_FAILURE		401									//道具失败
#define DBO_GR_GAME_FRAME_RESULT	402									//游戏结果

//比赛事件
#define DBO_GR_MATCH_EVENT_START	500									//比赛标识
#define DBO_GR_MATCH_SIGNUP_RESULT	501									//报名结果
#define DBO_GR_MATCH_UNSIGNUP_RESULT 502								//退赛结果
#define DBO_GR_MATCH_QUERY_RESULT	503									//查询结果
#define DBO_GR_MATCH_BUY_RESULT		504									//购买结果
#define DBO_GR_MATCH_REVIVE_INFO	505									//复活信息
#define DBO_GR_MATCH_REVIVE_RESULT	506									//复活结果		
#define DBO_GR_MATCH_RANK_LIST		507									//比赛排行
#define DBO_GR_MATCH_REWARD_RESULT  508									//奖励结果
#define DBO_GR_MATCH_CANCEL_RESULT	509									//比赛取消
#define DBO_GR_MATCH_EVENT_END		599									//比赛标识

//系统事件
#define DBO_GR_SYSTEM_MESSAGE_RESULT	600								//消息结果
#define DBO_GR_SYSTEM_MESSAGE_FINISH	601								//消息完成	
#define DBO_GR_SENSITIVE_WORDS		    602								//敏感词

//任务事件
#define DBO_GR_TASK_LIST			700									//任务列表
#define DBO_GR_TASK_LIST_END		701									//任务结束
#define DBO_GR_TASK_INFO			702									//任务信息
#define DBO_GR_TASK_RESULT			703									//任务结果

//兑换事件
#define DBO_GR_MEMBER_PARAMETER     800									//会员参数
#define DBO_GR_PURCHASE_RESULT		801									//购买结果
#define DBO_GR_EXCHANGE_RESULT		802									//兑换结果

//等级命令
#define DBO_GR_GROWLEVEL_CONFIG		820									//等级配置
#define DBO_GR_GROWLEVEL_PARAMETER	821									//等级参数
#define DBO_GR_GROWLEVEL_UPGRADE	822									//等级升级


//道具事件
#define DBO_GR_LOAD_GAME_PROPERTY	910									//加载道具
#define DBO_GR_GAME_PROPERTY_BUY	911									//购买道具
#define DBO_GR_QUERY_BACKPACK		912									//查询背包
#define DBO_GR_PROPERTY_USE			913									//道具使用
#define DBO_GR_QUERY_SEND_PRESENT	914									//查询赠送
#define DBO_GR_PROPERTY_PRESENT		915									//赠送道具
#define DBO_GR_GET_SEND_PRESENT		916									//获取赠送
#define DBO_GR_GAME_PROPERTY_FAILURE 920									//道具失败
//签到事件
#define DBO_GR_CHECKIN_INFO			1000								//签到信息
#define DBO_GR_CHECKIN_RESULT		1001								//签到结果

#define DBO_GR_BASEENSURE_RESULT	1100								//领取结果

//会员事件
#define DBO_GR_MEMBER_QUERY_INFO_RESULT				1201					//查询结果
#define DBO_GR_MEMBER_DAY_PRESENT_RESULT			1202					//送金结果
#define DBO_GR_MEMBER_DAY_GIFT_RESULT				1203					//礼包结果

//私人房间
#define DBO_GR_CREATE_SUCCESS		1300									//创建成功
#define DBO_GR_CREATE_FAILURE		1301									//创建失败
#define DBO_GR_CANCEL_CREATE_RESULT	1302						//取消创建
#define DBO_GR_LOAD_PERSONAL_PARAMETER		1303				//私人配置
#define DBO_GR_DISSUME_TABLE_RESULTE			1304				//解散房间结果
#define DBO_GR_CURRENCE_ROOMCARD_AND_BEAN			1305				//解散房间结果




//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GR_LogonUserID
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛编号
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//ID 登录
struct DBR_GR_LogonMobile
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//附加信息
	BYTE							cbPersonalServer;					//私人房间
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛编号
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//帐号登录
struct DBR_GR_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//游戏积分
struct DBR_GR_WriteGameScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//进出索引

	//附件信息
	bool							bTaskForward;						//任务跟进

	//成绩变量
	tagVariationInfo				VariationInfo;						//提取信息

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
};

//离开房间
struct DBR_GR_LeaveGameServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwInoutIndex;						//记录索引
	DWORD							dwLeaveReason;						//离开原因
	DWORD							dwOnLineTimeCount;					//在线时长
	byte									cbIsPersonalRoom;				//是否为私人房间

	//成绩变量
	tagVariationInfo				RecordInfo;							//记录信息
	tagVariationInfo				VariationInfo;						//提取信息

	//系统信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//游戏记录
struct DBR_GR_GameScoreRecord
{
	//桌子信息
	WORD							wTableID;							//桌子号码
	WORD							wUserCount;							//用户数目
	WORD							wAndroidCount;						//机器数目

	//损耗税收
	SCORE							lWasteCount;						//损耗数目
	SCORE							lRevenueCount;						//税收数目

	//统计信息
	DWORD							dwUserMemal;						//奖牌数目
	DWORD							dwPlayTimeCount;					//游戏时间

	//时间信息
	SYSTEMTIME						SystemTimeStart;					//开始时间
	SYSTEMTIME						SystemTimeConclude;					//结束时间

	//积分记录
	WORD							wRecordCount;						//记录数目
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//游戏记录
};

//开通银行
struct DBR_GR_UserEnableInsure
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D	
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//存入游戏币
struct DBR_GR_UserSaveScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出游戏币
struct DBR_GR_UserTakeScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转帐游戏币
struct DBR_GR_UserTransferScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址	
	SCORE							lTransferScore;						//转帐游戏币
	TCHAR							szAccounts[LEN_NICKNAME];			//目标用户
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//转帐备注
};

//加载机器
struct DBR_GR_LoadAndroidUser
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwAndroidCount;						//机器数目
	DWORD							AndroidCountMember0;				//普通会员	
	DWORD							AndroidCountMember1;				//一级会员
	DWORD							AndroidCountMember2;				//二级会员
	DWORD							AndroidCountMember3;				//三级会员
	DWORD							AndroidCountMember4;				//四级会员
	DWORD							AndroidCountMember5;				//五级会员
};

//查询银行
struct DBR_GR_QueryInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
};

//查询用户
struct DBR_GR_QueryTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
	DWORD							dwUserID;							//用户 I D
};

//道具购买
struct DBR_GR_PropertyBuy
{
	DWORD							dwUserID;							//购买对象
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwItemCount;						//道具数目
	BYTE							cbConsumeType;						//积分消费
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//道具请求
struct DBR_GR_PropertyRequest
{
	//购买信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	DWORD							dwSourceUserID;						//购买对象
	DWORD							dwTargetUserID;						//使用对象

	//消费模式
	BYTE                            cbRequestArea;			            //使用范围 
	BYTE							cbConsumeScore;						//积分消费
	SCORE							lFrozenedScore;						//冻结积分

	//用户权限
	DWORD                           dwUserRight;						//会员权限

	//系统信息
	WORD							wTableID;							//桌子号码
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询背包
struct DBR_GR_QueryBackpack
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwKindID;							//道具类型

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//道具使用
struct DBR_GR_PropertyUse
{
	//使用信息
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	DWORD							dwClientAddr;						//连接地址
};

//查询资格
struct DBR_GR_QueryMatchQualify
{	
	DWORD							dwUserID;							//用户ID
	DWORD							dwMatchID;							//比赛ID
	SCORE							lMatchNo;							//比赛场次
};

//购买保险
struct DBR_GR_MatchBuySafeCard
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwMatchID;							//比赛ID
	SCORE							lMatchNo;							//比赛场次	
	SCORE							lSafeCardFee;						//保险费用		
};

//比赛报名
struct DBR_GR_MatchSignup
{
	//报名费用
	DWORD							dwUserID;							//用户ID	
	SCORE							lInitScore;							//初始分数
	BYTE							cbSignupMode;						//报名方式
	WORD							wMaxSignupUser;						//最大人数

	//系统信息
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwMatchID;							//比赛ID
	SCORE							lMatchNo;							//比赛场次
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取消报名
struct DBR_GR_MatchUnSignup
{
	//报名费用
	DWORD							dwReason;							//取消原因
	DWORD							dwUserID;							//用户ID

	//系统信息
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwMatchID;							//比赛ID
	SCORE							lMatchNo;							//比赛场次
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛开始
struct DBR_GR_MatchStart
{
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次	
};

//报名开始
struct DBR_GR_SignupStart
{	
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次	
	BYTE							cbSignupMode;						//报名方式
};

//比赛结束
struct DBR_GR_MatchOver
{	
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	BOOL							bMatchFinish;						//比赛完成
	WORD							wPlayCount;							//游戏局数
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchEndTime;						//结束时间
};

//比赛取消
struct DBR_GR_MatchCancel
{
	BOOL							bMatchFinish;						//比赛完成
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	SCORE							lSafeCardFee;						//保险费用
};

//比赛淘汰
struct DBR_GR_MatchEliminate
{
	DWORD							dwUserID;							//用户标识
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次	
};

//查询复活
struct DBR_GR_MatchQueryRevive
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次		
};

//用户复活
struct DBR_GR_MatchUserRevive
{
	//比赛信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次

	//分数配置
	SCORE							lInitScore;							//初始分数
	SCORE							lCullScore;							//淘汰分数			

	//复活配置
	BYTE							cbReviveTimes;						//复活次数
	SCORE							lReviveFee;							//复活费用	
};

//比赛奖励
struct DBR_GR_MatchReward
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lRewardGold;						//奖励金币
	SCORE							lRewardIngot;						//奖励元宝
	DWORD							dwRewardExperience;					//奖励经验
	DWORD							dwClientAddr;						//连接地址
};

//记录成绩
struct DBR_GR_MatchRecordGrades
{
	//比赛信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwMatchID;							//比赛标识
	WORD							wServerID;							//房间标识
	SCORE							lMatchNo;							//比赛场次
	SCORE							lInitScore;							//初始分数
};

//喇叭发送
struct DBR_GR_Send_Trumpet
{
	//使用信息
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	DWORD							dwClientAddr;						//连接地址
	DWORD                           TrumpetColor;                       //喇叭颜色
	TCHAR							szSendNickName[16];					//玩家昵称
	TCHAR                           szTrumpetContent[TRUMPET_MAX_CHAR]; //喇叭内容
};


//查询赠送
struct DBR_GR_QuerySendPresent
{
	DWORD							dwUserID;							//接收者
	DWORD							dwClientAddr;						//客户端地址
};

//获取赠送
struct DBR_GR_GetSendPresent
{
	DWORD							dwUserID;							//接收者
	DWORD							dwClientAddr;						//客户端地址
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//赠送道具
struct DBR_GR_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvGameID;						//道具给谁(GameID)
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//0 根据玩家昵称  1 根据玩家GameID
	DWORD							dwClientAddr;						//客户端地址
	TCHAR							szRecvNickName[16];					//道具对谁使用（昵称）
};

//查询签到
struct DBR_GR_CheckInQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//执行签到
struct DBR_GR_CheckInDone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取低保
struct DBR_GR_TakeBaseEnsure
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询任务
struct DBR_GR_TaskQueryInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//放弃任务
struct DBR_GR_TaskGiveUp
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//领取任务
struct DBR_GR_TaskTake
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取奖励
struct DBR_GR_TaskReward
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询等级
struct DBR_GR_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//购买会员
struct DBR_GR_PurchaseMember
{
	DWORD							dwUserID;							//用户 I D
	BYTE							cbMemberOrder;						//会员标识
	WORD							wPurchaseTime;						//购买时间

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换游戏币
struct DBR_GR_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//用户标识
	SCORE							lExchangeIngot;						//兑换元宝

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换游戏币
struct DBR_GR_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//用户标识
	double							dExchangeBeans;						//兑换游戏豆

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员查询
struct DBR_GR_MemberQueryInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员赠送
struct DBR_GR_MemberDayPresent
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员礼包
struct DBR_GR_MemberDayGift
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//用户游戏数据
struct DBR_GR_WriteUserGameData
{
	//用户信息
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szUserGameData[LEN_GAME_DATA];		//游戏数据
};

//////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GR_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团 I D
	DWORD							dwCustomID;							//自定头像
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lIngot;								//用户元宝
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行	
	DOUBLE							dBeans;								//用户游戏豆

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D

	//用户权限
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限

	//索引变量
	DWORD							dwInoutIndex;						//记录索引
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识

	//任务变量
	WORD							wTaskID;							//任务标识
	BYTE							cbTaskStatus;						//任务状态
	DWORD							dwTaskProgress;						//任务进度	
	SYSTEMTIME						TaskTakeTime;						//领取时间

	//手机定义
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//辅助信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szDescribeString[128];				//错误消息
	TCHAR							szUserGameData[LEN_GAME_DATA];		//游戏数据

	//任务信息
	WORD							wTaskCount;							//任务数目
	tagUserTaskInfo					UserTaskInfo[TASK_MAX_COUNT];		//任务信息
};

//登录失败
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//配置信息
struct DBO_GR_GameParameter
{
	//汇率信息
	DWORD							dwMedalRate;						//奖牌汇率
	DWORD							dwRevenueRate;						//银行税收
	DWORD							dwExchangeRate;						//兑换比率
	DWORD							dwPresentExchangeRate;				//魅力游戏币兑换率
	DWORD							dwRateGold;							//游戏豆游戏币兑换率

	//经验奖励
	DWORD							dwWinExperience;					//赢局经验

	//版本信息
	DWORD							dwClientVersion;					//客户版本
	DWORD							dwServerVersion;					//服务版本

	SCORE							lEducateGrantScore;					//练习赠送
};

//列表信息
struct DBO_GR_GameColumnInfo
{
	LONG							lResultCode;						//结果代码
	BYTE							cbColumnCount;						//列表数目
	tagColumnItem					ColumnItemInfo[MAX_COLUMN];			//列表信息
};

//机器信息
struct DBO_GR_GameAndroidParameter
{
	LONG							lResultCode;						//结果代码
	WORD							wParameterCount;					//参数数目
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//机器信息
};

//机器信息
struct DBO_GR_GameAndroidInfo
{
	LONG							lResultCode;						//结果代码
	DWORD							dwBatchID;							//批次标识
	WORD							wAndroidCount;						//用户数目
	tagAndroidAccountsInfo			AndroidAccountsInfo[MAX_ANDROID];	//机器帐号
};
//////////////////////////////////////////////////////////////////////////
//道具节点
struct DBO_GR_GamePropertyItem
{
	//道具信息
	WORD							wIndex;								//道具标识
	WORD							wKind;								//功能类型
	WORD							wUseArea;							//发布范围
	WORD							wServiceArea;						//服务范围
	WORD							wRecommend;							//推荐标识
	WORD							cbSuportMobile;						//支持手机

	//销售价格
	SCORE							lPropertyGold;						//道具金币
	DOUBLE							dPropertyCash;						//道具价格
	SCORE							lPropertyUserMedal;					//道具金币
	SCORE							lPropertyLoveLiness;				//道具金币

	//赠送财富
	SCORE							lSendLoveLiness;					//赠送魅力
	SCORE							lRecvLoveLiness;					//接受魅力
	SCORE							lUseResultsGold;					//获得金币
	DWORD							wUseResultsValidTime;				//有效时间
	WORD							wUseResultsValidTimeScoreMultiple;	//有效倍率
	DWORD							wUseResultsGiftPackage;				//礼物拆包


	TCHAR							szName[PROPERTY_LEN_NAME];			//道具名称
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//使用信息
};

//加载结果
struct DBO_GR_GamePropertyListResult
{
	BYTE							cbSuccess;							//成功标志
};

//道具信息
struct DBO_GR_GamePropertyInfo
{
	LONG							lResultCode;						//结果代码
	BYTE							cbPropertyCount;					//道具数目
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//道具信息
};

//购买结果
struct DBO_GR_PropertyBuyResult
{
	//购买信息
	DWORD							dwUserID;							//用户 I D
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwItemCount;						//道具数目
	LONGLONG						lInsureScore;						//银行存款
	LONGLONG						lUserMedal;							//用户元宝
	LONGLONG						lLoveLiness;						//魅力值
	double							dCash;								//游戏豆
	BYTE							cbCurrMemberOrder;					//会员等级
	TCHAR							szNotifyContent[128];				//提示内容
};

//道具失败
struct DBO_GR_PropertyFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述信息
};


struct DBO_GR_PropertyUse
{
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	WORD							wUseArea;							//发布范围
	WORD							wServiceArea;						//服务范围
	DWORD							dwPropID;							//道具ID
	DWORD							wPropCount;							//使用数目
	DWORD							dwRemainderPropCount;				//剩余数量
	SCORE							Score;								//游戏金币

	SCORE							lSendLoveLiness;					//赠送魅力
	SCORE							lRecvLoveLiness;					//接受魅力
	SCORE							lUseResultsGold;					//获得金币					
	DWORD							dwPropKind;							//道具类型
	time_t							tUseTime;							//使用的时间
	DWORD							UseResultsValidTime;				//有效时长(秒)
	DWORD							dwHandleCode;						//处理结果
	TCHAR							szName[16];							//道具名称
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szNotifyContent[128];				//提示内容
};

//玩家道具Buff
struct DBO_GR_UserGamePropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff数目
	tagPropertyBuff					PropertyBuff[MAX_BUFF];			
};

//玩家喇叭
struct DBO_GR_UserGameTrumpet
{
	DWORD							dwTrumpetCount;						//小喇叭数
	DWORD							dwTyphonCount;						//大喇叭数		
};

//玩家发送喇叭
struct DBO_GR_Send_Trumpet
{
	//使用信息
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	DWORD                           TrumpetColor;                       //喇叭颜色
	BOOL							bSuccful;							//成功发送
	TCHAR							szSendNickName[16];					//玩家昵称
	TCHAR                           szTrumpetContent[TRUMPET_MAX_CHAR]; //喇叭内容
};

//查询赠送
struct DBO_GR_QuerySendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];		
};

//获取赠送
struct DBO_GR_GetSendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];											
};

//赠送道具
struct DBO_GR_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvUserID;						//道具给谁(UserID)
	DWORD							dwRecvGameID;						//道具给谁(GameID)
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//0 根据玩家昵称  1 根据玩家GameID
	TCHAR							szRecvNickName[16];					//道具对谁使用（昵称）
	int								nHandleCode;						//返回码
	TCHAR							szNotifyContent[64];				//提示内容
};

//签到奖励
struct DBO_GR_CheckInReward
{
	SCORE							lRewardGold[LEN_WEEK];				//奖励金额
};

//签到信息
struct DBO_GR_CheckInInfo
{
	WORD							wSeriesDate;						//连续日期
	bool							bTodayChecked;						//签到标识
};

//签到结果
struct DBO_GR_CheckInResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前分数
	TCHAR							szNotifyContent[128];				//提示内容
};

//低保参数
struct DBO_GR_BaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//低保结果
struct DBO_GR_BaseEnsureResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//银行资料
struct DBO_GR_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbEnjoinTransfer;					//转帐开关
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wRevenueTransferMember;				//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转帐条件
	DWORD							dwUserRight;						//用户权限
};

//银行成功
struct DBO_GR_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lFrozenedScore;						//冻结积分
	SCORE							lVariationScore;					//游戏币变化
	SCORE							lVariationInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct DBO_GR_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //游戏动作
	LONG							lResultCode;						//操作代码
	SCORE							lFrozenedScore;						//冻结积分
	TCHAR							szDescribeString[128];				//描述消息
};

//用户资料
struct DBO_GR_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwGameID;							//用户 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//帐号昵称
};

//开通结果
struct DBO_GR_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbInsureEnabled;					//使能标识
	TCHAR							szDescribeString[128];				//描述消息
};

//道具成功
struct DBO_GR_S_PropertySuccess
{
	//道具信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	WORD							wServiceArea;
	WORD							wPropKind;							//功能类型
	DWORD							dwSourceUserID;						//购买对象
	DWORD							dwTargetUserID;						//使用对象

	//消费模式
	BYTE                            cbRequestArea;						//请求范围
	BYTE							cbConsumeScore;						//积分消费
	SCORE							lFrozenedScore;						//冻结积分

	//会员权限
	DWORD                           dwUserRight;						//会员权限

	//结果信息
	SCORE							lConsumeGold;						//消费游戏币
	SCORE							lSendLoveLiness;					//赠送魅力
	SCORE							lRecvLoveLiness;					//接受魅力

	//会员信息
	BYTE							cbMemberOrder;						//会员等级
};

//任务列表
struct DBO_GR_TaskListInfo
{
	WORD							wTaskCount;							//任务数目
};

//任务信息
struct DBO_GR_TaskInfo
{
	WORD							wTaskCount;							//任务数量
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//任务状态
};

//签到结果
struct DBO_GR_TaskResult
{
	//结果信息
	bool							bSuccessed;							//成功标识
	WORD							wCommandID;							//命令标识
	WORD							wCurrTaskID;						//当前任务

	//财富信息
	SCORE							lCurrScore;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	
	//提示信息
	TCHAR							szNotifyContent[128];				//提示内容
};

//等级配置
struct DBO_GR_GrowLevelConfig
{
	WORD							wLevelCount;						//等级数目
	tagGrowLevelConfig				GrowLevelConfig[60];				//等级配置
};

//等级参数
struct DBO_GR_GrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级	
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//等级升级
struct DBO_GR_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szNotifyContent[128];				//升级提示
};

//会员参数
struct DBO_GR_MemberParameter
{
	WORD							wMemberCount;						//会员数目
	tagMemberParameterNew			MemberParameter[64];				//会员参数
};

//会员查询
struct DBO_GR_MemberQueryInfoResult
{
	bool							bPresent;							//会员送金
	bool							bGift;								//会员礼包
	DWORD							GiftSubCount;						//道具数量
	tagGiftPropertyInfo				GiftSub[50];						//包含道具
};

//会员赠送
struct DBO_GR_MemberDayPresentResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//会员礼包
struct DBO_GR_MemberDayGiftResult
{
	bool							bSuccessed;							//成功标识
	TCHAR							szNotifyContent[128];				//提示内容
};

//购买结果
struct DBO_GR_PurchaseResult
{
	bool							bSuccessed;							//成功标识
	BYTE							cbMemberOrder;						//会员系列
	DWORD							dwUserRight;						//用户权限
	SCORE							lCurrScore;							//当前游戏币
	DOUBLE							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};
 
//兑换结果
struct DBO_GR_ExchangeResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrInsure;						//当前银行
	SCORE							lCurrIngot;							//当前元宝
	double							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};

//报名结果
struct DBO_GR_MatchSingupResult
{
	bool							bResultCode;						//结果代码
	BYTE							cbSignupMode;						//报名方式
	DWORD							dwReason;							//取消原因
	DWORD							dwUserID;							//用户标识
	SCORE							lMatchNo;							//比赛场次
	SCORE							lCurrGold;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szDescribeString[128];				//描述消息
};

//查询结果
struct DBO_GR_MatchQueryResult
{
	bool							bHoldQualify;						//拥有资格
};

//购买结果
struct DBO_GR_MatchBuyResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrGold;							//当前金币
	TCHAR							szDescribeString[128];				//描述消息
};

//复活信息
struct DBO_GR_MatchReviveInfo
{	
	bool							bSuccessed;							//成功标识
	bool							bSafeCardRevive;					//保险卡复活
	BYTE							cbReviveTimesed;					//复活次数
};

//复活结果
struct DBO_GR_MatchReviveResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrGold;							//当前金币	
	TCHAR							szDescribeString[128];				//描述消息
};

//比赛排行
struct DBO_GR_MatchRankList
{
	BOOL							bMatchFinish;						//比赛完成
	WORD							wUserCount;							//用户数目
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	tagMatchRankInfo				MatchRankInfo[128];					//奖励信息
};

//奖励结果
struct DBO_GR_MatchRewardResult
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrGold;							//当前金币
};

//取消结果
struct DBO_GR_MatchCancelResult
{
	bool							bSuccessed;							//成功标识
	BOOL							bMatchFinish;						//比赛完成
};

//权限管理
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限
	bool							bGameRight;							//游戏权限
};

//权限管理
struct DBR_GR_ManageMatchRight
{	
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限	
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//系统消息
struct DBR_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //消息ID
	BYTE							cbMessageType;						//消息类型
	BYTE                            cbAllRoom;                          //全体房间
	DWORD							dwTimeRate;						    //时间频率
	__time64_t						tStartTime;							//开始时间
	__time64_t                      tConcludeTime;                      //结束时间
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};

//解锁机器人
struct DBR_GR_UnlockAndroidUser
{
	WORD							wServerID;							//房间ID
	WORD							wBatchID;							//批次ID
};

//游戏积分
struct DBR_GR_WritePersonalGameScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//进出索引

	//附件信息
	bool							bTaskForward;						//任务跟进

	//成绩变量
	tagVariationInfo				VariationInfo;						//提取信息

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
	
	//房主ID
	DWORD							dwRoomHostID;				//房主ID

	//私人房税收
	DWORD							dwPersonalRoomTax;			//私人房税收

	//房间ID
	TCHAR							szRoomID[ROOM_ID_LEN];				//房间ID

	//房间唯一标识
	TCHAR							szPersonalRoomGUID[PERSONAL_ROOM_GUID];				//房间唯一ID

	//视屏id,预留字段，目前无用
	char								szVideoNumber[LEN_VIDEO_NUMBER];				//视频ID

	//游戏局数
	int								nGamesNum;							//游戏局数

};

//创建桌子
struct DBR_GR_CreateTable
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//用户地址
	LONG								lCellScore;							//设置底分
	DWORD							dwServerID;							//房间标识
	DWORD							dwTableID;							//预分配桌号
	WORD								wJoinGamePeopleCount;						//参与游戏的人数
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	DWORD							dwRoomTax;								//单独一个私人房间的税率，千分比
	TCHAR							szPassword[LEN_PASSWORD];			//密码设置
	BYTE									cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	//其他位游戏自定义规则， 规则协议由游戏服务端和客户端商定
};


//
struct DBR_GR_CancelCreateTable
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwServerID;							//用户标识
	DWORD							dwClientAddr;						//用户地址
	DWORD							dwTableID;							//桌子 I D
	DWORD							dwReason;							//取消原因
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	TCHAR								szRoomID[ROOM_ID_LEN];
};

//推进任务
struct DBR_GR_PerformRoomTaskProgress
{
	DWORD							dwUserID;							//用户标识
	WORD							wKindID;							//游戏标志
	INT								nCreateRoomCount;					//创建房间数
	INT								nUseCardCount;						//使用房卡数
	INT								nDrawCount;							//约战局数
};


//创建成功
struct DBO_GR_CreateSuccess
{
	DWORD							dwUserID;							//用户标识
	LONG							lCellScore;							//设置底分
	DOUBLE							dCurBeans;							//当前游戏豆
	LONGLONG						lRoomCard;							//当前房卡
	DWORD							dwTableID;							//预分配桌号
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	WORD							wJoinGamePeopleCount;				//参与游戏的人数
	DWORD							dwRoomTax;							//单独一个私人房间的税率，千分比
	BYTE							cbIsJoinGame;						//玩家是否参与游戏
	TCHAR							szPassword[LEN_PASSWORD];			//密码设置
	TCHAR							szClientAddr[16];					//创建房间的IP地址
	BYTE							cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表设置 1 代表未设置
	//其他位游戏自定义规则， 规则协议由游戏服务端和客户端商定
	INT								iRoomCardFee;						//房卡消耗	
};

//创建失败
struct DBO_GR_CreateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//取消创建
struct DBO_GR_CancelCreateResult
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwTableID;							//桌子 I D
	DWORD							dwReason;							//取消原因

	LONG							lErrorCode;							//错误代码	
	TCHAR							szDescribeString[128];				//描述消息							
};


//解散结果
struct DBO_GR_DissumeResult
{
	DWORD					dwSocketID;							//网络标识
	DWORD					dwUserID;								//玩家ID
	BYTE							cbIsDissumSuccess;					//是否解散成功
	TCHAR						szRoomID[ROOM_ID_LEN];		//桌子 I D			
	SYSTEMTIME				sysDissumeTime;						//解散时间	
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//解散结果
struct DBO_GR_CurrenceRoomCardAndBeans
{
	DOUBLE					dbBeans;							//网络标识
	SCORE					lRoomCard;								//玩家ID
};

//私人玩家房参与信息
struct DBR_GR_WriteJoinInfo
{
	DWORD							dwUserID;							//用户标识
	WORD							wTableID;							//桌子 I D
	WORD							wChairID;							//椅子ID
	WORD							wKindID;							//类型ID
	TCHAR							szRoomID[ROOM_ID_LEN];				//房间标识
	TCHAR							szPeronalRoomGUID[PERSONAL_ROOM_GUID];				//房间唯一标识
};



//////////////////////////////////////////////////////////////////////////////////

#endif