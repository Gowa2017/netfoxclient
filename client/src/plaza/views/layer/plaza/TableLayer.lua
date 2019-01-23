--
-- Author: zhong
-- Date: 2016-08-09 09:40:00
--

--游戏转盘界面
local TableLayer = class("TableLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local rotateParam = 
{
	{key = 100	, 	angle = 360	},
	{key = 200	, 	angle = 330	},
	{key = 300	, 	angle = 300	},
	{key = 400	, 	angle = 270	},
	{key = 500	, 	angle = 240	},
	{key = 600	, 	angle = 210	},
	{key = 700	, 	angle = 180	},
	{key = 800	, 	angle = 150	},
	{key = 900	, 	angle = 120	},
	{key = 1000	, 	angle = 90	},
	{key = 1100	, 	angle = 60	},
	{key = 1200	, 	angle = 30	},
}

function TableLayer:ctor(scene)
	self._scene = scene

	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("table/TableLayer.csb", self)

	--转盘
	self.m_spTable = csbNode:getChildByName("sp_table")
	--转盘金币
	local node_point = self.m_spTable:getChildByName("node_point")
	self.m_tabAtlasPos = {}
	for i = 1, 12 do
		self.m_tabAtlasPos[i] = node_point:getChildByName(string.format("altas_pos_%d", i))
	end

	--开始按钮
	self.m_btnStart = csbNode:getChildByName("start_btn")
	self.m_btnStart:addTouchEventListener(function ( sender, tType )
		if tType == ccui.TouchEventType.ended then
            if self.m_nFreeCount <= 0 then
                if 1 == self.m_bConfigPay then
                    self._queryDialog = QueryDialog:create("您的免费次数已用完，是否付费 " .. self.m_lChargeFee .. " 进行抽奖？", function(ok)
                        if ok == true then
                            self:onRequestTable()
                        end
                        self._queryDialog = nil
                    end):setCanTouchOutside(false)
                    :addTo(self)
                else
                    showToast(self, "您的免费次数已用完！", 3)
                end               
            else
                self:onRequestTable()                
            end         	        	
        end
	end)

	--转盘机会
	self.m_atlasCount = csbNode:getChildByName("atlas_count")
	self.m_atlasCount:setString("")
	self.m_nFreeCount = 0

	--游戏结果
	self.m_atlasRes = csbNode:getChildByName("atlas_res")
	self.m_atlasRes:setString("")

	--金币背景
	self.m_spRes = csbNode:getChildByName("sp_res")
	self.m_spRes:setVisible(false)

	--是否转盘结束
	self.m_bRotateOver = true

	--转盘结果
	self.m_LotteryRes = {}

    -- 配置允许付费
    self.m_bConfigPay = 0
    -- 付费金额
    self.m_lChargeFee = 0
end

function TableLayer:onKeyBack( )
	if false == self.m_bRotateOver then
		showToast(self, "暂时不可返回", 2)
		return true
	end
	return false
end

function TableLayer:onEnterTransitionFinish( )
    self:showPopWait()
	--获取抽奖奖品配置
	local url = yl.HTTP_URL .. "/WS/Lottery.ashx"
 	appdf.onHttpJsionTable(url ,"GET","action=LotteryConfig",function(jstable,jsdata)
        dump(jstable, "jstable", 6)
        if type(jstable) == "table" then
            local data = jstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]
                if nil ~= valid and true == valid then
                    local list = data["list"]
                    if type(list) == "table" then
                        for i = 1, #list do
                            --配置转盘
                            local lottery = list[i]
                            rotateParam[lottery.ItemIndex].key = lottery.ItemQuota
                            self.m_tabAtlasPos[i]:setString(lottery.ItemQuota .. "")
                        end
                    end
                end
            end
            local msg = jstable["msg"]
            --获取用户信息
            local ostime = os.time()
            appdf.onHttpJsionTable(url ,"GET","action=LotteryUserInfo&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(sjstable,sjsdata)
                self:dismissPopWait()
                dump(sjstable, "sjstable", 6)
                if type(sjstable) == "table" then
                    local sdata = sjstable["data"]
                    if type(sdata) == "table" then
                        local valid = sdata["valid"]
                        msg = sdata["msg"]
                        if nil ~= valid and true == valid then                  
                            local slist = sdata["list"]
                            if type(slist) == "table" then
                                msg = nil
                                --免费转盘机会
                                local freecount = slist["FreeCount"] or 0
                                --剩余免费次数
                                local aready = slist["AlreadyCount"] or 0
                                -- 抽奖付费
                                self.m_lChargeFee = slist["ChargeFee"] or 0
                                -- 允许收费
                                self.m_bConfigPay = slist["IsCharge"] or 0

                                local leftcount = freecount - aready
                                freecount = (leftcount >= 0) and leftcount or 0
                                self.m_nFreeCount = freecount
                                self.m_atlasCount:setString(freecount .. "")
                            end                     
                        end 
                    end
                end           
            end)
        end 		
 	end)
end

function TableLayer:onTouchBegan(touch, event)
	return (self.m_bRotateOver and self:isVisible())
end

function TableLayer:onTouchEnded(touch, event)
	local pos = touch:getLocation()	
	local m_spBg = self.m_spTable
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self._scene:onKeyBack()
    end
end
    
-- 请求转盘
function TableLayer:onRequestTable()
    self.m_spRes:setVisible(false)
    self.m_atlasRes:setString("")
    self.m_bRotateOver = false
    self.m_btnStart:setEnabled(false)

    self:showPopWait()
    local ostime = os.time()
    local url = yl.HTTP_URL .. "/WS/Lottery.ashx"           
    appdf.onHttpJsionTable(url ,"GET","action=LotteryStart&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(jstable,jsdata)
        dump(jstable, "jstable", 6)
        local msg = "转盘数据获取失败"
        if type(jstable) == "table" then
            msg = jstable["msg"]
            local data = jstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]
                if nil ~= valid and true == valid then
                    local list = data["list"]
                    if type(list) == "table" then
                        self.m_LotteryRes = list
                        local idx = list["ItemIndex"]
                        msg = nil
                        if nil ~= idx then
                            self:rotateTo(tonumber(idx))

                            self.m_nFreeCount = self.m_nFreeCount - 1
                            self.m_nFreeCount = (self.m_nFreeCount >= 0) and self.m_nFreeCount or 0
                            self.m_atlasCount:setString(self.m_nFreeCount .. "")
                        else
                            msg = "抽奖异常"
                        end                        
                    end
                end
            end
        end
        
        self:dismissPopWait()
        if nil ~= msg then
            showToast(self, msg, 3)
            self.m_btnStart:setEnabled(true)
            self.m_bRotateOver = true
        end
    end) 
end

function TableLayer:rotateTable( toKey )
	local idx = 1
	for k,v in pairs(rotateParam) do
		if v.key == toKey then
			idx = k
			break
		end
	end
	
	self:rotateTo(idx)
end

function TableLayer:rotateTo( idx )
    if nil == rotateParam[idx] then
        self.m_bRotateOver = true
        self.m_btnStart:setEnabled(true)
        return
    end
	print("rotate to " .. rotateParam[idx].angle)

	local angle = rotateParam[idx].angle + 360 * 5
	local act = cc.RotateTo:create(10, angle)
	local easeRotate = cc.EaseCircleActionOut:create(act)
	local call = cc.CallFunc:create(function( )
		print("rotate over")
        GlobalUserItem.setTodayTable()
		
		self.m_btnStart:setEnabled(true)
		local str = string.format("%d", rotateParam[idx].key)
        self.m_atlasRes:setString(str)

        local toastMsg = ""
        local frame = nil
        --判断类型
        if 0 == self.m_LotteryRes.ItemType then 		--金币
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_res.png")
            toastMsg = "恭喜您获得" .. str .. "金币!"
        elseif 1 == self.m_LotteryRes.ItemType then 	--游戏豆
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_res_bean.png")
            toastMsg = "恭喜您获得" .. str .. "游戏豆!"
        elseif 2 == self.m_LotteryRes.ItemType then     --元宝
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_res_ingot.png")
            toastMsg = "恭喜您获得" .. str .. "元宝!"
        end
        if nil ~= frame then
            self.m_spRes:setSpriteFrame(frame)
            self.m_spRes:setVisible(true)
        end
        self._scene:coinDropDownAni(function()
            self.m_bRotateOver = true  
            showToast(self, toastMsg, 2) 
        end)
        
        GlobalUserItem.lUserScore = self.m_LotteryRes.Score or GlobalUserItem.lUserScore
        GlobalUserItem.dUserBeans = self.m_LotteryRes.Currency or GlobalUserItem.dUserBeans
        GlobalUserItem.lUserIngot = self.m_LotteryRes.UserMedal or GlobalUserItem.lUserIngot

        --通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end)
	local seq = cc.Sequence:create(easeRotate, call)
	self.m_spTable:stopAllActions()
	self.m_spTable:runAction(seq)
end

function TableLayer:showPopWait()
	self._scene:showPopWait()
end

function TableLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

return TableLayer