--
-- Author: zhong
-- Date: 2016-08-04 09:51:55
--

--存放logonserver部分结构体定义
--消息定义见 yl.lua
local login = {}

login.CMD_GP_AccountBind_Exists = 
{
	--目标用户
	{k = "szAccounts", t = "tchar", len = yl.LEN_ACCOUNTS},
	--用户密码
	{k = "szPassword", t = "tchar", len = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", len = yl.LEN_MACHINE_ID},
}

----------------------------------------------
--/服务命令
login.MDM_GP_USER_SERVICE				= 3									--用户服务

--账号服务
login.SUB_GP_MODIFY_MACHINE				= 100								--修改机器
login.SUB_GP_MODIFY_LOGON_PASS			= 101								--修改密码
login.SUB_GP_MODIFY_INSURE_PASS			= 102								--修改密码
login.SUB_GP_MODIFY_UNDER_WRITE			= 103								--修改签名

--修改头像
login.SUB_GP_USER_FACE_INFO				= 120								--头像信息
login.SUB_GP_SYSTEM_FACE_INFO			= 122								--系统头像
login.SUB_GP_CUSTOM_FACE_INFO			= 123								--自定头像

--个人资料
login.SUB_GP_USER_INDIVIDUAL			= 140								--个人资料
login.SUB_GP_QUERY_INDIVIDUAL			= 141								--查询信息
login.SUB_GP_MODIFY_INDIVIDUAL			= 152								--修改资料
login.SUB_GP_INDIVIDUAL_RESULT			= 153								--完善资料
login.SUB_GP_REAL_AUTH_QUERY			= 154								--认证请求
login.SUB_GP_REAL_AUTH_RESULT			= 155								--认证结果

--银行服务
login.SUB_GP_USER_ENABLE_INSURE			= 160								--开通银行
login.SUB_GP_USER_SAVE_SCORE			= 161								--存款操作
login.SUB_GP_USER_TAKE_SCORE			= 162								--取款操作
login.SUB_GP_USER_TRANSFER_SCORE		= 163								--转账操作
login.SUB_GP_USER_INSURE_INFO			= 164								--银行资料
login.SUB_GP_QUERY_INSURE_INFO			= 165								--查询银行
login.SUB_GP_USER_INSURE_SUCCESS		= 166								--银行成功
login.SUB_GP_USER_INSURE_FAILURE		= 167								--银行失败
login.SUB_GP_QUERY_USER_INFO_REQUEST	= 168								--查询用户
login.SUB_GP_QUERY_USER_INFO_RESULT		= 169								--用户信息
login.SUB_GP_USER_INSURE_ENABLE_RESULT 	= 170								--开通结果	
login.SUB_GP_QUERY_TRANSFER_REBATE		= 171								--查询返利
login.SUB_GP_QUERY_TRANSFER_REBATE_RESULT = 172								--查询结果
login.SUB_GP_QUERY_USERDATA_RESULT 		= 173								--查询结果
login.SUB_GP_QUERY_BANK_INFO			= 174								--查询用户绑定银行信息
login.SUB_GP_QUERY_BANK_INFO_RESULT		= 175								--查询用户绑定银行结果

login.SUB_GP_BIND_BANK_INFO				= 176									--绑定银行卡信息
login.SUB_GP_EXCHANGE_BANK_SCORE		= 177									--兑换金币
login.SUB_GP_QUERY_GAMELOCKINFO_RESULT		= 178									--查询锁表结果
--签到服务
login.SUB_GP_CHECKIN_QUERY				= 220								--查询签到
login.SUB_GP_CHECKIN_INFO				= 221								--签到信息
login.SUB_GP_CHECKIN_DONE				= 222								--执行签到
login.SUB_GP_CHECKIN_RESULT				= 223								--签到结果

--任务服务
login.SUB_GP_TASK_LOAD					= 240								--任务加载
login.SUB_GP_TASK_TAKE					= 241								--任务领取
login.SUB_GP_TASK_REWARD				= 242								--任务奖励
login.SUB_GP_TASK_GIVEUP				= 243								--任务放弃
login.SUB_GP_TASK_INFO					= 250								--任务信息
login.SUB_GP_TASK_LIST					= 251								--任务信息
login.SUB_GP_TASK_RESULT				= 252								--任务结果
login.SUB_GP_TASK_GIVEUP_RESULT			= 253								--放弃结果

--低保服务
login.SUB_GP_BASEENSURE_LOAD			= 260								--加载低保
login.SUB_GP_BASEENSURE_TAKE			= 261								--领取低保
login.SUB_GP_BASEENSURE_PARAMETER		= 262								--低保参数
login.SUB_GP_BASEENSURE_RESULT			= 263								--低保结果

--推广服务
login.SUB_GP_SPREAD_QUERY				= 280								--推广奖励
login.SUB_GP_SPREAD_INFO				= 281								--奖励参数

--等级服务
login.SUB_GP_GROWLEVEL_QUERY			= 300								--查询等级
login.SUB_GP_GROWLEVEL_PARAMETER		= 301								--等级参数
login.SUB_GP_GROWLEVEL_UPGRADE			= 302								--等级升级

--兑换服务
login.SUB_GP_EXCHANGE_QUERY				= 320								--兑换参数
login.SUB_GP_EXCHANGE_PARAMETER			= 321								--兑换参数
login.SUB_GP_PURCHASE_MEMBER			= 322								--购买会员
login.SUB_GP_PURCHASE_RESULT			= 323								--购买结果
login.SUB_GP_EXCHANGE_SCORE_BYINGOT		= 324								--兑换游戏币
login.SUB_GP_EXCHANGE_SCORE_BYBEANS		= 325								--兑换游戏币
login.SUB_GP_EXCHANGE_RESULT			= 326								--兑换结果

--会员服务
login.SUB_GP_MEMBER_PARAMETER			= 340								--会员参数
login.SUB_GP_MEMBER_QUERY_INFO			= 341								--会员查询
login.SUB_GP_MEMBER_DAY_PRESENT			= 342								--会员送金
login.SUB_GP_MEMBER_DAY_GIFT			= 343								--会员礼包
login.SUB_GP_MEMBER_PARAMETER_RESULT	= 350								--参数结果
login.SUB_GP_MEMBER_QUERY_INFO_RESULT	= 351								--查询结果
login.SUB_GP_MEMBER_DAY_PRESENT_RESULT	= 352								--送金结果
login.SUB_GP_MEMBER_DAY_GIFT_RESULT		= 353								--礼包结果

--抽奖服务
login.SUB_GP_LOTTERY_CONFIG_REQ			= 360								--请求配置
login.SUB_GP_LOTTERY_CONFIG				= 361								--抽奖配置
login.SUB_GP_LOTTERY_USER_INFO			= 362								--抽奖信息
login.SUB_GP_LOTTERY_START				= 363								--开始抽奖
login.SUB_GP_LOTTERY_RESULT				= 364								--抽奖结果

--游戏服务
login.SUB_GP_QUERY_USER_GAME_DATA		= 370								--查询数据	

--帐号绑定
login.SUB_GP_ACCOUNT_BINDING			= 380								--帐号绑定

--操作结果
login.SUB_GP_OPERATE_SUCCESS			= 500								--操作成功
login.SUB_GP_OPERATE_FAILURE			= 501								--操作失败

-----------------------------
--用户服务

--用户头像
login.CMD_GP_UserFaceInfo = 
{
	--头像标识
	{k = "wFaceID", t = "word"},
	--自定标识
	{k = "dwCustomID", t = "dword"}
}

--系统头像 修改头像
login.CMD_GP_SystemFaceInfo = 
{
	--头像标识
	{k = "wFaceID", t = "word"},
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},
}

--自定头像 修改头像
login.CMD_GP_CustomFaceInfo = 
{
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},
	--图片信息
	{k = "dwCustomFace", t = "ptr", s = yl.FACE_CX * yl.FACE_CY * 4}
}

-- 绑定机器
login.CMD_GP_ModifyMachine = 
{
	-- 绑定标志
	{ t = "byte", k = "cbBind" },
	-- 用户标识
	{ t = "dword", k = "dwUserID"},
	--用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	--机器序列
	{ t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 个人资料
login.CMD_GP_UserIndividual = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
}

-- 查询信息
login.CMD_GP_QueryIndividual = 
{
	-- 用户uid
	{ t = "dword", k = "dwUserID"},
	-- 用户密码
	{ t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
}

-- 携带信息
login.DTP_GP_UI_ACCOUNTS			 = 1									--用户账号	
login.DTP_GP_UI_NICKNAME			 = 2									--用户昵称
login.DTP_GP_UI_USER_NOTE			 = 3									--用户说明
login.DTP_GP_UI_UNDER_WRITE			 = 4									--个性签名
login.DTP_GP_UI_QQ				 	 = 5									--Q Q 号码
login.DTP_GP_UI_EMAIL				 = 6									--电子邮件
login.DTP_GP_UI_SEAT_PHONE			 = 7									--固定电话
login.DTP_GP_UI_MOBILE_PHONE		 = 8									--移动电话
login.DTP_GP_UI_COMPELLATION		 = 9									--真实名字
login.DTP_GP_UI_DWELLING_PLACE		 = 10									--联系地址
login.DTP_GP_UI_PASSPORTID    		 = 11									--身份标识
login.DTP_GP_UI_SPREADER			 = 12									--推广标识

-----------------------------
--银行服务

--银行资料
login.CMD_GP_UserInsureInfo = 
{
	{k = "cbEnjoinTransfer", t = "byte"},								--转账开关
	{k = "wRevenueTake", t = "word"},									--税收比例
	{k = "wRevenueTransfer", t = "word"},								--税收比例
	{k = "wRevenueTransferMember", t = "word"},							--税收比例
	{k = "wServerID", t = "word"},										--房间标识
	{k = "lUserScore", t = "double"},									--用户金币
	{k = "lUserInsure", t = "double"},									--银行金币
	{k = "lTransferPrerequisite", t = "double"},						--转账条件
}

--查询银行
login.CMD_GP_QueryInsureInfo = 
{
	--用户id
	{k = "dwUserID", t = "dword"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
}

--查询银行
login.CMD_GP_QueryBankInfo = 
{											
	--用户id
	{k = "dwUserID", t = "dword"}
}

--查询用户绑定银行结果
login.CMD_GP_QueryBankInfoResult =
{
	{k = "szRealName", t = "string", s = yl.LEN_REAL_NAME},						--真实姓名
	{k = "szBankNo", t = "string", s = yl.LEN_BANK_NO},							--银行卡号
	{k = "lDrawScore", t = "double"},											--可提现金额
    {k = "szAlipayID", t = "string", s = yl.LEN_BANK_ADDRESS},					--支付宝
}

--查询银行
login.CMD_GP_BindBankInfo = 
{											
	--用户id
	{k = "dwUserID", t = "dword"},
	{k = "szRealName", t = "tchar", s = yl.LEN_REAL_NAME},						--真实姓名
	{k = "szBankNo", t = "tchar", s = yl.LEN_BANK_NO},							--银行卡号
	{k = "szBankName", t = "tchar", s = yl.LEN_BANK_NAME},						--银行名称
	{k = "szBankAddress", t = "tchar", s = yl.LEN_BANK_ADDRESS},				--银行名称
    {k = "szAlipayID", t = "tchar", s = yl.LEN_ALIPAY_ID},				        --支付宝
}

--兑换金币
login.CMD_GP_ExchangeBankScore =
{											
	{k = "dwUserID", t = "dword"},								--用户id
	{k = "dwScore", t = "double"},								--真实姓名
	{k = "szBankPass", t = "tchar", s = yl.LEN_PASSWORD},		--银行卡号
	{k = "szOrderID", t = "thcar", s = yl.LEN_ORDER_ID},		--银行名称
    {k = "dwType", t = "dword"},	                            -- 0 bank 1alipay
}


--银行-用户信息
login.CMD_GP_UserTransferUserInfo = 
{
	{k = "dwTargetGameID", t = "dword"},								--目标用户
	{k = "szAccounts", t = "string", s = 32},							--目标用户
}

--用户数据结果
login.CMD_GP_QueryUserDataResult =
{
	{k = "dwUserID", t = "dword"}
}

-----------------------------
--任务服务

--任务结果
login.CMD_GP_TaskResult =
{
	--结果信息
	{k = "bSuccessed", t = "bool"},										--成功标识
	{k = "wCommandID", t = "word"},										--命令标识

	--财富信息
	{k = "lCurrScore", t = "score"},									--当前金币
	{k = "lCurrIngot", t = "score"},									--当前元宝

	--提示信息
	{k = "szNotifyContent", t = "string"},								--提示内容
}

----------------------------------------------

----------------------------------------------
--道具命令
login.MDM_GP_PROPERTY						=	6	

--道具信息
login.SUB_GP_QUERY_PROPERTY					=	1							--道具查询
login.SUB_GP_PROPERTY_BUY					=	2							--购买道具
login.SUB_GP_PROPERTY_USE					=	3							--道具使用
login.SUB_GP_QUERY_BACKPACKET				=	4							--背包查询
login.SUB_GP_PROPERTY_BUFF					=	5							--道具Buff
login.SUB_GP_QUERY_SEND_PRESENT				=	6							--查询赠送
login.SUB_GP_PROPERTY_PRESENT				=	7							--赠送道具
login.SUB_GP_GET_SEND_PRESENT				=	8							--获取赠送

login.SUB_GP_QUERY_PROPERTY_RESULT			=	101							--道具查询
login.SUB_GP_PROPERTY_BUY_RESULT			=	102							--购买道具
login.SUB_GP_PROPERTY_USE_RESULT			=	103							--道具使用
login.SUB_GP_QUERY_BACKPACKET_RESULT		=	104							--背包查询
login.SUB_GP_PROPERTY_BUFF_RESULT			=	105							--道具Buff
login.SUB_GP_QUERY_SEND_PRESENT_RESULT		=	106							--查询赠送
login.SUB_GP_PROPERTY_PRESENT_RESULT		=	107							--赠送道具
login.SUB_GP_GET_SEND_PRESENT_RESULT		=	108							--获取赠送

login.SUB_GP_QUERY_PROPERTY_RESULT_FINISH	=	201							--道具查询

login.SUB_GP_PROPERTY_FAILURE				=	404							--道具失败
--发送的道具
login.SendPresent = 
{
	{k = "dwUserID", t = "dword"},											--赠送者
	{k = "dwRecvUserID", t = "dword"},										--道具给谁
	{k = "dwPropID", t = "dword"},
	{k = "wPropCount", t = "word"},
	{k = "tSendTime", t = "score"},											--赠送的时间
	{k = "szPropName", t = "string", s = 16},								--道具名称
}

--获取赠送
login.CMD_GP_S_GetSendPresent = 
{
	{k = "wPresentCount", t = "word"}, 										--赠送次数
	{k = "Present", t = "table", d = login.SendPresent}						--道具
}

--道具成功
login.CMD_GP_S_PropertySuccess = --CMD_GP_PropertyBuyResult
{
	--购买信息
	{k = "dwUserID", t = "dword"},										--道具ID
	{k = "dwPropID", t = "dword"},										--道具ID
	{k = "dwPropCount", t = "dword"},									--道具数量	
	{k = "lInsureScore", t = "score"},									--银行存款
	{k = "lUserMedal", t = "score"},									--用户元宝	
	{k = "lLoveLiness", t = "score"},									--魅力值	
	{k = "dCash", t = "double"},										--游戏豆	
	--{k = "cbSuccessed", t = "byte"},									--成功标识	
	{k = "cbCurrMemberOrder", t = "byte"},								--会员等级
	{k = "szNotifyContent", t = "string"},								--提示内容
	
}

--道具失败
login.CMD_GP_PropertyFailure = 
{
	{k = "lErrorCode", t = "score"},									--错误代码
	{k = "szDescribeString", t = "string", s = 256},					--描述信息				
}

--获取赠送
login.CMD_GP_C_GetSendPresent = 
{
	{k = "dwUserID", t = "dword"},										--赠送者
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD}				--用户密码
}
----------------------------------------------


----------------------------------------------
--登陆命令
login.MDM_MB_LOGON							= 100

--游客登录
login.CMD_MB_LogonVisitor =
{
	--系统信息
	{ k = "wModuleID", t = "word" },									--模块标识
	{ k = "dwPlazaVersion", t = "dword"},								--广场版本
	{ k = "szAgentID", t = "tchar", s = yl.LEN_ACCOUNTS},									--代理标识
	{ k = "cbDeviceType", t = "byte"},                       			--设备类型

	--连接信息
	{ k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},			--机器标识
	{ k = "szMobilePhone", t = "tchar", s = yl.LEN_MOBILE_PHONE}		--电话号码
}

--注册帐号
login.CMD_MB_RegisterAccounts =
{
	--系统信息
	{ k = "wModuleID", t = "word" },										--模块标识
	{ k = "dwPlazaVersion", t = "dword"},									--广场版本
	{ k = "cbDeviceType", t = "byte"},                       				--设备类型

	--密码变量
	{ k = "szLogonPass", t = "tchar", s = yl.LEN_MD5},						--登录密码

	--注册信息
	{ k = "wFaceID", t = "word" },											--头像标识
	{ k = "cbGender", t = "byte"},											--用户性别
	{ k = "szAccounts", t = "tchar", s = yl.LEN_ACCOUNTS},					--登录帐号
	{ k = "szNickName", t = "tchar", s = yl.LEN_NICKNAME},					--用户昵称
	{ k = "dwSpreaderGameID", t = "dword"},									--推荐标识	
	{ k = "szAgentID", t = "tchar", s= yl.LEN_ACCOUNTS},										--代理标识	

	--连接信息
	{ k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},				--机器标识
	{ k = "szMobilePhone", t = "tchar", s = yl.LEN_MOBILE_PHONE},			--电话号码
	{ k = "szRegisterMobile", t = "tchar", s = yl.LEN_MOBILE_PHONE},		--注册手机号码-当账号用
}

--手机账号注册
login.CMD_MB_PhoneRegisterAccounts =
{
	--系统信息
	{ k = "wModuleID", t = "word" },								--模块标识
	{ k = "dwPlazaVersion", t = "dword" },							--广场版本
	{ k = "cbDeviceType", t = "byte" },                       		--设备类型

	--密码变量
	{ k = "szLogonPass", t = "tchar", s= yl.LEN_MD5},				--登录密码

	--注册信息
	{ k = "wFaceID", t = "word" },									--头像标识
	{ k = "cbGender", t = "byte" },									--用户性别
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--登录帐号
	{ k = "szNickName", t = "tchar", s= yl.LEN_NICKNAME },			--用户昵称
	{ k = "dwSpreaderGameID", t = "dword" },						--推荐标识	
	{ k = "szAgentID", t = "tchar", s= yl.LEN_ACCOUNTS },			--代理标识	
	{ k = "szVerifyCode", t = "tchar", s= yl.LEN_VERIFY_CODE},		--注册短信验证码

	--连接信息
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

--获取手机注册验证码
login.CMD_MB_GetRegisterVerifyCode =
{
	--系统信息
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--手机号
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

--获取修改登录密码验证码
login.CMD_MB_GetModifyPassVerifyCode =
{
	--系统信息
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--手机号
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

--获取修改银行信息验证码
login.CMD_MB_GetModifyBankInfoVerifyCode =
{
	--系统信息
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--手机号
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

login.CMD_MB_ModifyPassVerifyCode = 
{
	--系统信息
	{ k = "szAccounts", t = "tchar", s= yl.LEN_ACCOUNTS },			--用户账号
	{ k = "szPassword", t = "tchar", s= yl.LEN_PASSWORD },			--新密码
	{ k = "cbMode", t = "byte" },								--银行 登录
	{ k = "szVerifyCode", t = "tchar", s= yl.LEN_VERIFY_CODE },		--注册短信验证码
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--手机号
	--连接信息
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

--验证码修改银行信息
login.CMD_MB_ModifyBankInfoVerifyCode = 
{
	--系统信息
	{ k = "dwUserID", t = "dword"},			--用户ID
	{ k = "szBankNo", t = "tchar", s= yl.LEN_BANK_NO },			--银行卡号
	{ k = "szBankName", t = "tchar", s= yl.LEN_BANK_NAME },		--银行名称
	{ k = "szVerifyCode", t = "tchar", s= yl.LEN_VERIFY_CODE },		--注册短信验证码
	{ k = "szMobilePhone", t = "tchar", s= yl.LEN_MOBILE_PHONE },	--手机号
	{ k = "szBankAddress", t = "tchar", s= yl.LEN_BANK_ADDRESS },	--银行地址
	--连接信息
	{ k = "szMachineID", t = "tchar", s= yl.LEN_MACHINE_ID },		--机器标识
}

login.CMD_GP_VerifyCodeResult =
{
	{ k = "cbResultCode", t = "byte" },                       		
	{ k = "szDescString", t = "tchar", s= 64 },	
}

--获取用户信息
login.CMD_MB_QueryUserData =
{
	--系统信息
	{ k = "cbType", t = "byte" },								--操作类型 0 账号登录 2游客登录 5 微信登录
	{ k = "szWhere", t = "tchar", s= yl.LEN_WHERE_PARM },		--操作参数
}

--投诉代理
login.CMD_MB_TouSuDaili =
{
	--系统信息
	{ k = "dwUserID", t = "dword" },						--用户ID
	{ k = "szUserWx", t = "tchar", s= yl.LEN_AGENT_ID },	--我的微信号
	{ k = "szAgentWx", t = "tchar", s= yl.LEN_AGENT_ID },	--代理微信号
	{ k = "szReason", t = "tchar", s= yl.LEN_DESC },		--描述长度
	{ k = "szImageName", t = "tchar", s= yl.LEN_IMAGE_NAME },--文件名长度
}

--申请代理前检查
login.CMD_MB_ShenQingDailiCheck =
{
	--系统信息
	{ k = "dwUserID", t = "dword" },						--用户ID
}


--投诉代理
login.CMD_MB_ShenQingDaili =
{
	--系统信息
	{ k = "dwUserID", t = "dword" },						--用户ID
	{ k = "szAgentAcc", t = "tchar", s= yl.LEN_ACCOUNTS },	--我的微信号
	{ k = "szAgentName", t = "tchar", s= yl.LEN_AGENT_ID },	--代理微信号
	{ k = "szAlipay", t = "tchar", s= yl.LEN_ALIPAY },		--描述长度
	{ k = "szWeChat", t = "tchar", s= yl.LEN_WECHAT },--文件名长度
	{ k = "szQQ", t = "tchar", s= yl.LEN_QQ },--文件名长度
	{ k = "szNote", t = "tchar", s= yl.LEN_DESC },--文件名长度
}

--登陆模式
login.SUB_MB_LOGON_OTHERPLATFORM			= 4							--其他登陆

------
--发包结构
login.CMD_MB_LogonOtherPlatform = 
{
	{k = "wModuleID", t = "word"},										--模块标识
	{k = "dwPlazaVersion", t = "dword"},								--广场版本
	{k = "cbDeviceType", t = "byte"},									--设备类型
	--注册信息
	{k = "cbGender", t = "byte"},										--用户性别
	{k = "cbPlatformID", t = "byte"},									--平台编号
	{k = "szUserUin", t = "tchar", s = yl.LEN_USER_UIN},				--登陆帐号
	{k = "szNickName", t = "tchar", s = yl.LEN_NICKNAME},				--用户昵称
	{k = "szCompellation", t = "tchar", s = yl.LEN_COMPELLATION},		--真实名字
	{k = "szAgentID", t = "tchar", s= yl.LEN_ACCOUNTS},										--代理标识
	--连接信息
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},			--机器标识
	{k = "szMobilePhone", t = "tchar", s = yl.LEN_MOBILE_PHONE},		--电话号码
}

--锁表查询
login.CMD_MB_QueryGameLockInfo = 
{
	{ k = "dwUserID", t = "dword" }						--用户ID
}

--锁表查询结果
login.CMD_MB_QueryGameLockInfoResult = 
{
	{ k = "dwKindID", t = "dword" },						--用户ID
	{ k = "dwServerID", t = "dword" }						--用户ID
}

------
--回包结构
--登陆失败
login.CMD_MB_LogonFailure = 
{
	{k = "lResultCode", t = "int"},										--错误代码
	{k = "szDescribeString", t = "string"},								--描述消息
}

-- 操作失败
login.CMD_GP_OperateFailure = 
{
	-- 错误代码
	{t = "int", k = "lResultCode"},
	-- 描述消息
	{t = "string", k = "szDescribeString"},
}

-- 操作成功
login.CMD_GP_OperateSuccess = 
{
	-- 操作代码
	{t = "int", k = "lResultCode"},
	-- 描述消息
	{t = "string", k = "szDescribeString"},
}

return login