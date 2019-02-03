--
-- Author: zhong
-- Date: 2017-01-02 10:05:02
--
-- 私人房信息
local PrivateLayerModel = class("PrivateLayerModel", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- gameLayer ==> GameLayer.lua 
function PrivateLayerModel:ctor(gameLayer)
    ExternalFun.registerNodeEvent(self)
    self._gameLayer = gameLayer
end

-- 刷新信息
function PrivateLayerModel:onRefreshInfo()
    
end

-- 刷新邀请按钮
function PrivateLayerModel:onRefreshInviteBtn()
end

-- 私人房游戏结束
function PrivateLayerModel:onPriGameEnd( cmd_table )

end

return PrivateLayerModel