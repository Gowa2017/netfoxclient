local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local ModifyFrame = class("ModifyFrame",BaseFrame)

function ModifyFrame:ctor(view,callbcak)
	ModifyFrame.super.ctor(self,view,callbcak)
end
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

--修改系统头像
local MODIFY_SYSTEM_FACE = 4
--注册绑定
local REGISTE_BINDING = 5
-- 绑定设备
local BIND_MACHINE = 6
ModifyFrame.BIND_MACHINE = BIND_MACHINE
-- 查询个人信息
local QUERY_USERINFO = 7
-- 填写推广员
local INPUT_SPREADER = 8
ModifyFrame.INPUT_SPREADER = INPUT_SPREADER

--连接结果
function ModifyFrame:onConnectCompeleted()

	if self._oprateCode == 0 then	--修改信息
		self:sendModifyUserInfo()
	elseif self._oprateCode == 1 then	--修改登录密码
		self:sendModifyLogonPass()
	elseif self._oprateCode == 2 then 	--修改银行密码
		self:sendModifyBankPass()
	elseif self._oprateCode == 3 then 	--账号绑定
		self:sendAccountBinding()
	elseif self._oprateCode == MODIFY_SYSTEM_FACE then 	--修改系统头像
		self:sendModifySystemFace()
	elseif self._oprateCode == REGISTE_BINDING then --注册绑定
		self:sendAccountRegisteBinding()
	elseif self._oprateCode == BIND_MACHINE then --绑定设备
		self:sendBindMachine()
	elseif self._oprateCode == QUERY_USERINFO then -- 查询信息
		self:sendQueryUserInfo()
	elseif self._oprateCode == INPUT_SPREADER then -- 填写推广员
		self:sendInputSpreader()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知操作模式！")
		end		
	end
end

--网络信息(短连接)
function ModifyFrame:onSocketEvent(main,sub,pData)
	if main == yl.MDM_GP_USER_SERVICE then --用户服务
		if sub == yl.SUB_GP_OPERATE_SUCCESS then
			self:onSubOperateSuccess(pData)
		elseif sub == yl.SUB_GP_OPERATE_FAILURE then
			self:onSubOperateFailure(pData)
		elseif sub == yl.SUB_GP_USER_FACE_INFO then 	--修改头像
			self:onSubUserFaceInfoResult(pData)
		elseif sub == yl.SUB_GP_USER_INDIVIDUAL then
			self:onSubQueryUserInfoResult(pData)
		end
	end
	self:onCloseSocket()
end

--网络消息(长连接)
function ModifyFrame:onGameSocketEvent(main,sub,pData)
	
end

function ModifyFrame:onSubOperateSuccess(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	if self._oprateCode == 1 then
		GlobalUserItem.szPassword = self._szLogonNew
		--保存数据
		GlobalUserItem.onSaveAccountConfig()
	elseif self._oprateCode == 0 then
		GlobalUserItem.cbGender = self._cbGender
		GlobalUserItem.szNickName = self._szNickname
		GlobalUserItem.szSign = self._szSign
	elseif self._oprateCode == 3 then
		if nil ~= self._callBack then
			self._callBack(2,szDescribe)
		end		
		return
	elseif self._oprateCode == REGISTE_BINDING then
		if nil ~= self._callBack then
			self._callBack(2,szDescribe)
		end		
		return
	elseif self._oprateCode == BIND_MACHINE then
		if nil ~= self._callBack then
			self._callBack(BIND_MACHINE,szDescribe)
		end		
		return
	elseif self._oprateCode == INPUT_SPREADER then
		GlobalUserItem.szSpreaderAccount = self._SpreaderID
		if nil ~= self._callBack then
			self._callBack(INPUT_SPREADER,szDescribe)
		end		
		return
	end
	if nil ~= self._callBack then
		self._callBack(1,szDescribe)
	end	
end

function ModifyFrame:onSubOperateFailure(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	if nil ~= self._callBack then
		self._callBack(-1,szDescribe)
	end	
end

function ModifyFrame:onSubUserFaceInfoResult( pData )
	local wFaceId = pData:readword()
	local dwCustomId = pData:readdword()

	GlobalUserItem.wFaceID = wFaceId
	GlobalUserItem.dwCustomID = dwCustomId

	if nil ~= self._callBack then
		self._callBack(yl.SUB_GP_USER_FACE_INFO,"头像修改成功")
	end	
end

function ModifyFrame:onSubQueryUserInfoResult( pData )
	local cmd_table = ExternalFun.read_netdata(loginCMD.CMD_GP_UserIndividual, pData)
	-- 附加包
	local curlen = pData:getcurlen()
	local datalen = pData:getlen()
	print("*** curlen-"..curlen)
	print("*** datalen-"..datalen)

	local tmpSize = 0
	local tmpCmd = 0
	while curlen < datalen do
		tmpSize = pData:readword()
		tmpCmd = pData:readword()
		if not tmpSize or not tmpCmd then
		 	break
		end
		print("*** tmpSize-"..tmpSize)
		print("*** tmpCmd-"..tmpCmd)

		if tmpCmd == loginCMD.DTP_GP_UI_SPREADER then -- 推广
			GlobalUserItem.szSpreaderAccount = pData:readstring(tmpSize/2) or ""
		elseif tmpCmd == loginCMD.DTP_GP_UI_QQ then
			GlobalUserItem.szQQNumber = pData:readstring(tmpSize/2) or ""
			print("qq " .. GlobalUserItem.szQQNumber)
		elseif tmpCmd == loginCMD.DTP_GP_UI_EMAIL then
			GlobalUserItem.szEmailAddress = pData:readstring(tmpSize/2) or ""
			print("email " .. GlobalUserItem.szEmailAddress)
		elseif tmpCmd == loginCMD.DTP_GP_UI_SEAT_PHONE then
			GlobalUserItem.szSeatPhone = pData:readstring(tmpSize/2) or ""
			print("szSeatPhone " .. GlobalUserItem.szSeatPhone)
		elseif tmpCmd == loginCMD.DTP_GP_UI_MOBILE_PHONE then
			GlobalUserItem.szMobilePhone = pData:readstring(tmpSize/2) or ""
			print("szMobilePhone " .. GlobalUserItem.szMobilePhone)
		elseif tmpCmd == loginCMD.DTP_GP_UI_COMPELLATION then
			GlobalUserItem.szTrueName = pData:readstring(tmpSize/2) or ""
			print("szTrueName " .. GlobalUserItem.szTrueName)
		elseif tmpCmd == loginCMD.DTP_GP_UI_DWELLING_PLACE then
			GlobalUserItem.szAddress = pData:readstring(tmpSize/2) or ""
			print("szAddress " .. GlobalUserItem.szAddress)
		elseif tmpCmd == loginCMD.DTP_GP_UI_PASSPORTID then
			GlobalUserItem.szPassportID = pData:readstring(tmpSize/2) or ""
			print("szPassportID " .. GlobalUserItem.szPassportID)
		elseif tmpCmd == loginCMD.DTP_GP_UI_SPREADER then
			GlobalUserItem.szSpreaderAccount = pData:readstring(tmpSize/2) or ""
			print("szSpreaderAccount " .. GlobalUserItem.szSpreaderAccount)
		elseif tmpCmd == 0 then
			break
		else
			for i = 1, tmpSize do
				if not pData:readbyte() then
					break
				end
			end
		end
		curlen = pData:getcurlen()
	end
	if nil ~= self._callBack then
		self._callBack(loginCMD.SUB_GP_USER_INDIVIDUAL)
	end
end

function ModifyFrame:sendModifyUserInfo()
	local ModifyUserInfo = CCmd_Data:create(817)

	ModifyUserInfo:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MODIFY_INDIVIDUAL)
	ModifyUserInfo:pushbyte(self._cbGender)
	ModifyUserInfo:pushdword(GlobalUserItem.dwUserID)
	ModifyUserInfo:pushstring(md5(GlobalUserItem.szPassword),33)

	------
	--附加信息
	-- 昵称
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(yl.DTP_GP_UI_NICKNAME)
	ModifyUserInfo:pushstring(self._szNickname,yl.LEN_NICKNAME)
	-- 签名
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(yl.DTP_GP_MODIFY_UNDER_WRITE)
	ModifyUserInfo:pushstring(self._szSign,yl.LEN_UNDER_WRITE)
	-- qq
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_QQ)
	ModifyUserInfo:pushstring(GlobalUserItem.szQQNumber,yl.LEN_QQ)
	-- email
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_EMAIL)
	ModifyUserInfo:pushstring(GlobalUserItem.szEmailAddress,yl.LEN_EMAIL)
	-- 座机
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_SEAT_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.szSeatPhone,yl.LEN_SEAT_PHONE)
	-- 手机
	ModifyUserInfo:pushword(24)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_MOBILE_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.szMobilePhone,yl.LEN_MOBILE_PHONE)
	-- 真实姓名
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_COMPELLATION)
	ModifyUserInfo:pushstring(GlobalUserItem.szTrueName,yl.LEN_COMPELLATION)
	-- 联系地址
	ModifyUserInfo:pushword(256)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_DWELLING_PLACE)
	ModifyUserInfo:pushstring(GlobalUserItem.szAddress,yl.LEN_DWELLING_PLACE)
	-- 身份证
	ModifyUserInfo:pushword(38)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_PASSPORTID)
	ModifyUserInfo:pushstring(GlobalUserItem.szPassportID,yl.LEN_PASS_PORT_ID)
	--附加信息
	------

	if not self:sendSocketData(ModifyUserInfo) and nil ~= self._callBack then
		self._callBack(-1,"发送修改资料失败！")
	end
end

function ModifyFrame:sendModifyLogonPass()
	local ModifyLogonPass = CCmd_Data:create(136)	
	ModifyLogonPass:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MODIFY_LOGON_PASS)
	
	ModifyLogonPass:pushdword(GlobalUserItem.dwUserID)
	ModifyLogonPass:pushstring(md5(self._szLogonNew),33)
	ModifyLogonPass:pushstring(md5(self._szLogonOld),33)

	if not self:sendSocketData(ModifyLogonPass) and nil ~= self._callBack then
		self._callBack(-1,"发送修改登录失败！")
	end
end
function ModifyFrame:sendModifyBankPass()
	local ModifyInsurePass = CCmd_Data:create(136)

	ModifyInsurePass:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MODIFY_INSURE_PASS)
	
	ModifyInsurePass:pushdword(GlobalUserItem.dwUserID)
	ModifyInsurePass:pushstring(md5(self._szBankNew),33)
	ModifyInsurePass:pushstring(md5(self._szBankOld),33)

	if not self:sendSocketData(ModifyInsurePass) and nil ~= self._callBack then
		self._callBack(-1,"发送修改银行失败！")
	end
end

--CMD_GP_AccountBind_Exists
function ModifyFrame:sendAccountBinding()
	local AccountBinding = CCmd_Data:create(266) --4+66+66+64+66

	AccountBinding:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_ACCOUNT_BINDING_EXISTS)
	
	AccountBinding:pushdword(GlobalUserItem.dwUserID)
	AccountBinding:pushstring(md5(GlobalUserItem.szPassword),33)
	AccountBinding:pushstring(self._szMachine,yl.LEN_MACHINE_ID)

	AccountBinding:pushstring(self._szAccount,32)
	AccountBinding:pushstring(self._szPassword,33)

	if not self:sendSocketData(AccountBinding) and nil ~= self._callBack then
		self._callBack(-1,"发送绑定账号失败！")
	end
end

--CMD_GP_AccountBind
function ModifyFrame:sendAccountRegisteBinding()
	local AccountBinding = CCmd_Data:create(330) --4+66+66+64+66+64
	AccountBinding:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_ACCOUNT_BINDING)

	AccountBinding:pushdword(GlobalUserItem.dwUserID)
	AccountBinding:pushstring(md5(GlobalUserItem.szPassword),33)
	AccountBinding:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	
	AccountBinding:pushstring(self._szAccount,32)
	AccountBinding:pushstring(self._szPassword,33)
	AccountBinding:pushstring(self._szSpreader,yl.LEN_ACCOUNTS)

	if not self:sendSocketData(AccountBinding) and nil ~= self._callBack then
		self._callBack(-1,"发送绑定账号失败！")
	end
end

--发送修改头像
function ModifyFrame:sendModifySystemFace( )
	local sysmodify = ExternalFun.create_netdata(loginCMD.CMD_GP_SystemFaceInfo)
	sysmodify:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_SYSTEM_FACE_INFO)
	sysmodify:pushword(self._wFaceId)
	sysmodify:pushdword(self._dwUserId)
	sysmodify:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	sysmodify:pushstring(self._szMachine,yl.LEN_MACHINE_ID)

	if not self:sendSocketData(sysmodify) and nil ~= self._callBack then
		self._callBack(-1,"修改用户头像失败！")
	end
end

-- 发送绑定设备
function ModifyFrame:sendBindMachine()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_ModifyMachine)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_MODIFY_MACHINE)
	buffer:pushbyte(self._cbBind)
	buffer:pushdword(GlobalUserItem.dwUserID)
	buffer:pushstring(md5(self._strpw), yl.LEN_MD5)
	buffer:pushstring(self._szMachine,yl.LEN_MACHINE_ID)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"绑定设备失败！")
	end
end

-- 发送查询信息
function ModifyFrame:sendQueryUserInfo()
	local buffer = ExternalFun.create_netdata(loginCMD.CMD_GP_QueryIndividual)
	buffer:setcmdinfo(loginCMD.MDM_GP_USER_SERVICE,loginCMD.SUB_GP_QUERY_INDIVIDUAL)
	buffer:pushdword(GlobalUserItem.dwUserID)
	buffer:pushstring(md5(GlobalUserItem.szPassword), yl.LEN_MD5)

	if not self:sendSocketData(buffer) and nil ~= self._callBack then
		self._callBack(-1,"查询信息失败！")
	end
end

-- 发送填推广员
function ModifyFrame:sendInputSpreader()
	local ModifyUserInfo = CCmd_Data:create(817)
	ModifyUserInfo:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MODIFY_INDIVIDUAL)
	ModifyUserInfo:pushbyte(GlobalUserItem.cbGender)
	ModifyUserInfo:pushdword(GlobalUserItem.dwUserID)
	ModifyUserInfo:pushstring(md5(GlobalUserItem.szPassword),33)

	------
	--附加信息
	-- 昵称
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(yl.DTP_GP_UI_NICKNAME)
	ModifyUserInfo:pushstring(GlobalUserItem.szNickName,yl.LEN_NICKNAME)
	-- 签名
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(yl.DTP_GP_MODIFY_UNDER_WRITE)
	ModifyUserInfo:pushstring(GlobalUserItem.szSign,yl.LEN_UNDER_WRITE)
	-- qq
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_QQ)
	ModifyUserInfo:pushstring(GlobalUserItem.szQQNumber,yl.LEN_QQ)
	-- email
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_EMAIL)
	ModifyUserInfo:pushstring(GlobalUserItem.szEmailAddress,yl.LEN_EMAIL)
	-- 座机
	ModifyUserInfo:pushword(66)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_SEAT_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.szSeatPhone,yl.LEN_SEAT_PHONE)
	-- 手机
	ModifyUserInfo:pushword(24)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_MOBILE_PHONE)
	ModifyUserInfo:pushstring(GlobalUserItem.szMobilePhone,yl.LEN_MOBILE_PHONE)
	-- 真实姓名
	ModifyUserInfo:pushword(32)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_COMPELLATION)
	ModifyUserInfo:pushstring(GlobalUserItem.szTrueName,yl.LEN_COMPELLATION)
	-- 联系地址
	ModifyUserInfo:pushword(256)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_DWELLING_PLACE)
	ModifyUserInfo:pushstring(GlobalUserItem.szAddress,yl.LEN_DWELLING_PLACE)
	-- 身份证
	ModifyUserInfo:pushword(38)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_PASSPORTID)
	ModifyUserInfo:pushstring(GlobalUserItem.szPassportID,yl.LEN_PASS_PORT_ID)
	-- 推广员
	ModifyUserInfo:pushword(64)
	ModifyUserInfo:pushword(loginCMD.DTP_GP_UI_SPREADER)
	ModifyUserInfo:pushstring(self._SpreaderID,32)
	--附加信息
	------

	if not self:sendSocketData(ModifyUserInfo) and nil ~= self._callBack then
		self._callBack(-1,"发送修改资料失败！")
	end
end

function ModifyFrame:onModifyUserInfo(cbGender,szNickname,szSign)
	self._oprateCode = 0
	self._cbGender = cbGender
	if szNickname == nil then
		self._szNickname = GlobalUserItem.szNickName
	else
		self._szNickname = szNickname
	end

	self._szSign = szSign

	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end
function ModifyFrame:onModifyLogonPass(szOld,szNew)
	self._oprateCode = 1
	self._szLogonOld = szOld
	self._szLogonNew = szNew
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end
function ModifyFrame:onModifyBankPass(szOld,szNew)
	self._oprateCode = 2
	self._szBankOld = szOld
	self._szBankNew = szNew
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function ModifyFrame:onAccountBinding(szAccount,szPassword)
	--数据保存
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._szMachine = GlobalUserItem.szMachine

	--记录模式
	self._oprateCode = 3
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function ModifyFrame:onAccountRegisterBinding(szAccount, szPassword, szSpreader)
	--数据保存
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._szMachine = GlobalUserItem.szMachine
	self._szSpreader = szSpreader

	--记录模式
	self._oprateCode = REGISTE_BINDING
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function ModifyFrame:onModifySystemHead( wFaceId )
	--数据保存
	self._wFaceId = wFaceId
	self._dwUserId = GlobalUserItem.dwUserID
	self._szMachine = GlobalUserItem.szMachine

	--记录模式
	self._oprateCode = MODIFY_SYSTEM_FACE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 机器绑定
function ModifyFrame:onBindingMachine( cbBind, strpw )
	-- 数据保存
	self._cbBind = cbBind
	self._strpw = strpw
	self._szMachine = GlobalUserItem.szMachine

	--记录模式
	self._oprateCode = BIND_MACHINE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 信息查询
function ModifyFrame:onQueryUserInfo()
	--记录模式
	self._oprateCode = QUERY_USERINFO
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

-- 发送填写推广员
function ModifyFrame:onBindSpreader( nSpreaderID )
	self._SpreaderID = nSpreaderID
	--记录模式
	self._oprateCode = INPUT_SPREADER
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

return ModifyFrame