--[[
	签到界面
	2016_06_16 Ravioyla
]]

 local CheckinLayer = class("CheckinLayer", function(scene)
		local checkinLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return checkinLayer
end)

local CheckinFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.CheckinFrame")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

CheckinLayer.CHECKIN_NUMBER		= 7 		--连续签到天数

CheckinLayer.BT_EXIT			= 3
CheckinLayer.BT_VIPGIFT			= 4
CheckinLayer.BT_CHECKIN			= 5
CheckinLayer.BG_CHECKIN			= 6
CheckinLayer.ICON_CHECKIN_Y		= CheckinLayer.BG_CHECKIN+7
CheckinLayer.ICON_CHECKIN_N		= CheckinLayer.ICON_CHECKIN_Y+7
CheckinLayer.ICON_CHECKIN_DONE	= CheckinLayer.ICON_CHECKIN_N+7
CheckinLayer.ICON_GOLD			= CheckinLayer.ICON_CHECKIN_DONE+7
CheckinLayer.TEXT_GOLD			= CheckinLayer.ICON_GOLD+7
CheckinLayer.TEXT_DAY			= CheckinLayer.TEXT_GOLD+7
CheckinLayer.NUM_DAY			= CheckinLayer.TEXT_DAY+7
CheckinLayer.ICON_CHECKIN_DBG   = CheckinLayer.NUM_DAY+7

-- 进入场景而且过渡动画结束时候触发。
function CheckinLayer:onEnterTransitionFinish()
	if false == GlobalUserItem.bQueryCheckInData then
		self._scene:showPopWait()
		self._checkinFrame:onCheckinQuery()
	else
		self:onCheckinCallBack(1, nil, nil)
	end
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function CheckinLayer:onExitTransitionStart()
    return self
end

function CheckinLayer:onExit()
	if self._checkinFrame:isSocketServer() then
		self._checkinFrame:onCloseSocket()
	end
end

function CheckinLayer:ctor(scene)
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)
	
	local this = self

	self._scene = scene

	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	
	--网络回调
    local checkinCallBack = function(result,message,subMessage)
		return this:onCheckinCallBack(result,message,subMessage)
	end

	--网络处理
	self._checkinFrame = CheckinFrame:create(self,checkinCallBack)

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
	end
	display.newSprite("Checkin/title_checkin.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,326)
        self:addChild(sp)
	end
	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_2.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,326)
        self:addChild(sp)
	end

	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)

	--连续签到提示
	display.newSprite("Checkin/frame_checkin_2.png")
		:move(yl.WIDTH/2,555)
		:addTo(self)
	display.newSprite("Checkin/text_checkin_0.png")
		:move(yl.WIDTH/2,555)
		:addTo(self)
	self._txtDay = cc.LabelAtlas:_create("0", "Checkin/num_checkin_0.png", 22, 29, string.byte("0"))
    		:move(760,555)
    		:setAnchorPoint(cc.p(0.5,0.5))
    		:addTo(self)

    --签到按钮
    self._btConfig = ccui.Button:create("Checkin/bt_checkin_0.png","Checkin/bt_checkin_1.png","Checkin/bt_checkin_2.png")
	self._btConfig:move(yl.WIDTH/2,112)
	self._btConfig:setTag(CheckinLayer.BT_CHECKIN)		
	self._btConfig:addTo(self)
	self._btConfig:addTouchEventListener(self._btcallback)	
	self._btConfig:setEnabled(GlobalUserItem.wSeriesDate == 0)

	--签到展示区域
	for i=0,CheckinLayer.CHECKIN_NUMBER-1 do
		-- 签到背景
		display.newSprite("Checkin/back_checkin_frame.png")
		:move(178+163*i,326)
		:setTag(CheckinLayer.ICON_CHECKIN_DBG+i)
		:addTo(self)

		--背景
		display.newSprite("Checkin/back_checkin_0.png")
		:move(178+163*i,326)
		:setTag(CheckinLayer.BG_CHECKIN+i)
		:addTo(self)

		--天数背景
		display.newSprite("Checkin/text_checkin_1.png")
		:move(178+163*i,470)
		:setTag(CheckinLayer.TEXT_DAY+i)
		:addTo(self)

		--天数数字
		cc.LabelAtlas:_create(""..i+1, "Checkin/num_checkin_1.png", 19, 25, string.byte("0"))
    		:move(178+163*i,470)
    		:setAnchorPoint(cc.p(0.5,0.5))
    		:setTag(CheckinLayer.NUM_DAY+i)
    		:addTo(self)

    	--已签到标志
    	display.newSprite("Checkin/icon_checkin_1.png")
		:move(182+163*i,258)
		:setTag(CheckinLayer.ICON_CHECKIN_DONE+i)
		:setVisible(false)
		:addTo(self)

		--可签到标志
		display.newSprite("Checkin/text_checkin_2.png")
		:move(178+163*i,245)
		:setTag(CheckinLayer.ICON_CHECKIN_Y+i)
		:addTo(self)

		--不可签标志
    	display.newSprite("Checkin/text_checkin_3.png")
		:move(178+163*i,245)
		:setTag(CheckinLayer.ICON_CHECKIN_N+i)
		:setVisible(false)
		:addTo(self)

		--金币标志
		display.newSprite("Checkin/icon_checkin_0.png")
		:move(140+163*i,192)
		:setTag(CheckinLayer.ICON_GOLD+i)
		:addTo(self)

		--金币数字
		cc.LabelAtlas:_create("1000", "Checkin/num_checkin_3.png", 12, 16, string.byte("0"))
    		:move(183+163*i,192)
    		:setAnchorPoint(cc.p(0.5,0.5))
    		:setTag(CheckinLayer.TEXT_GOLD+i)
    		:addTo(self)
	end
	--VIP礼包按钮
	self._btVIPGift = ccui.Button:create("Checkin/bt_checkin_vip_1.png","Checkin/bt_checkin_vip_1.png")
		:move(1013,115)
		:setTag(CheckinLayer.BT_VIPGIFT)
		:addTo(self)
		:addTouchEventListener(self._btcallback)

	print("*cbMemberOrder-"..GlobalUserItem.cbMemberOrder)
	if GlobalUserItem.cbMemberOrder == 0 then
		self:getChildByTag(CheckinLayer.BT_VIPGIFT):loadTextureNormal("Checkin/bt_checkin_vip_0.png")
		self:getChildByTag(CheckinLayer.BT_VIPGIFT):loadTexturePressed("Checkin/bt_checkin_vip_0.png")
	end

	--礼物列表
	self.m_giftList = nil
	self.m_spListFrame = nil
	self.m_memberGiftLayer = nil
	self.m_tabGiftList = {}
	self.m_fSix = 0
end

--按键监听
function CheckinLayer:onButtonClickedEvent(tag,sender)
	if tag == CheckinLayer.BT_CHECKIN then
		self._scene:showPopWait()
		ExternalFun.enableBtn(self._btConfig, false)
		self._checkinFrame:onCheckinDone()		
	elseif tag == CheckinLayer.BT_VIPGIFT then
		if 0 == GlobalUserItem.cbMemberOrder then
			showToast(self, "非vip会员无法获取礼包", 2)
			return
		end
		self._scene:showPopWait()
		self._checkinFrame:onCheckMemberGift()
	end
end

--操作结果
function CheckinLayer:onCheckinCallBack(result, message, subMessage)
	local bRes = false
	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2)
	end

	if result == 1 then
		self:reloadCheckin()
	end

	if result == 10 and GlobalUserItem.bTodayChecked then
		self._scene:coinDropDownAni(function()
			ExternalFun.enableBtn(self._btConfig, not GlobalUserItem.bTodayChecked)
			self:reloadCheckin()
			local reword = GlobalUserItem.lRewardGold[GlobalUserItem.wSeriesDate] or 0
			showToast(self,"恭喜您获得" .. reword .. "游戏币" ,2)
		end)
	end

	--vip礼包查询结果
	if self._checkinFrame.QUERYMEMBERGIFT == result then
		ExternalFun.enableBtn(self._btConfig, false, true)
		self:reloadGiftList(message, subMessage)
	end

	--vip礼包领取结果
	if self._checkinFrame.GETMEMBERGIFT == result then
		--会员标记当日已签到
		if GlobalUserItem.cbMemberOrder ~= 0 then
			GlobalUserItem.setTodayCheckIn()
		end

		if nil ~= self.m_memberGiftLayer then
			self.m_memberGiftLayer:setVisible(false)
		end
		if GlobalUserItem.bTodayChecked == true then
			ExternalFun.enableBtn(self._btConfig, false)
			self._btConfig:setVisible(true)
		else
			ExternalFun.enableBtn(self._btConfig, true)
		end

		if 0 ~= GlobalUserItem.MemberList[GlobalUserItem.cbMemberOrder]._present then
			-- 领取送金
			self._checkinFrame:sendGetVipPresend()
			bRes = true
		end		
	end
	return bRes
end

function CheckinLayer:reloadCheckin()
	print("====== CheckinLayer:reloadCheckin =======")

	local todayCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_Y+GlobalUserItem.wSeriesDate)
	if nil ~= todayCheck then
		todayCheck:setVisible(not GlobalUserItem.bTodayChecked)
	end
	todayCheck = self:getChildByTag(CheckinLayer.BG_CHECKIN+GlobalUserItem.wSeriesDate)
	if nil ~= todayCheck and GlobalUserItem.bTodayChecked then
		todayCheck:setTexture("Checkin/back_checkin_1.png")
	end
	local canCheckBg = self:getChildByTag(CheckinLayer.ICON_CHECKIN_DBG+GlobalUserItem.wSeriesDate)
	if nil ~= canCheckBg then
		canCheckBg:setVisible(not GlobalUserItem.bTodayChecked)
	end

	local otherCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_N+GlobalUserItem.wSeriesDate)
	if nil ~= otherCheck then
		otherCheck:setVisible(GlobalUserItem.bTodayChecked)
	end

	local haveCheck = nil
	if GlobalUserItem.wSeriesDate ~= 0 then
		for i=0,GlobalUserItem.wSeriesDate-1 do
			self:getChildByTag(CheckinLayer.BG_CHECKIN+i):setTexture("Checkin/back_checkin_1.png")
			self:getChildByTag(CheckinLayer.TEXT_DAY+i):setTexture("Checkin/text_checkin_4.png")
			todayCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_Y+i)
			if nil ~= todayCheck then
				todayCheck:setVisible(false)
			end
			haveCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_DONE+i)
			if nil ~= haveCheck then
				haveCheck:setVisible(true)
			end
			canCheckBg = self:getChildByTag(CheckinLayer.ICON_CHECKIN_DBG+i)
			if nil ~= canCheckBg then
				canCheckBg:setVisible(false)
			end
			self:getChildByTag(CheckinLayer.ICON_GOLD+i):setTexture("Checkin/icon_checkin_2.png")
			local gold = GlobalUserItem.lRewardGold[i+1] or 0
			self:getChildByTag(CheckinLayer.TEXT_GOLD+i):setString("" .. gold)
		end		
	end
	for i = GlobalUserItem.wSeriesDate + 1, 6 do
		self:getChildByTag(CheckinLayer.BG_CHECKIN+i):setTexture("Checkin/back_checkin_1.png")

		todayCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_Y+i)
		if nil ~= todayCheck then
			todayCheck:setVisible(false)
		end
		canCheckBg = self:getChildByTag(CheckinLayer.ICON_CHECKIN_DBG+i)
		if nil ~= canCheckBg then
			canCheckBg:setVisible(false)
		end

		otherCheck = self:getChildByTag(CheckinLayer.ICON_CHECKIN_N+i)
		if nil ~= otherCheck then
			otherCheck:setVisible(true)
		end
	end

	self._txtDay:setString(""..GlobalUserItem.wSeriesDate)

	if GlobalUserItem.bTodayChecked == true then		
		ExternalFun.enableBtn(self._btConfig, false)
		self._btConfig:setVisible(true)
	else
		ExternalFun.enableBtn(self._btConfig, true)
	end

	for i=0,CheckinLayer.CHECKIN_NUMBER-1 do
		local reword = GlobalUserItem.lRewardGold[i+1] or 0
		self:getChildByTag(CheckinLayer.TEXT_GOLD+i):setString("".. reword .."")
	end

	print("====== CheckinLayer:reloadCheckin =======")

end

function CheckinLayer:reloadGiftList( tabGift ,subMessage)
	self.m_tabGiftList = tabGift

	if nil == self.m_giftList then
		--加载csb资源
		local rootLayer, csbNode = ExternalFun.loadRootCSB("Checkin/MemberGiftLayer.csb", self)
		self.m_memberGiftLayer = rootLayer

		--背景框
		self.m_spListFrame = csbNode:getChildByName("frame_vip")

		--列表
		local content = self.m_spListFrame:getChildByName("content")
		self.m_fSix = content:getContentSize().width / 6
		local m_tableView = cc.TableView:create(content:getContentSize())
		m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
		m_tableView:setPosition(content:getPosition())
		m_tableView:setDelegate()
		m_tableView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
		m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
		m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
		self.m_spListFrame:addChild(m_tableView)
		self.m_giftList = m_tableView
		content:removeFromParent()

		--领取按钮
		self.m_btnGetGift = csbNode:getChildByName("sure_btn")
		self.m_btnGetGift:addTouchEventListener(function ( sender, tType )
			if tType == ccui.TouchEventType.ended then
	         	self._scene:showPopWait()
				self._checkinFrame:onGetMemberGift()
	        end
		end)
	end
	self.m_memberGiftLayer:setVisible(true)
	self.m_giftList:reloadData()

	subMessage = subMessage or false
	self.m_btnGetGift:setEnabled(subMessage)
end

function CheckinLayer:getGiftCount(  )
	if nil == self.m_tabGiftList then
		return 0
	end
	return #self.m_tabGiftList
end

function CheckinLayer:onTouchBegan(touch, event)
	if nil ~= self.m_memberGiftLayer and self.m_memberGiftLayer:isVisible() then
		return true
	else
		return false
	end
end

function CheckinLayer:onTouchEnded( touch, event )
	local pos = touch:getLocation()	
	local m_spBg = self.m_spListFrame
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self.m_memberGiftLayer:setVisible(false)

        if GlobalUserItem.bTodayChecked == true then
			ExternalFun.enableBtn(self._btConfig, false)
			self._btConfig:setVisible(true)
		else
			ExternalFun.enableBtn(self._btConfig, true)
		end
    end
end

--tableview
function CheckinLayer:cellSizeForTable( view, idx )
	return self.m_fSix, 112
end

function CheckinLayer:numberOfCellsInTableView( view )
	return self:getGiftCount()
end

function CheckinLayer:tableCellAtIndex( view, idx )
	local cell = view:dequeueCell()
	if nil == self.m_tabGiftList then
		return cell
	end

	local giftitem = self.m_tabGiftList[idx + 1]
	local item = nil
	if nil == cell then
		cell = cc.TableViewCell:new()
		item = cc.Node:create()
		item:setPosition(self.m_fSix * 0.5, view:getViewSize().height * 0.5)
		item:setName("gift_item_view")
		cell:addChild(item)
	else
		item = cell:getChildByName("gift_item_view")
	end
	if nil ~= item and nil ~= giftitem then
		--图片
		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("icon_public_" .. giftitem.id .. ".png")
		if nil ~= frame then
			local sp = cc.Sprite:createWithSpriteFrame(frame)
			local icon = item:getChildByTag(1)
			if nil == icon then
				icon = cc.Sprite:createWithSpriteFrame(sp:getSpriteFrame())
				item:addChild(icon)
				icon:setTag(1)
			else
				icon:setSpriteFrame(sp:getSpriteFrame())
			end
		end			

		--数量
		str = string.format("%01d", giftitem.count)
		local atlascount = item:getChildByTag(2)
		if nil == atlascount then
			atlascount = ccui.TextAtlas:create(str, "Checkin/num_vip_0.png", 17, 22, "0")
			item:addChild(atlascount)
			atlascount:setTag(2)
			atlascount:setPosition(40, -40)
		else
			atlascount:setString(str)
		end
	end

	return cell
end

return CheckinLayer