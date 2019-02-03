--[[
    LoadViewCommand ֻ���� Mediators �� ViewComponents �Ĵ��������١�
    ���� ViewComponents ���л����̽��� ViewManager ������
    ���첽�ķ�ʽ�����������л������Ĺ������£�

        ������ȡ����ǰ���� Context�������������������Ȼ���������ɾ����
    ����ɾ������Ĺ����п��ܴ��ڶ���Ч������������������첽�ġ�
    �ⲿ�ֵĴ����� RemoveLayersCommand ��� ViewManager ʵ�֣��������µ�ǰ�����ĸ���㣻
    �������л������в�ͬ�ķ�ʽ����� ViewManager ���֣���������һ�ַ�ʽ������һ�������ĸ��ڵ㣻
    ���³����ĸ���㣨context�����в����������һ��ԭÿһ�����㣬���ڼ��ز���Ĺ��̿��ܴ��ڶ���Ч����
    �����������Ҳ���첽�ġ�
    �ⲿ�ֵĴ����� LoadLayersCommand ��� ViewManager ʵ�֣�
    �л�����������ɡ�
--]]
local LoadViewCommand = class("LoadViewCommand", puremvc.SimpleCommand)

function LoadViewCommand:execute(note)
    local msgData = note:getBody()
	local msgType = note:getType()

	--����ͼ�����Ļ���ջ
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	
--[[	--����Ҫ��ӵ���ͼ
	local willAddContext = msgData.context
	
	if (willAddContext:getParent() ~= nil) then
		assert(false, "this context already has parent context")
	end	--]]
	
	--suspend��running��dead��normal
	local eventCoroutine = nil
	eventCoroutine = coroutine.create(function()
		local willAddContext = nil
		
		local Context = require("client.src.model.base.Context")
		if (msgData.parm.canrepeat == false) then
			willAddContext = contextProxy:findContextByName(msgType) or Context.new()
		else
			willAddContext = Context.new()
		end

		--�ж��Ƿ�Ϊ�´����� Context ����
		if (willAddContext:getParent() == nil) then
			willAddContext:setName(msgType)
			willAddContext:setViewClass(msgData.viewClass)
			willAddContext:setMediatorClass(msgData.mediatorClass)			
		end	
		
		--�����������¸�ֵ
		if (type(msgData.parm) == "table") then
			willAddContext.data = {}
			for k, v in pairs(msgData.parm) do
				willAddContext.data[k] = v
			end
		end
		
		--���ô��� +1
		local refCount = willAddContext:retain()
		--printf("PUSH_VIEW %d", debug.getinfo(1).currentline)
		if (willAddContext.data.canrepeat == false) then
			if (refCount > 1) then
				printf("PUSH_VIEW FAILED,the view is find, attribute is not repeat!", debug.getinfo(1).currentline)
			else
				--�����Ļ�����ջ
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
			--�����Ļ�����ջ
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

--�ݹ�����������
function LoadViewCommand:recursiveResetView(context, callback, parentConroutine)
	if (context) then
		--for i = #childrenContext, 1, -1  do
		--	local childContext = childrenContext[i]
		--	self:recursiveResetView(childContext, callback)
		--end

		--ͨ��Э��ʵ����ͼһ��һ����������ͼջ
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
			
			--��ջ����ջ����
			curView:attach(BaseView.UNLOAD_VIEW_ASYNC_FINISHED, function (e)
				--��ͼ����
				if (nil == curView) then
					assert(false, "view expected not nil")								
				else
					--�Ƴ���ͼ��Ӧ��Mediator
					local curMediator = context.mediator
					if (nil ~= curMediator) then
						self.facade:removeMediator(curMediator.mediatorName)
						context:setMediator(nil)
					end
				end
											
				--�ȴ���ͼ������ɺ�������Ļ���
				curView:attach(BaseView.EXIT_VIEW_ASYNC_FINISHED, function (e)
					if (parentContext) then
						curView:removeFromParent()
						context:setView(nil)
					else
						--����ͼ�����Ļ���ջ
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
		
		--ͨ��Э��ʵ����ͼһ��һ���������ͼջ
		local OneByOneAddView = nil
		OneByOneAddView = coroutine.create(function()
			local curView = nil
			if (context.data.ctor ~= nil) then
				assert(type(context) == "table")
				curView = context.viewClass:create(unpack(context.data.ctor))
			else
				curView = context.viewClass:create()
			end
			--������Щ�첽���� ����retain()
			curView:retain()
			
			local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
			
			local parentContext = contextProxy:findContextByName(context.data.parentName)
			
			local currentRootContext = contextProxy:getCurrentRootContext()
		
			--�����Ļ�����ջ
			if (nil == parentContext) then --Ϊ�� һ���ǳ����ڵ�
				if  nil ~= currentRootContext then --˵���ǵ�һ����ӳ���
					if (string.find(curView:getDescription(), "Scene") ~= nil) then
						self:recursiveResetView(currentRootContext, nil, OneByOneAddView)
						coroutine.yield()
					end
				end	
			end
			
			--��ջ����ջ����
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
						--�Ƴ���ͼ��Ӧ��Mediator
						curView:unregisterScriptHandler()
					end
				end)
				--��ͼ����
				if (parentContext == nil) then
					if (string.find(curView:getDescription(), "Scene") ~= nil) then
						--��������ջ
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
				--����ͼ��Ϣ
				context:setView(curView)	
												
				--�ȴ���ͼ������ɺ�������Ļ���
				curView:attach(BaseView.ENTER_VIEW_ASYNC_FINISHED, function (e)
					if (context:getMediator() ~= nil) then
						--�Ѿ��󶨹��м�� ֱ�ӷ���
						printf("���ܴ��ڴ��󣬸���ͼ �����ظ����ͼ��������Ϊ��")
						return
					end
						
						--������ͼ��Ӧ��Mediator
					if (context.mediatorClass) then
						local targetMediator = context.mediatorClass.new(curView)
						targetMediator:setData(context.data)
						self.facade:registerMediator(targetMediator)
						context:setMediator(targetMediator)
					end
					
					local childrenContext = context:getChildren()
					for i = 1, #childrenContext do
						--�ݹ����
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
		
		--������Щ�첽���� ����retain()
		curView:retain()
		
		--��ջ����ջ���ָ�
		curView:attach(BaseView.LOAD_VIEW_ASYNC_FINISHED, function (e)
			--��ͼ����
			
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
			--��ͼ����
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

				--������ͼ��Ӧ��Mediator
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
				--����ͼ��Ϣ
				context:setView(curView)	
				
				--�ݹ�ָ���ͼ
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