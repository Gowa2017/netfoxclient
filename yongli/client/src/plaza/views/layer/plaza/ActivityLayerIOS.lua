--[[
	活动界面
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local ActivityLayerIOS = class("ActivityLayerIOS", cc.BaseLayer)

-- 进入场景而且过渡动画结束时候触发。
function ActivityLayerIOS:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ActivityLayerIOS:onExitTransitionStart()
    return self
end

function ActivityLayerIOS:ctor()
	self.super.ctor(self)

	--加载csb资源
	local csbPath = "activity/ActivityLayerIOS.csb"
	local rootLayer, csbNode = ExternalFun.loadRootCSB(csbPath, self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	
end

--初始化主界面
function ActivityLayerIOS:initUI()
	local activityBg = self._csbNode:getChildByName("activity_bg_1")
	--local btnClose = activityBg:getChildByName("btnClose")
	--btnClose:addClickEventListener(handler(self, self.onButtonClickedEvent))	
	
	local top_bg = activityBg:getChildByName("info_top_bg")
	--返回按钮
	self._return = top_bg:getChildByName("Button_return")
    self._return:addClickEventListener(handler(self, self.onButtonClickedEvent))
end

function ActivityLayerIOS:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function ActivityLayerIOS:onWillViewExit()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function ActivityLayerIOS:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	if ("Button_return" == sender:getName()) then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ACTIVITY_IOS_LAYER})
	end
end

function ActivityLayerIOS:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function ActivityLayerIOS:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return ActivityLayerIOS