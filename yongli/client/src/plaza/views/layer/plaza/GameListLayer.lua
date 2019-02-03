--[[
	游戏列表界面
	2017_08_19 MXM
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local GameListLayer = class("GameListLayer", function(gamelist)
		local gamelist_layer =  display.newLayer()
    return gamelist_layer
end)

local Update = appdf.req(appdf.CLIENT_SRC.."app.controllers.ClientUpdate")
local QueryDialog = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.QueryDialog")
local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	

GameListLayer.GAME_KIND_ZHAJINHUA = 6

-- 进入场景而且过渡动画结束时候触发。
function GameListLayer:onEnterTransitionFinish()
	self._listView:reloadData()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function GameListLayer:onExitTransitionStart()
    return self
end

function GameListLayer:onExit()
	ExternalFun.SAFE_RELEASE(self.m_gameAni)
	self.m_gameAni = nil
end

function GameListLayer:ctor(gamelist)
	print("============= 游戏列表界面创建 =============")
	self.m_bQuickStart = false
	self.m_gameAni = nil
	
	local this = self
	
	--重置游戏类型
	GlobalUserItem.setCurGameKind(0)
	
	--更新大厅显示的界面KindID
	local customEvent = cc.EventCustom:new(yl.RY_SHOW_VIEW_GAME_KIND)
	customEvent.obj = 0
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	
	

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
			self:onExit()
		end
	end)
	
	--游戏列表
	self._listView = cc.TableView:create(cc.size(yl.WIDTH, 440))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
	self._listView:setPosition(cc.p(0,155))
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
end

--获取父场景节点(ClientScene)
function GameListLayer:getRootNode( )
	return self:getParent():getParent():getParent():getParent()
end

--更新游戏进入记录
function GameListLayer:updateEnterGameInfo(info)
	--self:getRootNode():updateEnterGameInfo(info)
	GlobalUserItem.m_tabEnterGame = info
end

function GameListLayer:onEnterGame( gameinfo, isQuickStart)
	--判断房间获取
	local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
	if not roomCount or 0 == roomCount then
		--self:onLoadGameList(gameinfo._KindID)
		showToast(self,"游戏维护中，请稍后再试！",1)
		return
	end
	
	self:updateEnterGameInfo()
	self.m_bQuickStart = isQuickStart

	GlobalUserItem.setCurGameKind(tonumber(gameinfo._KindID))
	
	--更新大厅显示的界面KindID
	local customEvent = cc.EventCustom:new(yl.RY_SHOW_VIEW_GAME_KIND)
	customEvent.obj = tonumber(gameinfo._KindID)
	cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
	
	GlobalUserItem.szCurGameName = gameinfo._KindName
	--MXM这里改为直接供普通金币房使用了
	--if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then
		--print(" GameListLayer enter priGame ")
	--else
	-- 处理锁表
	local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
	if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
		GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
		self:getRootNode():onStartGame()
	else
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {parentName = VIEW_LIST.CLIENT_SCENE}, VIEW_LIST.ROOM_LIST_LAYER)
		--self:getRootNode():onChangeShowMode(yl.SCENE_ROOMLIST, self.m_bQuickStart)
	end
	--end
end

---------------------------------------------------------------------
-- listview 相关
function GameListLayer:cellHightLight(view,cell)
end

function GameListLayer:cellUnHightLight(view,cell)
end

--子视图大小
function GameListLayer:cellSizeForTable(view, idx)
  	return view:getParent().m_fThird , 360
end

--子视图数目
function GameListLayer:numberOfCellsInTableView(view)
	if (appConfigProxy ._appStoreSwitch == 0) then
		if not view:getParent()._gameList then
			return 0
		else
			return #view:getParent()._gameList
		end
	else
		if (#view:getParent()._gameList > 4) then
			return 4
		else
			return #view:getParent()._gameList
		end
	end
end

--子视图点击
function GameListLayer:tableCellTouched(view, cell)
	if GlobalUserItem.isAngentAccount() then
		return
	end
		
	local index = cell:getIdx() 

	--获取游戏信息
	local gameinfo = self._gameList[index+1]
	if  not gameinfo then
		showToast(self:getRootNode(),"未找到游戏信息！",2)
		return
	end
	gameinfo.gameIndex = index

	--下载/更新资源 clientscene:getApp
	local version = versionProxy:getResVersion(gameinfo._KindID)
	
	if version then
		if gameinfo._ServerResVersion > version then
			self:updateGame(gameinfo, index)
		else
			self:onEnterGame(gameinfo, false)
		end
	else
		if (appConfigProxy._appStoreSwitch == 0) then
			self:updateGame(gameinfo, index)
		else
			self:onEnterGame(gameinfo, false)
		end
	end

end
	
--获取子视图
function GameListLayer:tableCellAtIndex(view, idx)	
	local cell = view:dequeueCell()
	
	--cell 里面有东西 抹除 添加如新的内容
	if (cell and cell:getIdx() == -1) then
		cell:removeAllChildren()
	else
		cell = cc.TableViewCell:new()
	end
	
	local gameinfo = self._gameList[idx+1]
	gameinfo.gameIndex = idx
	local filestr = "GameList/game_"..gameinfo._KindID..".png"
	if false == cc.FileUtils:getInstance():isFileExist(filestr) then
		filestr = "GameList/default.png"
	end
	
	local cellpos = cc.p(self.m_fThird * 0.5, view:getViewSize().height * 0.5)
	local gameBackGround = display.newSprite(filestr)
	local mask = nil
	local spTip = nil
	gameBackGround:addTo(cell)
		:setAnchorPoint(cc.p(0.5, 0))
		:setPosition(self.m_fThird * 0.5, 0)
		
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

		local cycle = cc.Sprite:create("GameList/spinner_circle.png")
		if nil ~= cycle then
			cycle:setPosition(cellpos)
			cycle:setVisible(false)
			cycle:setScale(1.3)
			cycle:setName("download_cycle")
			cell:addChild(cycle)
		end			
	end	
	
	local filesplist = "GameList/act_game_"..gameinfo._KindID..".plist"
	if cc.FileUtils:getInstance():isFileExist(filesplist) then
--[[        self.m_gameAni = ExternalFun.loadTimeLine(filestrcsb)
        local gameCsbNode = ExternalFun.loadCSB(filestrcsb, gameBackGround)
              --:setAnchorPoint(cc.p(0.5,0))
	          --:setPosition(cellpos)
              :setPosition(cc.p(view:getParent().m_fThird * 0.395,view:getViewSize().height * 0.495))
		gameCsbNode:stopAllActions()
		self.m_gameAni:gotoFrameAndPlay(0,true)
		gameCsbNode:runAction(self.m_gameAni)--]]
		--使用代码创建动画，csb动画存在bug
		cc.SpriteFrameCache:getInstance():addSpriteFrames(filesplist)
		local frames = {}
		for i=1, 15 do
			local frameName = string.format("game_%d_%d.png", gameinfo._KindID, i)
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(frameName) 
			table.insert(frames, frame)
		end
		local  animation =cc.Animation:createWithSpriteFrames(frames,0.1)

		--cc.AnimationCache:getInstance():addAnimation(animation, Define.VOICE_ANIMATION_KEY)


		--local animation = cc.AnimationCache:getInstance():getAnimation(Define.VOICE_ANIMATION_KEY)
        if nil ~= animation then
			local actionSprite = display.newSprite(filestr)
			local contentSize = gameBackGround:getContentSize()
			actionSprite:addTo(gameBackGround)
			:setPosition(contentSize.width * 0.5, contentSize.height * 0.5)		
            local action = cc.RepeatForever:create(cc.Animate:create(animation))
            actionSprite:runAction(action)
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
	
	return cell
	
--[[
	
	if not cell then
		cell = cc.TableViewCell:new()
		game = display.newSprite(filestr)
		game:addTo(cell)
			:setAnchorPoint(cc.p(0.5, 0))
			:setPosition(self.m_fThird * 0.5, 0)
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

			local cycle = cc.Sprite:create("GameList/spinner_circle.png")
			if nil ~= cycle then
				cycle:setPosition(cellpos)
				cycle:setVisible(false)
				cycle:setScale(1.3)
				cycle:setName("download_cycle")
				cell:addChild(cycle)
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
	cell:setTag(gameinfo._KindID)
	
	return cell--]]
--[[
    --游戏列表动画效果-MXM

	
    
	
     --动画结束--------------

	return cell--]]
end
---------------------------------------------------------------------

--链接游戏
function GameListLayer:onLoadGameList(nKindID)
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
function GameListLayer:onLogonCallBack(result,message)
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
		local clientscene = self:getRootNode()
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
			--if PriRoom and true == PriRoom:getInstance():onLoginEnterRoomList() then
				--print("GameListLayer:onLogonCallBack:GameListLayer enter priGame ")
			--else
				-- 处理锁表
				local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
				if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
					GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
					clientscene:onStartGame()
				else
					clientscene:onChangeShowMode(yl.SCENE_ROOMLIST, self.m_bQuickStart)
				end
			--end
		end		
	end
end

--显示等待
function GameListLayer:showPopWait(isTransparent)
	self:getRootNode():showPopWait(isTransparent)
end

function GameListLayer:showGameUpdateWait()
	self.m_bGameUpdate = true
	ExternalFun.popupTouchFilter(1, false, "游戏更新中,请稍候！")
end

function GameListLayer:dismissGameUpdateWait()
	self.m_bGameUpdate = false
	ExternalFun.dismissTouchFilter()
end

--关闭等待
function GameListLayer:dismissPopWait()
	self:getRootNode():dismissPopWait()
end

function GameListLayer:updateGame(gameinfo, index)
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
function GameListLayer:onGameUpdate(gameinfo)
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
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	
	if cc.PLATFORM_OS_WINDOWS == targetPlatform then
		self:updateResult(true,"")
		return
	end
	
	local version = versionProxy:getResVersion(gameinfo._KindID)

	if (version ~= nil) then 
		--版本号非空 使用热更新
		local newfileurl = appConfigProxy._updateUrl.."/game/"..self._downgameinfo._Module.."res/filemd5List.json"
		local dst = device.writablePath .. "game/" .. self._downgameinfo._Type .. "/"
		
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			dst = device.writablePath .. "download/game/" .. self._downgameinfo._Type .. "/"
		end
		
		local src = device.writablePath.."game/"..self._downgameinfo._Module.."res/filemd5List.json"
		local downurl = appConfigProxy._updateUrl .. "/game/" .. self._downgameinfo._Type .. "/"

		--创建更新
		self._update = Update:create(newfileurl,dst,src,downurl)
		self._update:upDateClient(handler(self, self.updateProgress), handler(self, self.updateResult))
	else
		--版本号空 使用包更新
		--调用C++下载
		local url = appConfigProxy._updateUrl.."/game/"..self._downgameinfo._Module .. "game.zip"
		local saveurl = device.writablePath .. "game/"..self._downgameinfo._Module
		downFileAsync(url, saveurl, nil, function(code, parm)
			--下载回调
			if code == appdf.DOWN_PRO_INFO then --进度信息
				self:updateProgress(100,"", tonumber(parm))
			elseif code == appdf.DOWN_COMPELETED then --下载完毕
				unZipAsync(saveurl .. "game.zip",saveurl,function(code, parm)
					if (code == appdf.UNZIP_COMPELETED) then
						self:updateResult(true,"")
					else
						--忽略解压错误
						print("unzip error")
					end
				end)	
			else
				print("down error")
			end
		end, 5)	
	end
end

function GameListLayer:onUpdataNotify()
	showToast(self,"游戏版本信息错误！",1)
end

--更新进度
function GameListLayer:updateProgress(sub, msg, mainpersent)
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
function GameListLayer:updateResult(result,msg)
	if nil ~= self.m_spDownloadCycle then
		self.m_spDownloadCycle:stopAllActions()
		self.m_spDownloadCycle:setVisible(false)
	end
	self:dismissGameUpdateWait()
	
	if result == true then
		--local app = self:getRootNode():getApp()

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

function GameListLayer:onKeyBack()
	return self.m_bGameUpdate ~= false
end

return GameListLayer