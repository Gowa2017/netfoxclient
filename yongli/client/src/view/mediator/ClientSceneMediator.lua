local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClientSceneMediator = class("ClientSceneMediator", BaseMediator)
local ClientUpdate = appdf.req(appdf.CLIENT_SRC.."app.controllers.ClientUpdate")

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")	

function ClientSceneMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
				GAME_COMMAMD.UPDATE_USER_SCORE,				--订阅更新用户金币消息
				GAME_COMMAMD.UPDATE_INSURE_SCORE,			--订阅更新银行金币消息
				GAME_COMMAMD.UPDATE_USER_FACE,				--订阅更新用户头像消息
				GAME_COMMAMD.UPDATE_PLAZA_GAME_LIST,		--订阅更新大厅游戏列表顺序消息
				GAME_COMMAMD.HALL_GONGGAO_RESULT,			--订阅大厅公告内容（固定公告）
			}
end

function ClientSceneMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
	--通知Mediator绑定视图 更新视图状态
	if (msgName == GAME_COMMAMD.UPDATE_USER_SCORE) then
		self.viewComponent:emit("M2V_Update_UserScore", GlobalUserItem.lUserScore)
	elseif (msgName == GAME_COMMAMD.UPDATE_INSURE_SCORE) then
		self.viewComponent:emit("M2V_Update_InsureScore", GlobalUserItem.lUserInsure)
	elseif (msgName == GAME_COMMAMD.UPDATE_USER_FACE) then
		self.viewComponent:emit("M2V_Update_UserFace", GlobalUserItem)
	elseif (msgName == GAME_COMMAMD.UPDATE_PLAZA_GAME_LIST) then
		self.viewComponent:emit("M2V_Update_Plaza_Game_List")
	elseif (msgName == GAME_COMMAMD.HALL_GONGGAO_RESULT) then
		self.viewComponent:emit("M2V_Update_GongGao_Message", msgData, msgType)
	end
    --deal something
end

function ClientSceneMediator:onRegister()
    self.super.onRegister(self)
	--注册UI监听事件
	self.viewComponent:attach("V2M_CheckGameUpdate", handler(self, self.checkGameUpdate))
	--正在下载的游戏列表
	self._gameDownloading = {}
end

function ClientSceneMediator:checkGameUpdate(e, kinid)
	if GlobalUserItem.isAngentAccount() then
		self.viewComponent:emit("M2V_ClientSceneToast", "代理账号无法游戏！")
		return
	end
	if kinid == 410 then
	    self.viewComponent:emit("M2V_ClientSceneToast", "如果遇到登录失败的情况，请重新登录游戏！")
		return
	end
	for _,v in ipairs(self._gameDownloading) do
		if (tonumber(v._KindID) == kinid) then
			self.viewComponent:emit("M2V_ClientSceneToast", "游戏正在排队下载中，稍等片刻哦！")
			return
		end
	end

	local gameinfo = nil
	for i = 1, #appConfigProxy._gameList do
		local subItem = appConfigProxy._gameList[i]
		if (kinid == tonumber(subItem._KindID)) then
			gameinfo = subItem
			break
		end
	end
	
	if (not gameinfo) then
		self.viewComponent:emit("M2V_ClientSceneToast", "未找到游戏信息，请稍后再试！")
		return
	end
	
	--下载/更新资源 clientscene:getApp
	local version = versionProxy:getResVersion(gameinfo._KindID)
	
	if version then
		if gameinfo._ServerResVersion > version then
			self.viewComponent:emit("M2V_OnStartGameUpdate", gameinfo)
			if (#self._gameDownloading == 0) then
				table.insert(self._gameDownloading, gameinfo)
				self:updateGame(gameinfo)
			else
				table.insert(self._gameDownloading, gameinfo)
			end
		else
			--self.viewComponent:emit("M2V_OnStartGameUpdate", gameinfo, sender)
			self.viewComponent:emit("M2V_OnEnterGame", gameinfo)
			self:onEnterGame(gameinfo, false)
		end
	else
		if (appConfigProxy._appStoreSwitch == 0) then
			--更新参数
			local targetPlatform = cc.Application:getInstance():getTargetPlatform()			
			if cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
				self.viewComponent:emit("M2V_OnStartGameUpdate", gameinfo)			
				if (#self._gameDownloading == 0) then
					table.insert(self._gameDownloading, gameinfo)
					self:updateGame(gameinfo)
				else
					table.insert(self._gameDownloading, gameinfo)
				end
			else
				self:onEnterGame(gameinfo, false)
			end		
		else
			self:onEnterGame(gameinfo, false)
		end
	end
end

function ClientSceneMediator:updateGame(gameinfo)
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	
	--更新参数
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	
	local version = versionProxy:getResVersion(gameinfo._KindID)

	if (version ~= nil) then 
		--版本号非空 使用热更新
		local newfileurl = appConfigProxy._updateUrl.."/game/"..gameinfo._Module.."res/filemd5List.json"
		local dst = device.writablePath .. "game/" .. gameinfo._Type .. "/"
		
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			dst = device.writablePath .. "download/game/" .. gameinfo._Type .. "/"
		end
		
		local src = device.writablePath.."game/"..gameinfo._Module.."res/filemd5List.json"
		local downurl = appConfigProxy._updateUrl .. "/game/" .. gameinfo._Type .. "/"
		--创建热更新
		local clientUpdate = ClientUpdate:create(newfileurl,dst,src,downurl)
		clientUpdate:upDateClient(function (sub, msg, mainpersent)
									self:updateProgress(sub, msg, mainpersent, gameinfo) 
								  end,
								
								  function (result,msg)
									self:updateResult(result,msg,gameinfo) 
								  end)
		
		--清空游戏热更记录（防止重载大厅Lua)
		local hotFixUpdateProxy = AppFacade:getInstance():retrieveProxy("HotFixUpdateProxy")
		local updateFiles = hotFixUpdateProxy:setUpdateFiles(nil)				
	else
		--版本号空 使用包更新
		--调用C++下载
		local url = appConfigProxy._updateUrl.."/game/"..gameinfo._Module .. "game.zip"
		local saveurl = device.writablePath .. "game/"..gameinfo._Module
		
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			dst = device.writablePath .. "download/game/"..gameinfo._Module
		end
	
		downFileAsync(url, saveurl, nil, function(code, parm)
			--下载回调
			if code == appdf.DOWN_PRO_INFO then --进度信息
				self.viewComponent:emit("M2V_OnGameUpdateProgress", tonumber(parm), gameinfo)
				--self:updateProgress(100,"", tonumber(parm))
			elseif code == appdf.DOWN_COMPELETED then --下载完毕
				unZipAsync(saveurl .. "game.zip",saveurl,function(code, parm)
					if (code == appdf.UNZIP_COMPELETED) then
						self:updateResult(true,"", gameinfo)
					else
						--忽略解压错误
						self:updateResult(false,"解压失败！", gameinfo)
						print("unzip error")
					end
				end)	
			else
				self:updateResult(false,"下载失败！", gameinfo)
				print("down error")
			end
		end, 5)	
	end
end

--更新进度
function ClientSceneMediator:updateProgress(sub, msg, mainpersent, gameinfo)
	if (mainpersent ~= -1) then
		self.viewComponent:emit("M2V_OnGameUpdateProgress", mainpersent, gameinfo)
	end
end

--更新结果
function ClientSceneMediator:updateResult(result,msg, gameinfo)
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	
	--从正在下载队列中移除
	for i=1,#self._gameDownloading do
		if (self._gameDownloading[i]._KindID == gameinfo._KindID) then
			table.remove(self._gameDownloading, i)
			break
		end
	end
	
	if result == true then
		--local app = self:getRootNode():getApp()
		self.viewComponent:emit("M2V_OnGameUpdateFinished", 0, gameinfo)
		
		--更新版本号
		for k,v in pairs(appConfigProxy._gameList) do
			if v._KindID == gameinfo._KindID then
				versionProxy:setResVersion(v._ServerResVersion, v._KindID)
				v._Active = true
				break
			end
		end
		
		--继续下一个游戏下载
		if (#self._gameDownloading ~= 0) then
			self:updateGame(self._gameDownloading[1])
		end
		--self:onEnterGame(gameinfo, true)
	else
		self.viewComponent:emit("M2V_OnGameUpdateFinished", -1, gameinfo)
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {msg.."\n是否重试？",function(bReTry)
			if bReTry == true then
				self:updateGame(gameinfo)
			else
				--继续下一个游戏下载
				if (#self._gameDownloading ~= 0) then
					self:updateGame(self._gameDownloading[1])
				end
			end
		end}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
	end
end

function ClientSceneMediator:onEnterGame(gameinfo, isUpdate)
		--判断房间获取
	local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
	if not roomCount or 0 == roomCount then
		--self:onLoadGameList(gameinfo._KindID)
		self.viewComponent:emit("M2V_ClientSceneToast", "游戏维护中，请稍后再试！")
		return
	end
	
	GlobalUserItem.m_tabEnterGame = gameinfo
	
	GlobalUserItem.setCurGameKind(tonumber(gameinfo._KindID))
	
--[[	--更新大厅显示的界面KindID
	local customEvent = cc.EventCustom:new(yl.RY_SHOW_VIEW_GAME_KIND)
	customEvent.obj = tonumber(gameinfo._KindID)
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)--]]
	
	GlobalUserItem.szCurGameName = gameinfo._KindName
	--MXM这里改为直接供普通金币房使用了
	--if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then
		--print(" GameListLayer enter priGame ")
	--else
	-- 处理锁表
	--local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
	--if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
	--	GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
	--	self:onStartGame()
	--else
	
	--检查是否为百人场 类型游戏，是则直接进入游戏房间
	local kindlist = GlobalUserItem.kindlist
	
	for _, v in ipairs(kindlist) do
		if tonumber(gameinfo._KindID) == v.wKindID then
			if (bit.band(v.dwSuportType, yl.KIND_SUPPORT_BAIREN) ~= 0) then

				GlobalUserItem.setCurGameKind(gameinfo._KindID)
				
				local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
				local findContext = contextProxy:findContextByName(VIEW_LIST.CLIENT_SCENE)
				
				if (findContext == nil) then
					assert(false, "can't find ClientScene!")
				end
				
				local viewCompoment = findContext:getView()
				
				GlobalUserItem.nCurRoomIndex = 1
				GlobalUserItem.bPrivateRoom = (gameinfo.wServerType == yl.GAME_GENRE_PERSONAL)
				if viewCompoment:roomEnterCheck() then
					viewCompoment:onStartGame()
				end
				return
			end
		end
	end
	
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {isUpdate}}, VIEW_LIST.ROOM_LIST_LAYER)
		--self:getRootNode():onChangeShowMode(yl.SCENE_ROOMLIST, self.m_bQuickStart)
	--end
end

function ClientSceneMediator:onStartGame()
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	local findContext = contextProxy:findContextByName(VIEW_LIST.CLIENT_SCENE)
	
	if (findContext == nil) then
		assert(false, "can't find ClientScene!")
	end
	
	local viewCompoment = findContext:getView()
	
	if viewCompoment:roomEnterCheck() then
		viewCompoment:onStartGame()
	end		
end

return ClientSceneMediator