--[[
	等候界面
		2016_04_27 C.P
	功能：阻断用户输入，防止不必要的情况
]]
local PopGameWait = class("PopGameWait", cc.BaseLayer)

function PopGameWait:ctor(msg)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "EnterGame/EnterGameWaitLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

function PopGameWait:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		self._csbNodeAni:play("runAni", true)
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function PopGameWait:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

return PopGameWait