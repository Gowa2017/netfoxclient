--[[
	银行界面
	2017_08_25 MXM
]]

local BankOpenLayer = class("BankOpenLayer", cc.BaseLayer)

local BankFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BankFrame")
local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--开通银行
function BankOpenLayer:ctor(gameFrame)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "Bank/BankOpenLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

function BankOpenLayer:initUI()
	local imgBg = self._csbNode:getChildByName("bg")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_ok")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--密码输入	
	local pass_bg = imgBg:getChildByName("text_field_bg_1")
	local text_field_1 = pass_bg:getChildByName("text_field_1")	

	text_field_size = text_field_1:getContentSize()

	self.m_editNewPass = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入账号密码...")
		:addTo(text_field_1)
		
		
	--确认密码输入	
	local repass_bg = imgBg:getChildByName("text_field_bg_2")
	local text_field_2 = repass_bg:getChildByName("text_field_2")	
	
	text_field_size = text_field_2:getContentSize()
	
	self.m_editConfirmPass = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请确认账号密码...")
		:addTo(text_field_2)
		
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
	end)	
		
	-- 添加点击监听事件 遮罩
    local mask = self._csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
--[[		local pos = touch:getLocation();
		local pBg = self._csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_OPEN_LAYER})
		end
		return true--]]
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function BankOpenLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function BankOpenLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)

	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function BankOpenLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()


	local senderName = sender:getName()
	
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_OPEN_LAYER})
	elseif senderName == "btn_ok" then
		self:onEnableBank()
	end
end

--开通银行
function BankOpenLayer:onEnableBank()
	
	--参数判断
	local szPass = self.m_editNewPass:getText()
	local szPassConfirm = self.m_editConfirmPass:getText()

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",2)
		return
	end
	if #szPass < 6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	if #szPassConfirm < 1 then 
		showToast(self,"请在确认栏输入银行密码！",2)
		return
	end
	if #szPassConfirm < 6 then
		showToast(self,"确认栏密码必须大于6个字符，请重新输入！",2)
		return
	end

	if szPass ~= szPassConfirm then
		showToast(self,"设置栏和确认栏的密码不相同，请重新输入！",2)
        return
	end

    -- 与帐号不同
    if string.lower(szPass) == string.lower(GlobalUserItem.szAccount) then
        showToast(self,"密码不能与帐号相同，请重新输入！",2)
        return
    end

    -- 银行不同登陆
    if string.lower(szPass) == string.lower(GlobalUserItem.szPassword) then
        showToast(self, "银行密码不能与登录密码一致!", 2)
        return
    end
    
    --[[-- 首位为字母
    if 1 ~= string.find(szPass, "%a") then
        showToast(self,"密码首位必须为字母，请重新输入！",2)
        return
    end]]

	self:showPopWait()
	self:emit("V2M_OpenBankStart", szPass, function (code, msg)
		if (code == 0) then
			showToast(self, msg, 2)
			--关闭开通层
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_OPEN_LAYER})
			--打开银行界面
			if 0 ~= GlobalUserItem.cbInsureEnabled then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_LAYER)
			end		
		end
		self:dismissPopWait()
	end)
end

--显示等待
function BankOpenLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function BankOpenLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return BankOpenLayer