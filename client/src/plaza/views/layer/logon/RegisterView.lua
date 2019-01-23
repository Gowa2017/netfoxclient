local RegisterView = class("RegisterView",function()
		local registerView =  display.newLayer()
    return registerView
end)

RegisterView.BT_REGISTER = 1
RegisterView.BT_RETURN	 = 2
RegisterView.BT_AGREEMENT= 3
RegisterView.CBT_AGREEMENT = 4

RegisterView.bAgreement = true
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function RegisterView:ctor()
	local this = self
	self:setContentSize(yl.WIDTH,yl.HEIGHT)
	cc.SpriteFrameCache:getInstance():addSpriteFrames("public/public.plist")

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

	--背景
    display.newSprite("background_2.jpg")
        :move(yl.WIDTH/2,yl.HEIGHT/2)
        :addTo(self)

    --Top背景
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
    end

    --Top标题
    display.newSprite("Regist/title_regist.png")
    	:move(yl.WIDTH/2,yl.HEIGHT-51)
    	:addTo(self)

    --Top返回
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:setTag(RegisterView.BT_RETURN)
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(btcallback)

	--注册背景框
	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
        self:addChild(sp)
    end

	--帐号提示
	display.newSprite("Regist/icon_regist_tip.png")
		:move(293,540)
		:addTo(self)
	display.newSprite("Regist/text_regist_account.png")
		:move(392,540)
		:addTo(self)

	--账号输入
	self.edit_Account = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
		:move(730,540)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(31)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-31位字符")
		:addTo(self)

	--密码提示
	display.newSprite("Regist/icon_regist_tip.png")
		:move(293,450)
		:addTo(self)
	display.newSprite("Regist/text_regist_password.png")
		:move(392,450)
		:addTo(self)

	--密码输入	
	self.edit_Password = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
		:move(730,450)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:addTo(self)

	--确认密码提示
	display.newSprite("Regist/icon_regist_tip.png")
		:move(293,358)
		:addTo(self)
	display.newSprite("Regist/text_regist_confirm.png")
		:move(392,358)
		:addTo(self)

	--确认密码输入	
	self.edit_RePassword = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
		:move(730,358)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("6-26位英文字母，数字，下划线组合")
		:addTo(self)

	--推广员
	display.newSprite("Regist/text_regist_tuiguang.png")
		:move(392,268)
		:addTo(self)

	--推广员	
	self.edit_Spreader = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
		:move(730,268)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)--:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入推广员ID")
		:addTo(self)

	--条款协议
	self.cbt_Agreement = ccui.CheckBox:create("Regist/choose_regist_0.png","","Regist/choose_regist_1.png","","")
		:move(510,183)
		:setSelected(RegisterView.bAgreement)
		:setTag(RegisterView.CBT_AGREEMENT)
		:addTo(self)

	--显示协议
	ccui.Button:create("Regist/bt_regist_agreement.png","")
		:setTag(RegisterView.BT_AGREEMENT)
		:move(780,181)
		:addTo(self)
		:addTouchEventListener(btcallback)

	--注册按钮
	ccui.Button:create("Regist/bt_regist_0.png","")
		:setTag(RegisterView.BT_REGISTER)
		:move(yl.WIDTH/2,93)
		:addTo(self)
		:addTouchEventListener(btcallback)
end


function RegisterView:onButtonClickedEvent(tag,ref)
	if tag == RegisterView.BT_RETURN then
		self:getParent():getParent():onShowLogon()
	elseif tag == RegisterView.BT_AGREEMENT then
		self:getParent():getParent():onShowService()
	elseif tag == RegisterView.BT_REGISTER then
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
		local bAgreement = self:getChildByTag(RegisterView.CBT_AGREEMENT):isSelected()
		local szSpreader = string.gsub(self.edit_Spreader:getText(), " ", "")
		self:getParent():getParent():onRegister(szAccount,szPassword,szRePassword,bAgreement,szSpreader)
	end
end

function RegisterView:setAgreement(bAgree)
	self.cbt_Agreement:setSelected(bAgree)
end

return RegisterView