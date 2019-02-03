local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local BankMediator = class("BankMediator", BaseMediator)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

function BankMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --订阅更新金币 更新银行的消息
    return	{
				GAME_COMMAMD.UPDATE_USER_SCORE,
				GAME_COMMAMD.UPDATE_INSURE_SCORE,	
				GAME_COMMAMD.UPDATE_BANK_INFO,
			}
end

function BankMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
	if (msgName == GAME_COMMAMD.UPDATE_USER_SCORE) then
		self.viewComponent:emit("M2V_Update_UserScore", GlobalUserItem.lUserScore)
	elseif (msgName == GAME_COMMAMD.UPDATE_INSURE_SCORE) then
		self.viewComponent:emit("M2V_Update_InsureScore", GlobalUserItem.lUserInsure)
	elseif (msgName == GAME_COMMAMD.UPDATE_BANK_INFO) then
		self.viewComponent:emit("M2V_Update_BankInfo", msgData)
	end
    --deal something
end

function BankMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	
	--查询银行
	self.viewComponent:attach("V2M_QueryBankInfo", handler(self, self.onQueryBankInfo))
	--存款
	self.viewComponent:attach("V2M_SaveBankScore", handler(self, self.onSaveBankScore))
	--取款
	self.viewComponent:attach("V2M_TakeBankScore", handler(self, self.onTakeBankScore))
	--存取记录
	self.viewComponent:attach("V2M_QueryBankRecord", handler(self, self.onQueryBankRecord))
	--体现记录
	self.viewComponent:attach("V2M_QueryBankExchangeRecord", handler(self, self.onQueryExchangeRecord))
	--检查用户是否绑定过银行
	self.viewComponent:attach("V2M_CheckBindBankInfo", handler(self, self.onCheckBindBankInfo))
	--绑定银行
	self.viewComponent:attach("V2M_BindBankInfo", handler(self, self.onBindBankInfo))
	--兑换金币
	self.viewComponent:attach("V2M_ExchangeBankScore", handler(self, self.onExchangeBankScore))
end

function BankMediator:onExchangeBankScore(e, score, exchangeType)

    print("onExchangeBankScore:exchangeType", exchangeType)

	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"兑换金币中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local ExchangeBank = ExternalFun.create_netdata(logincmd.CMD_GP_ExchangeBankScore)
	ExchangeBank:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_EXCHANGE_BANK_SCORE)
	ExchangeBank:pushdword(GlobalUserItem.dwUserID)
	ExchangeBank:pushdouble(score)
	ExchangeBank:pushstring(md5(GlobalUserItem.szInsurePass),yl.LEN_PASSWORD)
	local orderid = ExternalFun.guid()
	ExchangeBank:pushstring(orderid,yl.LEN_ORDER_ID)
    ExchangeBank:pushdword(exchangeType)
	
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ExchangeBank, function (Datatable, Responce)
		if (Responce.code == 0) then
			if Datatable.sub == logincmd.SUB_GP_OPERATE_SUCCESS then
				--self:onSubOperateSuccess(Datatable.data)
				
				local pData = Datatable.data
				local lResultCode = pData:readint()
				local szDescribe = pData:readstring()
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {szDescribe,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
				
				--操作成功后查询银行 更新金币
				self:onQueryBankInfo()
				
				--更新缓存数据
				GlobalUserItem.BankInfoData.lDrawScore = GlobalUserItem.BankInfoData.lDrawScore - score
				
				self.viewComponent:emit("M2V_Exchange_Result", 0, score)
			elseif Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE then
				--self:onSubOperateFailure(Datatable.data)
				
				local pData = Datatable.data
				local lResultCode = pData:readint()
				local szDescribe = pData:readstring()
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {szDescribe,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)				
				
				self.viewComponent:emit("M2V_Exchange_Result", -1)
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "网络连接失败！")
		end
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)
end

--function BankMediator:onBindBankInfo(e, realName, bankNo, bankName, bankAddr, alipayID)
function BankMediator:onBindBankInfo(e, BankInfo)

    print("onBindBankInfo:realname", BankInfo.realname)
    print("onBindBankInfo:bankno", BankInfo.bankno)
    print("onBindBankInfo:bankname", BankInfo.bankname)
    print("onBindBankInfo:bankaddr", BankInfo.bankaddr)
    print("onBindBankInfo:alipayID", BankInfo.alipayID)


	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"绑定银行信息中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local BindBank = ExternalFun.create_netdata(logincmd.CMD_GP_BindBankInfo)
	BindBank:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_BIND_BANK_INFO)
	BindBank:pushdword(GlobalUserItem.dwUserID)
	BindBank:pushstring(BankInfo.realname,yl.LEN_REAL_NAME)
	BindBank:pushstring(BankInfo.bankno,yl.LEN_BANK_NO)
	BindBank:pushstring(BankInfo.bankname,yl.LEN_BANK_NAME)
	BindBank:pushstring(BankInfo.bankaddr,yl.LEN_BANK_ADDRESS)
    BindBank:pushstring(BankInfo.alipayID,yl.LEN_ALIPAY_ID)



	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, BindBank, function (Datatable, Responce)
		if (Responce.code == 0) then
			if Datatable.sub == logincmd.SUB_GP_OPERATE_SUCCESS then
				self:onSubOperateSuccess(Datatable.data)

				--更新数据
				GlobalUserItem.BankInfoData.szRealName = BankInfo.realname
				GlobalUserItem.BankInfoData.szBankNo = BankInfo.bankno
                GlobalUserItem.BankInfoData.szAlipayID = BankInfo.alipayID

                printf("BankInfo.realname:", BankInfo.realname)
                printf("BankInfo.bankNo:", BankInfo.bankno)
                printf("BankInfo.alipayID:", BankInfo.alipayID)

				self.viewComponent:emit("M2V_Bind_Result", 0, GlobalUserItem.BankInfoData)
			elseif Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE then
				self:onSubOperateFailure(Datatable.data)
				self.viewComponent:emit("M2V_Bind_Result", -1)
			end
		else
			--self.viewComponent:emit("M2V_ShowToast", "网络连接失败！")
            self.viewComponent:emit("M2V_ShowToast", "网络连接失败！"..Responce.code)
		end

		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)
end

function BankMediator:onSubOperateSuccess(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	self.viewComponent:emit("M2V_ShowToast", szDescribe)
end

function BankMediator:onSubOperateFailure(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	self.viewComponent:emit("M2V_ShowToast", szDescribe)
end

function BankMediator:onCheckBindBankInfo(e)
	--先读取缓存数据 没有就请求网络数据
	if (GlobalUserItem.BankInfoData ~= nil) then
        printf("onCheckBindBankInfo11111")
		self.viewComponent:emit("M2V_CheckBindBankResult", GlobalUserItem.BankInfoData)
		return
	end
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"查询银行信息中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local QueryBank = ExternalFun.create_netdata(logincmd.CMD_GP_QueryBankInfo)
	QueryBank:setcmdinfo(logincmd.MDM_GP_USER_SERVICE,logincmd.SUB_GP_QUERY_BANK_INFO)
	QueryBank:pushdword(GlobalUserItem.dwUserID)
	
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, QueryBank, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == logincmd.SUB_GP_QUERY_BANK_INFO_RESULT) then
				local sizeData = Datatable.data:getlen()
				--assert(Datatable.data:getlen() == ExternalFun.sizeof(logincmd.CMD_GP_QueryBankInfoResult))
				local ServerData = ExternalFun.read_netdata(logincmd.CMD_GP_QueryBankInfoResult, Datatable.data)
				
				--更新缓存数据
				GlobalUserItem.BankInfoData = ServerData
				
				--ServerData.szBankNo = ""

                printf("GlobalUserItem.BankInfoData:", GlobalUserItem.BankInfoData.szBankNo,GlobalUserItem.BankInfoData.szAlipayID)

                printf("onCheckBindBankInfo2222222")
				self.viewComponent:emit("M2V_CheckBindBankResult", ServerData)
			else
				self.viewComponent:emit("M2V_ShowToast", "网络连接失败！")
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "网络连接失败！")
		end
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)
end

function BankMediator:onQueryBankInfo(e)
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"查询银行信息中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local FlushData = CCmd_Data:create(70)
	FlushData:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_QUERY_INSURE_INFO)
	FlushData:pushdword(GlobalUserItem.dwUserID)
	local password = md5(GlobalUserItem.szPassword)
	FlushData:pushstring(password,33)
	
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, FlushData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_USER_INSURE_INFO) then
				local cmdtable = ExternalFun.read_netdata(logincmd.CMD_GP_UserInsureInfo, Datatable.data)
				GlobalUserItem.lUserScore = cmdtable.lUserScore
				GlobalUserItem.lUserInsure = cmdtable.lUserInsure
				
				--通知更新UI银行金币
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_INSURE_SCORE, GlobalUserItem.lUserInsure)
				--通知更新UI用户金币
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_SCORE, GlobalUserItem.lUserScore)
--[[				local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
				eventListener.obj = yl.RY_MSG_USERWEALTH
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)--]]
			else
				self.viewComponent:emit("M2V_ShowToast", "查询银行失败")
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "查询银行失败")
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)	
end

function BankMediator:onSaveBankScore(e, score)
	if (score == 0) then
		self.viewComponent:emit("M2V_ShowToast", "存款不能为0")
		return
	end
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local SaveData = CCmd_Data:create(78)
	SaveData:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_USER_SAVE_SCORE)
	SaveData:pushdword(GlobalUserItem.dwUserID)
	SaveData:pushdouble(score)
	SaveData:pushstring(GlobalUserItem.szMachine,33)
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, SaveData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if Datatable.sub == logincmd.SUB_GP_USER_INSURE_SUCCESS then
				self:onSubInsureSuccess(Datatable.data)
			elseif Datatable.sub == logincmd.SUB_GP_USER_INSURE_FAILURE then
				self:onSubInsureFailue(Datatable.data)
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "存银行失败")
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)		
end

function BankMediator:onTakeBankScore(e, insurepass, score)
	if (score == 0) then
		self.viewComponent:emit("M2V_ShowToast", "取款不能为0")
		return
	end
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	local TakeData = CCmd_Data:create(210-66)
	TakeData:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_USER_TAKE_SCORE)
	TakeData:pushdword(GlobalUserItem.dwUserID)
	TakeData:pushdouble(score)
	TakeData:pushstring(md5(insurepass),33)
	TakeData:pushstring(GlobalUserItem.szMachine,33)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, TakeData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if Datatable.sub == logincmd.SUB_GP_USER_INSURE_SUCCESS then
				self:onSubInsureSuccess(Datatable.data)
			elseif Datatable.sub == logincmd.SUB_GP_USER_INSURE_FAILURE then
				self:onSubInsureFailue(Datatable.data)
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "取银行失败")
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	end)		
end

function BankMediator:onQueryBankRecord(e)
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=getbankrecord&userid="..GlobalUserItem.dwUserID.."&signature="..GlobalUserItem:getSignature(os.time()).."&time="..os.time().."&number=20&page=1",function(jstable,jsdata)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
			if jstable then
				local code = jstable["code"]
				local recordArray = {}
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
						            item.tradeType = list[i]["TradeTypeDescription"]
						            item.swapScore = tonumber(list[i]["SwapScore"])
						            item.revenue = tonumber(list[i]["Revenue"])
						            item.date = GlobalUserItem:getDateNumber(list[i]["CollectDate"])
						            item.id = list[i]["TransferAccounts"]
						            table.insert(recordArray,item)
								end
							end
						end
					end
				end
				--更新UI
				self.viewComponent:emit("M2V_UpdateInsureRecord", recordArray)
			else
				self.viewComponent:emit("M2V_ShowToast", "抱歉，获取银行记录信息失败！")
			end
		end)
end

function BankMediator:onQueryExchangeRecord(e)
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	


	local function getStutas( param )
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
	
	
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=gettixianrecord&userid="..GlobalUserItem.dwUserID.."&signature="..GlobalUserItem:getSignature(os.time()).."&time="..os.time().."&number=20&page=1",function(jstable,jsdata)
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
		if jstable then
			local code = jstable["code"]
			local recordArray = {}
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
								item.Status = getStutas(list[i]["Status"])
								item.RejectReason = list[i]["RejectReason"]
								
								table.insert(recordArray,item)
							end
						end
					end
				end
			end

			self.viewComponent:emit("M2V_UpdateInsureExchangeRecord", recordArray)
		else
			self.viewComponent:emit("M2V_ShowToast", "抱歉，获取提现记录信息失败！")
		end
	end)
end

function BankMediator:onSubInsureSuccess(pData)
	local dwUserID = pData:readdword()
	if dwUserID == GlobalUserItem.dwUserID then
        local serverUserScore = GlobalUserItem:readScore(pData)
		GlobalUserItem.lUserInsure = GlobalUserItem:readScore(pData)
 
    
        GlobalUserItem.BankInfoData.lDrawScore = GlobalUserItem.BankInfoData.lDrawScore+ (GlobalUserItem.lUserScore-serverUserScore  )
	 
        GlobalUserItem.lUserScore= serverUserScore
		

		self.viewComponent:emit("M2V_ShowToast", "操作成功！")
		self.viewComponent:emit("M2V_InsureSuccess")
		self.viewComponent:emit("M2V_Exchange_Result", 0,GlobalUserItem.BankInfoData.lDrawScore)
		--通知更新UI银行金币
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_INSURE_SCORE, GlobalUserItem.lUserInsure)
		--通知更新UI用户金币
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_SCORE, GlobalUserItem.lUserScore)
		
--[[		--通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)--]]
	end
end



function BankMediator:onSubInsureFailue(pData)
	local lError = pData:readint()
	local szError = pData:readstring()
	self.viewComponent:emit("M2V_ShowToast", szError)
	self.viewComponent:emit("M2V_InsureFailed")
end

--绑定手机
function BankMediator:onBindPhone(e)

end

return BankMediator