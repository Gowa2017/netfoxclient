local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GongGaoLayerIOS = class("GongGaoLayerIOS", cc.BaseLayer)
function GongGaoLayerIOS:ctor()
	self.super.ctor(self)
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("GongGao/GongGaoIOS.csb", self)
	self._csbNodeAni = ExternalFun.loadTimeLine( "GongGao/GongGaoIOS.csb" )
	csbNode:runAction(self._csbNodeAni)
	self._csbNode = csbNode
end

function GongGaoLayerIOS:initUI()
	--按钮事件
	local btn_return = self._csbNode:getChildByName("gonggao_bg"):getChildByName("btn_close")
	btn_return:addClickEventListener(handler(self,self.onButtonClickedEvent))
	
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
		local pBg = self._csbNode:getChildByName("gonggao_bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GONGGAO_IOS_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function GongGaoLayerIOS:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function GongGaoLayerIOS:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end


function GongGaoLayerIOS:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function GongGaoLayerIOS:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function GongGaoLayerIOS:onButtonClickedEvent(sender)
	if (sender:getName() == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GONGGAO_IOS_LAYER})
		--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GONGGAO_LAYER)
	end
end

return GongGaoLayerIOS