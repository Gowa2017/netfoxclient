local BaseMediator = import(".BaseMediator")
local UserInfoMediator = class("UserInfoMediator", BaseMediator)
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function UserInfoMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
				GAME_COMMAMD.UPDATE_USER_SCORE,
				GAME_COMMAMD.UPDATE_INSURE_SCORE,
				GAME_COMMAMD.UPDATE_USER_FACE,
			}
end

function UserInfoMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
	if (msgName == GAME_COMMAMD.UPDATE_USER_SCORE) then
		self.viewComponent:emit("M2V_Update_UserScore", GlobalUserItem.lUserScore)
	elseif (msgName == GAME_COMMAMD.UPDATE_INSURE_SCORE) then
		self.viewComponent:emit("M2V_Update_InsureScore", GlobalUserItem.lUserInsure)
	elseif (msgName == GAME_COMMAMD.UPDATE_USER_FACE) then
		self.viewComponent:emit("M2V_Update_UserFace", GlobalUserItem)
	end
    --deal something
end

function UserInfoMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	self.viewComponent:attach("V2M_ModifySex", handler(self, self.onModifySex))
	self.viewComponent:attach("V2M_BindPhone", handler(self, self.onBindPhone))
	self.viewComponent:attach("V2M_GetVerifyCode", handler(self, self.onGetVerificationCode))
	
	--绑定手机号用的验证码
	self._cookie = nil
	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

function UserInfoMediator:onGetVerificationCode(e, phonenumber)
	self.m_getCodeTime = 0
	if (self.m_getCodeTime >0) then
		return
	end
	
	self.m_getCodeTime = 60
	
    print("a1111111111", phonenumber)
	local ostime = os.time()
	local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
	  appdf.onHttpJsionTable(url ,"GET","action=SendCode&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. phonenumber,function(jstable,jsdata)
      --LogAsset:getInstance():logData("action=GetMobileShareConfig&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime))
		print("appdf.onHttpJsionTable:"..url.."?action=SendCode&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. phonenumber)
		dump(jstable, "jstable", 6)
		local msg = "验证码获取失败"

        print("a2222222", phonenumber)
        --dump(jstable, "dumpjstable")

		if type(jstable) == "table" then
			msg = jstable["msg"]
            local code = jstable["code"]
            dump(self._cookie, "dumpjstableaaa")

			if code==0 then
				local data = jstable["data"]
				if type(data) == "table" then
					self._cookie=data["cookie"]
                    --self._cookie=data["code"]

                    dump(self._cookie, "dumpjstablbbbb")
                    dump(data["code"], "dumpjstablcccc")
                    dump(data["cookie"], "dumpjstabddddd")
                    dump(data["value1"], "dumpjstabeeeeee")
				end

				self.viewComponent:emit("M2V_GetCodeSuccess")
			else
				self.viewComponent:emit("M2V_GetCodeFailed", msg)
			end
		else
			self.viewComponent:emit("M2V_GetCodeFailed", "获取验证码失败，请检查网络")
		end
        --self._scene:dismissPopWait()
		if type(msg) == "string" and "" ~= msg then
			self.viewComponent:emit("M2V_ShowToast", msg)
		end
	end)
end

--修改性别
function UserInfoMediator:onModifySex(e, sex, callback)
	local ModifyUserInfo = CCmd_Data:create(817)
	ModifyUserInfo:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MODIFY_INDIVIDUAL)
	ModifyUserInfo:pushbyte(sex)
	ModifyUserInfo:pushdword(GlobalUserItem.dwUserID)
	ModifyUserInfo:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)

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

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ModifyUserInfo, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_OPERATE_SUCCESS) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				--更新数据
				GlobalUserItem.cbGender = sex
				--resetSystemFace()
				--通知视图
				callback(lResultCode, szDescribe)
			elseif (Datatable.sub == yl.SUB_GP_OPERATE_FAILURE) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				callback(lResultCode, szDescribe)
			end
		else
			callback(-1, "网络错误！")
		end
	end)
end

function UserInfoMediator:resetSystemFace()
	--修改头像
	local sysmodify = ExternalFun.create_netdata(loginCMD.CMD_GP_SystemFaceInfo)
	sysmodify:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_SYSTEM_FACE_INFO)
	sysmodify:pushword(0)
	sysmodify:pushdword(GlobalUserItem.dwUserId)
	sysmodify:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	sysmodify:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ModifyUserInfo, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_USER_FACE_INFO) then
				local wFaceId = pData:readword()
				local dwCustomId = pData:readdword()
				--更新数据
				GlobalUserItem.wFaceID = 0
				GlobalUserItem.dwCustomID = dwCustomId
			end
		end
	end)	
end

function UserInfoMediator:confirmInputBindData(bindphone, newpass,  confirm )	
	local phonelen = ExternalFun.stringLen(bindphone)
	if 0 == phonelen then
		return false, "手机号码不能为空!"
	end
	
	if phonelen<11 then
		return false, "请填写有效的11位手机号码"
	end
	
	local newlen = ExternalFun.stringLen(newpass)
	if 0 == newlen then
		return false, "绑定手机,密码不能为空!"
	end

	if newlen > 26 or newlen < 6 then
		return false, "密码请输入6-26位字符"
	end

	--空格
	local b,e = string.find(newpass, " ")
	if b ~= e then
		return false, "密码不能输入空格字符,请重新输入"
	end

	--密码确认
	if newpass ~= confirm then
		return false, "两次输入的密码不一致,请重新输入"
	end

	-- 首位为字母
	--[[if 1 ~= string.find(newpass, "%a") then
		return false, "密码首位必须为字母，请重新输入！"
	end]]

	-- 与帐号不同
	if string.lower(newpass) == string.lower(GlobalUserItem.szAccount) then
		return false, "密码不能与帐号相同，请重新输入！"
	end
    
	return true
end

--绑定手机
function UserInfoMediator:onBindPhone(e, phonenumber, verifycode, pass, newpass)
	if (self._cookie == nil or self._cookie == "") then
		self.viewComponent:emit("M2V_ShowToast", "请先获取验证码！")
		return
	end
	
	local result, msg = self:confirmInputBindData(phonenumber, pass, newpass)
	if (not result) then
		self.viewComponent:emit("M2V_ShowToast", msg)
		return
	end
	
    local ostime = os.time()
	local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
	  appdf.onHttpJsionTable(url ,"GET","action=BindPhone&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. phonenumber.."&code=".. verifycode.."&pass=".. newpass.."&type=BindPhone&cookie="..self._cookie,function(jstable,jsdata)
      print("appdf.onHttpJsionTablennnnnn:"..url.."?action=BindPhone&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime).."&phone=".. phonenumber.."&code=".. verifycode.."&pass=".. newpass.."&type=BindPhone&cookie="..self._cookie)

		--dump(jstable, "jstable", 6)
		local msg = "抱歉,绑定手机号码失败"
		if type(jstable) == "table" then
			msg = jstable["msg"]
            local code = jstable["code"]
			
			if code == 0 then
				--成功后把填写的手机号赋值给账号跟昵称 密码重新赋值
				if (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_VISITORS) then
					GlobalUserItem.szAccount = phonenumber
					GlobalUserItem.szNickName = phonenumber
					GlobalUserItem.szRegisterMobile = phonenumber
					GlobalUserItem.szPassword = newpass
					GlobalUserItem.cbLogonMode = yl.LOGON_MODE_ACCOUNTS
				else
					GlobalUserItem.szRegisterMobile = phonenumber
					GlobalUserItem.szPassword = newpass
					GlobalUserItem.cbLogonMode = yl.LOGON_MODE_ACCOUNTS
				end
				
				GlobalUserItem.onSaveAccountConfig()
				
				--self:clearEdit()
				self.viewComponent:emit("M2V_BindPhoneResult", 0)
				
				self.viewComponent:emit("M2V_ShowToast", "绑定手机号成功！")
				
				self:onBindSuccess()
				--self:getParent():removeFromParent()
			elseif msg =="验证码错误" then
				self.viewComponent:emit("M2V_BindPhoneResult", -1)
				if(self.count_error==nil) then
				  self.count_error = 1
				else
					self.count_error = self.count_error+1
				end
				if(self.count_error>=3) then
					self._cookie=""
					self.count_error=0
					
					self.viewComponent:emit("M2V_ShowToast", "验证码错误多次，请重新发送验证码")
					return
				end
				
			end
			
		end
		
       self.viewComponent:emit("M2V_ShowToast", msg)
        --self._scene:dismissPopWait()

	end)
end

function UserInfoMediator:onBindSuccess()
	local ostime = os.time()
    local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"操作中..."}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
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

                    local needupdate = false
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
                        needupdate = true
                    end
					
					if (needupdate) then
						--通知更新UI银行金币
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_INSURE_SCORE, GlobalUserItem.lUserInsure)
						--通知更新UI用户金币
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_SCORE, GlobalUserItem.lUserScore)
					end
                end
            end
        end
    end)
end

return UserInfoMediator