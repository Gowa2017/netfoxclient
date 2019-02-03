--[[
	商城详细界面
	2016_07_04 Ravioyla
]]

local ShopDetailLayer = class("ShopDetailLayer", function(scene)
		local shopDetailLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return shopDetailLayer
end)

local ShopDetailFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")

ShopDetailLayer.CBT_BEAN			= 1
ShopDetailLayer.CBT_INGOT			= 2
ShopDetailLayer.CBT_GOLD			= 3
ShopDetailLayer.CBT_LOVELINESS		= 4

ShopDetailLayer.BT_BUY1				= 20
ShopDetailLayer.BT_BUY2				= 21
ShopDetailLayer.BT_ADD				= 22
ShopDetailLayer.BT_MIN				= 23
ShopDetailLayer.BT_BLANK            = 24

-- 进入场景而且过渡动画结束时候触发。
function ShopDetailLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ShopDetailLayer:onExitTransitionStart()
    return self
end

function ShopDetailLayer:ctor(scene, gameFrame)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
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

	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender:getTag(),sender,eventType)
    end

    --网络回调
    local shopDetailCallBack = function(result,message)
		return this:onShopDetailCallBack(result,message)
	end

	--网络处理
	self._shopDetailFrame = ShopDetailFrame:create(self,shopDetailCallBack)
    self._shopDetailFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._shopDetailFrame
    end

    self._select = ShopDetailLayer.CBT_BEAN
    self._item = GlobalUserItem.buyItem
    self._buyNum = 0
    self._toUse = 0
    self._type = yl.CONSUME_TYPE_CASH

    display.newSprite("Shop/frame_shop_0.png")
        :move(yl.WIDTH/2,yl.HEIGHT - 51)
        :addTo(self)

	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
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

    display.newSprite("Shop/Detail/frame_detail_1.png")
    	:move(210,477)
    	:addTo(self)

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("icon_public_"..self._item.id..".png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        self:addChild(sp)
        sp:setPosition(210,477)
    end

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("text_public_"..self._item.id..".png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        self:addChild(sp)
        sp:setPosition(210,367)
    end

	local y = 300

	--游戏豆
	if self._item.bean ~= 0 then
		cc.Label:createWithTTF(string.formatNumberThousands(self._item.bean,true,",").."游戏豆", "fonts/round_body.ttf", 24)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(144,y)
       		:setTextColor(cc.c4b(255,255,0,255))
       		:addTo(self)
       	ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
			:move(110,y)
			:addTo(self)
			:setSelected(true)
			:setTag(ShopDetailLayer.CBT_BEAN)
			:addEventListener(cbtlistener)

		y = y-61
	end

	--元宝
	if self._item.ingot ~= 0 then
		cc.Label:createWithTTF(string.formatNumberThousands(self._item.ingot,true,",").."元宝", "fonts/round_body.ttf", 24)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(144,y)
       		:setTextColor(cc.c4b(255,153,0,255))
       		:addTo(self)
       	ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
			:move(110,y)
			:addTo(self)
			:setSelected(self._item.bean == 0)
			:setTag(ShopDetailLayer.CBT_INGOT)
			:addEventListener(cbtlistener)

		if self._item.bean == 0 then
			self._select = ShopDetailLayer.CBT_INGOT
		end

		y = y-61
	end

	--游戏币
	if self._item.gold ~= 0 then
		cc.Label:createWithTTF(string.formatNumberThousands(self._item.gold,true,",").."游戏币", "fonts/round_body.ttf", 24)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(144,y)
       		:setTextColor(cc.c4b(255,204,0,255))
       		:addTo(self)
       	ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
			:move(110,y)
			:addTo(self)
			:setSelected(self._item.ingot==0 and self._item.bean==0)
			:setTag(ShopDetailLayer.CBT_GOLD)
			:addEventListener(cbtlistener)

		if self._item.ingot==0 and self._item.bean==0 then
			self._select = ShopDetailLayer.CBT_GOLD
		end

		y = y-61
	end

	--魅力值
	--[[if self._item.loveliness ~= 0 then
		cc.Label:createWithTTF(string.formatNumberThousands(self._item.loveliness,true,",").."魅力值", "fonts/round_body.ttf", 24)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(144,y)
       		:setTextColor(cc.c4b(255,102,0,255))
       		:addTo(self)
       	ccui.CheckBox:create("Shop/Detail/cbt_detail_0.png","","Shop/Detail/cbt_detail_1.png","","")
			:move(110,y)
			:addTo(self)
			:setSelected(self._item.ingot==0 and self._item.bean==0 and self._item.gold==0)
			:setTag(ShopDetailLayer.CBT_LOVELINESS)
			:addEventListener(cbtlistener)

		if self._item.ingot==0 and self._item.bean==0 and self._item.gold==0 then
			self._select = ShopDetailLayer.CBT_LOVELINESS
		end

		y = y-61
	end]]

	display.newSprite("Shop/Detail/text_detail_0.png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(660,509)
			:addTo(self)

	for i=1,4 do
		display.newSprite("Shop/Detail/text_detail_"..i..".png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(660,439-(46*(i-1)))
			:addTo(self)
	end

	self._priceTag1 = display.newSprite("Shop/Detail/text_detail_5_0.png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(660,439-(46*(5-1)))
			:addTo(self)
	self._priceTag2 = display.newSprite("Shop/Detail/text_detail_6_0.png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(1142,439-(46*(1-1)))
			:addTo(self)
	self._priceTag3 = display.newSprite("Shop/Detail/text_detail_6_0.png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(1142,439-(46*(3-1)))
			:addTo(self)
	self._priceTag4 = display.newSprite("Shop/Detail/text_detail_7_0.png")
			:setAnchorPoint(cc.p(1.0,0.5))
			:move(1142,439-(46*(4-1)))
			:addTo(self)
    -- 数量
	self._txtPrice = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"), "Shop/num_shop_0.png", 16, 22, string.byte(".")) 
    		:move(1142,439-(46*(5-1)))
    		:setAnchorPoint(cc.p(1.0,0.5))
    		:addTo(self)

	self:onUpdatePrice()

	ccui.Button:create("Shop/Detail/bt_detail_0_0.png","Shop/Detail/bt_detail_0_1.png")
			:move(674,173)
			:setTag(ShopDetailLayer.BT_BUY1)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	ccui.Button:create("Shop/Detail/bt_detail_1_0.png","Shop/Detail/bt_detail_1_1.png")
			:move(999,173)
			:setTag(ShopDetailLayer.BT_BUY2)
			:addTo(self)
			:addTouchEventListener(self._btcallback)

	display.newSprite("Shop/Detail/frame_detail_2.png")
			:move(910,509)
			:addTo(self)
	ccui.Button:create("Shop/Detail/bt_detail_min.png","Shop/Detail/bt_detail_min.png")
			:move(689,509)
			:setTag(ShopDetailLayer.BT_MIN)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	ccui.Button:create("Shop/Detail/bt_detail_add.png","Shop/Detail/bt_detail_add.png")
			:move(1120,509)
			:setTag(ShopDetailLayer.BT_ADD)
			:addTo(self)
			:addTouchEventListener(self._btcallback)

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 编辑框
    local editbox = ccui.EditBox:create(cc.size(400, 48),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(cc.p(910,509))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setFontColor(cc.c3b(250,204,38))
        :setPlaceholderFontSize(30)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    self:addChild(editbox)
    editbox:setVisible(false)
    editbox:setText("1")
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_editNumber = editbox
    local btn = ccui.Button:create("blank.png","blank.png","blank.png", UI_TEX_TYPE_PLIST) 
    btn:setScale9Enabled(true)
    btn:setContentSize(cc.size(400, 48))
    btn:setPosition(cc.p(910,509))
    btn:setTag(ShopDetailLayer.BT_BLANK)
    btn:addTouchEventListener(self._btcallback)
    self:addChild(btn)    

    self._txtNum = cc.LabelAtlas:_create("0", "Shop/Detail/num_detail_0.png", 19, 25, string.byte(".")) 
            :move(910,509)
            :setAnchorPoint(cc.p(0.5,0.5))
            :addTo(self)
    -- 道具单价
	self._txtPrice1 = cc.LabelAtlas:_create("0", "Shop/Detail/num_detail_0.png", 19, 25, string.byte(".")) 
    		:move(1000,439) -- 439-(46*(1-1))
    		:setAnchorPoint(cc.p(1.0,0.5))
    		:addTo(self)

    local vip = GlobalUserItem.cbMemberOrder or 0
    local bShowDiscount = vip ~= 0
    self.m_discount = 100
    if vip ~= 0 then
        self.m_discount = GlobalUserItem.MemberList[vip]._shop
    end    
    -- 折扣
    self._txtDiscount = cc.Label:createWithTTF(self.m_discount .. "%折扣", "fonts/round_body.ttf", 25)
            :setAnchorPoint(cc.p(1.0,0.5))
            :move(1142,393) -- 439-(46*(2-1))
            :setTextColor(cc.c4b(255,0,0,255))
            :setVisible(bShowDiscount)
            :addTo(self)
    -- 会员标识
    local sp_vip = cc.Sprite:create("Information/atlas_vipnumber.png")
    if nil ~= sp_vip then
        sp_vip:setPosition(1142 - self._txtDiscount:getContentSize().width - 20, 393)
        self:addChild(sp_vip)
        sp_vip:setTextureRect(cc.rect(28*vip,0,28,26))
        sp_vip:setVisible(bShowDiscount)
    end
    -- 折后价格
    self._txtPrice2 = cc.LabelAtlas:_create("0", "Shop/Detail/num_detail_0.png", 19, 25, string.byte(".")) 
    		:move(1000,347) -- 439-(46*(3-1))
    		:setAnchorPoint(cc.p(1.0,0.5))
    		:addTo(self)
    -- 购买价格
    self._txtPrice3 = cc.LabelAtlas:_create("0", "Shop/Detail/num_detail_0.png", 19, 25, string.byte(".")) 
    		:move(1047,301) -- 439-(46*(4-1))
    		:setAnchorPoint(cc.p(1.0,0.5))
    		:addTo(self)

    self._buyNum = 1
    self:onUpdateNum()

	--功能描述
	cc.Label:createWithTTF("功能："..self._item.description, "fonts/round_body.ttf", 22)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(417,70)
       		:setTextColor(cc.c4b(136,164,224,255))
       		:addTo(self)

    -- 通知监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function ShopDetailLayer:onUserInfoChange( event  )
    print("----------userinfo change notify------------")

    local msgWhat = event.obj

    if nil ~= msgWhat and msgWhat == yl.RY_MSG_USERWEALTH then
        --更新财富
        self:onUpdatePrice()
    end
end

--按键监听
function ShopDetailLayer:onButtonClickedEvent(tag,sender)

	if tag == ShopDetailLayer.BT_ADD then
		self._buyNum = self._buyNum+1
        self.m_editNumber:setText(self._buyNum .. "")
		self:onUpdateNum()
	elseif tag == ShopDetailLayer.BT_MIN then
		if self._buyNum ~= 1 then
            self.m_editNumber:setText(self._buyNum .. "")
			self._buyNum = self._buyNum-1
			self:onUpdateNum()
		end
	elseif tag == ShopDetailLayer.BT_BUY1 then
        if GlobalUserItem.cbInsureEnabled == 0 and self._type == yl.CONSUME_TYPE_GOLD then
            showToast(self, "未设置银行密码，无法使用", 3)
            return
        end
		self._toUse = 0
        self._scene:showPopWait()
		self._shopDetailFrame:onPropertyBuy(self._type,self._buyNum,self._item.id,0)
	elseif tag == ShopDetailLayer.BT_BUY2 then
        if GlobalUserItem.cbInsureEnabled == 0 and self._type == yl.CONSUME_TYPE_GOLD then
            showToast(self, "未设置银行密码，无法使用", 3)
            return
        end
        
        self._scene:showPopWait()
        --判断是否是消耗小喇叭
        if self._item.id == yl.SMALL_TRUMPET then
        else
            self._toUse = 1
            self._shopDetailFrame:onPropertyBuy(self._type,self._buyNum,self._item.id,1)
        end
    elseif tag == ShopDetailLayer.BT_BLANK then
        self.m_editNumber:setVisible(true)
        self.m_editNumber:touchDownAction(self.m_editNumber, ccui.TouchEventType.ended)
	end
end

function ShopDetailLayer:onSelectedEvent(tag,sender,eventType)

	if self._select == tag then
		self:getChildByTag(tag):setSelected(true)
		return
	end

	self._select = tag
	self:onUpdatePrice()
	self:onUpdateNum()

	for i=1,ShopDetailLayer.CBT_LOVELINESS do
		if i ~= tag then
			if self:getChildByTag(i) then
				self:getChildByTag(i):setSelected(false)
			end
		end
	end

end

function ShopDetailLayer:onUpdatePrice()
	self._priceTag1:setTexture("Shop/Detail/text_detail_5_"..(self._select-1)..".png")
	self._priceTag2:setTexture("Shop/Detail/text_detail_6_"..(self._select-1)..".png")
	self._priceTag3:setTexture("Shop/Detail/text_detail_6_"..(self._select-1)..".png")
	self._priceTag4:setTexture("Shop/Detail/text_detail_7_"..(self._select-1)..".png")

	local priceStr = ""
	if self._select == ShopDetailLayer.CBT_BEAN then
		self._type = yl.CONSUME_TYPE_CASH
		priceStr = string.formatNumberThousands(GlobalUserItem.dUserBeans,true,"/")
	elseif self._select == ShopDetailLayer.CBT_INGOT then
		self._type = yl.CONSUME_TYPE_USEER_MADEL
		priceStr = string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/")
	elseif self._select == ShopDetailLayer.CBT_GOLD then
		self._type = yl.CONSUME_TYPE_GOLD
		priceStr = string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/")
	elseif self._select == ShopDetailLayer.CBT_LOVELINESS then
		self._type = yl.CONSUME_TYPE_LOVELINESS
		priceStr = string.formatNumberThousands(GlobalUserItem.dwLoveLiness,true,"/")
	end

	self._txtPrice:setString(priceStr)
end

function ShopDetailLayer:onUpdateNum()
    self._txtNum:setVisible(true)
    self._txtNum:setString(string.formatNumberThousands(self._buyNum,true,"."))

	local itemPrice = 0
	if self._select == ShopDetailLayer.CBT_BEAN then
		itemPrice = self._item.bean
	elseif self._select == ShopDetailLayer.CBT_INGOT then
		itemPrice = self._item.ingot
	elseif self._select == ShopDetailLayer.CBT_GOLD then
		itemPrice = self._item.gold
	elseif self._select == ShopDetailLayer.CBT_LOVELINESS then
		itemPrice = self._item.loveliness
	end

	self._txtPrice1:setString(string.formatNumberThousands(itemPrice,true,"."))
	self._txtPrice2:setString(string.formatNumberThousands(itemPrice * (self.m_discount * 0.01) ,true,"."))
	self._txtPrice3:setString(string.formatNumberThousands(itemPrice * self._buyNum  * (self.m_discount * 0.01),true,"."))
end

--操作结果
function ShopDetailLayer:onShopDetailCallBack(result,message)
	print("======== ShopDetailLayer:onShopDetailCallBack ========")
    local bRes = false
	self._scene:dismissPopWait()
	if type(message) == "string" and message ~= "" then
        showToast(self,message,2)		
	end

    --大喇叭购买且消费
    if result == yl.SUB_GP_PROPERTY_BUY_RESULT and 1 == self._toUse and message == yl.LARGE_TRUMPET then 
        self._toUse = 0    
        if nil ~= self._scene.getTrumpetSendLayer then
            bRes = true
            self._scene:getTrumpetSendLayer()
        end        
    end
    return bRes
end

function ShopDetailLayer:onEditEvent(event,editbox)
    if event == "began" then
        self._txtNum:setVisible(false)
    elseif event == "return" then
        local ndst = tonumber(editbox:getText())
        if "number" == type(ndst) then
            self._buyNum = ndst
        end
        editbox:setVisible(false)
        self:onUpdateNum()
    end
end

return ShopDetailLayer