--[[
	服务条款页面
	2016_06_03 Ravioyla
	功能：显示服务条款
]]

local ServiceLayer = class("ServiceLayer",function()
	local ServiceLayer =  display.newLayer()
    return ServiceLayer
end)

ServiceLayer.BT_EXIT		= 5

ServiceLayer.BT_CONFIRM		= 8
ServiceLayer.BT_CANCEL		= 9

function ServiceLayer:ctor()
	self:setContentSize(yl.WIDTH,yl.HEIGHT)
	local this = self
    cc.SpriteFrameCache:getInstance():addSpriteFrames("public/public.plist")

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

	local areaWidth = yl.WIDTH
	local areaHeight = yl.HEIGHT

    --背景
    display.newSprite("base/background_2.jpg")
        :move(yl.WIDTH/2,yl.HEIGHT/2)
        :addTo(self)

	--上方背景
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
    end
	--标题
	display.newSprite("Service/title_service.png")
		:move(areaWidth/2,yl.HEIGHT-51)
		:addTo(self)
	--返回
	ccui.Button:create("base/bt_return_0.png","base/bt_return_1.png")
    	:move(75,yl.HEIGHT-51)
    	:setTag(ServiceLayer.BT_EXIT)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

    --背景
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,325)
        self:addChild(sp)
    end
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_2.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,326)
        self:addChild(sp)
    end

	-- 读取文本
	self._scrollView = ccui.ScrollView:create()
									  :setContentSize(cc.size(1130,400))
									  :setAnchorPoint(cc.p(0.5, 0.5))
									  :setPosition(cc.p(areaWidth/2, 370))
									  :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
									  :setBounceEnabled(true)
									  :setScrollBarEnabled(false)
									  :addTo(self)

	local str = cc.FileUtils:getInstance():getStringFromFile("Service/Service.txt")
	self._strLabel = cc.Label:createWithTTF(str, "base/fonts/round_body.ttf", 25)
							 :setAnchorPoint(cc.p(0.5, 0))
						     :setLineBreakWithoutSpace(true)
						     :setMaxLineWidth(1100)
							 :setAlignment(cc.TEXT_ALIGNMENT_LEFT)
							 :setTextColor(cc.c4b(195,199,239,255))
						     :addTo(self._scrollView)
	self._strLabel:setPosition(cc.p(1130/2, 0))
	self._scrollView:setInnerContainerSize(cc.size(1130, self._strLabel:getContentSize().height))

	--按钮
	ccui.Button:create("Service/bt_service_confirm_0.png","Service/bt_service_confirm_1.png")
    	:move(870,103)
    	:setTag(ServiceLayer.BT_CONFIRM)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

    ccui.Button:create("Service/bt_service_cancel_0.png","Service/bt_service_cancel_1.png")
    	:move(465,103)
    	:setTag(ServiceLayer.BT_CANCEL)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

end

--按键监听
function ServiceLayer:onButtonClickedEvent(tag,sender)
	ExternalFun.playClickEffect()
	if tag == ServiceLayer.BT_EXIT then
		self:getParent():getParent():onShowRegister()
	elseif tag == ServiceLayer.BT_CONFIRM then
		self:getParent():getParent()._registerView:setAgreement(true)
		self:getParent():getParent():onShowRegister()
	elseif tag == ServiceLayer.BT_CANCEL then
		self:getParent():getParent()._registerView:setAgreement(false)
		self:getParent():getParent():onShowRegister()
	end
end

return ServiceLayer