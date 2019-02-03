--[[
	更多菜单
	2017_09_06 MXM
]]

local MoreMenuLayer = class("MoreMenuLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")


local Option = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.other.OptionLayer")

local UserInfo = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.UserInfoLayer")
local PromoterInputLayer = import(".PromoterInputLayer")

local Checkin = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.CheckinLayer")
local CheckinFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.CheckinFrame")

MoreMenuLayer.BT_CONFIG 		= 1
MoreMenuLayer.BT_CHECKIN 		= 2
MoreMenuLayer.BT_SHARE 			= 3

function MoreMenuLayer:ctor(scene)
	
	self._scene = scene
	
	 --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
	
	local btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	
	--加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/MoreMenuLayer.csb", self)
	self.m_rootLayer = rootLayer
	self.m_moremenuLayer = csbNode
	
	--菜单栏
    self.m_menulayout = csbNode:getChildByName("layout_menu")
    self.m_menulayout:retain()
    self.m_menulayout:release()
    self.m_menulayout:setScale(1.0, 0.01)--1.0, 0.01 --0.2, 1.0
    --self.m_menulayout:setVisible(false)
	
	--设置
    --self._btConfig = self.m_menulayout:getChildByName("Button_Setup")
    self._btConfig = csbNode:getChildByName("Button_Setup")
    self._btConfig:setTag(MoreMenuLayer.BT_CONFIG)
	self._btConfig:addTouchEventListener(btcallback)
	
	--签到
    --self._btCheckin = self.m_menulayout:getChildByName("Button_Checkin")
    self._btCheckin = csbNode:getChildByName("Button_Checkin")
    self._btCheckin:setTag(MoreMenuLayer.BT_CHECKIN)
	self._btCheckin:addTouchEventListener(btcallback)
	
	--分享
	self._btShare = self.m_menulayout:getChildByName("Button_Share")
	self._btShare:setTag(MoreMenuLayer.BT_SHARE)
	self._btShare:addTouchEventListener(btcallback)
	
	--加载动画
    local call = cc.CallFunc:create(function()
        self:setVisible(true)
    end)
    local scale = cc.ScaleTo:create(0.2, 1.0)
    self.m_actShowMenu = cc.Sequence:create(call, scale)
    ExternalFun.SAFE_RETAIN(self.m_actShowMenu)

    local scale1 = cc.ScaleTo:create(0.2, 1.0, 0.01)--0.2, 1.0, 0.0001  --0.2, 0.0001
    local call1 = cc.CallFunc:create(function( )
        self:setVisible(false)
    end)
    self.m_actHideMenu = cc.Sequence:create(scale1, call1)
    ExternalFun.SAFE_RETAIN(self.m_actHideMenu)

    self:setVisible(false)
	
end

function MoreMenuLayer:onButtonClickedEvent( tag,ref )
	ExternalFun.playClickEffect()
	if tag == MoreMenuLayer.BT_CONFIG then
			local _optionlayer = Option:create(self._scene)
			self.m_moremenuLayer:addChild(_optionlayer)	
	elseif tag == MoreMenuLayer.BT_CHECKIN then
			local checkLayer = Checkin:create(self._scene)
			self.m_moremenuLayer:addChild(checkLayer)
	elseif tag == MoreMenuLayer.BT_SHARE then
			local shareLayer = PromoterInputLayer:create(self._scene)
			self.m_moremenuLayer:addChild(shareLayer)
	end
end

function MoreMenuLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function MoreMenuLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local _menulayout = self.m_menulayout
    pos = _menulayout:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, _menulayout:getContentSize().width, _menulayout:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:showLayer(false)
		
    end
end

function MoreMenuLayer:showLayer(m_bshowMenu)
	local ani = nil
    if m_bshowMenu then
        ani = self.m_actShowMenu
    else 
        ani = self.m_actHideMenu
    end

    if nil ~= ani then
        self.m_menulayout:stopAllActions()
        self.m_menulayout:runAction(ani)
        self:setVisible(m_bshowMenu)
    end
end

function MoreMenuLayer:onExit( )
	
end

return MoreMenuLayer