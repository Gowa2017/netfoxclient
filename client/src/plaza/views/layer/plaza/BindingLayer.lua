--[[
	绑定账号界面
	2016_06_23 Ravioyla
]]

local BindingLayer = class("BindingLayer", function(scene)
		local BindingLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return BindingLayer
end)

local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")
local QueryDialog = require("app.views.layer.other.QueryDialog")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")

BindingLayer.BT_BINDING			= 15
BindingLayer.BT_BINDINGREGISTER = 16

-- 进入场景而且过渡动画结束时候触发。
function BindingLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function BindingLayer:onExitTransitionStart()
    return self
end

function BindingLayer:ctor(scene)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
        elseif eventType == "exit" then
            if self._modifyFrame:isSocketServer() then
                self._modifyFrame:onCloseSocket()
            end
		end
	end)

	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --网络回调
    local modifyCallBack = function(result,message)
		this:onModifyCallBack(result,message)
	end
    --网络处理
	self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,yl.HEIGHT - 51)
		self:addChild(sp)
	end
	display.newSprite("Binding/title_binding.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,320)
		self:addChild(sp)
	end
    ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)

    --提示
    display.newSprite("Binding/text_binding_0.png")
    	:move(yl.WIDTH/2,503)
    	:addTo(self)
    display.newSprite("Binding/text_binding_1.png")
    	:move(505,390)
    	:setAnchorPoint(cc.p(1,0.5))
    	:addTo(self)
    display.newSprite("Binding/text_binding_2.png")
    	:move(505,284)
    	:setAnchorPoint(cc.p(1,0.5))
    	:addTo(self)

    --账号输入
	self.edit_Account = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Binding/frame_binding_0.png"))
		:move(515,390)
		:setAnchorPoint(cc.p(0.0,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:addTo(self)

	--密码输入	
	self.edit_Password = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Binding/frame_binding_0.png"))
		:move(515,284)
		:setAnchorPoint(cc.p(0.0,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:addTo(self)

	--绑定按钮
	ccui.Button:create("Binding/bt_binding_0.png","Binding/bt_binding_1.png")
		:setTag(BindingLayer.BT_BINDING)
		:move(yl.WIDTH/2,140)
		:addTo(self)
		:addTouchEventListener(self._btcallback)

    --注册绑定按钮
    ccui.Button:create("Binding/bt_registerbind.png","Binding/bt_registerbind.png")
        :setTag(BindingLayer.BT_BINDINGREGISTER)
        :move(yl.WIDTH - 200,70)
        :addTo(self)
        :addTouchEventListener(self._btcallback)

end

--按键监听
function BindingLayer:onButtonClickedEvent(tag,sender)
	if tag == BindingLayer.BT_BINDING then
        local szAccount = self.edit_Account:getText()
        local szPassword = self.edit_Password:getText()
        --输入检测
        if szAccount == nil then
            showToast(self,"游戏帐号必须为6~31个字符，请重新输入！",2,cc.c4b(250,0,0,255))
            return
        end
        if nil == szPassword then
            showToast(self,"密码必须大于6个字符，请重新输入！",2,cc.c4b(250,0,0,255))
            return 
        end
        local len = #szAccount
        if len < 6 or len > 31 then
            showToast(self,"游戏帐号必须为6~31个字符，请重新输入！",2,cc.c4b(250,0,0,255))
            return
        end

        len = #szPassword
        if  len<6 then
            showToast(self,"密码必须大于6个字符，请重新输入！",2,cc.c4b(250,0,0,255))
            return
        end
        self.szAccount = szAccount
        self.szPassword = szPassword

        local tips = "绑定帐号后该游客信息将与新帐号合并，游客帐号将会被注销，绑定成功之后需要重新登录,是否绑定帐号?"
        self._queryDialog = QueryDialog:create(tips, function(ok)
            if ok == true then                
                self:bindingAccount()
            end
            self._queryDialog = nil
        end):setCanTouchOutside(false)
            :addTo(self)
	elseif tag == BindingLayer.BT_BINDINGREGISTER then
        self._scene:onChangeShowMode(yl.SCENE_BINDINGREG)
    end
end

--操作结果
function BindingLayer:onModifyCallBack(result,message)
	print("======== BindingLayer::onModifyCallBack ========")

	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2);
	end

	if result == 2 then
        self._scene:showPopWait()
        GlobalUserItem.setBindingAccount()
        GlobalUserItem.szPassword = self.szPassword
        GlobalUserItem.szAccount = self.szAccount
        --保存数据
        GlobalUserItem.onSaveAccountConfig()

        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function ()
            self._scene:dismissPopWait()
                  
            --重新登录
            GlobalUserItem.nCurRoomIndex = -1
            self:getParent():getParent():getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.LogonScene","FADE",1)
            GlobalUserItem.reSetData()
            --读取配置
            GlobalUserItem.LoadData()
            --断开好友服务器
            FriendMgr:getInstance():reSetAndDisconnect()
            --通知管理
            NotifyMgr:getInstance():clear()
            end)))
	end
end

function BindingLayer:bindingAccount()    
    self._scene:showPopWait()
    print(self.szPassword)
    self._modifyFrame:onAccountBinding(self.szAccount, md5(self.szPassword))
end

return BindingLayer