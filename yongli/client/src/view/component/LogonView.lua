--[[
  登录界面
      2015_12_03 C.P
      功能：登录/注册
--]]
local LogonView = class("LogonView", cc.BaseLayerView)

LogonView.BT_EXIT = 1

-- 进入场景而且过渡动画结束时候触发。
function LogonView:onEnterTransitionFinish()
    return self
end
-- 退出场景而且开始过渡动画时候触发。
function LogonView:onExitTransitionStart()
    return self
end

--[[function LogonView:onViewAsyncLoad()
	return self
end--]]

function LogonView:onViewAsyncEnter()
	return self
end
	
--[[function LogonView:onViewAsyncUnLoad()
	return self
end--]]

function LogonView:onViewAsyncExit()
	return self
end

-- 初始化界面
function LogonView:ctor()
	print("LogonView:onCreate")
	self.super.ctor(self)
	local this = self

	local visibleSize = cc.Director:getInstance():getVisibleSize()

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    local logonCallBack = function (result,message)
		this:onLogonCallBack(result,message)
	end

	self._backLayer = display.newLayer()
		:addTo(self)
	
	--返回键事件
	self._backLayer:registerScriptKeypadHandler(function(event)
		if event == "backClicked" then
			if this._popWait == nit then
				this:onButtonClickedEvent(LogonView.BT_EXIT)
			end
		end
	end)
	self._backLayer:setKeyboardEnabled(true)

	self._topLayer = display.newLayer()
		:addTo(self)

	--背景
    display.newSprite("base/background_1.jpg")
        :move(visibleSize.width/2,visibleSize.height/2)
        :addTo(self._backLayer)
 
 	--平台logo
	display.newSprite("client/Logon/logon_logo.png")
		:move(visibleSize.width/2,visibleSize.height-150)
		:addTo(self._backLayer)
		:runAction(cc.MoveTo:create(0.3,cc.p(visibleSize.width/2,visibleSize.height-150)))

	self._txtTips = cc.Label:createWithTTF("同步服务器信息中...", "base/fonts/round_body.ttf", 24)
		:setTextColor(cc.c4b(0,250,0,255))
		:setAnchorPoint(cc.p(1,0))
		:setVisible(false)
		:enableOutline(cc.c4b(0,0,0,255), 1)
		:move(visibleSize.width,0)
		:addTo(self._topLayer)
		
	--返回
	ccui.Button:create("base/bt_return_0.png","base/bt_return_1.png")
		:move(75,visibleSize.height-51)
		:setTag(LogonView.BT_EXIT)
		:addTo(self._backLayer)
		:addTouchEventListener(btcallback)
end

--按钮事件
function LogonView:onButtonClickedEvent(tag,ref)
	--退出按钮
	if tag == LogonView.BT_EXIT then
			--local a =  Integer64.new()

--[[	print(a:getstring())
	
		if self:getChildByTag(LogonView.DG_QUERYEXIT) then
			return
		end
		QueryExit:create("确认退出APP吗？",function(ok)
				if ok == true then
					os.exit(0)
				end
			end)
			:setTag(LogonView.DG_QUERYEXIT)
			:addTo(self)--]]
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.LOGON_SCENE})
	end
end

return LogonView