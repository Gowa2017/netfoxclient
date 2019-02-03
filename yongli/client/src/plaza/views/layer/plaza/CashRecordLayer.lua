--[[
	提现记录界面
	2017_09_09 MXM
]]

local CashRecordLayer = class("CashRecordLayer", function(scene)
		local cashRecordLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return cashRecordLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function CashRecordLayer:getStutas( param )
	if type(param) ~= "number" then
		return
	end
	
	if param == 0 or param > 3 then
		param = 1
	end
	
	local str1 = "申请中"
	local str2 = "已提现"
	local str3 = "拒绝"
	local str4 = "等待付款"
	
	local switch = 
	{
		[1] = function( )
			return  str1
		end,

		[2] = function( )
			return  str2
		end,

		[3] = function( )
			return str3
		end,

		[4] = function( )
			--print("case 4")
			return str4
		end

	}
   self.status = switch[param]
   --self.status = f
   return self.status()
								
end

-- 进入场景而且过渡动画结束时候触发。
function CashRecordLayer:onEnterTransitionFinish()
	self._scene:showPopWait()
	local this = self
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=gettixianrecord&userid="..GlobalUserItem.dwUserID.."&signature="..GlobalUserItem:getSignature(os.time()).."&time="..os.time().."&number=20&page=1",function(jstable,jsdata)
	print("appdf.onHttpJsionTable:"..BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx".."?action=gettixianrecord&userid="..GlobalUserItem.dwUserID.."&signature="..GlobalUserItem:getSignature(os.time()).."&time="..os.time().."&number=20&page=1")
			this._scene:dismissPopWait()
			if jstable then
				local code = jstable["code"]
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
						            item.ApplyDate = GlobalUserItem:getDateNumber(list[i]["ApplyDate"])
						            item.OrderID = list[i]["OrderID"]
						            item.SellMoney = list[i]["SellMoney"]
									item.Status = list[i]["Status"]
									item.RejectReason = list[i]["RejectReason"]
                                    
						            table.insert(self._cashRecordList,item)
								end
							end
						end
					end
				end

				this:onUpdateShow()
			else
				showToast(this,"抱歉，获取提现记录信息失败！",2,cc.c3b(250,0,0))
			end
		end)
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function CashRecordLayer:onExitTransitionStart()
    return self
end

function CashRecordLayer:ctor(scene)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		end
	end)

	self._cashRecordList = {}

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("CashRecord/CashRecordLayer.csb",self)
	
	--返回
	self._return = csbNode:getChildByName("Button_return")
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)
    
    --暂未记录提示
    self._nullTipSprite = csbNode:getChildByName("tip_no_record")
        :setVisible(false)

	--记录列表
	self._listView = cc.TableView:create(cc.size(1204, 454))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(67,60))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(self.tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

end

function CashRecordLayer:onUpdateShow()
	print("CashRecordLayer:onUpdateShow")

	if not self._cashRecordList then
		print("self._nullTipLabel:setVisible(true)")
		--self._nullTipLabel:setVisible(true)
        self._nullTipSprite:setVisible(true)
	else
		--self._nullTipLabel:setVisible(false)
        self._nullTipSprite:setVisible(false)
	end

	self._listView:reloadData()

end

---------------------------------------------------------------------

--子视图大小
function CashRecordLayer.cellSizeForTable(view, idx)
  	return 1204 , 76
end

--子视图数目
function CashRecordLayer.numberOfCellsInTableView(view)
	return #view:getParent()._cashRecordList
end
	
--获取子视图
function CashRecordLayer.tableCellAtIndex(view, idx)		
	local cell = view:dequeueCell()
	
	local item = view:getParent()._cashRecordList[idx+1]

	local width = 1204
	local height= 76

	if not cell then
		cell = cc.TableViewCell:new()
	else
		cell:removeAllChildren()
	end

	display.newSprite("BankRecord/table_bankrecord_cell_"..(idx%2)..".png")
		:move(width/2,height/2)
		:addTo(cell)

	--申请日期
	local date = os.date("%Y/%m/%d %H:%M:%S", tonumber(item.ApplyDate)/1000)
	-- print(date)
	-- print(""..tonumber(item.date))
	cc.Label:createWithTTF(date,"base/fonts/round_body.ttf",27)
		:move(140,height/2)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setTextColor(cc.c4b(216,187,115,255))
		:setAnchorPoint(cc.p(0.5,0.5))
		:addTo(cell)

    --申请单号
	cc.Label:createWithTTF(item.OrderID,"base/fonts/round_body.ttf",27)
		:move(455,height/2)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setTextColor(cc.c4b(216,187,115,255))
		:setAnchorPoint(cc.p(0.5,0.5))
		:addTo(cell)
    
    --金额
	cc.Label:createWithTTF(string.formatNumberFhousands(item.SellMoney),"base/fonts/round_body.ttf",27)
		:move(740,height/2)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setTextColor(cc.c4b(216,187,115,255))
		:setAnchorPoint(cc.p(0.5,0.5))
		:addTo(cell)
    
	 --状态
	local str_stutas = view:getParent():getStutas(item.Status)
	cc.Label:createWithTTF(string.format(tostring(str_stutas)),"base/fonts/round_body.ttf",27)
		:move(942,height/2)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setTextColor(cc.c4b(216,187,115,255))
		:setAnchorPoint(cc.p(0.5,0.5))
		:addTo(cell)--]]
		
    --备注
	cc.Label:createWithTTF(item.RejectReason,"base/fonts/round_body.ttf",27)
		:move(1058,height/2)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setTextColor(cc.c4b(216,187,115,255))
		:setAnchorPoint(cc.p(0.5,0.5))
		:addTo(cell)
    
   
	return cell
end
---------------------------------------------------------------------
return CashRecordLayer