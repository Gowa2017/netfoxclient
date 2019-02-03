--[[
	背包赠送界面
	2016_07_08 Ravioyla
]]

local BagTransLayer = class("BagTransLayer", function(scene)
		local BagTransLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return BagTransLayer
end)

local BagTransFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")

BagTransLayer.CBT_USERID			= 10
BagTransLayer.CBT_NICKNAME			= 11

BagTransLayer.BT_TRANS				= 21
BagTransLayer.BT_ADD				= 22
BagTransLayer.BT_MIN				= 23

-- 进入场景而且过渡动画结束时候触发。
function BagTransLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function BagTransLayer:onExitTransitionStart()
    return self
end

function BagTransLayer:ctor(scene, gameFrame)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
        elseif eventType == "exit" then
            if self._BagTransFrame:isSocketServer() then
                self._BagTransFrame:onCloseSocket()
            end
            if nil ~= self._BagTransFrame._gameFrame then
                self._BagTransFrame._gameFrame._shotFrame = nil
                self._BagTransFrame._gameFrame = nil
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
    local BagTransCallBack = function(result,message)
		this:onBagTransCallBack(result,message)
	end

	--网络处理
	self._BagTransFrame = BagTransFrame:create(self,BagTransCallBack)
    self._BagTransFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._BagTransFrame
    end

    self._item = GlobalUserItem.useItem
    self._transNum = 1
    self._type = yl.PRESEND_GAMEID

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
    	:move(210,458)
    	:addTo(self)

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("icon_public_".. self._item._index ..".png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(210,458)
        self:addChild(sp)
    end

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("text_public_".. self._item._index ..".png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(210,330)
        self:addChild(sp)
    end

	self._txtNum1 = cc.LabelAtlas:_create(""..self._item._count, "Bag/num_0.png", 20, 25, string.byte("0"))
    		:setAnchorPoint(cc.p(1.0,0.0))
    		:move(266,400)
    		:addTo(self)

	--数量
	display.newSprite("Bag/text_detail_3.png")
			:move(180,277)
			:addTo(self)
	self._txtNum2 = cc.LabelAtlas:_create(""..self._item._count, "Bag/num_3.png", 21, 26, string.byte("0"))
    		:setAnchorPoint(cc.p(0.0,0.5))
    		:move(246,277)
    		:addTo(self)

    --文字标签
	display.newSprite("Bag/text_detail_4.png")
			:setAnchorPoint(cc.p(0.0,0.5))
			:move(610,485)
			:addTo(self)
	display.newSprite("Bag/text_detail_5.png")
			:setAnchorPoint(cc.p(0.0,0.5))
			:move(845,485)
			:addTo(self)

    --依据ID
    ccui.CheckBox:create("Bag/cbt_choose_0.png","","Bag/cbt_choose_1.png","","")
		:move(587,485)
		:addTo(self)
		:setSelected(true)
		:setTag(BagTransLayer.CBT_USERID)
		:addEventListener(cbtlistener)
	--依据昵称
    ccui.CheckBox:create("Bag/cbt_choose_0.png","","Bag/cbt_choose_1.png","","")
		:move(824,485)
		:addTo(self)
		:setSelected(false)
		:setTag(BagTransLayer.CBT_NICKNAME)
		:addEventListener(cbtlistener)

	ccui.Button:create("Bag/bt_detail_present_0.png","Bag/bt_detail_present_1.png")
			:move(837,206)
			:setTag(BagTransLayer.BT_TRANS)
			:addTo(self)
			:addTouchEventListener(self._btcallback)

	--文字标签
	display.newSprite("Bag/text_detail_6.png")
			:move(533,408)
			:addTo(self)
	display.newSprite("Bag/text_detail_7.png")
			:move(533,318)
			:addTo(self)

	--接收玩家
	self.edit_trans = ccui.EditBox:create(cc.size(481,49), ccui.Scale9Sprite:create("Shop/Detail/frame_detail_2.png"))
		:move(853,408)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:addTo(self)

	--赠送数量
	display.newSprite("Shop/Detail/frame_detail_2.png")
			:move(853,318)
			:addTo(self)
	ccui.Button:create("Shop/Detail/bt_detail_min.png","Shop/Detail/bt_detail_min.png")
			:move(633,318)
			:setTag(BagTransLayer.BT_MIN)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	ccui.Button:create("Shop/Detail/bt_detail_add.png","Shop/Detail/bt_detail_add.png")
			:move(1065,318)
			:setTag(BagTransLayer.BT_ADD)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	self._txtBuy = cc.LabelAtlas:_create("1", "Shop/Detail/num_detail_0.png", 19, 25, string.byte("."))
    		:move(853,318)
    		:setAnchorPoint(cc.p(0.5,0.5))
    		:addTo(self)

    --右侧剩余
	display.newSprite("Bag/text_detail_2.png")
			:setAnchorPoint(cc.p(0.0,0.5))
			:move(1100,318)
			:addTo(self)
	self._txtNum3 = cc.LabelAtlas:_create(""..self._item._count, "Bag/num_1.png", 18, 23, string.byte("0"))
    		:move(1170,318)
    		:setAnchorPoint(cc.p(0.0,0.5))
    		:addTo(self)

	--功能描述
	cc.Label:createWithTTF("功能："..self._item._info, "fonts/round_body.ttf", 22)
        	:setAnchorPoint(cc.p(0.5,0.5))
        	:move(yl.WIDTH/2,70)
       		:setTextColor(cc.c4b(136,164,224,255))
       		:addTo(self)

    self:onUpdateNum()

end

function BagTransLayer:onSelectedEvent(tag,sender,eventType)

	local wType = 0
	if tag == BagTransLayer.CBT_USERID then
		wType = yl.PRESEND_GAMEID
	elseif tag == BagTransLayer.CBT_NICKNAME then
		wType = yl.PRESEND_NICKNAME
	end

	if self._type == wType then
		self:getChildByTag(tag):setSelected(true)
		return
	end

	self._type = wType

	for i=BagTransLayer.CBT_USERID,BagTransLayer.CBT_NICKNAME do
		if i ~= tag then
			self:getChildByTag(i):setSelected(false)
		end
	end

	self.edit_trans:setText("");

end

--按键监听
function BagTransLayer:onButtonClickedEvent(tag,sender)

	if tag == BagTransLayer.BT_ADD then
		if self._transNum < self._item._count then
			self._transNum = self._transNum+1
			self:onUpdateNum()
		end
	elseif tag == BagTransLayer.BT_MIN then
		if self._transNum ~= 1 then
			self._transNum = self._transNum-1
			self:onUpdateNum()
		end
	elseif tag == BagTransLayer.BT_TRANS then
		local szTarget = string.gsub(self.edit_trans:getText(), " ", "")
		if #szTarget < 1 then 
			showToast(self,"请输入赠送用户昵称或ID！",2)
			return
		end
		local gameid = 0
		if self._type == yl.PRESEND_GAMEID then
			gameid = tonumber(szTarget)
			szTarget = ""
			if gameid == 0 or gameid == nil then
				showToast(self,"请输入正确的ID！",2)
				return
			end
		end

		self._scene:showPopWait()
		self._BagTransFrame:onPropertyTrans(self._item._index,self._type,gameid,szTarget,self._transNum)
	end

end

function BagTransLayer:onUpdateNum()

	self._txtBuy:setString(string.formatNumberThousands(self._transNum,true,"/"))
	self._txtNum1:setString(""..self._item._count)
	self._txtNum2:setString(""..self._item._count)
	self._txtNum3:setString(""..self._item._count-self._transNum)

end

--操作结果
function BagTransLayer:onBagTransCallBack(result,message)

	print("======== BagTransLayer:onBagTransCallBack ========")

	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2);
	end

	if result==2 then
		self._item._count = self._item._count-self._transNum
		self._transNum = 1
		self:onUpdateNum()

		if self._item._count < 1 then
			self._scene:onKeyBack()
		end
	end

end

return BagTransLayer
