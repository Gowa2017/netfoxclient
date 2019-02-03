--
-- Author: zhong
-- Date: 2016-07-21 11:24:22
--
local game_cmd = {}

--[[
******
* 结构体描述
* {k = "key", t = "type", s = len, l = {}}
* k表示字段名,对应C++结构体变量名
* t表示字段类型,对应C++结构体变量类型
* s针对string变量特有,描述长度
* l针对数组特有,描述数组长度,以table形式,一维数组表示为{N},N表示数组长度,多维数组表示为{N,N},N表示数组长度
* 字符串型，发包定义tchar目的为了*2，回包定义string，因为网络底层读取会自动*2

** egg
* 取数据的时候,针对一维数组,假如有字段描述为 {k = "a", t = "byte", l = {3}}
* 则表示为 变量a为一个byte型数组,长度为3
* 取第一个值的方式为 a[1][1],第二个值a[1][2],依此类推

* 取数据的时候,针对二维数组,假如有字段描述为 {k = "a", t = "byte", l = {3,3}}
* 则表示为 变量a为一个byte型二维数组,长度都为3
* 则取第一个数组的第一个数据的方式为 a[1][1], 取第二个数组的第一个数据的方式为 a[2][1]
******
]]
----------------------------------------------------------------------------------------------
-- 登录信息
game_cmd.MDM_GR_LOGON = 1													-- 登录信息
-- 登录模式
game_cmd.SUB_GR_LOGON_USERID = 1											-- ID登录
game_cmd.SUB_GR_LOGON_MOBILE = 2											-- 手机登录
game_cmd.SUB_GR_LOGON_ACCOUNTS = 3 											-- 帐户登录
-- 登录结果
game_cmd.SUB_GR_LOGON_SUCCESS = 100 										-- 登录成功
game_cmd.SUB_GR_LOGON_FAILURE = 101 										-- 登录失败
game_cmd.SUB_GR_LOGON_FINISH = 102 											-- 登录完成
-- 升级提示
game_cmd.SUB_GR_UPDATE_NOTIFY = 200 										-- 升级提示

------
-- 消息结构
------

-- 登录成功
game_cmd.CMD_GR_LogonSuccess = 
{
	-- 用户权限
	{t = "dword", k = "dwUserRight"},
	-- 管理权限
	{t = "dword", k = "dwMasterRight"},
}
--
----------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------
--用户命令
game_cmd.MDM_GR_USER = 3													--用户信息

--用户动作
game_cmd.SUB_GR_USER_RULE 							= 1						--用户规则
game_cmd.SUB_GR_USER_WAIT_DISTRIBUTE				= 12					--等待分配

--用户状态
game_cmd.SUB_GR_REQUEST_FAILURE					 	= 103					--请求失败

------
--发包结构

------
--回包结构
--请求失败
game_cmd.CMD_GR_RequestFailure =
{
	{k = "lErrorCode", t = "int"},											--错误代码
	{k = "szDescribeString", t = "string"}									--描述信息
}

--用户规则
game_cmd.CMD_GR_UserRule = 
{
	{k = "cbRuleMask", t = "byte"},											--规则掩码
	{k = "wMinWinRate", t = "word"},										--最低胜率
	{k = "wMaxFleeRate", t = "word"},										--最高逃率
	{k = "lMaxGameScore", t = "score"},										--最高分数
	{k = "lMinGameScore", t = "score"},										--最低分数
}

--数据扩展
game_cmd.tagDataDescribe = 
{
	{k = "wDataSize", t = "word"},											--数据大小
	{k = "wDataDecribe", t = "word"},										--数据描述
}

----------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------
--银行命令
------
game_cmd.MDM_GR_INSURE = 5												   --用户信息
--命令
game_cmd.SUB_GR_ENABLE_INSURE_REQUEST            = 1                       --开通银行
game_cmd.SUB_GR_QUERY_INSURE_INFO                = 2                       --查询银行
game_cmd.SUB_GR_SAVE_SCORE_REQUEST               = 3                       --存款操作
game_cmd.SUB_GR_TAKE_SCORE_REQUEST               = 4                       --取款操作
game_cmd.SUB_GR_TRANSFER_SCORE_REQUEST           = 5                       --取款操作
game_cmd.SUB_GR_QUERY_USER_INFO_REQUEST          = 6                       --查询用户

game_cmd.SUB_GR_USER_INSURE_INFO                 = 100                     --银行资料
game_cmd.SUB_GR_USER_INSURE_SUCCESS              = 101                     --银行成功
game_cmd.SUB_GR_USER_INSURE_FAILURE              = 102                     --银行失败
game_cmd.SUB_GR_USER_TRANSFER_USER_INFO          = 103                     --用户资料
game_cmd.SUB_GR_USER_INSURE_ENABLE_RESULT        = 104                     --开通结果

-- 开通银行
game_cmd.CMD_GR_C_EnableInsureRequest =
{
	-- 游戏动作
	{t = "byte", k = "cbActivityGame"},
	-- 用户ID
	{t = "dword", k = "dwUserID"},
	-- 登陆密码
	{t = "tchar", k = "szLogonPass", s = yl.LEN_PASSWORD},
	-- 银行密码
	{t = "tchar", k = "szInsurePass", s = yl.LEN_PASSWORD},
	-- 机器序列
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 查询银行
game_cmd.CMD_GR_C_QueryInsureInfoRequest = 
{
	-- 游戏动作
	{t = "byte", k = "cbActivityGame"},
	-- 银行密码
	{t = "tchar", k = "szInsurePass", s = yl.LEN_PASSWORD},
}

--存款请求
game_cmd.CMD_GR_C_SaveScoreRequest = 
{
	--游戏动作
	{k = "cbActivityGame", t = "byte"},
	--存款数目
	{k = "lSaveScore", t = "score"},
}

--取款请求
game_cmd.CMD_GR_C_TakeScoreRequest = 
{
	--游戏动作
	{k = "cbAvtivityGame", t = "byte"},
	--取款数目
	{k = "lTakeScore", t = "score"},
	--银行密码
	{k = "szInsurePass", t = "tchar", s = yl.LEN_PASSWORD}
}

-- 转账金币
game_cmd.CMD_GP_C_TransferScoreRequest = 
{
	-- 游戏动作
	{t = "byte", k = "cbAvtivityGame"},
	-- 转账金币
	{t = "score", k = "lTransferScore"},
	-- 目标用户
	{t = "tchar", k = "szAccounts", s = yl.LEN_ACCOUNTS},
	-- 银行密码
	{t = "tchar", k = "szInsurePass", s = yl.LEN_PASSWORD},
	-- 转账备注
	{t = "tchar", k = "szTransRemark", s = yl.LEN_TRANS_REMARK},
}

-- 查询用户
game_cmd.CMD_GR_C_QueryUserInfoRequest = 
{
	-- 游戏动作
	{t = "byte", k = "cbAvtivityGame"},
	-- 昵称赠送
	{t = "byte", k = "cbByNickName"},
	-- 目标用户
	{t = "tchar", k = "szAccounts", s = yl.LEN_ACCOUNTS}
}

--银行资料
game_cmd.CMD_GR_S_UserInsureInfo = 
{
	{k = "cbActivityGame", t = "byte"},									--游戏动作
	{k = "cbEnjoinTransfer", t = "byte"},								--转账开关
	{k = "wRevenueTake", t = "word"},									--税收比例
	{k = "wRevenueTransfer", t = "word"},								--税收比例
	{k = "wRevenueTransferMember", t = "word"},							--税收比例
	{k = "wServerID", t = "word"},										--房间标识
	{k = "lUserScore", t = "score"},									--用户金币
	{k = "lUserInsure", t = "score"},									--银行金币
	{k = "lTransferPrerequisite", t = "score"},							--转账条件
}

--银行成功
game_cmd.CMD_GR_S_UserInsureSuccess = 
{
	--游戏动作
	{k = "cbActivityGame", t = "byte"},
	--用户金币
	{k = "lUserScore", t = "score"},
	--银行金币
	{k = "lUserInsure", t = "score"},
	--描述信息
	{k = "szDescribrString", t = "string"--[[, s = 128]]},
}

--银行失败
game_cmd.CMD_GR_S_UserInsureFailure = 
{
	--游戏动作
	{k = "cbActivityGame", t = "byte"},
	--错误代码
	{k = "lErrorCode", t = "int"},
	--描述消息
	{k = "szDescribeString", t = "string"--[[, s = 128]]},
}

--用户信息
game_cmd.CMD_GR_S_UserTransferUserInfo = 
{
	--游戏动作
	{k = "cbActivityGame", t = "byte"},
	--目标用户
	{k = "dwTargerUserID", t = "dword"},
	--目标用户
	{k = "szAccounts", t = "string"--[[, s = 32 LEN_ACCOUNTS]]},
}

-- 开通结果
game_cmd.CMD_GR_S_UserInsureEnableResult = 
{
	-- 游戏动作
	{t = "byte", k = "cbAvtivityGame"},
	-- 使能标识
	{t = "byte", k = "cbInsureEnabled"},
	--描述消息
	{k = "szDescribeString", t = "string"--[[, s = 128]]},
}
----------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------
--任务命令
------
game_cmd.MDM_GR_TASK					= 6									--任务命令

game_cmd.SUB_GR_TASK_LOAD_INFO			= 1									--加载任务
game_cmd.SUB_GR_TASK_TAKE				= 2									--领取任务
game_cmd.SUB_GR_TASK_REWARD				= 3									--任务奖励
game_cmd.SUB_GR_TASK_GIVEUP				= 4									--任务放弃

game_cmd.SUB_GR_TASK_INFO				= 11								--任务信息
game_cmd.SUB_GR_TASK_FINISH				= 12								--任务完成
game_cmd.SUB_GR_TASK_LIST				= 13								--任务列表
game_cmd.SUB_GR_TASK_RESULT				= 14								--任务结果
game_cmd.SUB_GR_TASK_GIVEUP_RESULT		= 15								--放弃结果

-- 加载任务
game_cmd.CMD_GR_C_LoadTaskInfo = 
{
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 用户密码
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD}
}

-- 放弃任务
game_cmd.CMD_GR_C_TakeGiveUp =
{
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 用户密码
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	-- 机器序列
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 领取任务
game_cmd.CMD_GR_C_TakeTask =
{
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 用户密码
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	-- 机器序列
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 领取奖励
game_cmd.CMD_GR_C_TaskReward =
{
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 用户密码
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
	-- 机器序列
	{t = "tchar", k = "szMachineID", s = yl.LEN_MACHINE_ID},
}

-- 任务参数
game_cmd.tagTaskParameter = 
{
	-- 基本信息
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 任务类型
	{t = "word", k = "wTaskType"},
	-- 任务目标
	{t = "word", k = "wTaskObject"},
	-- 玩家类型
	{t = "byte", k = "cbPlayerType"},
	-- 类型标识
	{t = "word", k = "wKindID"},
	-- 时间限制
	{t = "dword", k = "dwTimeLimit"},

	-- 奖励信息
	-- 奖励金币
	{t = "score", k = "lStandardAwardGold"},
	-- 奖励奖牌
	{t = "score", k = "lStandardAwardMedal"},
	-- 奖励金币
	{t = "score", k = "lMemberAwardGold"},
	-- 奖励奖牌
	{t = "score", k = "lMemberAwardMedal"},

	-- 描述信息
	-- 任务名称
	{t = "string", k = "szTaskName", s = yl.LEN_TASK_NAME},
	-- 任务描述
	{t = "string", k = "szTaskDescribe", s = 320}
}

-- 任务状态
game_cmd.tagTaskStatus = 
{
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 任务进度
	{t = "word", k = "wTaskProgress"},
	-- 任务状态
	{t = "byte", k = "cbTaskStatus"}
}

-- 任务信息
game_cmd.CMD_GR_S_TaskInfo =
{
	-- 任务数量
	{t = "word", k = "wTaskCount"},
	-- 任务状态
	--{t = "table", k = "TaskStatus", d = game_cmd.tagTaskStatus, l = 128}
}

-- 任务完成
game_cmd.CMD_GR_S_TaskFinish =
{
	-- 任务标识
	{t = "word", k = "wTaskID"},
	-- 任务名称
	{t = "string", k = "szTaskName", s = yl.LEN_TASK_NAME}
}

-- 任务结果
game_cmd.CMD_GR_S_TaskResult =
{
	-- 结果信息
	-- 成功标识
	{t = "bool", k = "bSuccessed"},
	-- 命令标识
	{t = "word", k = "wCommandID"},

	-- 财富信息
	-- 当前金币
	{t = "score", k = "lCurrScore"},
	-- 当前元宝
	{t = "score", k = "lCurrIngot"},

	--提示信息
	{t = "string", k = "szNotifyContent"}
}

----------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------
--道具命令
------
game_cmd.MDM_GR_PROPERTY							= 8									--道具命令

--道具信息
game_cmd.SUB_GR_QUERY_PROPERTY						= 1									--道具查询
game_cmd.SUB_GR_GAME_PROPERTY_BUY					= 2									--购买道具
game_cmd.SUB_GR_PROPERTY_BACKPACK					= 3									--背包道具
game_cmd.SUB_GR_PROPERTY_USE						= 4									--物品使用
game_cmd.SUB_GR_QUERY_SEND_PRESENT					= 5									--查询赠送
game_cmd.SUB_GR_PROPERTY_PRESENT					= 6									--赠送道具
game_cmd.SUB_GR_GET_SEND_PRESENT					= 7									--获取赠送

game_cmd.SUB_GR_QUERY_PROPERTY_RESULT				= 101								--道具查询
game_cmd.SUB_GR_QUERY_PROPERTY_RESULT_FINISH		= 111								--道具查询
game_cmd.SUB_GR_GAME_PROPERTY_BUY_RESULT			= 102								--购买道具
game_cmd.SUB_GR_PROPERTY_BACKPACK_RESULT			= 103								--背包道具
game_cmd.SUB_GR_PROPERTY_USE_RESULT					= 104								--物品使用
game_cmd.SUB_GR_QUERY_SEND_PRESENT_RESULT			= 105								--查询赠送
game_cmd.SUB_GR_PROPERTY_PRESENT_RESULT				= 106								--赠送道具
game_cmd.SUB_GR_GET_SEND_PRESENT_RESULT				= 107								--获取赠送

game_cmd.SUB_GR_PROPERTY_SUCCESS					= 201								--道具成功
game_cmd.SUB_GR_PROPERTY_FAILURE					= 202								--道具失败
game_cmd.SUB_GR_PROPERTY_MESSAGE					= 203                     			--道具消息
game_cmd.SUB_GR_PROPERTY_EFFECT						= 204                     			--道具效应
game_cmd.SUB_GR_PROPERTY_TRUMPET					= 205                     			--喇叭消息
game_cmd.SUB_GR_USER_PROP_BUFF						= 206								--道具Buff
game_cmd.SUB_GR_USER_TRUMPET						= 207								--喇叭数量

game_cmd.SUB_GR_GAME_PROPERTY_FAILURE 				= 404	  							--道具失败

-- 道具描述
game_cmd.tagPropertyInfo = 
{
	-- 道具信息
	-- 道具标识
	{t = "word", k = "wIndex"},
	-- 功能类型
	{t = "word", k = "wKind"},
	-- 会员折扣
	{t = "word", k = "wDiscount"},
	-- 发布范围
	{t = "word", k = "wUseArea"},
	-- 服务范围
	{t = "word", k = "wServiceArea"},
	-- 推荐标识
	{t = "word", k = "wRecommend"},
	-- 功能倍数
	{t = "word", k = "wScoreMultiple"},
	-- 支持手机
	{t = "byte", k = "cbSuportMobile"},

	-- 销售价格
	-- 道具金币
	{t = "score", k = "lPropertyGold"},
	-- 道具价格
	{t = "double", k = "dPropertyCash"},
	-- 道具金币
	{t = "score", k = "lPropertyUserMedal"},
	-- 道具金币
	{t = "score", k = "lPropertyLoveLiness"},

	-- 赠送魅力
	{t = "score", k = "lSendLoveLiness"},
	-- 接受魅力
	{t = "score", k = "lRecvLoveLiness"},
	-- 获得金币
	{t = "score", k = "lUseResultsGold"},

	-- 道具名称
	{t = "string", k = "szName", s = 32},
	-- 使用信息
	{t = "string", k = "szRegulationsInfo", s = 256},
}
-- 道具配置
game_cmd.CMD_GR_ConfigProperty =
{
	-- 道具数目
	{t = "byte", k = "cbPropertyCount"},
	-- 道具描述
	--{t = "table", k = "PropertyInfo", d = game_cmd.tagPropertyInfo, l = {128}}
}

-- 购买道具
game_cmd.CMD_GR_C_GamePropertyBuy = 
{
	-- 目标对象
	{t = "dword", k = "dwUserID"},
	-- 道具索引
	{t = "dword", k = "wPropertyIndex"},
	-- 购买数目
	{t = "dword", k = "wItemCount"},
	-- 积分消费
	{t = "byte", k = "cbConsumeType"},
	--用户密码
	{k = "szPassword", t = "tchar", s = yl.LEN_PASSWORD},
	--机器序列
	{k = "szMachineID", t = "tchar", s = yl.LEN_MACHINE_ID},
}

-- 购买结果
game_cmd.CMD_GR_S_PropertyBuyResult = 
{
	-- 购买信息
	-- 道具ID
	{t = "dword", k = "dwPropID"},
	-- 道具数量
	{t = "dword", k = "dwPropCount"},
	-- 银行存款
	{t = "score", k = "lInsureScore"},
	-- 用户元宝
	{t = "score", k = "lUserMedal"},
	-- 魅力值
	{t = "score", k = "lLoveLiness"},
	-- 游戏豆
	{t = "double", k = "dCash"},
	-- 成功标识
	{t = "byte", k = "cbSuccessed"},
	-- 会员等级
	{t = "byte", k = "cbCurrMemberOrder"},

	-- 提示内容
	{t = "string", k = "szNotifyContent"}
}

-- 道具成功
game_cmd.CMD_GR_S_PropertySuccess = 
{
	-- 使用环境
	{t = "byte", k = "cbRequestArea"},
	-- 功能类型
	{t = "word", k = "wKind"},
	-- 购买数目
	{t = "word", k = "wItemCount"},
	-- 道具索引
	{t = "word", k = "wPropertyIndex"},
	-- 目标对象
	{t = "dword", k = "dwSourceUserID"},
	-- 使用对象
	{t = "dword", k = "dwTargetUserID"}
}

-- 道具失败
game_cmd.CMD_GR_PropertyFailure = 
{
	-- 请求区域
	{t = "word", k = "wRequestArea"},
	-- 错误代码
	{t = "dword", k = "lErrorCode"},
	-- 描述信息
	{t = "string", k = "szDescribeString"}
}

-- 使用道具
game_cmd.CMD_GR_C_PropertyUse =
{
	-- 使用者
	{t = "dword", k = "dwUserID"},
	-- 对谁使用
	{t = "dword", k = "dwRecvUserID"},
	-- 道具ID
	{t = "dword", k = "dwPropID"},
	-- 使用数目
	{t = "word", k = "wPropCount"},
}

-- 使用道具
game_cmd.CMD_GR_S_PropertyUse = 
{
	-- 使用者
	{t = "dword", k = "dwUserID"},
	-- 发布范围
	{t = "word", k = "wUseArea"},
	-- 服务范围
	{t = "word", k = "wServiceArea"},
	-- 对谁使用
	{t = "dword", k = "dwRecvUserID"},
	-- 道具ID
	{t = "dword", k = "dwPropID"},
	-- 使用数目
	{t = "dword", k = "wPropCount"},
	-- 剩余数量
	{t = "dword", k = "dwRemainderPropCount"},
	-- 游戏金币
	{t = "score", k = "Score"},

	-- 赠送魅力
	{t = "score", k = "lSendLoveLiness"},
	-- 接受魅力
	{t = "score", k = "lRecvLoveLiness"},
	-- 获得金币
	{t = "score", k = "lUseResultsGold"},
	-- 道具类型
	{t = "dword", k = "dwPropKind"},
	-- 使用的时间
	{t = "dword", k = "tUseTime"},
	-- 有效时长
	{t = "dword", k = "UseResultsValidTime"},
	-- 处理结果
	{t = "dword", k = "dwHandleCode"},
	-- 道具名称
	{t = "string", k = "szName", s = 16},
	-- 会员标识
	{t = "byte", k = "cbMemberOrder"},
	-- 提示内容
	{t = "string", k = "szNotifyContent"}
}

-- 背包查询
game_cmd.CMD_GR_C_BackpackProperty = 
{
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 道具类型
	{t = "dword", k = "dwKindID"},
}

-- 单个背包道具
game_cmd.tagBackpackProperty = 
{
	-- 道具个数
	{t = "int", k = "nCount"},
	-- 道具描述
	{t = "table", k = "Property", d = game_cmd.tagPropertyInfo}
}

-- 背包查询
game_cmd.CMD_GR_S_BackpackProperty = 
{
	-- 用户标识
	{t = "dword", k = "dwUserID"},
	-- 状态
	{t = "dword", k = "dwStatus"},
	-- 个数
	{t = "dword", k = "dwCount"},
	-- 道具信息
	--{t = "table", k = "PropertyInfo", d = game_cmd.tagBackpackProperty, l = {1}}
}

-- 获取赠送
game_cmd.CMD_GR_C_GetSendPresent = 
{
	-- 赠送者
	{t = "dword", k = "dwUserID"},
	-- 用户密码
	{t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
}

-- 获取赠送
game_cmd.CMD_GR_S_GetSendPresent = 
{
	-- 赠送次数
	--
}

-- 赠送道具
game_cmd.CMD_GR_C_PropertyPresent = 
{
	-- 赠送者
	{t = "dword", k = "dwUserID"},
	-- 道具给谁
	{t = "dword", k = "dwRecvGameID"},
	-- 道具ID
	{t = "dword", k = "dwPropID"},
	-- 使用数目
	{t = "word", k = "wPropCount"},
	-- 目标类型
	{t = "word", k = "wType"},
	-- 对谁使用
	{t = "tchar", k = "szRecvNickName", s = 16}
}

-- 赠送道具
game_cmd.CMD_GR_S_PropertyPresent = 
{
	-- 赠送者
	{t = "dword", k = "dwUserID"},
	-- 道具给谁
	{t = "dword", k = "dwRecvGameID"},
	-- 道具ID
	{t = "dword", k = "dwPropID"},
	-- 使用数目
	{t = "word", k = "wPropCount"},
	-- 目标类型
	{t = "word", k = "wType"},
	-- 对谁使用
	{t = "tchar", k = "szRecvNickName", s = 16},
	-- 返回码
	{t = "int", k = "nHandleCode"},
	-- 提示内容
	{t = "string", k = "szNotifyContent", s = 64}
}

----------------------------------------------------------------------------------------------


----------------------------------------------
--/框架命令
game_cmd.MDM_GF_FRAME				= 100									--框架命令

--用户命令
game_cmd.SUB_GF_GAME_OPTION			= 1										--游戏配置
game_cmd.SUB_GF_USER_READY			= 2										--用户准备
game_cmd.SUB_GF_LOOKON_CONFIG		= 3										--旁观配置
game_cmd.SUB_GF_DISTRIBUTE_AFFIRM	= 4										--分组确认

--聊天命令
game_cmd.SUB_GF_USER_CHAT			= 10									--用户聊天
game_cmd.SUB_GF_USER_EXPRESSION		= 11									--用户表情
game_cmd.SUB_GF_USER_VOICE 			= 12 									--用户语音

--系统消息
game_cmd.SUB_GF_SYSTEM_MESSAGE		= 200									--系统消息
game_cmd.SUB_GF_ACTION_MESSAGE		= 201									--动作消息

------
--发包结构
--用户聊天
game_cmd.CMD_GF_C_UserChat = 
{
	{k = "wChatLength", t = "word"},										--内容长度
	{k = "dwChatColor", t = "dword"},										--颜色
	{k = "dwTargerUserID", t = "dword"},									--目标用户id
	{k = "szChatString", t = "tchar", s = yl.LEN_USER_CHAT},				--聊天内容
}

--用户表情
game_cmd.CMD_GF_C_UserExpression = 
{
	{k = "wItemIndex", t = "word"},											--表情索引
	{k = "dwTargerUserID", t = "dword"},									--目标用户id
}

------
--回包结构
--用户聊天
game_cmd.CMD_GF_S_UserChat = 
{
	{k = "wChatLength", t = "word"},										--内容长度
	{k = "dwChatColor", t = "dword"},										--颜色
	{k = "dwSendUserID", t = "dword"},										--发送用户id
	{k = "dwTargerUserID", t = "dword"},									--接收用户id
	--这里不能限定读取长度，会异常。。。, s = yl.LEN_USER_CHAT
	{k = "szChatString", t = "string"},										--聊天内容
}

--用户表情
game_cmd.CMD_GF_S_UserExpression = 
{
	{k = "wItemIndex", t = "word"},											--表情索引
	{k = "dwSendUserID", t = "dword"},										--发送用户id
	{k = "dwTargerUserID", t = "dword"},									--目标用户id
}

----------------------------------------------

----------------------------------------------
--游戏命令
game_cmd.MDM_GF_GAME 							= 200 		 				--游戏命令

--其他信息
game_cmd.DTP_GR_TABLE_PASSWORD					= 1							--桌子密码			
----------------------------------------------
return game_cmd