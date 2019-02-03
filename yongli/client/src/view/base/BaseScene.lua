local BaseScene = class("BaseScene", cc.Scene)
local Event = require("framework.notify.event")
local schduler = cc.Director:getInstance():getScheduler()
function BaseScene:ctor()
	self.event = Event.new() -- �¼�
	-- �¼�����
	self.eventQue = {}
end

function BaseScene:onEnter() end
function BaseScene:onExit() end
function BaseScene:onEnterTransitionFinish() end
function BaseScene:onExitTransitionStart() end
function BaseScene:onCleanup() end

-- �������ظ÷��������Է����ɱ༭�������Ľڵ�
function BaseScene:getRootNode()
	return self
end

-- ��ʼ����
function BaseScene:loadView()
	--������Դ
	self:onViewLoad()
end

-- ������ɺ���뷢�� BaseView.LOAD_VIEW_ASYNC_FINISHED ֪ͨ�첽�������
function BaseScene:onViewLoad()
	local handler = 0
	handler = schduler:scheduleScriptFunc(function()
		self:loadFinished()
		schduler:unscheduleScriptEntry(handler) 
	end, 0, false)	
end

function BaseScene:loadFinished()
	self:emit(BaseView.LOAD_VIEW_ASYNC_FINISHED)
end

-- ж�ؼ���
function BaseScene:unLoadView()
	--�첽ж����Դ
	self:onViewUnLoad()
end

-- ж����ɺ���뷢��BaseView.UNLOAD_VIEW_ASYNC_FINISHED ��Ϣ֪ͨж�����
function BaseScene:onViewUnLoad()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:unLoadFinished()
	end))
	self:runAction(action)	
end

function BaseScene:unLoadFinished()
	self:emit(BaseView.UNLOAD_VIEW_ASYNC_FINISHED)
end

-- ������ʵ�ֵļ����볡����
function BaseScene:willViewEnter()
	self:onWillViewEnter()
end

-- ������Ը��Ǹ÷���������һ�ζ���
-- Ȼ��֪ͨ����볡
-- ֪ͨ���ִ�� mediator �� onRegister ע����ͼ��Ӧ��Mediator��������������Ļ�������ͼջ��
function BaseScene:onWillViewEnter()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:enterViewFinished()
	end))
	self:runAction(action)
end

-- �������
function BaseScene:enterViewFinished()
	self:emit(BaseView.ENTER_VIEW_ASYNC_FINISHED)
end

-- ������ɺ󼴽����볡������
function BaseScene:willViewExit()
	self:onWillViewExit()
end

-- ִ��һЩ�˳�ǰ����
-- ������Ը��Ǹ÷���������һ�ζ���
-- ֪ͨ���ִ�� mediator �� onRemove ��ע����ͼ��Ӧ��Mediator�����Ҵ���ͼջ���Ƴ������Ļ���
function BaseScene:onWillViewExit()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:exitViewFinished()
	end))
	self:runAction(action)
end

function BaseScene:exitViewFinished()
	self:emit(BaseView.EXIT_VIEW_ASYNC_FINISHED)
end

--�Ƴ�UI�����¼�
function BaseScene:detach(event)
    -- �Ƴ������� UI �¼�
    for k, v in ipairs(self.eventQue) do
		if (v.event == event) then
			self.event:disconnect(v.event, v.callback)
			table.remove(self.eventQue, key)
			break
		end
    end
end

-- �����¼�
function BaseScene:attach(event, callback)
	assert(type(event) == "string", "expected string, but not now")
	assert(type(callback) == "function", "expected function, but not now")

    for k, v in ipairs(self.eventQue) do
		if (v.event == event) then
			assert(false, "aready attack this event")
			return
		end
    end
	
    self.event:connect(event, callback)
	-- ���¼���¼�����У�ע�� mediator ʱ�Ƴ�
    table.insert(self.eventQue, {
        event = event,
        callback = callback
    })
end

-- �����¼�
function BaseScene:emit(event, ...)
	assert(type(event) == "string", "expected string, but not now")
	self.event:emit(event, ...)
end

return BaseScene