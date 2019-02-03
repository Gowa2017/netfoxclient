--[[
    Context 描述了一个场景或者层使用哪个 Mediator 和 ViewComponent 创建节点，
    包含哪些关键状态（data），
    并实现了简单的嵌套关系
--]]
local Context = class("Context")

function Context:ctor(data)
    if (nil ~= data and type(data) == "table") then
        self.mediatorClass = data.mediatorClass
        self.viewClass = data.viewClass
    else
        self.mediatorClass = nil
        self.viewClass = nil
    end
	self.mediator = nil
	self.view = nil	
    self.data = {}
	self.name = ""
    self.parent = nil
    self.children = {}
	self.refcount = 0
end

function Context:retain()
	self.refcount = self.refcount + 1
	return self.refcount
end

function Context:release()
	self.refcount = self.refcount - 1
	return self.refcount
end

function Context:setMediatorClass(var)
	self.mediatorClass = var
end

function Context:getMediatorClass()
	return self.mediatorClass
end

function Context:setViewClass(var)
	self.viewClass = var
end

function Context:getViewClass()
	return self.viewClass
end

function Context:setMediator(var)
	self.mediator = var
end

function Context:getMediator()
	return self.mediator
end

function Context:setView(var)
	self.view = var
end

function Context:getView()
	return self.view
end

function Context:setName(var)
	self.name = var
end

function Context:getName()
	return self.name
end

function Context:getParent()
    return self.parent
end

function Context:setParent(context)
    self.parent = context
end

function Context:getCurrentContext()
    --return self.children[#self.children]
    for i = #self.children, 1, -1 do
		local context = self.children[i]:getCurrentContext()
		if (context) then
			return context
		end
	end

	return self
end

function Context:pushContext(context)
    assert(context.parent == nil, "context already has parent")
    context:setParent(self)
	
    table.insert(self.children, context)
end

function Context:popContext()
    local context = table.remove(self.children)
	
    context:setParent(nil)
	
    return context
end

function Context:removeContext(context)
    for i, v in ipairs(self.children) do
        if v == context then
           context:setParent(nil)
            return table.remove(self.children, i)
        end
		
		local childrenContext = v:getChildren()
		for j = #childrenContext, 1, -1 do
			return childrenContext[j]:removeContext(context)
		end		
    end

    return nil
end

function Context:removeContextByName(name)
    for i, v in ipairs(self.children) do
        if v:getName() == name then
           context:setParent(nil)
            return table.remove(self.children, i)
        end
		
		local childrenContext = v:getChildren()
		for j = #childrenContext, 1, -1 do
			return childrenContext[j]:removeContextByName(name)
		end		
    end

    return nil
end

--从栈顶往栈底查找，知道查找到第一个返回
function Context:findContextByName(name)
	--assert(type(name) == "string", "expected string!")
	if (type(name) ~= "string") then
		return nil
	end
	
    for i = #self.children, 1, -1 do
		local context = self.children[i]:findContextByName(name)
		if (context) then
			return context
		end
	end

	if self:getName() == name then
	   return self
	end

	return nil
end

function Context:getChildren()
    return self.children
end

return Context