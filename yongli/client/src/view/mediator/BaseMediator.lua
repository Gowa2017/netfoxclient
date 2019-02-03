local BaseMediator = class('BaseMediator', puremvc.Mediator)

function BaseMediator:ctor(viewComponent)
    BaseMediator.super.ctor(self, nil, viewComponent)
	self._data = nil
end

-- ע�� Mediator
function BaseMediator:onRegister()
    BaseMediator.super.onRegister(self)

    -- ע��ʱҪִ�е�
    self:onMediatorRegister()
end

-- ������ʵ��ע��ʱҪִ�е�
function BaseMediator:onMediatorRegister() end

-- ���������Ĳ�����ͨ�� context.data ��õĲ���
function BaseMediator:setData(data)
    self._data = data
end

function BaseMediator:onRemove()
    BaseMediator.super.onRemove(self)
    -- ע��ʱҪִ�е�
    self:onMediatorRemove()
end

-- ������ʵ��ע��ʱҪִ�е�
function BaseMediator:onMediatorRemove() end

return BaseMediator