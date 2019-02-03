--[[
	密码修改界面
	2017_09_05 MXM
]]
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ModifyBankPasswdLayer = class("ModifyBankPasswdLayer", cc.BaseLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function ModifyBankPasswdLayer:ctor()
	self.super.ctor(self)
	
    -- 加载csb资源
	local csbPath = "modify/ModifyBankPassLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)

	local imgBg = self._csbNode:getChildByName("bg")
	
	--标题
	self.sp_title = imgBg:getChildByName("title")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_ok")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --取消按钮
    btn = imgBg:getChildByName("btn_cancel")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))	


	--手机号输入
	local phone_bg = imgBg:getChildByName("text_field_bg_1")
	local text_field_1 = phone_bg:getChildByName("text_field_1")
	
	local text_field_size = text_field_1:getContentSize()
	
	self.m_editPhone = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(31)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入手机号码...")
		:addTo(text_field_1)
		
		
	--验证码
	local verify_bg = imgBg:getChildByName("text_field_bg_2")
	local text_field_2 = verify_bg:getChildByName("text_field_2")	

	text_field_size = text_field_2:getContentSize()
	
	self.m_editcode = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)--:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("验证码...")
		:addTo(text_field_2)
	

	--密码输入	
	local pass_bg = imgBg:getChildByName("text_field_bg_3")
	local text_field_3 = pass_bg:getChildByName("text_field_3")	

	text_field_size = text_field_3:getContentSize()

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
		:addTo(text_field_3)
		
		
	--确认密码输入	
	local repass_bg = imgBg:getChildByName("text_field_bg_4")
	local text_field_4 = repass_bg:getChildByName("text_field_4")	
	
	text_field_size = text_field_4:getContentSize()
	
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
		:addTo(text_field_4)
		
	self._btncode = imgBg:getChildByName("btn_get_verify_code")
	self._btncode:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	
	-- 添加点击监听事件 遮罩
    local mask = csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.MODIFY_BANK_PASS_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
	
end

function ModifyBankPasswdLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openNormalAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("openNoBindAni", function ()
		self:enterViewFinished()
	end)
	
	--判断是否是游客模式 游客模式显示不同的界面
	if GlobalUserItem.szRegisterMobile == "" then
		self._csbNodeAni:play("openNoBindAni", false)
	else
		self._csbNodeAni:play("openNormalAni", false)
	end
end

function ModifyBankPasswdLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeNormalAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("closeNoBindAni", function ()
		self:exitViewFinished()
	end)
	
	--判断是否是游客模式 游客模式显示不同的界面
	if GlobalUserItem.szRegisterMobile == "" then
		self._csbNodeAni:play("closeNoBindAni", false)
	else
		self._csbNodeAni:play("closeNormalAni", false)
	end
end

function ModifyBankPasswdLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	if senderName == "btn_get_verify_code" then
		self:getVerificationCode()
	elseif senderName == "btn_close" or senderName == "btn_cancel" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.MODIFY_BANK_PASS_LAYER})
	elseif senderName == "btn_ok" then
		local pass, msg = self:confirmPasswd()
		if (pass) then
			self:sendbankPassInfo()
		else
			self:showToast(msg)
		end
	end
end

function ModifyBankPasswdLayer:onModifyCallBack( result, tips )
	if type(tips) == "string" and "" ~= tips then
		self:showToast(tips)
	end
	
	if -1 ~= result then
		self:clearEdit()
	end
end

function ModifyBankPasswdLayer:clearEdit(  )
	self.m_editcode:setText("")
	self.m_editNewPass:setText("")
	self.m_editConfirmPass:setText("")
end

function ModifyBankPasswdLayer:confirmPasswd(  )
	local newpass = self.m_editNewPass:getText()
	local confirm = self.m_editConfirmPass:getText()

	local newlen = ExternalFun.stringLen(newpass)
	if 0 == newlen then
		return false, "新密码不能为空!"
	end

	if newlen > 26 or newlen < 6 then
		return false, "新密码请输入6-26位字符"
	end

	--空格
	local b,e = string.find(newpass, " ")
	if b ~= e then
		return false, "新密码不能输入空格字符,请重新输入"
	end

	--新旧密码
	if oldpass == newpass then
		return false, "新密码与原始密码一致,请重新输入"
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

--获取手机验证码
function ModifyBankPasswdLayer:getVerificationCode()
	local GetPassData = ExternalFun.create_netdata(logincmd.CMD_MB_GetModifyPassVerifyCode)
	GetPassData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_GET_MODIFY_PASS_VERIFYCODE)

	local phoneNumber = self.m_editPhone:getText()
	if not appdf.checkPhone(phoneNumber) then
		self:showToast("手机号不正确，请重新输入！")
		return
	end
	
	if GlobalUserItem.szRegisterMobile ~= phoneNumber then
		self:showToast("与绑定手机号不一致，请重新输入！")
		return
	end	
	
	GetPassData:pushstring(phoneNumber,yl.LEN_MOBILE_PHONE)
	GetPassData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)		
		
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, GetPassData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
				--清空原来的列表
				local code = Datatable.data:readbyte()
				if (code == 0) then
					local maxCount = 60
					local timeCount = 60
					self._btncode:runAction(cc.RepeatForever:create(cc.Sequence:create(
					cc.DelayTime:create(1),
					cc.CallFunc:create(function()
						if (timeCount <= 0) then
							self._btncode:setTitleText("获取验证码")
							self._btncode:setEnabled(true)
							self._btncode:stopAllActions()
						elseif (timeCount == maxCount) then
							self._btncode:setEnabled(false)
							self._btncode:setTitleText(string.format("发送中%d秒", timeCount))
							timeCount = timeCount - 1
						else
							self._btncode:setTitleText(string.format("发送中%d秒", timeCount))
							timeCount = timeCount - 1
						end
					end)
					)))
				end
				self:showToast(Datatable.data:readstring(64))
			end
		end
	end)
	
end

function ModifyBankPasswdLayer:sendloginPassInfo()
	local ModifyPassData = ExternalFun.create_netdata(logincmd.CMD_MB_ModifyPassVerifyCode)
	ModifyPassData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_MODIFY_PASS_VERIFYCODE)

	local phoneNumber = self.m_editPhone:getText()
	if not appdf.checkPhone(phoneNumber) then
		self:showToast("手机号不正确，请重新输入！")
		return
	end
	
	if GlobalUserItem.szRegisterMobile ~= phoneNumber then
		self:showToast("与绑定手机号不一致，请重新输入！")
		return
	end
	
	ModifyPassData:pushstring(GlobalUserItem.szAccount, yl.LEN_ACCOUNTS)
	ModifyPassData:pushstring(md5(self.m_editNewPass:getText()), yl.LEN_PASSWORD)
	ModifyPassData:pushbyte(1)
	ModifyPassData:pushstring(self.m_editcode:getText(),yl.LEN_VERIFY_CODE)
	ModifyPassData:pushstring(phoneNumber,yl.LEN_MOBILE_PHONE)
	ModifyPassData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)		
		
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ModifyPassData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
				--清空原来的列表
				local code = Datatable.data:readbyte()
				self:showToast(Datatable.data:readstring(64))
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_SUCCESS) then
				GlobalUserItem.szRegisterMobile = phoneNumber
				self.m_editPhone:setEnabled(false)
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				self:showToast(szDescribe)
				--修改完成后自动关闭
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.MODIFY_BANK_PASS_LAYER})
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_FAILURE) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				self:showToast(szDescribe)
			end
		end
	end)	

end

function ModifyBankPasswdLayer:sendbankPassInfo()
	local ModifyPassData = ExternalFun.create_netdata(logincmd.CMD_MB_ModifyPassVerifyCode)
	ModifyPassData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_MODIFY_PASS_VERIFYCODE)

	local phoneNumber = self.m_editPhone:getText()
	if not appdf.checkPhone(phoneNumber) then
		self:showToast("手机号不正确，请重新输入！")
		return
	end
	
	if GlobalUserItem.szRegisterMobile ~= phoneNumber then
		self:showToast("与绑定手机号不一致，请重新输入！")
		return
	end	
	
	ModifyPassData:pushstring(GlobalUserItem.szAccount, yl.LEN_ACCOUNTS)
	ModifyPassData:pushstring(md5(self.m_editNewPass:getText()), yl.LEN_PASSWORD)
	ModifyPassData:pushbyte(0)
	ModifyPassData:pushstring(self.m_editcode:getText(),yl.LEN_VERIFY_CODE)
	ModifyPassData:pushstring(phoneNumber,yl.LEN_MOBILE_PHONE)
	ModifyPassData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)		
		
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ModifyPassData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
				--清空原来的列表
				local code = Datatable.data:readbyte()
				self:showToast(Datatable.data:readstring(64))
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_SUCCESS) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				GlobalUserItem.szRegisterMobile = phoneNumber
				self.m_editPhone:setEnabled(false)
				self:showToast(szDescribe)
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.MODIFY_BANK_PASS_LAYER})
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_FAILURE) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				self:showToast(szDescribe)
			end
		end
	end)	

end

function ModifyBankPasswdLayer:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end

return ModifyBankPasswdLayer