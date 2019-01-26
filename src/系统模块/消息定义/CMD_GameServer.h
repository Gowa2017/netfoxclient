#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//登录命令

#define MDM_GR_LOGON				1									//登录信息

//登录模式
#define SUB_GR_LOGON_USERID			1									//I D 登录
#define SUB_GR_LOGON_MOBILE			2									//手机登录
#define SUB_GR_LOGON_ACCOUNTS		3									//帐户登录

//登录结果
#define SUB_GR_LOGON_SUCCESS		100									//登录成功
#define SUB_GR_LOGON_FAILURE		101									//登录失败
#define SUB_GR_LOGON_FINISH			102									//登录完成

//升级提示
#define SUB_GR_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////

//I D 登录
struct CMD_GR_LogonUserID
{
	//版本信息
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//进程版本

	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //房间密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	WORD							wKindID;							//类型索引
};

//手机登录
struct CMD_GR_LogonMobile
{
	//版本信息
	WORD							wGameID;							//游戏标识
	DWORD							dwProcessVersion;					//进程版本

	//桌子区域
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //房间密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//帐号登录
struct CMD_GR_LogonAccounts
{
	//版本信息
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//进程版本

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//登录成功
struct CMD_GR_LogonSuccess
{
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限
};

//登录失败
struct CMD_GR_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登录完成
struct CMD_GR_LogonFinish
{
	bool							bGuideTask;							//引领任务
};

//升级提示
struct CMD_GR_UpdateNotify
{
	//升级标志
	BYTE							cbMustUpdatePlaza;					//强行升级
	BYTE							cbMustUpdateClient;					//强行升级
	BYTE							cbAdviceUpdateClient;				//建议升级

	//当前版本
	DWORD							dwCurrentPlazaVersion;				//当前版本
	DWORD							dwCurrentFrameVersion;				//当前版本
	DWORD							dwCurrentClientVersion;				//当前版本
};

//////////////////////////////////////////////////////////////////////////////////
//配置命令

#define MDM_GR_CONFIG				2									//配置信息

#define SUB_GR_CONFIG_COLUMN		100									//列表配置
#define SUB_GR_CONFIG_SERVER		101									//房间配置
#define SUB_GR_CONFIG_PROPERTY		102									//道具配置
#define SUB_GR_CONFIG_FINISH		103									//配置完成
#define SUB_GR_CONFIG_USER_RIGHT	104									//玩家权限

//////////////////////////////////////////////////////////////////////////////////

//列表配置
struct CMD_GR_ConfigColumn
{
	BYTE							cbColumnCount;						//列表数目
	tagColumnItem					ColumnItem[MAX_COLUMN];				//列表描述
};

//房间配置
struct CMD_GR_ConfigServer
{
	//房间属性
	WORD							wTableCount;						//桌子数目
	WORD							wChairCount;						//椅子数目

	//房间配置
	WORD							wServerType;						//房间类型
	DWORD							dwServerRule;						//房间规则
};


//玩家权限
struct CMD_GR_ConfigUserRight
{
	DWORD							dwUserRight;						//玩家权限
};
//////////////////////////////////////////////////////////////////////////////////
//用户命令

#define MDM_GR_USER					3									//用户信息

//用户动作
#define SUB_GR_USER_RULE			1									//用户规则
#define SUB_GR_USER_LOOKON			2									//旁观请求
#define SUB_GR_USER_SITDOWN			3									//坐下请求
#define SUB_GR_USER_STANDUP			4									//起立请求
#define SUB_GR_USER_INVITE			5									//用户邀请
#define SUB_GR_USER_INVITE_REQ		6									//邀请请求
#define SUB_GR_USER_REPULSE_SIT  	7									//拒绝玩家坐下
#define SUB_GR_USER_KICK_USER       8                                   //踢出用户
#define SUB_GR_USER_INFO_REQ        9                                   //请求用户信息
#define SUB_GR_USER_CHAIR_REQ       10                                  //请求更换位置
#define SUB_GR_USER_CHAIR_INFO_REQ  11                                  //请求椅子用户信息
#define SUB_GR_USER_WAIT_DISTRIBUTE 12									//等待分配

//用户状态
#define SUB_GR_USER_ENTER			100									//用户进入
#define SUB_GR_USER_SCORE			101									//用户分数
#define SUB_GR_USER_STATUS			102									//用户状态
#define SUB_GR_REQUEST_FAILURE		103									//请求失败
#define SUB_GR_USER_GAME_DATA		104									//用户游戏数据

//聊天命令
#define SUB_GR_USER_CHAT			201									//聊天消息
#define SUB_GR_USER_EXPRESSION		202									//表情消息
#define SUB_GR_WISPER_CHAT			203									//私聊消息
#define SUB_GR_WISPER_EXPRESSION	204									//私聊表情
#define SUB_GR_COLLOQUY_CHAT		205									//会话消息
#define SUB_GR_COLLOQUY_EXPRESSION	206									//会话表情

//等级服务
#define SUB_GR_GROWLEVEL_QUERY			410								//查询等级
#define SUB_GR_GROWLEVEL_PARAMETER		411								//等级参数
#define SUB_GR_GROWLEVEL_UPGRADE		412								//等级升级
//////////////////////////////////////////////////////////////////////////////////

//旁观请求
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
};

//坐下请求
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码
};

//起立请求
struct CMD_GR_UserStandUp
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
	BYTE							cbForceLeave;						//强行离开
};

//邀请用户 
struct CMD_GR_UserInvite
{
	WORD							wTableID;							//桌子号码
	DWORD							dwUserID;							//用户 I D
};

//邀请用户请求 
struct CMD_GR_UserInviteReq
{
	WORD							wTableID;							//桌子号码
	DWORD							dwUserID;							//用户 I D
};

//用户分数
struct CMD_GR_UserScore
{
	DWORD							dwUserID;							//用户标识
	tagUserScore					UserScore;							//积分信息
};

//用户分数
struct CMD_GR_MobileUserScore
{
	DWORD							dwUserID;							//用户标识
	tagMobileUserScore				UserScore;							//积分信息
};

//用户状态
struct CMD_GR_UserStatus
{
	DWORD							dwUserID;							//用户标识
	tagUserStatus					UserStatus;							//用户状态
};

//用户游戏数据
struct CMD_GR_UserGameData
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szUserGameData[LEN_GAME_DATA];		//游戏数据					
};

//请求失败
struct CMD_GR_RequestFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[256];				//描述信息
};


//用户聊天
struct CMD_GR_C_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户聊天
struct CMD_GR_S_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户表情
struct CMD_GR_C_UserExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
};

//用户表情
struct CMD_GR_S_UserExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
};

//用户私聊
struct CMD_GR_C_WisperChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户私聊
struct CMD_GR_S_WisperChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//私聊表情
struct CMD_GR_C_WisperExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
};

//私聊表情
struct CMD_GR_S_WisperExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
};

//用户会话
struct CMD_GR_ColloquyChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwConversationID;					//会话标识
	DWORD							dwTargetUserID[16];					//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//邀请用户
struct CMD_GR_C_InviteUser
{
	WORD							wTableID;							//桌子号码
	DWORD							dwSendUserID;						//发送用户
};

//邀请用户
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//目标用户
};

//用户拒绝黑名单坐下
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子位置
	DWORD							dwUserID;							//用户 I D
	DWORD							dwRepulseUserID;					//用户 I D
};

//////////////////////////////////////////////////////////////////////////////////

//规则标志
#define UR_LIMIT_SAME_IP			0x01								//限制地址
#define UR_LIMIT_WIN_RATE			0x02								//限制胜率
#define UR_LIMIT_FLEE_RATE			0x04								//限制逃率
#define UR_LIMIT_GAME_SCORE			0x08								//限制积分

//用户规则
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//规则掩码
	WORD							wMinWinRate;						//最低胜率
	WORD							wMaxFleeRate;						//最高逃率
	LONG							lMaxGameScore;						//最高分数 
	LONG							lMinGameScore;						//最低分数
};

//请求用户信息
struct CMD_GR_UserInfoReq
{
	DWORD                           dwUserIDReq;                        //请求用户
	WORD							wTablePos;							//桌子位置
};

//请求用户信息
struct CMD_GR_ChairUserInfoReq
{
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子位置
};

//////////////////////////////////////////////////////////////////////////////////
//等级服务

//查询等级
struct CMD_GR_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//附加信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//等级配置
struct CMD_GR_GrowLevelConfig
{
	WORD							wLevelCount;						//等级数目
	tagGrowLevelConfig				GrowLevelItem[60];					//等级配置
};

//等级参数
struct CMD_GR_GrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//等级升级
struct CMD_GR_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////
//状态命令

#define MDM_GR_STATUS				4									//状态信息

#define SUB_GR_TABLE_INFO			100									//桌子信息
#define SUB_GR_TABLE_STATUS			101									//桌子状态

//////////////////////////////////////////////////////////////////////////////////

//桌子信息
struct CMD_GR_TableInfo
{
	WORD							wTableCount;						//桌子数目
	tagTableStatus					TableStatusArray[512];				//桌子状态
};

//桌子状态
struct CMD_GR_TableStatus
{
	WORD							wTableID;							//桌子号码
	tagTableStatus					TableStatus;						//桌子状态
};

//////////////////////////////////////////////////////////////////////////////////
//银行命令

#define MDM_GR_INSURE				5									//用户信息

//银行命令
#define SUB_GR_ENABLE_INSURE_REQUEST 1									//开通银行
#define SUB_GR_QUERY_INSURE_INFO	2									//查询银行
#define SUB_GR_SAVE_SCORE_REQUEST	3									//存款操作
#define SUB_GR_TAKE_SCORE_REQUEST	4									//取款操作
#define SUB_GR_TRANSFER_SCORE_REQUEST	5								//取款操作
#define SUB_GR_QUERY_USER_INFO_REQUEST	6								//查询用户

#define SUB_GR_USER_INSURE_INFO		100									//银行资料
#define SUB_GR_USER_INSURE_SUCCESS	101									//银行成功
#define SUB_GR_USER_INSURE_FAILURE	102									//银行失败
#define SUB_GR_USER_TRANSFER_USER_INFO	103								//用户资料
#define SUB_GR_USER_INSURE_ENABLE_RESULT 104							//开通结果

//////////////////////////////////////////////////////////////////////////////////

//开通银行
struct CMD_GR_C_EnableInsureRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户I D
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询银行
struct CMD_GR_C_QueryInsureInfoRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
};

//存款请求
struct CMD_GR_C_SaveScoreRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	SCORE							lSaveScore;							//存款数目
};

//取款请求
struct CMD_GR_C_TakeScoreRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	SCORE							lTakeScore;							//取款数目
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
};

//转账金币
struct CMD_GP_C_TransferScoreRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	SCORE							lTransferScore;						//转账金币
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//转账备注
};

//查询用户
struct CMD_GR_C_QueryUserInfoRequest
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
};

//银行资料
struct CMD_GR_S_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbEnjoinTransfer;					//转账开关
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wRevenueTransferMember;				//税收比例	
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	SCORE							lTransferPrerequisite;				//转账条件
};

//银行成功
struct CMD_GR_S_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //游戏动作
	SCORE							lUserScore;							//身上金币
	SCORE							lUserInsure;						//银行金币
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct CMD_GR_S_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //游戏动作
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//用户信息
struct CMD_GR_S_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwTargetGameID;						//目标用户
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
};

//开通结果
struct CMD_GR_S_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbInsureEnabled;					//使能标识
	TCHAR							szDescribeString[128];				//描述消息
};

//////////////////////////////////////////////////////////////////////////////////
//任务命令
#define MDM_GR_TASK					6									//任务命令

#define SUB_GR_TASK_LOAD_INFO		1									//加载任务
#define SUB_GR_TASK_TAKE			2									//领取任务
#define SUB_GR_TASK_REWARD			3									//任务奖励
#define SUB_GR_TASK_GIVEUP			4									//任务放弃


#define SUB_GR_TASK_INFO			11									//任务信息
#define SUB_GR_TASK_FINISH			12									//任务完成
#define SUB_GR_TASK_LIST			13									//任务列表
#define SUB_GR_TASK_RESULT			14									//任务结果
#define SUB_GR_TASK_GIVEUP_RESULT	15									//放弃结果

//////////////////////////////////////////////////////////////////////////////////
//加载任务
struct CMD_GR_C_LoadTaskInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//放弃任务
struct CMD_GR_C_TakeGiveUp
{
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取任务
struct CMD_GR_C_TakeTask
{
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取奖励
struct CMD_GR_C_TaskReward
{
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//任务信息
struct CMD_GR_S_TaskInfo
{
	WORD							wTaskCount;							//任务数量
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//任务状态
};

//任务完成
struct CMD_GR_S_TaskFinish
{
	WORD							wFinishTaskID;						//任务标识
	TCHAR							szTaskName[LEN_TASK_NAME];			//任务名称
};

//任务结果
struct CMD_GR_S_TaskResult
{
	//结果信息
	bool							bSuccessed;							//成功标识
	WORD							wCommandID;							//命令标识

	//财富信息
	SCORE							lCurrScore;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	
	//提示信息
	TCHAR							szNotifyContent[128];				//提示内容
};


//////////////////////////////////////////////////////////////////////////////////
//兑换命令

#define MDM_GR_EXCHANGE					7								//兑换命令

#define SUB_GR_EXCHANGE_LOAD_INFO		400								//加载信息
#define SUB_GR_EXCHANGE_PARAM_INFO		401								//兑换参数
#define SUB_GR_PURCHASE_MEMBER			402								//购买会员
#define SUB_GR_PURCHASE_RESULT			403								//购买结果
#define SUB_GR_EXCHANGE_SCORE_BYINGOT	404								//兑换游戏币
#define SUB_GR_EXCHANGE_SCORE_BYBEANS	405								//兑换游戏币
#define SUB_GR_EXCHANGE_RESULT			406								//兑换结果

//////////////////////////////////////////////////////////////////////////////////

//查询参数
struct CMD_GR_ExchangeParameter
{
	DWORD							dwExchangeRate;						//元宝游戏币兑换比率
	DWORD							dwPresentExchangeRate;				//魅力游戏币兑换率
	DWORD							dwRateGold;							//游戏豆游戏币兑换率
	WORD							wMemberCount;						//会员数目
	tagMemberParameter				MemberParameter[10];				//会员参数
};

//购买会员
struct CMD_GR_PurchaseMember
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbMemberOrder;						//会员标识
	WORD							wPurchaseTime;						//购买时间
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//购买结果
struct CMD_GR_PurchaseResult
{
	bool							bSuccessed;							//成功标识
	BYTE							cbMemberOrder;						//会员系列
	SCORE							lCurrScore;							//当前游戏币
	DOUBLE							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};

//兑换游戏币
struct CMD_GR_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//用户标识
	SCORE							lExchangeIngot;						//兑换数量
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//兑换游戏币
struct CMD_GR_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//用户标识
	double							dExchangeBeans;						//兑换数量
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//兑换结果
struct CMD_GR_ExchangeResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	double							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////
//道具命令
#define MDM_GR_PROPERTY				8									//道具命令

//道具信息
#define SUB_GR_QUERY_PROPERTY					1						//道具查询
#define SUB_GR_GAME_PROPERTY_BUY				2						//购买道具
#define SUB_GR_PROPERTY_BACKPACK				3						//背包道具
#define SUB_GR_PROPERTY_USE						4						//物品使用
#define SUB_GR_QUERY_SEND_PRESENT				5						//查询赠送
#define SUB_GR_PROPERTY_PRESENT					6						//赠送道具
#define SUB_GR_GET_SEND_PRESENT					7						//获取赠送

#define SUB_GR_QUERY_PROPERTY_RESULT			101						//道具查询
#define SUB_GR_QUERY_PROPERTY_RESULT_FINISH		111						//道具查询
#define SUB_GR_GAME_PROPERTY_BUY_RESULT			102						//购买道具
#define SUB_GR_PROPERTY_BACKPACK_RESULT			103						//背包道具
#define SUB_GR_PROPERTY_USE_RESULT				104						//物品使用
#define SUB_GR_QUERY_SEND_PRESENT_RESULT		105						//查询赠送
#define SUB_GR_PROPERTY_PRESENT_RESULT			106						//赠送道具
#define SUB_GR_GET_SEND_PRESENT_RESULT			107						//获取赠送

#define SUB_GR_PROPERTY_SUCCESS					201						//道具成功
#define SUB_GR_PROPERTY_FAILURE					202						//道具失败
#define SUB_GR_PROPERTY_MESSAGE					203                     //道具消息
#define SUB_GR_PROPERTY_EFFECT					204                     //道具效应
#define SUB_GR_PROPERTY_TRUMPET					205                     //喇叭消息
#define SUB_GR_USER_PROP_BUFF					206						//道具Buff
#define SUB_GR_USER_TRUMPET						207						//喇叭数量

#define SUB_GR_GAME_PROPERTY_FAILURE			404						//道具失败


//道具失败
struct CMD_GR_GamePropertyFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述信息
};

//购买道具
struct CMD_GR_PropertyBuy
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwItemCount;						//道具数目
	BYTE							cbConsumeType;						//积分消费
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//购买结果
struct CMD_GR_PropertyBuyResult
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

//购买道具
struct CMD_GR_C_GamePropertyUse
{
	//BYTE                            cbRequestArea;					//请求范围
	BYTE							cbConsumeType;						//积分消费
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引	
	DWORD							dwUserID;							//使用对象
};



//道具成功
struct CMD_GR_S_PropertySuccess
{
	BYTE                            cbRequestArea;						//使用环境
	WORD							wKind;								//功能类型
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	DWORD							dwSourceUserID;						//目标对象
	DWORD							dwTargetUserID;						//使用对象
};

//道具失败
struct CMD_GR_PropertyFailure
{
	WORD                            wRequestArea;                       //请求区域
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[256];				//描述信息
};

//道具消息
struct CMD_GR_S_PropertyMessage
{
	//道具信息
	WORD                            wPropertyIndex;                     //道具索引
	WORD                            wPropertyCount;                     //道具数目
	DWORD                           dwSourceUserID;                     //目标对象
	DWORD                           dwTargerUserID;                     //使用对象
};

//背包结果
struct DBO_GR_QueryBackpack
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwStatus;							//状态标识
	DWORD							dwCount;							//个数
	tagBackpackProperty				PropertyInfo[1];					//道具信息
};


//道具效应
struct CMD_GR_S_PropertyEffect
{
	DWORD                           wUserID;					        //用 户I D
	BYTE							wPropKind;							//道具类型
	BYTE							cbMemberOrder;						//会员等级
	DWORD							dwFleeCount;						//逃跑局数
	SCORE							lScore;								//负分清零
};

//发送喇叭
struct CMD_GR_C_SendTrumpet
{
	BYTE                           cbRequestArea;                       //请求范围 
	WORD                           wPropertyIndex;                      //道具索引 
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
};

//发送喇叭
struct CMD_GR_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //道具索引 
	DWORD                          dwSendUserID;                        //用户 I D
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szSendNickName[32];				    //玩家昵称 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
};

//背包道具
struct CMD_GR_C_BackpackProperty
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwKindID;							//道具类型
};

//背包道具
struct CMD_GR_S_BackpackProperty
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwStatus;							//状态
	DWORD							dwCount;							//个数
	tagBackpackProperty				PropertyInfo[1];					//道具信息
};

//使用道具
struct CMD_GR_C_PropertyUse
{
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//对谁使用
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
};

//使用道具
struct CMD_GR_S_PropertyUse
{
	DWORD							dwUserID;							//使用者
	WORD							wUseArea;							//发布范围
	WORD							wServiceArea;						//服务范围
	DWORD							dwRecvUserID;						//对谁使用
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

//道具Buff
struct CMD_GR_S_PropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff数目
	tagPropertyBuff					PropertyBuff[MAX_BUFF];	
};

//玩家喇叭
struct CMD_GR_S_UserTrumpet
{
	DWORD							dwTrumpetCount;						//小喇叭数
	DWORD							dwTyphonCount;						//大喇叭数	
};

//查询赠送
struct CMD_GR_C_QuerySendPresent
{
	DWORD							dwUserID;							//用户 I D
};

//查询赠送
struct CMD_GR_S_QuerySendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];		
};

//获取赠送
struct CMD_GR_C_GetSendPresent
{
	DWORD							dwUserID;							//赠送者
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//获取赠送
struct CMD_GR_S_GetSendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];	
};

//赠送道具
struct CMD_GR_C_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvGameID;						//道具给谁
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//目标类型
	TCHAR							szRecvNickName[16];					//对谁使用
};

//赠送道具
struct CMD_GR_S_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvGameID;						//道具给谁
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//目标类型
	TCHAR							szRecvNickName[16];					//对谁使用
	int								nHandleCode;						//返回码
	TCHAR							szNotifyContent[64];				//提示内容
};




//////////////////////////////////////////////////////////////////////////////////
//管理命令

#define MDM_GR_MANAGE				9									//管理命令

#define SUB_GR_SEND_WARNING			1									//发送警告
#define SUB_GR_SEND_MESSAGE			2									//发送消息
#define SUB_GR_LOOK_USER_IP			3									//查看地址
#define SUB_GR_KILL_USER			4									//踢出用户
#define SUB_GR_LIMIT_ACCOUNS		5									//禁用帐户
#define SUB_GR_SET_USER_RIGHT		6									//权限设置

//房间设置
#define SUB_GR_QUERY_OPTION			7									//查询设置
#define SUB_GR_OPTION_SERVER		8									//房间设置
#define SUB_GR_OPTION_CURRENT		9									//当前设置
#define SUB_GR_LIMIT_USER_CHAT		10									//限制聊天
#define SUB_GR_KICK_ALL_USER		11									//踢出用户
#define SUB_GR_DISMISSGAME		    12									//解散游戏

//////////////////////////////////////////////////////////////////////////////////

//发送警告
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//信息长度
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//警告消息
};

//系统消息
struct CMD_GR_SendMessage
{
	BYTE							cbGame;								//是否将消息广播至游戏
	BYTE							cbRoom;								//是否将消息广播至房间（桌子界面）
	BYTE							cbAllRoom;							//是否将消息广播至所有游戏
	BYTE                            cbLoop;                             //循环标志
	DWORD                           dwTimeRate;                         //循环间隔
	__time64_t                      tConcludeTime;                      //结束时间
	WORD							wChatLength;						//信息长度
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};

//查看地址
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//目标用户
};

//踢出用户
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//目标用户
};

//禁用帐户
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//目标用户
};

//权限设置
struct CMD_GR_SetUserRight
{
	//目标用户
	DWORD							dwTargetUserID;						//目标用户

	//绑定变量
	BYTE							cbGameRight;						//帐号权限
	BYTE							cbAccountsRight;					//帐号权限

	//权限变化
	BYTE							cbLimitRoomChat;					//大厅聊天
	BYTE							cbLimitGameChat;					//游戏聊天
	BYTE							cbLimitPlayGame;					//游戏权限
	BYTE							cbLimitSendWisper;					//发送消息
	BYTE							cbLimitLookonGame;					//旁观权限
};

//房间设置
struct CMD_GR_OptionCurrent
{
	DWORD							dwRuleMask;							//规则掩码
	tagServerOptionInfo				ServerOptionInfo;					//房间配置
};

//房间设置
struct CMD_GR_ServerOption
{
	tagServerOptionInfo				ServerOptionInfo;					//房间配置
};

//踢出所有用户
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//踢出提示
};

//解散游戏
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //解散桌号
};
//////////////////////////////////////////////////////////////////////////////////

//设置标志
#define OSF_ROOM_CHAT				1									//大厅聊天
#define OSF_GAME_CHAT				2									//游戏聊天
#define OSF_ROOM_WISPER				3									//大厅私聊
#define OSF_ENTER_TABLE				4									//进入游戏
#define OSF_ENTER_SERVER			5									//进入房间
#define OSF_SEND_BUGLE				12									//发送喇叭

//房间设置
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//设置标志
	BYTE							cbOptionValue;						//设置标志
};

//限制聊天
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//目标用户
	BYTE							cbLimitFlags;						//限制标志
	BYTE							cbLimitValue;						//限制与否
};

//////////////////////////////////////////////////////////////////////////////////
//比赛命令

#define MDM_GR_MATCH				10									//比赛命令

#define SUB_GR_MATCH_INFO			1									//比赛信息	
#define SUB_GR_MATCH_RULE			2									//比赛规则
#define SUB_GR_MATCH_FEE			3									//报名费用
#define SUB_GR_MATCH_NUM			4									//等待人数
#define SUB_GR_LEAVE_MATCH			5									//退出比赛
#define SUB_GR_MATCH_SAFECARD		6									//保险提醒
#define SUB_GR_MATCH_BUY_SAFECARD	7									//购买保险
#define SUB_GR_MATCH_WAIT_TIP		8									//等待提示
#define SUB_GR_MATCH_RESULT			9									//比赛结果
#define SUB_GR_MATCH_STATUS			10									//比赛状态
#define SUB_GR_MATCH_USTATUS		11									//比赛状态	
#define SUB_GR_MATCH_DESC			12									//比赛描述
#define SUB_GR_MATCH_GOLDUPDATE		13									//金币更新
#define SUB_GR_MATCH_ELIMINATE		14									//比赛淘汰
#define SUB_GR_MATCH_REVIVE			15									//复活提醒
#define SUB_GR_MATCH_USER_REVIVE	16									//用户复活


////////////////////////////////////////////////////////////////////////////////////////

//比赛规则
struct CMD_GR_Match_Rule
{
	//基本信息
	DWORD							dwMatchID;							//比赛标识	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称

	//报名信息
	BYTE							cbFeeType;							//费用类型
	BYTE							cbDeductArea;						//缴费区域
	LONGLONG						lSignupFee;							//报名费用	
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	DWORD							dwExperience;						//玩家经验

	//排名方式
	BYTE							cbRankingMode;						//排名方式	
	WORD							wCountInnings;						//统计局数
	BYTE							cbFilterGradesMode;					//筛选方式
	
	//奖励信息
	WORD							wRewardCount;						//奖励人数
	tagMatchRewardInfo				MatchRewardInfo[3];					//比赛奖励
	//比赛规则
	BYTE							cbMatchRule[512];					//比赛规则
};

//复活提醒
struct CMD_GR_Match_Revive
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNO;							//比赛场次

	//复活信息
	LONGLONG						lReviveFee;							//复活费用
	BYTE							cbReviveTimes;						//复活次数
	BYTE							cbReviveTimesed;					//复活次数
	TCHAR							szNotifyContent[128];				//提示内容
};

//保险提醒
struct CMD_GR_Match_SafeCard
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNO;							//比赛场次

	//保险费用
	LONGLONG						lSafeCardFee;						//保险费用
	TCHAR							szNotifyContent[128];				//提示内容
};

//购买保险
struct CMD_GR_Match_BuySafeCard
{
	bool							bBuySafeCard;						//购买标识
};

//费用提醒
struct CMD_GR_Match_Fee
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNO;							//比赛场次

	//报名信息
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbFeeType;							//费用类型
	BYTE							cbDeductArea;						//扣费区域
	LONGLONG						lSignupFee;							//报名费用
	TCHAR							szNotifyContent[128];				//提示内容
};

//比赛人数
struct CMD_GR_Match_Num
{
	DWORD							dwWaitting;							//等待人数
	DWORD							dwTotal;							//开赛人数
};

//赛事信息
struct CMD_GR_Match_Info
{
	TCHAR							szTitle[4][64];						//信息标题
    WORD							wGameCount;							//游戏局数
};

//提示信息
struct CMD_GR_Match_Wait_Tip
{
	LONGLONG						lScore;								//当前积分
	WORD							wRank;								//当前名次
	WORD							wCurTableRank;						//本桌名次
	WORD							wUserCount;							//当前人数
	WORD							wCurGameCount;						//当前局数
	WORD							wGameCount;							//总共局数
	WORD							wPlayingTable;						//游戏桌数
	TCHAR							szMatchName[LEN_SERVER];			//比赛名称
};

//比赛结果
struct CMD_GR_MatchResult
{	
	//奖励信息
	SCORE							lGold;								//金币奖励
	SCORE							lIngot;								//元宝奖励
	DWORD							dwExperience;						//经验奖励
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称

	//比赛信息
	WORD							wMatchRank;							//比赛名次
	TCHAR							szMatchName[64];					//比赛名称	
	SYSTEMTIME						MatchEndTime;						//结束时间	
};

#define MAX_MATCH_DESC				4									//最多描述

//比赛描述
struct CMD_GR_MatchDesc
{
	TCHAR							szTitle[MAX_MATCH_DESC][16];		//信息标题
	TCHAR							szDescribe[MAX_MATCH_DESC][64];		//描述内容
	COLORREF						crTitleColor;						//标题颜色
	COLORREF						crDescribeColor;					//描述颜色
};

//金币更新
struct CMD_GR_MatchGoldUpdate
{
	LONGLONG						lCurrGold;							//当前金币
};

//用户复活
struct CMD_GR_Match_UserRevive
{
	bool							bUseSafeCard;						//使用保险卡
};

//////////////////////////////////////////////////////////////////////////

//低保命令
#define MDM_GR_BASEENSURE				11

#define SUB_GR_BASEENSURE_LOAD			260								//加载低保
#define SUB_GR_BASEENSURE_TAKE			261								//领取低保
#define SUB_GR_BASEENSURE_PARAMETER		262								//低保参数
#define SUB_GR_BASEENSURE_RESULT		263								//低保结果

//领取低保
struct CMD_GR_BaseEnsureTake
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//低保参数
struct CMD_GR_BaseEnsureParamter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//低保结果
struct CMD_GR_BaseEnsureResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};
//////////////////////////////////////////////////////////////////////////
//签到命令
#define MDM_GR_CHECKIN					12
#define SUB_GR_CHECKIN_QUERY			220								//查询签到
#define SUB_GR_CHECKIN_INFO				221								//签到信息
#define SUB_GR_CHECKIN_DONE				222								//执行签到
#define SUB_GR_CHECKIN_RESULT			223								//签到结果

//查询签到
struct CMD_GR_CheckInQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//签到信息
struct CMD_GR_CheckInInfo
{	
	WORD							wSeriesDate;						//连续日期
	bool							bTodayChecked;						//签到标识
	SCORE							lRewardGold[LEN_WEEK];				//奖励金币	
};

//执行签到
struct CMD_GR_CheckInDone
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//签到结果
struct CMD_GR_CheckInResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前金币
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////
//会员命令
#define MDM_GR_MEMBER					13
#define SUB_GR_MEMBER_PARAMETER			340								//会员参数
#define SUB_GR_MEMBER_QUERY_INFO		341								//会员查询
#define SUB_GR_MEMBER_DAY_PRESENT		342								//会员送金
#define SUB_GR_MEMBER_DAY_GIFT			343								//会员礼包
#define SUB_GR_MEMBER_PARAMETER_RESULT	350								//参数结果
#define SUB_GR_MEMBER_QUERY_INFO_RESULT	351								//查询结果
#define SUB_GR_MEMBER_DAY_PRESENT_RESULT	352							//送金结果
#define SUB_GR_MEMBER_DAY_GIFT_RESULT	353								//礼包结果

//会员查询
struct CMD_GR_MemberQueryInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询结果
struct CMD_GR_MemberQueryInfoResult
{
	bool							bPresent;							//送金结果
	bool							bGift;								//礼物结果
	DWORD							GiftSubCount;						//道具数量
	tagGiftPropertyInfo				GiftSub[50];						//包含道具
};

//会员送金
struct CMD_GR_MemberDayPresent
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//送金结果
struct CMD_GR_MemberDayPresentResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//会员礼包
struct CMD_GR_MemberDayGift
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//礼包结果
struct CMD_GR_MemberDayGiftResult
{
	bool							bSuccessed;							//成功标识
	TCHAR							szNotifyContent[128];				//提示内容
};


//////////////////////////////////////////////////////////////////////////////////
//框架命令

#define MDM_GF_FRAME				100									//框架命令

//////////////////////////////////////////////////////////////////////////////////
//框架命令

//用户命令
#define SUB_GF_GAME_OPTION			1									//游戏配置
#define SUB_GF_USER_READY			2									//用户准备
#define SUB_GF_LOOKON_CONFIG		3									//旁观配置
#define SUB_GF_ANDEOID_QUIT			4									//机器人退出
//聊天命令
#define SUB_GF_USER_CHAT			10									//用户聊天
#define SUB_GF_USER_EXPRESSION		11									//用户表情
#define SUB_GF_USER_VOICE			12									//用户语音

//游戏信息
#define SUB_GF_GAME_STATUS			100									//游戏状态
#define SUB_GF_GAME_SCENE			101									//游戏场景
#define SUB_GF_LOOKON_STATUS		102									//旁观状态
#define SUB_GF_USER_DATA			103									//玩家数据
//系统消息
#define SUB_GF_SYSTEM_MESSAGE		200									//系统消息
#define SUB_GF_ACTION_MESSAGE		201									//动作消息
#define SUB_GF_PERSONAL_MESSAGE		202									//私人房消息


//////////////////////////////////////////////////////////////////////////////////

//游戏配置
struct CMD_GF_GameOption
{
	BYTE							cbAllowLookon;						//旁观标志
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwClientVersion;					//游戏版本
};

//旁观配置
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbAllowLookon;						//允许旁观
};

//旁观状态
struct CMD_GF_LookonStatus
{
	BYTE							cbAllowLookon;						//允许旁观
};

//游戏环境
struct CMD_GF_GameStatus
{
	BYTE							cbGameStatus;						//游戏状态
	BYTE							cbAllowLookon;						//旁观标志
};
//游戏环境
struct CMD_GF_GameUserData
{
	DWORD							cbUserCharID;						//游戏状态
};
//用户聊天
struct CMD_GF_C_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户聊天
struct CMD_GF_S_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户表情
struct CMD_GF_C_UserExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwTargetUserID;						//目标用户
};

//用户表情
struct CMD_GF_S_UserExpression
{
	WORD							wItemIndex;							//表情索引
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
};

//用户语音
struct CMD_GF_C_UserVoice
{
	DWORD							dwTargetUserID;						//目标用户
	DWORD							dwVoiceLength;						//语音长度
	BYTE							byVoiceData[MAXT_VOICE_LENGTH];		//语音数据
};

//用户语音
struct CMD_GF_S_UserVoice
{
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	DWORD							dwVoiceLength;						//语音长度
	BYTE							byVoiceData[MAXT_VOICE_LENGTH];		//语音数据
};

//////////////////////////////////////////////////////////////////////////////////
//游戏命令

#define MDM_GF_GAME					200									//游戏命令

//////////////////////////////////////////////////////////////////////////////////
//携带信息

//其他信息
#define DTP_GR_TABLE_PASSWORD		1									//桌子密码

//用户属性
#define DTP_GR_NICK_NAME			10									//用户昵称
#define DTP_GR_GROUP_NAME			11									//社团名字
#define DTP_GR_UNDER_WRITE			12									//个性签名
#define DTP_GR_QQ					13									//QQ号码
#define DTP_GR_MOBILE_PHONE			14									//手机号码

//附加信息
#define DTP_GR_USER_NOTE			20									//用户备注
#define DTP_GR_CUSTOM_FACE			21									//自定头像

//////////////////////////////////////////////////////////////////////////////////

//请求错误
#define REQUEST_FAILURE_NORMAL		0									//常规原因
#define REQUEST_FAILURE_NOGOLD		1									//金币不足
#define REQUEST_FAILURE_NOSCORE		2									//积分不足
#define REQUEST_FAILURE_PASSWORD	3									//密码错误

//////////////////////////////////////////////////////////////////////////////////
//+++++++++++++++++++++++++++约战房间
//私人房间
#define MDM_GR_PERSONAL_TABLE		210									//私人房间

#define SUB_GR_CREATE_TABLE			1									//创建桌子
#define SUB_GR_CREATE_SUCCESS		2									//创建成功
#define SUB_GR_CREATE_FAILURE		3									//创建失败
#define SUB_GR_CANCEL_TABLE			4									//解散桌子
#define SUB_GR_CANCEL_REQUEST		5									//请求解散
#define SUB_GR_REQUEST_REPLY		6									//请求答复
#define SUB_GR_REQUEST_RESULT		7									//请求结果
#define SUB_GR_WAIT_OVER_TIME		8									//超时等待
#define SUB_GR_PERSONAL_TABLE_TIP	9									//提示信息
#define SUB_GR_PERSONAL_TABLE_END	10							//结束消息
#define SUB_GR_HOSTL_DISSUME_TABLE	11							//房主强制解散桌子
//#define SUB_GR_HOSTL_DISSUME_TABLE	12							//房主强制解散桌子
#define SUB_GR_HOST_DISSUME_TABLE_RESULT	13							//解散桌子
//#define SUB_GR_USER_QUERY_ROOM_SCORE	14							//私人房间单个玩家请求房间成绩
//#define SUB_GR_USER_QUERY_ROOM_SCORE_RESULT	15							//私人房间单个玩家请求房间成绩结果
#define SUB_GR_CURRECE_ROOMCARD_AND_BEAN	16							//解散桌子
//创建桌子
struct CMD_GR_CreateTable
{
	LONGLONG						lCellScore;							    //底分设置
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	WORD								wJoinGamePeopleCount;			//参与游戏的人数
	DWORD							dwRoomTax;								//单独一个私人房间的税率，千分比
	TCHAR								szPassword[LEN_PASSWORD];	//密码设置
	BYTE									cbGameRule[RULE_LEN];				//游戏规则 弟 0 位标识 是否设置规则 0 代表未设置 1 代表设置
	//其他位游戏自定义规则， 规则协议由游戏服务端和客户端商定

};

//创建成功
struct CMD_GR_CreateSuccess
{
	TCHAR								szServerID[7];						//房间编号
	DWORD							dwDrawCountLimit;				//局数限制
	DWORD							dwDrawTimeLimit;				//时间限制
	DOUBLE							dBeans;								//游戏豆
	LONGLONG						lRoomCard;							//房卡数量
};

//创建失败
struct CMD_GR_CreateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//取消桌子
struct CMD_GR_CancelTable
{
	DWORD							dwReason;							//取消原因
	TCHAR							szDescribeString[128];				//描述信息
};

//解散桌子
struct CMD_GR_DissumeTable
{
	BYTE							cbIsDissumSuccess;					//是否解散成功
	TCHAR						szRoomID[ROOM_ID_LEN];				//桌子 I D			
	SYSTEMTIME				sysDissumeTime;						//解散时间	
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//请求解散
struct CMD_GR_CancelRequest
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwTableID;							//桌子 I D
	DWORD							dwChairID;							//椅子 I D
};

//房主强制解散游戏
struct CMD_GR_HostDissumeGame
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwTableID;							//桌子 I D
};




//请求答复
struct CMD_GR_RequestReply
{
	DWORD							dwUserID;							//用户I D
	DWORD							dwTableID;							//桌子 I D
	BYTE							cbAgree;							//用户答复
};

//请求结果
struct CMD_GR_RequestResult
{
	DWORD							dwTableID;							//桌子 I D
	BYTE							cbResult;							//请求结果
};

//超时等待
struct CMD_GR_WaitOverTime
{
	DWORD							dwUserID;							//用户 I D
};

//提示信息
struct CMD_GR_PersonalTableTip
{
	DWORD							dwTableOwnerUserID;					//桌主 I D
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	DWORD							dwPlayCount;						//已玩局数
	DWORD							dwPlayTime;							//已玩时间
	LONGLONG						lCellScore;							//游戏底分
	LONGLONG						lIniScore;								//初始分数
	TCHAR							szServerID[7];						//房间编号
	byte									cbIsJoinGame;					//是否参与游戏
	byte									cbIsGoldOrGameScore;	//金币场还是积分场 0 标识 金币场 1 标识 积分场 
};

//结束消息
struct CMD_GR_PersonalTableEnd
{
	TCHAR							szDescribeString[128];
	LONGLONG					lScore[MAX_CHAIR];
	int								nSpecialInfoLen;			//特殊信息长度
	byte								cbSpecialInfo[SPECIAL_INFO_LEN];//特殊信息数据
};


//私人房间消息
struct Personal_Room_Message
{
	TCHAR szMessage[260];
	BYTE  cbMessageType;
};

//强制解散桌子后的游戏都和房卡
struct CMD_GR_CurrenceRoomCardAndBeans
{
	DOUBLE					dbBeans;							//网络标识
	SCORE					lRoomCard;								//玩家ID
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif