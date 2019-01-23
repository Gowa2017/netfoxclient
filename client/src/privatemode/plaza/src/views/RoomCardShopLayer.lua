--
-- Author: zhong
-- Date: 2017-01-09 10:04:17
--
-- 房卡购买、兑换
local RoomCardShopLayer = class("RoomCardShopLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ShopDetailFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")

local BTN_ADD = 101
local BTN_SUB = 102
local BTN_BLANK = 103
local BTN_EXCHANGE = 104  -- 兑换
local BTN_BUY = 105 -- 购买
local CBT_NULL = 200
local CBT_BEAN = 201 -- 游戏豆选择
local CBT_GOLD = 202 -- 游戏币选择

-- param[tabParam] 参数列表
function RoomCardShopLayer:ctor( tabParam )
    self.m_tabParam = tabParam
    self:registerScriptHandler(function(eventType)
        if eventType == "enterTransitionFinish" then    
            
        elseif eventType == "exitTransitionStart" then 
        elseif eventType == "exit" then
            if self._shopDetailFrame:isSocketServer() then
                self._shopDetailFrame:onCloseSocket()
            end
            if nil ~= self._shopDetailFrame._gameFrame then
                self._shopDetailFrame._gameFrame._shotFrame = nil
                self._shopDetailFrame._gameFrame = nil
            end 

            if nil ~= self.m_listener then
                cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
                self.m_listener = nil
            end
        end
    end)
    --网络回调
    local shopDetailCallBack = function(result,message)
        return self:onShopDetailCallBack(result,message)
    end

    local gameFrame = PriRoom:getInstance():getNetFrame()._gameFrame
    --网络处理
    self._shopDetailFrame = ShopDetailFrame:create(self,shopDetailCallBack)
    self._shopDetailFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._shopDetailFrame
    end

    display.newSprite("Shop/frame_shop_0.png")
        :move(yl.WIDTH/2,yl.HEIGHT - 51)
        :addTo(self)

    ccui.Button:create("bt_return_0.png","bt_return_1.png")
        :move(75,yl.HEIGHT-51)
        :addTo(self)
        :addTouchEventListener(function(ref, type)
                if type == ccui.TouchEventType.ended then
                    PriRoom:getInstance():getPlazaScene():onKeyBack()
                end
            end)
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,325)
        self:addChild(sp)
    end

    display.newSprite("Shop/Detail/frame_detail_0.png")
        :move(840,350)
        :addTo(self)

    --按钮回调
    self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self._nCount = 1
    self.m_buyRate = 0
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Shop/Detail/RoomCardShopLayer.csb", self)
    local bExchange = tabParam.bExchange
    if nil == bExchange then
        return
    end

    if bExchange then
        self:initExchangeUI(csbNode)
    else
        self:initBuyUI(csbNode)
    end
    self:onUpdateNum()
end

-- 兑换ui
function RoomCardShopLayer:initExchangeUI(csbNode)
    local exchangeUi = csbNode:getChildByName("exchange_panel")
    exchangeUi:setVisible(true)
    local buyUi = csbNode:getChildByName("buy_panel")
    buyUi:setVisible(false)
    self.m_buyUi = buyUi

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 编辑框
    local editbox = ccui.EditBox:create(cc.size(400, 48),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(cc.p(872,520))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setFontColor(cc.c3b(250,204,38))
        :setPlaceholderFontSize(30)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    exchangeUi:addChild(editbox)
    editbox:setText("1")
    editbox:setVisible(false)
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_editNumber = editbox

    local btn = ccui.Button:create("blank.png","blank.png","blank.png", UI_TEX_TYPE_PLIST) 
    btn:setScale9Enabled(true)
    btn:setContentSize(cc.size(400, 48))
    btn:setPosition(cc.p(872,520))
    btn:setTag(BTN_BLANK)
    btn:addTouchEventListener(self._btcallback)
    exchangeUi:addChild(btn) 

    -- 加减按钮
    btn = exchangeUi:getChildByName("btn_add")
    btn:setTag(BTN_ADD)
    btn:addTouchEventListener(self._btcallback)

    btn = exchangeUi:getChildByName("btn_sub")
    btn:setTag(BTN_SUB)
    btn:addTouchEventListener(self._btcallback)

    -- 兑换按钮
    btn = exchangeUi:getChildByName("btn_exchange")
    btn:setTag(BTN_EXCHANGE)
    btn:addTouchEventListener(self._btcallback)

    -- 数量
    self.m_atlasCount = exchangeUi:getChildByName("atlas_count")

    -- 兑换数量
    self.m_atlasExchangeCount = exchangeUi:getChildByName("atlas_exchange")

    -- 剩余
    self.m_textLeft = exchangeUi:getChildByName("txt_left")
    self.m_textLeft:setString("剩余" .. GlobalUserItem.lRoomCard)

    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

-- 购买ui
function RoomCardShopLayer:initBuyUI(csbNode)
    local exchangeUi = csbNode:getChildByName("exchange_panel")
    exchangeUi:setVisible(false)
    local buyUi = csbNode:getChildByName("buy_panel")    
    buyUi:setVisible(true)
    self.m_buyUi = buyUi

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 编辑框
    local editbox = ccui.EditBox:create(cc.size(400, 48),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(cc.p(900,520))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setFontColor(cc.c3b(250,204,38))
        :setPlaceholderFontSize(30)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    buyUi:addChild(editbox)
    editbox:setText("1")
    editbox:setVisible(false)
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_editNumber = editbox

    local btn = ccui.Button:create("blank.png","blank.png","blank.png", UI_TEX_TYPE_PLIST) 
    btn:setScale9Enabled(true)
    btn:setContentSize(cc.size(400, 48))
    btn:setPosition(cc.p(900,520))
    btn:setTag(BTN_BLANK)
    btn:addTouchEventListener(self._btcallback)
    buyUi:addChild(btn) 

    -- 加减按钮
    btn = buyUi:getChildByName("btn_add")
    btn:setTag(BTN_ADD)
    btn:addTouchEventListener(self._btcallback)

    btn = buyUi:getChildByName("btn_sub")
    btn:setTag(BTN_SUB)
    btn:addTouchEventListener(self._btcallback)

    -- 购买按钮
    btn = buyUi:getChildByName("btn_buy")
    btn:setTag(BTN_BUY)
    btn:addTouchEventListener(self._btcallback)

    -- 数量
    self.m_atlasCount = buyUi:getChildByName("atlas_count")

    -- 价格
    self.m_atlasItemPrice = buyUi:getChildByName("atlas_price")
    -- 类型
    self.m_txtItemPriceType = buyUi:getChildByName("txt_pricetype")
    -- 折扣价格
    self.m_atlasDiscountPrice = buyUi:getChildByName("atlas_price_discount")
    -- 折后类型
    self.m_txtDiscountPriceType = buyUi:getChildByName("txt_pricetype_discount")
    -- 购买价格
    self.m_atlasBuyPrice = buyUi:getChildByName("atlas_price_buy")
    -- 购买类型
    self.m_txtBuyPriceType = buyUi:getChildByName("txt_pricetype_buy")

    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender,eventType)
    end
    self.m_nSelect = CBT_NULL 
    -- 类型选择
    local y = 300
    -- 游戏豆
    if nil ~= self.m_tabParam.item.bean and 0 ~= self.m_tabParam.item.bean then
        cc.Label:createWithTTF(string.formatNumberThousands(self.m_tabParam.item.bean,true,",").."游戏豆", "fonts/round_body.ttf", 24)
            :setAnchorPoint(cc.p(0.0,0.5))
            :move(144,y)
            :setTextColor(cc.c4b(255,255,0,255))
            :addTo(buyUi)
        ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
            :move(110,y)
            :addTo(buyUi)
            :setSelected(true)
            :setTag(CBT_BEAN)
            :addEventListener(cbtlistener)
        y = y-61
        self.m_nSelect = CBT_BEAN
        self.m_buyRate = self.m_tabParam.item.bean
    end
    -- 游戏币
    if nil ~= self.m_tabParam.item.gold and 0 ~= self.m_tabParam.item.gold then
        local bSelect = false
        if nil == self.m_tabParam.item.bean or 0 == self.m_tabParam.item.bean then
            self.m_nSelect = CBT_GOLD
            bSelect = true
            self.m_txtItemPriceType:setString("游戏币")
            self.m_buyRate = self.m_tabParam.item.gold
        end

        cc.Label:createWithTTF(string.formatNumberThousands(self.m_tabParam.item.gold,true,",").."游戏币", "fonts/round_body.ttf", 24)
            :setAnchorPoint(cc.p(0.0,0.5))
            :move(144,y)
            :setTextColor(cc.c4b(255,255,0,255))
            :addTo(buyUi)
        ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
            :move(110,y)
            :addTo(buyUi)
            :setSelected(bSelect)
            :setTag(CBT_GOLD)
            :addEventListener(cbtlistener)
    end
    local vip = GlobalUserItem.cbMemberOrder or 0
    local bShowDiscount = vip ~= 0
    self.m_discount = 100
    if vip ~= 0 then
        self.m_discount = GlobalUserItem.MemberList[vip]._shop
    end    
    -- 折扣
    self._txtDiscount = cc.Label:createWithTTF(self.m_discount .. "%折扣", "fonts/round_body.ttf", 24)
            :setAnchorPoint(cc.p(1.0,0.5))
            :move(1130,380)
            :setTextColor(cc.c4b(255,0,0,255))
            :setVisible(bShowDiscount)
            :addTo(self)
    -- 会员标识
    local sp_vip = cc.Sprite:create("Information/atlas_vipnumber.png")
    if nil ~= sp_vip then
        sp_vip:setPosition(1130 - self._txtDiscount:getContentSize().width - 20, 380)
        self:addChild(sp_vip)
        sp_vip:setTextureRect(cc.rect(28*vip,0,28,26))
        sp_vip:setVisible(bShowDiscount)
    end
    self:onUpdatePrice()
end

function RoomCardShopLayer:onButtonClickedEvent(tag, ref)
    if tag == BTN_ADD then
        self._nCount = self._nCount + 1
        self.m_editNumber:setText(self._nCount .. "")
        self:onUpdateNum()
    elseif tag == BTN_SUB then
        if self._nCount > 0 then
            self._nCount = self._nCount - 1
            self.m_editNumber:setText(self._nCount .. "")
            self:onUpdateNum()
        end        
    elseif tag == BTN_BLANK then
        self.m_editNumber:setVisible(true)
        self.m_editNumber:touchDownAction(self.m_editNumber, ccui.TouchEventType.ended)
    elseif tag == BTN_EXCHANGE then
        if self._nCount > 0 then
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():getNetFrame():onExchangeScore(self._nCount)
        end        
    elseif tag == BTN_BUY then
        if self._nCount > 0 then
            PriRoom:getInstance():showPopWait()
            if self.m_nSelect == CBT_BEAN then
                self._shopDetailFrame:onPropertyBuy(yl.CONSUME_TYPE_CASH, self._nCount, 501, 0)
            elseif self.m_nSelect == CBT_GOLD then
                self._shopDetailFrame:onPropertyBuy(yl.CONSUME_TYPE_GOLD, self._nCount, 501, 0)
            end
        end        
    end
end

function RoomCardShopLayer:onSelectedEvent(tag,sender,eventType)
    if self.m_nSelect == tag then
        self.m_buyUi:getChildByTag(tag):setSelected(true)
        return
    end
    self.m_nSelect = tag

    if tag == CBT_BEAN then
        self.m_buyRate = self.m_tabParam.item.bean
        if nil ~= self.m_buyUi:getChildByTag(CBT_GOLD) then
            self.m_buyUi:getChildByTag(CBT_GOLD):setSelected(false)  
        end     
    elseif tag == CBT_GOLD then
        self.m_buyRate = self.m_tabParam.item.gold
        if nil ~= self.m_buyUi:getChildByTag(CBT_BEAN) then
            self.m_buyUi:getChildByTag(CBT_BEAN):setSelected(false)
        end
    end
    self:onUpdatePrice()
    self:onUpdateNum()
end

function RoomCardShopLayer:onEditEvent(event,editbox)
    if event == "began" then
        self.m_atlasCount:setVisible(false)
    elseif event == "return" then
        local ndst = tonumber(editbox:getText())
        if "number" == type(ndst) then
            self._nCount = ndst
        end
        editbox:setVisible(false)
        self:onUpdateNum()
    end
end

function RoomCardShopLayer:onUpdateNum()
    self.m_atlasCount:setVisible(true)
    self.m_atlasCount:setString(string.formatNumberThousands(self._nCount,true,"."))

    -- 获取数量
    local bExchange = self.m_tabParam.bExchange
    if nil == bExchange then
        return
    end
    if bExchange then
        local count = self._nCount * self.m_tabParam.exchangeRate
        self.m_atlasExchangeCount:setString(string.formatNumberThousands(count,true,"."))
    else
        -- 道具价格
        local price = self.m_buyRate
        local str = string.formatNumberThousands(price,false,"*")
        str = string.gsub(str, "[.]", "/")
        str = string.gsub(str, "*", "[.]")
        self.m_atlasItemPrice:setString(str)
        -- 折后价格
        price = self.m_buyRate * (self.m_discount * 0.01)
        str = string.formatNumberThousands(price,false,"*")
        str = string.gsub(str, "[.]", "/")
        str = string.gsub(str, "*", "[.]")
        self.m_atlasDiscountPrice:setString(str)
        -- 购买价格
        price = self._nCount * self.m_buyRate * (self.m_discount * 0.01)
        str = string.formatNumberThousands(price,false,"*")
        str = string.gsub(str, "[.]", "/")
        str = string.gsub(str, "*", "[.]")        
        self.m_atlasBuyPrice:setString(str)
    end
end

function RoomCardShopLayer:onUpdatePrice()
    if CBT_BEAN == self.m_nSelect then
        self.m_txtItemPriceType:setString("游戏豆/个")
        self.m_txtBuyPriceType:setString("游戏豆")
        self.m_txtDiscountPriceType:setString("游戏豆/个")
    elseif CBT_GOLD == self.m_nSelect then
        self.m_txtItemPriceType:setString("游戏币/个")
        self.m_txtBuyPriceType:setString("游戏币")
        self.m_txtDiscountPriceType:setString("游戏币/个")
    end
end

function RoomCardShopLayer:onUserInfoChange( event )
    local msgWhat = event.obj
    if nil ~= msgWhat and msgWhat == yl.RY_MSG_USERWEALTH then
        local bExchange = self.m_tabParam.bExchange
        if nil == bExchange then
            return
        end 
        if bExchange then
            --更新财富
            self.m_textLeft:setString("剩余" .. GlobalUserItem.lRoomCard)
        end        
    end
end

function RoomCardShopLayer:onShopDetailCallBack(result,message)
    local bRes = false
    PriRoom:getInstance():dismissPopWait()
    if type(message) == "string" and message ~= "" then
        showToast(self,message,2)       
    end

    if 1 == result then
        PriRoom:getInstance():getPlazaScene():queryUserScoreInfo()
    end
    return bRes
end

return RoomCardShopLayer