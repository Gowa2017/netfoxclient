--[[
	银行界面
	2017_08_25 MXM
]]

local BankModifyLayer = class("BankModifyLayer", cc.BaseLayer)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--开通银行
function BankModifyLayer:ctor(data)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "Bank/BankModifyLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self._data = data or {}
end

function BankModifyLayer:initUI()
     --返回按钮
    local btn = ExternalFun.seekWigetByName(self._csbNode, "btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = ExternalFun.seekWigetByName(self._csbNode, "btn_ok")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--验证码按钮
	self._btncode = ExternalFun.seekWigetByName(self._csbNode, "btn_get_verify_code")	
	self._btncode:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--真实姓名
	local text_field_1 = ExternalFun.seekWigetByName(self._csbNode, "text_field_1")	
	text_field_size = text_field_1:getContentSize()
	
	--中文*号处理
	local dealName = ""
	local lenInByte = string.len(self._data.szRealName)
	local unicode_count = 0
	local index = 0
	
	while index < lenInByte do
		index = index + 1
		unicode_count = unicode_count + 1
		local curByte = string.byte(self._data.szRealName, index)
		local byteCount = 1;
		if curByte>0 and curByte<=127 then
			byteCount = 1
		elseif curByte>=192 and curByte<223 then
			byteCount = 2
		elseif curByte>=224 and curByte<239 then
			byteCount = 3
		elseif curByte>=240 and curByte<=247 then
			byteCount = 4
		end
		 
		local char = string.sub(self._data.szRealName, index, index+byteCount-1)
		if (index == 1) then
			dealName = dealName .. char
		else
			dealName = dealName .. "*"
		end
		index = index + byteCount -1
	end
	
	self.m_edit1 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("真实姓名...")
		:setText(dealName)
		:setEnabled(false)
		:addTo(text_field_1)
		
		
	--确认银行卡号
	local text_field_2 = ExternalFun.seekWigetByName(self._csbNode, "text_field_2")	
	text_field_size = text_field_2:getContentSize()

	self.m_edit2 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(23)
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("请输入银行卡号...")
		:addTo(text_field_2)
	self.m_edit2:registerScriptEditBoxHandler(handler(self, self.onEditEvent))
		
		
	--开户行
	local text_field_3 = ExternalFun.seekWigetByName(self._csbNode, "text_field_3")	
	text_field_size = text_field_3:getContentSize()

	self.m_edit3 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入开户行...")
		:addTo(text_field_3)
		
	--开户行地址
	local text_field_4 = ExternalFun.seekWigetByName(self._csbNode, "text_field_4")	
	text_field_size = text_field_4:getContentSize()

	self.m_edit4 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(128)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入开户行地址...")
		:addTo(text_field_4)
		
	--手机号码
	local text_field_5 = ExternalFun.seekWigetByName(self._csbNode, "text_field_5")	
	text_field_size = text_field_5:getContentSize()

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
	
	self.m_edit5 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(11)
		:setInputMode(cc.EDITBOX_INPUT_MODE_PHONENUMBER)
		:setPlaceHolder("请输入手机号码...")
		:setText(dealPhone)
		:setEnabled(false)
		:addTo(text_field_5)		
				
	--验证码
	local text_field_6 = ExternalFun.seekWigetByName(self._csbNode, "text_field_6")	
	text_field_size = text_field_6:getContentSize()

	self.m_edit6 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(6)
		:setInputMode(cc.EDITBOX_INPUT_MODE_PHONENUMBER)
		:setPlaceHolder("请输入验证码...")
		:addTo(text_field_6)		
		
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
	end)
	
	
	self:attach("M2V_ModifyBankValidCodeResult", function (e, data)
		if (data.cbResultCode == 0) then
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
		else
			self._btncode:stopAllActions()
			self._btncode:setTitleText("获取验证码")
			self._btncode:setEnabled(true)
		end
		
		showToast(self,data.szDescString,2)
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

function BankModifyLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function BankModifyLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)

	self._csbNodeAni:play("closeAni", false)
end

--输入框监听
function BankModifyLayer:onEditEvent(event,editbox)
	if event == "changed" then
		if (editbox == self.m_edit2) then
			local src = editbox:getText()
			if src == nil or src == "" then
				return
			end
			--去除空白符
			local realText = string.gsub(src, " ", "")
			--计算输入真实总长度
			local len = string.len(realText)
			--计算数字对（银行 4个数字 为一对，最后不足补1）
			local lenArray = math.modf(len / 4)
			if (len % 4) ~= 0 then
				lenArray = lenArray + 1
			end
			
			local resultText = ""
			for i = 1, lenArray do
				if (lenArray ~= i) then
					resultText = resultText .. string.sub(realText, (i - 1) * 4 + 1, 4 * i) .. " "
				else
					resultText = resultText .. string.sub(realText, (i - 1) * 4 + 1, 4 * i)
				end
			end
			
			editbox:setText(resultText)
		end
	elseif event == "return" then
--[[		local dst = editbox:getText()
		if dst == nil or dst == "" then
			return
		end
	
		editbox:setText(dst2f)--]]
	end
end

--按键监听
function BankModifyLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()

	local senderName = sender:getName()
	
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_MODIFY_LAYER})
	elseif senderName == "btn_ok" then
		self:onModifyBank()
	elseif senderName == "btn_get_verify_code" then
		self:onModifyBankValidCode()
	end
end

--开通银行
function BankModifyLayer:onModifyBank()
	
	--银行卡卡号
	local szBankNo = string.gsub(self.m_edit2:getText(), " ", "")
	--开户行
	local szBankName = self.m_edit3:getText()
	--开户行
	local szBankAddress = self.m_edit4:getText()
	--验证码
	local szValidCode = self.m_edit6:getText()
	
	if string.len(szBankNo) < 1 then 
		showToast(self,"银行卡号不能为空哦！",2)
		return
	else
		if not (string.len(szBankNo) == 16 or string.len(szBankNo) == 19) then
			showToast(self,"银行卡号不正确哦！",2)
			return
		end
	end
	
	if string.len(szBankName) < 1 then
		showToast(self,"开户行不能为空哦！",2)
		return
	end
	
	if string.len(szBankAddress) < 1 then
		showToast(self,"开户行地址不能为空哦！",2)
		return
	end	

	if string.len(szValidCode) < 1 then
		showToast(self,"验证码不能为空哦！",2)
		return
	end	

	self:emit("V2M_ModifyBankInfo", {szBankNo = szBankNo, szBankName = szBankName, szBankAddress = szBankAddress, szValidCode = szValidCode})
end

--开通银行
function BankModifyLayer:onModifyBankValidCode()
	
	local szMobilePhone = GlobalUserItem.szRegisterMobile

	if string.len(szMobilePhone) < 1 then
		showToast(self,"手机号不能为空哦！",2)
		return
	end	

	self._btncode:setEnabled(false)
	
	self:emit("V2M_ModifyBankValidCode", szMobilePhone)
end

--显示等待
function BankModifyLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function BankModifyLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return BankModifyLayer