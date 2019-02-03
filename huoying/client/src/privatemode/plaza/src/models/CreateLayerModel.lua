--
-- Author: zhong
-- Date: 2016-12-31 09:41:59
--
local CreateLayerModel = class("CreateLayerModel", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd_private = appdf.req(appdf.CLIENT_SRC .. "privatemode.header.CMD_Private")

function CreateLayerModel:ctor(scene)
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
    self._cmd_pri_login = cmd_private.login
    self._cmd_pri_game = cmd_private.game
end

-- 刷新界面
function CreateLayerModel:onRefreshInfo()
    print("base refresh")
end

-- 获取邀请分享内容
function CreateLayerModel:getInviteShareMsg( roomDetailInfo )
    print("base get invite")
    return {title = "", content = ""}
end

------
-- 网络消息
------
-- 私人房创建成功
function CreateLayerModel:onRoomCreateSuccess()
    -- 创建成功
    if 0 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame then
        -- 非必须加入
        PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATERESULT, nil, self._scene)
    end
    self:onRefreshInfo()
end

-- 私人房登陆完成
function CreateLayerModel:onLoginPriRoomFinish()
    print("base login finish")
    return false
end

return CreateLayerModel