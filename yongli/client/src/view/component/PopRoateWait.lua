local PopRoateWait = class("PopRoateWait", cc.BaseLayer)

function PopRoateWait:ctor(msg)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "base/RoateWaitLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	self._msg = msg
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

function PopRoateWait:initUI()
	local roate_item_bg_1 = self._csbNode:getChildByName("roate_item_bg_1")
	local text_tips = roate_item_bg_1:getChildByName("tips")
	if (self._msg and self._msg ~= "") then
		text_tips:setString(self._msg)
	end
end

function PopRoateWait:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		self._csbNodeAni:play("runAni", true)
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function PopRoateWait:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

return PopRoateWait