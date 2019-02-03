#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//登录命令
#define	DBR_GP_LOGON_GAMEID			1									//I D 登录
#define	DBR_GP_LOGON_ACCOUNTS		2									//帐号登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define DBR_GP_VERIFY_INDIVIDUAL	4									//验证资料
#define DBR_GP_LOGON_VISITOR		5									//游客登陆

//帐号服务
#define DBR_GP_MODIFY_MACHINE		10									//修改机器
#define DBR_GP_MODIFY_LOGON_PASS	11									//修改密码
#define DBR_GP_MODIFY_INSURE_PASS	12									//修改密码
#define DBR_GP_MODIFY_UNDER_WRITE	13									//修改签名
#define DBR_GP_MODIFY_INDIVIDUAL	14									//修改资料
#define DBR_GP_MODIFY_REAL_AUTH		15									//实名认证

//头像命令
#define DBR_GP_MODIFY_SYSTEM_FACE	20									//修改头像
#define DBR_GP_MODIFY_CUSTOM_FACE	21									//修改头像

//银行命令
#define DBR_GP_USER_ENABLE_INSURE	30									//开通银行
#define DBR_GP_USER_SAVE_SCORE		31									//存入游戏币
#define DBR_GP_USER_TAKE_SCORE		32									//提取游戏币
#define DBR_GP_USER_TRANSFER_SCORE	33									//转帐游戏币
#define DBR_GP_QUERY_BANK_INFO	34										//查询用户绑定银行信息
#define DBR_GP_BIND_BANK_INFO	35										//绑定银行卡信息
#define DBR_GP_EXCHANGE_BANK_SCORE	36										//兑换银行金币

//查询命令
#define DBR_GP_QUERY_INDIVIDUAL		40									//查询资料
#define DBR_GP_QUERY_INSURE_INFO	41									//查询银行
#define DBR_GP_QUERY_USER_INFO	    42									//查询用户
#define DBR_GP_QUERY_TRANSFER_REBATE	43								//查询返利

//系统命令
#define DBR_GP_ONLINE_COUNT_INFO	60									//在线信息
#define DBR_GP_LOAD_GAME_LIST		61									//加载列表
#define DBR_GP_LOAD_CHECKIN_REWARD	62									//签到奖励
#define DBR_GP_LOAD_TASK_LIST		63									//加载任务
#define DBR_GP_LOAD_BASEENSURE		64									//加载低保
#define DBR_GP_LOAD_MEMBER_PARAMETER 66									//会员参数
#define DBR_GP_LOAD_PLATFORM_PARAMETER 67								//平台参数
#define DBR_GP_LOAD_GROWLEVEL_CONFIG 68									//等级配置
#define DBR_GP_LOAD_GAME_PROPERTY_LIST	69								//加载道具


//机器命令
#define DBR_GP_GET_PARAMETER		70									//获取参数
#define DBR_GP_ADD_PARAMETER		71									//添加参数
#define DBR_GP_MODIFY_PARAMETER		72									//修改参数
#define DBR_GP_DELETE_PARAMETER		73									//删除参数

//签到命令
#define DBR_GP_CHECKIN_DONE			80									//执行签到
#define DBR_GP_CHECKIN_QUERY_INFO	81									//查询信息

//任务命令
#define DBR_GP_TASK_TAKE			90									//领取任务
#define DBR_GP_TASK_REWARD			91									//任务奖励
#define DBR_GP_TASK_QUERY_INFO		92									//加载任务	
#define DBR_GP_TASK_GIVEUP			93									//放弃任务

//低保命令
#define DBR_GP_BASEENSURE_TAKE		100									//领取低保

//等级命令
#define DBR_GP_GROWLEVEL_QUERY_IFNO	 110								//查询等级

//兑换命令
#define DBR_GP_PURCHASE_MEMBER		120									//购买会员
#define DBR_GP_EXCHANGE_SCORE_INGOT 121									//兑换游戏币
#define DBR_GP_EXCHANGE_SCORE_BEANS 122									//兑换游戏币

//推广命令
#define DBR_GP_QUERY_SPREAD_INFO	130									//推广信息

//抽奖服务
#define DBR_GP_LOTTERY_CONFIG_REQ	140									//请求配置
#define DBR_GP_LOTTERY_START		141									//开始抽奖

//游戏服务
#define DBR_GP_QUERY_USER_GAME_DATA	150									//游戏数据

//私人房间
#define DBR_MB_CREATE_PERSONAL_TABLE	160								//创建桌子

//认证命令
#define DBR_GP_LOAD_REAL_AUTH		161									//认证信息
#define DBR_MB_QUERY_PERSONAL_ROOM_INFO	162						//请求私人房间信息
#define DBR_GR_QUERY_USER_ROOM_SCORE	163							//玩家请求房间成绩
#define DBR_GR_CLOSE_ROOM_WRITE_DISSUME_TIME	 164				//关闭游戏服务器写私人房的结束时间
#define DBR_MB_GET_PERSONAL_PARAMETER	165							//房间配置
#define DBR_MB_QUERY_PERSONAL_ROOM_USER_INFO	 166			//私人房请求玩家信息
#define DBR_MB_ROOM_CARD_EXCHANGE_TO_SCORE	 167				//房卡兑换游戏币
#define DBR_GP_SERVER_DUMMY_ONLINE	168							//查询在线虚拟人数

//帐号绑定
#define DBR_GP_ACCOUNT_BIND			171									//帐号绑定
#define DBR_GP_ACCOUNT_BIND_EXISTS	172									//帐号绑定

#define DBR_GP_IP_LOCATION			173									//IP归属地

//会员命令
#define DBR_GP_MEMBER_QUERY_INFO	340									//会员查询
#define DBR_GP_MEMBER_DAY_PRESENT	341									//会员送金
#define DBR_GP_MEMBER_DAY_GIFT		342									//会员礼包

//道具命令
#define DBR_GP_PROPERTY_BUY			440									//购买道具
#define DBR_GP_PROPERTY_USE			441									//使用道具
#define DBR_GP_QUERY_BACKPACK		442									//查询背包
#define DBR_GP_USER_PROPERTY_BUFF	443									//加载Buff
#define DBR_GP_PROPERTY_PRESENT		444									//道具赠送
#define DBR_GP_QUERY_SEND_PRESENT	445									//查询赠送
#define DBR_GP_GET_SEND_PRESENT		446									//获取赠送
#define DBR_GP_QUERY_SINGLE			447									//查询单个
//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GP_LogonGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_GP_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//身份证号
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数

	//扩展信息
	DWORD							dwCheckUserRight;					//检查权限
};

//游客登陆
struct DBR_GP_LogonVisitor
{
	//登录信息
	BYTE							cbPlatformID;						//平台编号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号绑定
struct DBR_GP_AccountBind
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	TCHAR							szBindNewAccounts[LEN_ACCOUNTS];	//绑定帐号
	TCHAR							szBindNewPassword[LEN_PASSWORD];	//绑定密码
	TCHAR							szBindNewSpreader[LEN_ACCOUNTS];	//绑定推荐
};

//ip归属地
struct DBR_GP_IpLocation
{
	DWORD							dwUserID;
	TCHAR							szLocation[LEN_IP];	//ip归属地
};

//帐号绑定
struct DBR_GP_AccountBind_Exists
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	TCHAR							szBindExistsAccounts[LEN_ACCOUNTS];	//绑定帐号
	TCHAR							szBindExistsPassword[LEN_PASSWORD];	//绑定密码
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szAgentID[LEN_ACCOUNTS];			//代理标识
	DWORD							dwSpreaderGameID;					//推荐标识

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//验证资料
struct DBR_GP_VerifyIndividual
{
	//连接信息
	LPVOID							pBindParameter;						//绑定参数

	WORD							wVerifyMask;						//校验验证
	TCHAR							szVerifyContent[LEN_ACCOUNTS];		//验证内容
};

//修改机器
struct DBR_GP_ModifyMachine
{
	BYTE							cbBind;								//绑定标志
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改密码
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改签名
struct DBR_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//实名认证
struct DBR_GP_RealAuth
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
};

//修改头像
struct DBR_GP_ModifySystemFace
{
	//用户信息
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct DBR_GP_ModifyCustomFace
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改资料
struct DBR_GP_ModifyIndividual
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//帐号资料
	BYTE							cbGender;							//用户性别
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//用户信息
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//查询资料
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
};

//开通银行
struct DBR_GP_UserEnableInsure
{
	DWORD							dwUserID;							//用户 I D	
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//存入游戏币
struct DBR_GP_UserSaveScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询用户绑定银行信息
struct DBR_GP_QueryBankInfo
{
	DWORD							dwUserID;							//用户 I D
};

//绑定银行
struct DBR_GP_BindBankInfo
{											
	DWORD dwUserID;							//用户id
	TCHAR szRealName[LEN_REAL_NAME];		//真实姓名
	TCHAR szBankNo[LEN_BANK_NO];			//银行卡号
	TCHAR szBankName[LEN_BANK_NAME];		//银行名称
	TCHAR szBankAddress[LEN_BANK_ADDRESS];	//银行名称
	TCHAR szAlipayID[LEN_BANK_ADDRESS];		//支付宝
};

//绑定银行
struct DBR_GP_ExchangeBankScore
{											
	DWORD dwUserID;							//用户id
	SCORE dwScore;		//真实姓名
	TCHAR szBankPass[LEN_PASSWORD];			//银行卡号
	TCHAR szOrderID[LEN_ORDER_ID];		//银行名称
	DWORD dwClientAddr;						//连接地址
	DWORD dwType;							//0 bank 1 alipay
};

//取出游戏币
struct DBR_GP_UserTakeScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转帐游戏币
struct DBR_GP_UserTransferScore
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	SCORE							lTransferScore;						//转帐游戏币
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//转帐备注
};

//查询银行
struct DBR_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//查询用户
struct DBR_GP_QueryInsureUserInfo
{
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
};

//用户资料
struct DBO_GP_UserTransferUserInfo
{
	DWORD							dwGameID;							//用户 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐号
};

//查询返利
struct DBR_GP_QueryTransferRebate
{
	DWORD							dwUserID;							//用户I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//在线信息
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//类型数目
	DWORD							dwOnLineCountSum;					//总在线数
	DWORD							dwAndroidCountSum;					//总在线数
	tagOnLineInfoKindEx				OnLineCountKind[MAX_KIND];			//类型在线
};


//比赛报名
struct DBR_GP_MatchSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛报名
struct DBR_GP_MatchUnSignup
{
	//比赛信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//获取参数
struct DBR_GP_GetParameter
{
	WORD							wServerID;							//房间标识
};

//添加参数
struct DBR_GP_AddParameter
{
	WORD							wServerID;							//房间标识
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//修改参数
struct DBR_GP_ModifyParameter
{
	WORD							wServerID;							//房间标识
	tagAndroidParameter				AndroidParameter;					//机器参数
};

//删除参数
struct DBR_GP_DeleteParameter
{
	WORD							wServerID;							//房间标识
	DWORD							dwBatchID;							//批次标识
};

//查询签到
struct DBR_GP_CheckInQueryInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//执行签到
struct DBR_GP_CheckInDone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询任务
struct DBR_GP_TaskQueryInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//放弃任务
struct DBR_GP_TaskGiveUP
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
struct DBR_GP_TaskTake
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//任务奖励
struct DBR_GP_TaskReward
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取低保
struct DBR_GP_TakeBaseEnsure
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询等级
struct DBR_GP_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员查询
struct DBR_GP_MemberQueryInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员赠送
struct DBR_GP_MemberDayPresent
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//会员礼包
struct DBR_GP_MemberDayGift
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//道具购买
struct DBR_GP_PropertyBuy
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

//道具使用
struct DBR_GP_PropertyUse
{
	//使用信息
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	DWORD							dwClientAddr;						//连接地址
};

//查询单个
struct DBR_GP_PropertyQuerySingle
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwPropertyID;						//道具标识
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//购买会员
struct DBR_GP_PurchaseMember
{
	DWORD							dwUserID;							//用户 I D
	BYTE							cbMemberOrder;						//会员标识
	WORD							wPurchaseTime;						//购买时间

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换游戏币
struct DBR_GP_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//用户标识
	SCORE							lExchangeIngot;						//兑换数量

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换游戏币
struct DBR_GP_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//用户标识
	double							dExchangeBeans;						//兑换数量

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//推广信息
struct DBR_GP_QuerySpreadInfo
{
	DWORD							dwUserID;							//用户标识
};

//////////////////////////////////////////////////////////////////////////////////
//抽奖服务

//开始抽奖
struct DBR_GP_LotteryConfigReq
{
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
};

//开始抽奖
struct DBR_GP_LotteryStart
{
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////////////
//游戏服务
struct DBR_GP_QueryUserGameData
{
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szDynamicPass[LEN_MD5];				//用户密码	
};

//查询背包
struct DBR_GP_QueryBackpack
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwKindID;							//道具类型
	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//道具Buff
struct DBR_GP_UserPropertyBuff
{
	DWORD							dwUserID;							//用户标识
};

//道具赠送
struct DBR_GP_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvGameID;						//道具给谁(GameID)
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//0 根据玩家昵称  1 根据玩家GameID
	DWORD							dwClientAddr;						//客户端地址
	TCHAR							szRecvNickName[16];					//道具对谁使用（昵称）
};

//查询赠送
struct DBR_GP_QuerySendPresent
{
	DWORD							dwUserID;							//接收者
	DWORD							dwClientAddr;						//客户端地址
};

//获取赠送
struct DBR_GP_GetSendPresent
{
	DWORD							dwUserID;							//接收者
	DWORD							dwClientAddr;						//客户端地址
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};


//私人房
//////////////////////////////////////////////////////////////////////////////////
//创建桌子
struct DBR_MB_CreatePersonalTable 
{
	DWORD							dwUserID;							//用户标识

	//房间信息
	DWORD							dwServerID;							//房间I D
	DWORD							dwTableID;							//桌子I D

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//私人房间配置
struct DBR_MB_GetPersonalParameter
{
	DWORD							dwServerID;							//游戏I D
};



//////////////////////////////////////////////////////////////////////////////////
//输出信息

//登录结果
#define DBO_GP_LOGON_SUCCESS		100									//登录成功
#define DBO_GP_LOGON_FAILURE		101									//登录失败
#define DBO_GP_VALIDATE_MBCARD		102									//登录失败
#define DBO_GP_VALIDATE_PASSPORT	103									//登录失败
#define DBO_GP_VERIFY_RESULT		104									//验证结果									

//操作结果
#define DBO_GP_USER_FACE_INFO		110									//用户头像
#define DBO_GP_USER_INDIVIDUAL		111									//用户资料

//银行命令
#define DBO_GP_USER_INSURE_INFO		120									//银行资料
#define DBO_GP_USER_INSURE_SUCCESS	121									//银行成功
#define DBO_GP_USER_INSURE_FAILURE	122									//银行失败
#define DBO_GP_USER_INSURE_USER_INFO 123								//用户资料
#define DBO_GP_USER_INSURE_ENABLE_RESULT 124							//开通结果
#define DBO_GP_QUERY_TRANSFER_REBATE_RESULT	125							//查询结果
#define DBO_GP_QUERY_BANKINFO_SUCCESS 126								//查询绑定银行卡信息

//列表结果
#define DBO_GP_GAME_TYPE_ITEM		130									//种类信息
#define DBO_GP_GAME_KIND_ITEM		131									//类型信息
#define DBO_GP_GAME_NODE_ITEM		132									//节点信息
#define DBO_GP_GAME_PAGE_ITEM		133									//定制信息
#define DBO_GP_GAME_LIST_RESULT		134									//加载结果

//系统结果
#define DBO_GP_PLATFORM_PARAMETER	140									//平台参数
#define DBO_GP_SERVER_DUMMY_ONLINE	141									//房间虚拟人数
#define DBO_GP_SERVER_DUMMY_ONLINE_BEGIN	142							//房间虚拟人数
#define DBO_GP_SERVER_DUMMY_ONLINE_END	143								//房间虚拟人数
//机器命令
#define DBO_GP_ANDROID_PARAMETER	150									//机器参数	

//签到命令
#define DBO_GP_CHECKIN_REWARD		160									//签到奖励
#define DBO_GP_CHECKIN_INFO			161									//签到信息
#define DBO_GP_CHECKIN_RESULT		162									//签到结果

//任务命令
#define DBO_GP_TASK_LIST			170									//任务列表
#define DBO_GP_TASK_LIST_END		171									//列表结束
#define DBO_GP_TASK_INFO			172									//任务信息
#define DBO_GP_TASK_RESULT			173									//任务结果

//低保命令
#define DBO_GP_BASEENSURE_PARAMETER	180									//低保参数
#define DBO_GP_BASEENSURE_RESULT	181									//领取结果

//推广命令
#define DBO_GP_SPREAD_INFO			190									//推广参数

//认证参数
#define DBO_GP_REAL_AUTH_PARAMETER			191									//认证参数

//等级命令
#define DBO_GP_GROWLEVEL_CONFIG		200									//等级配置
#define DBO_GP_GROWLEVEL_PARAMETER	201									//等级参数
#define DBO_GP_GROWLEVEL_UPGRADE	202									//等级升级

//兑换命令
#define DBO_GP_PURCHASE_RESULT		211									//购买结果
#define DBO_GP_EXCHANGE_RESULT		212									//兑换结果
#define DBO_GP_ROOM_CARD_EXCHANGE_RESULT		213			//房卡兑换结果



//抽奖结果
#define DBO_GP_LOTTERY_CONFIG		230									//抽奖配置
#define DBO_GP_LOTTERY_USER_INFO	231									//抽奖信息
#define DBO_GP_LOTTERY_RESULT		232									//抽奖结果

//抽奖结果
#define DBO_GP_QUERY_USER_GAME_DATA	250									//游戏数据

//资料结果
#define DBO_GP_INDIVIDUAL_RESULT	260									//资料结果	
#define DBO_GP_QUERY_USERDATA_RESULT 261								//查询USERData结果

//道具命令
#define DBO_GP_GAME_PROPERTY_TYPE_ITEM		290									//类型节点
#define DBO_GP_GAME_PROPERTY_RELAT_ITEM		291									//道具关系
#define DBO_GP_GAME_PROPERTY_ITEM			292									//道具节点
#define DBO_GP_GAME_PROPERTY_SUB_ITEM		293									//道具节点
#define DBO_GP_GAME_PROPERTY_LIST_RESULT	294									//道具结果

#define DBO_GP_GAME_PROPERTY_BUY	300									//道具购买
#define DBO_GP_QUERY_BACKPACK		301									//查询背包
#define DBO_GP_GAME_PROPERTY_USE	302									//道具使用
#define DBO_GP_USER_PROPERTY_BUFF	303									//道具Buff
#define DBO_GP_PROPERTY_PRESENT		304									//道具赠送
#define DBO_GP_QUERY_SEND_PRESENT	305									//查询赠送
#define DBO_GP_GET_SEND_PRESENT		306									//获取赠送
#define DBO_GP_PROPERTY_QUERY_SINGLE	307								//道具购买
#define DBO_GP_GAME_PROPERTY_FAILURE 310									//道具失败

//会员结果
#define DBO_GP_MEMBER_PARAMETER						340					//会员参数
#define DBO_GP_MEMBER_QUERY_INFO_RESULT				341					//查询结果
#define DBO_GP_MEMBER_DAY_PRESENT_RESULT			342					//送金结果
#define DBO_GP_MEMBER_DAY_GIFT_RESULT				343					//礼包结果

//代理列表
#define DBO_GP_AGENT_GAME_KIND_ITEM		351								//类型信息

//服务结果
#define DBO_GP_OPERATE_SUCCESS		800									//操作成功
#define DBO_GP_OPERATE_FAILURE		801									//操作失败

//////////////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定索引
	DWORD							dwExperience;						//经验数值
	SCORE							lLoveLiness;						//用户魅力
	TCHAR							szPassword[LEN_MD5];				//登录密码	
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szDynamicPass[LEN_PASSWORD];		//动态密码
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//用户成绩
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserIngot;							//用户元宝
	SCORE							lUserInsure;						//用户银行	
	DOUBLE							dUserBeans;							//用户游戏豆

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间

	//扩展信息
	BYTE							cbInsureEnabled;					//使能标识
	DWORD							dwCheckUserRight;					//检查权限
	BYTE							cbIsAgent;							//代理标识	

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBO_GP_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBO_GP_ValidateMBCard
{
	UINT							uMBCardID;						//机器序列
};

//验证结果
struct DBO_GP_VerifyIndividualResult
{
	bool							bVerifyPassage;						//验证通过
	WORD							wVerifyMask;						//验证掩码
	TCHAR							szErrorMsg[128];					//错误消息
};

//头像信息
struct DBO_GP_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定索引
};

//个人资料
struct DBO_GP_UserIndividual
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址

	//推广信息
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推广信息
};

//银行资料
struct DBO_GP_UserInsureInfo
{
	BYTE							cbEnjoinTransfer;					//转帐开关
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wRevenueTransferMember;				//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转帐条件
};

//银行成功
struct DBO_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lVariationScore;					//游戏币变化
	SCORE							lVariationInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct  DBO_GP_UserInsureFailure
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//开通结果
struct DBO_GP_UserInsureEnableResult
{
	BYTE							cbInsureEnabled;					//使能标识
	TCHAR							szDescribeString[128];				//描述消息
};

//查询用户绑定银行结果
struct DBO_GP_QueryBankInfoResult
{
	TCHAR							szRealName[LEN_REAL_NAME];			//真实姓名
	TCHAR							szBankNo[LEN_BANK_NO];				//银行卡号
	SCORE							lDrawScore;							//可提现金额
	TCHAR							szAlipayID[LEN_BANK_ADDRESS];		//支付寶
};

//机器参数
struct DBO_GP_PlatformParameter
{
	DWORD							dwExchangeRate;						//兑换比率
	DWORD							dwPresentExchangeRate;				//魅力游戏币兑换率
	DWORD							dwRateGold;							//游戏豆游戏币兑换率
};

//机器参数
struct DBO_GP_AndroidParameter
{
	WORD							wSubCommdID;						//子命令码
	WORD							wServerID;							//房间标识
	WORD							wParameterCount;					//参数数目
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//机器参数
};

//资料结果
struct DBO_GP_IndividualResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前分数
	TCHAR							szDescribeString[128];				//描述消息
};

//查询结果
struct DBO_GP_QueryTransferRebateResult
{
	DWORD							dwUserID;							//用户I D
	BYTE							cbRebateEnabled;					//使能标识	
	SCORE							lIngot;								//返利元宝
	SCORE							lLoveLiness;						//返利魅力值
};

//签到奖励
struct DBO_GP_CheckInReward
{
	SCORE							lRewardGold[LEN_WEEK];				//奖励金额
};

//签到信息
struct DBO_GP_CheckInInfo
{
	WORD							wSeriesDate;						//连续日期
	bool							bTodayChecked;						//签到标识
};

//签到结果
struct DBO_GP_CheckInResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lScore;								//当前分数
	TCHAR							szNotifyContent[128];				//提示内容
};

//任务列表
struct DBO_GP_TaskListInfo
{
	WORD							wTaskCount;							//任务数目
};

//任务信息
struct DBO_GP_TaskInfo
{
	WORD							wTaskCount;							//任务数量
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//任务状态
};

//签到结果
struct DBO_GP_TaskResult
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

//低保参数
struct DBO_GP_BaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//低保结果
struct DBO_GP_BaseEnsureResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//推广信息
struct DBO_GP_UserSpreadInfo
{
	DWORD							dwSpreadCount;						//推广数量
	SCORE							lSpreadReward;						//推广奖励
};

//实名验证
struct DBO_GP_RealAuthParameter
{
	DWORD							dwAuthentDisable;					//验证开启
	SCORE							dwAuthRealAward;					//奖励金币
};

//等级配置
struct DBO_GP_GrowLevelConfig
{
	WORD							wLevelCount;						//等级数目
	tagGrowLevelConfig				GrowLevelConfig[60];				//等级配置
};

//等级参数
struct DBO_GP_GrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级	
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//等级升级
struct DBO_GP_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szNotifyContent[128];				//升级提示
};

//会员参数
struct DBO_GP_MemberParameter
{
	WORD							wMemberCount;						//会员数目
	tagMemberParameterNew			MemberParameter[64];				//会员参数
};

//会员查询
struct DBO_GP_MemberQueryInfoResult
{
	bool							bPresent;							//会员送金
	bool							bGift;								//会员礼包
	DWORD							GiftSubCount;						//道具数量
	tagGiftPropertyInfo				GiftSub[50];						//包含道具
};

//会员赠送
struct DBO_GP_MemberDayPresentResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lGameScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};

//会员礼包
struct DBO_GP_MemberDayGiftResult
{
	bool							bSuccessed;							//成功标识
	TCHAR							szNotifyContent[128];				//提示内容
};
//////////////////////////////////////////////////////////////////////////////////
//道具节点
struct DBO_GP_GamePropertyTypeItem
{
	DWORD							dwTypeID;							//类型标识
	DWORD							dwSortID;							//排序标识
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//道具节点
struct DBO_GP_GamePropertyRelatItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwTypeID;							//类型标识
};

//道具节点
struct DBO_GP_GamePropertyItem
{
	//道具信息
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwPropertyKind;						//功能类型

	BYTE							cbUseArea;							//使用范围
	BYTE							cbServiceArea;						//对象范围
	BYTE							cbRecommend;						//推荐标识

	//销售价格
	SCORE							lPropertyGold;						//道具金币
	DOUBLE							dPropertyCash;						//道具价格
	SCORE							lPropertyUserMedal;					//道具金币
	SCORE							lPropertyLoveLiness;				//道具金币

	//获得财富
	SCORE							lSendLoveLiness;					//获得魅力（赠送方）
	SCORE							lRecvLoveLiness;					//获得魅力（接收方）
	SCORE							lUseResultsGold;					//获得金币

	//持续效果
	DWORD							dwUseResultsValidTime;				//有效时间
	DWORD							dwUseResultsValidTimeScoreMultiple;	//有效倍率

	//礼物拆包
	DWORD							dwUseResultsGiftPackage;			//礼物拆包

	DWORD							dwSortID	;						//排序标识
	TCHAR							szName[PROPERTY_LEN_NAME];			 //道具名称
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//使用信息
};

//子道具节点
struct DBO_GP_GamePropertySubItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwOwnerPropertyID;					//道具标识
	DWORD                           dwPropertyCount;                    //道具数目
	DWORD							dwSortID;							//排序标识
};

//加载结果
struct DBO_GP_GamePropertyListResult
{
	BYTE							cbSuccess;							//成功标志
};

//购买结果
struct DBO_GP_PropertyBuyResult
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
struct DBO_GP_PropertyFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述信息
};

//查询结果
struct DBO_GP_PropertyQuerySingle
{
	//购买信息
	DWORD							dwUserID;							//用户 I D
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwItemCount;						//道具数目
};

//购买结果
struct DBO_GP_PurchaseResult
{
	bool							bSuccessed;							//成功标识
	BYTE							cbMemberOrder;						//会员系列
	SCORE							lCurrScore;							//当前游戏币
	DOUBLE							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};

//背包结果
struct DBO_GP_QueryBackpack
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwStatus;							//状态 0发送中 1发送结束
	DWORD							dwCount;							//个数
	tagBackpackProperty				PropertyInfo[1];					//道具信息
};
 
struct DBO_GP_PropertyUse
{
	DWORD							dwUserID;							//使用者
	DWORD							dwRecvUserID;						//道具对谁使用
	DWORD							dwPropID;							//道具ID
	DWORD							wPropCount;							//使用数目
	DWORD							dwScoreMultiple;					//效果倍数
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
	TCHAR							szNotifyContent[128];				//提示信息
};

//玩家道具Buff
struct DBO_GR_UserPropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff数目
	tagPropertyBuff					PropertyBuff[MAX_BUFF];			
};

//玩家赠送
struct DBO_GP_PropertyPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvGameID;						//道具给谁(GameID)
	DWORD							dwPropID;							//道具ID
	WORD							wPropCount;							//使用数目
	WORD							wType;								//0 根据玩家昵称  1 根据玩家GameID
	TCHAR							szRecvNickName[16];					//道具对谁使用（昵称）
	int								nHandleCode;						//返回码
	TCHAR							szNotifyContent[64];				//提示内容
};


//查询赠送
struct DBO_GP_QuerySendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];											
};

struct DBO_GP_GetSendPresent
{
	WORD							wPresentCount;						//赠送次数
	SendPresent						Present[MAX_PROPERTY];											
};

//兑换结果
struct DBO_GP_ExchangeResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	DOUBLE							dCurrBeans;							//当前游戏豆
	TCHAR							szNotifyContent[128];				//提示内容
};

//用户数据结果
struct DBO_GP_QueryUserDataResult
{
	DWORD							dwUserID;							//用户ID
};

//抽奖配置
struct DBO_GP_LotteryConfig
{
	WORD							wLotteryCount;						//奖项个数
	tagLotteryItem					LotteryItem[MAX_LOTTERY];			//奖项内容
};

//抽奖信息
struct DBO_GP_LotteryUserInfo
{
	BYTE							cbFreeCount;						//免费次数
	BYTE							cbAlreadyCount;						//已领次数
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	SCORE							lChargeFee;							//抽奖费用
};

//抽奖结果
struct DBO_GP_LotteryResult
{
	bool							bWined;								//中奖标识
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	SCORE							lUserScore;							//用户分数
	DOUBLE							dUserBeans;							//用户游戏豆
	tagLotteryItem					LotteryItem;						//中奖内容
};

//游戏数据
struct DBO_GP_QueryUserGameData
{
	WORD							wKindID;							//游戏标识
	DWORD							dwUserID;							//用户标识
	TCHAR							szUserGameData[1024];				//游戏数据
};

//操作失败
struct DBO_GP_OperateFailure
{
	bool							bCloseSocket;						//关闭连接
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct DBO_GP_OperateSuccess
{
	bool							bCloseSocket;						//关闭连接
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////

//游戏类型
struct DBO_GP_GameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct DBO_GP_GameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	WORD							wRecommend;							//推荐游戏
	WORD							wGameFlag;							//游戏标志
	DWORD							dwSuportType;						//支持类型
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct DBO_GP_GameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct DBO_GP_GamePage
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wPageID;							//定制索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//加载结果
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//成功标志
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//手机数据包

//登录命令
#define	DBR_MB_LOGON_GAMEID			700									//I D 登录
#define	DBR_MB_LOGON_ACCOUNTS		701									//帐号登录
#define DBR_MB_REGISTER_ACCOUNTS	702									//注册帐号
#define	DBR_MB_LOGON_OTHERPLATFORM	703									//其他登录
#define	DBR_MB_LOGON_VISITOR		704									//游客登录
#define DBR_MB_PHONE_REGISTER_ACCOUNTS	705								//手机号注册
#define DBR_MB_MODIFY_PASS	706								//修改手机验证码
#define DBR_MB_QUERY_USERDATA	707								//查询用户数据
#define DBR_MB_TOUSU_AGENT	708								//投诉代理
#define DBR_MB_SHENQING_AGENT_CHECK	709							//申请代理前检测
#define DBR_MB_SHENQING_AGENT	710								//申请代理
#define DBR_MB_QUERY_GAMELOCKINFO	711								//游戏锁表查询
#define DBR_MB_MODIFY_BANKINFO	712								//修改手机验证码

#define	DBO_MB_AGENT_GAME_KIND_ITEM		1100							//代理帐号
//登录结果
#define DBO_MB_LOGON_SUCCESS		900									//登录成功
#define DBO_MB_LOGON_FAILURE		901									//登录失败


#define DBO_MB_PERSONAL_PARAMETER	1000						//私人房间配置
#define DBO_MB_PERSONAL_ROOM_LIST	1001						//私人房间列表信息
#define DBO_MB_PERSONAL_FEE_LIST	    1002						//私人房间列表信息消耗费用信息
#define DBO_GR_QUERY_USER_ROOM_SCORE	    1003			//私人房间列表信息消耗费用信息
#define DBO_GR_QUERY_PERSONAL_ROOM_USER_INFO_RESULT	    1004			//私人房间列表信息消耗费用信息

#define DBO_MB_QUERY_GAMELOCKINFO_RESULT	    1005						//查询锁表信息结果

//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_MB_LogonGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_MB_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_MB_LogonOtherPlatform
{
	//登录信息
	BYTE							cbGender;							//用户性别
	BYTE							cbPlatformID;						//平台编号
	TCHAR							szUserUin[LEN_USER_UIN];			//用户Uin
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szAgentID[LEN_ACCOUNTS];			//代理标识
	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//游客登录
struct DBR_MB_LogonVisitor
{
	//登录信息
	BYTE							cbPlatformID;						//平台编号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	TCHAR							szAgentID[LEN_ACCOUNTS];			//代理标识

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//投诉代理
struct DBR_MB_TouSuDaili
{
	//用户ID
	DWORD dwUserID;								//用户ID
	TCHAR szUserWx[LEN_AGENT_ID];				//用户微信
	TCHAR szAgentWx[LEN_AGENT_ID];				//代理微信
	TCHAR szReason[LEN_DESC];					//代理微信
	TCHAR szImageName[LEN_IMAGE_NAME];			//代理微信
};

//申请代理前检查
struct DBR_MB_ShenQingDailiCheck
{
	DWORD dwUserID;								//用户ID
};

//申请代理
struct DBR_MB_ShenQingDaili
{
	DWORD							dwUserID;						//用户ID
	TCHAR							szAgentAcc[LEN_ACCOUNTS];		//代理账号
	TCHAR							szAgentName[LEN_AGENT_ID];		//代理名称
	TCHAR							szAlipay[LEN_ALIPAY];			//支付宝
	TCHAR							szWeChat[LEN_WECHAT];			//微信号
	TCHAR							szQQ[LEN_QQ];					//QQ号
	TCHAR							szNote[LEN_DESC];				//备注
};

//申请代理
struct DBR_MB_QueryGameLockInfo
{
	DWORD							dwUserID;						//用户ID
};


//帐号注册
struct DBR_MB_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	BYTE                            cbDeviceType;                       //设备类型
	DWORD							dwSpreaderGameID;					//推荐标识
	TCHAR							szAgentID[LEN_ACCOUNTS];							//代理标识

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//手机号注册
struct DBR_MB_PhoneRegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	BYTE                            cbDeviceType;                       //设备类型
	DWORD							dwSpreaderGameID;					//推荐标识
	TCHAR							szAgentID[LEN_ACCOUNTS];							//代理标识

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//修改密码
struct DBR_MB_ModifyPass
{
	//注册信息
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szPassword[LEN_PASSWORD];			//新密码
	BYTE							cbMode;								//银行 登录
	TCHAR							szVerifyCode[LEN_VERIFY_CODE];		//注册短信验证码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号
	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	//附加信息
	DWORD							dwClientAddr;						//连接地址
};

//验证码修改银行信息
struct DBR_MB_ModifyBankInfo
{
	//系统信息
	DWORD							dwUserID;						//用户ID
	TCHAR							szBankNo[LEN_BANK_NO];			//银行卡号
	TCHAR							szBankName[LEN_BANK_NAME];		//银行名称

	TCHAR							szBankAddress[LEN_BANK_ADDRESS];	//银行名称
	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	//附加信息
	DWORD							dwClientAddr;						//连接地址
};

//查询用户信息
struct DBR_MB_QueryUserData
{
	//系统信息
	BYTE cbType;								//操作类型
	TCHAR szWhere[LEN_WHERE_PARM];				//操作参数
};

//登录成功
struct DBO_MB_LogonSuccess
{
	//用户属性
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomID;							//自定头像
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwExperience;						//经验数值
	SCORE							lLoveLiness;						//用户魅力
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szDynamicPass[LEN_PASSWORD];		//动态密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szChangeLogonIP[LEN_IP];			//修改客户端显示归属地IP
	TCHAR							szRegisterMobile[LEN_MOBILE_PHONE];	//游客用来判断是否绑定了手机
	//用户成绩
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserIngot;							//用户元宝
	SCORE							lUserInsure;						//用户银行	
	DOUBLE							dUserBeans;							//用户游戏豆

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间

	//扩展信息
	BYTE							cbInsureEnabled;					//使能标识
	BYTE							cbIsAgent;							//代理标识						
	BYTE							cbMoorMachine;						//锁定机器
	SCORE							lRoomCard;						//玩家房卡
	DWORD						dwLockServerID;						//锁定房间
	DWORD						dwKindID;							//游戏类型

	SYSTEMTIME						RegisterData;					//注册时间
	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};


//登录失败
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//私人房间配置
struct DBO_MB_PersonalTableConfig
{
	DWORD							dwCount;							//配置数量
	tagPersonalTableParameter		PersonalTableParameter[50];			//配置信息
};

//私人房间列表信息
struct DBO_MB_PersonalRoomInfoList
{
	DWORD							dwUserID;													//配置数量
	tagPersonalRoomInfo		PersonalRoomInfo[MAX_CREATE_SHOW_ROOM];			//配置信息
	DBO_MB_PersonalRoomInfoList()
	{
		memset(PersonalRoomInfo, 0, sizeof(tagPersonalRoomInfo));
	}
};

//创建桌子
struct DBR_GR_QUERY_USER_ROOM_INFO
{
	DWORD							dwUserID;							//用户标识
};

//创建桌子
struct DBR_GR_CLOSE_ROOM_SERVER_ID
{
	DWORD							dwServerID;							//用户标识
};

//请求玩家的用户信息
struct DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO
{
	DWORD							dwUserID;							//用户标识
};

//私人房间列表信息
struct DBO_MB_PersonalRoomUserInfo
{
	SCORE							lRoomCard;								//房卡数量
	DOUBLE						dBeans;									//游戏豆
};

struct DBO_MB_QueryGameLockInfoResult
{
	DWORD							dwKindID;							//锁在游戏ID
	DWORD							dwServerID;							//游戏房间ID
};

//兑换游戏币
struct DBR_GP_ExchangeScoreByRoomCard
{
	DWORD							dwUserID;							//用户标识
	SCORE							lExchangeRoomCard;						//兑换数量

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//房卡兑换结果
struct DBO_GP_RoomCardExchangeResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrRoomCard;							//当前元宝
	TCHAR							szNotifyContent[128];				//提示内容
};

//////////////////////////////////////////////////////////////////////////////////

#endif