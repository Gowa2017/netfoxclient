local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ShopMediator = class("ShopMediator", BaseMediator)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()

function ShopMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --订阅更新金币 更新银行的消息
    return	{
				GAME_COMMAMD.UPDATE_USER_SCORE,
				GAME_COMMAMD.UPDATE_INSURE_SCORE,	
			}
end

function ShopMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
	if (msgName == GAME_COMMAMD.UPDATE_USER_SCORE) then
		self.viewComponent:emit("M2V_Update_UserScore", GlobalUserItem.lUserScore)
	elseif (msgName == GAME_COMMAMD.UPDATE_INSURE_SCORE) then
		self.viewComponent:emit("M2V_Update_InsureScore", GlobalUserItem.lUserInsure)
	end
    --deal something
end

function ShopMediator:onRegister()
    self.super.onRegister(self)
	--支付成功
	self.viewComponent:attach("V2M_PaySuccess", handler(self, self.onPaySuccess))
	--查询商城信息
	self.viewComponent:attach("V2M_QueryShopInfo", handler(self, self.onQueryShopInfo))
	--查询代理信息
	self.viewComponent:attach("V2M_QueryAgentInfo", handler(self, self.onQueryAgentInfo))
    --查询下级信息
   self.viewComponent:attach("V2M_QueryAgentInfo_User", handler(self, self.onQueryAgentInfo_User))
end

function ShopMediator:onPaySuccess(e)
	local ostime = os.time()
    local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"

	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"获取信息中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
    appdf.onHttpJsionTable(url ,"GET","action=GetScoreInfo&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(sjstable,sjsdata)
        AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
        if type(sjstable) == "table" then
            local data = sjstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]
                if true == valid then
                    local score = tonumber(data["Score"]) or 0
                    local bean = tonumber(data["Currency"]) or 0
                    local ingot = tonumber(data["UserMedal"]) or 0
                    local roomcard = tonumber(data["RoomCard"]) or 0
                    local insurescore = tonumber(data["InsureScore"]) or 0 --MXM增加银行金币

                    if score ~= GlobalUserItem.lUserScore 
                    	or bean ~= GlobalUserItem.dUserBeans
                    	or ingot ~= GlobalUserItem.lUserIngot
                    	or roomcard ~= GlobalUserItem.lRoomCard 
                        or insurescore ~= GlobalUserItem.lUserInsure then
                    	GlobalUserItem.dUserBeans = bean
                    	GlobalUserItem.lUserScore = score
                    	GlobalUserItem.lUserIngot = ingot
                    	GlobalUserItem.lRoomCard = roomcard
                        GlobalUserItem.lUserInsure = insurescore
                    end
					--通知更新UI银行金币
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_INSURE_SCORE, GlobalUserItem.lUserInsure)
					--通知更新UI用户金币
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_SCORE, GlobalUserItem.lUserScore)
                end
            end
        end
    end)
end

function ShopMediator:onQueryShopInfo(e, callback)
    local url = BaseConfig.WEB_HTTP_URL .. "/ws/MobileInterface.ashx"
	local appConfigProxy = self.facade:retrieveProxy("AppConfigProxy")
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
    appdf.onHttpJsionTable(url ,"GET","action=getpaylist",function(sjstable,sjsdata)
        AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
        if type(sjstable) == "table" then
            local leftmenudata = sjstable["Table"]
			local itemdata = sjstable["Table1"]
			
     
			--过滤渠道列表
			local filterleftmenu = {}

            --force insert VIP
             
            leftmenudata[1].ID = yl.PAYMENT_VIP
            leftmenudata[1].QudaoName = "VIP"
 
			for _, v in pairs(leftmenudata) do
                
                if (yl.PAYMENT_VIP == v.ID) then
				    table.insert(filterleftmenu, v)
			    end

				--[[if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_MAC == targetPlatform) then
					if (appConfigProxy._appStoreSwitch ~= 0) then
						if (yl.PAYMENT_APPLE ~= v.ID) then
							table.insert(filterleftmenu, v)
						end
					else
						if (yl.PAYMENT_APPSTORE_APPLE ~= v.ID) then
							table.insert(filterleftmenu, v)
						end
					end
				elseif (cc.PLATFORM_OS_ANDROID == targetPlatform) then
					if (yl.PAYMENT_APPLE ~= v.ID and yl.PAYMENT_APPSTORE_APPLE ~= v.ID) then
						table.insert(filterleftmenu, v)
					end
				else
					table.insert(filterleftmenu, v)
				end	]]--			
			end
			--过滤渠道列表数据
			local pricelist = {}
			--[[for _, v in pairs(itemdata) do
				if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_MAC == targetPlatform) then
					if (appConfigProxy._appStoreSwitch ~= 0) then
						if (yl.PAYMENT_APPLE ~= v.QudaoID) then
							pricelist[v.QudaoID] = pricelist[v.QudaoID] or {}
							table.insert(pricelist[v.QudaoID], v.Limit)
						end
					else
						if (yl.PAYMENT_APPSTORE_APPLE ~= v.QudaoID) then
							pricelist[v.QudaoID] = pricelist[v.QudaoID] or {}
							table.insert(pricelist[v.QudaoID], v.Limit)
						end
					end
				elseif (cc.PLATFORM_OS_ANDROID == targetPlatform) then
					if (yl.PAYMENT_APPLE ~= v.QudaoID and yl.PAYMENT_APPSTORE_APPLE ~= v.QudaoID) then
						pricelist[v.QudaoID] = pricelist[v.QudaoID] or {}
						table.insert(pricelist[v.QudaoID], v.Limit)
					end
				else
					pricelist[v.QudaoID] = pricelist[v.QudaoID] or {}
					table.insert(pricelist[v.QudaoID], v.Limit)
				end
			end]]--
			
			callback(filterleftmenu, pricelist)
        end
    end)
end

function ShopMediator:onQueryAgentInfo()
    local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
    AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
    appdf.onHttpJsionTable(url ,"GET","action=getagentpay",function(jstable,jsdata)
		--dump(jstable, "jstable", 7)
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
		local errmsg = ""
		if type(jstable) == "table" then
			local code = jstable["code"]
			if tonumber(code) == 0 then
				local datax = jstable["data"]
				local msg = jstable["msg"]
				errmsg = ""
				if type(msg) == "string" then
					errmsg = msg
				end
				
				if datax then
					local valid = datax["valid"]
					if valid == true then
						local list = datax["list"]
						if type(list) == "table" then
							local agentRechargeList = {}
							for i=1,#list do
								local item = {}
								item.qq = tonumber(list[i]["QQ"])
								item.name = list[i]["ShowName"]
								item.wechat = list[i]["WeChat"]
								table.insert(agentRechargeList, item)
							end
							self.viewComponent:emit("M2V_QueryAgentResult", agentRechargeList)
						end
					end
				end
			end
		end
		
		if type(errmsg) == "string" and "" ~= errmsg then
            --showToast(self,errmsg,2,cc.c3b(250,0,0))
        end 
    end)
end

return ShopMediator