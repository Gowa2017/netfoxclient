local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
--头像选择界面
local SelectHeadLayer = class("SelectHeadLayer", cc.Layer)

SelectHeadLayer.BTN_LOCAL = 101
SelectHeadLayer.BTN_SYS = 102

function SelectHeadLayer:ctor( viewparent )
	self.m_parent = viewparent
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)
	
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Userinfo/SelectHeadLayer.csb", self)
	self._rootlayer = rootLayer
	
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end
	------
	--选择方式
	self.m_spSelectType = csbNode:getChildByName("select_type")
	self.m_spSelectType:setVisible(false)
	--本地图片
	local btn = self.m_spSelectType:getChildByName("local_btn")
	btn:setTag(SelectHeadLayer.BTN_LOCAL)
	btn:addTouchEventListener(btnEvent)
	--系统头像
	btn = self.m_spSelectType:getChildByName("system_btn")
	btn:setTag(SelectHeadLayer.BTN_SYS)
	btn:addTouchEventListener(btnEvent)
	------
	
	------
	--系统头像列表
	self.m_spSysSelect = csbNode:getChildByName("head_list")
	self.m_spSysSelect:setVisible(true)
	self.m_tableView = nil
	self.m_tabSystemHead = {}
	self.m_vecTouchBegin = {x = 0, y = 0}
	--------------------------------
	
	self.m_listener = cc.EventListenerCustom:create(yl.RY_UPDATE_FACE_NOTIFY,handler(self, self.onUpdateFace))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function SelectHeadLayer:onUpdateFace( event  )
	--更新头像
	self:refreshSystemHeadList()--MXM
end

function SelectHeadLayer:refreshSystemHeadList(  )
	if nil == self.m_tableView then
		self.content = self.m_spSysSelect:getChildByName("content")
		self.m_tableView = cc.TableView:create(self.content:getContentSize())
		self.m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
		self.m_tableView:setPosition(self.content:getPosition())
		self.m_tableView:setDelegate()
		self.m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
		self.m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
		self.m_tableView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
		self.m_spSysSelect:addChild(self.m_tableView)
		self.m_tableView = self.m_tableView
		self.content:removeFromParent()
		self.m_tableView:reloadData()
	else
		self.m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
		self.m_tableView:reloadData()
		
	end
end

function SelectHeadLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	self.m_spSelectType:setVisible(false)
	if SelectHeadLayer.BTN_LOCAL == tag then
		local function callback( param )
			if type(param) == "string" then
				print("lua call back " .. param)
				if cc.FileUtils:getInstance():isFileExist(param) then
					--发送上传头像
					local url = BaseConfig.WEB_HTTP_URL .. "/WS/Account.ashx?action=uploadface"
					local uploader = CurlAsset:createUploader(url,param)
					if nil == uploader then
						showToast(self, "自定义头像上传异常", 2)
						return
					end
					local nres = uploader:addToFileForm("file", param, "image/png")
					--用户标示
					nres = uploader:addToForm("userID", GlobalUserItem.dwUserID)
					--登陆时间差
					local delta = tonumber(currentTime()) - tonumber(GlobalUserItem.LogonTime)
					print("time delta " .. delta)
					nres = uploader:addToForm("time", delta .. "")
					--客户端ip
					local ip = MultiPlatform:getInstance():getClientIpAdress() or "192.168.1.1"
					nres = uploader:addToForm("clientIP", ip)
					--机器码
					local machine = GlobalUserItem.szMachine or "A501164B366ECFC9E249163873094D50"
					nres = uploader:addToForm("machineID", machine)
					--会话签名
					nres = uploader:addToForm("signature", GlobalUserItem:getSignature(delta))
					if 0 ~= nres then
						showToast(self, "上传表单提交异常,error code ==> " .. nres, 2)
						return
					end
					self.m_parent:showPopWait()
					uploader:uploadFile(function (sender, ncode, msg)
						self:onUploadFaceResult(sender, ncode, msg, param)
					end)
				end
			end
		end
		MultiPlatform:getInstance():triggerPickImg(callback, true)
	elseif SelectHeadLayer.BTN_SYS == tag then
		self.m_spSysSelect:setVisible(true)
		self:refreshSystemHeadList()
	end
end

function SelectHeadLayer:onUploadFaceResult(sender, ncode, msg, param)
	self.m_parent:dismissPopWait()
	if 0 == ncode then
		if type(msg) == "string" then
			print("msg ==> " .. msg)
		end
		local ok, datatable = pcall(function ()
			return cjson.decode(msg)
		end)
		if ok then
			dump(datatable, "datatable")
			if nil ~= datatable.code and 0 == datatable.code then
				local msgdata = datatable.data
				dump(msgdata, "msgdata")
				if nil ~= msgdata and type(msgdata) == "table" then
					local valid = msgdata.valid
					if valid then
						cc.Director:getInstance():getTextureCache():removeTextureForKey(param)
						local sp = cc.Sprite:create(param)
						if nil ~= sp then
							GlobalUserItem.dwCustomID = tonumber(msgdata.CustomID)
							local frame = sp:getSpriteFrame()
							local framename = GlobalUserItem.dwUserID .. "_custom_" .. GlobalUserItem.dwCustomID .. ".ry"
							local oldframe = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
							if nil ~= oldframe then
								oldframe:release()
							end
							cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(framename)
							
							cc.SpriteFrameCache:getInstance():addSpriteFrame(frame, framename)
							frame:retain()
							
							if nil ~= self.m_parent and nil ~= self.m_parent.onFaceResultSuccess then
								self.m_parent:onFaceResultSuccess()
							end
							showToast(self, "自定义头像上传成功", 2)
						end
					else
						if type(msg.msg) == "string" then
							showToast(self, msg.msg, 2)
						end
					end
				end
			end
		else
			showToast(self, "自定义头像上传异常", 2)
		end
		return
	end
	showToast(self, "自定义头像上传异常, error code ==> " .. ncode, 2)
end

function SelectHeadLayer:onTouchBegan(touch, event)
	self.m_vecTouchBegin = {x = touch:getLocation().x, y = touch:getLocation().y}
	return self:isVisible()
end

function SelectHeadLayer:onTouchEnded( touch, event)
	local pos = touch:getLocation()
	if math.abs(pos.x - self.m_vecTouchBegin.x) > 30
		or math.abs(pos.y - self.m_vecTouchBegin.y) > 30 then
		self.m_vecTouchBegin = {x = 0, y = 0}
		
		return
	end
	
	local m_spBg = nil
	if self.m_spSelectType:isVisible() then
		m_spBg = self.m_spSelectType
	elseif self.m_spSysSelect:isVisible() then
		m_spBg = self.m_spSysSelect
		
		local touchHead = false
		local touchSp = nil
		for i,v in pairs(self.m_tabSystemHead) do
			local tmppos = v:convertToNodeSpace(pos)
			local headRect = cc.rect(0, 0, v:getContentSize().width, v:getContentSize().height)
			if true == cc.rectContainsPoint(headRect, tmppos) then
				touchHead = true
				touchSp = v
				break
			end
		end
		
		if true == touchHead and nil ~= touchSp then
			local tag = touchSp:getTag()
			print("touch head " .. tag)
			if nil ~= self.m_parent and nil ~= self.m_parent.sendModifySystemFace then
				self.m_parent:sendModifySystemFace(tag)
			end
			return
		end
	end
	
	--[[
	if nil == m_spBg then
		self:removeFromParent()
		return
	end
	
	pos = m_spBg:convertToNodeSpace(pos)
	local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
	if false == cc.rectContainsPoint(rec, pos) then
		self:removeFromParent()
	end --]]
end

--tableview---MXM修改系统头像
function SelectHeadLayer.cellSizeForTable( view, idx )
	return 940,190
end

function SelectHeadLayer.numberOfCellsInTableView( view )
	---一行5个，10个
	return 2
end

function SelectHeadLayer:tableCellAtIndex( view, idx )
	local cell = view:dequeueCell()
	idx = 1 - idx
	--[[if nil ~= cell and nil ~= cell:getChildByName("head_item_view") then
	cell:removeChildByName("head_item_view", true)
end--]]

if nil == cell then
	cell = cc.TableViewCell:new()
	local item = self:groupSysHead(idx, view)
	item:setPosition(view:getViewSize().width * 0.5, 0)
	item:setName("head_item_view")
	item:setTag(idx)
	cell:addChild(item)
else
	local item = cell:getChildByName("head_item_view")
	item:setTag(idx)
	self:updateCellItem(item, idx, view)
end

return cell
end

local xTable = {-400}
function SelectHeadLayer:groupSysHead( idx, view )
	local item = cc.Node:create()
	local xStart = -400
	local str = ""
	local head = nil
	local frame = nil
	local tag = 0
	for i = 0, 4 do
		head = nil
		tag = idx * 5 + i
		str = string.format("Avatar%d.png", tag)
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
		if nil ~= frame then
			head = cc.Sprite:createWithSpriteFrame(frame)
		end
		
		if nil ~= head then
			item:addChild(head)
			head:setTag(tag)
			local xPos = xStart + i * 190
			head:setPosition(xPos, 10)
			head:setScale(96 / 96)
			head:setName("head_" .. i)
			
			self.m_tabSystemHead[tag] = head
			
			local frame = nil
			if tag == GlobalUserItem.wFaceID then
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_select_bg.png")
			else
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_normal_frame.png")
			end
			if nil ~= frame then
				local spFrame = cc.Sprite:createWithSpriteFrame(frame)
				spFrame:setPosition(xPos, 10)
				item:addChild(spFrame, -1)
				spFrame:setName("frame_" .. i)
			end
		end
	end
	return item
end

function SelectHeadLayer:updateCellItem( item, idx, view )
	local tag = 0
	local frame = nil
	local str = ""
	for i = 0, 4 do
		head = nil
		tag = idx * 5 + i
		frame = nil
		str = ""
		
		local head = item:getChildByName("head_" .. i)
		if nil ~= head then
			str = string.format("Avatar%d.png", tag)
			frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
			if nil ~= frame then
				head:setSpriteFrame(frame)
				head:setTag(tag)
				
				self.m_tabSystemHead[tag] = head
			end
		end
		
		local spFrame = item:getChildByName("frame_" .. i)
		if nil ~= spFrame then
			frame = nil
			if tag == GlobalUserItem.wFaceID then
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_select_bg.png")
			else
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_normal_frame.png")
			end
			
			if nil ~= frame then
				spFrame:setSpriteFrame(frame)
			end
		end
	end
end