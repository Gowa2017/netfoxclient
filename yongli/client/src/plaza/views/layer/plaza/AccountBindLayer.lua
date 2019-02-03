--[[
	游客绑定手机
	2017_10_30 MXM
]]

local AccountBindLayer = class("AccountBindLayer", cc.Layer)
	
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local scheduler = cc.Director:getInstance():getScheduler()

local BTN_CLOSE = 1
local BTN_GETCODE = 2
local BTN_SUREBIND = 3

local _cookie = ""  --临时变量保存cookie验证码

function AccountBindLayer:ctor()
	 -- 注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
	
	local this = self

	-- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("VisitorBind/AccountBindLayer.csb", self)
	
	self.m_spBg = csbNode:getChildByName("accountbind_bg")
    local bg = self.m_spBg
	
	local function callback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	
	--关闭按钮
	self.btnclose = bg:getChildByName("btn_close")
	self.btnclose:setTag(BTN_CLOSE)
	self.btnclose:addTouchEventListener(callback)
	
	--手机号码
	--self.m_phonenums = bg:getChildByName("Text_phone")
		--:setText("")
	local tmp = bg:getChildByName("Text_phone")
	local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(11)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("请输入您的手机号码")
	bg:addChild(editbox)
	self.m_editphone = editbox
	
	--验证码
	local tmp = bg:getChildByName("Text_code")
	local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(6)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("请输入您的验证码")
	bg:addChild(editbox)
	self.m_editcode = editbox
	
	--获取验证码
	self._btncode = bg:getChildByName("btn_getcode")
		:setTitleText("获取验证码")
		:setTitleColor(cc.c4b(67,31,8,255))
		:setTitleFontSize(30)
	self._btncode:setTag(BTN_GETCODE)
	self._btncode:addTouchEventListener(callback)
	
	self._text_getcode = bg:getChildByName("Text_getcode")
		:setVisible(false)
		
	--新密码编辑
	tmp = bg:getChildByName("bind_edit_new")
	editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入您的新登录密码")
	bg:addChild(editbox)
	self.m_editNewPass = editbox
	--确认密码编辑
	tmp = bg:getChildByName("bind_edit_confirm")
	editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请再次输入您的新登录密码")
	bg:addChild(editbox)
	self.m_editConfirmPass = editbox	
	
	 --确定按钮
    self.btnsurebind = bg:getChildByName("btn_surebind")
    self.btnsurebind:setTag(BTN_SUREBIND)
    self.btnsurebind:addTouchEventListener(callback)
	
end

function AccountBindLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	if BTN_CLOSE == tag then
		self:getParent():removeFromParent()
	elseif BTN_SUREBIND == tag then
		local var, tips = self:confirmPasswd()
		if false == var then
			showToast(self, tips, 2)
			return
		end
		self:sendbindInfo()
	elseif BTN_GETCODE == tag then
		self:getVerificationCode()
	end
	
end

function AccountBindLayer:confirmPasswd(  )
	
	local bindphone = self.m_editphone:getText()
	local newpass = self.m_editNewPass:getText()
	local confirm = self.m_editConfirmPass:getText()
	
	local phonelen = ExternalFun.stringLen(bindphone)
	if 0 == phonelen then
		return false, "手机号码不能为空!"
	end
	
	if phonelen<11 then
		return false, "请填写有效的11位手机号码"
	end
	
	local newlen = ExternalFun.stringLen(newpass)
	if 0 == newlen then
		return false, "绑定手机,密码不能为空!"
	end

	if newlen > 26 or newlen < 6 then
		return false, "密码请输入6-26位字符"
	end

	--空格
	local b,e = string.find(newpass, " ")
	if b ~= e then
		return false, "密码不能输入空格字符,请重新输入"
	end

	--密码确认
	if newpass ~= confirm then
		return false, "两次输入的密码不一致,请重新输入"
	end

	-- 首位为字母
	--[[if 1 ~= string.find(newpass, "%a") then
		return false, "密码首位必须为字母，请重新输入！"
	end]]

	-- 与帐号不同
	if string.lower(newpass) == string.lower(GlobalUserItem.szAccount) then
		return false, "密码不能与帐号相同，请重新输入！"
	end
    
	return true
end

function AccountBindLayer:clearEdit(  )
	self.m_editphone:setText("")
	self.m_editcode:setText("")
	self.m_editNewPass:setText("")
	self.m_editConfirmPass:setText("")
end

--获取手机验证码
function AccountBindLayer:getVerificationCode()
	
	self.m_getCodeTime = 0
	if (self.m_getCodeTime >0) then
		return
	end
	
	self.m_getCodeTime = 60
	self._btncode:setEnabled(false)
	
	local ostime = os.time()
	local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
	  appdf.onHttpJsionTable(url ,"GET","action=SendCode&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. self.m_editphone:getText(),function(jstable,jsdata)
      --LogAsset:getInstance():logData("action=GetMobileShareConfig&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime))
		print("appdf.onHttpJsionTableaaaaaa:"..url.."?action=SendCode&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. self.m_editphone:getText())
		dump(jstable, "jstable", 6)
		local msg = "验证码获取失败"
		
        --self._cookie = jstable["code"]

		if type(jstable) == "table" then
			msg = jstable["msg"]
            local code = jstable["code"]
			if code==0 then
				local data = jstable["data"]
				if type(data) == "table" then
					--_cookie=data["cookie"]
                    _cookie=data["code"]
				end
				
				local maxCount = 60
				self._btncode:runAction(cc.RepeatForever:create(cc.Sequence:create(
				cc.DelayTime:create(1),
				cc.CallFunc:create(function()
					if (self.m_getCodeTime <= 0) then
						self._btncode:setTitleText("获取验证码")
						self._btncode:setEnabled(true)
						self._btncode:stopAllActions()
					elseif (self.m_getCodeTime == maxCount) then
						self._btncode:setEnabled(false)
						self._btncode:setTitleText(string.format("发送中%d秒", self.m_getCodeTime))
						self.m_getCodeTime = self.m_getCodeTime - 1
					else
						self._btncode:setTitleText(string.format("发送中%d秒", self.m_getCodeTime))
						self.m_getCodeTime = self.m_getCodeTime - 1
					end
				end)
				)))
			else
				self._btncode:setEnabled(true)
			end

		end
        --self._scene:dismissPopWait()
		if type(msg) == "string" and "" ~= msg then
			showToast(self,msg,2,cc.c3b(250,0,0))
		end
	end)

end

--[[function AccountBindLayer:update( dt )
	if ( self.m_getCodeTime <= 0) then
		self:unSchedule()
		self._btncode:setTitleText("获取验证码")
		self._btncode:setEnabled(true)
		return
	end
	
	self.m_getCodeTime =  self.m_getCodeTime-dt
	local str = string.format("发送中%ds", self.m_getCodeTime)
	self._btncode:setTitleText(str)
end--]]

function AccountBindLayer:sendbindInfo()
	
    local ostime = os.time()
	local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
	  appdf.onHttpJsionTable(url ,"GET","action=BindPhone&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. self.m_editphone:getText().."&code=".. self.m_editcode:getText().."&pass=".. self.m_editNewPass:getText().."&type=BindPhone&cookie=".._cookie,function(jstable,jsdata)
      print("appdf.onHttpJsionTablebbbbb:"..url.."?action=BindPhone&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. self.m_editphone:getText().."&code=".. self.m_editcode:getText().."&pass=".. self.m_editNewPass:getText().."&type=BindPhone&cookie=".._cookie)

		dump(jstable, "jstable", 6)
		local msg = "抱歉,绑定手机号码失败"
		if type(jstable) == "table" then
			msg = jstable["msg"]
            local code = jstable["code"]
			
			if code == 0 then
				--成功后把填写的手机号赋值给账号跟昵称 密码重新赋值
				GlobalUserItem.szAccount = self.m_editphone:getText()
				GlobalUserItem.szNickName = self.m_editphone:getText() 
				GlobalUserItem.szRegisterMobile = self.m_editphone:getText() 
				GlobalUserItem.szPassword = self.m_editNewPass:getText()
				--更新登录模式为账号登录
				GlobalUserItem.cbLogonMode = yl.LOGON_MODE_ACCOUNTS
				GlobalUserItem.onSaveAccountConfig()
				self:clearEdit()
				local runScene = cc.Director:getInstance():getRunningScene()
				if type(msg) == "string" and "" ~= msg then
					showToast(runScene,msg,2,cc.c3b(250,0,0))
				end
				self:getParent():removeFromParent()
			elseif msg =="验证码错误" then
				if(self.count_error==nil) then
				  self.count_error = 1
				else
					self.count_error = self.count_error+1
				end
				if(self.count_error>=3) then
					_cookie=""
					self.count_error=0
					showToast(self,"验证码错误多次，请重新发送验证码",2,cc.c3b(250,0,0))
					return
				end
				
			end
			
		end
		
        --self._scene:dismissPopWait()

	end)

end

return AccountBindLayer