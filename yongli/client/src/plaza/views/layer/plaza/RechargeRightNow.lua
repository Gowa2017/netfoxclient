local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
--头像选择界面
local RechargeRightNow = class("RechargeRightNow", cc.BaseLayer)

function RechargeRightNow:ctor()
	self.super.ctor(self)

    -- 加载csb资源
	local csbPath = "Shop/RechargeRightNow.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	local setbg = csbNode:getChildByName("setbg")
	
	
	self._btnClose = setbg:getChildByName("btn_close")
	self._btnClose:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	self._btnCancel = setbg:getChildByName("btn_cancel")
	self._btnCancel:addClickEventListener(handler(self, self.onButtonClickedEvent))	
	
	self._btnOk = setbg:getChildByName("btn_ok")
	self._btnOk:addClickEventListener(handler(self, self.onButtonClickedEvent))	
	
	
		-- 添加点击监听事件 遮罩
    local mask = csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = csbNode:getChildByName("setbg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.RECHARGE_RIGHT_NOW})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function RechargeRightNow:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function RechargeRightNow:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function RechargeRightNow:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	if (senderName == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.RECHARGE_RIGHT_NOW})
	elseif (senderName == "btn_ok") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.RECHARGE_RIGHT_NOW})
		local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
		if (appConfigProxy._appStoreSwitch == 0) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
		end
	elseif (senderName == "btn_cancel") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.RECHARGE_RIGHT_NOW})
	end
end

function RechargeRightNow:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end


function RechargeRightNow:onFocusEvent(sender, index, eventType)
	if (ccui.RadioButtonEventType.selected == eventType) then
		ExternalFun.playClickEffect()
		self._faceindex = index
	end
end

return RechargeRightNow