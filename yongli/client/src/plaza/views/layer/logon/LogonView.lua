local LogonNotice = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.LogonNotice")

local LogonView = class("LogonView",function()
		local logonView =  display.newLayer()
    return logonView
end)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

LogonView.BT_LOGON = 1
LogonView.BT_REGISTER = 2
LogonView.CBT_RECORD = 3
LogonView.CBT_AUTO = 4
LogonView.BT_VISITOR = 5
LogonView.BT_WEIBO = 6
LogonView.BT_QQ	= 7
LogonView.BT_THIRDPARTY	= 8
LogonView.BT_WECHAT	= 9
LogonView.BT_FGPW = 10 	-- 忘记密码

function LogonView:ctor(serverConfig, viewBase)
	local this = self
	self:setContentSize(yl.WIDTH,yl.HEIGHT)
	--ExternalFun.registerTouchEvent(self)

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender,eventType)
    end

    local editHanlder = function ( name, sender )
		self:onEditEvent(name, sender)
	end
	
    -- 加载csb资源
	local filePath = ""
	if (BaseConfig.AGENT_IDENTIFICATION == "") then
		filePath = "Logon/LogonView.csb"
	else
		filePath = device.writablePath .. "client/res/Logon/LogonView_" .. BaseConfig.AGENT_IDENTIFICATION .. ".csb"
		if (not cc.FileUtils:getInstance():isFileExist(filePath)) then
			filePath = "Logon/LogonView_" .. BaseConfig.AGENT_IDENTIFICATION .. ".csb"
			if (not cc.FileUtils:getInstance():isFileExist(filePath)) then
				filePath = "Logon/LogonView.csb"
			end
		end		
	end
	
    local rootLayer, csbNode = appdf.loadRootCSB(filePath,self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(filePath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	

	local text_field_bg_1 = self._csbNode:getChildByName("account_bg")
	local text_field_1 = text_field_bg_1:getChildByName("text_field_1")	

	local text_field_size = text_field_1:getContentSize()

	self.edit_Account = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(31)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入您的游戏账号...")
		:addTo(text_field_1)
	self.edit_Account:registerScriptEditBoxHandler(editHanlder)
	
	local text_field_bg_2 = self._csbNode:getChildByName("pass_bg")
	local text_field_2 = text_field_bg_2:getChildByName("text_field_2")	
	
	text_field_size = text_field_2:getContentSize()
	
	self.edit_Password = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入您的游戏密码...")
		:addTo(text_field_2)

	--记住密码
	self._rememberPass = self._csbNode:getChildByName("cbtRememberPass")
	self._rememberPass:setTag(LogonView.CBT_RECORD)
	self._rememberPass:addTouchEventListener(btcallback)

	-- --自动登录
	-- self.cbt_Auto = ccui.CheckBox:create("cbt_auto_0.png","","cbt_auto_1.png","","")
	-- 	:move(700-93,245)
	-- 	:setSelected(GlobalUserItem.bAutoLogon)
	-- 	:setTag(LogonView.CBT_AUTO)
	-- 	:addTo(self)

	--账号登录
	local btnAccountsLogon = self._csbNode:getChildByName("btnAccountLogon")
		:setTag(LogonView.BT_LOGON)
        btnAccountsLogon:setPressedActionEnabled(true)
	btnAccountsLogon:addTouchEventListener(btcallback)
	
	--按钮添加监听
	local accountListener = cc.EventListenerCustom:create(yl.RY_START_LOGON,function (e)
		--禁止连续点击
		btnAccountsLogon:setEnabled(false)
		btnAccountsLogon:runAction(cc.Sequence:create(cc.DelayTime:create(3), 
										cc.CallFunc:create(function()
											btnAccountsLogon:setEnabled(true)
										end)
										)
					)
	end)
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(accountListener, btnAccountsLogon)

	--注册按钮
	
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	
	local registerLogon = self._csbNode:getChildByName("btnRemember")
    registerLogon:setPressedActionEnabled(true)
	registerLogon:setTag(LogonView.BT_REGISTER)
				:setVisible(appConfigProxy._appStoreSwitch == 0)
	registerLogon:addTouchEventListener(btcallback)

	--游客登录
	local btnVisitorsLogon = self._csbNode:getChildByName("btnFastLogon")
    btnVisitorsLogon:setPressedActionEnabled(true)
		:setTag(LogonView.BT_VISITOR)
		:setName("btn_2")
	btnVisitorsLogon:addTouchEventListener(btcallback)
	--按钮添加监听
	local visitorsListener = cc.EventListenerCustom:create(yl.RY_START_LOGON,function (e)
		--禁止连续点击
		btnVisitorsLogon:setEnabled(false)
		btnVisitorsLogon:runAction(cc.Sequence:create(cc.DelayTime:create(3), 
										cc.CallFunc:create(function()
											btnVisitorsLogon:setEnabled(true)
										end)
										)
					)
	end)
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(visitorsListener, btnVisitorsLogon)

--[[	--微信登陆
	ccui.Button:create("Logon/thrid_part_wx_0.png", "Logon/thrid_part_wx_1.png", "Logon/thrid_part_wx_2.png")
		:setTag(LogonView.BT_WECHAT)
		:move(cc.p(0,0))
		:setVisible(false)
		:setEnabled(false)
		:setName("btn_3")
		:addTo(self)
		:addTouchEventListener(btcallback)--]]


	--平台公告背景
	local notice_bg = self._csbNode:getChildByName("noticebg")
	
	--appstore版 屏蔽登录公告
	if (appConfigProxy._appStoreSwitch == 0) then
		notice_bg:setVisible(true)
	else
		notice_bg:setVisible(false)
	end
	
	--登录公告
	local notice_layout = notice_bg:getChildByName("notice")
	notice_layout:setVisible(appConfigProxy._appStoreSwitch == 0)
	local notice_layout_size = notice_layout:getContentSize()
	local notice = LogonNotice:create(cc.size(notice_bg:getContentSize().width * 0.9, notice_bg:getContentSize().height * 0.80))
		:move(0,0)
		:addTo(notice_layout)
		
	--获取公告内容
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/ws/MobileInterface.ashx?action=GetMobileLoginNotice","get","",function (jstable)
		if type(jstable) == "table" then
			local data = jstable["data"]
			local msg = jstable["msg"]
			if type(data) == "table" then
				local valid = data["valid"]
				if nil ~= valid and true == valid then
					local list = data["notice"]
					if type(list)  == "table" then
						local index = 1
						for k,v in ipairs(list) do
							local content = string.gsub(v.content," ","\n")
							if (index == #list) then
								notice:addItem(v.title, 28, cc.c3b(255, 0, 0), content, 20, cc.c3b(255, 255, 0), nil, true)
							else
								notice:addItem(v.title, 28, cc.c3b(255, 0, 0), content, 20, cc.c3b(255, 255, 0), nil, false)
							end
							index = index + 1
						end
					end
				end
			end
		end
	end)
	
	

	self.m_serverConfig = serverConfig or {}
	
	self:refreshBtnList()
end

function LogonView:enableRegisterWithTimes(times)
	local ref = btn
	ref:setEnabled(false)
	ref:runAction(cc.Sequence:create(cc.DelayTime:create(3), 
									cc.CallFunc:create(function()
										ref:setEnabled(true)
									end)
									)
				)
end

function LogonView:refreshBtnList( )
	for i = 1, 3 do
		local btn = self:getChildByName("btn_" .. i)
		if btn ~= nil then
			btn:setVisible(false)
			btn:setEnabled(false)
		end
	end
	
	local btncount = 1
	local btnpos = 
	{
		{cc.p(667, 70), cc.p(0, 0), cc.p(0, 0)},
		{cc.p(463, 70), cc.p(868, 70), cc.p(0, 0)},
		{cc.p(740, 170), cc.p(1200, 717), cc.p(980, 717)}
	}	
	-- 1:帐号 2:游客 3:微信
	local btnlist = {"btn_1"}
	if false == GlobalUserItem.getBindingAccount() then
		table.insert(btnlist, "btn_2")
	end

	local enableWeChat = self.m_serverConfig["wxLogon"] or 1
	if 0 == enableWeChat then
		table.insert(btnlist, "btn_3")
	end

	local poslist = btnpos[#btnlist]
	for k,v in pairs(btnlist) do
		local tmp = self:getChildByName(v)
		if nil ~= tmp then
			tmp:setEnabled(true)
			tmp:setVisible(true)

			local pos = poslist[k]
            if nil ~= pos then
            	tmp:setPosition(pos)
            end
		end
	end
end

function LogonView:onEditEvent(name, editbox)
	--print(name)
	if "changed" == name then
		if editbox:getText() ~= GlobalUserItem.szAccount then
			self.edit_Password:setText("")
		end		
	end
end

function LogonView:onReLoadUser()
	if GlobalUserItem.szAccount ~= nil and GlobalUserItem.szAccount ~= "" then
		self.edit_Account:setText(GlobalUserItem.szAccount)
	end

	if GlobalUserItem.szPassword ~= nil and GlobalUserItem.szPassword ~= "" then
		self.edit_Password:setText(GlobalUserItem.szPassword)
	end
end

function LogonView:onButtonClickedEvent(tag,ref)
	ExternalFun.playClickEffect()
	if tag == LogonView.BT_REGISTER then
	GlobalUserItem.bVisitor = false
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW)
		self:getParent():getParent():onShowAccountRegister()
       -- showToast(self, "注册联系推广员" , 2)

	elseif tag == LogonView.BT_VISITOR then
		GlobalUserItem.bVisitor = true
		self:getParent():getParent():onVisitor()
        -- showToast(self, "注册联系推广员" , 2)
	elseif tag == LogonView.BT_LOGON then
		GlobalUserItem.bVisitor = false
		local szAccount = string.gsub(self.edit_Account:getText(), " ", "")
		local szPassword = string.gsub(self.edit_Password:getText(), " ", "")
		local bAuto = self._rememberPass:isSelected()
		local bSave = self._rememberPass:isSelected()
		self:getParent():getParent():onLogon(szAccount,szPassword,bSave,bAuto)

		--GlobalUserItem.szAccount = szAccount
		--GlobalUserItem.szPassword = szPassword
		--GlobalUserItem.bAutoLogon = bAuto
		--GlobalUserItem.bSavePassword = bSave
		--FriendMgr:getInstance():reSetAndLogin()
	elseif tag == LogonView.BT_THIRDPARTY then
		self.m_spThirdParty:setVisible(true)
	elseif tag == LogonView.BT_WECHAT then
		--平台判定
		local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
			self:getParent():getParent():thirdPartyLogin(yl.ThirdParty.WECHAT)
		else
			showToast(self, "不支持的登录平台 ==> " .. targetPlatform, 2)
		end
	elseif tag == LogonView.BT_FGPW then
		MultiPlatform:getInstance():openBrowser(BaseConfig.WEB_HTTP_URL .. "/Mobile/RetrievePassword.aspx")
	end
end

function LogonView:onTouchBegan(touch, event)
	return self:isVisible()
end

function LogonView:onTouchEnded(touch, event)
	local pos = touch:getLocation();
	local m_spBg = self.m_spThirdParty
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self.m_spThirdParty:setVisible(false)
    end
end

return LogonView