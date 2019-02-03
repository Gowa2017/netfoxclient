local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local BankModifyMediator = class("BankModifyMediator", BaseMediator)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")


function BankModifyMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
			}
end

function BankModifyMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
end

function BankModifyMediator:onRegister()
    self.super.onRegister(self)
	
	--提交修改信息
	self.viewComponent:attach("V2M_ModifyBankInfo", handler(self, self.onModifyBankInfo))
	--获取验证码
	self.viewComponent:attach("V2M_ModifyBankValidCode", handler(self, self.onModifyBankValidCode))

	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

--发送开通银行
function BankModifyMediator:onModifyBankInfo(e, parm)
	if (type(parm) ~= "table") then
		self.viewComponent:emit("M2V_ShowToast", "数据异常，请稍后再试！")
		return
	end
	
	local ServerData = ExternalFun.create_netdata(logincmd.CMD_MB_ModifyBankInfoVerifyCode)

	ServerData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_MODIFY_BANKINFO_VERIFYCODE)
	ServerData:pushdword(GlobalUserItem.dwUserID)
	ServerData:pushstring(parm.szBankNo,yl.LEN_BANK_NO)
	ServerData:pushstring(parm.szBankName,yl.LEN_BANK_NAME)
	ServerData:pushstring(parm.szValidCode,yl.LEN_VERIFY_CODE)
	ServerData:pushstring(GlobalUserItem.szRegisterMobile,yl.LEN_MOBILE_PHONE)
	ServerData:pushstring(parm.szBankAddress,yl.LEN_BANK_ADDRESS)
	ServerData:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
	

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ServerData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
				--清空原来的列表
				local code = Datatable.data:readbyte()
				local szDescribe = Datatable.data:readstring(64)
				self.viewComponent:emit("M2V_ShowToast", szDescribe)
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_SUCCESS) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				self.viewComponent:emit("M2V_ShowToast", szDescribe)
				
				--更新内存数据
				GlobalUserItem.BankInfoData.szBankNo = parm.szBankNo
				
				--更新视图
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_BANK_INFO, parm)
				
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.BANK_MODIFY_LAYER})
				
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_FAILURE) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				self.viewComponent:emit("M2V_ShowToast", szDescribe)
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "网络异常，请稍后再试！")
		end
	end)
end

function BankModifyMediator:onModifyBankValidCode(e, parm)
	if (type(parm) ~= "string") then
		self.viewComponent:emit("M2V_ShowToast", "数据异常，请稍后再试！")
		return
	end
	
	local ServerData = ExternalFun.create_netdata(logincmd.CMD_MB_GetModifyPassVerifyCode)
	ServerData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_GET_MODIFY_BANKINFO_VERIFYCODE)
	
	local phoneNumber = parm
	if not appdf.checkPhone(phoneNumber) then
		self.viewComponent:emit("M2V_ShowToast", "手机号不正确，请重新输入！")
		return
	end
	
	ServerData:pushstring(phoneNumber,yl.LEN_MOBILE_PHONE)
	ServerData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)		
		
	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ServerData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_VERIFY_CODE_RESULT) then
				--清空原来的列表
				local ClientData = ExternalFun.read_netdata(logincmd.CMD_GP_VerifyCodeResult, Datatable.data)
				self.viewComponent:emit("M2V_ModifyBankValidCodeResult", ClientData)
			end
		else
			self.viewComponent:emit("M2V_ShowToast", "网络异常，请稍后再试！")
		end
	end)
end

return BankModifyMediator