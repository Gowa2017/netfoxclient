--
-- Author: zhong
-- Date: 2016-12-30 14:50:07
--
-- 分享到指定平台
local TargetShareLayer = class("TargetShareLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local TAG_MASK = 101
local TAG_MASK2 = 102
-- 微信好友分享
local BT_WECHAT = 103
-- 朋友圈分享
local BT_CYCLE = 104
-- 我的好友分享
local BT_FRIEND = 105

-- callback 
-- @param[target] 分享平台
-- @param[bMyFriend] 是否分享到好友
function TargetShareLayer:ctor( callback )
    self.m_shareCallBack = callback
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("public/TargetShareLayer.csb", self)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local panel = csbNode:getChildByName("panel_holder")
    self.m_panelHolder = panel

    -- 微信好友分享
    local btn = panel:getChildByName("btn_wechat")
    btn:setTag(BT_WECHAT)
    btn:addTouchEventListener( touchFunC )

    -- 朋友圈分享
    btn = panel:getChildByName("btn_cycle")
    btn:setTag(BT_CYCLE)
    btn:addTouchEventListener( touchFunC )

    -- 好友分享
    btn = panel:getChildByName("btn_friend")
    btn:setTag(BT_FRIEND)
    btn:addTouchEventListener( touchFunC )

    panel:stopAllActions()
    local call = cc.CallFunc:create(function()
        ExternalFun.popupTouchFilter(1, false)
    end)
    local move = cc.MoveTo:create(0.2, cc.p(0, 0))
    local call2 = cc.CallFunc:create(function()
        ExternalFun.dismissTouchFilter()
    end)
    local seq = cc.Sequence:create(call, move, call2)
    panel:runAction(seq)
end

function TargetShareLayer:onButtonClickedEvent(tag, ref)
    if TAG_MASK == tag or TAG_MASK2 == tag then
        self:hide()
    elseif BT_WECHAT == tag then        
        self:hide(cc.CallFunc:create(function()
            if type(self.m_shareCallBack) == "function" then
                self.m_shareCallBack(yl.ThirdParty.WECHAT)
            end
        end))
    elseif BT_CYCLE == tag then        
        self:hide(cc.CallFunc:create(function()
            if type(self.m_shareCallBack) == "function" then
                self.m_shareCallBack(yl.ThirdParty.WECHAT_CIRCLE)
            end
        end))
    elseif BT_FRIEND == tag then
        self:hide(cc.CallFunc:create(function()
            if type(self.m_shareCallBack) == "function" then
                self.m_shareCallBack(nil, true)
            end
        end))
    end
end

function TargetShareLayer:hide( callfun )
    self.m_panelHolder:stopAllActions()
    local call = cc.CallFunc:create(function()
        ExternalFun.popupTouchFilter(1, false)
    end)
    local move = cc.MoveTo:create(0.2, cc.p(0, -150))
    local call2 = cc.CallFunc:create(function()
        ExternalFun.dismissTouchFilter()
        self:removeFromParent()
    end)
    if nil ~= callfun then
        seq = cc.Sequence:create(call, move, callfun, call2)
        self.m_panelHolder:runAction(seq)
    else
        seq = cc.Sequence:create(call, move, call2)
        self.m_panelHolder:runAction(seq)
    end    
end

return TargetShareLayer