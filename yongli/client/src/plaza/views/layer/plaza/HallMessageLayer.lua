--[[
	大厅消息层
]]
local HallMessageLayer = class("HallMessageLayer", cc.BaseLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

HallMessageLayer.CLOSE_SCROLL_ACTION_TAG = 1

function HallMessageLayer:ctor(data)
	self.super.ctor(self)
	
    -- 加载csb资源
	local csbPath = "plaza/HallMessageLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self.m_hallGongGaoMessage = {}
	self._data = data
	
	self._state = false
	
	--设置消息层层级高一点
	self:setLocalZOrder(999)
end

function HallMessageLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		self:showHallMessage(self._data.szSign)
	end)

	self._csbNodeAni:play("openAni", false)
end

function HallMessageLayer:onWillViewExit()
	local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_text")
	text_gonggao:setString("")
	text_gonggao:stopAllActions()
		
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

--内部调用
function HallMessageLayer:addHallMessage(text, callback)
	local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_text")
	local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_mask")
	local mask_contentsize = mask_gonggao:getContentSize()
	local text_content = text
	
	text_gonggao:setString(text_content)
	local time_speed = 100
	local speed = (mask_contentsize.width + text_gonggao:getContentSize().width) / time_speed
	local startPosition = cc.p(mask_contentsize.width, mask_contentsize.height / 2)
	local endPosition = cc.p(-text_gonggao:getContentSize().width, mask_contentsize.height / 2)
	
	text_gonggao:setPosition(startPosition)
	
	text_gonggao:stopAllActions()
	
	self._state = true
	local action = cc.RepeatForever:create(cc.Sequence:create(
	cc.MoveTo:create(speed, endPosition),
	cc.CallFunc:create(function()
		text_gonggao:setPosition(startPosition)
		if (callback) then
			callback()
		end
	end)))
	text_gonggao:runAction(action)
end

--显示大厅消息
function HallMessageLayer:showHallMessage(text)
	local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_text")
	local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_mask")
	local mask_contentsize = mask_gonggao:getContentSize()
	local text_content = text
	
	if (self._state == false) then
		local callBack = nil
		callBack = function()
			local item = table.remove(self.m_hallGongGaoMessage, 1)
			
			if (item == nil) then
				local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "hall_message_text")
				text_gonggao:stopAllActions()
				
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.HALL_MESSAGE_LAYER, viewcallback = function (view, state)
					if (state == "exit") then
						self._state = false
					end
				end})
			else
				self:addHallMessage(item, callBack)
			end
		end
		
		self:addHallMessage(text_content, callBack)
	else
		table.insert(self.m_hallGongGaoMessage, text_content)
	end
end

return HallMessageLayer