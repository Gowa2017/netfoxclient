--[[
	询问对话框
		2016_04_27 C.P
	功能：确定/取消 对话框 与用户交互
]]
local QueryDialog = class("QueryDialog", cc.BaseLayer)

--默认字体大小
QueryDialog.DEF_TEXT_SIZE 	= 32

--UI标识
QueryDialog.DG_QUERY_EXIT 	=  2 
QueryDialog.BT_CANCEL		=  0   
QueryDialog.BT_CONFIRM		=  1

-- 对话框类型
QueryDialog.QUERY_SURE 			= 1
QueryDialog.QUERY_SURE_CANCEL 	= 2

-- 进入场景而且过渡动画结束时候触发。
function QueryDialog:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function QueryDialog:onExitTransitionStart()
	self:unregisterScriptTouchHandler()
    return self
end

--窗外触碰
function QueryDialog:setCanTouchOutside(canTouchOutside)
	self._canTouchOutside = canTouchOutside
	return self
end

--msg 显示信息
--callback 交互回调
--txtsize 字体大小
function QueryDialog:ctor(msg, callback, canTouchOutside, queryType)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "base/QueryDialog.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode	
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	queryType = queryType or QueryDialog.QUERY_SURE_CANCEL
	self._callback = callback
	self._canTouchOutside = canTouchOutside

	local imgQueryDialog = self._csbNode:getChildByName("QueryDialogBg")
--[[	--区域外取消显示
	local  onQueryExitTouch = function(eventType, x, y)
		if not self._canTouchOutside then
			return true
		end

		if self._dismiss == true then
			return true
		end

		if eventType == "began" then
			local rect = imgQueryDialog:getBoundingBox()
        	if cc.rectContainsPoint(rect,cc.p(x,y)) == false then
        		self:dismiss()
    		end
		end
    	return true
    end
	self:registerScriptTouchHandler(onQueryExitTouch)--]]
	
	
	
    local mask = csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		if not self._canTouchOutside then
			return true
		end

		local pos = touch:getLocation();
		local pBg = csbNode:getChildByName("QueryDialogBg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			self:dismiss()
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)	
	
		
	
	local btnSure = imgQueryDialog:getChildByName("btn_sure")
	local btnOK = imgQueryDialog:getChildByName("btn_ok")
	local btnCancel = imgQueryDialog:getChildByName("btn_cancel")
	local btnExit = imgQueryDialog:getChildByName("btn_close")

	if QueryDialog.QUERY_SURE == queryType then
		btnSure:setVisible(true)
		btnSure:addClickEventListener(handler(self, self.onButtonClickedEvent))
		btnOK:setVisible(false)
		btnCancel:setVisible(false)
	else
		btnOK:setVisible(true)
		btnOK:addClickEventListener(handler(self, self.onButtonClickedEvent))
		btnSure:setVisible(false)
		btnCancel:setVisible(true)
		btnCancel:addClickEventListener(handler(self, self.onButtonClickedEvent))
	end
	
	btnExit:addClickEventListener(handler(self, self.onButtonClickedEvent))

	local textMsg = imgQueryDialog:getChildByName("tips")
	
	if (msg ~= nil) then
		assert(type(msg) == "string")
		textMsg:setString(msg)
	else
		textMsg:setString("")
	end	
end

function QueryDialog:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function QueryDialog:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

--按键点击
function QueryDialog:onButtonClickedEvent(ref)
	--取消显示
	self:dismiss()
	--通知回调
	if self._callback then
		self._callback(ref:getName() == "btn_ok")
	end
end

--取消消失
function QueryDialog:dismiss()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
end

return QueryDialog
