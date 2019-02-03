local BaseMediator = class('BaseMediator', puremvc.Mediator)

function BaseMediator:ctor(viewComponent)
    BaseMediator.super.ctor(self, nil, viewComponent)
	self._data = nil
end

-- 注册 Mediator
function BaseMediator:onRegister()
    BaseMediator.super.onRegister(self)

    -- 注册时要执行的
    self:onMediatorRegister()
end

-- 由子类实现注册时要执行的
function BaseMediator:onMediatorRegister() end

-- 设置上下文参数，通过 context.data 获得的参数
function BaseMediator:setData(data)
    self._data = data
end

function BaseMediator:onRemove()
    BaseMediator.super.onRemove(self)
    -- 注销时要执行的
    self:onMediatorRemove()
end

-- 由子类实现注销时要执行的
function BaseMediator:onMediatorRemove() end

return BaseMediator