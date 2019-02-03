--[[
	每日必做界面
	2016_06_23 Ravioyla
]]

local EveryDayLayer = class("EveryDayLayer", function(scene)
		local everyDayLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return everyDayLayer
end)
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local CheckinFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.CheckinFrame")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

EveryDayLayer.BT_CELL			= 15
EveryDayLayer.BT_CHECKIN		= 16
EveryDayLayer.BT_ROLLTABLE		= 17
EveryDayLayer.BT_DIBAO			= 18
EveryDayLayer.BT_CHARGE			= 19
EveryDayLayer.BT_SHARE			= 20
EveryDayLayer.BT_TASK			= 21

-- 进入场景而且过渡动画结束时候触发。
function EveryDayLayer:onEnterTransitionFinish()
    self._scene:showPopWait()
    self._checkinFrame:onBaseEnsureLoad()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function EveryDayLayer:onExitTransitionStart()
    return self
end

function EveryDayLayer:ctor(scene)
	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
        elseif eventType == "exit" then
            if self._checkinFrame:isSocketServer() then
                self._checkinFrame:onCloseSocket()
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
    local checkinCallBack = function(result,message)
		this:onCheckinCallBack(result,message)
	end

	--网络处理
	self._checkinFrame = CheckinFrame:create(self,checkinCallBack)

	self._EveryDayList = {}

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,yl.HEIGHT - 51)
		self:addChild(sp)
	end
	display.newSprite("EveryDay/title_everyday.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
        self:addChild(sp)
    end
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_2.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
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

	--记录列表
	self._listView = cc.TableView:create(cc.size(1163, 508))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(88,68))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(self.tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
end

function EveryDayLayer:onUpdateShow()

	self._listView:reloadData()

end

--按键监听
function EveryDayLayer:onButtonClickedEvent(tag,sender)

	if tag == EveryDayLayer.BT_CELL then
		local idx = sender.nCusTag
		idx = idx+EveryDayLayer.BT_CELL+1
		if idx == EveryDayLayer.BT_CHECKIN then
			self:getParent():getParent():onChangeShowMode(yl.SCENE_CHECKIN)
		elseif idx == EveryDayLayer.BT_ROLLTABLE then
			self:getParent():getParent():onChangeShowMode(yl.SCENE_TABLE)
		elseif idx == EveryDayLayer.BT_DIBAO then
			self._scene:showPopWait()
			self._checkinFrame:onBaseEnsureTake()
		elseif idx == EveryDayLayer.BT_CHARGE then
			--print("点击首充")
			self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_BEAN)
		elseif idx == EveryDayLayer.BT_SHARE then
			--print("点击分享")
			local function sharecall( isok )
				if type(isok) == "string" and isok == "true" then
					showToast(self, "分享成功", 2)
					--获取奖励
					local ostime = os.time()
					local url = yl.HTTP_URL .. "/WS/MobileInterface.ashx"
					local param = "action=GetMobileShare&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime) .. "&machineid=" .. GlobalUserItem.szMachine
					appdf.onHttpJsionTable(url ,"GET", param, function(jstable,jsdata)
						dump(jstable, "desciption", 6)
                        --LogAsset:getInstance():logData(jsonStr,true)
                        GlobalUserItem.setTodayShare()
                        
                        if type(jstable) == "table" then
                            local data = jstable["data"]
                            local msg = jstable["msg"]
                            if type(data) == "table" and type(msg) == "string" and "" ~= msg then
                                GlobalUserItem.lUserScore = data["Score"] or GlobalUserItem.lUserScore
                                --通知更新        
                                local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                                eventListener.obj = yl.RY_MSG_USERWEALTH
                                cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                                if type(msg) == "string" and "" ~= msg then
                                    showToast(self, msg, 3)
                                end                         
                            end
                        end
						
					end)
				end
			end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            MultiPlatform:getInstance():customShare(sharecall, nil, nil, url, "")
		elseif idx == EveryDayLayer.BT_TASK then
			self:getParent():getParent():onChangeShowMode(yl.SCENE_TASK)
		end
	end
end

--操作结果
function EveryDayLayer:onCheckinCallBack(result,message)
	print("======== EveryDayLayer:onCheckinCallBack ========")

	self._scene:dismissPopWait()
	if  type(message) == "string" and message ~= "" then
		showToast(self,message,2);
	end

    if result == self._checkinFrame.BASEENSUREQUERY then
        local condition = message.condition or 0
        local amount = message.amount or 0
        local times = message.times or 0
        local tmpField = {}
        tmpField["Field1"] = 
        {
            idx = 1,
            str = "连续签到天数越多，奖励越多！",
            order = GlobalUserItem.isFirstCheckIn() and 6 or 0,
        }
        tmpField["Field2"] = 
        {
            idx = 2,
            str = "每天送您免费" .. GlobalUserItem.nTableFreeCount .. "次人品大爆炸的机会！",
            order = GlobalUserItem.isFirstTable() and 5 or -1,
        }
        tmpField["Field3"] = 
        {
            idx = 3,
            str = "游戏币数量低于" .. condition .. ",每次可以领取" .. amount .. "(限" .. times .. "次/天)。",
            order = 4,
        }
        tmpField["Field4"] = 
        {
            idx = 4,
            str = "每日一充，就那么任性！",
            order = GlobalUserItem.isFirstPay() and 3 or -2,
        }
        tmpField["Field5"] = 
        {
            idx = 5,
            str = "觉得我们做得好，请分享给你的朋友即可获得" .. GlobalUserItem.nShareSend .. "游戏币！",
            order = GlobalUserItem.isFirstShare() and 2 or -3,
        }
        tmpField["Field6"] = 
        {
            idx = 6,
            str = "今天您的任务还没完成哦~",
            order = 1,
        }
        for k,v in pairs(GlobalUserItem.tabDayTaskCache) do
            local field = tmpField[k]
            if nil ~= tmpField[k] then
                table.insert(self._EveryDayList, field)
            end
        end
        table.sort(self._EveryDayList, function(a,b)
            return a.order > b.order
        end)

        self:onUpdateShow()
    end
end

---------------------------------------------------------------------

--子视图大小
function EveryDayLayer.cellSizeForTable(view, idx)
  	return 1161 , 508/4
end

--子视图数目
function EveryDayLayer.numberOfCellsInTableView(view)
	return #view:getParent()._EveryDayList
end
	
--获取子视图
function EveryDayLayer.tableCellAtIndex(view, idx)	
	
	local cell = view:dequeueCell()
	
	local item = view:getParent()._EveryDayList[idx+1]
    local src_idx = item.idx - 1

	local width = 1161
	local height= 508/4

	if not cell then

		cell = cc.TableViewCell:new()
		display.newSprite("EveryDay/cell_everyday.png")
		:move(width/2,height/2)
		:addTo(cell)

		--图标
		display.newSprite("EveryDay/icon_everyday_"..src_idx..".png")
			:setTag(1)
    		:move(110,height/2)
    		:addTo(cell)

    	--标题
    	display.newSprite("EveryDay/text_everyday_"..src_idx..".png")
    		:setTag(2)
    		:move(182,88)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(cell)

    	--提示
		local clip = ClipText:createClipText(cc.size(720, 30), item.str, "fonts/round_body.ttf", 28)
		clip:setTag(3)
		clip:setPosition(182,41)
			--:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		clip:setTextColor(cc.c4b(255,255,255,255))
		clip:setAnchorPoint(cc.p(0,0.5))
        cell:addChild(clip)

		--按钮
    	ccui.Button:create("EveryDay/bt_everyday_"..src_idx.."_0.png","EveryDay/bt_everyday_"..src_idx.."_1.png")
    		:move(1000,height/2)
    		:setTag(EveryDayLayer.BT_CELL)
    		:addTo(cell)
    		:addTouchEventListener(view:getParent()._btcallback)

	end

	cell:getChildByTag(1):setTexture("EveryDay/icon_everyday_"..src_idx..".png")
	cell:getChildByTag(2):setTexture("EveryDay/text_everyday_"..src_idx..".png")
	cell:getChildByTag(3):setString(item.str)
    cell:getChildByTag(EveryDayLayer.BT_CELL).nCusTag = src_idx
	cell:getChildByTag(EveryDayLayer.BT_CELL):loadTextureNormal("EveryDay/bt_everyday_"..src_idx.."_0.png")
	cell:getChildByTag(EveryDayLayer.BT_CELL):loadTexturePressed("EveryDay/bt_everyday_"..src_idx.."_1.png")

	return cell
end
---------------------------------------------------------------------
return EveryDayLayer