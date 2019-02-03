local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local RegisterView = class("RegisterView",function()
		local registerView =  display.newLayer()
    return registerView
end)

RegisterView.BT_REGISTER = "btnRegister"
RegisterView.BT_RETURN	 = "btnReturn"
RegisterView.BT_AGREEMENT= "btnAgreement"
RegisterView.CBT_AGREEMENT = "cbtAgreement"
RegisterView.BT_GET_VERIFY_CODE = "btnVerifyCode"

RegisterView.bAgreement = true
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function RegisterView:ctor()

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Register/RigisterView.csb", self)
	self._csbNode = csbNode

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref)
        end
    end
	
	local btnReturn = csbNode:getChildByName(RegisterView.BT_RETURN)
	btnReturn:addTouchEventListener(btcallback)
	
	local sp_public_frame = csbNode:getChildByName("sp_public_frame")

	--账号输入
	local contentSize = sp_public_frame:getChildByName("text_field_regist_1"):getContentSize()
	self.edit_Account = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Register/text_field_regist.png"))
		:move(contentSize.width / 2,contentSize.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(31)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-31位字符")
		:addTo(sp_public_frame:getChildByName("text_field_regist_1"))

	--密码输入	
	self.edit_Password = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Register/text_field_regist.png"))
		:move(contentSize.width / 2,contentSize.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:addTo(sp_public_frame:getChildByName("text_field_regist_2"))
		
		
	--验证码输入
	self.edit_VerifyCode = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Register/text_field_regist.png"))
		:move(contentSize.width / 2,contentSize.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(8)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("1-8位字符")
		:addTo(sp_public_frame:getChildByName("text_field_regist_3"))

--[[	--确认密码输入	
	self.edit_RePassword = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
		:move(730,358)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:addTo(self)--]]

	--推广员	
	self.edit_Spreader = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Register/text_field_regist.png"))
		:move(contentSize.width / 2,contentSize.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)--:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入推广员ID")
		:addTo(sp_public_frame:getChildByName("text_field_regist_4"))

	--条款协议
	self.cbt_Agreement = sp_public_frame:getChildByName(RegisterView.CBT_AGREEMENT)
		:setSelected(RegisterView.bAgreement)

	--显示协议
	sp_public_frame:getChildByName(RegisterView.BT_AGREEMENT):addTouchEventListener(btcallback)

	--注册按钮
	sp_public_frame:getChildByName(RegisterView.BT_REGISTER):addTouchEventListener(btcallback)
	
	--获取验证码
	self._btn_verifyCode = sp_public_frame:getChildByName(RegisterView.BT_GET_VERIFY_CODE)
	self._btn_verifyCode:addTouchEventListener(btcallback)
end


function RegisterView:onButtonClickedEvent(ref)
	ExternalFun.playClickEffect()
	if ref:getName() == RegisterView.BT_RETURN then
		self:getParent():getParent():onShowLogon()
	elseif ref:getName() == RegisterView.BT_AGREEMENT then
		--暂时屏蔽协议按钮
		--self:getParent():getParent():onShowService()
	elseif ref:getName() == RegisterView.BT_REGISTER then
		-- 判断 非 数字、字母、下划线、中文 的帐号
		local szAccount = self.edit_Account:getText()
		local filter = string.find(szAccount, "^[a-zA-Z0-9_\128-\254]+$")
		if nil == filter then
			showToast(self, "帐号包含非法字符, 请重试!", 1)
			return
		end
		szAccount = string.gsub(szAccount, " ", "")
		local szPassword = string.gsub(self.edit_Password:getText(), " ", "")
		local szVerifyCode = string.gsub(self.edit_VerifyCode:getText(), " ", "")
		local bAgreement = self.cbt_Agreement:isSelected()
		local szSpreader = string.gsub(self.edit_Spreader:getText(), " ", "")
		self:getParent():getParent():onRegister(szAccount,szPassword,szVerifyCode,bAgreement,szSpreader)
	elseif ref:getName() == RegisterView.BT_GET_VERIFY_CODE then
		local RegisterData = ExternalFun.create_netdata(logincmd.CMD_MB_GetRegisterVerifyCode)
		RegisterData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_PHONE_GET_REGISTER_VERIFYCODE)

		local szAccount = self.edit_Account:getText()

		szAccount = string.gsub(szAccount, " ", "")		
		
		if false == (appdf.checkPhone(szAccount)) then
			showToast(self, "请输入合法的手机号码!", 1)
			return
		end
		
		RegisterData:pushstring(szAccount,yl.LEN_MOBILE_PHONE)
		RegisterData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)		
			
		appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, RegisterData, function (Datatable, Responce)
			if (Responce.code == 0) then
				if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
					--清空原来的列表
					local code = Datatable.data:readbyte()
					if (code == 0) then
						local maxCount = 60
						local timeCount = 60
						self._btn_verifyCode:runAction(cc.RepeatForever:create(cc.Sequence:create(
						cc.DelayTime:create(1),
						cc.CallFunc:create(function()
							if (timeCount <= 0) then
								self._btn_verifyCode:setTitleText("获取验证码")
								self._btn_verifyCode:setEnabled(true)
								self._btn_verifyCode:stopAllActions()
							elseif (timeCount == maxCount) then
								self._btn_verifyCode:setEnabled(false)
								self._btn_verifyCode:setTitleText(string.format("发送中%d秒", timeCount))
								timeCount = timeCount - 1
							else
								self._btn_verifyCode:setTitleText(string.format("发送中%d秒", timeCount))
								timeCount = timeCount - 1
							end
						end)
						)))
					end
					showToast(self, Datatable.data:readstring(64), 1)
				end
			end
		end)
	end
end

function RegisterView:setAgreement(bAgree)
	self.cbt_Agreement:setSelected(bAgree)
end

return RegisterView