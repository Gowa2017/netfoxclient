--[[
	游客是否绑定手机提示界面
	2017_10_30 MXM
]]

local VisitorBindLayer = class("VisitorBindLayer", cc.BaseLayer)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local AccountBindLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.AccountBindLayer")

function VisitorBindLayer:ctor()
	self.super.ctor(self)


	-- 加载csb资源
    -- 加载csb资源
	local csbPath = "VisitorBind/VisitorBindLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

--初始化主界面
function VisitorBindLayer:initUI()
    local bg = self._csbNode:getChildByName("bg")
	
	--关闭按钮
	self.btnclose = bg:getChildByName("btn_close")
	self.btnclose:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--去绑定按钮
	self.btnvisitorbind = bg:getChildByName("btn_update")
	self.btnvisitorbind:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--按钮事件

	-- 添加点击监听事件 遮罩
    local mask = self._csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = bg
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.VISITOR_BIND_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)	
end

function VisitorBindLayer:onWillViewEnter()
	self:initUI()
	
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function VisitorBindLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:play("closeAni", false)
end

function VisitorBindLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.VISITOR_BIND_LAYER})
	elseif senderName == "btn_update" then
		local function callback(view, act)
			if (act == "exit") then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {2}}, VIEW_LIST.PERSON_LAYER)
			end
		end
		
		--先移除 绑定层 成功后 通过回调 添加 个人绑定手机号层
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.VISITOR_BIND_LAYER, viewcallback = callback})
	end
	
end

return VisitorBindLayer