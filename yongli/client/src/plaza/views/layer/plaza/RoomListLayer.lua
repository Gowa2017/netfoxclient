--[[
	房间列表界面
	2017_08_20 MXM
]]
local GameServerItem   = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameServerItem")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local RoomListLayer = class("RoomListLayer", cc.BaseLayer)

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")

function RoomListLayer:ctor(isNoPlayMusic, isQuickStart)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = ""
	if (appConfigProxy._appStoreSwitch == 0) then
		csbPath = "RoomList/GameRoom.csb"
	else
		csbPath = "RoomList/GameRoomAppStore.csb"
	end	
	
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self.m_isNoPlayMusic = isNoPlayMusic
	self.m_bIsQuickStart = isQuickStart
end

-- 进入场景而且过渡动画结束时候触发。
function RoomListLayer:onEnterTransitionFinish()

end
-- 退出场景而且开始过渡动画时候触发。
function RoomListLayer:onExitTransitionStart()

end

function RoomListLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("zeroRoomEnterAni", function ()
		self:enterViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("oneRoomEnterAni", function ()
		self:enterViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("twoRoomEnterAni", function ()
		self:enterViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("threeRoomEnterAni", function ()
		self:enterViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("fourRoomEnterAni", function ()
		self:enterViewFinished()
	end)
	
	--加载标题
	
	local imgViewTitle = self._csbNode:getChildByName("room_title")
	local frameName = string.format("RoomList/room_title_%d.png", GlobalUserItem.getCurGameKind())
	imgViewTitle:loadTexture(frameName)
	
	if (not self.m_isNoPlayMusic) then
		--播放音效
		local musicPath = string.format("sound/game_%d.mp3", GlobalUserItem.getCurGameKind())
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, musicPath)
	end
	
	for i = 1, #self.m_tabRoomListInfo do
		local iteminfo = self.m_tabRoomListInfo[i]
		local wLv = (iteminfo == nil and 0 or iteminfo.wServerLevel)

		if 8 == wLv then
			--比赛场单独处理
		else
			local rule = (iteminfo == nil and 0 or iteminfo.dwServerRule)
			--wLv = (bit.band(yl.SR_ALLOW_AVERT_CHEAT_MODE, rule) ~= 0) and 10 or iteminfo.wServerLevel
			wLv = (wLv ~= 0) and wLv or 1
			local wRoom = math.mod(wLv, 6)--math.mod(wLv, 3)--bit.band(wLv, 3)
			local szName = (iteminfo == nil and "房间名称" or iteminfo.szServerName)
			local szCount = (iteminfo == nil and "0" or(iteminfo.dwOnLineCount..""))
			local szServerScore = (iteminfo == nil and "0" or string.format("%0.2f", iteminfo.lCellScore))
			local szOnLineCount = (iteminfo == nil and "0" or iteminfo.dwOnLineCount)
			local dwAndroidCount = (iteminfo == nil and "0" or iteminfo.dwAndroidCount)
			local dwDummyCount = (iteminfo == nil and "0" or iteminfo.dwDummyCount)
			local szEnterScore = (iteminfo == nil and "0" or string.format("%0.2f", iteminfo.lEnterScore))
			local enterGame = GlobalUserItem.m_tabEnterGame

			--房间btn
			local btnRoom = self._csbNode:getChildByName(string.format("btn_room_%d", wLv))
			assert(btnRoom ~= nil)
			
			--加载图片v
            local normalPath = string.format("RoomList/btn_level%d_0.png", wLv)
			local selectedPath = string.format("RoomList/btn_level%d_1.png", wLv)
           
            --if GlobalUserItem.getCurGameKind()== 407 then
            --    normalPath = string.format("RoomList/fish_%d_0.png", wLv)
			--    selectedPath = string.format("RoomList/fish_%d_1.png", wLv)  
            --end

			btnRoom:loadTextures(normalPath, selectedPath)
			
			--准入分数
			local textEnterScore = btnRoom:getChildByName("difen")
			textEnterScore:setString("入场:" .. szEnterScore)
			
			--成绩
			local textServerScore = btnRoom:getChildByName("zunru")
			textServerScore:setString(szServerScore)
			
			--点击回调函数
			btnRoom:addClickEventListener(function ()
				self:onGameBtnCallBack(btnRoom, iteminfo)
			end)
			
			--检查房间背景资源--留着以后用
			--local modulestr = string.gsub(enterGame._KindName, "%.", "/")
			--[[local path = "game/" .. modulestr .. "res/roomlist/icon_roomlist_" .. wRoom .. ".png"
			local framename = enterGame._KindID .. "_icon_roomlist_" .. wRoom .. ".png"
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
			if nil ~= frame then
				local sp = cc.Sprite:createWithSpriteFrame(frame)
				sp:setPosition(cc.p(self.m_fThree * 0.5, view:getViewSize().height * 0.5 + 10))
				cell:addChild(sp)
			elseif cc.FileUtils:getInstance():isFileExist(path) then
				--房间类型
				display.newSprite(path)
					:setPosition(cc.p(self.m_fThree * 0.5, view:getViewSize().height * 0.5 + 10))
					:addTo(cell)
			end
			--]]

			--房间类型
			--[[framename = enterGame._KindID .. "_title_icon_" .. wLv .. ".png"
			frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
			path = "game/" .. modulestr .. "res/roomlist/title_icon_" .. wLv .. ".png"
			if nil ~= frame then
				local sp = cc.Sprite:createWithSpriteFrame(frame)
				sp:setPosition(cc.p(self.m_fThree * 0.5,100))
				cell:addChild(sp)
			elseif cc.FileUtils:getInstance():isFileExist(path) then		
				display.newSprite(path)
					:setPosition(cc.p(self.m_fThree * 0.5,100))
					:addTo(cell)
			else--]]
			
			
			
			
			
			
--[[			local default = "RoomList/title_icon_" .. wLv .. ".png"
			if cc.FileUtils:getInstance():isFileExist(default) then
				--默认资源
				display.newSprite(default)
					:setPosition(cc.p(self.m_fThree * 0.5,200))
					:addTo(cell)
			end--]]
			--end

			--底注
			--[[display.newSprite("RoomList/text_roomlist_cellscore.png")
				--:setPosition(cc.p(self.m_fThree * 0.5 - 10,70))
				:setAnchorPoint(cc.p(1.0,0.5))
				:addTo(cell)--]]
--[[
			local serverScore = cc.LabelAtlas:_create(szServerScore, "RoomList/num_roomlist_cellscore.png", 14, 19, string.byte(".")) 
				:move(120,22)
				:setAnchorPoint(cc.p(0,0.5))
				:addTo(cell)
		   --在线人数
			local onlineCount = cc.LabelAtlas:_create(szOnLineCount + dwAndroidCount + dwDummyCount, "RoomList/num_roomlist_cellscore.png", 14, 19, string.byte(".")) 
				:move(self.m_fThree * 0.5 - 28,58)
				:setAnchorPoint(cc.p(0,0.5))
				:addTo(cell)

			--最低进入条件
			local enterScore = cc.LabelAtlas:_create(szEnterScore, "RoomList/num_roomlist_cellscore.png", 14, 19, string.byte(".")) 
				:move(265,22)
				:setAnchorPoint(cc.p(0,0.5))
				:addTo(cell)--]]
				
			--监听以更新实时UI人数
			local listener = cc.EventListenerCustom:create(yl.RY_UPDATE_SERVERINFO_NOTIFY,function (e)
				if (type(e.obj) ~= "table") then
					return
				end
				
				self:updateGameServerInfo()
				
				local roomItemInfo = e.obj[i]
				if (roomItemInfo) then
					--onlineCount:setString(string.format("%d", roomItemInfo.dwOnLineCount + roomItemInfo.dwAndroidCount + roomItemInfo.dwDummyCount))
					textServerScore:setString(string.format("%0.2f", roomItemInfo.lCellScore))
					textEnterScore:setString(string.format("入场:%0.2f", string.formatNumberFhousands(roomItemInfo.lEnterScore)))
				else
					textServerScore:setString("0.0")
					--onlineCount:setString("0")
					textEnterScore:setString("入场:0.0")
				end
			end)
			cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, btnRoom)
		end
	end
	
	
	local roomCount = #self.m_tabRoomListInfo
	if (roomCount == 0) then
		self._csbNodeAni:play("zeroRoomEnterAni", false)
	elseif (roomCount) == 1 then
		self._csbNodeAni:play("oneRoomEnterAni", false)
	elseif (roomCount == 2) then
		self._csbNodeAni:play("twoRoomEnterAni", false)
	elseif (roomCount == 3) then
		self._csbNodeAni:play("threeRoomEnterAni", false)
	elseif (roomCount == 4) then
		self._csbNodeAni:play("fourRoomEnterAni", false)
	else
		assert(false, "room count expected not more than 4 room, please check")
		self._csbNodeAni:play("fourRoomEnterAni", false)
	end
	
end

function RoomListLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("zeroRoomExitAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("oneRoomExitAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("twoRoomExitAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("threeRoomExitAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:setAnimationEndCallFunc("fourRoomExitAni", function ()
		self:exitViewFinished()
	end)
	
	
	local roomCount = #self.m_tabRoomListInfo
	if (roomCount == 0) then
		self._csbNodeAni:play("zeroRoomExitAni", false)
	elseif (roomCount) == 1 then
		self._csbNodeAni:play("oneRoomExitAni", false)
	elseif (roomCount == 2) then
		self._csbNodeAni:play("twoRoomExitAni", false)
	elseif (roomCount == 3) then
		self._csbNodeAni:play("threeRoomExitAni", false)
	elseif (roomCount == 4) then
		self._csbNodeAni:play("fourRoomExitAni", false)
	else
		assert(false, "room count expected not more than 4 room, please check")
		self._csbNodeAni:play("fourRoomExitAni", false)
	end
	
end

function RoomListLayer:initUI()
	--返回按钮
	self._btnReturn = self._csbNode:getChildByName("btn_return")
	self._btnReturn:addClickEventListener(handler(self, self.onBtnCallBack))
	
	--充值按钮
	self._btnRecharge = self._csbNode:getChildByName("user_score"):getChildByName("btn_recharge")
	self._btnRecharge:addClickEventListener(handler(self, self.onBtnCallBack))
	
	--金币
	self._txtUserScore = self._csbNode:getChildByName("user_score"):getChildByName("text_score")

	local str = string.formatNumberTThousands(GlobalUserItem.lUserScore)
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	if (GlobalUserItem.lUserScore < 10000) then
		self._txtUserScore:setString(str)
	elseif (GlobalUserItem.lUserScore >= 10000 and GlobalUserItem.lUserScore < 100000000) then
		self._txtUserScore:setString(str .. "万")
	elseif (GlobalUserItem.lUserScore >= 100000000) then
		self._txtUserScore:setString(str .. "亿")
	end
	
	
	--注册UI监听事件
	self:attach("M2V_Update_UserScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		if (score < 10000) then
			self._txtUserScore:setString(str)
		elseif (score >= 10000 and score < 100000000) then
			self._txtUserScore:setString(str .. "万")
		elseif (score >= 100000000) then
			self._txtUserScore:setString(str .. "亿")
		end
	end)	
	
	--公告
	self._notify = self._csbNode:getChildByName("sp_trumpet_bg")
	btn = self._notify:getChildByName("btn_trumpet")
	
	--快速开始
	self._btnQuickStart = self._csbNode:getChildByName("btn_quick_start")
	self._btnQuickStart:addClickEventListener(handler(self, self.onBtnCallBack))
	
	--大厅固定公告消息视图监听
	self:attach("M2V_Update_GongGao_Message", function (e, data, msg)
		local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_text")
		local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_mask")
		local mask_contentsize = mask_gonggao:getContentSize()
		local text_content = ""
		for _,v in ipairs(data) do
			text_content = text_content .. v.content .. "\t\t"
		end
		
		self:showGongGao(text_content, nil)
	end)
	
	local text_content = ""
	for _,v in ipairs(appConfigProxy._hallNotice) do
		text_content = text_content .. v.content .. "\t\t"
	end
	self:showGongGao(text_content, nil)
	
	if true == self.m_bIsQuickStart then
		self:stopAllActions()
		GlobalUserItem.nCurRoomIndex = 1
		self:onStartGame()
	end
	
	--房间信息
	self.m_tabRoomListInfo = {}
	
	--先用上次缓存的数据创建房间视图
	self:updateGameServerInfo()
	
	--在房间界面 每10秒请求一次房间信息
	local repeatAction = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(10), cc.CallFunc:create(function ()
		--去请求新的房间列表
		local QueryServerInfo = CCmd_Data:create(2)
		QueryServerInfo:setcmdinfo(yl.MDM_MB_SERVER_LIST,yl.SUB_QUEREY_SERVER)
		QueryServerInfo:pushword(yl.INVALID_WORD)
		appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, QueryServerInfo, function (Datatable, Responce)
			if (Responce.code == 0) then
				if (Datatable.sub == yl.SUB_MB_LIST_SERVER_START) then
					--清空原来的列表
					GlobalUserItem.roomlist = {}
				elseif (Datatable.sub == yl.SUB_MB_LIST_SERVER_END) then
					--列表发送完成，更新视图通知
					local customEvent = cc.EventCustom:new(yl.RY_UPDATE_SERVERINFO_NOTIFY)
					customEvent.obj = self.m_tabRoomListInfo
					cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
				elseif (Datatable.sub == yl.SUB_MB_LIST_SERVER) then
					--计算房间数目
					local len = Datatable.data:getlen()
					
					if (len - math.floor(len/yl.LEN_GAME_SERVER_ITEM)*yl.LEN_GAME_SERVER_ITEM) ~= 0 then
						assert(false)
						print("roomlist_len_error"..len)
						return
					end
					
					local itemcount =  math.floor(len/yl.LEN_GAME_SERVER_ITEM)
					local tempAllRoom = {}
					--读取房间信息
					for i = 1,itemcount do
						local item = GameServerItem:create()
										:onInit(Datatable.data)
						if not item then 
							break
						end
						if nil == tempAllRoom[item.wKindID] then
							tempAllRoom[item.wKindID] = {}
						end

						if item.wServerType == yl.GAME_GENRE_PERSONAL then
							if GlobalUserItem.bEnableRoomCard then
								if PriRoom then
									PriRoom:getInstance().m_tabPriModeGame[item.wKindID] = true
								end				
								table.insert(tempAllRoom[item.wKindID], item)
							end
						else
							table.insert(tempAllRoom[item.wKindID], item)
						end		
					end
					
					-- 整理列表
					for k,v in pairs(tempAllRoom) do
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
				end
			end

		end)
	end)))
	self:runAction(repeatAction)
end

--显示公告
function RoomListLayer:showGongGao(text, callback)
	local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_text")
	local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_mask")
	local mask_contentsize = mask_gonggao:getContentSize()
	local text_content = text
	
	text_gonggao:setString(text_content)
	local time_speed = 80
	local speed = (mask_contentsize.width + text_gonggao:getContentSize().width) / time_speed
	local startPosition = cc.p(mask_contentsize.width, mask_contentsize.height / 2)
	local endPosition = cc.p(-text_gonggao:getContentSize().width, mask_contentsize.height / 2)
	
	text_gonggao:setPosition(startPosition)
	
	text_gonggao:stopAllActions()
	local action = cc.RepeatForever:create(cc.Sequence:create(
	cc.MoveTo:create(speed, endPosition),
	cc.CallFunc:create(function()
		text_gonggao:setPosition(startPosition)
		if (callback) then
			callback()
		end
	end)))
	text_gonggao:runAction(action)
end

function RoomListLayer:onGameBtnCallBack(sender, gameinfo)

	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	local findContext = contextProxy:findContextByName(VIEW_LIST.CLIENT_SCENE)
	
	if (findContext == nil) then
		assert(false, "can't find ClientScene!")
	end
	
	local viewCompoment = findContext:getView()
	
	GlobalUserItem.nCurRoomIndex = gameinfo._nRoomIndex
	GlobalUserItem.bPrivateRoom = (gameinfo.wServerType == yl.GAME_GENRE_PERSONAL)
	if viewCompoment:roomEnterCheck() then
		viewCompoment:onStartGame()
	end	
end

function RoomListLayer:onBtnCallBack(sender)
	local senderName = sender:getName()
	if (senderName == "btn_return") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ROOM_LIST_LAYER})
	elseif (senderName == "btn_recharge") then
		if (appConfigProxy._appStoreSwitch == 0) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
		else
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
		end		
	elseif (senderName == "btn_quick_start") then
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		local findContext = contextProxy:findContextByName(VIEW_LIST.CLIENT_SCENE)
		
		if (findContext == nil) then
			assert(false, "can't find ClientScene!")
		end
		
		local viewCompoment = findContext:getView()
		
		GlobalUserItem.nCurRoomIndex = 1
		if viewCompoment:roomEnterCheck() then
			viewCompoment:onStartGame()
		end	
	end
end

function RoomListLayer:updateGameServerInfo()
	self.m_tabRoomListInfo = {}
	for k,v in pairs(GlobalUserItem.roomlist) do
		if tonumber(v[1]) == GlobalUserItem.getCurGameKind() then
			local listinfo = v[2]
			if type(listinfo) ~= "table" then
				break
			end
			local normalList = {}
			for k,v in pairs(listinfo) do
				if v.wServerType ~= yl.GAME_GENRE_PERSONAL then
					table.insert( normalList, v)
				end
			end
			self.m_tabRoomListInfo = normalList
			break
		end
	end	
end

function RoomListLayer:onStartGame(index)
	local iteminfo = GlobalUserItem.GetRoomInfo(index)
	if iteminfo ~= nil then
		if PriRoom and true == GlobalUserItem.bPrivateRoom then
			self:showPopWait()
		else
			self._scene:showGameWait()
		end
		
		self._scene:onStartGame(index)
	end
end

return RoomListLayer