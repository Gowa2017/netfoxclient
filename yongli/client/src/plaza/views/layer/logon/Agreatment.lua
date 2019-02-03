--[[
	密码修改界面
	2017_09_05 MXM
]]
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local Agreatment = class("Agreatment", cc.BaseLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function Agreatment:ctor()
	self.super.ctor(self)
	
    -- 加载csb资源
	local csbPath = "Register/Agreatment.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)

	local imgBg = self._csbNode:getChildByName("bg")
	
	--标题
	self.sp_title = imgBg:getChildByName("title")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_ok")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --取消按钮
    btn = imgBg:getChildByName("btn_cancel")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))	

	
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
		local pBg = csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.REGISTER_AGREATMENT})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
	
end

function Agreatment:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openNormalAni", function ()
		self:enterViewFinished()
	end)
	
	
	self._csbNodeAni:play("openNormalAni", false)
end

function Agreatment:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeNormalAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeNormalAni", false)
end

function Agreatment:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	if senderName == "btn_close" or senderName == "btn_cancel" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.REGISTER_AGREATMENT})
	elseif senderName == "btn_ok" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.REGISTER_AGREATMENT})
	end
end

function Agreatment:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end

return Agreatment