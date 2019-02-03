--[[
	背包界面
	2016_07_06 Ravioyla
]]

local BagLayer = class("BagLayer", function(scene)
		local bagLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return bagLayer
end)

local ShopDetailFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")

BagLayer.CBT_GEM	= 1
BagLayer.CBT_CARD	= 2
BagLayer.CBT_ITEM	= 3
BagLayer.CBT_GIFT	= 4

BagLayer.BT_GEM		= 100
BagLayer.BT_CARD	= BagLayer.BT_GEM+200
BagLayer.BT_ITEM	= BagLayer.BT_CARD+200
BagLayer.BT_GIFT	= BagLayer.BT_ITEM+200

-- 进入场景而且过渡动画结束时候触发。
function BagLayer:onEnterTransitionFinish()
	self._scene:showPopWait()
	self._shopDetailFrame:onQuerySend()

    return self
end

-- 退出场景而且开始过渡动画时候触发。
function BagLayer:onExitTransitionStart()
    return self
end

function BagLayer:ctor(scene, gameFrame)
	
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
    local bagCallBack = function(result,message)
		this:onBagCallBack(result,message)
	end

	--网络处理
	self._shopDetailFrame = ShopDetailFrame:create(self,bagCallBack)
    self._shopDetailFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._shopDetailFrame
    end

    self._select = BagLayer.CBT_GEM

    --显示队列
	self._showList = {}
	--数据队列
    self._gemList  = {}
    self._cardList = {}
    self._itemList = {}
    self._giftList = {}

    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
    end
	display.newSprite("Bag/title_bag.png")
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

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
        self:addChild(sp)
    end
    display.newSprite("Bag/frame_1.png")
    	:move(806,320)
    	:addTo(self)
    display.newSprite("Bag/frame_2.png")
    	:move(178,320)
    	:addTo(self)

    --宝石
    ccui.CheckBox:create("Bag/cbt_0_0.png","","Bag/cbt_0_1.png","","")
		:move(190,470)
		:addTo(self)
		:setSelected(true)
		:setTag(BagLayer.CBT_GEM)
		:addEventListener(cbtlistener)

	--卡片
    ccui.CheckBox:create("Bag/cbt_1_0.png","","Bag/cbt_1_1.png","","")
		:move(190,470-104)
		:addTo(self)
		:setSelected(false)
		:setTag(BagLayer.CBT_CARD)
		:addEventListener(cbtlistener)

	--道具
    ccui.CheckBox:create("Bag/cbt_2_0.png","","Bag/cbt_2_1.png","","")
		:move(190,470-104*2)
		:addTo(self)
		:setSelected(false)
		:setTag(BagLayer.CBT_ITEM)
		:addEventListener(cbtlistener)

	--礼物
    ccui.CheckBox:create("Bag/cbt_3_0.png","","Bag/cbt_3_1.png","","")
		:move(190,470-104*3)
		:addTo(self)
		:setSelected(false)
		:setTag(BagLayer.CBT_GIFT)
		:addEventListener(cbtlistener)

	self._scrollView = ccui.ScrollView:create()
									  :setContentSize(cc.size(938,458))
									  :setAnchorPoint(cc.p(0.5, 0.5))
									  :setPosition(cc.p(806, 320))
									  :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
									  :setBounceEnabled(true)
									  :setScrollBarEnabled(false)
									  :addTo(self)

end

--按键监听
function BagLayer:onButtonClickedEvent(tag,sender)
    local beginPos = sender:getTouchBeganPosition()
    local endPos = sender:getTouchEndPosition()
    if math.abs(endPos.x - beginPos.x) > 30 
        or math.abs(endPos.y - beginPos.y) > 30 then
        print("BagLayer:onButtonClickedEvent ==> MoveTouch Filter")
        return
    end
	print("***** button clicked-"..tag.." ******")

	if (tag>BagLayer.BT_GEM) and (tag<BagLayer.BT_CARD) then
		GlobalUserItem.useItem = self._gemList[tag-BagLayer.BT_GEM]
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BAGDETAIL)
	elseif (tag>BagLayer.BT_CARD) and (tag<BagLayer.BT_ITEM) then
		GlobalUserItem.useItem = self._cardList[tag-BagLayer.BT_CARD]
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BAGDETAIL)
	elseif (tag>BagLayer.BT_ITEM) and (tag<BagLayer.BT_GIFT) then
		GlobalUserItem.useItem = self._itemList[tag-BagLayer.BT_ITEM]
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BAGDETAIL)
	elseif (tag>BagLayer.BT_GIFT) and (tag<BagLayer.BT_GIFT+200) then
		showToast(self,"手机端暂不支持礼物道具，请前往PC客户端使用！",2);
	end

end

function BagLayer:onSelectedEvent(tag,sender,eventType)

	if self._select == tag then
		self:getChildByTag(tag):setSelected(true)
		return
	end

	self._select = tag

	for i=1,4 do
		if i ~= tag then
			self:getChildByTag(i):setSelected(false)
		end
	end

	--刷新界面
	self:onClearShowList()
	self:onUpdateShowList()

end

--操作结果
function BagLayer:onBagCallBack(result,message)
	print("======== BagLayer:onBagCallBack ========")

	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" and result ~= 5 then
		showToast(self,message,2);
	end

	if result==yl.SUB_GP_QUERY_BACKPACKET_RESULT then
		if #message == 0 then
			showToast(self, "背包为空", 2)
			return
		end

		self._gemList  = {}
    	self._cardList = {}
    	self._itemList = {}
   		self._giftList = {}

		for i=1,#message do
			local item = message[i]
			if math.floor(item._index/100) == 0 then
				table.insert(self._giftList,item)
			elseif math.floor(item._index/100) == 1 then
				table.insert(self._gemList,item)
			elseif math.floor(item._index/100) == 2 then
				table.insert(self._cardList,item)
			elseif math.floor(item._index/100) == 3 then
				table.insert(self._itemList,item)
			end
		end

		--刷新界面
		self:onClearShowList()
		self:onUpdateShowList()

	end

end

--清除当前显示
function BagLayer:onClearShowList()
	for i=1,#self._showList do
		self._showList[i]:removeFromParent()
	end
	self._showList = nil
	self._showList = {}
end

--更新当前显示
function BagLayer:onUpdateShowList()

	local theList = {}
	local tag = 0
	if self._select == BagLayer.CBT_GEM then
		theList = self._gemList
		tag = BagLayer.BT_GEM
	elseif self._select == BagLayer.CBT_CARD then
		theList = self._cardList
		tag = BagLayer.BT_CARD
	elseif self._select == BagLayer.CBT_ITEM then
		theList = self._itemList
		tag = BagLayer.BT_ITEM
	elseif self._select == BagLayer.CBT_GIFT then
		theList = self._giftList
		tag = BagLayer.BT_GIFT
	end

	--计算scroll滑动高度
	local scrollHeight = 0
	if #theList<19 then
		scrollHeight = 458
		self._scrollView:setInnerContainerSize(cc.size(938, 458+20))
	else
		scrollHeight = 155*math.floor((#theList+math.floor(#theList%6))/6)
		self._scrollView:setInnerContainerSize(cc.size(938, scrollHeight+20))
	end

	for i=1,#theList do
		local item = theList[i]
		self._showList[i] = cc.LayerColor:create(cc.c4b(100, 100, 100, 0), 143, 143)
    		:move(80+math.floor((i-1)%6)*154-143/2,scrollHeight-(80+math.floor((i-1)/6)*154)-143/2+20)
    		:addTo(self._scrollView)

		ccui.Button:create("Bag/frame_3.png","Bag/frame_3.png")
			:setContentSize(cc.size(143, 143))
			:move(143/2,143/2)
			:setTag(tag+i)
			:addTo(self._showList[i])
            :setSwallowTouches(false)
			:addTouchEventListener(self._btcallback)

        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("icon_public_"..item._index..".png")
        if nil ~= frame then
            local sp = cc.Sprite:createWithSpriteFrame(frame)
            sp:setPosition(71.5, 71.5)
            self._showList[i]:addChild(sp)
        end

		cc.LabelAtlas:_create(""..item._count, "Bag/num_0.png", 20, 25, string.byte("0"))
    		:setAnchorPoint(cc.p(1.0,0.5))
    		:move(128,25)
    		:addTo(self._showList[i])

	end
end

return BagLayer