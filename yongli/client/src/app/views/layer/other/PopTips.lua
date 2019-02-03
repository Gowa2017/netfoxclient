--[[
	等候界面
		2016_04_27 C.P
	功能：阻断用户输入，防止不必要的情况
]]
local PopTips = class("PopTips", cc.BaseLayer)

function PopTips:ctor(msg, second, callback)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "base/PopTips.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	
	self._second = second or 0
	self._callback = callback
	self._msg = msg or ""
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
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
		local pBg = self._csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOW_POP_TIPS})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)	
end

function PopTips:initUI()
	local bg = self._csbNode:getChildByName("bg")
	self._textMsg = bg:getChildByName("tips")
	self._timeMsg = bg:getChildByName("times")
	self._textMsg:setString(self._msg)
	self._timeMsg:setString(string.format("%d", self._second))
	
end

function PopTips:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		--界面回调
		local timeCount = self._second
		local delayAction = cc.DelayTime:create(1)
		local callAction = cc.CallFunc:create(function ()
			timeCount = timeCount - 1
			
			self._timeMsg:setString(string.format("%d", timeCount))
			
			if (timeCount == 0) then
				if (self._callback) then
					self._callback()
				end
			end
		end)
		local seqAction = cc.Sequence:create(delayAction, callAction)
		local timeScheduleAction = cc.Repeat:create(seqAction, self._second)
		
		self:runAction(timeScheduleAction)
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function PopTips:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

return PopTips