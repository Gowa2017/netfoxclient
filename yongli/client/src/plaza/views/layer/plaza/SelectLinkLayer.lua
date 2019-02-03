local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
--头像选择界面
local SelectLinkLayer = class("SelectLinkLayer", cc.BaseLayer)

function SelectLinkLayer:ctor(msg)
	self.super.ctor(self)

    -- 加载csb资源
	local csbPath = "Userinfo/SelectLinkLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	
	self._msg = msg or ""
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self._textMsg = ExternalFun.seekWigetByName(self._csbNode, "Text_1")
	self._textMsg:setString(self._msg)
	
	local res, result = MultiPlatform:getInstance():copyToClipboard(msg)
	if true == res then
		--showToast(self, "复制到剪贴板成功!", 1)
	else
		if type(result) == "string" then
			showToast(self, result, 1, cc.c3b(250,0,0))
		end
	end	

	self._btnClose = ExternalFun.seekWigetByName(self._csbNode, "btn_close")
	self._btnClose:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	self._btnWechat = ExternalFun.seekWigetByName(self._csbNode, "btn_wechat")
	self._btnWechat:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--self._btnQQ = ExternalFun.seekWigetByName(self._csbNode, "btn_qq")
	--self._btnQQ:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	-- 添加点击监听事件 遮罩
    local mask = ExternalFun.seekWigetByName(self._csbNode, "mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = ExternalFun.seekWigetByName(self._csbNode, "setbg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SELECT_LINK_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function SelectLinkLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function SelectLinkLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function SelectLinkLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	local senderName = sender:getName()
	if (senderName == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SELECT_LINK_LAYER})
	elseif (senderName == "btn_wechat") then
		local function viewcallback(view, state)
			if (state == "enter") then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ROATEWAIT_LAYER})
				cc.Application:getInstance():openURL("weixin://")
			end
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在前往微信，请稍后..."}, viewcallback = viewcallback, canrepeat = false}, VIEW_LIST.ROATEWAIT_LAYER)	
	elseif (senderName == "btn_qq") then
		local function viewcallback(view, state)
			if (state == "enter") then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ROATEWAIT_LAYER})
				 MultiPlatform:getInstance():openQQ()
			end
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在前往QQ，请稍后..."}, viewcallback = viewcallback, canrepeat = false}, VIEW_LIST.ROATEWAIT_LAYER)		
	end
end

function SelectLinkLayer:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end

return SelectLinkLayer