--[[
用户信息界面
2017_08_24 MXM
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local PromoterInputLayer = import(".PromoterInputLayer")
local SelectHeadLayer = import(".SelectHeadLayer")
--local BingExchange = import(".BingExchange")
local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")

---------------------------------------------------------------------------
--个人信息界面
local UserInfoLayer = class("UserInfoLayer", cc.BaseLayer)

UserInfoLayer.BT_MODIFY_INFO 	= 1
UserInfoLayer.BT_BANK			= 2
UserInfoLayer.BT_MODIFY_PASS	= 3
UserInfoLayer.BT_ADD			= 4
UserInfoLayer.BT_EXIT			= 5

UserInfoLayer.BT_BINDING 		= 6
UserInfoLayer.BT_VIP 			= 7
UserInfoLayer.BT_NICKEDIT 		= 8
UserInfoLayer.BT_SIGNEDIT 		= 9
UserInfoLayer.BT_TAKE 			= 10
UserInfoLayer.BT_RECHARGE 		= 11
UserInfoLayer.BT_EXCHANGE 		= 12
UserInfoLayer.BT_AGENT 			= 16
UserInfoLayer.BT_CONFIRM		= 15
UserInfoLayer.LAY_SELHEAD		= 17
UserInfoLayer.BT_BAG			= 18
UserInfoLayer.BT_QRCODE 		= 19
UserInfoLayer.BT_PROMOTER 		= 20
UserInfoLayer.CBT_PLAYER			= 21
UserInfoLayer.CBT_FACESET			= 22
UserInfoLayer.CBT_EXCHAGE			= 23
UserInfoLayer.CBT_SAFE		        = 24

function UserInfoLayer:ctor(pageIndex)
	self.super.ctor(self)
	
    -- 加载csb资源
	local csbPath = "Userinfo/UserInfoScene.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	self.m_getCodeTime = 0
	self._defaultPageIndex = pageIndex or 1
	
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
	end)
	
	self:attach("M2V_GetCodeSuccess", function (e)
		local maxCount = 60
		self.m_getCodeTime = 60
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
	end)
	
	self:attach("M2V_GetCodeFailed", function (e, msg)
		self._btncode:setEnabled(true)
		showToast(self, msg, 2)
	end)
	
	self:attach("M2V_BindPhoneResult", function (e, code)
		if (code == 0) then
			self._btnBindPhone:setEnabled(false)
			self.m_editPhone:setEnabled(false)
			self._btncode:setEnabled(false)
			
			--更新视图状态 并切换
			--手机号输入
			local tips = self._areadyBind:getChildByName("tips_0")
			--处理已经绑定过的手机号 * 号显示
			if (GlobalUserItem.szRegisterMobile ~= "") then
				local dealPhone = string.gsub(GlobalUserItem.szRegisterMobile, "(.?)(.?)(.?)(.*)", function (cat1, cat2, cat3, cat5)
					cat5 = string.reverse(cat5)
					cat5 = string.gsub(cat5, "(.?)(.?)(.?)(.*)", function (scat1, scat2, scat3, scat5)
						local scat5 = string.gsub(scat5, ".", "*")
						local lastFix = scat1 .. scat2 .. scat3 .. scat5
						return string.reverse(lastFix)
					end)
					local preFix = cat1 .. cat2 .. cat3 .. cat5
					return preFix
				end)
				
				tips:setString("您已经绑定了手机号:" .. dealPhone)
			end				
			self._csbNodeAni:play("bindPhoneToAready", false)
		else
			self._btncode:setEnabled(true)
			self._btnBindPhone:setEnabled(true)
		end
	end)	
end

function UserInfoLayer:initUI()
	--玩家信息页
	local bg = self._csbNode:getChildByName("bg")
	self._playerinfoLeft = bg:getChildByName("UserInfoLeft")
	self._playerinfoRight = bg:getChildByName("UserInfoRight"):getChildByName("right_bg")
	
	--男女单选按钮
    self._radioSexButtonGroup = ccui.RadioButtonGroup:create()
    self._radioSexButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioSexButtonGroup)
	--男
	local rto_boy_container_1 = self._playerinfoRight:getChildByName("rto_boy_container_1")
	local rto_boy_container_1_size = rto_boy_container_1:getContentSize()
	self.m_rtoBoy = ccui.RadioButton:create("btnUnsel.png","btnUnsel.png","btnSel.png","btnUnsel.png","btnUnsel.png", ccui.TextureResType.plistType)
	self.m_rtoBoy:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBoy:setPosition(rto_boy_container_1_size.width / 2, rto_boy_container_1_size.height / 2)
	self.m_rtoBoy:setName("rto_boy")
	rto_boy_container_1:addChild(self.m_rtoBoy)
	self._radioSexButtonGroup:addRadioButton(self.m_rtoBoy)
		
	--女
	local rto_girl_container_2 = self._playerinfoRight:getChildByName("rto_girl_container_2")
	local rto_girl_container_2_size = rto_girl_container_2:getContentSize()
	self.m_rtoGirl = ccui.RadioButton:create("btnUnsel.png","btnUnsel.png","btnSel.png","btnUnsel.png","btnUnsel.png", ccui.TextureResType.plistType)
	self.m_rtoGirl:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoGirl:setPosition(rto_girl_container_2_size.width / 2, rto_girl_container_2_size.height / 2)
	self.m_rtoGirl:setName("rto_girl")
	rto_girl_container_2:addChild(self.m_rtoGirl)
	self._radioSexButtonGroup:addRadioButton(self.m_rtoGirl)
	
	--修改账号密码
	self.m_btnModifyAccountPass = self._playerinfoRight:getChildByName("btn_modify_pass")
	self.m_btnModifyAccountPass:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--修改银行密码
	self.m_btnModifyBanktPass = self._playerinfoRight:getChildByName("btn_modify_bank")
	self.m_btnModifyBanktPass:addClickEventListener(handler(self, self.onButtonClickedEvent))	

	--左侧单选按钮组
    self._radioTopButtonGroup = ccui.RadioButtonGroup:create()
    self._radioTopButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioTopButtonGroup)	
	
	--返回
	local info_top_bg = bg:getChildByName("info_top_bg")
	self.m_btnReturn = info_top_bg:getChildByName("btn_return")
	self.m_btnReturn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	

	
	--个人信息
	local rto_person_container_1 = info_top_bg:getChildByName("rto_person_container_1")
	local rto_container_1_size = rto_person_container_1:getContentSize()
	self.m_rtoPsersonInfo = ccui.RadioButton:create("btnPerson0.png","btnPerson0.png","btnPerson1.png","btnPerson0.png","btnPerson0.png", ccui.TextureResType.plistType)
	self.m_rtoPsersonInfo:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoPsersonInfo:setPosition(rto_container_1_size.width / 2, rto_container_1_size.height / 2)
	self.m_rtoPsersonInfo:setName("rto_person_info")
	rto_person_container_1:addChild(self.m_rtoPsersonInfo)
	self._radioTopButtonGroup:addRadioButton(self.m_rtoPsersonInfo)
		
	--绑定手机
	local rto_bind_container_2 = info_top_bg:getChildByName("rto_bind_container_2")
	local rto_container_2_size = rto_bind_container_2:getContentSize()
	self.m_rtoBindPhone = ccui.RadioButton:create("btnBindPhone0.png","btnBindPhone0.png","btnBindPhone1.png","btnBindPhone0.png","btnBindPhone0.png", ccui.TextureResType.plistType)
	self.m_rtoBindPhone:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBindPhone:setPosition(rto_container_2_size.width / 2, rto_container_2_size.height / 2)
	self.m_rtoBindPhone:setName("rto_bind_phone")
	rto_bind_container_2:addChild(self.m_rtoBindPhone)
	self._radioTopButtonGroup:addRadioButton(self.m_rtoBindPhone)
	
	-----玩家头像------------
	self.m_btnHead = self._playerinfoLeft:getChildByName("btn_head")
	local btnHeadBackMask = self._playerinfoLeft:getChildByName("head_back_mask")
	local btnHeadBackMaskSize = btnHeadBackMask:getContentSize()
	self._sphead = HeadSprite:createClipHead(GlobalUserItem, btnHeadBackMaskSize.width)
	self._sphead:setPosition(cc.p(btnHeadBackMaskSize.width / 2, btnHeadBackMaskSize.height / 2))
	self._sphead:enableHeadFrame(true)
	self:attach("M2V_Update_UserFace", function (e, obj)
		self._sphead:updateHead(obj)
	end)
	btnHeadBackMask:addChild(self._sphead)
	self.m_btnHead:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--ID
	self.m_id = self._playerinfoLeft:getChildByName("text_gameid")
	self.m_id:setString("ID:" .. GlobalUserItem.dwGameID.."")
	
	--金币
	self.m_userGold = self._playerinfoLeft:getChildByName("atlas_coin")
	
	local str = string.formatNumberTThousands(GlobalUserItem.lUserScore)
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	if (GlobalUserItem.lUserScore < 10000) then
		self.m_userGold:setString(str)
	elseif (GlobalUserItem.lUserScore >= 10000 and GlobalUserItem.lUserScore < 100000000) then
		self.m_userGold:setString(str .. "万")
	elseif (GlobalUserItem.lUserScore >= 100000000) then
		self.m_userGold:setString(str .. "亿")
	end

	--注册UI监听事件
	self:attach("M2V_Update_UserScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		if (score < 10000) then
			self.m_userGold:setString(str)
		elseif (score >= 10000 and score < 100000000) then
			self.m_userGold:setString(str .. "万")
		elseif (score >= 100000000) then
			self.m_userGold:setString(str .. "亿")
		end
	end)
	
	--金币+按钮
	self.m_btnAddGold = self._playerinfoLeft:getChildByName("btn_addcz")
	self.m_btnAddGold:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--银行金币
	self.m_bankGold = self._playerinfoLeft:getChildByName("atlas_bankscore")
	
    str = string.formatNumberTThousands(GlobalUserItem.lUserInsure)  --MXM更新大厅银行金币
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	
	if (GlobalUserItem.lUserInsure < 10000) then
		self.m_bankGold:setString(str)
	elseif (GlobalUserItem.lUserInsure >= 10000 and GlobalUserItem.lUserInsure < 100000000) then
		self.m_bankGold:setString(str .. "万")
	elseif (GlobalUserItem.lUserInsure >= 100000000) then
		self.m_bankGold:setString(str .. "亿")
	end	
	
	--注册UI监听事件
	self:attach("M2V_Update_InsureScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		if (score < 10000) then
			self.m_bankGold:setString(str)
		elseif (score >= 10000 and score < 100000000) then
			self.m_bankGold:setString(str .. "万")
		elseif (score >= 100000000) then
			self.m_bankGold:setString(str .. "亿")
		end	
	end)
	
	--银行+按钮
	self.m_btnAddInsure = self._playerinfoLeft:getChildByName("btn_insure")
	self.m_btnAddInsure:addClickEventListener(handler(self, self.onButtonClickedEvent))	
	
	--账号
	self.m_account = self._playerinfoRight:getChildByName("text_account")
	self.m_account:setString(GlobalUserItem.szAccount.."")
		
	--性别选择
	if (GlobalUserItem.cbGender == yl.GENDER_MANKIND) then
		self._radioSexButtonGroup:setSelectedButton(self.m_rtoBoy)
	else
		self._radioSexButtonGroup:setSelectedButton(self.m_rtoGirl)
	end
	
	--IP
	self.m_ip = self._playerinfoRight:getChildByName("text_ip")
	self.m_ip:setString(GlobalUserItem.szIpAdress.."")
	
	--归属地
	self.m_iplocation = self._playerinfoRight:getChildByName("text_location")
	self.m_iplocation:setString(GlobalUserItem.szIpLocation)

	--注册时间
	self.m_regtime = self._playerinfoRight:getChildByName("text_reg_time")
	local RegisterData = GlobalUserItem.RegisterData
	self.m_regtime:setString(string.format("%d.%d.%d", RegisterData.wYear, RegisterData.wMonth, RegisterData.wDay))
	
	--初始化绑定界面
	self:initBindPhoneUI()
	self:initAreadyBindUI()
end

function UserInfoLayer:initBindPhoneUI()
	--玩家信息页
	local bg = self._csbNode:getChildByName("bg")
	self._playerinfoBind = bg:getChildByName("BindPhone")
	
	--手机号输入
	local phone_bg = self._playerinfoBind:getChildByName("text_field_bg_1")
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
		:setEnabled(GlobalUserItem.szRegisterMobile == "")
		
	--处理已经绑定过的手机号 * 号显示
	local dealPhone = string.gsub(GlobalUserItem.szRegisterMobile, "(.?)(.?)(.?)(.*)", function (cat1, cat2, cat3, cat5)
		cat5 = string.reverse(cat5)
		cat5 = string.gsub(cat5, "(.?)(.?)(.?)(.*)", function (scat1, scat2, scat3, scat5)
			local scat5 = string.gsub(scat5, ".", "*")
			local lastFix = scat1 .. scat2 .. scat3 .. scat5
			return string.reverse(lastFix)
		end)
		local preFix = cat1 .. cat2 .. cat3 .. cat5
		return preFix
	end)	

	self.m_editPhone:setText(dealPhone)
		
		
	--验证码
	local verify_bg = self._playerinfoBind:getChildByName("text_field_bg_2")
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
		
	self._btncode = self._playerinfoBind:getChildByName("btn_get_verify_code")
	self._btncode:setEnabled(GlobalUserItem.szRegisterMobile == "")
	self._btncode:addClickEventListener(handler(self, self.onButtonClickedEvent))	

	--确定绑定
	self._btnBindPhone = self._playerinfoBind:getChildByName("btn_bind_phone")
	self._btnBindPhone:addClickEventListener(handler(self, self.onButtonClickedEvent))
	if (GlobalUserItem.szRegisterMobile ~= "") then
		self._btnBindPhone:setEnabled(false)
	end

	--账号密码	
	local pass_bg = self._playerinfoBind:getChildByName("text_field_bg_3")
	local text_field_3 = pass_bg:getChildByName("text_field_3")	

	text_field_size = text_field_3:getContentSize()

	self.m_editPass = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
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
		
	
	if (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_WECHAT) then
		pass_bg:setOpacity(64)
		self.m_editPass:setEnabled(false)
		self.m_editPass:setPlaceHolder("微信登录，无法修改密码...")
	elseif (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_ACCOUNTS or 
			GlobalUserItem.cbLogonMode == yl.LOGON_MODE_REGISTER_ACCOUNTS or 
			GlobalUserItem.cbLogonMode == yl.LOGON_MODE_REGISTER_PHONE) then
		pass_bg:setOpacity(64)
		self.m_editPass:setEnabled(false)
		self.m_editPass:setText(GlobalUserItem.szPassword)
	elseif (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_VISITORS) then
		self.m_editPass:setEnabled(true) --只有游客登录 允许修改密码
	end
		
	--密码输入	
	local repass_bg = self._playerinfoBind:getChildByName("text_field_bg_4")
	local text_field_4 = repass_bg:getChildByName("text_field_4")	
	
	text_field_size = text_field_4:getContentSize()
	
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
		:setPlaceHolder("请确认账号密码...")
		:addTo(text_field_4)

	if (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_WECHAT) then
		repass_bg:setOpacity(64)
		self.m_editNewPass:setEnabled(false)
		self.m_editNewPass:setPlaceHolder("微信登录，无法修改密码...")
	elseif (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_ACCOUNTS or 
			GlobalUserItem.cbLogonMode == yl.LOGON_MODE_REGISTER_ACCOUNTS or 
			GlobalUserItem.cbLogonMode == yl.LOGON_MODE_REGISTER_PHONE) then
		repass_bg:setOpacity(64)
		self.m_editNewPass:setEnabled(false)
		self.m_editNewPass:setText(GlobalUserItem.szPassword)
	elseif (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_VISITORS) then
		self.m_editNewPass:setEnabled(true) --只有游客登录 允许修改密码
	end

		
	--推广员	
	local spreader_bg = self._playerinfoBind:getChildByName("text_field_bg_5")
	local text_field_5 = spreader_bg:getChildByName("text_field_5")	
	text_field_size = text_field_5:getContentSize()
	self.m_editSpreader = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入推广员ID...")
		:addTo(text_field_5)	

	self._btncode = self._playerinfoBind:getChildByName("btn_get_verify_code")
	self._btncode:addClickEventListener(handler(self, self.onButtonClickedEvent))
end

function UserInfoLayer:initAreadyBindUI()
	--玩家信息页
	local bg = self._csbNode:getChildByName("bg")
	self._areadyBind = bg:getChildByName("AreadyBind")
	
	--手机号输入
	local tips = self._areadyBind:getChildByName("tips_0")
	--处理已经绑定过的手机号 * 号显示
	if (GlobalUserItem.szRegisterMobile ~= "") then
		local dealPhone = string.gsub(GlobalUserItem.szRegisterMobile, "(.?)(.?)(.?)(.*)", function (cat1, cat2, cat3, cat5)
			cat5 = string.reverse(cat5)
			cat5 = string.gsub(cat5, "(.?)(.?)(.?)(.*)", function (scat1, scat2, scat3, scat5)
				local scat5 = string.gsub(scat5, ".", "*")
				local lastFix = scat1 .. scat2 .. scat3 .. scat5
				return string.reverse(lastFix)
			end)
			local preFix = cat1 .. cat2 .. cat3 .. cat5
			return preFix
		end)
		
		tips:setString("您已经绑定了手机号:" .. dealPhone)
	end
end

function UserInfoLayer:onViewLoad()
	--异步加载修改缓存图
	appdf.loadImage("client/res/modify/modify.plist","client/res/modify/modify.png", function ()
		printf("modify load finished\n")
	end)
	
	self:loadFinished()
end

function UserInfoLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("enterView", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("showBindPhone", function ()
		self:enterViewFinished()
	end)	
	
	self._csbNodeAni:setAnimationEndCallFunc("showAreadyBind", function ()
		self:enterViewFinished()
	end)		
	
	--跳转界面
	if (self._defaultPageIndex == 1) then
		self._radioTopButtonGroup:setSelectedButton(self.m_rtoPsersonInfo)
		self._csbNodeAni:play("enterView", false)
	elseif (self._defaultPageIndex == 2) then
		if (GlobalUserItem.szRegisterMobile == "") then
			self._radioTopButtonGroup:setSelectedButton(self.m_rtoBindPhone)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/first_bind.mp3")
			self._csbNodeAni:play("showBindPhone", false)
		else
			self._radioTopButtonGroup:setSelectedButton(self.m_rtoBindPhone)
			self._csbNodeAni:play("showAreadyBind", false)
		end
	end
end

function UserInfoLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("exitViewUserInfo", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("exitViewBindPhone", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("exitAreadyBind", function ()
		self:exitViewFinished()
	end)		
	
	if (self.m_rtoPsersonInfo:isSelected()) then
		self._csbNodeAni:play("exitViewUserInfo", false)
	elseif (self.m_rtoBindPhone:isSelected()) then
		if (GlobalUserItem.szRegisterMobile == "") then
			self._csbNodeAni:play("exitViewBindPhone", false)
		else
			self._csbNodeAni:play("exitAreadyBind", false)
		end		
	else
		assert(false, "what page yet?")
	end
end

function UserInfoLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	if (senderName == "btn_return") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.PERSON_LAYER})
	elseif (senderName == "btn_head") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SELECT_SYSTEM_HEAD_LAYER)
	elseif (senderName == "btn_modify_pass") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.MODIFY_ACCOUNT_PASS_LAYER)
	elseif (senderName == "btn_modify_bank") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.MODIFY_BANK_PASS_LAYER)
	elseif (senderName == "btn_addcz") then
		local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
		if (appConfigProxy._appStoreSwitch == 0) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
		end		
	elseif (senderName == "btn_insure") then
		if 0 == GlobalUserItem.cbInsureEnabled then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_OPEN_LAYER)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_LAYER)
		end
	elseif (senderName == "btn_get_verify_code") then
		-- phonenumber, verifycode, newpass)
		self._btncode:setEnabled(false)
		local phonenumber = self.m_editPhone:getText()
		self:emit("V2M_GetVerifyCode", phonenumber)
	elseif (senderName == "btn_bind_phone") then
		-- phonenumber, verifycode, newpass)
		local phonenumber = self.m_editPhone:getText()
		local verifycode = self.m_editcode:getText()
		local pass = self.m_editPass:getText()
		local newpass = self.m_editNewPass:getText()
		self:emit("V2M_BindPhone", phonenumber, verifycode, pass, newpass)
	end
--[[	if tag == UserInfoLayer.CBT_PLAYER then
		
		self._UserInfoLayout:setVisible(true)
		self.m_playerinfo:setVisible(true)
		self.playerinfoBtn:setEnabled(false)
		self._head:setVisible(true)
		
		self._selecthead:setVisible(false)
		self.m_faceset:setVisible(false)
		self.facesetBtn:setEnabled(true)
		
		self.m_exchage:setVisible(false)
		if (nil ~= self.m_bingExchange) then
			self.m_bingExchange:removeFromParent()
			self.m_bingExchange = nil
		end
		self.exchageBtn:setEnabled(true)
		
		self.m_safe:setVisible(false)
		self.safeBtn:setEnabled(true)
	end
	
	if tag == UserInfoLayer.CBT_FACESET then
		
		self._UserInfoLayout:setVisible(false)
		self.m_playerinfo:setVisible(false)
		self.playerinfoBtn:setEnabled(true)
		self._head:setVisible(false)
		
		self._selecthead:setVisible(true)
		self.m_faceset:setVisible(true)
		self.facesetBtn:setEnabled(false)
		
		self.m_exchage:setVisible(false)
		if (nil ~= self.m_bingExchange) then
			self.m_bingExchange:removeFromParent()
			self.m_bingExchange = nil
		end
		self.exchageBtn:setEnabled(true)
		
		self.m_safe:setVisible(false)
		self.safeBtn:setEnabled(true)
	end
	
	if tag == UserInfoLayer.CBT_EXCHAGE then
		
		self._UserInfoLayout:setVisible(false)
		self.m_playerinfo:setVisible(false)
		self.playerinfoBtn:setEnabled(true)
		self._head:setVisible(false)
		
		self._selecthead:setVisible(false)
		self.m_faceset:setVisible(false)
		self.facesetBtn:setEnabled(true)
		
		--WebView 有隐藏的bug，父节点隐藏后，其WebView作为子节点 似乎在某些设备上无法隐藏
		self.m_bingExchange = BingExchange:create(cc.size(950, 550))
		self.m_bingExchange:setPosition(785, 330)
		self.m_exchage:addChild(self.m_bingExchange)
		
		self.m_bingExchange = BingExchange:create(self)
		self.m_exchage:addChild(self.m_bingExchange)
	
		self.m_exchage:setVisible(true)
		self.exchageBtn:setEnabled(false)
		
		self.m_safe:setVisible(false)
		self.safeBtn:setEnabled(true)
	end
	
	if tag == UserInfoLayer.CBT_SAFE then
		
		self._UserInfoLayout:setVisible(false)
		self.m_playerinfo:setVisible(false)
		self.playerinfoBtn:setEnabled(true)
		self._head:setVisible(false)
		
		self._selecthead:setVisible(false)
		self.m_faceset:setVisible(false)
		self.facesetBtn:setEnabled(true)
		
		self.m_exchage:setVisible(false)
		if (nil ~= self.m_bingExchange) then
			self.m_bingExchange:removeFromParent()
			self.m_bingExchange = nil
			
		end
		self.exchageBtn:setEnabled(true)
		
		self.m_safe:setVisible(true)
		self.safeBtn:setEnabled(false)
	end
	
	if tag == UserInfoLayer.BT_EXIT then
		self._scene:onKeyBack()
	elseif tag == UserInfoLayer.BT_TAKE then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		self._scene:onChangeShowMode(yl.SCENE_BANK)
	elseif tag == UserInfoLayer.BT_AGENT then
		self._scene:onChangeShowMode(yl.SCENE_AGENT)
	elseif tag == UserInfoLayer.BT_BINDING then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		self:getRootNode():onChangeShowMode(yl.SCENE_BINDING)
	elseif tag == UserInfoLayer.BT_CONFIRM then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		
		local szNickname = string.gsub(self.edit_Nickname:getText(), " ", "")
		
		--判断昵称长度
		if ExternalFun.stringLen(szNickname) < 6 then
			showToast(self, "游戏昵称必须大于6位以上,请重新输入!", 2)
			return
		end
		
		--判断是否有非法字符
		if true == ExternalFun.isContainBadWords(szNickname) then
			showToast(self, "昵称中包含敏感字符,请重试", 2)
			return
		end
		
		local szSign = string.gsub(self.edit_Sign:getText(), " ", "")
		
		--判断是否有非法字符
		if true == ExternalFun.isContainBadWords(szSign) then
			showToast(self, "个性签名中包含敏感字符,请重试", 2)
			return
		end
		
		if szNickname ~= GlobalUserItem.szNickName or szSign ~= GlobalUserItem.szSign then
			self:showPopWait()
			self._modifyFrame:onModifyUserInfo(GlobalUserItem.cbGender,szNickname,szSign)
		end
	elseif tag == UserInfoLayer.BT_VIP then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		
		self:getRootNode():onChangeShowMode(yl.SCENE_SHOP, ShopLayer.CBT_VIP)
	elseif tag == UserInfoLayer.BT_RECHARGE then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		
		self._scene:onChangeShowMode(yl.SCENE_SHOP, ShopLayer.CBT_BEAN)
	elseif tag == UserInfoLayer.BT_EXCHANGE then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		
		self._scene:onChangeShowMode(yl.SCENE_SHOP, ShopLayer.CBT_ENTITY)
	elseif tag == UserInfoLayer.BT_BAG then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		
		self._scene:onChangeShowMode(yl.SCENE_BAG)
	elseif tag == UserInfoLayer.BT_QRCODE then
		--local qrlayer = QrCodeLayer:create(self)
		--self:addChild(qrlayer)
		local prolayer = PromoterInputLayer:create(self)
		self:addChild(prolayer)
	elseif tag == UserInfoLayer.BT_PROMOTER then
		--local prolayer = PromoterInputLayer:create(self)
		--self:addChild(prolayer)
	end--]]
end

function UserInfoLayer:onClickUserHead( )
	if GlobalUserItem.isAngentAccount() then
		return
	end
	if GlobalUserItem.notEditAble() then
		return
	end
	
	local tmp = SelectHeadLayer:create(self)
	tmp:setTag(UserInfoLayer.LAY_SELHEAD)
	self:addChild(tmp)
end

function UserInfoLayer:onSelectedEvent(sender,eventType)
	if (ccui.RadioButtonEventType.selected == eventType) then
		ExternalFun.playClickEffect()
		
		if GlobalUserItem.isAngentAccount() then
			sender:setSelected(not sender:isSelected())
			return
		end
		if GlobalUserItem.notEditAble() then
			sender:setSelected(not sender:isSelected())
			return
		end

		if sender:getName() == "rto_person_info" then
			if (GlobalUserItem.szRegisterMobile == "") then
				self._csbNodeAni:play("showUserInfo", false)
			else
				self._csbNodeAni:play("areadyToUserInfo", false)
			end
		elseif sender:getName() == "rto_bind_phone" then
			if (GlobalUserItem.szRegisterMobile == "") then
				--self._radioTopButtonGroup:setSelectedButton(self.m_rtoBindPhone)
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/first_bind.mp3")
				self._csbNodeAni:play("showBindPhone", false)
			else
				--self._radioTopButtonGroup:setSelectedButton(self.m_rtoBindPhone)
				self._csbNodeAni:play("showAreadyBind", false)
			end
		elseif (sender:getName() == "rto_boy") then
			--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.POPWAIT_LAYER)
			self:emit("V2M_ModifySex", yl.GENDER_MANKIND, function (code, msg)
				--AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
				--showToast(self, msg, 1)
				if (code == 0) then
					--更新头像
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_FACE)
				else
					--self._radioSexButtonGroup:setSelectedButtonWithoutEvent(1)
					showToast(self, msg, 1)
				end
			end)
		elseif (sender:getName() == "rto_girl") then
			--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.POPWAIT_LAYER)
			self:emit("V2M_ModifySex", yl.GENDER_FEMALE, function (code, msg)
				--AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
				--showToast(self, msg, 1)
				if (code == 0) then
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_FACE)
				else
					--self._radioSexButtonGroup:setSelectedButtonWithoutEvent(0)
					showToast(self, msg, 1)
				end
			end)
		end
	end
end

function UserInfoLayer:onEditEvent(name, editbox)
	if "return" == name then
		if "edit_sign" == editbox:getName() then
			if GlobalUserItem.isAngentAccount() then
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			if GlobalUserItem.notEditAble() then
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			
			local szSign = string.gsub(self.edit_Sign:getText(), " ", "")
			--判断长度
			if ExternalFun.stringLen(szSign) < 1 then
				showToast(self, "个性签名不能为空", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			
			--判断emoji
			if ExternalFun.isContainEmoji(szSign) then
				showToast(self, "个性签名中包含非法字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			
			--判断是否有非法字符
			if true == ExternalFun.isContainBadWords(szSign) then
				showToast(self, "个性签名中包含敏感字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			self.m_szSign = szSign
			if szSign == GlobalUserItem.szSign then
				return
			end
		elseif "edit_nickname" == editbox:getName() then
			if GlobalUserItem.isAngentAccount() then
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end
			
			if GlobalUserItem.notEditAble() then
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end
			
			local szNickname = string.gsub(self.edit_Nickname:getText(), " ", "")
			--判断长度
			if ExternalFun.stringLen(szNickname) < 6 then
				showToast(self, "游戏昵称必须大于6位以上,请重新输入!", 2)
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end
			
			--判断emoji
			if ExternalFun.isContainEmoji(szNickname) then
				showToast(self, "昵称中包含非法字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			
			--判断是否有非法字符
			if true == ExternalFun.isContainBadWords(szNickname) then
				showToast(self, "昵称中包含敏感字符,请重试", 2)
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end
			self.m_szNick = szNickname
			if szNickname == GlobalUserItem.szNickName then
				return
			end
		end
		
		if self.m_szNick == "" or self.m_szSign == "" then
			return
		end
		self:showPopWait()
		self._modifyFrame:onModifyUserInfo(GlobalUserItem.cbGender,self.m_szNick,self.m_szSign)
	end
end

function UserInfoLayer:onExit( )
--[[	if nil ~= self.edit_Sign then
		self.edit_Sign:unregisterScriptEditBoxHandler()
	end
	
	if nil ~= self.edit_Nickname then
		self.edit_Nickname:unregisterScriptEditBoxHandler()
	end
	
	if self._modifyFrame:isSocketServer() then
		self._modifyFrame:onCloseSocket()
	end--]]
end

function UserInfoLayer:onEnterTransitionFinish()
	--self:showPopWait()
	--self._modifyFrame:onQueryUserInfo()
end

function UserInfoLayer:sendModifySystemFace( wFaceId )
	self:showPopWait()
	self._modifyFrame:onModifySystemHead(wFaceId)
end

--操作结果
function UserInfoLayer:onModifyCallBack(result,message)
	self:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2);
	end
	if -1 == result then
		return
	end
	
	bGender = (GlobalUserItem.cbGender == yl.GENDER_MANKIND and true or false)
	self._cbtMan:setSelected(bGender)
	self._cbtWoman:setSelected(not bGender)
	
	if yl.SUB_GP_USER_FACE_INFO == result then
		self:onFaceResultSuccess()
		self:removeChildByTag(UserInfoLayer.LAY_SELHEAD, true)
	elseif loginCMD.SUB_GP_USER_INDIVIDUAL == result then
		-- 推广员按钮
		--local noSpreader = GlobalUserItem.szSpreaderAccount == ""
		--self.m_btnPromoter:setVisible(noSpreader)
		--self.m_btnPromoter:setEnabled(noSpreader)
	elseif self._modifyFrame.INPUT_SPREADER == result then
		--local noSpreader = GlobalUserItem.szSpreaderAccount == ""
		--self.m_btnPromoter:setVisible(noSpreader)
		--self.m_btnPromoter:setEnabled(noSpreader)
	end
end

function UserInfoLayer:onKeyBack(  )
	if nil ~= self._scene._popWait then
		showToast(self, "当前操作不可返回", 2)
	end
	return true
end

--显示等待
function UserInfoLayer:showPopWait()
	self._scene:showPopWait()
end

--关闭等待
function UserInfoLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

--头像更改结果
function UserInfoLayer:onFaceResultSuccess()
	--更新头像
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_FACE)
end

return UserInfoLayer