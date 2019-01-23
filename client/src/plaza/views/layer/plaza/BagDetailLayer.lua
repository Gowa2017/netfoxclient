--[[
	背包详细界面
	2016_07_06 Ravioyla
]]

local BagDetailLayer = class("BagDetailLayer", function(scene)
		local bagDetailLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return bagDetailLayer
end)

local BagDetailFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")

BagDetailLayer.BT_USE				= 20
BagDetailLayer.BT_TRANS				= 21
BagDetailLayer.BT_ADD				= 22
BagDetailLayer.BT_MIN				= 23

-- 进入场景而且过渡动画结束时候触发。
function BagDetailLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function BagDetailLayer:onExitTransitionStart()
    return self
end

function BagDetailLayer:ctor(scene, gameFrame)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
        elseif eventType == "exit" then
            if nil ~= self.m_listener then
                self:getEventDispatcher():removeEventListener(self.m_listener)
                self.m_listener = nil
            end

            if self._BagDetailFrame:isSocketServer() then
                self._BagDetailFrame:onCloseSocket()
            end
            if nil ~= self._BagDetailFrame._gameFrame then
                self._BagDetailFrame._gameFrame._shotFrame = nil
                self._BagDetailFrame._gameFrame = nil
            end
		end
	end)

	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --网络回调
    local BagDetailCallBack = function(result,message)
		this:onBagDetailCallBack(result,message)
	end

	--网络处理
	self._BagDetailFrame = BagDetailFrame:create(self,BagDetailCallBack)
    self._BagDetailFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._BagDetailFrame
    end

    self._item = GlobalUserItem.useItem
    self._useNum = 1

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
        self:addChild(sp)
        sp:setPosition(210,458)
    end

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("text_public_".. self._item._index ..".png")
    if nil ~= sp then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        self:addChild(sp)
        sp:setPosition(210,330)
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

	ccui.Button:create("Bag/bt_present_0.png","Bag/bt_present_1.png")
			:move(208,158)
			:setTag(BagDetailLayer.BT_TRANS)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	ccui.Button:create("Bag/bt_use_0.png","Bag/bt_use_1.png")
			:move(844,241)
			:setTag(BagDetailLayer.BT_USE)
			:addTo(self)
			:addTouchEventListener(self._btcallback)

	display.newSprite("Bag/text_detail_0.png")
			:move(533,460)
			:addTo(self)
	display.newSprite("Bag/text_detail_1.png")
			:move(533,360)
			:addTo(self)

	display.newSprite("Shop/Detail/frame_detail_2.png")
			:move(853,460)
			:addTo(self)
	ccui.Button:create("Shop/Detail/bt_detail_min.png","Shop/Detail/bt_detail_min.png")
			:move(633,460)
			:setTag(BagDetailLayer.BT_MIN)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	ccui.Button:create("Shop/Detail/bt_detail_add.png","Shop/Detail/bt_detail_add.png")
			:move(1065,460)
			:setTag(BagDetailLayer.BT_ADD)
			:addTo(self)
			:addTouchEventListener(self._btcallback)
	self._txtBuy = cc.LabelAtlas:_create("1", "Shop/Detail/num_detail_0.png", 19, 25, string.byte("."))
    		:move(853,460)
    		:setAnchorPoint(cc.p(0.5,0.5))
    		:addTo(self)

    display.newSprite("Shop/Detail/frame_detail_2.png")
			:move(853,360)
			:addTo(self)

	local area = ""
	if bit:_and(self._item._area,yl.PT_ISSUE_AREA_PLATFORM) then
		area = "大厅适用 "
	end
	if bit:_and(self._item._area,yl.PT_ISSUE_AREA_SERVER) then
		area = area.."房间适用 "
	end
	if bit:_and(self._item._area,yl.PT_ISSUE_AREA_GAME) then
		area = area.."游戏适用 "
	end
	cc.Label:createWithTTF(area, "fonts/round_body.ttf", 22)
        	:setAnchorPoint(cc.p(0.0,0.5))
        	:move(620,360)
       		:setTextColor(cc.c4b(195,199,239,255))
       		:addTo(self)

    --右侧剩余
	display.newSprite("Bag/text_detail_2.png")
			:setAnchorPoint(cc.p(0.0,0.5))
			:move(1100,460)
			:addTo(self)
	self._txtNum3 = cc.LabelAtlas:_create(""..self._item._count, "Bag/num_1.png", 18, 23, string.byte("0"))
    		:move(1170,460)
    		:setAnchorPoint(cc.p(0.0,0.5))
    		:addTo(self)

	--功能描述
	cc.Label:createWithTTF("功能："..self._item._info, "fonts/round_body.ttf", 22)
        	:setAnchorPoint(cc.p(0.5,0.5))
        	:move(yl.WIDTH/2,70)
       		:setTextColor(cc.c4b(136,164,224,255))
       		:addTo(self)

    self:onUpdateNum()

    --通知监听
    local function eventListener(event)
        if self._item._index == yl.LARGE_TRUMPET then
            self._item._count = GlobalUserItem.nLargeTrumpetCount
        end
        self:onUpdateNum()
    end
    self.m_listener = cc.EventListenerCustom:create(yl.TRUMPET_COUNT_UPDATE_NOTIFY, eventListener)
    self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_listener, 1)
end

--按键监听
function BagDetailLayer:onButtonClickedEvent(tag,sender)

	if tag == BagDetailLayer.BT_ADD then
		if self._useNum < self._item._count then
			self._useNum = self._useNum+1
			self:onUpdateNum()
		end
	elseif tag == BagDetailLayer.BT_MIN then
		if self._useNum ~= 1 then
			self._useNum = self._useNum-1
			self:onUpdateNum()
		end
	elseif tag == BagDetailLayer.BT_USE then
        --判断是否是消耗大小喇叭
        if self._item._index == yl.LARGE_TRUMPET or self._item._index == yl.SMALL_TRUMPET then
            if self._item._index == yl.LARGE_TRUMPET and nil ~= self._scene.getTrumpetSendLayer then           
                self._scene:getTrumpetSendLayer()
            end
        else
            self._scene:showPopWait()        
            self._BagDetailFrame:onPropertyUse(self._item._index,self._useNum)
        end		
	elseif tag == BagDetailLayer.BT_TRANS then
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BAGTRANS)
	end

end

function BagDetailLayer:onUpdateNum()

	self._txtBuy:setString(string.formatNumberThousands(self._useNum,true,"/"))
	self._txtNum1:setString(""..self._item._count)
	self._txtNum2:setString(""..self._item._count)
	self._txtNum3:setString(""..self._item._count-self._useNum)

end

--操作结果
function BagDetailLayer:onBagDetailCallBack(result,message)

	print("======== BagDetailLayer:onBagDetailCallBack ========")

	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2);
	end

	if result==2 then
		self._item._count = self._item._count-self._useNum
		self._useNum = 1
		self:onUpdateNum()

		if self._item._count < 1 then
			self._scene:onKeyBack()
		end
	end

end

return BagDetailLayer
