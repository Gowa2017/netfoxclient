--[[
	等候界面
		2016_04_27 C.P
	功能：阻断用户输入，防止不必要的情况
]]
local PopWait = class("PopWait", function(isTransparent)
	if isTransparent then
		return display.newLayer(cc.c4b(0, 0, 0, 0))
	else
	 	return display.newLayer(cc.c4b(0, 0, 0, 125))
	end    
end)

function PopWait:ctor(isTransparent)
	
	if width == nil or height == nil then
		self:setContentSize(display.width,display.height)
	else
		self:setContentSize(width,height)
	end
	
	local function onTouch(eventType, x, y)
        return true
    end
	self:setTouchEnabled(true)
	self:registerScriptTouchHandler(onTouch)

	if not isTransparent then
		cc.Sprite:create("wait_round.png")
			:addTo(self)
			:move(appdf.WIDTH/2,appdf.HEIGHT/2 )	
			:runAction(cc.RepeatForever:create(cc.RotateBy:create(2 , 360)))
	end	
end

--显示
function PopWait:show(parent,message)
	self:addTo(parent)
	return self
end

--显示状态
function PopWait:isShow()
	return not self._dismiss 
end

--取消显示
function PopWait:dismiss()
	if self._dismiss then
		return
	end
	self._dismiss  = true
	self:runAction(cc.RemoveSelf:create())
end

return PopWait