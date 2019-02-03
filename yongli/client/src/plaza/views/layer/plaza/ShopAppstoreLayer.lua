--[[
	商城界面
	2017_08_26 MXM

    包含 JunFuTongPay ShopPay ShopLayer 三个类
]]


local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local AgentRechargeLayer = import(".AgentRechargeLayer")
local ShopExchange = import(".ShopExchange")

local CBT_WECHAT = 101
local CBT_ALIPAY = 102
local CBT_JFT = 103

local PAYTYPE = {}
PAYTYPE[CBT_WECHAT] =
{
    str = "wx",
    plat = yl.ThirdParty.WECHAT
}
PAYTYPE[CBT_ALIPAY] =
{
    str = "zfb",
    plat = yl.ThirdParty.ALIPAY
}
PAYTYPE[CBT_JFT] =
{
    str = "jft",
    plat = yl.ThirdParty.JFT
}

--竣付通页面
local JunFuTongPay = class("ShopPay", cc.Layer)
local JFT_RETURN = 1
function JunFuTongPay:ctor(parent, itemname, price, paylist, token)
    self.m_parent = parent
    self.m_parent.m_bJunfuTongPay = true
    GlobalUserItem.bJftPay = true
    self.m_token = token

    ExternalFun.registerTouchEvent(self, true)
    local btnpos = 
    {
        {cc.p(0.5, 0.3)},
        {cc.p(0.35, 0.3), cc.p(0.65, 0.3)},
    }

    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Shop/JunFuTongPay.csb", self)

    --背景
    local bg = csbNode:getChildByName("pay_bg")
    local bgsize = bg:getContentSize()

    itemname = itemname or ""
    price = price or 0

    --商品名称
    bg:getChildByName("text_name"):setString(itemname)

    --支付金额
    bg:getChildByName("text_price"):setString(price)

    --按钮回调
    local btcallback = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --返回按钮
    local btn = bg:getChildByName("btn_return")
    btn:setTag(JFT_RETURN)
    btn:addTouchEventListener(btcallback)

    --微信支付
    btn = bg:getChildByName("jft_pay3")
    btn:addTouchEventListener(btcallback)
    btn:setVisible(false)
    btn:setEnabled(false)

    --支付宝支付
    btn = bg:getChildByName("jft_pay4")
    btn:addTouchEventListener(btcallback)
    btn:setVisible(false)
    btn:setEnabled(false)

    local str = ""
    local tpos = btnpos[#paylist] or {}
    for k,v in pairs(paylist) do
        str = "jft_pay" .. v
        local paybtn = bg:getChildByName(str)
        if nil ~= paybtn then
            paybtn:setEnabled(true)
            paybtn:setVisible(true)
            paybtn:setTag(v)
            local pos = tpos[k]
            if nil ~= pos then
                paybtn:setPosition(cc.p(pos.x * bgsize.width, pos.y * bgsize.height))
            end
        end
    end

    --调起支付
    self.m_bCallPay = false
    --监听
    local function eventCall( event )
        if true == self.m_bCallPay then            
            self:queryUserScoreInfo()
        end
    end
    self.m_listener = cc.EventListenerCustom:create(yl.RY_JFTPAY_NOTIFY,handler(self, eventCall))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function JunFuTongPay:queryUserScoreInfo()
    if nil ~= self.m_parent.queryUserScoreInfo then
        self.m_parent:queryUserScoreInfo(function(needUpdate)
            if true == needUpdate then
                self.m_parent:updateScoreInfo()
                --重新请求支付列表
                self.m_parent:reloadBeanList()
            end
            GlobalUserItem.bJftPay = false
            self:removeFromParent()
        end)
    end
end

function JunFuTongPay:onTouchBegan(touch, event)
    return self:isVisible()
end

function JunFuTongPay:onExit()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end
end

function JunFuTongPay:onButtonClickedEvent(tag, sender)
	ExternalFun.playClickEffect()
    print(tag)
    if tag == JFT_RETURN then
        self.m_parent.m_bJunfuTongPay = false
        GlobalUserItem.bJftPay = false
        --重新请求支付列表
        self.m_parent:reloadBeanList()
        self:removeFromParent()
    else
        local plat = 0
        local str = ""
        if 3 == tag then
            plat = yl.ThirdParty.WECHAT
            str = "微信未安装,无法进行微信支付"
        elseif 4 == tag then
            plat = yl.ThirdParty.ALIPAY
            str = "支付宝未安装,无法进行支付宝支付"
        end
        --判断应用是否安装
        if false == MultiPlatform:getInstance():isPlatformInstalled(plat) then
            showToast(self, str, 2, cc.c4b(250,255,255,255))
            return
        end
        self.m_parent:showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
            self.m_parent:dismissPopWait()
            end)))
        local function payCallBack(param)
            --[[self.m_parent:dismissPopWait()
            if type(param) == "string" and "true" == param then
                GlobalUserItem.setTodayPay()
                
                showToast(self, "支付成功", 2)
                --更新用户游戏豆
                GlobalUserItem.dUserBeans = GlobalUserItem.dUserBeans + self.m_nCount
                --通知更新        
                local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                eventListener.obj = yl.RY_MSG_USERWEALTH
                cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                self:hide()
                --重新请求支付列表
                self.m_parent:reloadBeanList()
            else
                showToast(self, "支付异常", 2)
            end]]
        end
        self.m_bCallPay = true
        MultiPlatform:getInstance():thirdPartyPay(PAYTYPE[CBT_JFT].plat, {paytype = tag, token = self.m_token}, payCallBack)
    end
end

--支付选择页面
local ShopPay = class("ShopPay", cc.Layer)


local BT_CLOSE = 201
local BT_SURE = 202
local BT_CANCEL = 203

function ShopPay:ctor(parent)
	self.m_parent = parent
	--价格
	self.m_fPrice = 0.00
	--数量
	self.m_nCount = 0
    -- appid
    self.m_nAppId = 0

	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Shop/ShopPayLayer.csb", self)

	self.m_spBgKuang = csbNode:getChildByName("shop_pay_bg")
	self.m_spBgKuang:setScale(0.0001)
	local bg = self.m_spBgKuang

	--商品
	self.m_textProName = bg:getChildByName("text_name")
	self.m_textProName:setString("")

	--价格
	self.m_textPrice = bg:getChildByName("text_price")
	self.m_textPrice:setString("")

	--按钮回调
	local btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end 
    --关闭按钮
    local btn = bg:getChildByName("btn_close")
    btn:setTag(BT_CLOSE)
    btn:addTouchEventListener(btcallback)

    --确定按钮
    btn = bg:getChildByName("btn_sure")
    btn:setTag(BT_SURE)
    btn:addTouchEventListener(btcallback)

    --取消按钮
    btn = bg:getChildByName("btn_cancel")
    btn:setTag(BT_CANCEL)
    btn:addTouchEventListener(btcallback)

	local cbtlistener = function (sender,eventType)
    	self:onSelectedEvent(sender:getTag(),sender)
    end

    local enableList = {}
    --微信支付
    local cbt = bg:getChildByName("check_wechat")
    cbt:setTag(CBT_WECHAT)
    cbt:setSelected(true)
    cbt:addEventListener(cbtlistener)
    cbt:setVisible(false)
    cbt:setEnabled(false)
    self.m_cbtWeChat = cbt
    if yl.WeChat.PartnerID ~= " " and yl.WeChat.PartnerID ~= "" then
        table.insert(enableList, "check_wechat")
    end
    local wpos = cc.p(self.m_cbtWeChat:getPositionX(), self.m_cbtWeChat:getPositionY())
    local wtext = bg:getChildByName("check_wechat_t")
    wtext:setVisible(false)
    local wtpos = cc.p(wtext:getPositionX(), wtext:getPositionY())

    --支付宝支付
    cbt = bg:getChildByName("check_alipay")
    cbt:setTag(CBT_ALIPAY)
    cbt:setSelected(false)
    cbt:addEventListener(cbtlistener)
    cbt:setVisible(false)
    cbt:setEnabled(false)
    self.m_cbtAlipay = cbt
    if yl.AliPay.PartnerID ~= " " and yl.AliPay.PartnerID ~= "" then
        table.insert(enableList, "check_alipay")
    end
    local apos = cc.p(self.m_cbtAlipay:getPositionX(), self.m_cbtAlipay:getPositionY())
    local atext = bg:getChildByName("check_alipay_t")
    atext:setVisible(false)
    local atpos = cc.p(atext:getPositionX(), atext:getPositionY())

    --竣付通支付
    cbt = bg:getChildByName("check_jft")
    cbt:setTag(CBT_JFT)
    cbt:setSelected(false)
    cbt:addEventListener(cbtlistener)
    cbt:setVisible(false)
    cbt:setEnabled(false)
    self.m_cbtJft = cbt  
    if yl.JFT.PartnerID ~= " " and yl.JFT.PartnerID ~= "" then
        table.insert(enableList, "check_jft")
    end
    local jpos = cc.p(self.m_cbtJft:getPositionX(), self.m_cbtJft:getPositionY())
    local jtext = bg:getChildByName("check_jft_t")
    jtext:setVisible(false)
    local jtpos = cc.p(jtext:getPositionX(), jtext:getPositionY())

    local cbtPosition = 
    {   
        {wpos},
        {wpos, apos},
        {wpos, apos, jpos}
    }
    local textPosition = 
    {
        {wtpos},
        {wtpos, atpos},
        {wtpos, atpos, jtpos}
    }
    local poslist = cbtPosition[#enableList]
    local tposlist = textPosition[#enableList]
    for k,v in pairs(enableList) do
        local tmp = bg:getChildByName(v)
        if nil ~= tmp then
            tmp:setEnabled(true)
            tmp:setVisible(true)

            local pos = poslist[k]
            if nil ~= pos then
                tmp:setPosition(pos)
            end
        end
        tmp = bg:getChildByName(v .. "_t")
        if nil ~= tmp then
            tmp:setVisible(true)
            local pos = tposlist[k]
            if nil ~= pos then
                tmp:setPosition(pos)
            end
        end
    end

    self.m_select = nil
    if #enableList > 0 then
        local tmp = bg:getChildByName(enableList[1])
        if nil ~= tmp then
            tmp:setSelected(true)
            self.m_select = tmp:getTag()
        end
    end

	--加载动画
	self.m_actShowAct = cc.ScaleTo:create(0.2, 1.0)
	ExternalFun.SAFE_RETAIN(self.m_actShowAct)

	local scale = cc.ScaleTo:create(0.2, 0.0001)
	local call = cc.CallFunc:create(function( )
		self:showLayer(false)
	end)
	self.m_actHideAct = cc.Sequence:create(scale, call)
	ExternalFun.SAFE_RETAIN(self.m_actHideAct)

	self:showLayer(false)
end

function ShopPay:isPayMethodValid()
    if (yl.WeChat.PartnerID ~= " " and yl.WeChat.PartnerID ~= "") 
        or (yl.AliPay.PartnerID ~= " " and yl.AliPay.PartnerID ~= "")
        or (yl.JFT.PartnerID ~= " " and yl.JFT.PartnerID ~= "")
    then
        return true
    else
        return false
    end
end

function ShopPay:showLayer(var)
	self:setVisible(var)

	if true == var then
		self.m_spBgKuang:stopAllActions()
		self.m_spBgKuang:runAction(self.m_actShowAct)
	end
end

function ShopPay:refresh(count, name, sprice, fprice, appid)
	self.m_textProName:setString(name)
	self.m_textPrice:setString(sprice)

	self.m_fPrice = fprice
	self.m_nCount = count
    self.m_nAppId = appid
end

function ShopPay:onButtonClickedEvent(tag, sender)
	ExternalFun.playClickEffect()
	if tag == BT_CLOSE or tag == BT_CANCEL then
		self:hide()
	elseif tag == BT_SURE then
        if nil == self.m_select then
            return
        end

        local str = "无法支付"
        local plat = PAYTYPE[self.m_select].plat
        if yl.ThirdParty.WECHAT == PAYTYPE[self.m_select].plat then
            plat = yl.ThirdParty.WECHAT
            str = "微信未安装,无法进行微信支付"
        elseif yl.ThirdParty.ALIPAY == PAYTYPE[self.m_select].plat then
            plat = yl.ThirdParty.ALIPAY
            str = "支付宝未安装,无法进行支付宝支付"
        end
        --判断应用是否安装
        if false == MultiPlatform:getInstance():isPlatformInstalled(plat) and plat ~= yl.ThirdParty.JFT then
            showToast(self, str, 2, cc.c4b(250,255,255,255))
            return
        end
        
		self.m_parent:showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
            self.m_parent:dismissPopWait()
            end)))
		--生成订单
		local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
        local account = GlobalUserItem.dwGameID
        --[[if GlobalUserItem.bThirdPartyLogin then
            account = GlobalUserItem.thirdPartyData.szAccount
        end
        account = string.urlencode(account)]]
		local action = "action=CreatPayOrderID&gameid=" .. account .. "&amount=" .. self.m_fPrice .. "&paytype=" .. PAYTYPE[self.m_select].str .. "&appid=" .. self.m_nAppId
        --print(action)
		appdf.onHttpJsionTable(url,"GET",action,function(jstable,jsdata)
            dump(jstable, "jstable", 6)
			if type(jstable) == "table" then
				local data = jstable["data"]
				if type(data) == "table" then
					if nil ~= data["valid"] and true == data["valid"] then
						local payparam = {}
						if self.m_select == CBT_WECHAT then --微信支付
							--获取微信支付订单id
							local paypackage = data["PayPackage"]
							if type(paypackage) == "string" then
								local ok, paypackagetable = pcall(function()
					       			return cjson.decode(paypackage)
					    		end)
					    		if ok then
					    			local payid = paypackagetable["prepayid"]
					    			if nil == payid then
										showToast(self, "微信支付订单获取异常", 2)
										return 
									end
									payparam["info"] = paypackagetable
					    		else
					    			showToast(self, "微信支付订单获取异常", 2)
					    			return
					    		end
							end
						elseif self.m_select == CBT_JFT then --竣付通支付
                            self:onJunFuTongPay(data)
                            return
                        end
						--订单id
						payparam["orderid"] = data["OrderID"]						
						--价格
						payparam["price"] = self.m_fPrice
						--商品名
						payparam["name"] = self.m_textProName:getString()

						local function payCallBack(param)
							self.m_parent:dismissPopWait()
							if type(param) == "string" and "true" == param then
                                GlobalUserItem.setTodayPay()
                                self:emit("V2M_PaySuccess")
								showToast(self, "支付成功", 2)
								--更新用户游戏豆
								GlobalUserItem.dUserBeans = GlobalUserItem.dUserBeans + self.m_nCount
								--通知更新        
								local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
							    eventListener.obj = yl.RY_MSG_USERWEALTH
							    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                                self:hide()
                                --重新请求支付列表
                                self.m_parent:reloadBeanList()

                                self.m_parent:updateScoreInfo()
							else
								showToast(self, "支付异常", 2)
							end
						end
						MultiPlatform:getInstance():thirdPartyPay(PAYTYPE[self.m_select].plat, payparam, payCallBack)
					else
                        if type(jstable["msg"]) == "string" and jstable["msg"] ~= "" then
                            showToast(self, jstable["msg"], 2)
                        end
                    end
				end
			end
		end)
	end
end

function ShopPay:onSelectedEvent(tag, sender)
	if self.m_select == tag then
		self.m_spBgKuang:getChildByTag(tag):setSelected(true)
		return
	end

	self.m_select = tag

	for i=101,103 do
		if i ~= tag then
			self.m_spBgKuang:getChildByTag(i):setSelected(false)
		end
	end

	--微信支付
	if (tag == CBT_WECHAT) then
		print("wechat")
	end

	--支付宝
	if (tag == CBT_ALIPAY) then
		print("alipay")
	end

	--俊付通
	if (tag== CBT_JFT) then
		print("jft")
	end
end

function ShopPay:onTouchBegan(touch, event)
	return self:isVisible()
end

function ShopPay:onTouchEnded(touch, event)
	local pos = touch:getLocation();
	local m_spBg = self.m_spBgKuang
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:hide()
    end  
end

function ShopPay:onExit()
	ExternalFun.SAFE_RELEASE(self.m_actShowAct)
	self.m_actShowAct = nil
	ExternalFun.SAFE_RELEASE(self.m_actHideAct)
	self.m_actHideAct = nil
end

function ShopPay:onJunFuTongPay(data)
    --请求token
    local uid = yl.JFT["PartnerID"]
    local oid = data["OrderID"] or ""                            
    local mon = "" .. self.m_fPrice
    local rurl = yl.JFT["NotifyURL"]
    local nurl = yl.JFT["NotifyURL"]
    local tt = os.date("*t")
    local odrdertime = string.format("%d%02d%02d%02d%02d%02d", tt.year, tt.month, tt.day, tt.hour, tt.min, tt.sec)
    local sign_str = uid .. "&" .. oid .. "&" .. mon .. "&" .. rurl .. "&" .. nurl .. "&" .. odrdertime .. yl.JFT["PayKey"]
    local md5_signstr = md5(sign_str)
    local postdata = string.format("p1_usercode=%s&p2_order=%s&p3_money=%s&p4_returnurl=%s&p5_notifyurl=%s&p6_ordertime=%s&p7_sign=%s&p9_paymethod=SDK&p24_remark=2045", 
        uid,
        oid,
        mon,
        rurl,
        nurl,
        odrdertime,
        md5_signstr)
    appdf.onHttpJsionTable(yl.JFT["TokenURL"],"GET",postdata,function(tokentable,tokendata)
        self.m_parent:dismissPopWait()
        dump(tokentable, "tokentable", 6)
        local msg = "竣付通token获取失败"
        if type(tokentable) == "table" then
            local flag = tokentable["flag"] or "0"
            if flag == "1" then
                msg = nil
                local token = tokentable["token"] or ""
                --获取支付列表
                self.m_parent:showPopWait()
                MultiPlatform:getInstance():getPayList(token, function(listjson)
                    self.m_parent:dismissPopWait()
                    if type(listjson) == "string" and "" ~= listjson then
                        local ok, listtable = pcall(function()
                            return cjson.decode(listjson)
                        end)
                        if ok then
                            dump(listtable, "listtable", 6)
                            -- typeid=3 为微信， typeid=4为支付宝
                            local itemname = self.m_textProName:getString()
                            local itemprice = self.m_textPrice:getString()
                            local jft = JunFuTongPay:create(self.m_parent, itemname, itemprice, listtable, token)
                            self.m_parent:addChild(jft)
                            self:hide()
                        end
                    end                                            
                end)      
            end                                    
        end
        
        if type(msg) == "string" and "" ~= msg then
            showToast(self, msg, 3, cc.c4b(250,255,255,255))
        end        
    end)
end

function ShopPay:hide()
    self.m_spBgKuang:stopAllActions()
    self.m_spBgKuang:runAction(self.m_actHideAct)
end

--商城页面
local ShopLayer = class("ShopLayer", cc.BaseLayer)

--WEB
ShopLayer.TYPEID_WEB        = 1
--金币
ShopLayer.TYPEID_SCORE		= 5
--金豆
ShopLayer.TYPEID_BEAN		= 6
--VIP
ShopLayer.TYPEID_VIP		= 7
--道具
ShopLayer.TYPEID_PROPERTY	= 8
--实物兑换
ShopLayer.TYPEID_ENTITY		= 9
--代理充值
ShopLayer.TYPEID_AGENT_RECHARGE		= 10
--虚拟物品兑换
ShopLayer.TYPEID_VIRTUAL		= 11


-- 支付模式
local APPSTOREPAY = 10 -- iap支付
local THIRDPAY = 20 -- 第三方支付

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")

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
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		local musicPath = string.format("sound/shop_%d.mp3", math.random(1,2))
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, musicPath)
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function ShopLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:play("closeAni", false)
end

function ShopLayer:ctor(stmod)
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
	
	stmod = stmod or ShopLayer.TYPEID_BEAN
	GlobalUserItem.tabShopCache =  GlobalUserItem.tabShopCache or {}
    self.m_nPayMethod = GlobalUserItem.tabShopCache["nPayMethod"] or THIRDPAY

    self._selectType = stmod
	--金币购买列表
	self._scoreList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_SCORE] or {}
	--道具购买列表
    self._propertyList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_PROPERTY] or {}
	--VIP购买列表
    self._vipList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_VIP] or {}
    --游戏豆购买列表
    self._beanList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_BEAN] or {}
    --实物兑换页面
    self._goodsList = nil
	--代理充值列表
	self._agentRechargeList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_AGENT_RECHARGE] or {}
	--虚拟物品兑换购买列表
	self._virtualGoodsList = GlobalUserItem.tabShopCache[ShopLayer.TYPEID_VIRTUAL] or {}
    --商城左侧商品类型条目
    self._shopTypeIdList = GlobalUserItem.tabShopCache["shopTypeIdList"] or {}
    --购买界面
    self.m_payLayer = nil
    --购买汇率
    self.m_nRate = GlobalUserItem.tabShopCache["shopRate"] or 0
    --竣付通支付界面
    self.m_bJunfuTongPay = false
    --道具关联信息
    self.m_tabPropertyRelate = GlobalUserItem.tabShopCache["propertyRelate"] or {}
	
--[[    --左侧单选按钮
    self._radioButtonGroup = ccui.RadioButtonGroup:create()
    self._radioButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioButtonGroup)

    self:initRadioButton(self._shopTypeIdList)--]]
end

--初始化主界面
function ShopLayer:initUI()
	local bg = self._csbNode:getChildByName("bg")
	local top_bg = bg:getChildByName("info_top_bg")
	

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

  
 	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	
	--左侧按钮菜单
	local leftMenu = bg:getChildByName("left_menu")
	--左侧单选按钮组
    self._radioLeftMenuButtonGroup = ccui.RadioButtonGroup:create()
    self._radioLeftMenuButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioLeftMenuButtonGroup)		
	--商城
	local rto_container_1 = leftMenu:getChildByName("radio_container_1")
	local rto_container_1_size = rto_container_1:getContentSize()
	self.m_rtoShop = ccui.RadioButton:create("btn_shop_coin_0.png","btn_shop_coin_0.png","btn_shop_coin_1.png","btn_shop_coin_0.png","btn_shop_coin_0.png", ccui.TextureResType.plistType)
	self.m_rtoShop:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoShop:setPosition(rto_container_1_size.width / 2, rto_container_1_size.height / 2)
	self.m_rtoShop:setName("rto_bank_shop")
	rto_container_1:addChild(self.m_rtoShop)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoShop)
		
	--代理
	local rto_container_2 = leftMenu:getChildByName("radio_container_2")
	local rto_container_2_size = rto_container_2:getContentSize()
	self.m_rtoDaili = ccui.RadioButton:create("btn_shop_daili_0.png","btn_shop_daili_0.png","btn_shop_daili_1.png","btn_shop_daili_0.png","btn_shop_daili_0.png", ccui.TextureResType.plistType)
	self.m_rtoDaili:addEventListener(handler(self, self.onSelectedEvent))	
	self.m_rtoDaili:setPosition(rto_container_2_size.width / 2, rto_container_2_size.height / 2)
	self.m_rtoDaili:setName("rto_bank_daili")
	rto_container_2:addChild(self.m_rtoDaili)
	self._radioLeftMenuButtonGroup:addRadioButton(self.m_rtoDaili)
	

	--代理
	local rto_container_3 = leftMenu:getChildByName("radio_container_3")
	local rto_container_3_size = rto_container_3:getContentSize()
	
	local rto_container_4 = leftMenu:getChildByName("radio_container_4")
	local rto_container_4_size = rto_container_4:getContentSize()

	
	local rto_container_5 = leftMenu:getChildByName("radio_container_5")
	local rto_container_5_size = rto_container_5:getContentSize()
	
	self:initShopUI()
	self:initAgentUI()
end

function ShopLayer:initShopUI()
	if 0 == table.nums(self._shopTypeIdList) then
        --请求商城信息
		self:getShopInfo()
    else
        --刷新界面显示
        self:updateShopUI()
	end	
    return self
end

function ShopLayer:initAgentUI()

end

function ShopLayer:updateShopUI()
	local curShopList = GlobalUserItem.tabShopCache[self._selectType]
	if (curShopList == nil) then
		--请求代理充值数据
		if (self._selectType == ShopLayer.TYPEID_AGENT_RECHARGE) then
			self:requestAgentPayList()
		--请求充值列表数据（金豆）
		elseif (self._selectType == ShopLayer.TYPEID_BEAN) then
			self:requestPayList()
		--请求VIP数据
		elseif (self._selectType == ShopLayer.TYPEID_VIP) then
			self:requestPropItemByTypeID(self._selectType)
		--请求道具数据
		elseif (self._selectType == ShopLayer.TYPEID_PROPERTY) then
			self:requestPropItemByTypeID(self._selectType)
		--请求金币数据
		elseif (self._selectType == ShopLayer.TYPEID_SCORE) then
			self:requestPropItemByTypeID(self._selectType)
		--不支持的类型
		else
			error("shop not support typeid" .. self._selectType .. "yet")
		end
		return
	else
		--更新界面
		self:onUpdateShowList(self._selectType)
	end
end

function ShopLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function ShopLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

function ShopLayer:getShopInfo()
    self:showPopWait()
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobilePropertyType",function(jstable,jsdata)
		self:dismissPopWait()
        --dump(jstable, "jstable", 6)
        if type(jstable) == "table" then
			local data = jstable["data"]
			if type(data) == "table" then
				if nil ~= data["valid"] and true == data["valid"] then
					local list = data["list"]
					if type(list) == "table" then
						local index = 1
						for k,v in pairs(list) do
							self._shopTypeIdList[index] = v
							index = index + 1
						end
                        --更新全局数据
                        GlobalUserItem.tabShopCache["shopTypeIdList"] = self._shopTypeIdList
                        --刷新界面显示
                        self:updateShopUI()
						return
					end					
				end
			end

			local msg = jstable["msg"]
			if type(msg) == "string" then
				showToast(self, msg, 2)
			end
		end
	end)

end

--获取父场景节点(ClientScene)
function ShopLayer:getRootNode()
	return self:getParent():getParent():getParent():getParent()
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
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_APPSTORE_LAYER})
	end
end

function ShopLayer:onRechargeClickedEvent(sender, itemInfo)
	ExternalFun.playClickEffect()
    assert(type(itemInfo) == "table", "expected table data, but not now!")
    if (type(itemInfo) ~= "table") then
        return
    end

	local typeid = sender:getTag()

	if typeid == ShopLayer.TYPEID_SCORE then
		--游戏币获取
		GlobalUserItem.buyItem = itemInfo
		local url = string.format("%s/Mobile/pay.aspx?account=%s&amount=%d", BaseConfig.WEB_HTTP_URL, GlobalUserItem.szAccount, itemInfo.cash)
		cc.Application:getInstance():openURL(url)
--        if GlobalUserItem.buyItem.id == "game_score" and PriRoom then
--            self:getParent():getParent():onChangeShowMode(PriRoom.LAYTAG.LAYER_EXCHANGESCORE, GlobalUserItem.buyItem.resultGold)
--        else
--            self:getParent():getParent():onChangeShowMode(yl.SCENE_SHOPDETAIL)
--        end
	elseif typeid == ShopLayer.TYPEID_BEAN then
		--游戏豆获取
        local item = itemInfo
        if nil == item then
            return
        end
        local bThirdPay = true

        if BaseConfig.APPSTORE_VERSION
            and (targetPlatform == cc.PLATFORM_OS_IPHONE or targetPlatform == cc.PLATFORM_OS_IPAD) then
            if self.m_nPayMethod == APPSTOREPAY then
                bThirdPay = false
                local payparam = {}
                payparam.http_url = BaseConfig.WEB_HTTP_URL
                payparam.uid = GlobalUserItem.dwUserID
                payparam.productid = item.nProductID
                payparam.price = item.price

                self:showPopWait()
                self:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
                    self:dismissPopWait()
                end)))
                showToast(self, "正在连接iTunes Store...", 4)
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
                        showToast(self, "支付异常", 2)
                    end
                end
                MultiPlatform:getInstance():thirdPartyPay(yl.ThirdParty.IAP, payparam, payCallBack)
            end
        end

        if bThirdPay then
            if ShopPay:isPayMethodValid() then
				if nil == self.m_payLayer then
					self.m_payLayer = ShopPay:create(self)
					self:addChild(self.m_payLayer)
				end            
				local sprice = string.format("%.2f元", item.price)
				self.m_payLayer:refresh(item.count, item.name, sprice, item.price, item.appid)
				self.m_payLayer:showLayer(true)
			else
				local account = string.urlencode(GlobalUserItem.szAccount)
				local url = string.format("%s/Mobile/Pay.aspx?account=%s&amount=%d", BaseConfig.WEB_HTTP_URL, account, itemInfo.price)
				cc.Application:getInstance():openURL(url)
            end

        end			
	elseif typeid == ShopLayer.TYPEID_VIP then
		--vip购买
		GlobalUserItem.buyItem = self._vipList[ShopLayer.TYPEID_VIP]
		self:getRootNode():onChangeShowMode(yl.SCENE_SHOPDETAIL)
	elseif typeid == ShopLayer.TYPEID_PROPERTY then
		--道具购买
		GlobalUserItem.buyItem = self._propertyList[ShopLayer.TYPEID_PROPERTY]
        if GlobalUserItem.buyItem.id == "room_card" and PriRoom then
            self:getRootNode():onChangeShowMode(PriRoom.LAYTAG.LAYER_BUYCARD, GlobalUserItem.buyItem)
        else
            self:getRootNode():onChangeShowMode(yl.SCENE_SHOPDETAIL)
        end	
	elseif typeid == ShopLayer.TYPEID_AGENT_RECHARGE then
		local agentRechargeLayer = AgentRechargeLayer:create(itemInfo)
		if (nil ~= agentRechargeLayer) then
			self:addChild(agentRechargeLayer)
		end
	end
end

function ShopLayer:onSelectedEvent(sender,eventType)
    if (ccui.RadioButtonEventType.selected == eventType) then
		if (sender:getName() == "rto_bank_shop") then
			self._csbNodeAni:play("show_shop", false)
			self._selectType=ShopLayer.TYPEID_BEAN
			if 0 == #self._scoreList then
				self:updateShopUI() 
			else
				self:onUpdateScore()
			end
		elseif (sender:getName() == "rto_bank_daili") then
			self._csbNodeAni:play("show_agent", false)
			self._selectType=ShopLayer.TYPEID_AGENT_RECHARGE
			if (#self._agentRechargeList==0) then
				self:updateShopUI() 
			else
				self:onUpdateRecharge()
			end
		end
    elseif (ccui.RadioButtonEventType.unselected == eventType) then
		
    end
end

function ShopLayer:requestAgentPayList()
    local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
    self:showPopWait()
    appdf.onHttpJsionTable(url ,"GET","action=getagentpay",function(jstable,jsdata)
		dump(jstable, "jstable", 7)
		self:dismissPopWait()
		local errmsg = ""
		if type(jstable) == "table" then
			local code = jstable["code"]
			if tonumber(code) == 0 then
				local datax = jstable["data"]
				local msg = jstable["msg"]
				errmsg = ""
				if type(msg) == "string" then
					errmsg = msg
				end
				
				if datax then
					local valid = datax["valid"]
					if valid == true then
						local list = datax["list"]
						if type(list) == "table" then
							self._agentRechargeList = {}
							for i=1,#list do
								local item = {}
								item.qq = tonumber(list[i]["QQ"])
								item.name = list[i]["ShowName"]
								item.wechat = list[i]["WeChat"]
								table.insert(self._agentRechargeList, item)
							end
							GlobalUserItem.tabShopCache[ShopLayer.TYPEID_AGENT_RECHARGE] = self._agentRechargeList
							self:onUpdateAgentList()
						end
					end
				end
			end
		end
		
		if type(errmsg) == "string" and "" ~= errmsg then
            showToast(self,errmsg,2,cc.c3b(250,0,0))
        end 
    end)
end

function ShopLayer:requestPayList()
    local isIap = 0
	
	local function getPayList()
		--获取支付数据
		local beanurl = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
		local ostime = os.time()
		self:showPopWait()
		
		local tagId = 0
		if (appConfigProxy._appStoreSwitch == 0) then
			tagId=1 --android充值
		else
			tagId=2 --IOS内购
		end
		
		appdf.onHttpJsionTable(beanurl ,"GET","action=GetPayProduct&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime) .. "&typeID=" .. isIap .. "&tagId=" .. tagId,function(sjstable,sjsdata)
			--dump(sjstable, "支付列表", 6)
			local errmsg = "获取支付列表异常!"
			self:dismissPopWait()
			if type(sjstable) == "table" then
				local sjdata = sjstable["data"]
				local msg = sjstable["msg"]
				errmsg = nil
				if type(msg) == "string" then
					errmsg = msg
				end
				
				if type(sjdata) == "table" then
					local isFirstPay = sjdata["IsPay"] or "0"
					isFirstPay = tonumber(isFirstPay)
					local sjlist = sjdata["list"]
					if type(sjlist) == "table" then
						--置空
						self._beanList = {}
						for i = 1, #sjlist do
							local sitem = sjlist[i]
							local item = {}
							item.price = sitem["Price"]
							item.isfirstpay = isFirstPay
							item.paysend = sitem["AttachCurrency"] or "0"
							item.paysend = tonumber(item.paysend)
							item.paycount = sitem["PresentCurrency"] or "0"
							item.paycount = tonumber(item.paycount)
							item.price = tonumber(item.price)
							item.count = item.paysend + item.paycount
							item.description  = sitem["Description"]                                        
							item.name = sitem["ProductName"]
							item.sortid = tonumber(sitem["SortID"]) or 0
							item.appid = tonumber(sitem["AppID"])
							item.nProductID = sitem["ProductID"] or ""

							--首充赠送
							if 0 ~= item.paysend then
								--当日未首充
								if 0 == isFirstPay then
									item.nOrder = 1
									table.insert(self._beanList, item)
								end
							else
								table.insert(self._beanList, item)
							end                                             
						end
						table.sort(self._beanList, function(a,b)
								return a.sortid < b.sortid
							end)
						GlobalUserItem.tabShopCache[ShopLayer.TYPEID_BEAN] = self._beanList
						self:onUpdateBeanList()
					end
				end
			end

			if type(errmsg) == "string" and "" ~= errmsg then
				showToast(self,errmsg,2,cc.c3b(250,0,0))
			end 
		end)
	end
	
	-- 内购开关判断
	if BaseConfig.APPSTORE_VERSION and (targetPlatform == cc.PLATFORM_OS_IPHONE or targetPlatform == cc.PLATFORM_OS_IPAD) then
		if (appConfigProxy._appStoreSwitch ~= 0) then
			GlobalUserItem.tabShopCache["nPayMethod"] = APPSTOREPAY
			self.m_nPayMethod = APPSTOREPAY
		end
		getPayList()
	else
		-- 请求列表
		getPayList()
	end
end

--请求对应类型的数据条目
function ShopLayer:requestPropItemByTypeID(nTypeID)
	assert(type(nTypeID) == "number", "expected number data, but not now!")
    if (type(nTypeID) ~= "number") then
        return
    end
	
	--金豆API特殊处理

	self:showPopWait()
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobileProperty&TypeID=" .. nTypeID,function(jstable,jsdata)
		self:dismissPopWait()
		dump(jstable, "jstable", 7)
		if type(jstable) == "table" then
			local code = jstable["code"]
			local tmpList = {}

			if tonumber(code) == 0 then
				local datax = jstable["data"]
				if datax then
					local valid = datax["valid"]
					if valid == true then
						local listcount = datax["total"]
						local list = datax["list"]
						if type(list) == "table" then
							for i=1,#list do
								local item = {}
								item.id = tonumber(list[i]["ID"])
								item.name = list[i]["Name"]
								item.cash = tonumber(list[i]["Cash"])
								item.gold = tonumber(list[i]["Gold"])
								item.ingot = tonumber(list[i]["UserMedal"])
								item.loveliness = tonumber(list[i]["LoveLiness"])
								item.resultGold = tonumber(list[i]["UseResultsGold"])
								item.description = list[i]["RegulationsInfo"]
								item.sortid = tonumber(list[i]["SortID"] or "0")
								item.minPrice = tonumber(list[i]["minPrice"]) or 0

								if (item.loveliness ~= 0) and (item.cash == 0 and item.gold == 0 and item.ingot == 0) then

								else
									if nTypeID == ShopLayer.TYPEID_PROPERTY and item.id == 501 then
										if GlobalUserItem.bEnableRoomCard then
											item.id = "room_card"
											table.insert(tmpList, item)
										end
									elseif nTypeID == ShopLayer.TYPEID_SCORE and item.id == 501 and 0 ~= item.resultGold then
										if GlobalUserItem.bEnableRoomCard then
											item.id = "game_score"
											item.minPrice = item.resultGold
											item.cash = 0
											item.gold = 0
											item.ingot = 0
											item.loveliness = 0
											table.insert(tmpList, item)
										end                                        
									else
										table.insert(tmpList, item)
									end
								end                                 
							end
						end
					end
				end
			end

			--产品排序
			table.sort(tmpList, function(a,b)
				return a.sortid < b.sortid
			end)
			
			if nTypeID == ShopLayer.TYPEID_VIP then
				GlobalUserItem.tabShopCache[ShopLayer.TYPEID_VIP] = tmpList
				self._vipList = tmpList
			elseif nTypeID == ShopLayer.TYPEID_PROPERTY then               
				GlobalUserItem.tabShopCache[ShopLayer.TYPEID_PROPERTY] = tmpList
				self._propertyList = tmpList
			elseif nTypeID == ShopLayer.TYPEID_SCORE then         
				GlobalUserItem.tabShopCache[ShopLayer.TYPEID_SCORE] = tmpList
				self._scoreList = tmpList
			end
			
			self:onUpdateShowList(nTypeID)
		else
			showToast(self,"抱歉，获取道具信息失败！",2,cc.c3b(250,0,0))
		end
	end)
end

function ShopLayer:reloadBeanList()
    GlobalUserItem.tabShopCache[ShopLayer.TYPEID_BEAN] = {}
    self._beanList = {}
end

function ShopLayer:updateScoreInfo()
   self._txtGold:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
   self._txtBean:setString(string.formatNumberThousands(GlobalUserItem.dUserBeans,true,"/"))
   self._txtIngot:setString(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"))
end

--更新游戏币
function ShopLayer:onUpdateScore()
	self:onUpdateShowList(ShopLayer.TYPEID_SCORE)
end

--更新游戏豆
function ShopLayer:onUpdateBeanList()
	self:onUpdateShowList(ShopLayer.TYPEID_BEAN)
end

--更新代理信息
function ShopLayer:onUpdateAgentList()
	self:onUpdateShowList(ShopLayer.TYPEID_AGENT_RECHARGE)
end

--更新VIP
function ShopLayer:onUpdateVIP()
	self:onUpdateShowList(ShopLayer.TYPEID_VIP)
end

--更新道具
function ShopLayer:onUpdateProperty()
	self:onUpdateShowList(ShopLayer.TYPEID_PROPERTY)
end

--更新实物兑换列表
function ShopLayer:onUpdateGoodsList()
	local url = BaseConfig.WEB_HTTP_URL .. "/SyncLogin.aspx?userid=" .. GlobalUserItem.dwUserID .. "&time=".. os.time() .. "&signature="..GlobalUserItem:getSignature(os.time()).."&url=/Mobile/Shop/Goods.aspx"
	if nil == self._goodsList then
		--平台判定
		local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
			self._goodsList = ccexp.WebView:create()
		    self._goodsList:setPosition(cc.p(805,324))
		    self._goodsList:setContentSize(cc.size(938,520))
		    
            self._goodsList:setJavascriptInterfaceScheme("ryweb")
		    self._goodsList:setScalesPageToFit(true)
		    self._goodsList:setOnJSCallback(function ( sender, url )
                self:queryUserScoreInfo(function(ok)
                    if ok then
                        self:updateScoreInfo()
                        self._goodsList:reload()
                    end
                end)
		    end)

		    self._goodsList:setOnDidFailLoading(function ( sender, url )
		    	self:dismissPopWait()
		    	print("open " .. url .. " fail")
		    end)
		    self._goodsList:setOnShouldStartLoading(function(sender, url)
		        print("onWebViewShouldStartLoading, url is ", url)	        
		        return true
		    end)
		    self._goodsList:setOnDidFinishLoading(function(sender, url)
		    	self:dismissPopWait()
                ExternalFun.visibleWebView(self._goodsList, true)
		        print("onWebViewDidFinishLoading, url is ", url)
		    end)
		    self:addChild(self._goodsList)
		end
	end

	if nil ~= self._goodsList then
		self:showPopWait()
        ExternalFun.visibleWebView(self._goodsList, false)
		self._goodsList:loadURL(url)
	end
end

function ShopLayer:onUpdateRecharge()
	self:onUpdateShowList(ShopLayer.TYPEID_AGENT_RECHARGE)
end

function ShopLayer:onUpdateVirtualGoods()
	self:onUpdateShowList(ShopLayer.TYPEID_VIRTUAL)
end

--更新当前显示
function ShopLayer:onUpdateShowList(typeid)
    --check parm
	local bg = self._csbNode:getChildByName("bg")
    assert(type(self._csbNode) == "userdata", "expected userdata data, but not now!")
    if (type(self._csbNode) ~= "userdata") then
        return
    end

    assert(type(typeid) == "number", "expected number data, but not now!")
    if (type(typeid) ~= "number") then
        return
    end

	if (self._selectType==ShopLayer.TYPEID_SCORE) then

    elseif (self._selectType==ShopLayer.TYPEID_BEAN) then
		local layout_shop = bg:getChildByName("layout_shop")
		local listView = layout_shop:getChildByName("ListView_1");
		assert(type(listView) == "userdata", "expected userdata data, but not now!")
		if (type(listView) ~= "userdata") then
			return
		end

		listView:removeAllItems()
		
        local itemLayoutTemplate = layout_shop:getChildByName("ScoreItemLayout")
        local itemLayoutTemplateChildrens = itemLayoutTemplate:getChildren()
	    
        local itemLayout = nil

		local MAX_SCORE_LOGO = 6
		
        for i=1,#self._beanList do
		    local itemInfo = self._beanList[i]
            local index = (i - 1) % #itemLayoutTemplateChildrens;
            
            if (index == 0) then
                itemLayout = itemLayoutTemplate:clone()
                listView:pushBackCustomItem(itemLayout)
            end

            assert(type(itemLayout) == "userdata", "expected userdata data, but not now!")
            if (type(itemLayout) ~= "userdata") then
                return
            end

            local btn = itemLayout:getChildByName(string.format("shop_item_%d", index))
		    btn :setVisible(true)
				:setTag(typeid)
			    :addClickEventListener(function(ref)
					self:onRechargeClickedEvent(ref, itemInfo)
                end)

            --获取的总金币
            local top_item = btn:getChildByName("top_item")
			
            local text_score = btn:getChildByName("score")
			text_score:setString(string.format("金币x%d", itemInfo.count))
			
			local text_rmb = btn:getChildByName("rmb")
			text_rmb:setString(string.format("%d元", itemInfo.price))
			
			local img_score = btn:getChildByName("Image_1")
			
			local frame_name = string.format("shop_item_icon%d.png", i)
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(frame_name)
			if (frame) then
				img_score:loadTexture(frame_name, ccui.TextureResType.plistType)
			else
				img_score:loadTexture(string.format("shop_item_icon%d.png", MAX_SCORE_LOGO), ccui.TextureResType.plistType)
			end
        end
    elseif (self._selectType==ShopLayer.TYPEID_VIP) then
		for i=1,#self._vipList do
			
		end
    elseif (self._selectType==ShopLayer.TYPEID_PROPERTY) then
		for i=1,#self._propertyList do
			
		end
    elseif (self._selectType==ShopLayer.TYPEID_ENTITY) then
		for i=1,#self._goodsList do
			
		end
    elseif (self._selectType==ShopLayer.TYPEID_AGENT_RECHARGE) then
		local layout_agent = bg:getChildByName("layout_agent")
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
		for i=1,#self._agentRechargeList do
			local itemInfo = self._agentRechargeList[i]
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
		    btn :setTag(typeid)
                :setVisible(true)
			    :addTouchEventListener(function(ref, type)
                    if type == ccui.TouchEventType.ended then
                        self:onAgentButtonClickedEvent(ref, itemInfo)
        	        end
                end)
			local text = btn:getChildByName("Text_6")
			text:setString(itemInfo.name .. "\n" .. "微信:" .. itemInfo.wechat .. "\n" .. "QQ:" .. itemInfo.qq)
		end
    elseif (self._selectType==ShopLayer.TYPEID_VIRTUAL) then
		for i=1,#self._virtualGoodsList do
			
		end
    end
end

function ShopLayer:onKeyBack()
    if nil ~= self.m_payLayer then
        if true == self.m_payLayer:isVisible() then
            return true
        end
        
        if true == self.m_bJunfuTongPay then
            return true
        end
    end
    return false
end

function ShopLayer:queryUserScoreInfo(queryCallback)
    if nil ~= self._scene.queryUserScoreInfo then
        self._scene:queryUserScoreInfo(queryCallback)
    end
end

return ShopLayer