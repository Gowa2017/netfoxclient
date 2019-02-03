--默认游戏桌子界面

local RoomLayer = class("RoomLayer", function(frameEngine,scene)
		local roomLayer =  display.newLayer()
    return roomLayer
end)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

RoomLayer.BT_FIND = 1
RoomLayer.BT_LOCK = 2

function RoomLayer:ctor(frameEngine, scene, bQuickStart)
	--椅子位置定义 (以桌子背景为父节点得到的坐标)
	self.USER_POS = 
	{
		{cc.p(168,219)},	--1
		{cc.p(168,219),cc.p(168,-29)}, --2
		{cc.p(18,18),cc.p(170,222),cc.p(320,18)}, -- 3
		{cc.p(-18,109),cc.p(170,220),cc.p(360,109),cc.p(170,-27)}, -- 4
		{cc.p(-3,160),cc.p(170,220),cc.p(342,160),cc.p(310,6),cc.p(30,6)}, -- 5
		{cc.p(14,180),cc.p(170,220),cc.p(324,180),cc.p(324,19),cc.p(170,-26),cc.p(14,19)}, --6
		{cc.p(-20,100),cc.p(33,200),cc.p(170,220),cc.p(304,200),cc.p(360,100),cc.p(304,0),cc.p(33,0)}, --7
		{cc.p(-20,100),cc.p(33,200),cc.p(170,220),cc.p(304,200),cc.p(360,100),cc.p(304,0),cc.p(170,-26),cc.p(33,0)}, -- 8
		{cc.p(-18,72),cc.p(9,178),cc.p(107,220),cc.p(224,220),cc.p(326,178),cc.p(352,72),cc.p(282,-8),cc.p(167,-30),cc.p(51,-8)} --9
	}

	self._frameEngine = frameEngine
	self._scene = scene
	self.m_bQuickStart = bQuickStart or false

	self:setContentSize(yl.WIDTH,yl.HEIGHT)

	local this = self

    self._btcallBack = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self._frameEngine:setViewFrame(self)

    self._tableCount = self._frameEngine._wTableCount
	self._chairCount = self._frameEngine._wChairCount
	self._cellCount = self._tableCount --math.floor(self._tableCount/2)

	--[[if self._tableCount > 0 then
		if math.mod(self._tableCount ,2) ~= 0 then
			self._cellCount = self._cellCount + 1
		end
	end]]

	self._chairMode = 1
	if self._chairCount > 9 or self._chairCount == 0 then
		self._chairMode = 1
	else
		self._chairMode = self._chairCount
	end

	self._bSitdown = true

	--cell宽度
	self:getCellWidth()

	--区域设置
	self:setContentSize(yl.WIDTH,yl.HEIGHT)

	--房间列表
	self._listView = cc.TableView:create(cc.size(yl.WIDTH, 420))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)   
	self._listView:setPosition(cc.p(0,yl.HEIGHT/2-220))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:registerScriptHandler(self.tableCellTouched, cc.TABLECELL_TOUCHED)
	self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	self._listView:reloadData()
	ccui.Button:create("Room/bt_lock_0.png","Room/bt_lock_1.png")
		:addTo(self)
		:setTag(RoomLayer.BT_LOCK)
		:move(70,580)
		--:setScale(0.75)
		:addTouchEventListener(self._btcallBack)

	if true == self.m_bQuickStart then
		self:stopAllActions()
		self:onSitDown(1, 1)
	end

	self.m_bEnableKeyBack = true
end

function RoomLayer:onExitRoom()
    self:stopAllActions()
    self:dismissPopWait()
    self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
end

--显示等待
function RoomLayer:showPopWait()
	if self._scene and self._scene.showPopWait then
		self._scene:showPopWait()
	end
end

--关闭等待
function RoomLayer:dismissPopWait()
	if self._scene and self._scene.dismissPopWait then
		self._scene:dismissPopWait()
	end
end

function RoomLayer:onButtonClickedEvent(tag,ref)
	print("RoomLayer:onButtonClickedEvent:"..tag)
	if tag == RoomLayer.BT_LOCK then
		self.m_bEnableKeyBack = false
		self._scene:createPasswordEdit("请输入要设置的桌子密码", function(pass)
			self.m_bEnableKeyBack = true
			self._frameEngine:SendEncrypt(pass)
		end)
	elseif tag >410 and tag < 420 then
		local tableid = ref:getParent():getParent():getIdx()
		local chairid = tag - 411
		self:onSitDown(tableid, chairid)
	end
end

--子视图大小
function RoomLayer:cellSizeForTable(view, idx)
  	return self.m_nCellWidth , 420
end

--子视图数目
function RoomLayer.numberOfCellsInTableView(view)
	return view:getParent()._cellCount
end

function RoomLayer.tableCellTouched(view, cell)

end

--获取子视图
function RoomLayer:tableCellAtIndex(view, idx)
	local cell = view:dequeueCell()
	--椅子模式
	local chairMode = self._chairMode
	--用户列表
	local tableUser = self._tableUser
	--游戏引擎
	local engine = self._frameEngine

	local chair_pos = self.USER_POS[chairMode]
	local bg1 = nil

	if not cell then
		cell = cc.TableViewCell:new()

		local btcallback =  function(ref, type)
			 if type == ccui.TouchEventType.ended then
         		view:getParent():onButtonClickedEvent(ref:getTag(),ref)
       		 end
		end
		--桌子背景
		bg1 = self:createTableBg()
			:move(yl.WIDTH/6 , 420 / 2)
			:setTag(100)
			:addTo(cell)		

		--椅子用户
		for i =1 , 9 do
			ccui.Button:create("Room/bg_chair.png","Room/bg_chair.png")
				:setTag(410+i)
				:addTo(bg1)
				:addTouchEventListener(btcallback)
		end
	else
		bg1 = cell:getChildByTag(100)
	end
	if nil == bg1 then
		return cell
	end	

	chair_pos = bg1.chair_pos
	if nil == bg1 or type(chair_pos) ~= "table" then
		return cell
	end

	--桌子号
	local tableid = idx --idx*2

	--是否显示桌子
	local bShow = tableid < view:getParent()._tableCount
	cell:getChildByTag(100):setVisible(bShow)
	if bShow then
		cell:getChildByTag(100):getChildByTag(1):setString(string.format("%03d",tableid+1) .. "号桌")
		local tablestatus = engine:getTableInfo(tableid+1)
		if tablestatus then
			if tablestatus.cbPlayStatus and tablestatus.cbPlayStatus == 1 then
				cell:getChildByTag(100):getChildByTag(2):setTexture("Room/flag_playstatus.png")
			else
				cell:getChildByTag(100):getChildByTag(2):setTexture("Room/flag_waitstatus.png")
			end

			if tablestatus.cbTableLock and tablestatus.cbTableLock == 1 then
				cell:getChildByTag(100):getChildByTag(3):setVisible(true)
			else
				cell:getChildByTag(100):getChildByTag(3):setVisible(false)
			end

			self:updateTable(bg1, tablestatus.cbPlayStatus)
		else
			cell:getChildByTag(100):getChildByTag(2):setTexture("Room/flag_waitstatus.png")
			cell:getChildByTag(100):getChildByTag(3):setVisible(false)
		end
	end
	for i = 1,9 do
		local headclip = bg1:getChildByTag(310+i)
		if i>chairMode or  not bShow then
			if nil ~= headclip then
				headclip:setVisible(false)
			end
			bg1:getChildByTag(410+i):setVisible(false)
		else
			local useritem = engine:getTableUserItem(tableid, i-1)
			local pos = chair_pos[i] or cc.p(0, 0)
			bg1:getChildByTag(410+i):move(pos.x,pos.y)
				:setVisible(true)
			if not useritem then
				local tmpclip = bg1:getChildByTag(310+i)
				if nil ~= tmpclip then
					tmpclip:setVisible(false)
				end
				bg1:getChildByTag(410+i):loadTextures("Room/chair_empty.png","Room/chair_empty.png")
			else				
				if nil == headclip then
					local head = HeadSprite:createClipHead(useritem, 40)
					head:setTag(310+i)
					bg1:addChild(head)
					headclip = head
				else
					headclip:updateHead(useritem)
				end
				headclip:move(pos.x,pos.y)
						:setVisible(true)
				bg1:getChildByTag(410+i):loadTextures("Room/bg_chair.png","Room/bg_chair.png")
			end
		end
	end

	return cell
end

--根据游戏类型、游戏人数区分桌子底图
function RoomLayer:createTableBg()
	local entergame = self._scene:getEnterGameInfo()
	local sp_table = nil
	local chair_pos = nil

	--自定义资源
	local modulestr = string.gsub(entergame._KindName, "%.", "/")
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	local customRoomFile = ""
	if cc.PLATFORM_OS_WINDOWS == targetPlatform then
		customRoomFile = "game/" .. modulestr .. "src/views/GameRoomLayer.lua"
	else
		customRoomFile = "game/" .. modulestr .. "src/views/GameRoomLayer.luac"
	end
	if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
		sp_table, chair_pos = appdf.req(customRoomFile):getTableParam()
	end

	local bgSize = cc.size(0, 0)
	--默认资源
	if nil == sp_table or nil == chair_pos then
		print("RoomLayer:createTableBg default param")
		sp_table = cc.Sprite:create("Room/bg_table.png")
		chair_pos = self.USER_POS[self._chairMode]

		bgSize = sp_table:getContentSize()
		--桌号背景
		display.newSprite("Room/bg_tablenum.png")
			:addTo(sp_table)
			:move(bgSize.width * 0.5,10)
		ccui.Text:create("", "fonts/round_body.ttf", 16)
			:addTo(sp_table)
			:setColor(cc.c4b(255,193,200,255))
			:setTag(1)
			:move(bgSize.width * 0.5,12)
		--状态
		display.newSprite("Room/flag_waitstatus.png")
			:addTo(sp_table)
			:setTag(2)
			:move(bgSize.width * 0.5, bgSize.height * 0.5)
	end
	bgSize = sp_table:getContentSize()
	--锁桌
	display.newSprite("Room/plazz_sp_locker.png")
		:addTo(sp_table)
		:setTag(3)
		:move(bgSize.width * 0.5, bgSize.height * 0.5)
	sp_table.chair_pos = chair_pos

	return sp_table
end

-- 更新桌子图(游戏状态/非游戏状态)
function RoomLayer:updateTable( spTable ,cbStatus )
	local tableFile = ""

	local entergame = self._scene:getEnterGameInfo()
	local modulestr = string.gsub(entergame._KindName, "%.", "/")
	if 1 == cbStatus then
		-- 游戏中
		tableFile = "game/" .. modulestr .. "res/roomlist/roomtable_play.png"
	else 
		-- 等待中
		tableFile = "game/" .. modulestr .. "res/roomlist/roomtable.png"
	end
	
	if cc.FileUtils:getInstance():isFileExist(tableFile) then
		spTable:setTexture(tableFile)
	end
end

function RoomLayer:getCellWidth()
	local entergame = self._scene:getEnterGameInfo()

	--自定义数量
	local modulestr = string.gsub(entergame._KindName, "%.", "/")
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	local customRoomFile = ""
	if cc.PLATFORM_OS_WINDOWS == targetPlatform then
		customRoomFile = "game/" .. modulestr .. "src/views/GameRoomLayer.lua"
	else
		customRoomFile = "game/" .. modulestr .. "src/views/GameRoomLayer.luac"
	end

	local count = nil
	if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
		count = appdf.req(customRoomFile):getShowCount()
	end
	count = count or 3

	self.m_nCellWidth = yl.WIDTH / count
end

--查找桌子
function RoomLayer:onFindTable()
	-- body
end

--加密桌子
function RoomLayer:onCreateLockTable()
	-- body
end

--加密桌子
function RoomLayer:onEnterLockTable(tableid,chairid)
	self.m_bEnableKeyBack = false
	self._scene:createPasswordEdit("请输入桌子密码", function(pass)
			self.m_bEnableKeyBack = true
			if self._frameEngine:SitDown(tableid, chairid, pass) then
				self:showPopWait()
			end
		end)
end

--坐下桌子
function RoomLayer:onSitDown(tableid,chairid)
	print("onSitDown table"..tableid.." chair"..chairid)
	local tablestatus = self._frameEngine:getTableInfo(tableid + 1)
	if tablestatus then
		--已经开始 判断是否动态加入
		if tablestatus.cbPlayStatus ~= 0 then
			print("已经开始")
			--return
		end
		--加锁处理 显示密码界面
		if tablestatus.cbTableLock ~= 0 then
			self:onEnterLockTable(tableid,chairid)
			return
		end
	end
	
	if self._frameEngine:SitDown(tableid,chairid) then
		self:showPopWait()
	end
end

function RoomLayer:onKeyBack()
	return not self.m_bEnableKeyBack
end

function RoomLayer:upDataTableStatus(tableid,tablestatus)
	self._listView:updateCellAtIndex(tableid - 1)--math.floor(tableid/2))
end

function RoomLayer:onEventUserEnter(tableid,chairid,useritem)
	self._listView:updateCellAtIndex(tableid)--math.floor(tableid/2))
end

function RoomLayer:onEventUserStatus(useritem,newstatus,oldstatus)	
	if oldstatus and oldstatus.wTableID ~= yl.INVALID_TABLE then
		self._listView:updateCellAtIndex(oldstatus.wTableID)--math.floor(oldstatus.wTableID/2))
	end
	if newstatus and newstatus.wTableID ~= yl.INVALID_TABLE then
		self._listView:updateCellAtIndex(newstatus.wTableID)--math.floor(newstatus.wTableID/2))
	end
end

function RoomLayer:onGetTableInfo()
	if nil ~= self._listView then
		self._listView:reloadData()
	end	
end

function RoomLayer:onQuickStart()
	if self._frameEngine:QueryChangeDesk() == true then
		self:showPopWait()
	end
end

function RoomLayer:onEnterTable()
	self._scene:onEnterTable()
end

function RoomLayer:onReQueryFailure(code,msg)	
	self:dismissPopWait()

	if msg and #msg > 0 then
		showToast(self._scene,msg,1)
	end
end

return RoomLayer