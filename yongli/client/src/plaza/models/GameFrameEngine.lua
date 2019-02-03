local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local GameFrameEngine = class("GameFrameEngine",BaseFrame)

local UserItem = appdf.req(appdf.CLIENT_SRC.."plaza.models.ClientUserItem")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")
local QueryExit = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.QueryDialog")

function GameFrameEngine:ctor(view,callbcak)
	GameFrameEngine.super.ctor(self,view,callbcak)
	self._kindID = 0
	self._kindVersion = 0

	-- 短连接服务
	self._shotFrame = nil
	-- 语音队列
	self._tabVoiceMsgQueue = {}
	self._bPlayVoiceRecord = false
end

function GameFrameEngine:setKindInfo(id,version)
	self._kindID = id 
	self._kindVersion = version
	return self
end

function GameFrameEngine:onInitData()

	--房间信息 以后转移
	self._wTableCount = 0
	self._wChairCount = 0
	self._wServerType = 0
	self._dwServerRule = 0
	self._UserList = {}
	self._tableUserList = {}
	self._tableStatus = {}
	--self._delayEnter = false

	self._wTableID	 	= yl.INVALID_TABLE
	self._wChairID	 	= yl.INVALID_CHAIR
	self._cbTableLock	= 0
	self._cbGameStatus 	= 0
	self._cbAllowLookon	= 0
	self.bChangeDesk = false
	self.bEnterAntiCheatRoom = false 		--进入防作弊房间
	GlobalUserItem.bWaitQuit = false 		-- 退出等待
	self._tabVoiceMsgQueue = {}
	self._bPlayVoiceRecord = false
	self._nPlayVoiceId = nil
end

function GameFrameEngine:setEnterAntiCheatRoom( bEnter )
	self.bEnterAntiCheatRoom = bEnter
end

--连接房间
function GameFrameEngine:onLogonRoom()
	self._roomInfo = GlobalUserItem.GetRoomInfo()

	if not self._roomInfo and nil ~= self._callBack then
		self._callBack(-1,"获取房间信息失败！")
		return
	end
	print("登录房间："..self._roomInfo.szServerAddr.."#"..self._roomInfo.wServerPort)
	if not self:onCreateSocket(self._roomInfo.szServerAddr,self._roomInfo.wServerPort) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end
--连接结果
function GameFrameEngine:onConnectCompeleted()
	if nil ~= self._socket then
		self._socket:setdelaytime(0)
	end

	local dataBuffer = CCmd_Data:create(213)

	--初始化参数
	self:onInitData()

	dataBuffer:setcmdinfo(yl.MDM_GR_LOGON,yl.SUB_GR_LOGON_MOBILE)
	dataBuffer:pushword(self._kindID)
	dataBuffer:pushdword(self._kindVersion)
	
	dataBuffer:pushbyte(yl.DEVICE_TYPE)
	dataBuffer:pushword(0x0011)
	dataBuffer:pushword(255)

	dataBuffer:pushdword(GlobalUserItem.dwUserID)
	dataBuffer:pushstring(GlobalUserItem.szDynamicPass,33)
	dataBuffer:pushstring(GlobalUserItem.szRoomPasswd,33)
	dataBuffer:pushstring(GlobalUserItem.szMachine,33)

	if not self:sendSocketData(dataBuffer) and nil ~= self._callBack then
		self._callBack(-1,"发送登录失败！")
	end
	
	if (self._callBack ~= nil) then
		self._callBack(0,"")		
	end
end

--网络信息
function GameFrameEngine:onSocketEvent(main,sub,dataBuffer)
	--登录信息
	if main == yl.MDM_GR_LOGON then
		self:onSocketLogonEvent(sub,dataBuffer)
	--配置信息
	elseif main == yl.MDM_GR_CONFIG then
		self:onSocketConfigEvent(sub,dataBuffer)
	--用户信息
	elseif main == yl.MDM_GR_USER then
		self:onSocketUserEvent(sub,dataBuffer)
	--状态信息
	elseif main == yl.MDM_GR_STATUS then
		self:onSocketStatusEvent(sub,dataBuffer)
	elseif main == yl.MDM_GF_FRAME then
		self:onSocketFrameEvent(sub,dataBuffer)
	elseif main == yl.MDM_CM_SYSTEM then
		self:onSocketFrameSystem(sub,dataBuffer)
	elseif main == yl.MDM_GF_GAME then
		if self._viewFrame and self._viewFrame.onEventGameMessage then
			self._viewFrame:onEventGameMessage(sub,dataBuffer)
		end
	elseif main == game_cmd.MDM_GR_INSURE then
		if self._viewFrame and self._viewFrame.onSocketInsureEvent then
			self._viewFrame:onSocketInsureEvent(sub,dataBuffer)
		end

		-- 短连接服务
		if nil ~= self._shotFrame and nil ~= self._shotFrame.onGameSocketEvent then
			self._shotFrame:onGameSocketEvent(main,sub,dataBuffer)
		end
	--[[elseif main == game_cmd.MDM_GR_TASK 
		or main == game_cmd.MDM_GR_PROPERTY 
		then
		-- 短连接服务
		if nil ~= self._shotFrame and nil ~= self._shotFrame.onGameSocketEvent then
			self._shotFrame:onGameSocketEvent(main,sub,dataBuffer)
		end]]
	else
		-- 短连接服务
		if nil ~= self._shotFrame and nil ~= self._shotFrame.onGameSocketEvent then
			self._shotFrame:onGameSocketEvent(main,sub,dataBuffer)
		end
		-- 私人房
		if PriRoom then
			PriRoom:getInstance():getNetFrame():onGameSocketEvent(main,sub,dataBuffer)
		end
	end
end

function GameFrameEngine:onSocketLogonEvent(sub,dataBuffer)
	--登录完成
	if sub == game_cmd.SUB_GR_LOGON_FINISH then	
		self:onSocketLogonFinish()
	-- 登录成功
	elseif sub == game_cmd.SUB_GR_LOGON_SUCCESS then
		local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_LogonSuccess, dataBuffer)
		dump(cmd_table, "CMD_GR_LogonSuccess", 4)
	--登录失败
	elseif sub == game_cmd.SUB_GR_LOGON_FAILURE then	
		local errorCode = dataBuffer:readint()
		local msg = dataBuffer:readstring()
		print("登录房间失败:"..errorCode.."#"..msg)
		self:onCloseSocket()

		if (errorCode == yl.FAILURE_TYPE_ROOM_COIN_NOTENOUGH) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.RECHARGE_RIGHT_NOW)
		end
		
		if nil ~= self._callBack then
			self._callBack(-1,"温馨提示:" .. msg)
		end		
	--升级提示
	elseif sub == game_cmd.SUB_GR_UPDATE_NOTIFY then
		if nil ~= self._callBack then
			self._callBack(-1,"版本信息错误")
		end		
	end
end

function GameFrameEngine:startGame()
	if GlobalUserItem.isAntiCheat() then
		print("防作弊")
		assert(self:SitDown(yl.INVALID_TABLE,yl.INVALID_CHAIR), "sitdown failed the frame engine socket disconnect!")
		return
	end	
	
	local t,c = yl.INVALID_TABLE,yl.INVALID_CHAIR
	-- 找桌
	local bGet = false
	for k,v in pairs(self._tableStatus) do
		-- 未锁 未玩			
		if v.cbTableLock == 0 and v.cbPlayStatus == 0 then
			local st = k - 1
			local chaircount = self._wChairCount
			for i = 1, chaircount  do					
				local sc = i - 1
				if nil == self.getTableUserItem(st, sc) then
					t = st
					c = sc
					bGet = true
					break
				end
			end
		end

		if bGet then
			break
		end
	end
	print( " fast enter " .. t .. " ## " .. c)
	if (self:SitDown(t,c)) then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {canrepeat = false}, VIEW_LIST.GAME_WAIT_LAYER)
	end
end

--登录完成
function GameFrameEngine:onSocketLogonFinish()
	local myUserItem   =  self:GetMeUserItem()
	if not myUserItem and nil ~= self._callBack then
		self._callBack(-1,"获取自己信息失败！")
		return
	end
	if GlobalUserItem.bPrivateRoom and PriRoom then		
		PriRoom:getInstance():onLoginPriRoomFinish()
	else
		if self._wTableID ~= yl.INVALID_TABLE then
			self:LoadGameView(function ()
				self:SendGameOption()
			end)
		else
			--用户第一次进入，未坐桌 或 重连操作，上一局已结束，当前局未坐桌
			self:startGame()
		end
	end
end

--房间配置
function GameFrameEngine:onSocketConfigEvent(sub,dataBuffer)
	--房间配置
	if sub == yl.SUB_GR_CONFIG_SERVER then
		self._wTableCount  		= dataBuffer:readword()
		self._wChairCount  		= dataBuffer:readword()
		self._wServerType  		= dataBuffer:readword()
		self._dwServerRule 		= dataBuffer:readdword()
		GlobalUserItem.dwServerRule = self._dwServerRule

		--是否进入防作弊
		self:setEnterAntiCheatRoom(GlobalUserItem.isAntiCheat())
		print("房间配置[table:"..self._wTableCount.."][chair:"..self._wChairCount.."][type:"..self._wServerType.."][rule:"..self._dwServerRule.."]")
	--配置完成
	elseif sub == yl.SUB_GR_CONFIG_FINISH then

	end
end

function GameFrameEngine:GetTableCount()
	return self._wTableCount
end

function GameFrameEngine:GetChairCount()
	return self._wChairCount
end

function GameFrameEngine:GetServerType()
	return self._wServerType
end

function GameFrameEngine:GetServerRule()
	return self._dwServerRule
end

--房间取款准许
function GameFrameEngine:OnRoomAllowBankTake()
	return bit.band(self._dwServerRule,0x00010000) ~= 0
end

--房间存款准许
function GameFrameEngine:OnRoomAllowBankSave()
	return bit.band(self._dwServerRule,0x00040000) ~= 0
end

--游戏取款准许
function GameFrameEngine:OnGameAllowBankTake()
	return bit.band(self._dwServerRule,0x00020000) ~= 0
end

--游戏存款准许
function GameFrameEngine:OnGameAllowBankSave()
	return bit.band(self._dwServerRule,0x00080000) ~= 0
end

function GameFrameEngine:IsAllowAvertCheatMode( )
	return bit.band(self._dwServerRule, yl.SR_ALLOW_AVERT_CHEAT_MODE) ~= 0
end

--是否更新大厅金币
function GameFrameEngine:IsAllowPlazzScoreChange()
	return (self._wServerType ~= yl.GAME_GENRE_SCORE) and (self._wServerType ~= yl.GAME_GENRE_EDUCATE)
end

--游戏赠送准许
function GameFrameEngine:OnGameAllowBankTransfer()
	return false
end

--用户信息
function GameFrameEngine:onSocketUserEvent(sub,dataBuffer)
	--等待分配
	if sub == game_cmd.SUB_GR_USER_WAIT_DISTRIBUTE then
		--showToast(self._viewFrame, "正在进行分组,请稍后...", 3)
		print("正在进行分组,请稍后...")
	--用户进入
	elseif sub == yl.SUB_GR_USER_ENTER then
		self:onSocketUserEnter(dataBuffer)
	--用户积分
	elseif sub == yl.SUB_GR_USER_SCORE then
		self:onSocketUserScore(dataBuffer)
	--用户状态
	elseif sub == yl.SUB_GR_USER_STATUS then
		self:onSocketUserStatus(dataBuffer)
	--请求失败
	elseif sub == yl.SUB_GR_REQUEST_FAILURE then	
		self:onSocketReQuestFailure(dataBuffer)
	end
end
--用户进入
function GameFrameEngine:onSocketUserEnter(dataBuffer)
	local userItem = UserItem:create()

	userItem.dwGameID		= dataBuffer:readdword()
	userItem.dwUserID		= dataBuffer:readdword()

	--自己判断
	local bMySelfInfo = (userItem.dwUserID == GlobalUserItem.dwUserID)

	--非法过滤
	if not self._UserList[GlobalUserItem.dwUserID]  then
		if	bMySelfInfo == false then
			print("还未有自己信息，不处理其他用户信息")
			return
		end
	else 
		if bMySelfInfo == true then
			print("已有自己信息，不再次处理自己信息")
			return
		end
	end

	local int64 = Integer64.new()

	--读取信息
	userItem.wFaceID 		= dataBuffer:readword()
	userItem.dwCustomID		= dataBuffer:readdword()

	userItem.cbGender		= dataBuffer:readbyte()
	userItem.cbMemberOrder	= dataBuffer:readbyte()

	userItem.wTableID		= dataBuffer:readword()
	userItem.wChairID		= dataBuffer:readword()
	userItem.cbUserStatus 	= dataBuffer:readbyte()

--[[	userItem.lScore			= dataBuffer:readscore(int64):getvalue()
	userItem.lIngot			= dataBuffer:readscore(int64):getvalue()--]]
	
	userItem.lScore			= dataBuffer:readdouble()
	userItem.lIngot			= dataBuffer:readdouble()
	userItem.dBeans			= dataBuffer:readdouble()
	userItem.bAndroid 		= dataBuffer:readbool()
	
	userItem.dwWinCount		= dataBuffer:readdword()
	userItem.dwLostCount	= dataBuffer:readdword()
	userItem.dwDrawCount	= dataBuffer:readdword()
	userItem.dwFleeCount	= dataBuffer:readdword()
	userItem.dwExperience	= dataBuffer:readdword()
	--userItem.lIntegralCount = dataBuffer:readscore(int64):getvalue()
	userItem.lIntegralCount = dataBuffer:readdouble()
	userItem.dwAgentID		= dataBuffer:readdword()
	userItem.dwIpAddress 	= dataBuffer:readdword() -- ip地址	
	userItem.szChangeLogonIP = dataBuffer:readstring(yl.LEN_IP) --伪造IP归属地	
	
	--用户进入请求ip所在归属地
	if nil == userItem.dwIpAddress or 0 == userItem.dwIpAddress then
		userItem.dwIpAddress = ExternalFun.random_longip()
		local ipTable = ExternalFun.long2ip(userItem.dwIpAddress)
		local r1 = ipTable.b
		local r2 = ipTable.s
		local r3 = ipTable.m
		local r4 = ipTable.p
		if nil == r1 or nil == r2 or nil == r3 or nil == r4 then
			userItem.szIpAddress = ""
		else
			userItem.szIpAddress = r4 .. "." .. r3 .. "." .. r2 .. "." .. r1
		end
	else
		local ipTable = ExternalFun.long2ip(userItem.dwIpAddress)
		local r1 = ipTable.b
		local r2 = ipTable.s
		local r3 = ipTable.m
		local r4 = ipTable.p
		if nil == r1 or nil == r2 or nil == r3 or nil == r4 then
			userItem.szIpAddress = ""
		else
			userItem.szIpAddress = r1 .. "." .. r2.. "." .. r3 .. "." .. r4
		end
	end

	
	if (userItem.szChangeLogonIP and userItem.szChangeLogonIP ~= "") then
		--使用伪造的IP归属地
		userItem.szAdressLocation = userItem.szChangeLogonIP
	else
		appdf.onHttpJsionTable(string.format("http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip=%s", userItem.szIpAddress),"get","",function (datatable)
			--appdf.onHttpJsionTable("http://api.ip138.com/query/?ip="..userItem.szIpAddress.."&datatype=jsonp&token=99f2651ade06917e66d9f977e3c66460","get","",function (datatable)
			if type(datatable) == "table" then
				local code = datatable["ret"]
				if (code == 1) then
				--if (code == "ok") then
					userItem.szAdressLocation = string.format("%s %s %s", datatable.country, datatable.province, datatable.city)
					--userItem.szAdressLocation = string.format("%s %s %s %s", datatable.data[1], datatable.data[2], datatable.data[3], datatable.data[4])
				else
					userItem.szAdressLocation = "获取失败"
				end
			else--请求失败
				userItem.szAdressLocation = "未知区域"
			end
		end)
	end
	
	

	local curlen = dataBuffer:getcurlen()
	local datalen = dataBuffer:getlen()
	local tmpSize 
	local tmpCmd
	while curlen<datalen do
		tmpSize = dataBuffer:readword()
		tmpCmd = dataBuffer:readword()
		if not tmpSize or not tmpCmd then
		 	break
		end
		if tmpCmd == yl.DTP_GR_NICK_NAME then
			userItem.szNickName 	= dataBuffer:readstring(tmpSize/2)

			if not userItem.szNickName or (self:IsAllowAvertCheatMode() == true and userItem.dwUserID ~=  GlobalUserItem.dwUserID) then
				userItem.szNickName = "游戏玩家"
			end
		elseif tmpCmd == yl.DTP_GR_UNDER_WRITE then
			userItem.szSign = dataBuffer:readstring(tmpSize/2)
			if not userItem.szSign or (self:IsAllowAvertCheatMode() == true and userItem.dwUserID ~=  GlobalUserItem.dwUserID) then
				userItem.szSign = "此人很懒，没有签名"
			end
		elseif tmpCmd == 0 then
			break
		else
			for i = 1, tmpSize do
				if not dataBuffer:readbyte() then
					break
				end
			end
		end
		curlen = dataBuffer:getcurlen()
	end
	print("GameFrameEngine enter ==> ", userItem.szNickName, userItem.dwIpAddress, userItem.dwDistance)

	-- userItem:testlog()

	--添加/更新到缓存
	local bAdded
	local item = self._UserList[userItem.dwUserID] 
	if item ~= nil then
		item.dwGameID		= userItem.dwGameID
		item.lScore			= userItem.lScore	
		item.lIngot			= userItem.lIngot	
		item.dBeans			= userItem.dBeans	
		item.wFaceID 		= userItem.wFaceID
		item.dwCustomID		= userItem.dwCustomID
		item.cbGender		= userItem.cbGender
		item.cbMemberOrder	= userItem.cbMemberOrder
		item.wTableID		= userItem.wTableID
		item.wChairID		= userItem.wChairID
		item.cbUserStatus 	= userItem.cbUserStatus
		item.dwWinCount 	= userItem.dwWinCount
		item.dwLostCount 	= userItem.dwLostCount
		item.dwDrawCount 	= userItem.dwDrawCount
		item.dwFleeCount 	= userItem.dwFleeCount
		item.dwExperience 	= userItem.dwExperience
		item.szNickName     = userItem.szNickName
		bAdded = true
	end

	if not bAdded then
		self._UserList[userItem.dwUserID] = userItem
	end

	--记录自己桌椅号
	if userItem.dwUserID ==  GlobalUserItem.dwUserID then
		self._wTableID = userItem.wTableID
		self._wChairID = userItem.wChairID
	end

	if userItem.wTableID ~= yl.INVALID_TABLE  and userItem.cbUserStatus ~= yl.US_LOOKON then
		self:onUpDataTableUser(userItem.wTableID,userItem.wChairID,userItem)

		if self._viewFrame and self._viewFrame.onEventUserEnter then
			self._viewFrame:onEventUserEnter(userItem.wTableID,userItem.wChairID,userItem)
		end
	end

--[[	if bMySelfInfo == true and self._delayEnter == true then
		self._delayEnter = false
		self:onSocketLogonFinish()
	end--]]
end
--用户积分
function GameFrameEngine:onSocketUserScore(dataBuffer)
	
	local dwUserID = dataBuffer:readdword()

	local int64 = Integer64.new()
	local item = self._UserList[dwUserID]
	if  item ~= nil then
		--更新数据
		--item.lScore = dataBuffer:readscore(int64):getvalue()
		item.lScore = dataBuffer:readdouble()
		item.dBeans =  dataBuffer:readdouble()

		item.dwWinCount = dataBuffer:readdword()
		item.dwLostCount = dataBuffer:readdword()
		item.dwDrawCount = dataBuffer:readdword()
		item.dwFleeCount = dataBuffer:readdword()

		item.dwExperience = dataBuffer:readdword()

		print("更新用户["..dwUserID.."]["..item.szNickName.."]["..item.lScore.."]")
		
		--自己信息
		if item.dwUserID == GlobalUserItem.dwUserID and self:IsAllowPlazzScoreChange() then
			print("更新金币")
			GlobalUserItem.lUserScore = item.lScore
			GlobalUserItem.dUserBeans = item.dBeans
		end

		--通知更新界面
		if self._wTableID ~= yl.INVALID_TABLE and self._viewFrame and self._viewFrame.onEventUserScore  then
			self._viewFrame:onEventUserScore(item)
		end
	end  
end

function GameFrameEngine:LoadGameView(callback)
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	--查找是否游添加过游戏视图(应为视图是异步添加的，有可能还在添加队列里面)
	local findContext = contextProxy:findContextByName(VIEW_LIST.GAME_LAYER)
	if (findContext ~= nil) then
		local view = findContext:getView()
		assert(view ~= nil, "view maybe release, please check!")
		if (self._viewFrame ~= view) then
			self._viewFrame = view
		end
		if (callback) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GAME_WAIT_LAYER})	
			callback()
		end		
	else
		local entergame = GlobalUserItem.m_tabEnterGame
		if nil ~= entergame then
			local viewClassPath = appdf.GAME_SRC.. entergame._KindName .. "src.views.GameLayer"
			local function addViewAsyncCallBack(view, act)
				if (act == "enter") then
					self._viewFrame = view
					if (callback) then
						callback()
					end
					--关闭等待层
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GAME_WAIT_LAYER})
					--更新大厅游戏引用次数
					local gameListAccessProxy = AppFacade:getInstance():retrieveProxy("GameListAccessProxy")
					gameListAccessProxy:addRef(entergame._KindID)
					--更新大厅UI游戏列表
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_PLAZA_GAME_LIST)
				end
			end
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {viewClassPath = viewClassPath, ctor = {self}, viewcallback = addViewAsyncCallBack}, VIEW_LIST.GAME_LAYER)
		else
			assert(false, "游戏记录错误")
		end
	end
end

--用户状态
function GameFrameEngine:onSocketUserStatus(dataBuffer)

	--读取信息
	local dwUserID 		= dataBuffer:readdword()
	local newstatus = {}
	newstatus.wTableID   	= dataBuffer:readword()
	newstatus.wChairID   	= dataBuffer:readword()
	newstatus.cbUserStatus	= dataBuffer:readbyte()

--[[	--过滤观看
	if newstatus.cbUserStatus == yl.US_LOOKON then
		return
	end--]]

	--获取自己
	local myUserItem  =  self:GetMeUserItem()

	--未找到自己
	if not myUserItem then
		if newstatus.wTableID ~= yl.INVALID_TABLE then
			self._delayEnter = true
			self:QueryUserInfo(newstatus.wTableID,newstatus.wChairID)
			return
		end

		--非法信息
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"用户信息获取不正确,请重新登录！")
		end		
		return
	end	

	--自己判断
	local bMySelfInfo = (dwUserID == myUserItem.dwUserID)
	
	local useritem = self._UserList[dwUserID]

	--找不到用户
	if useritem == nil then
		--当前桌子用户
		if newstatus.wTableID ~= yl.INVALID_TABLE then
			--虚拟信息
			local newitem = UserItem:create()
			newitem.szNickName = "游戏玩家"
			newitem.dwUserID = dwUserID
			newitem.cbUserStatus = cbUserStatus
			newitem.wTableID = newstatus.wTableID
			newitem.wChairID = newstatus.wChairID

			--self._UserList[dwUserID] = newitem
			self:onUpDataTableUser(newitem.wTableID,newitem.wChairID,newitem)
			--发送查询
			self:QueryUserInfo(newstatus.wTableID,newstatus.wChairID)
		end
		return
	end

	-- 记录旧状态
	local oldstatus = {}
	oldstatus.wTableID = useritem.wTableID
	oldstatus.wChairID = useritem.wChairID
	oldstatus.cbUserStatus = useritem.cbUserStatus
	--更新信息
	useritem.cbUserStatus = newstatus.cbUserStatus
	useritem.wTableID = newstatus.wTableID
	useritem.wChairID = newstatus.wChairID

	--清除旧桌子椅子记录
	if oldstatus.wTableID ~= yl.INVALID_TABLE then
		--新旧桌子不同 新旧椅子不同
		if (oldstatus.wTableID ~= newstatus.wTableID) or (oldstatus.wChairID ~= newstatus.wChairID) then
			self:onUpDataTableUser(oldstatus.wTableID, oldstatus.wChairID, nil)
		end
	end
	--新桌子记录
	if newstatus.wTableID ~= yl.INVALID_TABLE then
		self:onUpDataTableUser(newstatus.wTableID, newstatus.wChairID, useritem)
	end

	--自己状态
	if  bMySelfInfo == true then

		self._wTableID = newstatus.wTableID
		self._wChairID = newstatus.wChairID
		--离开
		if newstatus.cbUserStatus == yl.US_NULL then
			print("自己离开")
			if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
				self._viewFrame:onExitRoom()
			end
		--起立
		elseif newstatus.cbUserStatus == yl.US_FREE and oldstatus.cbUserStatus > yl.US_FREE then
			print("自己起立")
			if self._viewFrame and self._viewFrame.onExitTable and not GlobalUserItem.bWaitQuit then
				if self.bEnterAntiCheatRoom then
					print("防作弊换桌")
					self:OnResetGameEngine()
				elseif not self.bChangeDesk then
					self._viewFrame:onExitTable()
				else
					self.bChangeDesk = false
					self:OnResetGameEngine()
				end
			end
		--坐下
		elseif newstatus.cbUserStatus >yl.US_FREE and oldstatus.cbUserStatus <yl.US_SIT then
			print("自己坐下")
			self.bChangeDesk = false
			
			self:LoadGameView(function ()
				self:SendGameOption()
			end)

			if self._viewFrame and self._viewFrame.onEventUserStatus then
			 	self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
								--旁观
		elseif newstatus.cbUserStatus == yl.US_LOOKON then
			print("自己坐下")
			self.bChangeDesk = false
			
			self:LoadGameView(function ()
				self:SendGameOption()
			end)
			
			if self._viewFrame and self._viewFrame.onEventUserStatus then
			 	self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		elseif newstatus.wTableID ~= yl.INVALID_TABLE and self.bChangeDesk == true then
			print("换位")
			
			self:LoadGameView(function ()
				self:SendGameOption()
			end)

			--showToast(self._viewFrame,"找到游戏桌子，正在获取场景中...",1)
			
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		else 
			print("自己新状态:"..newstatus.cbUserStatus)
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		end 
	--他人状态
	else
		--更新用户
		if oldstatus.wTableID ~= yl.INVALID_TABLE or newstatus.wTableID ~= yl.INVALID_TABLE then
			if self._viewFrame and self._viewFrame.onEventUserStatus then
				self._viewFrame:onEventUserStatus(useritem,newstatus,oldstatus)
			end
		end
		--删除用户
		if newstatus.cbUserStatus == yl.US_NULL then
			self:onRemoveUser(dwUserID)
		end
	end
end

--请求失败
function GameFrameEngine:onSocketReQuestFailure(dataBuffer)
	local cmdtable = ExternalFun.read_netdata(game_cmd.CMD_GR_RequestFailure, dataBuffer)

	if  self._viewFrame and self._viewFrame.onReQueryFailure then
		self._viewFrame:onReQueryFailure(cmdtable.lErrorCode,cmdtable.szDescribeString)
 	else
 		print("not viewframe or not onReQueryFailure")
	end
	
	if self.bChangeDesk == true  then
		self.bChangeDesk = false
		if  self._viewFrame and self._viewFrame.onExitTable and not GlobalUserItem.bWaitQuit then
			--self._viewFrame:onExitTable()
			self:SendGameOption()
		end
	end
	-- 清理锁表
	GlobalUserItem.dwLockServerID = 0
	GlobalUserItem.dwLockKindID = 0
end

--状态信息
function GameFrameEngine:onSocketStatusEvent(sub,dataBuffer)
	if sub == yl.SUB_GR_TABLE_INFO then
		print("SUB_GR_TABLE_INFO")
		local wTableCount = dataBuffer:readword()
		for i = 1, wTableCount do
			self._tableStatus[i] ={}
			self._tableStatus[i].cbTableLock = dataBuffer:readbyte()					
			self._tableStatus[i].cbPlayStatus = dataBuffer:readbyte()
			self._tableStatus[i].lCellScore = dataBuffer:readint()
		end
		if self._viewFrame and self._viewFrame.onGetTableInfo then
			self._viewFrame:onGetTableInfo()
		end
	elseif sub == yl.SUB_GR_TABLE_STATUS then	--桌子状态		
		local wTableID = dataBuffer:readword() + 1		
		self._tableStatus[wTableID] ={}
		self._tableStatus[wTableID].cbTableLock = dataBuffer:readbyte()					
		self._tableStatus[wTableID].cbPlayStatus = dataBuffer:readbyte()
		self._tableStatus[wTableID].lCellScore = dataBuffer:readint()

		print("SUB_GR_TABLE_STATUS ==> " .. wTableID--[[ .. " ==> " .. self._tableStatus[wTableID].cbPlayStatus]])
		if self._viewFrame and self._viewFrame.upDataTableStatus then
			self._viewFrame:upDataTableStatus(wTableID)
		end
	end
end
function GameFrameEngine:onSocketFrameSystem(sub,dataBuffer)
	local wType = dataBuffer:readword()
	local wLength = dataBuffer:readword()
	local szString = dataBuffer:readstring()
	print("系统消息#"..wType.."#"..szString)
	local bCloseRoom = bit.band(wType,yl.SMT_CLOSE_ROOM)
	local bCloseGame = bit.band(wType,yl.SMT_CLOSE_GAME)
	local bCloseLink = bit.band(wType,yl.SMT_CLOSE_LINK)
	
	if nil ~= self._callBack then
		self._callBack(0,szString)
	end		
end
--框架信息
function GameFrameEngine:onSocketFrameEvent(sub,dataBuffer)
	--游戏状态
	if sub == yl.SUB_GF_GAME_STATUS then
		self._cbGameStatus = dataBuffer:readword()
		self._cbAllowLookon = dataBuffer:readword()
	elseif sub == yl.SUB_GF_USER_DATA then
		local UserChairID = dataBuffer:readdword()
		local viewid = self._viewFrame:SwitchViewChairID(UserChairID)
		self._viewFrame._gameView:OnUpdateUserStatus(viewid)
	--游戏场景
	elseif sub == yl.SUB_GF_USER_DATA then
		local UserChairID = dataBuffer:readdword()
		local viewid = self._viewFrame:SwitchViewChairID(UserChairID)
		self._viewFrame._gameView:OnUpdateUserStatus(viewid)
	elseif sub == yl.SUB_GF_GAME_SCENE then
		if self._viewFrame and self._viewFrame.onEventGameScene then
			self._viewFrame:onEventGameScene(self._cbGameStatus,dataBuffer)
		else
			print("game scene did not respon")
			if not self._viewFrame then
				print ("viewframe is nil")
			else
				printf("onEventGameScene is ni viewframe is %d", self._viewFrame:getTag())
			end
		end
	--系统消息
	elseif sub == yl.SUB_GF_SYSTEM_MESSAGE then
		self:onSocketSystemMessage(dataBuffer)
	--动作消息
	elseif sub == yl.SUB_GF_ACTION_MESSAGE then
		self:onSocketActionMessage(dataBuffer)
	--用户聊天
	elseif sub == game_cmd.SUB_GF_USER_CHAT then
		local chat = ExternalFun.read_netdata(game_cmd.CMD_GF_S_UserChat, dataBuffer)
		--获取玩家昵称
		local useritem = self._UserList[chat.dwSendUserID]
		if not  useritem then
			return
		end
		if self._wTableID == yl.INVALID_CHAIR or self._wTableID ~= useritem.wTableID then
			return
		end

		chat.szNick = useritem.szNickName

		GameChatLayer.addChatRecordWith(chat)

		if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserChat then
			-- 播放声音
			local idx = GameChatLayer.compareWithText(chat.szChatString)
			if nil ~= idx then
				local sound_path = "sound/" .. useritem.cbGender .. "_" .. idx .. ".wav"
				if GlobalUserItem.bSoundAble then
					AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename(sound_path),false)
				end
			end
			self._viewFrame:onUserChat(chat,useritem.wChairID)
		end
	--用户表情
	elseif sub == game_cmd.SUB_GF_USER_EXPRESSION then
		local expression = ExternalFun.read_netdata(game_cmd.CMD_GF_S_UserExpression, dataBuffer)
		--获取玩家昵称
		local useritem = self._UserList[expression.dwSendUserID]

		if not  useritem then
			return
		end
		if self._wTableID == yl.INVALID_CHAIR or self._wTableID ~= useritem.wTableID then
			return
		end
		
		expression.szNick = useritem.szNickName

		GameChatLayer.addChatRecordWith(expression, true)
		if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserExpression then
			self._viewFrame:onUserExpression(expression,useritem.wChairID)
		end
	-- 用户语音
	elseif sub == game_cmd.SUB_GF_USER_VOICE then
		AudioRecorder:getInstance():saveRecordFile(dataBuffer, function(uid, tid, spath)
			local msgTab = {}
			msgTab.uid = uid
			msgTab.tid = tid
			msgTab.spath = spath
			table.insert(self._tabVoiceMsgQueue, msgTab)
			
			self:popVocieMsg()
        end)
	end
end

--系统消息
function GameFrameEngine:onSocketSystemMessage(dataBuffer)
	local wType = dataBuffer:readword()
	local wLength = dataBuffer:readword()
	local szString = dataBuffer:readstring()
	print("系统消息#"..wType.."#"..szString)
	local bCloseRoom = bit.band(wType,yl.SMT_CLOSE_ROOM)
	local bCloseGame = bit.band(wType,yl.SMT_CLOSE_GAME)
	local bCloseLink = bit.band(wType,yl.SMT_CLOSE_LINK)
	if self._viewFrame then
		--showToast(self._viewFrame,szString,2,cc.c3b(250,0,0))
	end
	if bCloseRoom ~= 0 or bCloseGame ~= 0 or bCloseLink ~=0 then
		self:setEnterAntiCheatRoom(false)
		if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
			self._viewFrame:onExitRoom()
		else
			self:onCloseSocket()
		end
	end
end

--系统动作
function GameFrameEngine:onSocketActionMessage(dataBuffer)
	local wType = dataBuffer:readword()
	local wLength = dataBuffer:readword()
	local nButtonType = dataBuffer:readint()
	local szString = dataBuffer:readstring()
	print("系统动作#"..wType.."#"..szString)
		
	local bCloseRoom = bit.band(wType,yl.SMT_CLOSE_ROOM)
	local bCloseGame = bit.band(wType,yl.SMT_CLOSE_GAME)
	local bCloseLink = bit.band(wType,yl.SMT_CLOSE_LINK)

	if self._viewFrame then
		--showToast(self._viewFrame,szString,2,cc.c3b(250,0,0))
	end
	if bCloseRoom ~= 0 or bCloseGame ~= 0 or bCloseLink ~=0 then
		self:setEnterAntiCheatRoom(false)
		if self._viewFrame and self._viewFrame.onExitRoom and not GlobalUserItem.bWaitQuit then
			self._viewFrame:onExitRoom()
		else
			self:onCloseSocket()
		end
	end
end


--更新桌椅用户
function GameFrameEngine:onUpDataTableUser(tableid,chairid,useritem)
	local id = tableid + 1
	local idex = chairid + 1
	if not self._tableUserList[id]  then
		self._tableUserList[id] = {}
	end
	if useritem then
		self._tableUserList[id][idex] = useritem.dwUserID
	else
		self._tableUserList[id][idex] = nil
	end
end

--获取桌子用户
function GameFrameEngine:getTableUserItem(tableid,chairid)
	local id = tableid + 1
	local idex = chairid + 1
	if self._tableUserList[id]  then
		local userid = self._tableUserList[id][idex] 
		if userid then
			return self._UserList[userid]
		end
	end
end

function GameFrameEngine:getTableInfo(index)
	if index > 0  then
		return self._tableStatus[index]
	end
end

--获取自己游戏信息
function GameFrameEngine:GetMeUserItem()
	return self._UserList[GlobalUserItem.dwUserID]
end

--获取游戏状态
function GameFrameEngine:GetGameStatus()
	return self._cbGameStatus
end

--设置游戏状态
function GameFrameEngine:SetGameStatus(cbGameStatus)
	self._cbGameStatus = cbGameStatus
end

--获取桌子ID
function GameFrameEngine:GetTableID()
	return self._wTableID
end

--获取椅子ID
function GameFrameEngine:GetChairID()
	return self._wChairID
end

--移除用户
function GameFrameEngine:onRemoveUser(dwUserID)
	self._UserList[dwUserID] = nil
end

--坐下请求
function GameFrameEngine:SitDown(table ,chair,password)
	local dataBuffer = CCmd_Data:create(70)
	dataBuffer:setcmdinfo(yl.MDM_GR_USER,yl.SUB_GR_USER_SITDOWN)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	self._reqTable = table
	self._reqChair = chair
	if password then
		dataBuffer:pushstring(password,yl.LEN_PASSWORD)
	end

	--记录坐下信息
	if nil ~= GlobalUserItem.m_tabEnterGame and type(GlobalUserItem.m_tabEnterGame) == "table" then
		print("update game info")
		GlobalUserItem.m_tabEnterGame.nSitTable = table
		GlobalUserItem.m_tabEnterGame.nSitChair = chair
	end
	return self:sendSocketData(dataBuffer)
end

--查询用户
function GameFrameEngine:QueryUserInfo(table ,chair)
	local dataBuffer = CCmd_Data:create(4)
	dataBuffer:setcmdinfo(yl.MDM_GR_USER,yl.SUB_GR_USER_CHAIR_INFO_REQ)
	dataBuffer:pushword(table)
	dataBuffer:pushword(chair)
	return self:sendSocketData(dataBuffer)
end

--换位请求
function GameFrameEngine:QueryChangeDesk()
	self.bChangeDesk = true
	local dataBuffer = CCmd_Data:create(0)
	dataBuffer:setcmdinfo(yl.MDM_GR_USER,yl.SUB_GR_USER_CHAIR_REQ)
	return self:sendSocketData(dataBuffer)
end

--起立请求
function GameFrameEngine:StandUp(bForce)
	local dataBuffer = CCmd_Data:create(5)
	dataBuffer:setcmdinfo(yl.MDM_GR_USER,yl.SUB_GR_USER_STANDUP)
	dataBuffer:pushword(self:GetTableID())
	dataBuffer:pushword(self:GetChairID())
	dataBuffer:pushbyte(not bForce and 0 or 1)
	return self:sendSocketData(dataBuffer)
end

--发送准备
function GameFrameEngine:SendUserReady(dataBuffer)
	local userReady = dataBuffer
	if not userReady then
		userReady = CCmd_Data:create(0)
	end
	userReady:setcmdinfo(yl.MDM_GF_FRAME,yl.SUB_GF_USER_READY)
	return self:sendSocketData(userReady)
end

--场景规则
function GameFrameEngine:SendGameOption()
	local dataBuffer = CCmd_Data:create(9)
	dataBuffer:setcmdinfo(yl.MDM_GF_FRAME,yl.SUB_GF_GAME_OPTION)
	dataBuffer:pushbyte(0)
	dataBuffer:pushdword(appdf.VersionValue(6,7,0,1))
	dataBuffer:pushdword(self._kindVersion)
	return self:sendSocketData(dataBuffer)
end

--加密桌子
function GameFrameEngine:SendEncrypt(pass)
	local passlen = string.len(pass) * 2 --14--(ExternalFun.stringLen(pass)) * 2
	print("passlen ==> " .. passlen)
	local len = passlen + 4 + 13--(sizeof game_cmd.CMD_GR_UserRule)
	print("len ==> " .. len)
	local cmddata = CCmd_Data:create(len)
	cmddata:setcmdinfo(game_cmd.MDM_GR_USER, game_cmd.SUB_GR_USER_RULE)
	cmddata:pushbyte(0)
	cmddata:pushword(0)
	cmddata:pushword(0)
	cmddata:pushint(0)
	cmddata:pushint(0)
	cmddata:pushword(passlen)
	cmddata:pushword(game_cmd.DTP_GR_TABLE_PASSWORD)	
	cmddata:pushstring(pass, passlen / 2)

	return self:sendSocketData(cmddata)
end

--发送文本聊天 game_cmd.CMD_GF_C_UserChat
--[msg] 聊天内容
--[tagetUser] 目标用户
function GameFrameEngine:sendTextChat( msg, tagetUser , color)
	if type(msg) ~= "string" then
		print("聊天内容异常")
		return false, "聊天内容异常!"
	end
	--敏感词判断
	if true == ExternalFun.isContainBadWords(msg) then
		print("聊天内容包含敏感词汇")
		return false, "聊天内容包含敏感词汇!"
	end
	msg = msg .. "\0"

	tagetUser = tagetUser or yl.INVALID_USERID
	color = color or 16777215 --appdf.ValueToColor( 255,255,255 )
	local msgLen = string.len(msg)
	local defineLen = yl.LEN_USER_CHAT * 2

	local cmddata = CCmd_Data:create(266 - defineLen + msgLen * 2)
	cmddata:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_CHAT)
	cmddata:pushword(msgLen)
	cmddata:pushdword(color)
	cmddata:pushdword(tagetUser)
	cmddata:pushstring(msg, msgLen)

	return self:sendSocketData(cmddata)
end

--发送表情聊天 game_cmd.CMD_GF_C_UserExpressio
--[idx] 表情图片索引
--[tagetUser] 目标用户
function GameFrameEngine:sendBrowChat( idx, tagetUser )
	tagetUser = tagetUser or yl.INVALID_USERID

	local cmddata = CCmd_Data:create(6)
	cmddata:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_EXPRESSION)
	cmddata:pushword(idx)
	cmddata:pushdword(tagetUser)

	return self:sendSocketData(cmddata)
end

function GameFrameEngine:OnResetGameEngine()
	if self._viewFrame and self._viewFrame.OnResetGameEngine then
		self._viewFrame:OnResetGameEngine()
	end
end

function GameFrameEngine:popVocieMsg()
	if self._bPlayVoiceRecord then
		return
	end

	local msgTab = self._tabVoiceMsgQueue[1]
	if type(msgTab) == "table" then
		local uid = msgTab.uid
		local spath = msgTab.spath

		--获取玩家
		local useritem = self._UserList[uid]
		if nil ~= useritem then
			-- 录音开始
			if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserVoiceStart then
				self._viewFrame:onUserVoiceStart(useritem, spath)
			end

			self._nPlayVoiceId = VoiceRecorderKit.startPlayVoice(spath)
        	AudioRecorder:getInstance():setFinishCallBack(self._nPlayVoiceId, function(voiceid, filename)
                print("play over " .. filename)
                VoiceRecorderKit.finishPlayVoice()                
                self._bPlayVoiceRecord = false
                self._nPlayVoiceId = nil

                -- 录音结束
                if nil ~= self._viewFrame and nil ~= self._viewFrame.onUserVoiceEnded then
                	self._viewFrame:onUserVoiceEnded(useritem, spath)
                end    
                self:setPlayingVoice(false)     
            end)
			self._bPlayVoiceRecord = true
		end
	end
	table.remove(self._tabVoiceMsgQueue, 1)
end

function GameFrameEngine:setPlayingVoice( bPlaying )
	self._bPlayVoiceRecord = bPlaying
	if false == bPlaying then
		self:popVocieMsg()
	end 
end

function GameFrameEngine:clearVoiceQueue()
	self._tabVoiceMsgQueue = {}
	self._bPlayVoiceRecord = false
	if nil ~= self._nPlayVoiceId then
		ccexp.AudioEngine:stop(self._nPlayVoiceId)
        VoiceRecorderKit.finishPlayVoice()
        self._nPlayVoiceId = nil
	end
end

return GameFrameEngine