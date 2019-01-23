--
-- Author: zhong
-- Date: 2016-12-03 13:01:43
--

local private_define = appdf.req(appdf.CLIENT_SRC .. "privatemode.header.Define_Private")
local private_struct = {}

-- 时间定义
private_struct.SYSTEMTIME = 
{
    {t = "word", k = "wYear"},
    {t = "word", k = "wMonth"},
    {t = "word", k = "wDayOfWeek"},
    {t = "word", k = "wDay"},
    {t = "word", k = "wHour"},
    {t = "word", k = "wMinute"},
    {t = "word", k = "wSecond"},
    {t = "word", k = "wMilliseconds"},
}

-- 私人房间配置
private_struct.tagPersonalTableParameter = 
{
    -- 时间限制
    {t = "dword", k = "dwDrawTimeLimit"},
    -- 局数限制
    {t = "dword", k = "dwDrawCountLimit"},
    -- 初始分数
    {t = "score", k = "lIniScore"},
    -- 创建费用
    {t = "score", k = "lFeeScore"},
}

-- 私人房间玩家信息
private_struct.tagPersonalUserScoreInfo = 
{
    -- 玩家ID
    {t = "dword", k = "dwUserID"},
    -- 玩家昵称
    {t = "string", k = "szUserNicname", s = yl.LEN_NICKNAME},

    -- 积分信息
    -- 用户分数
    {t = "score", k = "lScore"},
    -- 用户成绩
    {t = "score", k = "lGrade"},
    -- 税收总数
    {t = "score", k = "lTaxCount"},
}

-- 用于请求单个私人房间信息
private_struct.tagPersonalRoomInfo = 
{
    {t = "string", k = "szRoomID", s = private_define.ROOM_ID_LEN},
    -- 消耗类型(0 游戏豆; 1 房卡)
    {t = "byte", k = "cbCardOrBean"},
    -- 消耗游戏豆 或 房卡的数量
    {t = "score", k = "lFeeCardOrBeanCount"},
    -- 私人放进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},
    -- 是否解散房间
    {t = "byte", k = "cbIsDisssumRoom"},
    -- 私人房间创建时间
    {t = "table", k = "sysCreateTime", d = private_struct.SYSTEMTIME},
    -- 私人房间结束时间
    {t = "table", k = "sysDissumeTime", d = private_struct.SYSTEMTIME},
    -- 税收总数
    {t = "score", k = "lTaxCount"},
    -- 私人房间所有玩家信息
    {t = "table", k = "PersonalUserScoreInfo", d = private_struct.tagPersonalUserScoreInfo, l = {private_define.PERSONAL_ROOM_CHAIR}}
}

private_struct.tagQueryPersonalRoomUserScore = 
{
    {t = "string", k = "szRoomID", s = private_define.ROOM_ID_LEN},
   -- 房主昵称
    {t = "string", k = "szUserNicname", s = yl.LEN_NICKNAME},
   -- 私人放进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},
    -- 私人房间创建时间
    {t = "table", k = "sysCreateTime", d = private_struct.SYSTEMTIME},
    -- 私人房间结束时间
    {t = "table", k = "sysDissumeTime", d = private_struct.SYSTEMTIME},

    -- 私人房间所有玩家信息
    {t = "table", k = "PersonalUserScoreInfo", d = private_struct.tagPersonalUserScoreInfo, l = {private_define.PERSONAL_ROOM_CHAIR}}
}

private_struct.tagPersonalRoomOption = 
{
    -- 消耗类型(0 游戏豆; 1 房卡)
    {t = "byte", k = "cbCardOrBean"},
    -- 消耗数量
    {t = "score", k = "lFeeCardOrBeanCount"},
    -- 是否参与 (0 不参与; 1 参与)
    {t = "byte", k = "cbIsJoinGame"},
    -- 房间最小人数
    {t = "byte", k = "cbMinPeople"},
    -- 房间最大人数
    {t = "byte", k = "cbMaxPeople"},
    -- 私人房的最大底分
    {t = "score", k = "lMaxCellScore"},
    -- 玩家初始分
    {t = "score", k = "lIniScore"},

    -- 玩家能够创建的私人房的最大数目
    {t = "word", k = "wCanCreateCount"},
    -- 私人房进行游戏的最大局数
    {t = "dword", k = "dwPlayTurnCount"},
    -- 私人房进行游戏的最大时间 单位秒
    {t = "dword", k = "dwPlayTimeLimit"},

    -- 一局开始多长时间后解散桌子 单位秒
    {t = "dword", k = "dwTimeAfterBeginCount"},
    -- 掉线多长时间后解散桌子 单位秒
    {t = "dword", k = "dwTimeOffLineCount"},
    -- 多长时间未开始游戏解散桌子 单位秒
    {t = "dword", k = "dwTimeNotBeginGame"},
    -- 私人房创建多长时间后无人坐桌解散桌子
    {t = "dword", k = "dwTimeAfterCreateRoom"},
}

return private_struct