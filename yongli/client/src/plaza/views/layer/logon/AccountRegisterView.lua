local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local AccountRegisterView = class("AccountRegisterView",cc.BaseLayer)

AccountRegisterView.BT_REGISTER = "btnRegister"
AccountRegisterView.BT_RETURN	 = "btnReturn"
AccountRegisterView.BT_AGREEMENT= "btnAgreement"
AccountRegisterView.CBT_AGREEMENT = "cbtAgreement"

AccountRegisterView.bAgreement = true
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function AccountRegisterView:ctor()
	self.super.ctor(self)

    -- 加载csb资源
	local csbPath = "Register/AccountRigisterView.csb"
	
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)		
    self._csbNode = csbNode
	

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref)
        end
    end
	
	local btnReturn = csbNode:getChildByName(AccountRegisterView.BT_RETURN)
	btnReturn:addTouchEventListener(btcallback)
	
	--local sp_public_frame = csbNode:getChildByName("sp_public_frame")

	--账号输入
		--帐号背景
	local account_bg = self._csbNode:getChildByName("text_field_regist_bg_1")
	local text_field_1 = account_bg:getChildByName("text_field_regist_1")
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
		:setPlaceHolder("6-31位字符")
		:setVisible(account_bg:isVisible())
		:addTo(text_field_1)

	--密码输入	

	local pass_bg = self._csbNode:getChildByName("text_field_regist_bg_2")
	local text_field_2 = pass_bg:getChildByName("text_field_regist_2")	
	
	self.edit_Password = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:setVisible(pass_bg:isVisible())
		:addTo(text_field_2)
		
		
	--确认密码输入	
	
	local repass_bg = self._csbNode:getChildByName("text_field_regist_bg_3")
	local text_field_3 = repass_bg:getChildByName("text_field_regist_3")	
	
	self.edit_RePassword = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:setVisible(repass_bg:isVisible())
		:addTo(text_field_3)

	--推广员	

	local tuiguang_bg = self._csbNode:getChildByName("text_field_regist_bg_4")
	local text_field_4 = tuiguang_bg:getChildByName("text_field_regist_4")	

	self.edit_Spreader = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)--:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入推广员ID")
		:setVisible(tuiguang_bg:isVisible())
		:addTo(text_field_4)

	--条款协议
	self.cbt_Agreement =  csbNode:getChildByName(AccountRegisterView.CBT_AGREEMENT)
		:setSelected(AccountRegisterView.bAgreement)

	--显示协议
	csbNode:getChildByName(AccountRegisterView.BT_AGREEMENT):addTouchEventListener(btcallback)

	--注册按钮
	csbNode:getChildByName(AccountRegisterView.BT_REGISTER):addTouchEventListener(btcallback)
end

function AccountRegisterView:onViewLoad()
	self:loadFinished()
end

function AccountRegisterView:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function AccountRegisterView:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:play("closeAni", false)
end

function AccountRegisterView:onButtonClickedEvent(ref)
	ExternalFun.playClickEffect()
	local senderName = ref:getName()
	if senderName == AccountRegisterView.BT_RETURN then
		--self:getParent():getParent():onShowLogon()
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ACCOUNT_REGISTER_LAYER})
	elseif senderName == AccountRegisterView.BT_AGREEMENT then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.REGISTER_AGREATMENT)
	elseif senderName == AccountRegisterView.BT_REGISTER then
		-- 判断 非 数字、字母、下划线、中文 的帐号
		local szAccount = self.edit_Account:getText()
		local filter = string.find(szAccount, "^[a-zA-Z0-9_\128-\254]+$")
		if nil == filter then
			showToast(self, "帐号包含非法字符, 请重试!", 1)
			return
		end
		szAccount = string.gsub(szAccount, " ", "")
		local szPassword = string.gsub(self.edit_Password:getText(), " ", "")
		local szRePassword = string.gsub(self.edit_RePassword:getText(), " ", "")
		local bAgreement = self.cbt_Agreement:isSelected()
		local szSpreader = string.gsub(self.edit_Spreader:getText(), " ", "")
		
		--兼容旧的写法
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		local findContext = contextProxy:findContextByName(VIEW_LIST.LOGON_SCENE)
		
		if (findContext == nil) then
			assert(false, "can't find ClientScene!")
		end
		
		local viewCompoment = findContext:getView()
		
		viewCompoment:onRegisterByAccount(szAccount,szPassword,szRePassword,bAgreement,szSpreader)
	end
end

function AccountRegisterView:setAgreement(bAgree)
	self.cbt_Agreement:setSelected(bAgree)
end

return AccountRegisterView