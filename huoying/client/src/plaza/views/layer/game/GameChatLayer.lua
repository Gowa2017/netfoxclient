--
-- Author: zhong
-- Date: 2016-08-12 14:35:14
--
--通用聊天层

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()

--聊天展现层
local GameChatLayer = class("GameChatLayer", cc.Layer)
--聊天内容
local ChatItem = class("ChatItem", cc.Node)
--表情面板
local BrowPanel = class("BrowPanel", cc.Node)

--关闭
local BT_CLOSE = 101
--发送聊天
local BT_SEND = 102
--文本聊天
local CBT_TEXT = 201
--表情聊天
local CBT_BROW = 202
--聊天记录
local CBT_RECORD = 203

local CHAT_ITEM_NAME = "game_chat_item"
local CHAT_BROW_PANEL = "game_brow_panel"

local GAMECHAT_LISTENER = "gamechat_listener"

--表情tableview宽度
local LEN_BROWVIEW_WIDTH = 480
--tableview宽度
local LEN_TABLEVIEW_WIDTH = 580
--聊天文本大小
local SIZE_CHAT_LABEL = 25
--聊天内容宽度
local LEN_CHAT_WIDTH = 580

--聊天记录
local chatRecord = {}
--文本聊天内容
local textChatConfig = {}

function GameChatLayer.getRecordConfig()
	return 
	{
		bBrow = false,
		strChat = "",
		nIdx = 0,
		szNick = ""
	}
end

function GameChatLayer.loadTextChat()
	local str = cc.FileUtils:getInstance():getStringFromFile("public/chat_text.json")
	local ok, datatable = pcall(function()
			       return cjson.decode(str)
			    end)
	if true == ok and type(datatable) == "table" then
		for k,v in pairs(datatable) do
			local record = {}
			record.bBrow = false
			record.strChat = v
			textChatConfig[k] = record
		end
	else
		print("load text chat error!")
	end

	--[[cc.SpriteFrameCache:getInstance():addSpriteFrames("public/brow.plist")

	--缓存表情
	local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/brow.plist")
	local framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame then
				frame:retain()
			end
		end
	end]]
end
GameChatLayer.loadTextChat()

-- 默认短语对比
function GameChatLayer.compareWithText(str)
    for k,v in pairs(textChatConfig) do
        if v.strChat == str then
            return (k - 1)
        end
    end
    return nil
end

function GameChatLayer:ctor(netframe, csbfile)
	if nil == netframe then
		print("net frame can't be nil")
		return
	end
	self.m_netframe = netframe

	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	csbfile = csbfile or "public/GameChatLayer.csb"
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB(csbfile, self)
	local chat_bg = csbNode:getChildByName("chat_bg")
	self.m_spChatBg = chat_bg

	--内容区域
	self.m_spChatArea = chat_bg:getChildByName("chat_area")

	local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    --关闭按钮
    local btn = chat_bg:getChildByName("btn_close")
    btn:setTag(BT_CLOSE)
    btn:addTouchEventListener(btncallback)

    --发送按钮
    btn = chat_bg:getChildByName("btn_send")
    btn:setTag(BT_SEND)
    btn:addTouchEventListener(btncallback)

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    --编辑框
	local tmp = chat_bg:getChildByName("edit_frame")
	local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
        :setFontColor(cc.c3b(0, 0, 0))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入聊天信息")
	chat_bg:addChild(editbox)
	self.m_editChat = editbox
    self.m_editChat:registerScriptEditBoxHandler(editHanlder)
    self.m_bEditChat = false

	local checkEvent = function (sender,eventType)
    	self:onCheckBoxClickEvent(sender,eventType)
    end

    self.m_nSelect = CBT_TEXT
    --文本聊天
    local cbt = chat_bg:getChildByName("text_check")
    cbt:setTag(CBT_TEXT)
    cbt:addEventListener(checkEvent)
    cbt:setSelected(true)
    self.m_checkText = cbt
    self.m_spChatArea:setVisible(false)

    --历史记录
    cbt = chat_bg:getChildByName("record_check")
    cbt:setTag(CBT_RECORD)
    cbt:addEventListener(checkEvent)
    cbt:setSelected(false)
    self.m_checkRecord = cbt   

    --内容
    self.m_textTableView = nil
    self.m_browTableView = nil
    self.m_recordTableView = nil

    --默认显示
    self:showTextChat(true)

    self:setVisible(false)

    --ios设置父节点缩放值，会导致editbox显示异常
    if targetPlatform == cc.PLATFORM_OS_ANDROID then
    	self.m_spChatBg:setScale(0.000001)
    end
    --动作
    local call = cc.CallFunc:create(function( )
    	self:setVisible(true)
    	if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
    		self.m_recordTableView:reloadData()
            self:moveRecordToLastRow()
    	end
    end)
    
    local sca = cc.ScaleTo:create(0.2, 1.0)
    local call1 = cc.CallFunc:create(function()    	
    	if targetPlatform ~= cc.PLATFORM_OS_ANDROID then    		
    		self.m_editChat:setVisible(true)
    	end  
    	self.m_editChat:setEnabled(true)	
    end)
    self.m_actShow = cc.Sequence:create(call, sca, call1)
    ExternalFun.SAFE_RETAIN(self.m_actShow)

    local call2 = cc.CallFunc:create(function (  )    	
    	if targetPlatform ~= cc.PLATFORM_OS_ANDROID then
    		self.m_editChat:setVisible(false)
    	end
    	self.m_editChat:setEnabled(false)
    end)
    local sca2 = cc.ScaleTo:create(0.2, 0.000001)
    local call3 = cc.CallFunc:create(function (  )
    	self:setVisible(false)
    end)
    self.m_actHide = cc.Sequence:create(call2, sca2, call3)
    ExternalFun.SAFE_RETAIN(self.m_actHide)

    self.m_listener = cc.EventListenerCustom:create(GAMECHAT_LISTENER,handler(self, function(target,event)
	    	if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
	    		self.m_recordTableView:reloadData()
                self:moveRecordToLastRow()
	    	end
    	end))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function GameChatLayer:showGameChat( bShow )
	--防作弊不聊天
	if GlobalUserItem.isAntiCheat() then
		local runScene = cc.Director:getInstance():getRunningScene()
		showToast(runScene, "防作弊房间禁止聊天", 3)
		return
	end

	local ani = nil
	if bShow then
		ani = self.m_actShow
	else 
		ani = self.m_actHide
	end

	if nil ~= ani then
		self.m_spChatBg:stopAllActions()
		if targetPlatform == cc.PLATFORM_OS_ANDROID then
    		self.m_spChatBg:runAction(ani)
    	else
    		self:setVisible(bShow)
            if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
                self.m_recordTableView:reloadData()
                self:moveRecordToLastRow()
            end
    	end 		
	end
end

function GameChatLayer:showTextChat(bShow)
	if bShow then
		if nil == self.m_textTableView then
			self.m_textTableView = self:getDataTableView(cc.size(LEN_TABLEVIEW_WIDTH, 240))
			self.m_spChatBg:addChild(self.m_textTableView)
		end
		self.m_textTableView:reloadData()
	end

	if nil ~= self.m_textTableView then
		self.m_textTableView:setVisible(bShow)
	end
end

function GameChatLayer:showRecord(bShow)
	if bShow then
		if nil == self.m_recordTableView then
			self.m_recordTableView = self:getDataTableView(cc.size(LEN_TABLEVIEW_WIDTH, 240))
			self.m_spChatBg:addChild(self.m_recordTableView)
		end
		self.m_recordTableView:reloadData()
        self:moveRecordToLastRow()
	end

	if nil ~= self.m_recordTableView then
		self.m_recordTableView:setVisible(bShow)
	end
end

function GameChatLayer:moveRecordToLastRow()    
    local container = self.m_recordTableView:getContainer()
    local needToLastRow = false
    if nil ~= container then
        needToLastRow = (container:getContentSize().height >= self.m_recordTableView:getViewSize().height)
        --self.m_recordTableView:setTouchEnabled(needToLastRow)
    end
    if needToLastRow then
        self.m_recordTableView:setContentOffset(cc.p(0,0),false)
    end
end

function GameChatLayer:getDataTableView(size)
	--tableview
	local m_tableView = cc.TableView:create(size)
	m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
	m_tableView:setPosition(cc.p(45,110))
	m_tableView:setDelegate()
	m_tableView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
	m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	m_tableView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
	m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
	return m_tableView
end

function GameChatLayer:onButtonClickedEvent( tag, sender )
	if BT_CLOSE == tag then
		self:showGameChat(false)
	elseif BT_SEND == tag then
		local chatstr = self.m_editChat:getText()
		chatstr = string.gsub(chatstr, " " , "")
        if ExternalFun.stringLen(chatstr) > 128 then
            showToast(self, "聊天内容过长", 2)
            return
        end
        --判断emoji
        if ExternalFun.isContainEmoji(chatstr) then
            showToast(self, "聊天内容包含非法字符,请重试", 2)
            return
        end

        --敏感词过滤  
        if true == ExternalFun.isContainBadWords(chatstr) then
            showToast(self, "聊天内容包含敏感词汇!", 2)
            return
        end

		if "" ~= chatstr then
			local valid, msg = self:sendTextChat(chatstr)
			if false == valid and type(msg) == "string" and "" ~= msg then
				showToast(self, msg, 2)
			else
				self.m_editChat:setText("")
			end
		end
	end
end

function GameChatLayer:onCheckBoxClickEvent(sender, eventType)
	if nil == sender then
		return
	end

	local tag = sender:getTag()
	if self.m_nSelect == tag then
		sender:setSelected(true)
		return
	end
	self.m_nSelect = tag
	self.m_editChat:setText("")

	local bShowText = false
	local bShowRecord = false
	if CBT_TEXT == tag then
		bShowText = true
		bShowRecord = false
	elseif CBT_RECORD == tag then
		bShowText = false
		bShowRecord = true
	end

	self.m_checkText:setSelected(bShowText)
	self.m_checkRecord:setSelected(bShowRecord)
	self:showTextChat(bShowText)
	self:showRecord(bShowRecord)
    self.m_spChatArea:setVisible(bShowRecord)
end

function GameChatLayer:onEnterTransitionFinish()
	
end

function GameChatLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditChat = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditChat = string.len(src) ~= 0
    end
end

function GameChatLayer:onExit()
	chatRecord = {}

	ExternalFun.SAFE_RELEASE(self.m_actShow)
	self.m_actShow = nil
    ExternalFun.SAFE_RELEASE(self.m_actHide)
    self.m_actHide = nil

    if nil ~= self.m_listener then
    	cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
    end
end

function GameChatLayer:onTouchBegan(touch, event)
	return self:isVisible()
end

function GameChatLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spChatBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) and false == self.m_bEditChat then
        self:showGameChat(false)
    end

    if true == self.m_bEditChat then
        self.m_bEditChat = false
    end
end

function GameChatLayer:isTouchBrowEnable()
	return (CBT_BROW == self.m_nSelect)
end

--发送文本聊
function GameChatLayer:sendTextChat(msg)
	if nil ~= self.m_netframe and nil ~= self.m_netframe.sendTextChat then
		return self.m_netframe:sendTextChat(msg)
	end
	return false, ""
end

--发送表情聊天
function GameChatLayer:sendBrowChat(idx)
	if nil ~= self.m_netframe and nil ~= self.m_netframe.sendBrowChat then
		return self.m_netframe:sendBrowChat(idx)
	end
	return false, ""
end

--添加聊天记录
function GameChatLayer.addChatRecord(rec)
	rec = rec or GameChatLayer.getRecordConfig()
	table.insert(chatRecord, 1, rec)

	--通知
	local eventListener = cc.EventCustom:new(GAMECHAT_LISTENER)
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function GameChatLayer.addChatRecordWith(cmdtable, bBrow)
	local rec = GameChatLayer.getRecordConfig()
	rec.bBrow = bBrow
	rec.szNick = cmdtable.szNick
	if bBrow then
		rec.nIdx = cmdtable.wItemIndex
	else
		rec.strChat = cmdtable.szChatString
	end

	GameChatLayer.addChatRecord(rec)
end

--tableview
function GameChatLayer:cellSizeForTable( view, idx )
	if CBT_TEXT == self.m_nSelect then
		local record = textChatConfig[idx + 1]
		if nil ~= record then
			return 580, 60--self:getTextChatSize(record.strChat)
		end
		return 0,0
	elseif CBT_RECORD == self.m_nSelect then
		local record = chatRecord[idx + 1]
		if nil ~= record then
			if true == record.bBrow then
				return LEN_TABLEVIEW_WIDTH,80
			else
				local wi, he, si = self:getTextChatSize(record.strChat)
				return wi, he + si
			end
		end
		return 0,0
	end
	return 0,0
end

function GameChatLayer:getTextChatSize(str)
	local tmp = cc.LabelTTF:create(str, "fonts/round_body.ttf", SIZE_CHAT_LABEL, cc.size(LEN_CHAT_WIDTH,0))
	local tmpsize = tmp:getContentSize()
	tmp:setString("网")
	return LEN_TABLEVIEW_WIDTH, tmpsize.height + 15, tmp:getContentSize().height
end

function GameChatLayer:numberOfCellsInTableView( view )
	if CBT_TEXT == self.m_nSelect then
		return #textChatConfig
	elseif CBT_RECORD == self.m_nSelect then			
		return #chatRecord
	end
	return 0
end

function GameChatLayer:tableCellAtIndex(view, idx)	
	local cell = view:dequeueCell()
	local dataitem = nil

	if nil == cell then
		cell = cc.TableViewCell:new()
		dataitem = self:getDataItemAt(view,idx)
		if nil ~= dataitem then
			dataitem:setName(CHAT_ITEM_NAME)
			cell:addChild(dataitem)
		end
	else
		dataitem = cell:getChildByName(CHAT_ITEM_NAME)
	end
	if nil ~= dataitem then
		self:refreshDataItemAt(view, dataitem, idx)
	end

	cell:setTag(idx)
	return cell
end

function GameChatLayer:tableCellTouched(view, cell)
	if CBT_TEXT == self.m_nSelect then
		local idx = cell:getIdx() + 1
		if nil ~= textChatConfig[idx] then
			self:sendTextChat(textChatConfig[idx].strChat)
		end		
	end
end

local idxTable = {}
function GameChatLayer:getDataItemAt(view, idx)
	if CBT_TEXT == self.m_nSelect  then
        local chatitem = ChatItem:create(false)
        chatitem:setName(CHAT_ITEM_NAME)
        return chatitem
    elseif CBT_RECORD == self.m_nSelect then
		local chatitem = ChatItem:create(true)
		chatitem:setName(CHAT_ITEM_NAME)
		return chatitem
	end
	return nil
end

function GameChatLayer:refreshDataItemAt(view, item, idx)	
	if nil == item then
		return 
	end

	if CBT_TEXT == self.m_nSelect then
		local record = textChatConfig[idx + 1]
		if nil ~= record then
			item:refreshTextItem(record.strChat)
		end			
	elseif CBT_RECORD == self.m_nSelect then
		local record = chatRecord[idx + 1]
		if nil ~= record then
			item:refreshRecordItem(record.strChat, record.szNick)
		end
	end

	local size = item:getContentSize()
	item:setPosition(size.width * 0.5, 0)
end

function ChatItem:ctor( bRecord )
    if not bRecord then
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("chat_item_framebg.png")
        if nil ~= frame then
            self.m_spLine = cc.Sprite:createWithSpriteFrame(frame)
            self.m_spLine:setPosition(0, self.m_spLine:getContentSize().height * 0.5)
            self:addChild(self.m_spLine)
        end
    end

	self.m_labelChat = nil
	self.m_clipUserName = nil
end

function ChatItem:refreshTextItem(str,sendusernick)
	if nil == self.m_labelChat then
		self.m_labelChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", SIZE_CHAT_LABEL, cc.size(LEN_CHAT_WIDTH,0), cc.TEXT_ALIGNMENT_LEFT)
		self.m_labelChat:setPosition(2, self.m_spLine:getContentSize().height * 0.5)

		self:addChild(self.m_labelChat)
	else
		self.m_labelChat:setString(str)
	end
	local labSize = self.m_labelChat:getContentSize()
	self:setContentSize(cc.size(LEN_TABLEVIEW_WIDTH, labSize.height + 15))
end

function ChatItem:refreshRecordItem(str,sendusernick)
    if nil == self.m_labelChat then
        self.m_labelChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", 20, cc.size(LEN_CHAT_WIDTH,0), cc.TEXT_ALIGNMENT_LEFT)
        self.m_labelChat:setAnchorPoint(cc.p(0.5, 0))
        self.m_labelChat:setPositionY(5)

        self:addChild(self.m_labelChat)
    else
        self.m_labelChat:setString(str)
    end
    local labSize = self.m_labelChat:getContentSize()
    self:addSendUser(labSize.height + 10, sendusernick)
    if nil ~= sendusernick and type(sendusernick) == "string" then
        labSize.height = labSize.height + self.m_clipUserName:getContentSize().height + 5
    end
    self:setContentSize(cc.size(LEN_TABLEVIEW_WIDTH, labSize.height + 15))
end

function ChatItem:addSendUser(posHeight,sendusernick)
	if nil ~= sendusernick and type(sendusernick) == "string" then
		if nil == self.m_clipUserName then
			self.m_clipUserName = ClipText:createClipText(cc.size(200,20), sendusernick .. ":", "fonts/round_body.ttf", 20)
			self.m_clipUserName:setTextColor(cc.c3b(36, 236, 255))
			self.m_clipUserName:setAnchorPoint(cc.p(0, 0.5))
			self.m_clipUserName:setPositionX(-LEN_CHAT_WIDTH * 0.5)
			self:addChild(self.m_clipUserName)
		else
			self.m_clipUserName:setString(sendusernick .. ":")
		end

		self.m_clipUserName:setPositionY(posHeight + self.m_clipUserName:getContentSize().height * 0.5)		
	else
		if nil ~= self.m_clipUserName then
			self.m_clipUserName:setVisible(false)
		end
	end
end

return GameChatLayer