local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local BankOpenMediator = class("BankOpenMediator", BaseMediator)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

function BankOpenMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
			}
end

function BankOpenMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
    --deal something
end

function BankOpenMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	self.viewComponent:attach("V2M_OpenBankStart", handler(self, self.onOpenBankStart))

	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

--发送开通银行
function BankOpenMediator:onOpenBankStart(e, bankpass, callback)
	local EnableBank = CCmd_Data:create(202)
	local password = md5(GlobalUserItem.szPassword)
	local newpassword = md5(bankpass)
	local machine  = GlobalUserItem.szMachine
	EnableBank:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_USER_ENABLE_INSURE)
	EnableBank:pushdword(GlobalUserItem.dwUserID)
	EnableBank:pushstring(password,33)
	EnableBank:pushstring(newpassword,33)
	EnableBank:pushstring(machine,33)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, EnableBank, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == logincmd.SUB_GP_USER_INSURE_ENABLE_RESULT) then
				GlobalUserItem.cbInsureEnabled = Datatable.data:readbyte()
				local szTipString = Datatable.data:readstring()
				if GlobalUserItem.cbInsureEnabled ~= 0 then
					callback(0,szTipString)
					self:queryBankInfo()
				else
					callback(-1,szTipString)
				end	
			else
				callback(-1, "银行开通失败！")
			end
		else
			callback(-1, "发送开通失败！")
		end
	end)
end

function BankOpenMediator:queryBankInfo()
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
				--通知更新        
				local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
				eventListener.obj = yl.RY_MSG_USERWEALTH
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
			else
				self.viewComponent:emit("M2V_ShowToast", "查询银行失败")
				--callback(-1, "银行开通失败！")
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "查询银行失败")
			--callback(-1, "发送开通失败！")
		end
	end)	
end

--绑定手机
function BankOpenMediator:onBindPhone(e)

end

return BankOpenMediator