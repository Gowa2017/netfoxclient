--
-- Author: zhong
-- Date: 2016-12-03 14:36:31
--
local private_define = {}

private_define.ROOM_ID_LEN = 7                                              -- 私人房ID长度
private_define.MAX_CREATE_PERSONAL_ROOM = 20                                -- 个人创建私人房最大数量
private_define.PERSONAL_ROOM_CHAIR = 8
private_define.RULE_LEN = 100                                               -- 私人房补充规则
private_define.LEN_PERSONAL_ROOM_SCORE = 854                                -- sizeof(tagQueryPersonalRoomUserScore)
private_define.LEN_PERSONAL_TABLE_PARAMETER = 24                            -- sizeof(tagPersonalTableParameter)

local tabModule = {}
-- 模块定义
tabModule.PRIMODULE                = "client.src.privatemode."              -- 私人房模块
tabModule.PRIHEADER                = "client.src.privatemode.header."       -- 私人房header
tabModule.PLAZAMODULE              = "client.src.privatemode.plaza.src."    -- 大厅模块
tabModule.GAMEMODULE               = "client.src.privatemode.game."         -- 游戏模块
private_define.tabModule = tabModule

-- 场景/层 tag定义
local tabLayTag = {}
tabLayTag.LAYER_ROOMLIST           = 1001                                   -- 私人房模式房间列表
tabLayTag.LAYER_CREATEPRIROOME     = 1002                                   -- 创建私人房界面
tabLayTag.LAYER_MYROOMRECORD       = 1003                                   -- 我的房间(记录)
tabLayTag.LAYER_HELP               = 1004                                   -- 帮助界面
tabLayTag.LAYER_ROOMID             = 1005                                   -- 房间ID输入
tabLayTag.LAYER_CREATERESULT       = 1006                                   -- 创建结果
tabLayTag.LAYER_EXCHANGESCORE      = 1007                                   -- 房卡兑换游戏币
tabLayTag.LAYER_BUYCARD            = 1008                                   -- 游戏豆购买房卡
tabLayTag.LAYER_FRIENDLIST         = 1009                                   -- 好友分享列表
private_define.tabLayTag = tabLayTag

-- 游戏服务器登陆操作定义
local tabLoginAction = {}
tabLoginAction.ACT_NULL            = 2000                                   -- 未定义
tabLoginAction.ACT_CREATEROOM      = 2001                                   -- 创建房间
tabLoginAction.ACT_SEARCHROOM      = 2002                                   -- 搜索房间
tabLoginAction.ACT_DISSUMEROOM     = 2003                                   -- 解散房间
tabLoginAction.ACT_ENTERTABLE      = 2004                                   -- 进入游戏

private_define.tabLoginAction = tabLoginAction
return private_define