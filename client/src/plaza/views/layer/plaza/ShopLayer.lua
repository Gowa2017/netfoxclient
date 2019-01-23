--[[
	商城界面
	2016_06_28 Ravioyla

    包含 JunFuTongPay ShopPay ShopLayer 三个类
]]


local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local ClientConfig = appdf.req(appdf.BASE_SRC .."app.models.ClientConfig")

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
            showToast(self, str, 2, cc.c4b(250,0,0,255))
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
            showToast(self, str, 2, cc.c4b(250,0,0,255))
            return
        end
        
		self.m_parent:showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
            self.m_parent:dismissPopWait()
            end)))
		--生成订单
		local url = yl.HTTP_URL .. "/WS/MobileInterface.ashx"
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
            showToast(self, msg, 3, cc.c4b(250,0,0,255))
        end        
    end)
end

function ShopPay:hide()
    self.m_spBgKuang:stopAllActions()
    self.m_spBgKuang:runAction(self.m_actHideAct)
end

--商城页面
local ShopLayer = class("ShopLayer", function(scene)
		local shopLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return shopLayer
end)

ShopLayer.CBT_SCORE			= 1
ShopLayer.CBT_BEAN			= 2
ShopLayer.CBT_VIP			= 3
ShopLayer.CBT_PROPERTY		= 4
ShopLayer.CBT_ENTITY		= 5

ShopLayer.BT_SCORE			= 30
ShopLayer.BT_VIP			= 50
ShopLayer.BT_PROPERTY		= 60
ShopLayer.BT_GOODS			= 120
ShopLayer.BT_BEAN			= 520

ShopLayer.BT_ORDERRECORD    = 1001
ShopLayer.BT_BAG            = 1002

local SHOP_BUY = {}
SHOP_BUY[ShopLayer.BT_SCORE] = "shop_score_buy"
SHOP_BUY[ShopLayer.BT_BEAN] = "shop_bean_buy"
SHOP_BUY[ShopLayer.BT_VIP] = "shop_vip_buy"
SHOP_BUY[ShopLayer.BT_PROPERTY] = "shop_prop_buy"
SHOP_BUY[ShopLayer.BT_GOODS] = "shop_goods_buy"

-- 支付模式
local APPSTOREPAY = 10 -- iap支付
local THIRDPAY = 20 -- 第三方支付

-- 进入场景而且过渡动画结束时候触发。
function ShopLayer:onEnterTransitionFinish()
	--self:loadPropertyAndVip(self._select)    
	if 0 == table.nums(self._shopTypeIdList) then        
		self:getShopPropertyType()
    else
        --刷新界面显示
        self:updateCheckBoxList()
	end	
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ShopLayer:onExitTransitionStart()
    return self
end

--scene
--stmod 进入商店后的选择类型
function ShopLayer:ctor(scene, stmod)
	stmod = stmod or ShopLayer.CBT_SCORE
    self.m_nPayMethod = GlobalUserItem.tabShopCache["nPayMethod"] or THIRDPAY
	
	local this = self
	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		end
	end)

	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender:getTag(),sender,eventType)
    end

    self._select = stmod

	self._showList = {}

	self._scoreList = GlobalUserItem.tabShopCache["shopScoreList"] or {}
    self._propertyList = GlobalUserItem.tabShopCache["shopPropertyList"] or {}
    self._vipList = GlobalUserItem.tabShopCache["shopVipList"] or {}
    --游戏豆购买列表
    self._beanList = GlobalUserItem.tabShopCache["shopBeanList"] or {}
    --实物兑换页面
    self._goodsList = nil
    --商店物品typeid
    self._shopTypeIdList = GlobalUserItem.tabShopCache["shopTypeIdList"] or {}
    --购买界面
    self.m_payLayer = nil
    --购买汇率
    self.m_nRate = GlobalUserItem.tabShopCache["shopRate"] or 0
    --竣付通支付界面
    self.m_bJunfuTongPay = false
    --道具关联信息
    self.m_tabPropertyRelate = GlobalUserItem.tabShopCache["propertyRelate"] or {}

    display.newSprite("Shop/frame_shop_0.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,320)
		self:addChild(sp)
	end
	
    --返回按钮
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)

    --兑换记录
    local topBtn = ccui.Button:create("Information/btn_ubag_0.png","Information/btn_ubag_1.png")
    topBtn:move(yl.WIDTH - 90,yl.HEIGHT-51)
        :addTo(self)
        :setTag(ShopLayer.BT_BAG)
        :addTouchEventListener(function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    local tag = ref:getTag()
                    if tag == ShopLayer.BT_ORDERRECORD then
                        this._scene:onChangeShowMode(yl.SCENE_ORDERRECORD)
                    elseif tag == ShopLayer.BT_BAG then
                        this._scene:onChangeShowMode(yl.SCENE_BAG)
                    end                    
                end
            end)
    self.m_btnTopBtn = topBtn


	--金币、游戏豆、元宝
	display.newSprite("Shop/frame_shop_2.png")
		:move(365,699)
		:addTo(self)
	display.newSprite("Shop/icon_shop_0.png")
		:move(247,699)
		:addTo(self)
	display.newSprite("Shop/frame_shop_3.png")
		:move(679,699)
		:addTo(self)
	display.newSprite("Shop/icon_shop_1.png")
		:move(555,699)
		:addTo(self)
	display.newSprite("Shop/frame_shop_4.png")
		:move(1000,699)
		:addTo(self)
	display.newSprite("Shop/icon_shop_2.png")
		:move(877,699)
		:addTo(self)

	self._txtGold = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"), "Shop/num_shop_0.png", 16, 22, string.byte(".")) 
    		:move(293,699)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self)
    self._txtBean = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.dUserBeans,true,"/"), "Shop/num_shop_0.png", 16, 22, string.byte(".")) 
    		:move(603,699)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self)
    self._txtIngot = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"), "Shop/num_shop_0.png", 16, 22, string.byte(".")) 
    		:move(924,699)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self)

    display.newSprite("Shop/frame_shop_5.png")
    	:move(806,320)
    	:addTo(self)
    display.newSprite("Shop/frame_shop_6.png")
    	:move(178,320)
    	:addTo(self)

    --游戏币
    ccui.CheckBox:create("Shop/bt_shop_0_0.png","","Shop/bt_shop_0_1.png","","")
		:move(190,530)
		:addTo(self)
		:setSelected(false)
        :setVisible(false)
        :setEnabled(false)
        :setName("check" .. 5)
		:setTag(ShopLayer.CBT_SCORE)
		:addEventListener(cbtlistener)

	--游戏豆
    ccui.CheckBox:create("Shop/bt_shop_1_0.png","","Shop/bt_shop_1_1.png","","")
		:move(190,530-104)
		:addTo(self)
		:setSelected(false)
        :setVisible(false)
        :setEnabled(false)
        :setName("check" .. 6)
		:setTag(ShopLayer.CBT_BEAN)
		:addEventListener(cbtlistener)

	--VIP
    ccui.CheckBox:create("Shop/bt_shop_2_0.png","","Shop/bt_shop_2_1.png","","")
		:move(190,530-104*2)
		:addTo(self)
		:setSelected(false)
        :setVisible(false)
        :setEnabled(false)
        :setName("check" .. 7)
		:setTag(ShopLayer.CBT_VIP)
		:addEventListener(cbtlistener)

	--道具
    ccui.CheckBox:create("Shop/bt_shop_3_0.png","","Shop/bt_shop_3_1.png","","")
		:move(190,530-104*3)
		:addTo(self)
		:setSelected(false)
        :setVisible(false)
        :setEnabled(false)
        :setName("check" .. 8)
		:setTag(ShopLayer.CBT_PROPERTY)
		:addEventListener(cbtlistener)

	--实物
    ccui.CheckBox:create("Shop/bt_shop_4_0.png","","Shop/bt_shop_4_1.png","","")
		:move(190,530-104*4)
		:addTo(self)
		:setSelected(false)
        :setVisible(false)
        :setEnabled(false)
        :setName("check" .. 9)
		:setTag(ShopLayer.CBT_ENTITY)
		:addEventListener(cbtlistener)
    self.m_tabCheckBoxPosition = 
    {   
        {cc.p(190,530)},
        {cc.p(190,530), cc.p(190,426)},
        {cc.p(190,530), cc.p(190,426), cc.p(190,322)},
        {cc.p(190,530), cc.p(190,426), cc.p(190,322), cc.p(190,218)},
        {cc.p(190,530), cc.p(190,426), cc.p(190,322), cc.p(190,218), cc.p(190,114)},
        {cc.p(190,530), cc.p(190,426), cc.p(190,322), cc.p(190,218), cc.p(190,114), cc.p(190,10)}
    }
    self.m_tabActiveCheckBox = GlobalUserItem.tabShopCache["shopActiveCheckBox"] or {}

	self._scrollView = ccui.ScrollView:create()
									  :setContentSize(cc.size(938,520))
									  :setAnchorPoint(cc.p(0.5, 0.5))
									  :setPosition(cc.p(805, 314))
									  :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
									  :setBounceEnabled(true)
									  :setScrollBarEnabled(false)
									  :addTo(self)
end

function ShopLayer:updateCheckBoxList()
    local poslist = self.m_tabCheckBoxPosition[#self.m_tabActiveCheckBox]
    if nil == poslist then
        return
    end
    for k,v in pairs(self.m_tabActiveCheckBox) do
        local tmp = self:getChildByName(v)
        if nil ~= tmp then
            tmp:setEnabled(true)
            tmp:setVisible(true)            

            local pos = poslist[k]
            if nil ~= pos then
                tmp:setPosition(pos)
            end
        end
    end

    --选择的类型
    local tmp = self:getChildByTag(self._select)
    if nil ~= tmp and tmp:isVisible() then
        tmp:setSelected(true)
        --请求物品列表
        self:loadPropertyAndVip(self._select)
    end
end

function ShopLayer:getShopPropertyType()
    self._scene:showPopWait()
	appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobilePropertyType",function(jstable,jsdata)
		self._scene:dismissPopWait()
        dump(jstable, "jstable", 6)
        if type(jstable) == "table" then
			local data = jstable["data"]
			if type(data) == "table" then
				if nil ~= data["valid"] and true == data["valid"] then
					local list = data["list"]
					if type(list) == "table" then
						for k,v in pairs(list) do
							self._shopTypeIdList["check" .. v.TypeID] = v
                            table.insert(self.m_tabActiveCheckBox, "check" .. v.TypeID)
						end
                        GlobalUserItem.tabShopCache["shopTypeIdList"] = self._shopTypeIdList
                        GlobalUserItem.tabShopCache["shopActiveCheckBox"] = self.m_tabActiveCheckBox
                        --刷新界面显示
                        self:updateCheckBoxList()
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

--按键监听
function ShopLayer:onButtonClickedEvent(tag,sender)
    local beginPos = sender:getTouchBeganPosition()
    local endPos = sender:getTouchEndPosition()
    if math.abs(endPos.x - beginPos.x) > 30 
        or math.abs(endPos.y - beginPos.y) > 30 then
        print("ShopLayer:onButtonClickedEvent ==> MoveTouch Filter")
        return
    end

	local name = sender:getName()

	if name == SHOP_BUY[ShopLayer.BT_SCORE] then
		--游戏币获取
		GlobalUserItem.buyItem = self._scoreList[tag-ShopLayer.BT_SCORE]
        if GlobalUserItem.buyItem.id == "game_score" and PriRoom then
            self:getParent():getParent():onChangeShowMode(PriRoom.LAYTAG.LAYER_EXCHANGESCORE, GlobalUserItem.buyItem.resultGold)
        else
            self:getParent():getParent():onChangeShowMode(yl.SCENE_SHOPDETAIL)
        end
	elseif name == SHOP_BUY[ShopLayer.BT_BEAN] then
		--游戏豆获取
        local item = self._beanList[tag - ShopLayer.BT_BEAN]
        if nil == item then
            return
        end
        local bThirdPay = true

        if ClientConfig.APPSTORE_VERSION 
            and (targetPlatform == cc.PLATFORM_OS_IPHONE or targetPlatform == cc.PLATFORM_OS_IPAD) then
            if self.m_nPayMethod == APPSTOREPAY then
                bThirdPay = false
                local payparam = {}
                payparam.http_url = yl.HTTP_URL
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
                        --更新用户游戏豆
                        GlobalUserItem.dUserBeans = GlobalUserItem.dUserBeans + item.count
                        --通知更新        
                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                        eventListener.obj = yl.RY_MSG_USERWEALTH
                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                        --重新请求支付列表
                        self:reloadBeanList()

                        self:updateScoreInfo()
                    else
                        showToast(self, "支付异常", 2)
                    end
                end
                MultiPlatform:getInstance():thirdPartyPay(yl.ThirdParty.IAP, payparam, payCallBack)
            end
        end

        if bThirdPay then
            if false == ShopPay:isPayMethodValid() then
                showToast(self, "支付服务未开通!", 2, cc.c4b(250,0,0,255))
                return
            end
            if nil == self.m_payLayer then
                self.m_payLayer = ShopPay:create(self)
                self:addChild(self.m_payLayer)
            end            
            local sprice = string.format("%.2f元", item.price)
            self.m_payLayer:refresh(item.count, item.name, sprice, item.price, item.appid)
            self.m_payLayer:showLayer(true)
        end			
	elseif name == SHOP_BUY[ShopLayer.BT_VIP] then
		--vip购买
		GlobalUserItem.buyItem = self._vipList[tag-ShopLayer.BT_VIP]
		self:getParent():getParent():onChangeShowMode(yl.SCENE_SHOPDETAIL)
	elseif name == SHOP_BUY[ShopLayer.BT_PROPERTY] then
		--道具购买
		GlobalUserItem.buyItem = self._propertyList[tag-ShopLayer.BT_PROPERTY]
        if GlobalUserItem.buyItem.id == "room_card" and PriRoom then
            self:getParent():getParent():onChangeShowMode(PriRoom.LAYTAG.LAYER_BUYCARD, GlobalUserItem.buyItem)
        else
            self:getParent():getParent():onChangeShowMode(yl.SCENE_SHOPDETAIL)
        end		
	end
end

function ShopLayer:onSelectedEvent(tag,sender,eventType)

	if self._select == tag then
		self:getChildByTag(tag):setSelected(true)
		return
	end

	self._select = tag

	for i=1,5 do
		if i ~= tag then
			self:getChildByTag(i):setSelected(false)
		end
	end

	--游戏币
	if (tag == ShopLayer.CBT_SCORE) then
		if 0 == #self._scoreList then
			self:loadPropertyAndVip(tag) 
		else
			self:onUpdateScore()
		end
	end

	--游戏豆
	if (tag == ShopLayer.CBT_BEAN) then
		self:onClearShowList()
		if 0 == #self._beanList then
			self:loadPropertyAndVip(tag) 
		else
			self:onUpdateBeanList()
		end		
	end

	--vip
	if (tag==ShopLayer.CBT_VIP) then
		if (#self._vipList==0) then
			self:loadPropertyAndVip(tag) 
		else
			self:onUpdateVIP();
		end
	end

	--道具
	if (tag==ShopLayer.CBT_PROPERTY) then
		if (#self._propertyList==0) then
			self:loadPropertyAndVip(tag)
		else
			self:onUpdateProperty()
		end
	end

    local topBtnTag = ShopLayer.BT_BAG
    local normalFile = "Information/btn_ubag_0.png"
    local pressFile = "Information/btn_ubag_1.png"
	--实物
	if (tag == ShopLayer.CBT_ENTITY) then
		self:onClearShowList()
		self:onUpdateGoodsList()

        topBtnTag = ShopLayer.BT_ORDERRECORD
        normalFile = "Shop/bt_shop_exchange_0.png"
        pressFile = "Shop/bt_shop_exchange_1.png"
	end
    self.m_btnTopBtn:setTag(topBtnTag)
    self.m_btnTopBtn:loadTextureNormal(normalFile)
    self.m_btnTopBtn:loadTexturePressed(pressFile)
end

--网络请求
function ShopLayer:loadPropertyAndVip(tag)	
	local this = self
    local typid = 0

    local cbt = self:getChildByTag(tag)
    if nil ~= cbt then
        if nil ~= self._shopTypeIdList[cbt:getName()] then
            typid = self._shopTypeIdList[cbt:getName()].TypeID
        end
    end

    --实物特殊处理
    if tag == ShopLayer.CBT_ENTITY then
    	self:onUpdateGoodsList()
    --游戏豆额外处理
    elseif tag == ShopLayer.CBT_BEAN then
        if 0 ~= #self._beanList then
            self:onUpdateBeanList()
            return
        end
    	self._scene:showPopWait()
        if ClientConfig.APPSTORE_VERSION 
            and (targetPlatform == cc.PLATFORM_OS_IPHONE or targetPlatform == cc.PLATFORM_OS_IPAD) then
            -- 内购开关
            appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=iosnotappstorepayswitch",function(jstable,jsdata)
                local errmsg = "获取支付配置异常!"
                if type(jstable) == "table" then
                    local jdata = jstable["data"]
                    if type(jdata) == "table" then
                        local valid = jdata["valid"] or false
                        if true == valid then
                            errmsg = nil
                            local value = jdata["State"] or "0"
                            value = tonumber(value)
                            if 1 == value then
                                GlobalUserItem.tabShopCache["nPayMethod"] = APPSTOREPAY
                                self.m_nPayMethod = APPSTOREPAY
                                self:requestPayList(1)
                            else
                                GlobalUserItem.tabShopCache["nPayMethod"] = THIRDPAY
                                -- 请求列表
                                self:requestPayList()
                            end                            
                        end
                    end
                end

                self._scene:dismissPopWait()
                if type(errmsg) == "string" and "" ~= errmsg then
                    showToast(self,errmsg,2,cc.c3b(250,0,0))
                end
            end)
        else
            -- 请求列表
            self:requestPayList()
        end
    else
        if tag == ShopLayer.CBT_VIP and 0 ~= #self._vipList then
            self:onUpdateVIP()
            return
        elseif tag == ShopLayer.CBT_PROPERTY and 0 ~= #self._propertyList then
            self:onUpdateProperty()
            return
        elseif tag == ShopLayer.CBT_SCORE and 0 ~= #self._scoreList then
            self:onUpdateScore()
            return
        end
        self:requestPropertyList(typid, tag)
    end	
end

function ShopLayer:requestPayList(isIap)
    isIap = isIap or 0
    local beanurl = yl.HTTP_URL .. "/WS/MobileInterface.ashx"
    local ostime = os.time()

    self._scene:showPopWait()
    appdf.onHttpJsionTable(beanurl ,"GET","action=GetPayProduct&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime) .. "&typeID=" .. isIap,function(sjstable,sjsdata)
        dump(sjstable, "支付列表", 6)
        local errmsg = "获取支付列表异常!"
        self._scene:dismissPopWait()
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
                        item.nOrder = 0
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
                            if a.nOrder ~= b.nOrder then
                                return a.nOrder > b.nOrder
                            else
                                return a.sortid < b.sortid
                            end
                        end)
                    GlobalUserItem.tabShopCache["shopBeanList"] = self._beanList
                    self:onUpdateBeanList()
                end
            end
        end

        if type(errmsg) == "string" and "" ~= errmsg then
            showToast(self,errmsg,2,cc.c3b(250,0,0))
        end 
    end)
end

function ShopLayer:requestPropertyList(nTypeID, tag)
    nTypeID = nTypeID or 0
    self._scene:showPopWait()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobileProperty&TypeID=" .. nTypeID,function(jstable,jsdata)
        self._scene:dismissPopWait()
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
                                item.bean = tonumber(list[i]["Cash"])
                                item.gold = tonumber(list[i]["Gold"])
                                item.ingot = tonumber(list[i]["UserMedal"])
                                item.loveliness = tonumber(list[i]["LoveLiness"])
                                item.resultGold = tonumber(list[i]["UseResultsGold"])
                                item.description = list[i]["RegulationsInfo"]
                                item.sortid = list[i]["SortID"] or "0"
                                item.sortid = tonumber(item.sortid)
                                item.minPrice = tonumber(list[i]["minPrice"]) or 0

                                if (item.loveliness ~= 0) and (item.bean == 0 and item.gold == 0 and item.ingot == 0) then

                                else
                                    if tag == ShopLayer.CBT_PROPERTY and item.id == 501 then
                                        if GlobalUserItem.bEnableRoomCard then
                                            item.id = "room_card"
                                            table.insert(tmpList, item)
                                        end
                                    elseif tag == ShopLayer.CBT_SCORE and item.id == 501 and 0 ~= item.resultGold then
                                        if GlobalUserItem.bEnableRoomCard then
                                            item.id = "game_score"
                                            item.minPrice = item.resultGold
                                            item.bean = 0
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

            if tag == ShopLayer.CBT_VIP then
                GlobalUserItem.tabShopCache["shopVipList"] = tmpList
                self._vipList = tmpList
                self:onUpdateVIP()
            elseif tag == ShopLayer.CBT_PROPERTY then               
                GlobalUserItem.tabShopCache["shopPropertyList"] = tmpList
                self._propertyList = tmpList
                self:onUpdateProperty()
            elseif tag == ShopLayer.CBT_SCORE then         
                GlobalUserItem.tabShopCache["shopScoreList"] = tmpList
                self._scoreList = tmpList
                self:onUpdateScore()
            end            
        else
            showToast(self,"抱歉，获取道具信息失败！",2,cc.c3b(250,0,0))
        end
    end)
end

function ShopLayer:reloadBeanList()
    self:onClearShowList()
    GlobalUserItem.tabShopCache["shopBeanList"] = {}
    self._beanList = {}
    self:loadPropertyAndVip(ShopLayer.CBT_BEAN)
end

function ShopLayer:updateScoreInfo()
   self._txtGold:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
   self._txtBean:setString(string.formatNumberThousands(GlobalUserItem.dUserBeans,true,"/"))
   self._txtIngot:setString(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"))
end

--更新游戏币
function ShopLayer:onUpdateScore()
	self:onClearShowList()
	self:onUpdateShowList(self._scoreList,ShopLayer.BT_SCORE)
end

--更新游戏豆
function ShopLayer:onUpdateBeanList()
	self:onClearShowList()
	self:onUpdateShowList(self._beanList,ShopLayer.BT_BEAN)
end

--更新VIP
function ShopLayer:onUpdateVIP()
	self:onClearShowList()
	self:onUpdateShowList(self._vipList,ShopLayer.BT_VIP)
end

--更新道具
function ShopLayer:onUpdateProperty()
	self:onClearShowList()
	self:onUpdateShowList(self._propertyList,ShopLayer.BT_PROPERTY)
end

--更新实物兑换列表
function ShopLayer:onUpdateGoodsList()
	local url = yl.HTTP_URL .. "/SyncLogin.aspx?userid=" .. GlobalUserItem.dwUserID .. "&time=".. os.time() .. "&signature="..GlobalUserItem:getSignature(os.time()).."&url=/Mobile/Shop/Goods.aspx"
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
		    	self._scene:dismissPopWait()
		    	print("open " .. url .. " fail")
		    end)
		    self._goodsList:setOnShouldStartLoading(function(sender, url)
		        print("onWebViewShouldStartLoading, url is ", url)	        
		        return true
		    end)
		    self._goodsList:setOnDidFinishLoading(function(sender, url)
		    	self._scene:dismissPopWait()
                ExternalFun.visibleWebView(self._goodsList, true)
		        print("onWebViewDidFinishLoading, url is ", url)
		    end)
		    self:addChild(self._goodsList)
		end
	end

	if nil ~= self._goodsList then
		self._scene:showPopWait()
        ExternalFun.visibleWebView(self._goodsList, false)
		self._goodsList:loadURL(url)
	end
end

--清除当前显示
function ShopLayer:onClearShowList()
	for i=1,#self._showList do
		self._showList[i]:removeFromParent()
	end
	self._showList = nil
	self._showList = {}

	if nil ~= self._goodsList then
        self._goodsList:removeFromParent()
        self._goodsList = nil
	end
end

--更新当前显示
function ShopLayer:onUpdateShowList(theList,tag)
	local bGold = (self._select==ShopLayer.CBT_SCORE)
	local bOther= (self._select~=ShopLayer.CBT_SCORE)
	local bBean = (self._select==ShopLayer.CBT_BEAN)

	--计算scroll滑动高度
	local scrollHeight = 0
	if #theList<7 then
		scrollHeight = 520
		self._scrollView:setInnerContainerSize(cc.size(1130, 550))
	else
		scrollHeight = 260 * math.ceil(#theList / 3)--math.floor((#theList+math.floor(#theList%3))/3)
		self._scrollView:setInnerContainerSize(cc.size(1130, scrollHeight+30))
	end

	for i=1,#theList do
		local item = theList[i]

		self._showList[i] = cc.LayerColor:create(cc.c4b(100, 100, 100, 0), 261, 240)
    		:move(160+math.floor((i-1)%3)*310-130,scrollHeight-(8+120+math.floor((i-1)/3)*270)-90)
    		:addTo(self._scrollView)

		local btn = ccui.Button:create("Shop/frame_shop_7.png","Shop/frame_shop_7.png")
		btn:setContentSize(cc.size(261, 240))
			:move(130,120)
			:setTag(tag+i)
			:setSwallowTouches(false)
			:setName(SHOP_BUY[tag])
			:addTo(self._showList[i])
			:addTouchEventListener(self._btcallback)
   		
   		local price = 0
		local sign = nil
		local pricestr = ""

   		--物品信息
   		local showSp = nil
   		--标题
   		local titleSp = nil
   		if bBean then
   			showSp = display.newSprite("Shop/icon_shop_5.png")
   			local atlas = cc.LabelAtlas:_create(string.gsub(item.count .. "", "[.]", "/"), "Shop/num_shop_5.png", 20, 25, string.byte("/"))
   			atlas:setAnchorPoint(cc.p(1.0,0.5))
   			self._showList[i]:addChild(atlas) 
   			local name = display.newSprite("Shop/text_shop_1.png")
   			name:setAnchorPoint(cc.p(0,0.5))
   			self._showList[i]:addChild(name)
   			local wid = (atlas:getContentSize().width + name:getContentSize().width) / 2   			
   			atlas:setPosition(130 + (atlas:getContentSize().width - wid), 220)
   			name:setPosition(atlas:getPositionX(), 220)

   			price = item.price
   			pricestr = "￥" .. string.formatNumberThousands(price,true,",")

   			--首充
   			if nil ~= item.paysend and 0 ~= item.paysend then
   				local fsp = cc.Sprite:create("Shop/shop_firstpay_sp.png")
   				fsp:setAnchorPoint(cc.p(0,1.0))
   				fsp:setPosition(-8,248)
   				self._showList[i]:addChild(fsp)
   				local isFirstPay = item.isfirstpay == 0
   				btn:setEnabled(isFirstPay)
   			end
   		else
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("icon_public_"..item.id..".png")
            if nil ~= frame then
                showSp = cc.Sprite:createWithSpriteFrame(frame)
            end
            if cc.FileUtils:getInstance():isFileExist("Shop/title_property_"..item.id..".png") then
                titleSp = display.newSprite("Shop/title_property_"..item.id..".png")
            end   			

   			--[[local goldLabel = cc.LabelAtlas:_create(string.formatNumberThousands(item.resultGold,true,"/"), "Shop/num_shop_1.png", 20, 25, string.byte("/")) 
	    		:move(130,220)
	    		:setAnchorPoint(cc.p(0.5,0.5))
	    		:setVisible(bGold)
	    		:addTo(self._showList[i])
	    	display.newSprite("Shop/sign_shop_2.png")
				:move(130-goldLabel:getContentSize().width/2-13,220)
				:setVisible(bGold)
				:addTo(self._showList[i])]]
			sign = 0
			if item.bean == 0 then
				if item.ingot == 0 then
					if item.gold == 0 then
                        if item.loveliness == 0 then
                            price = item.minPrice
                            sign = 4
                        else
                            price = item.loveliness
                            sign = 3
                        end						
					else
						price = item.gold
						sign = 2
					end
				else
					price = item.ingot
					sign = 1
				end
			else
				price = item.bean
			end
			pricestr = string.formatNumberThousands(price,true,",")

            --
            local icon_star = cc.Sprite:create("Shop/icon_star_sp.png")
            if nil ~= icon_star then
                icon_star:setPosition(130, 140)
                self._showList[i]:addChild(icon_star, 1)
            end
   		end
		if price == 0 then
			print("======= ***** 价格信息有误 ***** =======")
			return
		end

		if nil ~= showSp then
			showSp:setPosition(130, 120)
			self._showList[i]:addChild(showSp)
		end
		if nil ~= titleSp then
			titleSp:setPosition(130, 220)
			--titleSp:setVisible(bOther)
			self._showList[i]:addChild(titleSp)
		end		

		local priceLabel = cc.Label:createWithTTF(pricestr, "fonts/round_body.ttf", 24)
        	:setAnchorPoint(cc.p(0.5,0.5))
        	:move(130,34)
       		:setTextColor(cc.c4b(255,255,255,255))
       		:addTo(self._showList[i])

       	if nil ~= sign then     
            local width = 0  		
            if cc.FileUtils:getInstance():isFileExist("Shop/sign_shop_"..sign..".png") then
    	       	local spsign = display.newSprite("Shop/sign_shop_"..sign..".png")
                width = spsign:getContentSize().width + priceLabel:getContentSize().width
    	       	spsign:setAnchorPoint(cc.p(0.0,0.5))
    				:move((260-width)/2,34)
    				:addTo(self._showList[i])
                width = (260-width) * 0.5 + spsign:getContentSize().width
            end

			priceLabel:setAnchorPoint(cc.p(0,0.5))
			priceLabel:setPosition(width,34)
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

function ShopLayer:showPopWait()
	self._scene:showPopWait()
end

function ShopLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

function ShopLayer:queryUserScoreInfo(queryCallback)
    if nil ~= self._scene.queryUserScoreInfo then
        self._scene:queryUserScoreInfo(queryCallback)
    end
end

return ShopLayer