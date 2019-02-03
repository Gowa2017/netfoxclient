--[[
	我的代理界面
	2016_06_23 Ravioyla
]]
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local AgentRechargeLayer = class("AgentRechargeLayer", cc.BaseLayer)

-- 进入场景而且过渡动画结束时候触发。
function AgentRechargeLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function AgentRechargeLayer:onExitTransitionStart()
    return self
end

function AgentRechargeLayer:ctor(itemInfo)
	self.super.ctor(self)
	
	assert(type(itemInfo) == "table", "error expected table data")
	if (type(itemInfo) ~= "table") then
		return
	end
	
	--保存数据
	self._itemInfo = itemInfo
	
	--发送标识
	self._sendSuccess = false

    -- 加载csb资源
	local csbPath = "Shop/AgentRecharge/AgentRechargeStyle_0.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	
end

function AgentRechargeLayer:initUI()
	--按钮回调事件
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender)
		end
	end
	
	local bg = self._csbNode:getChildByName("bg")
	
	--关闭按钮
	local btnClose = ExternalFun.seekWigetByName(self._csbNode, "btn_close")
	btnClose:addTouchEventListener(btnEvent)
	
	local btnToushu = ExternalFun.seekWigetByName(self._csbNode, "btn_toushu")
	btnToushu:addTouchEventListener(btnEvent)
	
--[[	--qq拷贝按钮
	local btnQQ = bg:getChildByName("btn_qq")
	btnQQ:addTouchEventListener(btnEvent)
	local btnQQText = btnQQ:getChildByName("text")
	btnQQText:setString(string.format("%d",self._itemInfo.qq))
	
	--微信拷贝
	local btnWeChat= bg:getChildByName("btn_wechat")
	btnWeChat:addTouchEventListener(btnEvent)
	local btnWeChatText = btnWeChat:getChildByName("text")
	btnWeChatText:setString(self._itemInfo.wechat)
	
	--订单号拷贝
	local btnOrderCode= bg:getChildByName("btn_order")
	btnOrderCode:addTouchEventListener(btnEvent)
	self._orderid = ExternalFun.guid()
	local btnOrderText = btnOrderCode:getChildByName("text")
	btnOrderText:setString(self._orderid)--]]
	
	local clipLayout = bg:getChildByName("clip")
	
	local layout_agent = clipLayout:getChildByName("layout_agent")
	local listview = layout_agent:getChildByName("ListView_1")
	
	if (self._itemInfo.wechat ~= nil and self._itemInfo.wechat ~= "") then
		local itemLayout = layout_agent:getChildByName("AgentRechargeWXItemLayout"):clone()

		--微信拷贝
		local btnWeChat= itemLayout:getChildByName("btn_wechat")
		btnWeChat:addTouchEventListener(btnEvent)
		local btnWeChatText = btnWeChat:getChildByName("id")
		btnWeChatText:setString(self._itemInfo.wechat)
		
		listview:pushBackCustomItem(itemLayout)
	end	
	
	if (self._itemInfo.qq ~= nil and self._itemInfo.qq ~= "") then
		local itemLayout = layout_agent:getChildByName("AgentRechargeQQItemLayout"):clone()
		
		--QQ拷贝
		local btnQQChat= itemLayout:getChildByName("btn_qq")
		btnQQChat:addTouchEventListener(btnEvent)
		local btnQQChatText = btnQQChat:getChildByName("id")
		btnQQChatText:setString(self._itemInfo.qq)		
		
		listview:pushBackCustomItem(itemLayout)
	end	
	
	local function onTouchBegan( touch, event )
		return self:isVisible()
	end

	local function onTouchEnded( touch, event )
		local location = touch:getLocation()	
		location = bg:convertToNodeSpace(location)
		local rec = cc.rect(0, 0, bg:getContentSize().width, bg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, location) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.AGENT_RECHARGE_LAYER})
		end
	end
	
	local listener = cc.EventListenerTouchOneByOne:create()
	listener:setSwallowTouches(true)
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
end

--按键监听
function AgentRechargeLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	local name = sender:getName()
	
	if (name == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.AGENT_RECHARGE_LAYER})
	elseif (name == "btn_qq") then
		local res, msg = MultiPlatform:getInstance():copyToClipboard(string.format("%d", self._itemInfo.qq))
		if true == res then
			local function callback()
				printf("打开QQ")
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOW_POP_TIPS})
				cc.Application:getInstance():openURL(string.format("mqq://im/chat?chat_type=wpa&uin=%s&version=1&src_type=web", self._itemInfo.qq))
			end
			
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"复制成功，系统正在前往QQ，请稍后！", 3, callback}}, VIEW_LIST.SHOW_POP_TIPS)
		else
			if type(msg) == "string" then
				showToast(self, msg, 1, cc.c3b(250,0,0))
			end
		end
		
		
	elseif (name == "btn_wechat") then
		local res, msg = MultiPlatform:getInstance():copyToClipboard(self._itemInfo.wechat)
		if true == res then
			local function callback()
				printf("打开微信")
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOW_POP_TIPS})
				cc.Application:getInstance():openURL("weixin://")
			end
			
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"复制成功，系统正在前往微信，请稍后！", 3, callback}}, VIEW_LIST.SHOW_POP_TIPS)
			--showToast(self, "复制到剪贴板成功!", 1)
		else
			if type(msg) == "string" then
				showToast(self, msg, 1, cc.c3b(250,0,0))
			end
		end
	elseif (name == "btn_order") then

		local ostime = os.time()
		local orderurl = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
		
		if (not self._sendSuccess) then
			--发送订单号
			appdf.onHttpJsionTable(orderurl ,"GET","action=agentpayorder&userid=" .. GlobalUserItem.dwUserID .. "&orderId=" .. self._orderid .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(jstable,jsdata)
				dump(jstable, "订单号生成", 6)
				local errmsg = "订单号获取异常!"
				if type(jstable) == "table" then
					local code = jstable["code"]
					if tonumber(code) == 0 then
						local jdata = jstable["data"]
						local msg = jstable["msg"]
						errmsg = nil
						if type(msg) == "string" then
							errmsg = msg
						end
						
						self._sendSuccess = true
						
						--执行复制
						local res, msg = MultiPlatform:getInstance():copyToClipboard(self._orderid)
						if true == res then
							showToast(self, "订单号复制到剪贴板成功!", 1)
						else
							if type(msg) == "string" then
								showToast(self, msg, 1, cc.c3b(250,0,0))
							end
						end
					else
						local jdata = jstable["data"]
						local msg = jstable["msg"]
						errmsg = nil
						if type(msg) == "string" then
							errmsg = msg
						end
						
						if type(errmsg) == "string" and "" ~= errmsg then
							showToast(self,errmsg,2,cc.c3b(250,0,0))
						end 
					end
				end
			end)
		else
			--执行复制
			local res, msg = MultiPlatform:getInstance():copyToClipboard(self._orderid)
			if true == res then
				showToast(self, "订单号复制到剪贴板成功!", 1)
			else
				if type(msg) == "string" then
					showToast(self, msg, 1, cc.c3b(250,0,0))
				end
			end
		end
	elseif (name == "btn_toushu") then
		local agentid = ""
		if (self._itemInfo.wechat and self._itemInfo.wechat ~= "") then
			agentid = agentid .. self._itemInfo.wechat
		end
		
		if (self._itemInfo.qq and self._itemInfo.qq ~= "") then
			if (agentid == "") then
				agentid = self._itemInfo.qq
			else
				agentid = agentid .. "/" .. self._itemInfo.qq
			end
		end
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {agentid}}, VIEW_LIST.SHOP_TOUSU_DAILI)
	else
		assert(false, "AgentRechargeLayer catch not support btn event")
	end
end

function AgentRechargeLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function AgentRechargeLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

return AgentRechargeLayer