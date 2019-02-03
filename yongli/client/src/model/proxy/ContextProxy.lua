local ContextProxy = class("ContextProxy", puremvc.Proxy)

function ContextProxy:ctor()
	self.super.ctor(self)
    self.proxyName = "ContextProxy"
	
	self.data = {}
	self.eventQueue = {}
end

function ContextProxy:pushBackEvent(event)
	table.insert(self.eventQueue, event)
end

function ContextProxy:popFrontEvent(event)
	return table.remove(self.eventQueue, 1)
end

function ContextProxy:removeEvent(id)
    for i = 1, #self.eventQueue do
		if (id == self.eventQueue[i].DATA.id) then
			return table.remove(self.eventQueue, i)
		end
	end	
end

function ContextProxy:frontEvent(event)
	return self.eventQueue[1]
end

function ContextProxy:backEvent(event)
	return self.eventQueue[#self.eventQueue]
end

function ContextProxy:getCurrentRootContext()
    return self.data[#self.data]
end

function ContextProxy:getCurrentContext()
    for i = #self.data, 1, -1 do
		local context = self.data[i]:getCurrentContext()
		if (context) then
			return context
		end
	end
	
	return nil	
end

function ContextProxy:pushContext(context)
	table.insert(self.data, context)
end

--从栈顶往栈底查找，知道查找到第一个返回
function ContextProxy:findContextByName(name)
	--assert(type(name) == "string", "expected string!")
	if (type(name) ~= "string") then
		return nil
	end
	
    for i = #self.data, 1, -1 do
		local context = self.data[i]:findContextByName(name)
		if (context) then
			return context
		end
		
--[[		local childrenContext = context:getChildren()
		for j = #childrenContext, 1, -1 do
			if (childrenContext[j]:getName() == name) then
				return childrenContext[j]
			else
				context = childrenContext[j]:findContextByName(name)
				if (context) then
					return context
				end
			end
		end--]]
	end
	
	return nil
end

--从栈顶往栈底查找，知道查找到第一个返回
function ContextProxy:findRootContextByName(name)
	--assert(type(name) == "string", "expected string!")
	if (type(name) ~= "string") then
		return nil
	end
	
    for i = #self.data, 1, -1 do
		local context = self.data[i]
		if (context:getName() == name) then
			return context
		end
	end
	
	return nil
end

--从栈顶往栈底查找，直到查找到第一个返回
function ContextProxy:getLastRootContext()
    for i = #self.data, 1, -1 do
		local context = self.data[i]
		if (context:getView() == nil and context:getMediator() == nil) then
			return context
		end
	end
	return nil
end

--从栈顶往栈底查找，知道查找到第一个返回
function ContextProxy:removeContextByName(name)
	assert(type(name) == "string", "expected string!")
	if (type(name) ~= "string") then
		return nil
	end
	
    for i = #self.data, 1, -1 do
		local context = self.data[i]
		if (context:getName() == name) then
			return table.remove(self.data, i)
		end
		
		local childrenContext = context:getChildren()
		for j = #childrenContext, 1, -1 do
			local childContext = childrenContext[j]
			if (childContext:getName() == name) then
				local reContext = self:removeContextByName(name)
				if (reContext ~= nil) then
					return reContext
				end
			end
		end
	end
	return nil
end

--从栈顶往栈底查找，知道查找到第一个返回
function ContextProxy:removeContext(context)
	assert(type(context) == "table", "expected string!")
	if (type(context) ~= "table") then
		return nil
	end
	
    for i = #self.data, 1, -1 do
		local curcontext = self.data[i]
		if (curcontext == context) then
			return table.remove(self.data, i)
		end
		
		local childrenContext = context:getChildren()
		for j = #childrenContext, 1, -1 do
			local childContext = childrenContext[j]
			if (childContext == context) then
				return self:removeContext(childContext)
			end
		end
	end
	
	return nil
end


function ContextProxy:removeRootContext(context)
    for i = #self.data, 1, -1 do
		local curcontext = self.data[i]
		if (context == curcontext) then
			return table.remove(self.data, i)
		end
	end	

    return nil
end

-- 1 2 3代表栈底到栈顶 -1 -2 -3代表栈顶到栈底
function ContextProxy:getRootContextByIndex(index)
	assert(math.abs(index) <= #self.data)
	if (0 == #self.data) then
		return nil
	end
	
	local pos = 0
	if (index > 0) then
		pos = index
	else
		pos = (#self.data + 1) + index
	end
	
    return self.data[pos]
end

--获得context的上一层环境
function ContextProxy:getRootLastContext(context)
	assert(type(context) == "table", "context not right")
	if (0 == #self.data) then
		return nil
	end
	
    for i = #self.data, 1, -1 do
		local curcontext = self.data[i]
		if (context == curcontext) then
			if (i > 1) then
				return self.data[i - 1]
			else
				return nil
			end
		end
	end
	
	return nil
end

function ContextProxy:popContext()
    if (0 ~= #self.data) then
        return table.remove(self.data)
    else
        return nil
    end
end

function ContextProxy:cleanContext()
    self.data = {}
end

function ContextProxy:getContextCount()
    return #self.data
end

return ContextProxy