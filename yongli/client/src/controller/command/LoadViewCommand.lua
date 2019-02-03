--[[
    LoadViewCommand 只处理 Mediators 和 ViewComponents 的创建和销毁。
    而把 ViewComponents 的切换过程交给 ViewManager 处理，并
    以异步的方式串连起来。切换场景的过程如下：

        首先是取出当前场景 Context，按层序遍历场景树，然后逆序逐个删除，
    由于删除层结点的过程有可能存在动画效果，所以这个过程是异步的。
    这部分的代码由 RemoveLayersCommand 配合 ViewManager 实现，最终留下当前场景的根结点；
    场景的切换可以有不同的方式（详见 ViewManager 部分），以其中一种方式载入下一个场景的根节点；
    对新场景的根结点（context）进行层序遍历，逐一还原每一个层结点，由于加载层结点的过程可能存在动画效果，
    所以这个过程也是异步的。
    这部分的代码由 LoadLayersCommand 配合 ViewManager 实现；
    切换场景过程完成。
--]]
local LoadViewCommand = class("LoadViewCommand", puremvc.SimpleCommand)

function LoadViewCommand:execute(note)
    local msgData = note:getBody()
	local msgType = note:getType()

	--总视图上下文环境栈
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	
--[[	--查找要添加的视图
	local willAddContext = msgData.context
	
	if (willAddContext:getParent() ~= nil) then
		assert(false, "this context already has parent context")
	end	--]]
	
	--suspend、running、dead、normal
	local eventCoroutine = nil
	eventCoroutine = coroutine.create(function()
		local willAddContext = nil
		
		local Context = require("client.src.model.base.Context")
		if (msgData.parm.canrepeat == false) then
			willAddContext = contextProxy:findContextByName(msgType) or Context.new()
		else
			willAddContext = Context.new()
		end

		--判断是否为新创建的 Context 界面
		if (willAddContext:getParent() == nil) then
			willAddContext:setName(msgType)
			willAddContext:setViewClass(msgData.viewClass)
			willAddContext:setMediatorClass(msgData.mediatorClass)			
		end	
		
		--附加数据重新赋值
		if (type(msgData.parm) == "table") then
			willAddContext.data = {}
			for k, v in pairs(msgData.parm) do
				willAddContext.data[k] = v
			end
		end
		
		--引用次数 +1
		local refCount = willAddContext:retain()
		--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
		if (willAddContext.data.canrepeat == false) then
			if (refCount > 1) then
				printf("PUSH_VIEW FAILED,the view is find, attribute is not repeat!", debug.getinfo(1).currentline)
			else
				--上下文环境入栈
				--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
				self:recursiveAddView(willAddContext, function (context)
					if (context == willAddContext) then
						if (willAddContext.data.viewcallback ~= nil) then
							willAddContext.data.viewcallback(context:getView(), "enter")
						end
					end
				end, eventCoroutine)
				--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
				coroutine.yield()
			end
		else
			--上下文环境入栈
			--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
			self:recursiveAddView(willAddContext, function (context)
				if (context == willAddContext) then
					if (willAddContext.data.viewcallback ~= nil) then
						willAddContext.data.viewcallback(context:getView(), "enter")
					end
				end
			end, eventCoroutine)
			
			coroutine.yield()
		end
		--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
		local frontEventCoroutine = contextProxy:frontEvent()
		if (frontEventCoroutine) then
			--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
			if (frontEventCoroutine ~= eventCoroutine) then
				--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
				local result, msg = coroutine.resume(frontEventCoroutine)
				assert(result, msg)
			else
				--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
				contextProxy:popFrontEvent()
				local nxtEventCoroutine = contextProxy:frontEvent()
				if (nxtEventCoroutine) then
					--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
					local result, msg = coroutine.resume(nxtEventCoroutine)
					assert(result, msg)					
				end							
			end
		end
	end)
	
	local frontCoroutine = contextProxy:frontEvent()	
	
	contextProxy:pushBackEvent(eventCoroutine)
	--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
	if (frontCoroutine == nil) then
		--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
		local result, msg = coroutine.resume(eventCoroutine)
		assert(result, msg)				
	end
end

--递归重置上下文
function LoadViewCommand:recursiveResetView(context, callback, parentConroutine)
	if (context) then
		--for i = #childrenContext, 1, -1  do
		--	local childContext = childrenContext[i]
		--	self:recursiveResetView(childContext, callback)
		--end

		--通过协程实现视图一个一个的重置视图栈
		local OneByOneResetView = nil
		OneByOneResetView = coroutine.create(function()
			local childrenContext = context:getChildren()
			for i = #childrenContext, 1, -1  do
				local childContext = childrenContext[i]
				self:recursiveResetView(childContext, callback, OneByOneResetView)
				coroutine.yield()
			end

			local curView = context:getView()
			assert(curView ~= nil, "curView expected not nil")

			local parentContext = context:getParent()			
			
			--从栈底往栈顶加
			curView:attach(BaseView.UNLOAD_VIEW_ASYNC_FINISHED, function (e)
				--视图进入
				if (nil == curView) then
					assert(false, "view expected not nil")								
				else
					--移除视图对应的Mediator
					local curMediator = context.mediator
					if (nil ~= curMediator) then
						self.facade:removeMediator(curMediator.mediatorName)
						context:setMediator(nil)
					end
				end
											
				--等待视图进入完成后绑定上下文环境
				curView:attach(BaseView.EXIT_VIEW_ASYNC_FINISHED, function (e)
					if (parentContext) then
						curView:removeFromParent()
						context:setView(nil)
					else
						--总视图上下文环境栈
						if (string.find(curView:getDescription(), "Scene")) then
							context:setView(nil)
						else
							assert(false, "this context has not parent, view expected scene, but not now")
						end
					end
					
					if (callback) then
						callback(context)
					end
					
					if (parentConroutine) then
						local result, msg = coroutine.resume(parentConroutine)
						assert(result, msg)						
					end
				end)		
				curView:willViewExit()
			end)
			curView:unLoadView()
		end)
		local result, msg = coroutine.resume(OneByOneResetView)
		assert(result, msg)				
	end
end

function LoadViewCommand:recursiveAddView(context, callback, parentConroutine)
	if (context) then
		assert(context:getView() == nil)
		if (context:getView() ~= nil) then
			return
		end
		
		--通过协程实现视图一个一个的添加视图栈
		local OneByOneAddView = nil
		OneByOneAddView = coroutine.create(function()
			local curView = nil
			if (context.data.ctor ~= nil) then
				assert(type(context) == "table")
				curView = context.viewClass:create(unpack(context.data.ctor))
			else
				curView = context.viewClass:create()
			end
			--下面有些异步操作 必须retain()
			curView:retain()
			
			local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
			
			local parentContext = contextProxy:findContextByName(context.data.parentName)
			
			local currentRootContext = contextProxy:getCurrentRootContext()
		
			--上下文环境入栈
			if (nil == parentContext) then --为空 一定是场景节点
				if  nil ~= currentRootContext then --说明非第一次添加场景
					if (string.find(curView:getDescription(), "Scene") ~= nil) then
						self:recursiveResetView(currentRootContext, nil, OneByOneAddView)
						coroutine.yield()
					end
				end	
			end
			
			--从栈底往栈顶加
			curView:attach(BaseView.LOAD_VIEW_ASYNC_FINISHED, function (e)
				curView:registerScriptHandler(function(eventType)
					if eventType == "enter" then
						curView:onEnter()
					elseif eventType == "exit" then
						curView:onExit()
					elseif eventType == "enterTransitionFinish" then
						curView:onEnterTransitionFinish()
					elseif eventType == "exitTransitionStart" then
						curView:onExitTransitionStart()
					elseif eventType == "cleanup" then
						curView:onCleanup()
						--移除视图对应的Mediator
						curView:unregisterScriptHandler()
					end
				end)
				--视图进入
				if (parentContext == nil) then
					if (string.find(curView:getDescription(), "Scene") ~= nil) then
						--主上下文栈
						local findContext = contextProxy:findRootContextByName(context:getName())
						if (not findContext) then
							contextProxy:pushContext(context)
						else
							assert(false, "aready has same context name, please check")
						end	
						
						if (context.data.transition == nil) then
							cc.Director:getInstance():replaceScene(curView)
						else
							local data = context.data
							local newView = display.wrapScene(curView, data.transition or "FADE", data.time or 1, data.more)
							cc.Director:getInstance():replaceScene(newView)
						end				
					else
						currentRootContext:pushContext(context)
						
						local curRootView = currentRootContext:getView()
						assert(curRootView ~= nil)
						curRootView:addChild(curView)
					end		
				else
					parentContext:pushContext(context)
					
					local parentView = parentContext:getView()
					if (parentView == nil) then
						assert(false, "not parentView!")
					else
						parentView:addChild(curView)
					end
				end
				
					
				curView:release()
				--绑定视图信息
				context:setView(curView)	
												
				--等待视图进入完成后绑定上下文环境
				curView:attach(BaseView.ENTER_VIEW_ASYNC_FINISHED, function (e)
					if (context:getMediator() ~= nil) then
						--已经绑定过中间件 直接返回
						printf("可能存在错误，该视图 尝试重复发送加载完成行为！")
						return
					end
						
						--创建视图对应的Mediator
					if (context.mediatorClass) then
						local targetMediator = context.mediatorClass.new(curView)
						targetMediator:setData(context.data)
						self.facade:registerMediator(targetMediator)
						context:setMediator(targetMediator)
					end
					
					local childrenContext = context:getChildren()
					for i = 1, #childrenContext do
						--递归加载
						local childContext = childrenContext[i]
						self:recursiveAddView(childContext, callback)
					end

					if (callback) then
						callback(context)
					end

					if (parentConroutine) then
						local result, msg = coroutine.resume(parentConroutine)
						assert(result, msg)								
					end
				end)		
				curView:willViewEnter()
					
			end)
			curView:loadView()
		end)
		local result, msg = coroutine.resume(OneByOneAddView)
		assert(result, msg)				
	end
end

function LoadViewCommand:recursiveRestoreView(context, callback)
	if (context) then
		assert(context:getView() == nil)
		if (context:getView() ~= nil) then
			return
		end
		
		local curView = nil
		if (context.data.ctor ~= nil) then
			assert(type(context) == "table")
			curView = context.viewClass:create(unpack(context.data.ctor))
		else
			curView = context.viewClass:create()
		end
		local parentContext = context:getParent()
		local parentView = context:getView()
		
		--下面有些异步操作 必须retain()
		curView:retain()
		
		--从栈底往栈顶恢复
		curView:attach(BaseView.LOAD_VIEW_ASYNC_FINISHED, function (e)
			--视图进入
			
			curView:registerScriptHandler(function(eventType)
				if eventType == "enter" then
					curView:onEnter()
				elseif eventType == "exit" then
					curView:onExit()
				elseif eventType == "enterTransitionFinish" then
					curView:onEnterTransitionFinish()
				elseif eventType == "exitTransitionStart" then
					curView:onExitTransitionStart()
				elseif eventType == "cleanup" then
					curView:onCleanup()
					curView:unregisterScriptHandler()
				end
			end)
			--视图进入
			if (parentView == nil) then
				if (string.find(curView:getDescription(), "Scene") ~= nil) then
					if (context.data.transition == nil) then
						cc.Director:getInstance():replaceScene(curView)
					else
						local data = context.data
						local newView = display.wrapScene(curView, data.transition or "FADE", data.time or 1, data.more)
						cc.Director:getInstance():replaceScene(newView)
					end		
				else
					assert(false, "not restore view expected scene, but the view is not scene, this context parent view could be release")
				end										
			else
				parentView:addChild(curView)
			end
			
												
			curView:attach(BaseView.ENTER_VIEW_ASYNC_FINISHED, function (e)

				--创建视图对应的Mediator
				assert(context.mediator == nil)
				if (context.mediator ~= nil) then
					return
				end
				
				if (context.mediatorClass) then
					local targetMediator = context.mediatorClass.new(curView)
					targetMediator:setData(context.data)
					self.facade:registerMediator(targetMediator)
					context:setMediator(targetMediator)
				end
				curView:release()
				--绑定视图信息
				context:setView(curView)	
				
				--递归恢复视图
				local childrenContext = context:getChildren()
				for i = #childrenContext, 1, -1 do
					local childContext = childrenContext[i]
					self:recursiveRestoreView(childContext, callback)
				end
				if (callback) then
					callback(context)
				end
				
			end)		
			curView:willViewEnter()
		end)
		curView:loadView()
		
	end
end

return LoadViewCommand