--
-- Author: zhong
-- Date: 2016-12-03 11:36:14
--
-- 私人房 命令
-- 登陆服务器: private_cmd.login
-- 游戏服务器: private_cmd.game

local private_struct = appdf.req(appdf.CLIENT_SRC .. "privatemode.header.Struct_Private")
local private_define = appdf.req(appdf.CLIENT_SRC .. "privatemode.header.Define_Private")

local private_cmd = {}
private_cmd.struct = private_struct
private_cmd.define = private_define

----------------------------------------------------------------------------------------------
-- 登陆服务器
local login = {}
------
-- 命令
------
-- 私人房间
login.MDM_MB_PERSONAL_SERVICE = 200

login.SUB_MB_QUERY_GAME_SERVER = 204                                    -- 创建房间
login.SUB_MB_QUERY_GAME_SERVER_RESULT = 205                             -- 创建结果
login.SUB_MB_SEARCH_SERVER_TABLE = 206                                  -- 搜索房间
login.SUB_MB_SEARCH_RESULT = 207                                        -- 搜索结果

login.SUB_MB_GET_PERSONAL_PARAMETER = 208                               -- 私人房间配置
login.SUB_MB_PERSONAL_PARAMETER = 209                                   -- 私人房间属性
login.SUB_MB_PERSONAL_FEE_PARAMETER = 212                               -- 私人房费用配置

login.SUB_MB_QUERY_PERSONAL_ROOM_LIST = 210                             -- 私人房列表
login.SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT = 211                      -- 私人房列表

login.SUB_MB_DISSUME_SEARCH_SERVER_TABLE = 213                          -- 为解散桌子搜索ID
login.SUB_MB_DISSUME_SEARCH_RESULT = 214                                -- 解散时搜索私人房间返回结果

login.SUB_GR_USER_QUERY_ROOM_SCORE = 216                                -- 私人房间单个玩家请求房间成绩
login.SUB_GR_USER_QUERY_ROOM_SCORE_RESULT = 217                         -- 私人房间单个玩家请求房间成绩结果
login.SUB_GR_USER_QUERY_ROOM_SCORE_RESULT_FINSIH = 218                  -- 私人房间单个玩家请求房间成绩完成

login.SUB_MB_ROOM_CARD_EXCHANGE_TO_SCORE = 221                          -- 房卡兑换游戏币
login.SUB_GP_EXCHANGE_ROOM_CARD_RESULT = 222                            -- 房卡兑换游戏币结果
------
-- 消息结构
------
-- 创建房间
login.CMD_MB_QueryGameServer = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 类型ID
    {t = "dword", k = "dwKindID"},
    -- 是否参与 (0 不参与; 1 参与)
    {t = "byte", k = "cbIsJoinGame"},
}

-- 创建结果
login.CMD_MB_QueryGameServerResult = 
{
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 是否可以创建房间
    {t = "bool", k = "bCanCreateRoom"},
    -- 错误描述
    {t = "string", k = "szErrDescrybe"}
}

-- 强制解散搜索房间
login.CMD_MB_SearchServerTable = 
{
    -- 房间ID
    {t = "tchar", k = "szServerID", s = private_define.ROOM_ID_LEN},
}

-- 进入游戏搜索房间
login.CMD_MB_SerchServerTableEnter = 
{
    -- 房间ID
    {t = "tchar", k = "szServerID", s = private_define.ROOM_ID_LEN},
    -- 类型ID
    {t = "dword", k = "dwKindID"},
}

-- 搜索结果
login.CMD_MB_SearchResult = 
{
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
}

-- 查询私人房间配置
login.CMD_MB_GetPersonalParameter = 
{
    -- 类型ID
    {t = "dword", k = "dwKindID"},
}

-- 查询私人房列表
login.CMD_MB_QeuryPersonalRoomList = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 类型ID
    {t = "dword", k = "dwKindID"},
}

-- 私人房间列表信息
login.CMD_MB_PersonalRoomInfoList = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 配置信息
    {t = "table", k = "PersonalRoomInfo", d = private_struct.tagPersonalRoomInfo, l = {private_define.MAX_CREATE_PERSONAL_ROOM}}
}

-- 解散时搜索结果
login.CMD_MB_DissumeSearchResult = 
{
    -- 房间ID
    {t = "dword", k = "dwServerID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
}

-- 房卡兑换游戏币
login.CMD_GP_ExchangeScoreByRoomCard = 
{
    -- 用户标识
    {t = "dword", k = "dwUserID"},
    -- 房卡数量
    {t = "score", k = "lRoomCard"},
    -- machineid
    {t = "tchar", k = "szMachieID", s = yl.LEN_MACHINE_ID},
}

-- 房卡兑换游戏币结果
login.CMD_GP_ExchangeRoomCardResult = 
{
    -- 成功标识
    {t = "bool", k = "bSuccessed"},
    -- 当前游戏币
    {t = "score", k = "lCurrScore"},
    -- 当前房卡
    {t = "score", k = "lRoomCard"},
    -- 提示内容
    {t = "string", k = "szNotifyContent"}
}

private_cmd.login = login
-- 登陆服务器
----------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------
-- 游戏服务器
local game = {}
------
-- 命令
------
-- 私人房间
game.MDM_GR_PERSONAL_TABLE = 210                                        -- 

game.SUB_GR_CREATE_TABLE = 1                                            -- 创建桌子
game.SUB_GR_CREATE_SUCCESS = 2                                          -- 创建成功
game.SUB_GR_CREATE_FAILURE = 3                                          -- 创建失败
game.SUB_GR_CANCEL_TABLE = 4                                            -- 解散桌子
game.SUB_GR_CANCEL_REQUEST = 5                                          -- 请求解散
game.SUB_GR_REQUEST_REPLY = 6                                           -- 请求答复
game.SUB_GR_REQUEST_RESULT = 7                                          -- 请求结果
game.SUB_GR_WAIT_OVER_TIME = 8                                          -- 超时等待
game.SUB_GR_PERSONAL_TABLE_TIP = 9                                      -- 提示信息
game.SUB_GR_PERSONAL_TABLE_END = 10                                     -- 结束消息
game.SUB_GR_HOSTL_DISSUME_TABLE = 11                                    -- 房主强制解散桌子
game.SUB_GR_CANCEL_TABLE_RESULT = 13                                    -- 强制解散结果
game.SUB_GR_CURRECE_ROOMCARD_AND_BEAN = 16                              -- 强制解散桌子后的游戏豆和房卡数量
game.SUB_GR_CHANGE_CHAIR_COUNT = 17                                     -- 改变椅子数量
game.SUB_GF_PERSONAL_MESSAGE = 202                                      -- 私人房消息

game.CANCELTABLE_REASON_PLAYER = 0                                      -- 玩家取消
game.CANCELTABLE_REASON_SYSTEM = 1                                      -- 系统取消
game.CANCELTABLE_REASON_ENFOCE = 2                                      -- 强制解散桌子
game.CANCELTABLE_REASON_ERROR = 3                                       -- 错误取消

------
-- 消息结构
------
-- 创建桌子
game.CMD_GR_CreateTable = 
{
    -- 底分设置
    {t = "score", k = "lCellScore"},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 参与游戏的人数
    {t = "word", k = "wJoinGamePeopleCount"},
    -- 单独税率
    {t = "dword", k = "dwRoomTax"},
    -- 密码设置
    {t = "tchar", k = "szPassword", s = yl.LEN_PASSWORD},
    -- 游戏规则，0位标识，0表示未设置，1表示设置设置，后面+自定义规则数据
    {t = "byte", k = "cbGameRule", l = private_define.RULE_LEN},
}

-- 创建成功
game.CMD_GR_CreateSuccess = 
{
    -- 房间编号
    {t = "string", k = "szServerID", s = private_define.ROOM_ID_LEN},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 游戏豆
    {t = "double", k = "dBeans"},
    -- 房卡数量
    {t = "score", k = "lRoomCard"},
}

-- 创建失败
game.CMD_GR_CreateFailure = 
{
    -- 错误代码
    {t = "int", k = "lErrorCode"},
    -- 描述消息
    {t = "string", k = "szDescribeString"},
}

-- 取消桌子
game.CMD_GR_CancelTable = 
{
    -- 取消原因
    {t = "dword", k = "dwReason"},
    -- 描述消息
    {t = "string", k = "szDescribeString"--[[, s = 128]]},
}

-- 请求解散
game.CMD_GR_CancelRequest = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
    -- 椅子ID
    {t = "dword", k = "dwChairID"},
}

-- 请求答复
game.CMD_GR_RequestReply = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
    -- 用户答复(1 同意; 0 不同意)
    {t = "byte", k = "cbAgree"},
}

-- 请求结果
game.CMD_GR_RequestResult = 
{
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
    -- 请求结果
    {t = "byte", k = "cbResult"},
}

-- 超时等待
game.CMD_GR_WaitOverTime = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
}

-- 提示信息
game.CMD_GR_PersonalTableTip = 
{
    -- 桌主ID
    {t = "dword", k = "dwTableOwnerUserID"},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 已玩局数
    {t = "dword", k = "dwPlayCount"},
    -- 已玩时间
    {t = "dword", k = "dwPlayTime"},
    -- 游戏底分
    {t = "score", k = "lCellScore"},
    -- 初始分数
    {t = "score", k = "lIniScore"},
    -- 房间编号
    {t = "string", k = "szServerID", s = private_define.ROOM_ID_LEN},
    -- 是否参与游戏
    {t = "byte", k = "cbIsJoinGame"},
    -- 金币场0, 积分场1
    {t = "byte", k = "cbIsGoldOrGameScore"},
}

-- 结束消息
game.CMD_GR_PersonalTableEnd = 
{
    {t = "string", k = "szDescribeString", s = 128},
    {t = "score", k = "lScore", l = {100}},
    -- 特殊信息长度 
    {t = "int", k = "nSpecialInfoLen"},
    -- 特殊信息数据
    --{t = "byte", k = "cbSpecialInfo", l = {200}}
}

-- 房主强制解散
game.CMD_GR_HostDissumeGame = 
{
    -- 用户ID
    {t = "dword", k = "dwUserID"},
    -- 桌子ID
    {t = "dword", k = "dwTableID"},
}

-- 解散桌子
game.CMD_GR_DissumeTable = 
{
    -- 是否解散成功
    {t = "byte", k = "cbIsDissumSuccess"},
    -- 桌子ID
    {t = "string", k = "szRoomID", s = private_define.ROOM_ID_LEN},
    -- 解散时间
    {t = "table", k = "sysDissumeTime", d = private_struct.SYSTEMTIME},
    -- 用户信息
    {t = "table", k = "PersonalUserScoreInfo", d = private_struct.tagPersonalUserScoreInfo, l = {private_define.PERSONAL_ROOM_CHAIR}}
}

-- 私人房消息
game.Personal_Room_Message = 
{
    -- 提示信息
    {t = "string", k = "szMessage", s = 260},
    -- 信息类型,暂时无用
    {t = "byte", k = "cbMessageType"},
}

-- 强制解散桌子后的游戏豆和房卡
game.CMD_GR_CurrenceRoomCardAndBeans = 
{
    -- 游戏豆
    {t = "double", k = "dbBeans"},
    -- 房卡
    {t = "score", k = "lRoomCard"},
}

-- 改变椅子数量
game.CMD_GR_ChangeChairCount = 
{
    -- 椅子数量
    {t = "dword", k = "dwChairCount"},
}

private_cmd.game = game
-- 游戏服务器
----------------------------------------------------------------------------------------------
return private_cmd