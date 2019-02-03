local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local AgentRechargeLayer = import(".AgentRechargeLayer")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")

--商城页面
local ShopLayer = class("ShopLayer", cc.BaseLayer)

-- 进入场景而且过渡动画结束时候触发。
function ShopLayer:onEnterTransitionFinish()
	return self
end

-- 退出场景而且开始过渡动画时候触发。
function ShopLayer:onExitTransitionStart()
    return self
end

function ShopLayer:onWillViewEnter()
	self:initUI()	
	self._csbNodeAni:setAnimationEndCallFunc("open_normal", function ()
		self:enterViewFinished()
		self:afterInitUI()
		local musicPath = string.format("sound/shop_%d.mp3", math.random(1,2))
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, musicPath)
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("change_title_call", function ()
		--更换标题回调
		self:changeShopTitle()
	end)
	
	self._csbNodeAni:play("open_normal", false)
end

function ShopLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("close_normal", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:setAnimationEndCallFunc("close_agent", function ()
		if (self._rechargeTitle) then
			self:exitViewFinished()
		end
	end)
	
	local curSelectIndex = self._radioLeftMenuButtonGroup:getSelectedButtonIndex()
	if (curSelectIndex ~= -1) and (self._radioLeftMenuButtonGroup:getRadioButtonByIndex(curSelectIndex):getTag() == yl.PAYMENT_VIP) then
		--是代理充值页
		self._csbNodeAni:play("close_agent", false)
	else
		--其他
		self._csbNodeAni:play("close_normal", false)
	end
end

function ShopLayer:ctor(index)
	self.super.ctor(self)

    -- 加载csb资源
	local csbPath = ""
	if (appConfigProxy._appStoreSwitch == 0) then
		csbPath = "Shop/ShopLayer.csb"
	else
		csbPath = "Shop/ShopLayerAppStore.csb"
	end
	
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)		
    self._csbNode = csbNode
	
    self._selectType = index or yl.PAYMENT_ALIPAY
end

--初始化主界面
function ShopLayer:initUI()
	local bg = self._csbNode:getChildByName("bg")
	local top_bg = bg:getChildByName("info_top_bg")

	local layout_shop = ExternalFun.seekWigetByName(self._csbNode, "layout_shop")

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
	
	--返回按钮
	self._return = top_bg:getChildByName("Button_return")
    self._return:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	
	--充值渠道标题
	self._rechargeTitle = layout_shop:getChildByName("recharge_logo")
	
	
	--重置
	self._resetPrice = layout_shop:getChildByName("btn_take_reset")
	self._resetPrice:addClickEventListener(handler(self, self.onButtonClickedEvent))
	--金额按钮列表
	self._btnPrice1 = layout_shop:getChildByName("btn_price_1")
	:setVisible(false)
	self._btnPrice1:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice2 = layout_shop:getChildByName("btn_price_2")
	:setVisible(false)
	self._btnPrice2:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice3 = layout_shop:getChildByName("btn_price_3")
	:setVisible(false)
	self._btnPrice3:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice4 = layout_shop:getChildByName("btn_price_4")
	:setVisible(false)
	self._btnPrice4:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice5 = layout_shop:getChildByName("btn_price_5")
	:setVisible(false)
	self._btnPrice5:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice6 = layout_shop:getChildByName("btn_price_6")
	:setVisible(false)
	self._btnPrice6:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice7 = layout_shop:getChildByName("btn_price_7")
	:setVisible(false)
	self._btnPrice7:addClickEventListener(handler(self, self.onButtonClickedEvent))
	self._btnPrice8 = layout_shop:getChildByName("btn_price_8")
	:setVisible(false)
	self._btnPrice8:addClickEventListener(handler(self, self.onButtonClickedEvent))	

	--左侧单选按钮组
    self._radioLeftMenuButtonGroup = ccui.RadioButtonGroup:create()
    self._radioLeftMenuButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioLeftMenuButtonGroup)
end

function ShopLayer:afterInitUI()
	local bg = self._csbNode:getChildByName("bg")
	
--[[	
yl.PAYMENT_ALIPAY		= 2		--支付宝支付渠道
yl.PAYMENT_WECHAT		= 3		--微信支付渠道
yl.PAYMENT_QQ			= 6		--QQ支付渠道
yl.PAYMENT_QUICK		= 7		--快捷支付渠道
yl.PAYMENT_JINDONG		= 9		--京东支付渠道
yl.PAYMENT_BAIDU		= 10	--百度支付渠道
yl.PAYMENT_VIP			= 11	--代理充值
yl.PAYMENT_APPLE		= 12	--苹果内购
yl.PAYMENT_APPSTORE_APPLE=100	--苹果审核内购	
--]]
	
	--查询商城信息
	self:emit("V2M_QueryShopInfo", function (leftmenu, shopdata)
		self.m_shopdata = shopdata
		self.m_leftmemudata = leftmenu
		local index = 0
		local listView = ExternalFun.seekWigetByName(self._csbNode,"ListViewMenu")
		
		local bFindVip = false
		for _, v in pairs(leftmenu) do
			index = index + 1
			
			local rto_container = ExternalFun.seekWigetByName(self._csbNode,"radio_container_template"):clone()
			rto_container:setName(string.format("radio_container_%d", index))
			listView:pushBackCustomItem(rto_container)
			local rto_container_size = rto_container:getContentSize()
			
			--初始化首个充值渠道为默认
			if (index == 1) then
				self._selectType = v.ID
				self:changeShopTitle()
				self:changeShopUI()
			end
			
			if (v.ID == yl.PAYMENT_ALIPAY) then
				local rto_item = ccui.RadioButton:create("btn_shop_alipay_0.png","btn_shop_alipay_0.png","btn_shop_alipay_1.png","btn_shop_alipay_0.png","btn_shop_alipay_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_WECHAT) then
				local rto_item = ccui.RadioButton:create("btn_shop_weixin_0.png","btn_shop_weixin_0.png","btn_shop_weixin_1.png","btn_shop_weixin_0.png","btn_shop_weixin_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_QQ) then
				local rto_item = ccui.RadioButton:create("btn_shop_qq_0.png","btn_shop_qq_0.png","btn_shop_qq_1.png","btn_shop_qq_0.png","btn_shop_qq_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_QUICK) then
				local rto_item = ccui.RadioButton:create("btn_shop_quick_0.png","btn_shop_quick_0.png","btn_shop_quick_1.png","btn_shop_quick_0.png","btn_shop_quick_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_JINDONG) then
				local rto_item = ccui.RadioButton:create("btn_shop_jingdong_0.png","btn_shop_jingdong_0.png","btn_shop_jingdong_1.png","btn_shop_jingdong_0.png","btn_shop_jingdong_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_BAIDU) then
				local rto_item = ccui.RadioButton:create("btn_shop_baidu_0.png","btn_shop_baidu_0.png","btn_shop_baidu_1.png","btn_shop_baidu_0.png","btn_shop_baidu_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_VIP) then
				local rto_item = ccui.RadioButton:create("btn_shop_daili_0.png","btn_shop_daili_0.png","btn_shop_daili_1.png","btn_shop_daili_0.png","btn_shop_daili_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
				bFindVip = true
			elseif (v.ID == yl.PAYMENT_APPLE) then
				local rto_item = ccui.RadioButton:create("btn_shop_apple_0.png","btn_shop_apple_0.png","btn_shop_apple_1.png","btn_shop_apple_0.png","btn_shop_apple_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)	
			elseif (v.ID == yl.PAYMENT_APPSTORE_APPLE) then
				local rto_item = ccui.RadioButton:create("btn_shop_apple_0.png","btn_shop_apple_0.png","btn_shop_apple_1.png","btn_shop_apple_0.png","btn_shop_apple_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_ALIPAY_SM) then
				local rto_item = ccui.RadioButton:create("btn_shop_alism_0.png","btn_shop_alism_0.png","btn_shop_alism_1.png","btn_shop_alism_0.png","btn_shop_alism_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_WECHAT_SM) then
				local rto_item = ccui.RadioButton:create("btn_shop_wxsm_0.png","btn_shop_wxsm_0.png","btn_shop_wxsm_1.png","btn_shop_wxsm_0.png","btn_shop_wxsm_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)
			elseif (v.ID == yl.PAYMENT_QQ_SM) then
				local rto_item = ccui.RadioButton:create("btn_shop_qqsm_0.png","btn_shop_qqsm_0.png","btn_shop_qqsm_1.png","btn_shop_qqsm_0.png","btn_shop_qqsm_0.png", ccui.TextureResType.plistType)
				rto_item:addEventListener(handler(self, self.onSelectedEvent))	
				rto_item:setPosition(rto_container_size.width / 2, rto_container_size.height / 2)
				rto_item:setName(string.format("rto_shop_item_%d", index))
				rto_item:setTag(v.ID)
				rto_container:addChild(rto_item)
				self._radioLeftMenuButtonGroup:addRadioButton(rto_item)				
			else
				assert(false, "不支持的支付类别")
			end
		end
		
		--左侧充值列表自动上下滑动 提示玩家
		local items = listView:getItems()
		if (#items > 6) then
			self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function ()
				listView:scrollToItem(#items - 1, cc.p(0,0.5),cc.p(0,0.5), 2)
			end), cc.DelayTime:create(0.5), cc.CallFunc:create(function ()
				if (#items > 0) then
					listView:scrollToItem(1, cc.p(0,0.5),cc.p(0,0.5), 2)
				end
			end)))
		end
		
		self:initShopUI()
		if (bFindVip) then
			self:initAgentUI()
		end
	end)
end

function ShopLayer:initShopUI()
	local bg = self._csbNode:getChildByName("bg")
	local top_bg = bg:getChildByName("info_top_bg")

	local layout_shop = ExternalFun.seekWigetByName(self._csbNode, "layout_shop")
	
	local shop_item = self.m_shopdata[self._selectType]
	
	--第一个就是 代理充值
	if (self._selectType == yl.PAYMENT_VIP) then
		self._csbNodeAni:play("normal_to_agent_up", false)
	end
	
	--输入框
	local text_field_bg_1 = layout_shop:getChildByName("text_field_bg_1")
	local text_field_1 = text_field_bg_1:getChildByName("text_field_1")	
	text_field_size = text_field_1:getContentSize()
	self.m_edit1 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_DECIMAL)
		:setPlaceHolder("请输入充值金额...")
		:addTo(text_field_1)	
	
	--编辑框 响应回调
	local function onEditEvent(event,editbox)
		if event == "return" then
			local cur_shop_item = self.m_shopdata[self._selectType]
			local src = editbox:getText()
			if src == nil or src == "" then
				return
			end
			
			local ndst = tonumber(src)
			
			if (type(ndst) ~= "number") then
				return
			end
			
			local min_item = nil
			local max_item = nil
			
			if (#cur_shop_item == 1) then
				min_item = cur_shop_item[1]
				max_item = cur_shop_item[1]
			else
				min_item = cur_shop_item[1]
				max_item = cur_shop_item[#cur_shop_item]
			end
			
			if ndst >= min_item and ndst <= max_item then
				editbox:setText(string.format("%0.2f", ndst))
			elseif (ndst < min_item) then
				editbox:setText(string.format("%0.2f", min_item))
			elseif (ndst > max_item) then
				editbox:setText(string.format("%0.2f", max_item))
			end
		end
	end
	
	
	self._btnRecharge = layout_shop:getChildByName("btn_recharge")
	self._btnRecharge:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	self.m_edit1:registerScriptEditBoxHandler(onEditEvent)		
	
	if (shop_item == nil) then
		return
	end
		
	for i = 1, #shop_item do
		local btn = layout_shop:getChildByName(string.format("btn_price_%d", i))
		btn:setTitleText(string.format("%d元", shop_item[i]))
		btn:setVisible(true)
		btn:setTag(shop_item[i])
	end
end

function ShopLayer:changeShopUI()
	local bg = self._csbNode:getChildByName("bg")
	local top_bg = bg:getChildByName("info_top_bg")

	local layout_clip = bg:getChildByName("layout_clip")
	local layout_shop = layout_clip:getChildByName("layout_shop")
	
	local shop_item = self.m_shopdata[self._selectType]
	
	if (shop_item == nil) then
		return
	end
	
	if (self.m_edit1) then
		self.m_edit1:setText("")
	end
	
	for i = 1, 8 do
		local btn = layout_shop:getChildByName(string.format("btn_price_%d", i))
		
		if (i <= #shop_item) then
			btn:setTitleText(string.format("%d元", shop_item[i]))
			btn:setVisible(true)
			btn:setTag(shop_item[i])
		else
			btn:setVisible(false)
		end
	end
end

function ShopLayer:changeShopTitle()
	--更换标题回调
	local title = ""
	
	if (self._selectType == yl.PAYMENT_ALIPAY) then
		title = "img_shop_alipay.png"
	elseif (self._selectType == yl.PAYMENT_WECHAT) then
		title = "img_shop_wechat.png"
	elseif (self._selectType == yl.PAYMENT_QQ) then
		title = "img_shop_qq.png"
	elseif (self._selectType == yl.PAYMENT_QUICK) then
		title = "img_shop_quick.png"
	elseif (self._selectType == yl.PAYMENT_JINDONG) then
		title = "img_shop_jingdong.png"
	elseif (self._selectType == yl.PAYMENT_BAIDU) then
		title = "img_shop_baidu.png"
	elseif (self._selectType == yl.PAYMENT_APPLE) then
		title = "img_shop_apple.png"
	elseif (self._selectType == yl.PAYMENT_APPSTORE_APPLE) then
		title = "img_shop_apple.png"
	elseif (self._selectType == yl.PAYMENT_ALIPAY_SM) then
		title = "img_shop_alismpay.png"
	elseif (self._selectType == yl.PAYMENT_WECHAT_SM) then
		title = "img_shop_wxsmpay.png"
	elseif (self._selectType == yl.PAYMENT_QQ_SM) then
		title = "img_shop_qqsmpay.png"
	end
	
	if (title ~= "") then
		self._rechargeTitle:loadTexture(title, UI_TEX_TYPE_PLIST)
	end
end


function ShopLayer:initAgentUI()
	local bg = self._csbNode:getChildByName("bg")
	local layout_clip = bg:getChildByName("layout_clip")
	local layout_agent = layout_clip:getChildByName("layout_agent")
	
	local user_info_bg =  layout_agent:getChildByName("user_info_bg")
	--我的ID
	local text_id = user_info_bg:getChildByName("text_id")
	local text_account = user_info_bg:getChildByName("text_account")
	
	text_id:setString(string.format("%d", GlobalUserItem.dwGameID))
	text_account:setString(GlobalUserItem.szAccount)
	
	local btn_copy_my_info = layout_agent:getChildByName("btn_copy_my_info")
	btn_copy_my_info:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_sq_daili = layout_agent:getChildByName("btn_sq_daili")
	btn_sq_daili:addClickEventListener(handler(self, self.onButtonClickedEvent))
	local btn_ts_daili = layout_agent:getChildByName("btn_ts_daili")
	btn_ts_daili:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	--注册UI查询事件
	self:attach("M2V_QueryAgentResult", function (e, agentlist)
		local listView = layout_agent:getChildByName("ListView_1");
		assert(type(listView) == "userdata", "expected userdata data, but not now!")
		if (type(listView) ~= "userdata") then
			return
		end

		listView:removeAllItems()
		
		assert(type(listView) == "userdata", "expected userdata data, but not now!")
		if (type(listView) ~= "userdata") then
			return
		end

		listView:removeAllItems()
		
        local itemLayoutTemplate = layout_agent:getChildByName("AgentRechargeItemLayout")
        local itemLayoutTemplateChildrens = itemLayoutTemplate:getChildren()
	    
        local itemLayout = nil
		for i=1,#agentlist do
			local itemInfo = agentlist[i]
            local index = (i - 1) % #itemLayoutTemplateChildrens;
            
            if (index == 0) then
                itemLayout = itemLayoutTemplate:clone()
                listView:pushBackCustomItem(itemLayout)
            end

            assert(type(itemLayout) == "userdata", "expected userdata data, but not now!")
            if (type(itemLayout) ~= "userdata") then
                return
            end

            local btn = itemLayout:getChildByName(string.format("agent_item_%d", index))
		    btn :setVisible(true)
			    :addTouchEventListener(function(ref, type)
                    if type == ccui.TouchEventType.ended then
                        self:onAgentButtonClickedEvent(ref, itemInfo)
        	        end
                end)
			local text = btn:getChildByName("Text_6")
			text:setString(itemInfo.name)
		end
	end)
	
	self:emit("V2M_QueryAgentInfo")
end

function ShopLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function ShopLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

--按键监听
function ShopLayer:onAgentButtonClickedEvent(sender, itemInfo)
	ExternalFun.playClickEffect()

	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {itemInfo}}, VIEW_LIST.AGENT_RECHARGE_LAYER)
end

--按键监听
function ShopLayer:onButtonClickedEvent(sender, itemInfo)
	ExternalFun.playClickEffect()

	local senderName = sender:getName()

	if senderName == "Button_return" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_LAYER})
	elseif (senderName == "btn_copy_my_info") then
		
		local res, msg = MultiPlatform:getInstance():copyToClipboard(string.format("我的ID:%d  我的账号:%s", GlobalUserItem.dwGameID, GlobalUserItem.szAccount))
		
		if true == res then
			local function callback()
				printf("打开微信")
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOW_POP_TIPS})
				cc.Application:getInstance():openURL("weixin://")
			end
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"复制成功，系统正在前往微信，请稍后！", 3, callback}}, VIEW_LIST.SHOW_POP_TIPS)
	
		else
			if type(msg) == "string" then
				showToast(self, msg, 1, cc.c3b(250,0,0))
			end
		end
	elseif (senderName == "btn_sq_daili") then
		local ClientData = ExternalFun.create_netdata(logincmd.CMD_MB_ShenQingDailiCheck)
		ClientData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_SHENGQING_AGENT_CHECK)
		ClientData:pushdword(GlobalUserItem.dwUserID)
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在检查申请资质，请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)
		appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ClientData, function (Datatable, Responce)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
			if (Responce.code == 0) then
				if Datatable.sub == logincmd.SUB_GP_OPERATE_SUCCESS then
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.AGENT_AGREATMENT)
				elseif Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE then
					local pData = Datatable.data
					local lResultCode = pData:readint()
					local szDescribe = pData:readstring()
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {szDescribe,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)		
				end
			else
				showToast(self, "网络连接失败！", 2)
			end
		end)
	elseif (senderName == "btn_ts_daili") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_TOUSU_DAILI)
	elseif (senderName == "btn_price_1") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_2") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_3") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_4") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_5") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_6") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_7") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_price_8") then
		local number = sender:getTag()
		local strNumber = string.format("%0.2f", number)
		self.m_edit1:setText(strNumber)
	elseif (senderName == "btn_take_reset") then
		self.m_edit1:setText("")
	elseif (senderName == "btn_recharge") then
		local amount = tonumber(self.m_edit1:getText()) or 0
		
		if (amount == 0) then
			showToast(self, "请输入正确的充值数额！", 1)
			return
		else
			if (self._selectType == yl.PAYMENT_APPLE or self._selectType == yl.PAYMENT_APPSTORE_APPLE) then
                local function payCallBack(param)
                    if type(param) == "string" and "true" == param then
                        GlobalUserItem.setTodayPay()

                        showToast(self, "支付成功", 2)
						
						self:emit("V2M_PaySuccess")
                        --更新用户游戏豆
                        GlobalUserItem.dUserBeans = GlobalUserItem.dUserBeans + item.count
                        --通知更新        
                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                        eventListener.obj = yl.RY_MSG_USERWEALTH
                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                        --重新请求支付列表
                        --self:reloadBeanList()

                        --self:updateScoreInfo()
                    else
                        showToast(self, "支付失败", 2)
                    end
                end
				
                local payparam = {}
                payparam.http_url = BaseConfig.WEB_HTTP_URL
                payparam.uid = GlobalUserItem.dwUserID
                payparam.productid = ""
                payparam.price = amount
				
                MultiPlatform:getInstance():thirdPartyPay(yl.ThirdParty.IAP, payparam, payCallBack)
			else
				local url = string.format("%s/Mobile/PayInterFace.aspx?account=%s&amount=%d&qudaoId=%d", BaseConfig.WEB_HTTP_URL, string.urlencode(GlobalUserItem.szAccount), amount, self._selectType)
				cc.Application:getInstance():openURL(url)
			end			
		end
	end
end

function ShopLayer:onSelectedEvent(sender,eventType)
    if (ccui.RadioButtonEventType.selected == eventType) then
		local oldSelect = self._selectType
		local newSelect = sender:getTag()
		self._selectType = newSelect
		
		--更新价格按钮状态
		self:changeShopUI()
		--计算移动方向
		local oldSelectSortID = 0
		local newSelectSortID = 0
		for k,v in pairs(self.m_leftmemudata) do
			if (v.ID == oldSelect) then
				oldSelectSortID = k
			elseif (v.ID == newSelect) then
				newSelectSortID = k
			end
		end
		
		--播放动画
		if (oldSelect == yl.PAYMENT_VIP) then
			--更换标题回调
			self:changeShopTitle()
			if (oldSelectSortID > newSelectSortID) then
				self._csbNodeAni:play("agent_to_normal_up", false)
			else
				self._csbNodeAni:play("agent_to_normal_down", false)
			end
		elseif (newSelect == yl.PAYMENT_VIP) then
			if (oldSelectSortID > newSelectSortID) then
				self._csbNodeAni:play("normal_to_agent_up", false)
			else
				self._csbNodeAni:play("normal_to_agent_down", false)
			end
		else
			self._csbNodeAni:play("change_to_other_normal", false)
		end
    elseif (ccui.RadioButtonEventType.unselected == eventType) then

    end
end

return ShopLayer