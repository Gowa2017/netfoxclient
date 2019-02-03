--[[
	约牌游戏列表界面
	2017_09_21 MXM
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameServerItem   = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameServerItem")

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")	

local ManageGameListLayer = class("ManageGameListLayer", function(gamelist)
		local gamelist_layer =  display.newLayer()
    return gamelist_layer
end)

local Update = appdf.req(appdf.BASE_SRC.."app.controllers.ClientUpdate")
local QueryDialog = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.QueryDialog")
local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")

--ManageGameListLayer.GAME_KIND_ZHAJINHUA = 6

-- 进入场景而且过渡动画结束时候触发。
function ManageGameListLayer:onEnterTransitionFinish()
	self._listView:reloadData()
	--发送消息
	self:runAction(cc.Sequence:create(cc.DelayTime:create(0.1), cc.CallFunc:create(function()
		local customEvent = cc.EventCustom:new(yl.RY_SELECT_PRIVATELIST_NOTIFY)
		customEvent.kind = GlobalUserItem.nCurGameKind
		--customEvent.cell = cell
		cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	
	end)))

    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ManageGameListLayer:onExitTransitionStart()
    return self
end

function ManageGameListLayer:ctor(scene, gamelist)
	print("============= 游戏列表界面创建 =============")
	self.m_bQuickStart = false
	self._scene = scene

	local this = self
	--重置游戏类型
	--GlobalUserItem.setCurGameKind(0)

	self:setContentSize(yl.WIDTH,yl.HEIGHT)

	self._gameList = gamelist

    local logonCallBack = function (result,message)
		this:onLogonCallBack(result,message)
	end

	self._logonFrame = LogonFrame:create(self,logonCallBack)
	

	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			this:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			this:onExitTransitionStart()
		elseif eventType == "exit" then
			if self._logonFrame:isSocketServer() then
				self._logonFrame:onCloseSocket()
			end
		end
	end)

	--游戏列表
	self._listView = cc.TableView:create(cc.size(295, 665)) 
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)--垂直滚动      
	self._listView:setPosition(cc.p(6,40))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
	self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	self._listView:registerScriptHandler(handler(self, self.cellHightLight), cc.TABLECELL_HIGH_LIGHT)
	self._listView:registerScriptHandler(handler(self, self.cellUnHightLight), cc.TABLECELL_UNHIGH_LIGHT)

	self._txtTips = ccui.Text:create("", "base/fonts/round_body.ttf", 32)
         	:setAnchorPoint(cc.p(1,0))
         	:move(yl.WIDTH,110)
			:setVisible(false)
         	:setTextColor(cc.c4b(0,250,0,255))
         	:addTo(self)

    self.m_fThird = yl.WIDTH / 4

    --下载提示
    self.m_spDownloadMask = nil
    self.m_szMaskSize = cc.size(0,0)
    self.m_labDownloadTip = nil
    self.m_spDownloadCycle = nil
    self.m_bGameUpdate = false
	self.m_selectgame = nil
	self.m_bselgame = false
	
	--房卡游戏列表
	self.myfanka = {}
	for _, v in ipairs(appConfigProxy._gameList) do
		if v._KindType == 1 or v._KindType == 3 then
			table.insert(self.myfanka,1,v)
		end
	end
		
end

--获取父场景节点(ClientScene)
function ManageGameListLayer:getRootNode( )
	return self._scene
end

--更新游戏进入记录
function ManageGameListLayer:updateEnterGameInfo(info)
	self:getRootNode():updateEnterGameInfo(info)
end

function ManageGameListLayer:onEnterGame( gameinfo, isQuickStart)
	
	self:updateEnterGameInfo(gameinfo)
	self.m_bQuickStart = isQuickStart

	--判断房间获取
	local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
	if not roomCount or 0 == roomCount then
		--self:onLoadGameList(gameinfo._KindID)
		print("ManageGameListLayer 房间列表为空")		
	end
	GlobalUserItem.setCurGameKind(tonumber(gameinfo._KindID))
	self._scene:loadPriGameList(tonumber(gameinfo._KindID))
	GlobalUserItem.szCurGameName = gameinfo._KindName
	if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then

		print(" ManageGameListLayer enter priGame ")
	--[[else
		-- 处理锁表
		local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
		if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
			GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
			self._scene:onStartGame()
		else
			self._scene:onChangeShowMode(yl.SCENE_ROOMLIST, self.m_bQuickStart)
		end--]]
	end
	
end

---------------------------------------------------------------------
-- listview 相关
function ManageGameListLayer:cellHightLight(view,cell)
	
end

function ManageGameListLayer:cellUnHightLight(view,cell)
	
end

--子视图大小
function ManageGameListLayer:cellSizeForTable(view, idx)
  	return view:getParent().m_fThird , 135
end

--子视图数目
function ManageGameListLayer:numberOfCellsInTableView(view)
	return table.nums(self.myfanka)
	--[[if not view:getParent()._gameList then
		return 0
	else
  		return #view:getParent()._gameList
  	end--]]
end

--子视图点击
function ManageGameListLayer:tableCellTouched(view, cell)
	if GlobalUserItem.isAngentAccount() then
		return
	end
		
	local index = cell:getIdx() 
	local gamelistLayer = view:getParent()

	--获取游戏信息
	--local gameinfo = gamelistLayer._gameList[index+1]
	local gameinfo = self.myfanka[index+1]
	if  not gameinfo then
		showToast(self._scene,"未找到游戏信息！",2)
		return
	end
	gameinfo.gameIndex = index
	
	--下载/更新资源 clientscene:getApp
	--local app = gamelistLayer:getParent():getParent():getApp()
		local version = versionProxy:getResVersion(gameinfo._KindID)
		if not version or gameinfo._ServerResVersion > version then
			gamelistLayer:updateGame(gameinfo, index)
		else
			gamelistLayer:onEnterGame(gameinfo, false)
		end
	--end
	
	--发送消息
	local customEvent = cc.EventCustom:new(yl.RY_SELECT_PRIVATELIST_NOTIFY)
	customEvent.kind = GlobalUserItem.nCurGameKind
	customEvent.cell = cell
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	
	--self._scene.m_actStartTargetGameAni:play(string.format("%d", GlobalUserItem.nCurGameKind),true)
end
	
--获取子视图
function ManageGameListLayer:tableCellAtIndex(view, idx)	
	local cell = view:dequeueCell()
	
	local gameinfo = self.myfanka[idx+1]
	gameinfo.gameIndex = idx
	
	
	local filestr = "PriGameList/game_"..gameinfo._KindID..".png"
	if false == cc.FileUtils:getInstance():isFileExist(filestr) then
		filestr = "PriGameList/default.png"
	end
	local game = nil
	local mask = nil
	local spTip = nil
	local cellpos = cc.p(view:getParent().m_fThird * 0.5,view:getViewSize().height * 0.5)
			
	if not cell then
		cell = cc.TableViewCell:new()
		game = display.newSprite(filestr)
		game:addTo(cell)
			:setAnchorPoint(cc.p(0.5, 0))
			:setPosition(view:getParent().m_fThird * 0.5, 0)
			:setTag(1)
			
		local maskSp = cc.Sprite:create(filestr)
		local pos = cc.p(0,0)
		if nil ~= maskSp then			
			maskSp:setColor(cc.BLACK)
			maskSp:setOpacity(100)
			local size = maskSp:getContentSize()
			--maskSp:setAnchorPoint(cc.p(0, 0))
			maskSp:setPosition(cc.p(size.width * 0.5,size.height * 0.5))
			maskSp:setName("download_mask_sp")			

			mask = ccui.Layout:create()
			mask:setClippingEnabled(true)
			mask:setAnchorPoint(cc.p(0.5,0))
			mask:setPosition(cc.p(view:getParent().m_fThird * 0.5, 0))
			mask:setContentSize(size)
			mask:addChild(maskSp)
			cell:addChild(mask)
			mask:setName("download_mask")

			spTip = cc.Label:createWithTTF("", "base/fonts/round_body.ttf", 32)
				:enableOutline(cc.c4b(0,0,0,255), 1)
				:move(cellpos)
				:setName("download_mask_tip")
				:addTo(cell)

			local cycle = cc.Sprite:create("PriGameList/spinner_circle.png")
			if nil ~= cycle then
				cycle:setPosition(cellpos)
				cycle:setVisible(false)
				cycle:setScale(1.3)
				cycle:setName("download_cycle")
				cell:addChild(cycle)
			end	
			
			local selectgame = cc.Sprite:create("PriGameList/game_"..gameinfo._KindID.."_1.png")
			if nil ~= selectgame then
				selectgame:setAnchorPoint(cc.p(0.5, 0))
				selectgame:setPosition(view:getParent().m_fThird * 0.5, 0)
				selectgame:setVisible(false)
				selectgame:setName("selectgame")
				cell:addChild(selectgame)
			end	
			
			
	
		end	
	else
		game = cell:getChildByTag(1)
		game:setTexture(filestr)

		mask = cell:getChildByName("download_mask")
		if nil ~= mask then
			local sp = mask:getChildByName("download_mask_sp")
			if nil ~= sp then
				local size = sp:getContentSize()
				sp:setTexture(filestr)
				sp:setPosition(cc.p(size.width * 0.5,size.height * 0.5))
				mask:setContentSize(size)
			end

			spTip = mask:getChildByName("download_mask_tip")
			if nil ~= spTip then
				local size = mask:getContentSize()
				spTip:setPosition(cellpos)
			end
		end
	end	

	if nil ~= mask then
		mask:setVisible(not gameinfo._Active)
	end

	if nil ~= spTip then
		spTip:setString("")
	end
	cell:setVisible(true)
	cell:setTag(gameinfo._KindID)
	
	
	--[[local prikindid = cc.UserDefault:getInstance():getIntegerForKey("prikindid")
	cell:setTag(prikindid)
	
	if GlobalUserItem.nCurGameKind == prikindid then
		self.m_selectgame = cell:getChildByName("selectgame")
		self.m_selectgame:setVisible(true)
	end--]]
	--游戏列表动画效果-MXM
	--[[local filestrcsb = "GameList/act_game_"..gameinfo._KindID..".csb"
	if false == cc.FileUtils:getInstance():isFileExist(filestrcsb) then
		filestrcsb = "GameList/default.png"
	else
		game.m_actGame = ExternalFun.loadTimeLine(filestrcsb)
		local gamekindact = ExternalFun.loadCSB(filestrcsb, game)
			  --:setAnchorPoint(cc.p(0.5,0))
			  --:setPosition(cellpos)
			  :setPosition(cc.p(view:getParent().m_fThird * 0.395,view:getViewSize().height * 0.495))
		game.m_gameKind = gamekindact

		ExternalFun.SAFE_RETAIN(game.m_actGame)
		game.m_gameKind:stopAllActions()
		game.m_actGame:gotoFrameAndPlay(0,true)
		game.m_gameKind:runAction(game.m_actGame)
	end--]]
	 --动画结束--------------
	
	--监听消息
	local listener = cc.EventListenerCustom:create(yl.RY_SELECT_PRIVATELIST_NOTIFY,function (event)
		local kindID = event.kind
			if (event.cell ~= nil) then
				if (event.cell ~= cell) then
					self.m_selectgame = cell:getChildByName("selectgame")
					self.m_selectgame:setVisible(false)
				else
					self.m_selectgame = cell:getChildByName("selectgame")
					self.m_selectgame:setVisible(true)

				end
			elseif (kindID ~= nil) then
				if (kindID ~= cell:getTag()) then
						self.m_selectgame = cell:getChildByName("selectgame")
						self.m_selectgame:setVisible(false)
				else
					self.m_selectgame = cell:getChildByName("selectgame")
					self.m_selectgame:setVisible(true)
					if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then
						
					end
				end
			else
				assert(false, "")
			end
		end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, cell)--]]
	
	return cell
		
end
---------------------------------------------------------------------

--链接游戏
function ManageGameListLayer:onLoadGameList(nKindID)
	if not nKindID then
		self:dismissPopWait()
		local ru = cc.Director:getInstance():getRunningScene()
		if nil ~= ru then
			showToast(ru,"游戏ID有误！",1)
		end	
		return
	end
	GlobalUserItem.setCurGameKind(tonumber(nKindID))
	--如果是有游客
	if GlobalUserItem.bVisitor then		
		if self._logonFrame:onLogonByVisitor() then
			self:showPopWait()
		end
	--如果是第三方
	elseif GlobalUserItem.bThirdPartyLogin then
		local td = GlobalUserItem.thirdPartyData
		--szAccount, szNick, cbgender, platform
		if self._logonFrame:onLoginByThirdParty(td.szAccount,td.szNick, td.cbGender, td.platform) then
			self:showPopWait()
		end
	else
		if self._logonFrame:onLogonByAccount(GlobalUserItem.szAccount,GlobalUserItem.szPassword) then
			self:showPopWait()
		end
	end
end

--链接游戏回掉
function ManageGameListLayer:onLogonCallBack(result,message)
	self:dismissPopWait()
	if  message ~= nil  and type(message) == "string" then
		local ru = cc.Director:getInstance():getRunningScene()
		if nil ~= ru then
			showToast(ru,message,2)
		end		
	end
	if result == 0 then
		self:onUpdataNotify()
	elseif result == 1 then		
		local clientscene = self._scene
		--判断是否是快速开始
		if nil ~= clientscene.m_bQuickStart and true == clientscene.m_bQuickStart then
			local roominfo = GlobalUserItem.GetRoomInfo(GlobalUserItem.nCurRoomIndex)
			if nil == roominfo then
				return
			end
			if bit.band(roominfo.wServerKind, yl.GAME_GENRE_PERSONAL) ~= 0 then
				--showToast(self, "房卡房间不支持快速开始！", 2)
				return
			end
			clientscene:onStartGame()
		else
			if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then
				print("ManageGameListLayer:onLogonCallBack:ManageGameListLayer enter priGame ")
			else
				-- 处理锁表
				local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
				if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
					GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
					clientscene:onStartGame()
				else
					clientscene:onChangeShowMode(yl.SCENE_ROOMLIST, self.m_bQuickStart)
				end
			end
		end		
	end
end

--显示等待
function ManageGameListLayer:showPopWait(isTransparent)
	self._scene:showPopWait(isTransparent)
end

function ManageGameListLayer:showGameUpdateWait()
	self.m_bGameUpdate = true
	ExternalFun.popupTouchFilter(1, false, "游戏更新中,请稍候！")
end

function ManageGameListLayer:dismissGameUpdateWait()
	self.m_bGameUpdate = false
	ExternalFun.dismissTouchFilter()
end

--关闭等待
function ManageGameListLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

function ManageGameListLayer:updateGame(gameinfo, index)
	local cell = nil
	if nil ~= index then
		cell = self._listView:cellAtIndex(index)
	end

	self:onGameUpdate(gameinfo)
	if nil ~= cell then		
		self.m_spDownloadMask = cell:getChildByName("download_mask")
		if nil ~= self.m_spDownloadMask then
			self.m_szMaskSize = self.m_spDownloadMask:getContentSize()			
		end
		self.m_labDownloadTip = cell:getChildByName("download_mask_tip")
		if nil ~= self.m_labDownloadTip then
			self.m_labDownloadTip:setString("0%")
		end
		self.m_spDownloadCycle = cell:getChildByName("download_cycle")
		if nil ~= self.m_spDownloadCycle then
			self.m_spDownloadCycle:stopAllActions()
			self.m_spDownloadCycle:setVisible(true)
			self.m_spDownloadCycle:runAction(cc.RepeatForever:create(cc.RotateBy:create(1.0, 360)))
		end
	end
end

--更新游戏
function ManageGameListLayer:onGameUpdate(gameinfo)
	--失败重试
	if not gameinfo and self._update ~= nil then
		self:showGameUpdateWait()
		--self._txtTips:setString("同步服务器信息中...")
		self._update:UpdateFile()
		return 
	end

	if not gameinfo and not self._downgameinfo then 
		showToast(self,"无效游戏信息！",1)
		return
	end

	self:showGameUpdateWait()
	--self._txtTips:setString("同步服务器信息中...")
	
	--记录
	if gameinfo ~= nil then
		self._downgameinfo = gameinfo
	end

	--更新参数
	local newfileurl = appConfigProxy._updateUrl.."/game/"..self._downgameinfo._Module.."/res/filemd5List.json"
	local dst = device.writablePath .. "game/" .. self._downgameinfo._Type .. "/"
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if cc.PLATFORM_OS_WINDOWS == targetPlatform then
		dst = device.writablePath .. "download/game/" .. self._downgameinfo._Type .. "/"
	end
	
	local src = device.writablePath.."game/"..self._downgameinfo._Module.."/res/filemd5List.json"
	local downurl = appConfigProxy._updateUrl .. "/game/" .. self._downgameinfo._Type .. "/"

	--创建更新
	self._update = Update:create(newfileurl,dst,src,downurl)
	self._update:upDateClient(handler(self, self.updateProgress), handler(self, self.updateResult))
end

function ManageGameListLayer:onUpdataNotify()
	showToast(self,"游戏版本信息错误！",1)
end

--更新进度
function ManageGameListLayer:updateProgress(sub, msg, mainpersent)
	if (mainpersent ~= -1) then
		local permsg = string.format("%d%%", mainpersent)
		if nil ~= self.m_spDownloadMask then
			local scale = (95 - mainpersent) / 100		
			self.m_spDownloadMask:setContentSize(self.m_szMaskSize.width, self.m_szMaskSize.height * scale)
		end
		
		if nil ~= self.m_labDownloadTip then
			self.m_labDownloadTip:setString(permsg)
		end
	end
end

--更新结果
function ManageGameListLayer:updateResult(result,msg)
	if nil ~= self.m_spDownloadCycle then
		self.m_spDownloadCycle:stopAllActions()
		self.m_spDownloadCycle:setVisible(false)
	end
	self:dismissGameUpdateWait()
	
	if result == true then
		--更新版本号
		for k,v in pairs(appConfigProxy._gameList) do
			if v._KindID == self._downgameinfo._KindID then
				versionProxy:setResVersion(v._ServerResVersion, v._KindID)
				v._Active = true
				break
			end
		end

		self._txtTips:setString("OK")
		self:onEnterGame(self._downgameinfo)
	else
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene then			
			if nil ~= self.m_spDownloadMask then	
				self.m_spDownloadMask:setContentSize(self.m_szMaskSize.width, self.m_szMaskSize.height)
			end

			if nil ~= self.m_labDownloadTip then
				self.m_labDownloadTip:setString("")
			end

			self._txtTips:setString("")
			QueryDialog:create(msg.."\n是否重试？",function(bReTry)
					if bReTry == true then
						self:onGameUpdate(self._downgameinfo)
					end
				end)
				:addTo(runScene)
		end		
	end
end

function ManageGameListLayer:onKeyBack()
	return self.m_bGameUpdate ~= false
end

return ManageGameListLayer