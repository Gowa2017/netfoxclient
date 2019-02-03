local BaseLayer = class("BaseLayer", cc.Layer)
local Event = require("framework.notify.event")
local schduler = cc.Director:getInstance():getScheduler()
function BaseLayer:ctor()
	self.event = Event.new() -- 事件
	-- 事件队列
	self.eventQue = {}
end

function BaseLayer:onEnter() end
function BaseLayer:onExit() end
function BaseLayer:onEnterTransitionFinish() end
function BaseLayer:onExitTransitionStart() end
function BaseLayer:onCleanup() end

-- 子类重载该方法，可以返回由编辑器创建的节点
function BaseLayer:getRootNode()
	return self
end

-- 开始加载
function BaseLayer:loadView()
	--加载资源
	self:onViewLoad()
end

-- 加载完成后必须发送 BaseView.LOAD_VIEW_ASYNC_FINISHED 通知异步加载完成
function BaseLayer:onViewLoad()
	local handler = 0
	handler = schduler:scheduleScriptFunc(function()
		self:loadFinished()
		schduler:unscheduleScriptEntry(handler) 
	end, 0, false)		
end

function BaseLayer:loadFinished()
	self:emit(BaseView.LOAD_VIEW_ASYNC_FINISHED)
end

-- 卸载加载
function BaseLayer:unLoadView()
	--异步卸载资源
	self:onViewUnLoad()
end

-- 卸载完成后必须发送BaseView.UNLOAD_VIEW_ASYNC_FINISHED 消息通知卸载完成
function BaseLayer:onViewUnLoad()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:unLoadFinished()
	end))
	self:runAction(action)	
end

function BaseLayer:unLoadFinished()
	self:emit(BaseView.UNLOAD_VIEW_ASYNC_FINISHED)
end

-- 由子类实现的即将入场操作
function BaseLayer:willViewEnter()
	self:onWillViewEnter()
end

-- 子类可以覆盖该方法，播放一段动画
-- 然后通知完成入场
-- 通知后会执行 mediator 的 onRegister 注册视图对应的Mediator，并且添加上下文环境到视图栈中
function BaseLayer:onWillViewEnter()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:enterViewFinished()
	end))
	self:runAction(action)
end

-- 子类调用
function BaseLayer:enterViewFinished()
	self:emit(BaseView.ENTER_VIEW_ASYNC_FINISHED)
end

-- 加载完成后即将进入场景调用
function BaseLayer:willViewExit()
	self:onWillViewExit()
end

-- 执行一些退场前操作
-- 子类可以覆盖该方法，播放一段动画
-- 通知后会执行 mediator 的 onRemove 反注册视图对应的Mediator，并且从视图栈中移除上下文环境
function BaseLayer:onWillViewExit()
	local action =cc.Sequence:create(cc.DelayTime:create(0),
	cc.CallFunc:create(function()
		self:exitViewFinished()
	end))
	self:runAction(action)
end

function BaseLayer:exitViewFinished()
	self:emit(BaseView.EXIT_VIEW_ASYNC_FINISHED)
end

--移除UI监听事件
function BaseLayer:detach(event)
    -- 移除关联的 UI 事件
    for k, v in ipairs(self.eventQue) do
		if (v.event == event) then
			self.event:disconnect(v.event, v.callback)
			table.remove(self.eventQue, key)
			break
		end
    end
end

-- 监听事件
function BaseLayer:attach(event, callback)
	assert(type(event) == "string", "expected string, but not now")
	assert(type(callback) == "function", "expected function, but not now")

    for k, v in ipairs(self.eventQue) do
		if (v.event == event) then
			assert(false, "aready attack this event")
			return
		end
    end
	
    self.event:connect(event, callback)
	-- 将事件记录到队列，注销 mediator 时称除
    table.insert(self.eventQue, {
        event = event,
        callback = callback
    })
end

-- 监听事件
function BaseLayer:emit(event, ...)
	assert(type(event) == "string", "expected string, but not now")
	self.event:emit(event, ...)
end

return BaseLayer