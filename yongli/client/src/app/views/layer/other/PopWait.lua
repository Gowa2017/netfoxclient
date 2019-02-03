--[[
	等候界面
		2016_04_27 C.P
	功能：阻断用户输入，防止不必要的情况
]]
local PopWait = class("PopWait", cc.BaseLayer)

function PopWait:ctor(msg)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "base/PopWait.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	
	local textTips = self._csbNode:getChildByName("tips")
	if (msg ~= nil) then
		assert(type(msg) == "string")
		textTips:setString(msg)
	else
		textTips:setString("")
	end
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

function PopWait:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		self._csbNodeAni:play("roation", true)
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function PopWait:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

return PopWait