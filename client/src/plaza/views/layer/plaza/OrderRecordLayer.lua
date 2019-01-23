--[[
	实物兑换界面
	2016_06_22 Ravioyla
]]

local OrderRecordLayer = class("OrderRecordLayer", function(scene)
		local orderRecordLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return orderRecordLayer
end)

-- 进入场景而且过渡动画结束时候触发。
function OrderRecordLayer:onEnterTransitionFinish()
	self._scene:showPopWait()
	local this = self
	appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=getorderrecord&userid="..GlobalUserItem.dwUserID.."&signature="..GlobalUserItem:getSignature(os.time()).."&time="..os.time().."&number=20&page=1",function(jstable,jsdata)
			this._scene:dismissPopWait()
			dump(jstable, "jstable")		
			if type(jstable) == "table" then				
				local code = jstable["code"]
				if tonumber(code) == 0 then
					local datax = jstable["data"]
					if type(datax) == "table" then
						local valid = datax["valid"]
						if valid == true then
							local listcount = datax["total"]
							local list = datax["list"]
							if type(list) == "table" then
								for i=1,#list do
									local item = {}
						            item.tradeType = list[i]["OrderStatusDescription"]
						            item.name = list[i]["AwardName"]
						            item.price = tonumber(list[i]["TotalAmount"])
						            item.count = tonumber(list[i]["AwardCount"])
						            item.date = GlobalUserItem:getDateNumber(list[i]["BuyDate"])
						            table.insert(self._OrderRecordList,item)
								end
							end
						end
					end
				end

				this:onUpdateShow()
			else
				showToast(this,"抱歉，获取订单记录信息失败！",2,cc.c3b(250,0,0))
			end
		end)
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function OrderRecordLayer:onExitTransitionStart()
    return self
end

function OrderRecordLayer:ctor(scene)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		end
	end)

	self._OrderRecordList = {}

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,yl.HEIGHT - 51)
		self:addChild(sp)
	end
	display.newSprite("OrderRecord/title_orderrecord.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,326)
		self:addChild(sp)
	end
	display.newSprite("BankRecord/frame_back_2.png")
		:move(yl.WIDTH/2,326)
		:addTo(self)

	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)

	--标题列
	display.newSprite("OrderRecord/text_orderrecord_0.png")
		:move(118+80+10,550)
		:addTo(self)
	display.newSprite("OrderRecord/text_orderrecord_1.png")
		:move(350+80+30,550)
		:addTo(self)
	display.newSprite("OrderRecord/text_orderrecord_2.png")
		:move(585+80,550)
		:addTo(self)
	display.newSprite("OrderRecord/text_orderrecord_3.png")
		:move(818+80-30,550)
		:addTo(self)
	display.newSprite("OrderRecord/text_orderrecord_4.png")
		:move(1050+80-20,550)
		:addTo(self)

	--无记录提示
	self._nullTipLabel = cc.Label:createWithTTF("没有兑换记录","fonts/round_body.ttf",32)
			:move(yl.WIDTH/2,326)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			-- :setVisible(false)
			:addTo(self)

	--记录列表
	self._listView = cc.TableView:create(cc.size(1161, 454))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(90,62))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(self.tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

	display.newSprite("BankRecord/frame_back_1.png")
		:move(yl.WIDTH/2,326)
		:addTo(self)

end

function OrderRecordLayer:onUpdateShow()

	if 0 == #self._OrderRecordList then
		self._nullTipLabel:setVisible(true)
	else
		self._nullTipLabel:setVisible(false)
	end

	self._listView:reloadData()

end

---------------------------------------------------------------------

--子视图大小
function OrderRecordLayer.cellSizeForTable(view, idx)
  	return 1161 , 76
end

--子视图数目
function OrderRecordLayer.numberOfCellsInTableView(view)
	return #view:getParent()._OrderRecordList
end
	
--获取子视图
function OrderRecordLayer.tableCellAtIndex(view, idx)	
	
	local cell = view:dequeueCell()
	
	local item = view:getParent()._OrderRecordList[idx+1]

	local width = 1161
	local height= 76

	if not cell then
		cell = cc.TableViewCell:new()
		display.newSprite("BankRecord/table_bankrecord_cell_"..(idx%2)..".png")
		:move(width/2,height/2)
		:addTo(cell)

		--日期
		local date = os.date("%Y-%m-%d %H:%M", tonumber(item.date)/1000)
		cc.Label:createWithTTF(date,"fonts/round_body.ttf",28)
			:move(118,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)

		cc.Label:createWithTTF(item.name,"fonts/round_body.ttf",28)
			:move(350+20,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)

		cc.Label:createWithTTF(string.formatNumberThousands(item.count,true,","),"fonts/round_body.ttf",28)
			:move(585-10,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)

		cc.Label:createWithTTF(string.formatNumberThousands(item.price,true,",").."元宝","fonts/round_body.ttf",28)
			:move(818-40,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)

		cc.Label:createWithTTF(item.tradeType,"fonts/round_body.ttf",28)
			:move(1050-30,height/2)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setTextColor(cc.c4b(206,175,255,255))
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)
	end

	return cell
end
---------------------------------------------------------------------
return OrderRecordLayer