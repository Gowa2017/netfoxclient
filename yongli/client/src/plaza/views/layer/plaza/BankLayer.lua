--[[
	银行界面
	2017_08_25 MXM
]]

local BankLayer = class("BankLayer", cc.BaseLayer)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function BankLayer:ctor(pageindex)
	self.super.ctor(self)
	
	self._pageDefaultIndex = pageindex or 0
    -- 加载csb资源
	local csbPath = "Bank/BankLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	
	
	self.edit_take = nil
	self.edit_save = nil
	self.edit_take_pass = nil
	self.bank_name = nil
	
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
	end)
	
	self:attach("M2V_InsureSuccess", function (e, msg)
		if (self.edit_take) then
			self.edit_take:setText("")
		end
		
		if (self.edit_save) then
			self.edit_save:setText("")
		end
		
		if (self.edit_take_pass) then
			self.edit_take_pass:setText("")
		end
	end)

	self:attach("M2V_InsureFailed", function (e, msg)
		showToast(self, msg, 2)
	end)
	
	self:attach("M2V_UpdateInsureRecord", function (e, data)
		self:initSaveTakeRecordUI(data)
	end)
	
	self:attach("M2V_UpdateInsureExchangeRecord", function (e, data)
		self:initExchangeRecordUI(data)
	end)
	
	self:attach("M2V_CheckBindBankResult", function (e, data)
		if (data.szBankNo == "" and data.szAlipayID == "") then
			self:initExchangeSubBind(self._exchangeNode)

			if (self._pageDefaultIndex == 3) then
				self._csbNodeAni:play("show_exchange_bind", false)
			end
		else
			self:initExchangeSubExchange(self._exchangeNode, data)
			
			if (self._pageDefaultIndex == 3) then
				self._csbNodeAni:play("show_exchange_exchange", false)
			end			
		end
	end)
	
	self:attach("M2V_Bind_Result", function (e, code, data)
		if (code == 0) then
			self:initExchangeSubExchange(self._exchangeNode, data)
			--切换到兑换页面
			self._csbNodeAni:play("show_exchange_exchange", false)			
		end
	end)
	
	self:attach("M2V_Exchange_Result", function (e, code, score)
		if (code == 0) then
			self.edit_exchange:setText("")
			--更新可提现金额
			self._exchange_coin_0:setString(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		end
	end)	
end

--初始化主界面
function BankLayer:initUI()
	local bg = self._csbNode:getChildByName("bg")
	local top_bg = bg:getChildByName("info_top_bg")
	--返回按钮
	self._return = top_bg:getChildByName("Button_return")
    self._return:addClickEventListener(handler(self, self.onButtonClickedEvent))

  
    --携带金币
    self._txtScore = top_bg:getChildByName("atlas_coin")
	local str = string.formatNumberTThousands(GlobalUserItem.lUserScore)
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	if (GlobalUserItem.lUserScore < 10000) then
		self._txtScore:setString(str)
	elseif (GlobalUserItem.lUserScore >= 10000 and GlobalUserItem.lUserScore < 100000000) then
		self._txtScore:setString(str .. "万")
	elseif (GlobalUserItem.lUserScore >= 100000000) then
		self._txtScore:setString(str .. "亿")
	end

	--注册UI监听事件
	self:attach("M2V_Update_UserScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		
		if (score < 10000) then
			self._txtScore:setString(str)
		elseif (score >= 10000 and score < 100000000) then
			self._txtScore:setString(str .. "万")
		elseif (score >= 100000000) then
			self._txtScore:setString(str .. "亿")
		end				
	end)	
		

    --银行金币
    self._txtInsure = top_bg:getChildByName("atlas_bankscore")
	
    str = string.formatNumberTThousands(GlobalUserItem.lUserInsure)  --MXM更新大厅银行金币
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	
	if (GlobalUserItem.lUserInsure < 10000) then
		self._txtInsure:setString(str)
	elseif (GlobalUserItem.lUserInsure >= 10000 and GlobalUserItem.lUserInsure < 100000000) then
		self._txtInsure:setString(str .. "万")
	elseif (GlobalUserItem.lUserInsure >= 100000000) then
		self._txtInsure:setString(str .. "亿")
	end	

	--银行金币
	--注册UI监听事件
	self:attach("M2V_Update_InsureScore", function (e, score)
		local str = string.formatNumberTThousands(score)  --MXM更新大厅银行金币
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		
		if (score < 10000) then
			self._txtInsure:setString(str)
		elseif (score >= 10000 and score < 100000000) then
			self._txtInsure:setString(str .. "万")
		elseif (score >= 100000000) then
			self._txtInsure:setString(str .. "亿")
		end			
	end)	
	
	--注册UI监听事件
	self:attach("M2V_Update_BankInfo", function (e, parm)
		local szBankNo = parm.szBankNo or ""
		local szBankAddress = parm.szBankAddress or ""
		local szBankName = parm.szBankName or ""
		local text_bank_no = ExternalFun.seekWigetByName(self._csbNode, "bankAccount")
		
		local dealBankNo = string.gsub(szBankNo, "(.?)(.?)(.?)(.?)(.*)", function (cat1, cat2, cat3, cat4, cat5)
			cat5 = string.reverse(cat5)
			cat5 = string.gsub(cat5, "(.?)(.?)(.?)(.?)(.*)", function (scat1, scat2, scat3, scat4, scat5)
				local scat5 = string.gsub(scat5, ".", "*")
				local lastFix = scat1 .. scat2 .. scat3 .. scat4 .. scat5
				return string.reverse(lastFix)
			end)
			local preFix = cat1 .. cat2 .. cat3 .. cat4 .. cat5
			return preFix
		end)	
		text_bank_no:setString(dealBankNo)
	end)

	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	
	--左侧按钮菜单
	local leftMenu = bg:getChildByName("left_menu")
	--左侧单选按钮组
    self._radioLeftMenuButtonGroup = ccui.RadioButtonGroup:create()
    self._radioLeftMenuButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioLeftMenuButtonGroup)		
	--存入
	local rto_container_1 = leftMenu:getChildByName("radio_container_1")
	local rto_container_1_size = rto_container_1:getContentSize()
	self.m_rtoBankSave = ccui.RadioButton:create("btn_bank_save_0.png","btn_bank_save_0.png","btn_bank_save_1.png","btn_bank_save_0.png","btn_bank_save_0.png", ccui.TextureResType.plistType)
	self.m_rtoBankSave:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBankSave:setPosition(rto_container_1_size.width / 2, rto_container_1_size.height / 2)
	self.m_rtoBankSave:setName("rto_bank_save")
	rto_container_1:addChild(self.m_rtoBankSave)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoBankSave)
		
	--取出
	local rto_container_2 = leftMenu:getChildByName("radio_container_2")
	local rto_container_2_size = rto_container_2:getContentSize()
	self.m_rtoBankTake = ccui.RadioButton:create("btn_bank_take_0.png","btn_bank_take_0.png","btn_bank_take_1.png","btn_bank_take_0.png","btn_bank_take_0.png", ccui.TextureResType.plistType)
	self.m_rtoBankTake:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBankTake:setPosition(rto_container_2_size.width / 2, rto_container_2_size.height / 2)
	self.m_rtoBankTake:setName("rto_bank_take")
	rto_container_2:addChild(self.m_rtoBankTake)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoBankTake)
	
	--存取记录
	local rto_container_3 = leftMenu:getChildByName("radio_container_3")
	local rto_container_3_size = rto_container_3:getContentSize()
	self.m_rtoBankSaveTakeRecord = ccui.RadioButton:create("btn_bank_record_0.png","btn_bank_record_0.png","btn_bank_record_1.png","btn_bank_record_0.png","btn_bank_record_0.png", ccui.TextureResType.plistType)
	self.m_rtoBankSaveTakeRecord:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBankSaveTakeRecord:setPosition(rto_container_3_size.width / 2, rto_container_3_size.height / 2)
	self.m_rtoBankSaveTakeRecord:setName("rto_bank_save_take_record")
	rto_container_3:addChild(self.m_rtoBankSaveTakeRecord)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoBankSaveTakeRecord)
	
	--兑换
	local rto_container_4 = leftMenu:getChildByName("radio_container_4")
	local rto_container_4_size = rto_container_4:getContentSize()
	self.m_rtoBankExchange = ccui.RadioButton:create("btn_bank_exchange_0.png","btn_bank_exchange_0.png","btn_bank_exchange_1.png","btn_bank_exchange_0.png","btn_bank_exchange_0.png", ccui.TextureResType.plistType)
	self.m_rtoBankExchange:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBankExchange:setPosition(rto_container_4_size.width / 2, rto_container_4_size.height / 2)
	self.m_rtoBankExchange:setName("rto_bank_exchange")
	rto_container_4:addChild(self.m_rtoBankExchange)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoBankExchange)			
	
	--兑换记录
	local rto_container_5 = leftMenu:getChildByName("radio_container_5")
	local rto_container_5_size = rto_container_5:getContentSize()
	self.m_rtoBankExchangeRecord = ccui.RadioButton:create("btn_bank_exchange_record_0.png","btn_bank_exchange_record_0.png","btn_bank_exchange_record_1.png","btn_bank_exchange_record_0.png","btn_bank_exchange_record_0.png", ccui.TextureResType.plistType)
	self.m_rtoBankExchangeRecord:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoBankExchangeRecord:setPosition(rto_container_5_size.width / 2, rto_container_5_size.height / 2)
	self.m_rtoBankExchangeRecord:setName("rto_bank_exchange_record")
	rto_container_5:addChild(self.m_rtoBankExchangeRecord)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoBankExchangeRecord)
	
	self:initSaveUI()
	self:initTakeUI()
	self:initSaveTakeRecordUI()
	self:initExchangeUI()
	self:initExchangeRecordUI()
	
	--选择默认页
	--cocos2d 的bug 代码选择不响应Selected事件 暂时用该方法解决
	if (self._pageDefaultIndex == 0) then
		self._csbNodeAni:play("show_save", false)
	elseif (self._pageDefaultIndex == 1) then
		self._csbNodeAni:play("show_take", false)
	elseif (self._pageDefaultIndex == 2) then
		self._csbNodeAni:play("show_save_take_record", false)
	elseif (self._pageDefaultIndex == 3) then
		--这个界面是异步的 所以需要等待网络返回后跳转
		--self._csbNodeAni:play("show_exchange_bind", false)
	elseif (self._pageDefaultIndex == 4) then
		self._csbNodeAni:play("show_exchange_record", false)
	end	
	self._radioLeftMenuButtonGroup:setSelectedButtonWithoutEvent(self._pageDefaultIndex)
end

--初始化存款界面
function BankLayer:initSaveUI()
	local bg = self._csbNode:getChildByName("bg")
    --游戏币存
    self._saveNode = bg:getChildByName("layout_save")
	
	local text_field_bg_1 =  self._saveNode:getChildByName("text_field_bg_1")
	local text_field_1 =  text_field_bg_1:getChildByName("text_field_1")
	local text_field_size = text_field_1:getContentSize()
	--金额输入
	self.edit_save = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_DECIMAL)
		:setPlaceHolder("请输入操作金额")
		:addTo(text_field_1)
		
	
	--重置
	local btn_reset = self._saveNode:getChildByName("btn_save_reset")
	btn_reset:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--金币中文格式化
	self.text_format_save_score = self._saveNode:getChildByName("format_score")


	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "changed" then
			local src = editbox:getText()
			if src == nil or src == "" then
				return
			end
			
			local dst =  src --string.gsub(src,"([^0-9])","")		
			--editbox:setText(dst)
			local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
			
			local ndst = tonumber(result)
			
			if type(ndst) == "number" and ndst < 9999999999999 then
				self.text_format_save_score:setString(ExternalFun.numberTransiform(tonumber(ndst)))
			else
				self.text_format_save_score:setString("")
			end
			
			--editbox:setText(result)
		elseif event == "return" then
			local dst = editbox:getText()
			if dst == nil or dst == "" then
				return
			end
			local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
			local ndst = tonumber(result)
			local dst2f = string.format("%0.2f", ndst)
			if type(ndst) == "number" and ndst < 9999999999999 then
				self.text_format_save_score:setString(ExternalFun.numberTransiform(tonumber(dst2f)))
			else
				self.text_format_save_score:setString("")
			end

			editbox:setText(dst2f)
		end
	end	

	--确定取款
	local btn_take_ok = self._saveNode:getChildByName("btn_save_ok")
	btn_take_ok:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self.edit_save:registerScriptEditBoxHandler(onEditEvent)
	
	--金币键盘
	local btn_save_10 = self._saveNode:getChildByName("btn_save_10")
	btn_save_10:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_save_100 = self._saveNode:getChildByName("btn_save_100")
	btn_save_100:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_save_1000 = self._saveNode:getChildByName("btn_save_1000")
	btn_save_1000:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_save_10000 = self._saveNode:getChildByName("btn_save_10000")
	btn_save_10000:addClickEventListener(handler(self, self.onButtonClickedEvent))
end

--初始化取款界面
function BankLayer:initTakeUI()
	--游戏币取
	local bg = self._csbNode:getChildByName("bg")
    self._takeNode = bg:getChildByName("layout_take")
	
	local text_field_bg_1 =  self._takeNode:getChildByName("text_field_bg_1")
	local text_field_1 =  text_field_bg_1:getChildByName("text_field_1")
	local text_field_size = text_field_1:getContentSize()
	--金额输入
	self.edit_take = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_DECIMAL)
		:setPlaceHolder("请输入操作金额")
		:addTo(text_field_1)
		
	--金币中文格式化
	self.text_format_take_score = self._takeNode:getChildByName("format_score")	
		
	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "changed" then
			local src = editbox:getText()
			if src == nil or src == "" then
				return
			end
			
			local dst =  src --string.gsub(src,"([^0-9])","")		
			--editbox:setText(dst)
			local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
			
			local ndst = tonumber(result)
			
			if type(ndst) == "number" and ndst < 9999999999999 then
				self.text_format_take_score:setString(ExternalFun.numberTransiform(tonumber(ndst)))
			else
				self.text_format_take_score:setString("")
			end
			
			--editbox:setText(result)
		elseif event == "return" then
			local dst = editbox:getText()
			if dst == nil or dst == "" then
				return
			end
			local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
			local ndst = tonumber(result)
			local dst2f = string.format("%0.2f", ndst)
			if type(ndst) == "number" and ndst < 9999999999999 then
				self.text_format_take_score:setString(ExternalFun.numberTransiform(tonumber(dst2f)))
			else
				self.text_format_take_score:setString("")
			end

			editbox:setText(dst2f)
		end
	end	

	--确定取款
	local btn_take_ok = self._takeNode:getChildByName("btn_take_ok")
	btn_take_ok:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self.edit_take:registerScriptEditBoxHandler(onEditEvent)
	
	--银行密码
	local text_field_bg_2 =  self._takeNode:getChildByName("text_field_bg_2")
	local text_field_2 =  text_field_bg_2:getChildByName("text_field_2")
	local text_field_size = text_field_2:getContentSize()	
	self.edit_take_pass = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入银行密码")
		:addTo(text_field_2)		
		
	
	--重置
	local btn_reset = self._takeNode:getChildByName("btn_take_reset")
	btn_reset:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--金币键盘
	local btn_take_10 = self._takeNode:getChildByName("btn_take_10")
	btn_take_10:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_100 = self._takeNode:getChildByName("btn_take_100")
	btn_take_100:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_1000 = self._takeNode:getChildByName("btn_take_1000")
	btn_take_1000:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_10000 = self._takeNode:getChildByName("btn_take_10000")
	btn_take_10000:addClickEventListener(handler(self, self.onButtonClickedEvent))
end

--初始化存取款记录界面
function BankLayer:initSaveTakeRecordUI(data)
	if (not data) then
		return
	end
	local layout_save_take_record = self._csbNode:getChildByName("bg"):getChildByName("layout_save_take_record")
	local item = layout_save_take_record:getChildByName("record_item")
	local record_list = layout_save_take_record:getChildByName("record_list")
	record_list:removeAllItems()
	for i = 1, #data do
		local itemClone = item:clone()
		local textData = itemClone:getChildByName("data")
		local textType = itemClone:getChildByName("type")
		local textScore = itemClone:getChildByName("score")
		local textFee = itemClone:getChildByName("fee")
		
		--日期
		local date = os.date("%Y/%m/%d %H:%M:%S", tonumber(data[i].date)/1000)
		textData:setString(date)

		--交易类别	
		textType:setString(data[i].tradeType)
		
		--交易金额
		textScore:setString(string.formatNumberFhousands(data[i].swapScore))
		
--[[			--转账ID
		cc.Label:createWithTTF(item.id,"base/fonts/round_body.ttf",32)
			:move(812,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(216,187,115,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)
			:setVisible(false)
		--]]
		
		textFee:setString(string.formatNumberFhousands(data[i].revenue))
		
		record_list:pushBackCustomItem(itemClone)
	end
end

--初始化兑换界面
function BankLayer:initExchangeUI()
	--游戏币取
	local bg = self._csbNode:getChildByName("bg")
    self._exchangeNode = bg:getChildByName("layout_exchange")
	
	self:emit("V2M_CheckBindBankInfo")
end

function BankLayer:initExchangeSubBind(parent)
	--local layout_sub_bind = parent:getChildByName("self.layout_sub_bind")
    self.layout_sub_bind = parent:getChildByName("layout_sub_bind")
	
	local text_field_bg_1 =  self.layout_sub_bind:getChildByName("text_field_bg_1")
	local text_field_1 =  text_field_bg_1:getChildByName("text_field_1")
	local text_field_size = text_field_1:getContentSize()
	--真实姓名
	self.edit_realname = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入真实姓名")
		:addTo(text_field_1)

    --支付宝
	local text_field_bg_5 =  self.layout_sub_bind:getChildByName("text_field_bg_5")
    :setVisible(false)
	local text_field_5 =  text_field_bg_5:getChildByName("text_field_5")
	local text_field_size5 = text_field_5:getContentSize()
	
	self.edit_alipayID = ccui.EditBox:create(text_field_size5, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(128)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入支付宝账号")
		:addTo(text_field_5)
		

    --------		
	local account_field_bg_2 =  self.layout_sub_bind:getChildByName("text_field_bg_2")
	local text_field_2 =  account_field_bg_2:getChildByName("text_field_2")
	local text_field_size = text_field_2:getContentSize()
	
	--银行卡账号
	self.edit_bankaccount = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(23)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("请输入银行卡账号")
		:addTo(text_field_2)
		
	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "changed" then
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
		elseif event == "return" then
	--[[		local dst = editbox:getText()
			if dst == nil or dst == "" then
				return
			end
		
			editbox:setText(dst2f)--]]
		end
	end	
	self.edit_bankaccount:registerScriptEditBoxHandler(onEditEvent)

	--开户行地址
	--local text_field_bg_3 =  self.layout_sub_bind:getChildByName("text_field_bg_3")
    self.text_field_bg_3 =  self.layout_sub_bind:getChildByName("text_field_bg_3")
	local text_field_3 =  self.text_field_bg_3:getChildByName("text_field_3")
	local text_field_size = text_field_3:getContentSize()

	self.edit_bankaddr = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入开户行地址")
		:addTo(text_field_3)

	--开户行地址
	local text_field_bg_4 =  self.layout_sub_bind:getChildByName("text_field_bg_4")
	local text_field_4 =  text_field_bg_4:getChildByName("text_field_4")
	local text_field_size = text_field_4:getContentSize()
	self.img_else_bank_field_bg = text_field_bg_4
	
	
	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "changed" then
		elseif event == "return" then
			local src = editbox:getText()
			if src == nil or src == "" then
				return
			end
			
			self.bank_name = src
		end
	end	
	
	--其他银行
	self.edit_bankelse = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setEnabled(false)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入其他银行名称")
		:addTo(text_field_4)
	self.edit_bankelse:registerScriptEditBoxHandler(onEditEvent)


	--左侧单选按钮组
    self._radioBankButtonGroup = ccui.RadioButtonGroup:create()
    self._radioBankButtonGroup:setAllowedNoSelection(true)
    self:addChild(self._radioBankButtonGroup)

	--创建银行单选框
	for i=1, 10 do
		local rto_container = self.layout_sub_bind:getChildByName(string.format("radio_container_%d", i))
		local rto_container_size = rto_container:getContentSize()
		local normalImg = string.format("bank_logo_%d.png", i)
		local radioButton = ccui.RadioButton:create(normalImg,normalImg,"img_bank_mask.png",normalImg,normalImg, ccui.TextureResType.plistType)
		radioButton:addEventListener(handler(self, self.onSelectedEvent))	
		radioButton:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
		radioButton:setName(string.format("rto_bank_%d", i))
		rto_container:addChild(radioButton)
		self._radioBankButtonGroup:addRadioButton(radioButton)
	end
		
	--立刻绑定
	local btn_bind = self.layout_sub_bind:getChildByName("btn_bind")
	btn_bind:addClickEventListener(handler(self, self.onButtonClickedEvent))
end

function BankLayer:initExchangeSubExchange(parent, data)
	local layout_sub_exchange = parent:getChildByName("layout_sub_exchange")
	
	--self._subexchangeNode = layout_sub_exchange
	
	local text_field_bg_1 =  layout_sub_exchange:getChildByName("text_field_bg_1")
	local text_field_1 =  text_field_bg_1:getChildByName("text_field_1")
	local text_field_size = text_field_1:getContentSize()
	
	--真实姓名*号处理(因为包含中文字符比较特殊)
	local text_real_name = layout_sub_exchange:getChildByName("realName")

    local realnamestr = ''
    if data.szRealName ~= nil then
        realnamestr = data.szRealName
    end

    printf("realnamestr....", data.szRealName) 
       
	local lenInByte = string.len(realnamestr)
	local unicode_count = 0
	local index = 0
	local dealName = ""
	while index < lenInByte do
		index = index + 1
		unicode_count = unicode_count + 1
		local curByte = string.byte(realnamestr, index)
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
		 
		local char = string.sub(realnamestr, index, index+byteCount-1)
		if (index == 1) then
			dealName = dealName .. char
		else
			dealName = dealName .. "*"
		end
		index = index + byteCount -1
	end
	
	text_real_name:setString(dealName)
	
    ------支付宝
	local text_alipayid = layout_sub_exchange:getChildByName("alipayid")

    local alipayIDstr = ''
    if data.szAlipayID ~= nil then
        alipayIDstr = data.szAlipayID
    end

    printf("alipayIDstr....", data.szAlipayID)       

	local lenInByte_alipayID = string.len(alipayIDstr)
	local unicode_count_alipayID = 0
	local index_alipayID = 0
	local deal_alipayID = ""
	while index_alipayID < lenInByte_alipayID do
		index_alipayID = index_alipayID + 1
		unicode_count_alipayID = unicode_count_alipayID + 1
		local curByte_alipayID = string.byte(alipayIDstr, index_alipayID)
		local byteCount_alipayID = 1;
		if curByte_alipayID>0 and curByte_alipayID<=127 then
			byteCount_alipayID = 1
		elseif curByte_alipayID>=192 and curByte_alipayID<223 then
			byteCount_alipayID = 2
		elseif curByt_alipayIDe>=224 and curByte_alipayID<239 then
			byteCount_alipayID = 3
		elseif curByte_alipayID>=240 and curByte_alipayID<=247 then
			byteCount_alipayID = 4
		end
		 
		local char_alipayID = string.sub(alipayIDstr, index_alipayID, index_alipayID+byteCount_alipayID-1)
		if (index_alipayID <= 5) then
			deal_alipayID = deal_alipayID .. char_alipayID
		else
			deal_alipayID = deal_alipayID .. "*"
		end
		index_alipayID = index_alipayID + byteCount_alipayID -1
	end
	
	text_alipayid:setString(deal_alipayID)

    ---------

    local bannostr = ''
    if data.szBankNo ~= nil then
        bannostr = data.szBankNo
    end

    printf("bannostr....", data.szBankNo) 

	--银行卡*号处理
	local text_bank_no = layout_sub_exchange:getChildByName("bankAccount")
	local dealBankNo = string.gsub(bannostr, "(.?)(.?)(.?)(.?)(.*)", function (cat1, cat2, cat3, cat4, cat5)
		cat5 = string.reverse(cat5)
		cat5 = string.gsub(cat5, "(.?)(.?)(.?)(.?)(.*)", function (scat1, scat2, scat3, scat4, scat5)
			local scat5 = string.gsub(scat5, ".", "*")
			local lastFix = scat1 .. scat2 .. scat3 .. scat4 .. scat5
			return string.reverse(lastFix)
		end)
		local preFix = cat1 .. cat2 .. cat3 .. cat4 .. cat5
		return preFix
	end)	
	text_bank_no:setString(dealBankNo)
	
	--当前可兑换金币
	self._exchange_coin_0 = layout_sub_exchange:getChildByName("atlas_coin_0")
	self._exchange_coin_0:setString(string.format("%0.2f", data.lDrawScore))

	
	--金额输入
	--金币中文格式化
	--self.text_format_take_score = self._takeNode:getChildByName("format_score")	
		
	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "changed" then

		elseif event == "return" then
			local dst = editbox:getText()
			if dst == nil or dst == "" then
				return
			end
			local number = tonumber(editbox:getText())
			
			if (number > data.lDrawScore) then
				editbox:setText(string.format("%0.2f", data.lDrawScore))
				
				--闪烁动画
				local blinkAction = cc.Blink:create(2, 5)
				self._exchange_coin_0:runAction(blinkAction)
			end
		end
	end
	
	self.edit_exchange = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2, text_field_size.height / 2)
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(32)
		:setPlaceholderFontSize(32)
		:setMaxLength(13)
		:setAnchorPoint(cc.p(0.5, 0.5))
		:setFontColor(cc.c4b(255,255,255,255))
		:setPlaceHolder("请输入提现金额")
		:setInputMode(cc.EDITBOX_INPUT_MODE_DECIMAL)
		self.edit_exchange:addTo(text_field_1)
		
	self.edit_exchange:registerScriptEditBoxHandler(onEditEvent)
		
	--确定体现
	--local btn_exchange_ok = layout_sub_exchange:getChildByName("btn_exchange_ok")
	--btn_exchange_ok:addClickEventListener(handler(self, self.onButtonClickedEvent))
    --btn_exchange_ok.setVisible(false)
    
    --银行
    local btn_bank_ok = layout_sub_exchange:getChildByName("btn_bank_ok")
	btn_bank_ok:addClickEventListener(handler(self, self.onButtonClickedEvent))

    --支付宝
    local btn_exchange_ok_alipayid = layout_sub_exchange:getChildByName("btn_alipay_ok")
	btn_exchange_ok_alipayid:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--修改银行信息
--	local btn_exchange_modifybank = layout_sub_exchange:getChildByName("btn_exchange_modifybank")
--	btn_exchange_modifybank:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	
	--重置
	local btn_reset = layout_sub_exchange:getChildByName("btn_exchange_reset")
	btn_reset:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--金币键盘
	local btn_take_50 = layout_sub_exchange:getChildByName("btn_exchange_50")
	btn_take_50:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_100 = layout_sub_exchange:getChildByName("btn_exchange_100")
	btn_take_100:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_500 = layout_sub_exchange:getChildByName("btn_exchange_500")
	btn_take_500:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_1000 = layout_sub_exchange:getChildByName("btn_exchange_1000")
	btn_take_1000:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_take_5000 = layout_sub_exchange:getChildByName("btn_exchange_5000")
	btn_take_5000:addClickEventListener(handler(self, self.onButtonClickedEvent))	
end

--初始化兑换记录界面
function BankLayer:initExchangeRecordUI(data)
	if (not data) then
		return
	end
	
	local layout_exchange_record = self._csbNode:getChildByName("bg"):getChildByName("layout_exchange_record")
	local item = layout_exchange_record:getChildByName("exchange_item")
	local exhcange_list = layout_exchange_record:getChildByName("exhcange_list")
	exhcange_list:removeAllItems()
	for i = 1, #data do
		local itemClone = item:clone()
		local textData = itemClone:getChildByName("data")
		local textOrderId = itemClone:getChildByName("orderid")
		local textScore = itemClone:getChildByName("score")
		local textState = itemClone:getChildByName("state")
		local textNote = itemClone:getChildByName("note")
		
		--申请日期
		local date = os.date("%Y/%m/%d %H:%M:%S", tonumber(data[i].ApplyDate)/1000)
		-- print(date)
		-- print(""..tonumber(item.date))
		textData:setString(date)

		--申请单号
		textOrderId:setString(data[i].OrderID)
		
		--金额
		textScore:setString(string.formatNumberFhousands(data[i].SellMoney))
		
		 --状态
		textState:setString(data[i].Status)
			
		--备注
		textNote:setString(data[i].RejectReason)
	
		exhcange_list:pushBackCustomItem(itemClone)
	end
end

function BankLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		--查询银行
		if 1 == GlobalUserItem.cbInsureEnabled then
			self:emit("V2M_QueryBankInfo")
		end
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/bank.mp3")
		self:initUI()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function BankLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:play("closeAni", false)
--[[	self._csbNodeAni:setAnimationEndCallFunc("closeVisitorAni", function ()
		self:exitViewFinished()
	end)
	
	--判断是否是游客模式 游客模式显示不同的界面
	if GlobalUserItem.bVistor then
		self._csbNodeAni:play("closeNormalAni", false)
	else
		self._csbNodeAni:play("closeVisitorAni", false)
	end--]]
end

function BankLayer:onEnterTransitionFinish( )
    --if 1 == GlobalUserItem.cbInsureEnabled then
    --   self:showPopWait()
    --    self._bankFrame:onGetBankInfo()
    --end
end

function BankLayer:onExit()
    --if self._bankFrame:isSocketServer() then
    --    self._bankFrame:onCloseSocket()
    --end
    --if nil ~= self._bankFrame._gameFrame then
    --    self._bankFrame._gameFrame._shotFrame = nil
    --    self._bankFrame._gameFrame = nil
    --end
end

function BankLayer:onFlushBank()
	self:showPopWait()
	self._bankFrame:onFlushBank()
end

--按键监听
function BankLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	
	if (senderName == "Button_return") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_LAYER})
	elseif (senderName == "btn_take_ok") then
		local takescore = tonumber(self.edit_take:getText())
		local insurepass = self.edit_take_pass:getText()
		self:emit("V2M_TakeBankScore", insurepass, takescore or 0)
	elseif (senderName == "btn_take_reset") then
		self.edit_take:setText("")
		self.edit_take_pass:setText("")
		self.text_format_take_score:setString("")
	elseif (senderName == "btn_save_ok") then
		local savescore = tonumber(self.edit_save:getText())
		self:emit("V2M_SaveBankScore", savescore or 0)
	elseif (senderName == "btn_save_reset") then
		self.edit_save:setText("")
		self.text_format_save_score:setString("")
	elseif (senderName == "btn_exchange_reset") then
		self.edit_exchange:setText("")
	elseif (senderName == "btn_save_10") then
		if (GlobalUserItem.lUserScore < 20) then
			self.edit_save:setText(string.format("%0.2f", GlobalUserItem.lUserScore))
		else
			self.edit_save:setText(string.format("%0.2f", 20))
		end
	elseif (senderName == "btn_save_100") then
		if (GlobalUserItem.lUserScore < 100) then
			self.edit_save:setText(string.format("%0.2f", GlobalUserItem.lUserScore))
		else
			self.edit_save:setText(string.format("%0.2f", 100))
		end
	elseif (senderName == "btn_save_1000") then
		if (GlobalUserItem.lUserScore < 1000) then
			self.edit_save:setText(string.format("%0.2f", GlobalUserItem.lUserScore))
		else
			self.edit_save:setText(string.format("%0.2f", 1000))
		end
	elseif (senderName == "btn_save_10000") then
		if (GlobalUserItem.lUserScore < 10000) then
			self.edit_save:setText(string.format("%0.2f", GlobalUserItem.lUserScore))
		else
			self.edit_save:setText(string.format("%0.2f", 10000))
		end
	elseif (senderName == "btn_take_10") then
		if (GlobalUserItem.lUserInsure < 20) then
			self.edit_take:setText(string.format("%0.2f", GlobalUserItem.lUserInsure))
		else
			self.edit_take:setText(string.format("%0.2f", 20))
		end
	elseif (senderName == "btn_take_100") then
		if (GlobalUserItem.lUserInsure < 100) then
			self.edit_take:setText(string.format("%0.2f", GlobalUserItem.lUserInsure))
		else
			self.edit_take:setText(string.format("%0.2f", 100))
		end
	elseif (senderName == "btn_take_1000") then
		if (GlobalUserItem.lUserInsure < 1000) then
			self.edit_take:setText(string.format("%0.2f", GlobalUserItem.lUserInsure))
		else
			self.edit_take:setText(string.format("%0.2f", 1000))
		end
	elseif (senderName == "btn_take_10000") then
		if (GlobalUserItem.lUserInsure < 10000) then
			self.edit_take:setText(string.format("%0.2f", GlobalUserItem.lUserInsure))
		else
			self.edit_take:setText(string.format("%0.2f", 10000))
		end	
		
	elseif (senderName == "btn_exchange_50") then
		if (GlobalUserItem.BankInfoData.lDrawScore < 50) then
			self.edit_exchange:setText(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		else
			self.edit_exchange:setText(string.format("%0.2f", 50))
		end	
	elseif (senderName == "btn_exchange_100") then
		if (GlobalUserItem.BankInfoData.lDrawScore < 100) then
			self.edit_exchange:setText(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		else
			self.edit_exchange:setText(string.format("%0.2f", 100))
		end	
	elseif (senderName == "btn_exchange_500") then
		if (GlobalUserItem.BankInfoData.lDrawScore < 500) then
			self.edit_exchange:setText(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		else
			self.edit_exchange:setText(string.format("%0.2f", 500))
		end	
	elseif (senderName == "btn_exchange_1000") then
		if (GlobalUserItem.BankInfoData.lDrawScore < 1000) then
			self.edit_exchange:setText(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		else
			self.edit_exchange:setText(string.format("%0.2f", 1000))
		end	
	elseif (senderName == "btn_exchange_5000") then
		if (GlobalUserItem.BankInfoData.lDrawScore < 5000) then
			self.edit_exchange:setText(string.format("%0.2f", GlobalUserItem.BankInfoData.lDrawScore))
		else
			self.edit_exchange:setText(string.format("%0.2f", 5000))
		end
	elseif (senderName == "btn_bind") then
		if (GlobalUserItem.BankInfoData.szBankNo == "") then
			local bankname = self.bank_name
			local realname = self.edit_realname:getText()

			local bankno = string.gsub(self.edit_bankaccount:getText(), " ", "")

            local alipayID = ""
            local bankaddr = ""

            if self.alipayflag then
                alipayID = self.edit_bankaddr:getText()
            else
			    bankaddr = self.edit_bankaddr:getText()
            end
			
			if ((bankname == nil or bankname == "") and (alipayID == nil or alipayID == "")) then
				showToast(self, "请选择一个银行！")
				return
			elseif (realname == nil or realname == "") then
				showToast(self, "真实姓名不能为空")
				return
			elseif ((bankno == nil or bankno == "") and (alipayID == nil or alipayID == "")) then
				showToast(self, "银行卡号不能为空")
				return
			elseif ((bankaddr == nil or bankaddr == "") and (alipayID == nil or alipayID == "")) then
				showToast(self, "银行开户地址不能为空")
				return
			end

            BankInfo = {
                realname =nil,
                bankno = nil,
                bankname =nil,
                bankaddr =nil,
                alipayID =nil       
            }

            BankInfo.realname = realname
            BankInfo.bankno = bankno
            BankInfo.bankname = bankname
            BankInfo.bankaddr = bankaddr
            BankInfo.alipayID = alipayID

            --print("V2M_BindBankInfo:alipayID", BankInfo.alipayID)
			--self:emit("V2M_BindBankInfo", realname, bankno, bankname, bankaddr, alipayID)

            self:emit("V2M_BindBankInfo", BankInfo)
		else
			showToast(self, "你已经绑定过银行卡信息了！",1)
		end
	elseif (senderName == "btn_bank_ok") then
		local number = tonumber(self.edit_exchange:getText())
		
		if (number == nil or number == 0) then
			showToast(self, "提现金额不能为0！")
			return
		end
		
        if (GlobalUserItem.BankInfoData.szBankNo == nil or GlobalUserItem.BankInfoData.szBankNo == "") then
			showToast(self, "未绑定银行卡！")
			return
		end

		if (GlobalUserItem.szRegisterMobile ~= "") then
			self:emit("V2M_ExchangeBankScore", number, 0)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"您还未绑定手机，无法提现，是否前往绑定？",function(ok)
				if ok == true then
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {2}}, VIEW_LIST.PERSON_LAYER)
				end
			end}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
		end	
    -------
    	elseif (senderName == "btn_alipay_ok") then
		local number = tonumber(self.edit_exchange:getText())
		
		if (number == nil or number == 0) then
			showToast(self, "提现金额不能为0！")
			return
		end

        if (GlobalUserItem.BankInfoData.szAlipayID == nil or GlobalUserItem.BankInfoData.szAlipayID == "") then
			showToast(self, "未绑定支付宝！")
			return
		end
		
		if (GlobalUserItem.szRegisterMobile ~= "") then
			self:emit("V2M_ExchangeBankScore", number, 1)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"您还未绑定手机，无法提现，是否前往绑定？",function(ok)
				if ok == true then
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {2}}, VIEW_LIST.PERSON_LAYER)
				end
			end}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
		end
    -------    	
	elseif (senderName == "btn_exchange_modifybank") then
		if (GlobalUserItem.BankInfoData == nil or GlobalUserItem.BankInfoData.szBankNo == "") then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"您还未绑定银行卡信息，无法修改银行信息哦！",nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
		else
			if (GlobalUserItem.szRegisterMobile ~= "") then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {GlobalUserItem.BankInfoData}}, VIEW_LIST.BANK_MODIFY_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"您还未绑定手机，无法修改银行信息，是否前往绑定？",function(ok)
					if ok == true then
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {2}}, VIEW_LIST.PERSON_LAYER)
					end
				end}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
			end
		end
	end
end

--输入框监听
function BankLayer:onEditEvent(event,editbox)
	if event == "changed" then
		local src = editbox:getText()
		if src == nil or src == "" then
			return
		end
		
		local dst =  src --string.gsub(src,"([^0-9])","")		
		--editbox:setText(dst)
		local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
		
		local ndst = tonumber(result)
		
		if type(ndst) == "number" and ndst < 9999999999999 then
			self.m_textNumber:setString(ExternalFun.numberTransiform(tonumber(ndst)))
		else
			self.m_textNumber:setString("")
		end
		
		--editbox:setText(result)
	elseif event == "return" then
		local dst = editbox:getText()
		if dst == nil or dst == "" then
			return
		end
		local numstr = self.m_textNumber:getString()
		local result = string.gsub(dst, "([%d]*)([%p])([%d]*).*", "%1%2%3")
		local ndst = tonumber(result)
		local dst2f = string.format("%0.2f", ndst)
		if type(ndst) == "number" and ndst < 9999999999999 then
			self.m_textNumber:setString(ExternalFun.numberTransiform(tonumber(dst2f)))
		else
			self.m_textNumber:setString("")
		end

		editbox:setText(dst2f)
	end
end


function BankLayer:onSelectedEvent(sender,eventType)
	local senderName = sender:getName()
	
    self.alipayflag = false

	if (ccui.RadioButtonEventType.selected == eventType) then
		ExternalFun.playClickEffect()
		
		if (senderName == "rto_bank_save") then
			self._csbNodeAni:play("show_save", false)
		elseif (senderName == "rto_bank_take") then
			self._csbNodeAni:play("show_take", false)
		elseif (senderName =="rto_bank_save_take_record") then
			self._csbNodeAni:play("show_save_take_record", false)
			self:emit("V2M_QueryBankRecord")
		elseif (senderName == "rto_bank_exchange") then
			if (GlobalUserItem.BankInfoData.szBankNo == "" and  GlobalUserItem.BankInfoData.szAlipayID == "") then
				if (self.bank_name == "") then
					self._csbNodeAni:play("show_exchange_else_bind", false)	
				else
					self._csbNodeAni:play("show_exchange_bind", false)	
				end				
			else
				self._csbNodeAni:play("show_exchange_exchange", false)	
			end
		elseif (senderName == "rto_bank_exchange_record") then
			self._csbNodeAni:play("show_exchange_record", false)
			self:emit("V2M_QueryBankExchangeRecord")
		elseif (senderName == "rto_bank_1") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)
			
			self.bank_name = "工商银行"

            self.alipayflag = false

		elseif (senderName == "rto_bank_2") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)
			
			self.bank_name = "建设银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_3") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)
			
			self.bank_name = "农业银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_4") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)
			
			self.bank_name = "交通银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_5") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)

			self.bank_name = "中国银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_6") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)

			self.bank_name = "招商银行"
		elseif (senderName == "rto_bank_7") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)

			self.bank_name = "民生银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_8") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)

			self.bank_name = "邮政储蓄银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_9") then
			self.edit_bankelse:setEnabled(false)
			self._csbNodeAni:play("show_exchange_bind", false)

			self.bank_name = "光大银行"

            self.alipayflag = false
		elseif (senderName == "rto_bank_10") then
			self.edit_bankelse:setEnabled(true)
			self._csbNodeAni:play("show_exchange_else_bind", false)

			self.edit_bankelse:setText("")
			self.bank_name = ""

            self.alipayflag = true
		else
			assert(false, "undefine radio event")
		end	

        --------------- 
        if self.alipayflag then
                    local text_1_0_bg_3 =  self.text_field_bg_3:getChildByName("Text_1_0")
                    :setText("支付宝:");

                    self.edit_bankaddr:setPlaceHolder("请输入支付宝账号")

                    local account_field_bg_2 =  self.layout_sub_bind:getChildByName("text_field_bg_2")
                    :setVisible(false)
        else
                    local text_1_0_bg_3 =  self.text_field_bg_3:getChildByName("Text_1_0")
                    :setText("开户行地址:");

                    self.edit_bankaddr:setPlaceHolder("请输入开户行地址")

                    local account_field_bg_2 =  self.layout_sub_bind:getChildByName("text_field_bg_2")
                    :setVisible(true)
        end
        ---------------	

	end

 
--[[	if tag == BankLayer.CBT_SAVEORTAKE or tag == BankLayer.CBT_TRANSFER then
		local transfermode = (tag == BankLayer.CBT_TRANSFER)
		self._cbtTransfer:setSelected(transfermode)
		self._cbtSaveTake:setSelected(not transfermode)

        self._transferNode:setVisible(transfermode)
        self._takesaveNode:setVisible(not transfermode)
		self._transferArea:setVisible(transfermode)
		self._takesaveArea:setVisible(not transfermode)

		self.edit_Score:setText("")
		self.edit_TransferScore:setText("")
		self.m_textNumber:setString("")--]]

        --手续费
       --[[ local str = string.format("提示:存入游戏币免手续费,取出将扣除%d‰的手续费。存款无需输入银行密码。", self.m_tabBankConfigInfo.wRevenueTake)
		--调整位置
		if transfermode then 
			self.m_textNumber:setPosition(930, 365)
            str = string.format("提示:普通玩家游戏币赠送需扣除%d‰的手续费。", self.m_tabBankConfigInfo.wRevenueTransfer)
            if 0 ~= GlobalUserItem.cbMemberOrder then
                local vipConfig = GlobalUserItem.MemberList[GlobalUserItem.cbMemberOrder]
                str = str .. vipConfig._name .. "扣除" .. vipConfig._insure .. "‰手续费。"
            end
            self._notifyTextPresent:setString(str)
		else
			self.m_textNumber:setPosition(930, 330)
            self._notifyText:setString(str)
		end--]]        
--[[	elseif tag == BankLayer.CBT_BY_ID or tag == BankLayer.CBT_BY_NAME then
		local byID = (tag == BankLayer.CBT_BY_ID)
		self.edit_TransferDst:setText("")
		self.cbt_TransferByID:setSelected(byID)
		self.cbt_TransferByName:setSelected(not byID)

		local szRes = (byID and "bnak_word_targetid.png" or "bnak_word_targetname.png")
		self._labelTarget:setTexture(szRes)
	end--]]
end

--取款操作
function BankLayer:onTakeScore()

	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_Score:getText(),"([^0-9.])","")
    --szScore = string.gsub(szScore, "[.]", "")
	local szPass = self.edit_Password:getText()
    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end

	local lOperateScore = tonumber(szScore)
	--[[if lOperateScore < 1 then
		showToast(self,"请输入正确金额！",2)
		return
	end--]]

    if lOperateScore > GlobalUserItem.lUserInsure then
        showToast(self,"您银行游戏币的数目余额不足,请重新输入游戏币数量！",2)
        return
    end

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",2)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	self:showPopWait()
	self._bankFrame:onTakeScore(lOperateScore,szPass)
end

--存款
function BankLayer:onSaveScore()
	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_Score:getText(),"([^0-9.])","")	
    --szScore = string.gsub(szScore, "[.]", "")

	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end
	
	local lOperateScore = tonumber(szScore)
	
	--[[if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end--]]
	
    if lOperateScore > GlobalUserItem.lUserScore then
        showToast(self,"您所携带游戏币的数目余额不足,请重新输入游戏币数量!",2)
        return
    end

	self:showPopWait()

	self._bankFrame:onSaveScore(lOperateScore)
end

function BankLayer:onTransferScore()
	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_TransferScore:getText(),"([^0-9])","")
	local szPass = self.edit_TransferPassword:getText()
	local szTarget = self.edit_TransferDst:getText()
	local byID = 1--self.cbt_TransferByID:isSelected() and 1 or 0;

	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end

	local lOperateScore = tonumber(szScore)
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

	if #szPass < 1 then 
		showToast(self,"请输入钱包密码！",2)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	if #szTarget < 1 then 
		showToast(self,"请输入赠送用户ID！",2)
		return
	end

	self:showPopWait()
	self._bankFrame:onTransferScore(lOperateScore,szTarget,szPass,byID)
end

--获取父场景节点(ClientScene)
function BankLayer:getRootNode( )
	return self:getParent():getParent():getParent():getParent()
end

--操作结果
function BankLayer:onBankCallBack(result,message)

	self:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2)
	end

	if result == 1 then
		self._txtScore:setString(string.formatNumberFhousands(GlobalUserItem.lUserScore))
		self._txtInsure:setString(string.formatNumberFhousands(GlobalUserItem.lUserInsure))
		self.edit_TransferDst:setText("")
		self.edit_TransferScore:setText("")
		self.edit_TransferPassword:setText("")
		self.edit_Score:setText("")
		self.edit_Password:setText("")
		self.m_textNumber:setString("")
		--更新大厅
		self:getRootNode()._gold:setString(string.formatNumberFhousands(GlobalUserItem.lUserScore))

        if self._bankFrame._oprateCode == BankFrame.OP_SEND_SCORE then
            -- 转账凭证
            self:showCerLayer(self._bankFrame._tabTarget)
        end
	end

	if result == self._bankFrame.OP_GET_BANKINFO then
		local enableTransfer = (0 == message.cbEnjoinTransfer)
		--self._cbtTransfer:setEnabled(enableTransfer)
		--self._cbtTransfer:setVisible(enableTransfer)
        --self._cbtSaveTake:setEnabled(enableTransfer)
        --self._cbtSaveTake:setVisible(enableTransfer)

		--取款收费比例
		--local str = string.format("提示:存入游戏币免手续费,取出将扣除%d‰的手续费。存款无需输入银行密码。", message.wRevenueTake)
        --self._notifyText:setString(str)

        self._txtInsure:setString(string.formatNumberFhousands(GlobalUserItem.lUserInsure))
        self._txtScore:setString(string.formatNumberFhousands(GlobalUserItem.lUserScore))
	end
end

function BankLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function BankLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

local TAG_MASK = 101
local BTN_SHARE = 102
local BTN_SAVEPIC = 103
-- 显示凭证
function BankLayer:showCerLayer( tabData )
    if type(tabData) ~= "table" then
        return
    end
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Bank/BankCerLayer.csb", self)
    local stamp = tabData.opTime or os.time()

    local hide = function()
        local scale1 = cc.ScaleTo:create(0.2, 0.0001)
        local call1 = cc.CallFunc:create(function()
            rootLayer:removeFromParent()
        end)
        csbNode.m_imageBg:runAction(cc.Sequence:create(scale1, call1))
    end
    local url = GlobalUserItem.getShareUrl()
    -- 截图分享
    local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
    local area = cc.rect(0, 0, framesize.width, framesize.height)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local tag = ref:getTag()
            if TAG_MASK == tag then
                hide()
            elseif BTN_SHARE == tag then
                ExternalFun.popupTouchFilter(0, false)
                captureScreenWithArea(area, "ce_code.png", function(ok, savepath)
                    ExternalFun.dismissTouchFilter()
                    if ok then
                        MultiPlatform:getInstance():customShare(function(isok)
                                    end, "转账凭证", "分享我的转账凭证", url, savepath, "true")
                    end
                end)
            elseif BTN_SAVEPIC == tag then
                ExternalFun.popupTouchFilter(0, false)
                captureScreenWithArea(area, "ce_code.png", function(ok, savepath)         
                    if ok then  
                        if true == MultiPlatform:getInstance():saveImgToSystemGallery(savepath, stamp .. "ce_code.png") then
                            showToast(self, "您的转账凭证图片已保存至系统相册", 1)
                        end
                    end
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function()
                        ExternalFun.dismissTouchFilter()
                    end)))
                end)
            end           
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setTouchEnabled(true)
    image_bg:setSwallowTouches(true)
    image_bg:setScale(0.00001)
    csbNode.m_imageBg = image_bg

    -- 赠送人昵称
    local sendnick = ClipText:createClipText(cc.size(210, 30), GlobalUserItem.szAccount, nil, 30)
    sendnick:setTextColor(cc.c3b(79, 212, 253))
    sendnick:setAnchorPoint(cc.p(0, 0.5))
    sendnick:setPosition(cc.p(260, 450))
    image_bg:addChild(sendnick)

    -- 赠送人ID
    local sendid = image_bg:getChildByName("txt_senduid")
    sendid:setString(GlobalUserItem.dwGameID .. "")

    -- 接收人昵称
    local recnick = ClipText:createClipText(cc.size(210, 30), tabData.opTargetAcconts or "", nil, 30)
    recnick:setTextColor(cc.c3b(79, 212, 253))
    recnick:setAnchorPoint(cc.p(0, 0.5))
    recnick:setPosition(cc.p(810, 450))
    image_bg:addChild(recnick)

    -- 接收人ID
    local recid = image_bg:getChildByName("txt_recuid")
    local reuid = tabData.opTargetID or 0
    recid:setString(reuid .. "")

    -- 赠送游戏币
    local sendcount = image_bg:getChildByName("atlas_sendnum")
    local count = tabData.opScore or 0
    sendcount:setString("" .. count)

    -- 大写
    local szcount = image_bg:getChildByName("txt_sendnum")
    local szstr = ""
    if count < 9999999999999 then
        szstr = ExternalFun.numberTransiform(count)
    end
    szcount:setString(szstr)

    -- 日期
    local txtdate = image_bg:getChildByName("txt_date")    
    local tt = os.date("*t", stamp)
    txtdate:setString(string.format("%d.%02d.%02d-%02d:%02d:%02d", tt.year, tt.month, tt.day, tt.hour, tt.min, tt.sec))

    -- 凭证
    local cer = image_bg:getChildByName("txt_cerno")
    cer:setString(md5(stamp))

    -- 分享
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener( touchFunC )

    -- 保存
    btn = image_bg:getChildByName("btn_save")
    btn:setTag(BTN_SAVEPIC)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    image_bg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

return BankLayer