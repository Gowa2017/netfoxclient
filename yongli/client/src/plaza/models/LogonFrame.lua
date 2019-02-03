--[[
	登录模块
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")

local LogonFrame = class("LogonFrame",BaseFrame)

local GameServerItem   = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameServerItem")
local GameKindItem   = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameKindItem")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

function LogonFrame:ctor(view,callback)
	LogonFrame.super.ctor(self,view,callback)
	self._plazaVersion = appdf.VersionValue(6,7,0,1)
	self._stationID = yl.STATION_ID
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	local tmp = yl.DEVICE_TYPE_LIST[targetPlatform]
	self._deviceType = tmp or yl.DEVICE_TYPE
	self._szMachine = MultiPlatform:getInstance():getMachineId()

	self.m_angentServerList = {}
	self._tempAllRoom = {}
	self._tempAllKind = {}
end

--连接结果
function LogonFrame:onConnectCompeleted()
	
	if self._logonMode == yl.LOGON_MODE_ACCOUNTS then
		self:sendLogon()
	elseif self._logonMode == yl.LOGON_MODE_REGISTER_ACCOUNTS then
		self:sendAccountRegister() 
	elseif self._logonMode == yl.LOGON_MODE_REGISTER_PHONE then
		self:sendPhoneAccountRegister()
	elseif self._logonMode == yl.LOGON_MODE_VISITORS then 
		self:sendVisitor()
	elseif self._logonMode == yl.LOGON_MODE_WECHAT then
		self:sendThirdPartyLogin()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知登录模式！")
		end		
	end
	
end

--网络信息
function LogonFrame:onSocketEvent(main,sub,pData)
	if (main == yl.MDM_MB_LOGON) or (main == yl.MDM_GP_LOGON) then --登录命令
		self:onSubLogonEvent(sub,pData)
	elseif main == yl.MDM_MB_SERVER_LIST then --房间列表
		self:onRoomListEvent(sub,pData)
	end
end

--玩家信息
function LogonFrame:onSubLogonEvent(sub,pData)
	local sub =pData:getsub()
	--登录成功读取用户信息
	if sub == yl.SUB_MB_LOGON_SUCCESS then
		print("=========== 登录成功 ===========")
		
		
		GlobalUserItem.szMachine = self._szMachine
		GlobalUserItem.onLoadData(pData)
		
		MultiPlatform:getInstance():setAlias(GlobalUserItem.szAccount)
		--GlobalUserItem.szIpAdress = MultiPlatform:getInstance():getClientIpAdress() or ""
		--获取IP归属地
--[[		appdf.onHttpJsionTable(string.format("http://ip.taobao.com/service/getIpInfo.php?ip=%s", GlobalUserItem.szIpAdress),"get","",function (datatable)
			if type(datatable) == "table" then
				local code = datatable["code"]
				if (code == 0) then
					local databuffer = datatable["data"]
					if databuffer then
						GlobalUserItem.szIpLocation = string.format("%s %s %s", databuffer.area, databuffer.region, databuffer.city)
					end
				else--请求成功，但是数据存在异常
					GlobalUserItem.szIpLocation = "未知区域"
				end
			else--请求失败
				GlobalUserItem.szIpLocation = "未知区域"
			end
		end)--]]
		
	--重置房间
		GlobalUserItem.roomlist = {}
		if PriRoom then
			PriRoom:getInstance().m_tabPriModeGame = {}
		end
		self._tempAllRoom = {}
		
		if (GlobalUserItem.szChangeLogonIP and GlobalUserItem.szChangeLogonIP ~= "") then
			GlobalUserItem.szIpLocation = GlobalUserItem.szChangeLogonIP
			self:sendIpLocation()
			--self:onCloseSocket();
		else
			appdf.onHttpJsionTable(string.format("http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip=%s", GlobalUserItem.szIpAdress),"get","",function (datatable)
			--appdf.onHttpJsionTable(string.format("http://api.ip138.com/query/?ip="..GlobalUserItem.szIpAdress.."&datatype=jsonp&token=99f2651ade06917e66d9f977e3c66460"),"get","",function (datatable)
				if type(datatable) == "table" then
					local code = datatable["ret"]
					if (code == 1) then
					--if (code == "ok") then
						GlobalUserItem.szIpLocation = string.format("%s %s %s", datatable.country, datatable.province, datatable.city)
						--GlobalUserItem.szIpLocation = string.format("%s %s %s %s", datatable.data[1], datatable.data[2], datatable.data[3], datatable.data[4])
					else
						GlobalUserItem.szIpLocation = "获取失败"
					end
				else--请求失败
					GlobalUserItem.szIpLocation = "未知区域"
				end
				--更新ip归属地到数据库
				local IpData = CCmd_Data:create(68)
				IpData:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_IP_LOCATION)
				IpData:pushdword(GlobalUserItem.dwUserID)
				IpData:pushstring(GlobalUserItem.szIpLocation,yl.LEN_IP)

				appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, IpData, function (Datatable, Responce)
					if (Responce.code ~= 0) then
						showToast(runScene, "IP地址获取失败！", 1)
					end
				end)
			end)
		end
			
	--会员信息
	elseif sub == yl.SUB_GP_MEMBER_PARAMETER_RESULT then
		local count = pData:readword()
		GlobalUserItem.MemberList = {}
		for i=1,count do
			local item = {}
			item._order = pData:readbyte()
			item._name  = pData:readstring(16)
			item._right = pData:readdword()
			item._task  = pData:readdword()
			item._shop  = pData:readdword()
			item._insure= pData:readdword()
			item._present=pData:readdword()
			item._gift  = pData:readdword() 
			GlobalUserItem.MemberList[item._order] = item

			--dump(item, "会员信息", 6)
		end
	--登录失败
	elseif sub == yl.SUB_MB_LOGON_FAILURE then
		print("=========== 登录失败 ===========")
		-- CMD_MB_LogonFailure
		local cmdtable = ExternalFun.read_netdata(logincmd.CMD_MB_LogonFailure, pData)
		if 10 == cmdtable.lResultCode then
			GlobalUserItem.setBindingAccount()
			if nil ~= self._callBack then
				self._callBack(10,cmdtable.szDescribeString)
			end
		end
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,cmdtable.szDescribeString)
		end		
	--更新APP
	elseif sub == yl.SUB_MB_UPDATE_NOTIFY then
		local cbMustUpdate = pData:readbyte()
		local cbAdviceUpdate = pData:readbyte()
		local dwCurrentVersion = pData:readdword()
		print("update_notify:"..cbMustUpdate.."#"..cbAdviceUpdate.."#"..dwCurrentVersion)
		local tmpV = appdf.ValuetoVersion(dwCurrentVersion)
		print(tmpV.p.."."..tmpV.m.."."..tmpV.s.."."..tmpV.b)
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"版本信息错误！")
		end		
	end
end

--房间信息
function LogonFrame:onRoomListEvent(sub,pData)
	if sub == yl.SUB_MB_LIST_FINISH then	--列表完成
			self:onCloseSocket();
		if self._logonMode == yl.LOGON_MODE_ACCOUNTS then
			GlobalUserItem.szAccount = self._szAccount
			GlobalUserItem.szPassword = self._szPassword
		elseif self._logonMode == yl.LOGON_MODE_REGISTER_ACCOUNTS then
			GlobalUserItem.szAccount = self._szRegAccount
			GlobalUserItem.szPassword = self._szRegPassword
		elseif self._logonMode == yl.LOGON_MODE_REGISTER_PHONE then
			GlobalUserItem.szAccount = self._szMobilePhone
			GlobalUserItem.szRegisterMobile = self._szMobilePhone
			GlobalUserItem.szPassword = self._szRegPassword
		elseif self._logonMode == yl.LOGON_MODE_VISITORS then 						--游客登录
		elseif self._logonMode == yl.LOGON_MODE_WECHAT then 						--微信登陆
		end
		
		-- 整理类型列表
		table.sort(self._tempAllKind, function(a, b)
			return a.wSortID < b.wSortID
		end)		
		for k,v in pairs(self._tempAllKind) do
			--加入缓存
			table.insert(GlobalUserItem.kindlist,v)
		end		
		
		-- 整理房间列表
		for k,v in pairs(self._tempAllRoom) do
			table.sort(v, function(a, b)
				return a.wSortID < b.wSortID
			end)
			for i = 1, #v do
				v[i]._nRoomIndex = i
			end
			local roomlist = {}
			--记录游戏ID
			table.insert(roomlist,k)
			table.insert(roomlist,v)
			if PriRoom then
				PriRoom:getInstance().m_tabPriRoomList[k] = v
			end
			--加入缓存
			table.insert(GlobalUserItem.roomlist,roomlist)
		end

		--登录完成
		--self:onCloseSocket() --无状态
		if nil ~= self._callBack then
			self._callBack(1)
		end
	
	elseif sub == yl.SUB_MB_LIST_SERVER then	--列表数据
		self:onSubRoomListInfo(pData)
	elseif sub == yl.SUB_MB_LIST_KIND then		--游戏类型数据
		self:onSubKindListInfo(pData)
	elseif sub == yl.SUB_MB_AGENT_KIND then 	--代理列表
		self:onSubAngentListInfo(pData)
	end
end

--房间列表
function LogonFrame:onSubRoomListInfo(pData)
	--计算房间数目
	local len = pData:getlen()
	print("=============== onSubRoomListInfo ================")
	print("onSubRoomListInfo:"..len)
	if (len - math.floor(len/yl.LEN_GAME_SERVER_ITEM)*yl.LEN_GAME_SERVER_ITEM) ~= 0 then
		print("roomlist_len_error"..len)
		return
	end
	local itemcount =  math.floor(len/yl.LEN_GAME_SERVER_ITEM)
	print("=============== onSubRoomListInfo ================ ", itemcount)
	
	--读取房间信息
	for i = 1,itemcount do
		local item = GameServerItem:create()
						:onInit(pData)
		if not item then 
			break
		end
		if nil == self._tempAllRoom[item.wKindID] then
			self._tempAllRoom[item.wKindID] = {}
		end

		if item.wServerType == yl.GAME_GENRE_PERSONAL then
			if GlobalUserItem.bEnableRoomCard then
				if PriRoom then
					PriRoom:getInstance().m_tabPriModeGame[item.wKindID] = true
				end				
				table.insert(self._tempAllRoom[item.wKindID], item)
			end
		else
			table.insert(self._tempAllRoom[item.wKindID], item)
		end		
	end
end

function LogonFrame:onSubKindListInfo(pData)
	--计算游戏类型数目
	local len = pData:getlen()

	if (len - math.floor(len/yl.LEN_GAME_KIND_ITEM)*yl.LEN_GAME_KIND_ITEM) ~= 0 then
		print("roomlist_len_error"..len)
		return
	end
	local itemcount =  math.floor(len/yl.LEN_GAME_KIND_ITEM)
	
	--读取房间信息
	for i = 1,itemcount do
		local item = GameKindItem:create()
						:onInit(pData)
		if not item then 
			break
		end
		
		table.insert(self._tempAllKind, item)
	end	
end

function LogonFrame:onSubAngentListInfo(pData)
	self.m_angentServerList = {}
	--计算数目
	local len = pData:getlen()
	print("=============== onSubAngentListInfo ================")
	print("onSubAngentListInfo:"..len)
	if (len - math.floor(len/4)*4) ~= 0 then
		print("angentlist_len_error"..len)
		return
	end
	print("=============== onSubAngentListInfo ================")
	local itemcount =  math.floor(len/4)
	
	--读取房间信息
	for i = 1,itemcount do
		local kind = pData:readword()
		local sort = pData:readword()
		
		self.m_angentServerList[kind] = {KindID = kind, SortID = sort}
	end
end

function LogonFrame:onLogonByAccount(szAccount,szPassword)
	--数据保存
	self._szAccount = szAccount--"asd7002";
	self._szPassword= szPassword--md5("asd7002")
	self._szMobilePhone = "0123456789"
	--发送开始登录
	local customEvent = cc.EventCustom:new(yl.RY_START_LOGON)
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	--记录模式
	self._logonMode = yl.LOGON_MODE_ACCOUNTS
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:onLogonByVisitor()
	--记录模式	
	self._logonMode = yl.LOGON_MODE_VISITORS
	--发送开始登录
	local customEvent = cc.EventCustom:new(yl.RY_START_LOGON)
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:onLoginByThirdParty(szAccount, szNick, cbgender, platform)
	--数据保存
	self._szAccount = szAccount
	self._szNickName = szNick
	self._cbLoginGender = cbgender
	self._cbPlatform = platform
	--发送开始登录
	local customEvent = cc.EventCustom:new(yl.RY_START_LOGON)
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)	--记录模式
	self._logonMode = yl.LOGON_MODE_WECHAT
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:onRegisterByPhone(szPhoneNumber,szPassword,szVerifyCode,cbGender,szSpreader)
	--数据保存
	self._szMobilePhone = szPhoneNumber
	self._szRegPassword= szPassword
	self._szRegVerifyCode = szVerifyCode
	self._cbRegGender = cbGender
	self._szMobilePhone = "0123456789"
	self._szSpreader = szSpreader

	--记录模式
	self._logonMode = yl.LOGON_MODE_REGISTER_PHONE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:onRegisterByAccount(szAccount,szPassword,szRePassword,cbGender,szSpreader)
	--数据保存
	self._szRegAccount = szAccount
	self._szRegPassword= szPassword
	self._cbRegGender = cbGender
	self._szMobilePhone = "0123456789"
	self._szSpreader = szSpreader

	--记录模式
	self._logonMode = yl.LOGON_MODE_REGISTER_ACCOUNTS
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
		return false
	end
	return true
end

function LogonFrame:sendLogon()
	local LogonData = CCmd_Data:create(235)
	LogonData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_LOGON_ACCOUNTS)
	LogonData:pushword(--[[GlobalUserItem.getCurGameKind()]]yl.INVALID_WORD)
	LogonData:pushdword(self._plazaVersion)
	LogonData:pushbyte(self._deviceType)

	LogonData:pushstring(string.upper(md5(self._szPassword)),yl.LEN_MD5)

	LogonData:pushstring(self._szAccount,yl.LEN_ACCOUNTS)
	LogonData:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	LogonData:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)

	--print("send logon:account-"..self._szAccount.." password-"..self._szPassword)
	
	--发送失败
	if not self:sendSocketData(LogonData) and nil ~= self._callBack then
		self._callBack(-1,"发送登录失败！")
	end
end

function LogonFrame:sendIpLocation()
	local IpData = CCmd_Data:create(68)
	IpData:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_IP_LOCATION)
	IpData:pushdword(GlobalUserItem.dwUserID)
	IpData:pushstring(GlobalUserItem.szIpLocation,yl.LEN_IP)
	--发送失败
	if not self:sendSocketData(IpData) and nil ~= self._callBack then
		self._callBack(-1,"发送登录失败！")
	end
end

function LogonFrame:sendVisitor()
	local VisitorData = ExternalFun.create_netdata(logincmd.CMD_MB_LogonVisitor)
	
	VisitorData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_LOGON_VISITOR)
	VisitorData:pushword(yl.INVALID_WORD)
	VisitorData:pushdword(self._plazaVersion)
	VisitorData:pushstring(BaseConfig.AGENT_IDENTIFICATION,yl.LEN_ACCOUNTS)
	VisitorData:pushbyte(self._deviceType)

	self._szMachine = MultiPlatform:getInstance():getMachineId()
	VisitorData:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	VisitorData:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)

		--发送失败
	if not self:sendSocketData(VisitorData) and nil ~= self._callBack then
		self._callBack(-1,"发送游客登录失败！")
	end
end

function LogonFrame:sendThirdPartyLogin( )
	local cmddata = ExternalFun.create_netdata(logincmd.CMD_MB_LogonOtherPlatform)
	cmddata:setcmdinfo(logincmd.MDM_MB_LOGON,logincmd.SUB_MB_LOGON_OTHERPLATFORM)

	cmddata:pushword(--[[GlobalUserItem.getCurGameKind()]]yl.INVALID_WORD)
	cmddata:pushdword(self._plazaVersion)
	cmddata:pushbyte(self._deviceType)
	cmddata:pushbyte(self._cbLoginGender)
	cmddata:pushbyte(self._cbPlatform)
	cmddata:pushstring(self._szAccount,33) --LEN_USER_UIN
	cmddata:pushstring(self._szNickName,yl.LEN_NICKNAME)
	cmddata:pushstring(self._szNickName,yl.LEN_COMPELLATION)
	cmddata:pushstring(BaseConfig.AGENT_IDENTIFICATION,yl.LEN_ACCOUNTS)
	cmddata:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	cmddata:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)

	if not self:sendSocketData(cmddata) and nil ~= self._callBack then
		self._callBack(-1,"发送微信登录失败！")
	end
end

function LogonFrame:sendAccountRegister()
	local RegisterData = ExternalFun.create_netdata(logincmd.CMD_MB_RegisterAccounts)
	
	RegisterData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_REGISTER_ACCOUNTS)

	RegisterData:pushword(--[[GlobalUserItem.getCurGameKind()]]yl.INVALID_WORD)
	RegisterData:pushdword(self._plazaVersion)
	RegisterData:pushbyte(self._deviceType)

	local md5pass = md5(self._szRegPassword)
	RegisterData:pushstring(string.upper(md5pass),yl.LEN_MD5)

	RegisterData:pushword(1)
	RegisterData:pushbyte(self._cbRegGender)

	RegisterData:pushstring(self._szRegAccount,yl.LEN_ACCOUNTS)
	RegisterData:pushstring(self._szRegAccount,yl.LEN_NICKNAME)

	RegisterData:pushdword(tonumber(self._szSpreader))
	RegisterData:pushstring(BaseConfig.AGENT_IDENTIFICATION,yl.LEN_ACCOUNTS)

	RegisterData:pushstring(self._szMachine,yl.LEN_MACHINE_ID)
	RegisterData:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)

	--发送失败
	if not self:sendSocketData(RegisterData) and nil ~= self._callBack then
		self._callBack(-1,"发送注册失败！")
	end
end

function LogonFrame:sendPhoneAccountRegister()
	local RegisterData = ExternalFun.create_netdata(logincmd.CMD_MB_PhoneRegisterAccounts)
	
	RegisterData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_PHONE_REGISTER_VERIFYCODE)

	RegisterData:pushword(yl.INVALID_WORD)
	RegisterData:pushdword(self._plazaVersion)
	RegisterData:pushbyte(self._deviceType)

	local md5pass = md5(self._szRegPassword)
	RegisterData:pushstring(string.upper(md5pass),yl.LEN_MD5)

	RegisterData:pushword(1)
	RegisterData:pushbyte(self._cbRegGender)

	RegisterData:pushstring(self._szMobilePhone,yl.LEN_MOBILE_PHONE)
	RegisterData:pushstring(self._szMobilePhone,yl.LEN_NICKNAME)

	RegisterData:pushdword(tonumber(self._szSpreader))
	RegisterData:pushstring(BaseConfig.AGENT_IDENTIFICATION,yl.LEN_ACCOUNTS)
	RegisterData:pushstring(self._szRegVerifyCode,yl.LEN_VERIFY_CODE)
	RegisterData:pushstring(self._szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(RegisterData) and nil ~= self._callBack then
		self._callBack(-1,"发送注册失败！")
	end
end

return LogonFrame