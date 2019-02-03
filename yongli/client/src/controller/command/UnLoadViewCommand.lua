--[[
	UnLoadViewCommand 有两个参数，一个是要进行移除的 context 对象。
	当 context 是一个场景结点时（context.parent == nil），
	只对其子结点进行逆层序移除；否则连同 context 对应的层结点一起移除。
	另一个参数是一个回调函数，在清除完成后被调用。
--]]

local UnLoadViewCommand = class('UnLoadViewCommand', puremvc.SimpleCommand)

function UnLoadViewCommand:execute(note)
    local msgData = note:getBody()
	--总视图上下文环境栈
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	
	--suspend、running、dead、normal
	local eventCoroutine = nil
	eventCoroutine = coroutine.create(function()
		local willRemoveName = msgData.Name
		local willRemoveContext = nil
		if (willRemoveName ~= nil) then
			willRemoveContext = contextProxy:findContextByName(willRemoveName)
			
			if (willRemoveContext == nil) then
				--printf("POP_VIEW FAILED,the view can't find %d", debug.getinfo(1).currentline)
				local frontEventCoroutine = contextProxy:frontEvent()
				if (frontEventCoroutine) then
					if (frontEventCoroutine ~= eventCoroutine) then
						local result, msg = coroutine.resume(frontEventCoroutine)
						assert(result, msg)	
					else
						contextProxy:popFrontEvent()
						local nxtEventCoroutine = contextProxy:popFrontEvent()
						if (nxtEventCoroutine) then
							local result, msg = coroutine.resume(nxtEventCoroutine)
							assert(result, msg)							
						end					
					end
				end
				return
			end
			
--[[			if (willRemoveContext:islock()) then
				printf("POP_VIEW FAILED,the view is unloading now, please wait async exit finished :%d", debug.getinfo(1).currentline)
				local frontEventCoroutine = contextProxy:frontEvent()
				if (frontEventCoroutine) then
					if (frontEventCoroutine ~= eventCoroutine) then
						local result, msg = coroutine.resume(frontEventCoroutine)
						assert(result, msg)								
					else
						contextProxy:popFrontEvent()
						local nxtEventCoroutine = contextProxy:popFrontEvent()
						if (nxtEventCoroutine) then
							local result, msg = coroutine.resume(nxtEventCoroutine)
							assert(result, msg)								
						end					
					end
				end
				return
			end	--]]	
		else
			willRemoveContext = contextProxy:getCurrentContext()
	
			if (willRemoveContext == nil) then
				--printf("POP_VIEW FAILED,the view can't find %d", debug.getinfo(1).currentline)
				local frontEventCoroutine = contextProxy:frontEvent()
				if (frontEventCoroutine) then
					if (frontEventCoroutine ~= eventCoroutine) then
						local result, msg = coroutine.resume(frontEventCoroutine)
						assert(result, msg)								
					else
						contextProxy:popFrontEvent()
						local nxtEventCoroutine = contextProxy:popFrontEvent()
						if (nxtEventCoroutine) then
							local result, msg = coroutine.resume(nxtEventCoroutine)
							assert(result, msg)									
						end					
					end
				end
				return
			end
			
--[[			if (willRemoveContext:islock()) then
				printf("POP_VIEW FAILED,the view is unloading now, please wait async exit finished :%d", debug.getinfo(1).currentline)
				local frontEventCoroutine = contextProxy:frontEvent()
				if (frontEventCoroutine) then
					if (frontEventCoroutine ~= eventCoroutine) then
						local result, msg = coroutine.resume(frontEventCoroutine)
						assert(result, msg)	
					else
						contextProxy:popFrontEvent()
						local nxtEventCoroutine = contextProxy:popFrontEvent()
						if (nxtEventCoroutine) then
							local result, msg = coroutine.resume(nxtEventCoroutine)
							assert(result, msg)	
						end					
					end
				end
				return
			end--]]
		end
		
		-- load context nData
		if (type(msgData) == "table") then
			for k, v in pairs(msgData) do
				willRemoveContext.data[k] = v
			end
		end		
		
		local parentContext = contextProxy:findContextByName(willRemoveContext.data.parentName)
		local currentRootContext = contextProxy:getCurrentRootContext()
		--上下文环境入栈
		if (nil == parentContext) then --为空 一定是场景节点
			if  nil == currentRootContext then --说明没有场景
				assert(false, "context stack is empty, please check!")
			else
				local refCount = willRemoveContext:release()
				if (refCount > 1) then
					printf("POP_VIEW FAILED,the view is find, attribute is not repeat!", debug.getinfo(1).currentline)
				else
					self:recursiveRemoveView(willRemoveContext, function (context)
						if (context == willRemoveContext) then
							if (willRemoveContext.data.viewcallback ~= nil) then
								willRemoveContext.data.viewcallback(context:getView(), "exit")
							end
						end
					end, eventCoroutine)
					coroutine.yield()
				end
			end	
		else
			local refCount = willRemoveContext:release()
			if (refCount > 1) then
				printf("POP_VIEW FAILED,the view is find, attribute is not repeat!", debug.getinfo(1).currentline)
			else
				self:recursiveRemoveView(willRemoveContext, function (context)
					if (context == willRemoveContext) then
						if (willRemoveContext.data.viewcallback ~= nil) then
							willRemoveContext.data.viewcallback(context:getView(), "exit")
						end
					end
				end, eventCoroutine)
				coroutine.yield()
			end
		end
	
		local frontEventCoroutine = contextProxy:frontEvent()
		if (frontEventCoroutine) then
			if (frontEventCoroutine ~= eventCoroutine) then
				local result, msg = coroutine.resume(frontEventCoroutine)
				assert(result, msg)					
			else
				contextProxy:popFrontEvent()
				local nxtEventCoroutine = contextProxy:frontEvent()
				if (nxtEventCoroutine) then
					local result, msg = coroutine.resume(nxtEventCoroutine)
					assert(result, msg)						
				end					
			end
		end
	end)
	
	local frontCoroutine = contextProxy:frontEvent()	
	contextProxy:pushBackEvent(eventCoroutine)
	if (frontCoroutine == nil) then
		printf("POP_VIEW %d", debug.getinfo(1).currentline)
		local result, msg = coroutine.resume(eventCoroutine)
		assert(result, msg)			
	end
end

function UnLoadViewCommand:recursiveRemoveView(context, callback, parentConroutine)
	if (context) then
		--printf("UnLoadViewCommand---------------------%d", debug.getinfo(1).currentline)
		local parentContext = context:getParent()
		
		local childrenContext = context:getChildren()
		
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		
		--通过协程实现视图一个一个的退出视图栈
		local OneByOneExitView = nil
		OneByOneExitView = coroutine.create(function()
			--printf("UnLoadViewCommand---------------------%d", debug.getinfo(1).currentline)
			for i = #childrenContext, 1, -1  do
				--递归加载
				local childContext = childrenContext[i]
				self:recursiveRemoveView(childContext, callback, OneByOneExitView)
				coroutine.yield()
			end
			--printf("UnLoadViewCommand---------------------%d", debug.getinfo(1).currentline)
			local curView = context:getView()
			if (curView) then
				--从栈底往栈顶加
				curView:attach(BaseView.UNLOAD_VIEW_ASYNC_FINISHED, function (e)
					
					--视图进入
					if (nil == curView) then
						assert(false, "view expected not nil")								
					else
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
							parentContext:removeContext(context)
						else
							if (nil ~= string.find(curView:getDescription(), "Scene")) then
								local lastContext = contextProxy:getRootLastContext(context)
								context:setView(nil)
								assert(contextProxy:removeContext(context) ~= nil, "remove context failed, please check context exits")
								self:recursiveRestoreView(lastContext, callback)
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
			else
				
				if (parentContext) then
					parentContext:removeContext(context)
				else
					assert(contextProxy:removeContext(context) ~= nil, "remove context failed, please check context exits")
				end			
				printf("curView not in scene, remove context")
			end
		end)
		local result, msg = coroutine.resume(OneByOneExitView)
		assert(result, msg)			
	end
end

function UnLoadViewCommand:recursiveRestoreView(context, callback)
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
		
		--下面有些异步操作 必须retain()
		curView:retain()
		
		--从栈底往栈顶恢复
		curView:attach(BaseView.LOAD_VIEW_ASYNC_FINISHED, function (e)
			
			--context:lock()
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
			if (parentContext == nil) then
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
				local parentView = parentContext:getView()
				assert(parentView ~= nil, "parentView expected not be nil!")
				parentView:addChild(curView)
			end
												
												
			--等待视图进入完成后绑定上下文环境
			curView:attach(BaseView.ENTER_VIEW_ASYNC_FINISHED, function (e)
				
				--创建视图对应的Mediator
				--context:unlock()
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

return UnLoadViewCommand