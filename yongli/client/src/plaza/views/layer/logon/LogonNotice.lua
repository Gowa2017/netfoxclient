--[[
	活动界面
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local LogonNotice = class("LogonNotice", ccui.Layout)

LogonNotice.url = BaseConfig.WEB_HTTP_URL .. "/Mobile/Notice.aspx"

LogonNotice.DIR_UP = "DIR_UP"
LogonNotice.DIR_DOWN = "DIR_DOWN"
-- 进入场景而且过渡动画结束时候触发。
function LogonNotice:onEnterTransitionFinish()
end

-- 退出场景而且开始过渡动画时候触发。
function LogonNotice:onExitTransitionStart()
end

function LogonNotice:onExit()
end

function LogonNotice:ctor(contentSize)
	if ("table" ~= type(contentSize)) then
		contentSize = cc.size(200, 200)
	end
	
	self:setContentSize(contentSize)
	
	ExternalFun.registerNodeEvent(self)
	
	self.ItemArray = {}
	self.ItemSizeArray = {}
	self._speed = 40
	self._curSpeedCursor = 0
	self._itemMarginY = 10
	self._dir = LogonNotice.DIR_UP
	self.scrollLayout = ccui.Layout:create()
	self.scrollLayout:setContentSize(contentSize)
	--self.scrollLayout:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
	--self.scrollLayout:setBackGroundColor(cc.c3b(255, 255, 0))
	self.scrollLayout:setOpacity(128)
	self.scrollLayout:setPosition(cc.p(0, self:getContentSize().height))
	self:addChild(self.scrollLayout)
	
	self:setClippingEnabled(true)
	
	--self:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
	--self:setBackGroundColor(cc.c3b(255, 0, 128))
	--self:initScrollLayer(scrollLayout)
	
	--开启定时器
	--self:scheduleUpdateWithPriorityLua(handler(self, self.update), 0)

	self:setScrollSpeed(self._speed)
	--scrollLayout:setPosition(self.scrollEndPosition)
	
	--添加关闭消息监听
	--local listener = cc.EventListenerCustom:create(yl.RY_LOGON_CLOSE,handler(self, self.onLogonClose))
    --cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
end

function LogonNotice:setItemMarginY(var)
	self._itemMarginY = var
end

function LogonNotice:setScrollSpeed(speed)
	if (self.scrollLayout) then
		self.scrollLayout:stopAllActions()
		
		local startPosition = cc.p(0, self:getContentSize().height)
		local endPosition = cc.p(0, -self.scrollLayout:getContentSize().height)
		local action = cc.RepeatForever:create(cc.Sequence:create(cc.MoveTo:create(speed, endPosition),
		cc.CallFunc:create(function()
			self.scrollLayout:setPosition(startPosition)
		end)))
		
		self.scrollLayout:runAction(action)
	end
end

function LogonNotice:setScrollDir(dir)
	local endPosition = nil
	local startPosition = nil
	if (dir == LogonNotice.DIR_DOWN) then
		startPosition = cc.p(0, self:getContentSize().height)
		endPosition = cc.p(0, -self.scrollLayout:getContentSize().height)
	elseif (dir == LogonNotice.DIR_UP) then
		startPosition = cc.p(0, -self.scrollLayout:getContentSize().height)
		endPosition = cc.p(0, self:getContentSize().height)
	else
		assert(false, "un support dir")
	end
	
	self._dir = dir
	
	self.scrollLayout:setPosition(startPosition)
	
	if (self.scrollLayout) then
		self.scrollLayout:stopAllActions()
		local action = cc.RepeatForever:create(cc.Sequence:create(
		cc.MoveTo:create(self._speed, endPosition),
		cc.CallFunc:create(function()

			
			self.scrollLayout:setPosition(startPosition)
		end)))
		self.scrollLayout:runAction(action)
	end
end

function LogonNotice:updateView()
	self.scrollLayout:removeAllChildren()
	
	self.ItemSizeArray = {}
	local contentItemLayoutArray = {}
	
	local scrollLayoutSize = self.scrollLayout:getContentSize()
	--创建好所有的ItemUI项
	for _,v in ipairs(self.ItemArray) do
		local contentItemLayout = ccui.Layout:create()
		contentItemLayout:setAnchorPoint(cc.p(0.5, 1))
		local titleText = cc.Label:createWithTTF(v.title, "base/fonts/round_body.ttf", v.titlefontsize)
		titleText:setLineBreakWithoutSpace(true)
		titleText:setColor(v.titlecolor)
		--titleText:setAlignment(cc.TEXT_ALIGNMENT_LEFT)
		titleText:setDimensions(scrollLayoutSize.width, 0)
		local contentText = cc.Label:createWithTTF(v.content, "base/fonts/round_body.ttf", v.contentfontsize)
		contentText:setLineBreakWithoutSpace(true)
		contentText:setDimensions(scrollLayoutSize.width, 0)
		contentText:setColor(v.contentcolor)
		--计算公告条目 宽高
		
		local titleTextSize = titleText:getContentSize()
		local contentTextSize = contentText:getContentSize()
		local contentItemLayoutSize = cc.size(scrollLayoutSize.width, titleTextSize.height + contentTextSize.height)
		table.insert(self.ItemSizeArray, contentItemLayoutSize)
		contentItemLayout:setContentSize(contentItemLayoutSize)
		--contentItemLayout:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
		--contentItemLayout:setBackGroundColor(cc.c3b(255, 0, 255))
		contentItemLayout:setOpacity(128)
		titleText:setPosition(contentItemLayoutSize.width / 2, contentItemLayoutSize.height - titleTextSize.height / 2)
		contentItemLayout:addChild(titleText)
		contentText:setPosition(contentItemLayoutSize.width / 2, contentTextSize.height / 2)
		contentItemLayout:addChild(contentText)
		table.insert(contentItemLayoutArray, contentItemLayout)
	end
	
	--设置滚动层的宽高
	local scrollContentHeight = 0
	for _,v in ipairs(self.ItemSizeArray) do
		scrollContentHeight = scrollContentHeight + v.height
	end
	
	scrollContentHeight = scrollContentHeight + (#self.ItemSizeArray - 1) * self._itemMarginY
	
	self.scrollLayout:setContentSize(cc.size(scrollLayoutSize.width, scrollContentHeight))

	
	--逐个添加到视图
	local index = 0
	for k,v in ipairs(self.ItemSizeArray) do
		local item = table.remove(contentItemLayoutArray, 1)
		item:setPosition(scrollLayoutSize.width / 2, scrollContentHeight)
		self.scrollLayout:addChild(item)
		scrollContentHeight = scrollContentHeight - (v.height + self._itemMarginY)
		index = index + 1
	end
end

function LogonNotice:addItem(title, titlefontsize, titlecolor, content, contentfontsize, contentcolor, sort, updateview)
	assert(type(title) == "string", "expected string but not now")
	assert(type(content) == "string", "expected string but not now")
	if (nil ~= sort) then
		assert(type(sort) == "number", "expected string but not now")
	end
	
	if (sort) then
		table.insert(self.ItemArray, sort, {title = title, content = content, titlecolor = titlecolor, titlefontsize = titlefontsize, contentfontsize = contentfontsize, contentcolor = contentcolor})
	else
		table.insert(self.ItemArray, {title = title, content = content, titlecolor = titlecolor, titlefontsize = titlefontsize, contentfontsize = contentfontsize, contentcolor = contentcolor})
	end
	if (updateview) then
		
		self:updateView()
		
		local startPosition = nil
		local endPosition = nil
		if (self._dir == LogonNotice.DIR_DOWN) then
			startPosition = cc.p(0, self:getContentSize().height)
			endPosition = cc.p(0, -self.scrollLayout:getContentSize().height)
		elseif (self._dir == LogonNotice.DIR_UP) then
			startPosition = cc.p(0, -self.scrollLayout:getContentSize().height)
			endPosition = cc.p(0, self:getContentSize().height)
		else
			assert(false, "un support dir")
		end
		
		self.scrollLayout:setPosition(startPosition)
		
		if (self.scrollLayout) then
			self.scrollLayout:stopAllActions()
			local action = cc.RepeatForever:create(cc.Sequence:create(
			cc.MoveTo:create(self._speed, endPosition),
			cc.CallFunc:create(function()
				self.scrollLayout:setPosition(startPosition)
			end)))
			self.scrollLayout:runAction(action)
		end
	end
end

return LogonNotice