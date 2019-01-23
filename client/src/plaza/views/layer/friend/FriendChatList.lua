FriendChatList = class("FriendChatList")

local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local QueryDialog = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
--最大label文字宽度
FriendChatList.MAX_LINE_WIDTH = 320

function FriendChatList:ctor(view, dwUserID, superLayer)
	self.m_ChatLayer = view;
    self.m_Super = superLayer

	self.m_FriendTableView =  nil;
	self.m_ChatTableView =   nil;
	self.m_UserNickLab = nil;
    self.m_dwCurrentFriendID = dwUserID

    self.m_nFriendCount = #FriendMgr:getInstance():getFriendList()
    local curFriend = FriendMgr:getInstance():getFriendByID(dwUserID)
    if nil ~= curFriend then
        self.chatTab = FriendMgr:getInstance():getUserRecordMsg(curFriend.dwUserID);
        self.chatTab = self:sortTable(self.chatTab);

        --设置当前聊天对象id
        if nil ~= self.m_Super.setCurrentChatUser then
            self.m_Super:setCurrentChatUser(curFriend.dwUserID)
        end
    end
    print("FriendChatList currentuser ==> ", curFriend.szNickName)
	self:createChatList(self.m_ChatLayer);

	local LabInput = appdf.getNodeByName(view,"LabInput");
	local EditMsg = appdf.getNodeByName(view,"sendMsgEdit");
	local BtnSendMsg = appdf.getNodeByName(view,"BtnSend");

	if not EditMsg then
        local EditBoxSize = cc.size(LabInput:getContentSize().width+10, LabInput:getContentSize().height+10);
        EditMsg = cc.EditBox:create(EditBoxSize,"Friend/dikuang20.png");
        EditMsg:setFontSize(30)
        EditMsg:setPlaceholderFontSize(30)
        EditMsg:setFontName("fonts/round_body.ttf")
        EditMsg:setFontColor(cc.c3b(206, 211, 255));
        EditMsg:setPlaceHolder("请输入要发送的消息文字");
        EditMsg:setPlaceholderFontName("fonts/round_body.ttf")
        EditMsg:setPlaceholderFontColor(cc.c3b(206, 211, 255));
        EditMsg:setMaxLength(32);
        EditMsg:setReturnType(cc.KEYBOARD_RETURNTYPE_DONE);
        EditMsg:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        EditMsg:setPosition(LabInput:getPosition());
        EditMsg:setName("sendMsgEdit");
        LabInput:getParent():addChild(EditMsg);
    end

 	if LabInput then
        LabInput:removeFromParent();
 	end   

    --发送按钮
	BtnSendMsg:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then 
            local curFriend = FriendMgr:getInstance():getFriendByID( self.m_dwCurrentFriendID )
            if nil == curFriend then
                return
            end

            --判断离线
            if curFriend.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then
                showToast(self.m_ChatLayer, "好友已离线!", 3)
                return
            end
        	local content = EditMsg:getText()
            if string.len(content)<1 then
                showToast(self.m_ChatLayer, "聊天内容不能为空", 3)
        	else
                --敏感词过滤  
                if true == ExternalFun.isContainBadWords(content) then
                    showToast(self.m_ChatLayer, "聊天内容包含敏感词汇!", 3)
                    return
                end             
        		local sendTab = {};
                sendTab.dwUserID = GlobalUserItem.dwUserID
                sendTab.dwTargetUserID = curFriend.dwUserID;
                sendTab.dwFontColor = 0
                sendTab.cbFontSize = 0
                sendTab.cbFontAttri = 0
                sendTab.szFontName = "微软雅黑"
                sendTab.szMessageContent = content;

                FriendMgr:getInstance():sendMessageFriend(sendTab);
                EditMsg:setText("");
            end
	 	end
	 end);
end

function FriendChatList:createChatList(view)	
	local FriendListTopBar = appdf.getNodeByName(view,"Image_3");
	local FriendChatListTopBar = appdf.getNodeByName(view,"Image_5");

	local FriendListSize = FriendListTopBar:getContentSize();
	local ChatListSize = FriendChatListTopBar:getContentSize();
	local ChatLayerSize = view:getContentSize();

	self.m_UserNickLab = appdf.getNodeByName(view,"LabNick");
    local curFriend = FriendMgr:getInstance():getFriendByID( self.m_dwCurrentFriendID )
    local nickname = ""
    if nil ~= curFriend then
        nickname = curFriend.szNickName
    end
    self.m_UserNickLab:setString(nickname)	

	--好友列表
	if not self.m_FriendTableView  then
		self.m_FriendTableView = cc.TableView:create(cc.size(FriendListSize.width, ChatLayerSize.height-125));
		self.m_FriendTableView:setColor(cc.c3b(158, 200, 200))
	 		self.m_FriendTableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
			self.m_FriendTableView:setPosition(cc.p(FriendListTopBar:getPositionX()-FriendListSize.width/2-5,30));
		self.m_FriendTableView:setDelegate()
		
		view:addChild(self.m_FriendTableView)
		self.m_FriendTableView:registerScriptHandler(handler(self,self.tableCellTouched),cc.TABLECELL_TOUCHED)
		self.m_FriendTableView:registerScriptHandler(handler(self,self.cellSizeForTable),cc.TABLECELL_SIZE_FOR_INDEX)
		self.m_FriendTableView:registerScriptHandler(handler(self,self.tableCellAtIndex),cc.TABLECELL_SIZE_AT_INDEX)
		self.m_FriendTableView:registerScriptHandler(handler(self,self.numberOfCellsInTableView),cc.NUMBER_OF_CELLS_IN_TABLEVIEW);
	end

	--当前好友聊天列表
	if not self.m_ChatTableView  then
		self.m_ChatTableView = cc.TableView:create(cc.size(ChatListSize.width, ChatLayerSize.height-200));
		self.m_ChatTableView:setColor(cc.c3b(255, 255, 255))
 		self.m_ChatTableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
		self.m_ChatTableView:setPosition(cc.p(FriendChatListTopBar:getPositionX()-ChatListSize.width/2,110));
		self.m_ChatTableView:setDelegate()
		view:addChild(self.m_ChatTableView)
		self.m_ChatTableView:registerScriptHandler(handler(self, self.tableCellTouched),cc.TABLECELL_TOUCHED)
		self.m_ChatTableView:registerScriptHandler(handler(self, self.cellSizeForTable),cc.TABLECELL_SIZE_FOR_INDEX)
		self.m_ChatTableView:registerScriptHandler(handler(self, self.tableCellAtIndex),cc.TABLECELL_SIZE_AT_INDEX)
		self.m_ChatTableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView),cc.NUMBER_OF_CELLS_IN_TABLEVIEW);
	end

    self.m_FriendTableView:reloadData()
	self.m_ChatTableView:reloadData()
	self:moveToLastRow()
    self:enableChatListTouch()
end

function FriendChatList:tableCellTouched(table,cell)
    if table == self.m_FriendTableView  then
        self.m_dwCurrentFriendID = cell:getTag()
        local curFriend = FriendMgr:getInstance():getFriendByID( self.m_dwCurrentFriendID )
        if nil == curFriend then
            return
        end
        print("FriendChatList:tableCellTouched ==> ", curFriend.szNickName)
        local dwUserID = curFriend.dwUserID
        self.m_dwCurrentFriendID = dwUserID
        
        --设置当前聊天对象id
        if nil ~= self.m_Super.setCurrentChatUser then
            self.m_Super:setCurrentChatUser(dwUserID)
        end
        --隐藏通知
        local node = self:getNotifyNode(dwUserID)
        if nil ~= node then
            NotifyMgr:getInstance():hideNotify(node, true)
        end

        self.chatTab = FriendMgr:getInstance():getUserRecordMsg(dwUserID);
        self.chatTab = self:sortTable(self.chatTab);

        self.m_UserNickLab:setString(curFriend.szNickName);
        self.m_FriendTableView:reloadData()
        self.m_ChatTableView:reloadData()
        self:moveToLastRow()
        self:enableChatListTouch()
	end
end

function FriendChatList:cellSizeForTable(table,idx) 
	local FriendListTopBar = appdf.getNodeByName(self.m_ChatLayer,"Image_3");
	local FriendChatListTopBar = appdf.getNodeByName(self.m_ChatLayer,"Image_5");
	local FriendListSize = FriendListTopBar:getContentSize();
	
	if table == self.m_FriendTableView then
 		return FriendListSize.width,68;
	elseif table == self.m_ChatTableView  then
		local ChatListSize = FriendChatListTopBar:getContentSize();
		local curUserTab = self.chatTab[idx+1];
        if true == curUserTab.bImage then
            return 220, 130
        else
            local labelSize =  self:calculationLableSize(curUserTab.szMessageContent)
            if labelSize.height < 30 then
                labelSize.height = 40;
            end
            return ChatListSize.width,labelSize.height+30;
        end
	end
end

function FriendChatList:tableCellAtIndex(table, idx)
    local cell = table:dequeueCell()

	if table == self.m_FriendTableView  then
		local curUserTab = FriendMgr:getInstance():getFriendList()[self.m_nFriendCount - idx]
        if nil == curUserTab then
            return cc.TableViewCell:new()
        end

        local defaultCell = nil
		if nil == cell then
       	 	cell = cc.TableViewCell:new()

        	defaultCell = cc.Sprite:create("Friend/dikuang10.png");
        	defaultCell:setName("defaultCell");
        	defaultCell:setAnchorPoint(cc.p(0,0));
        	defaultCell:setPosition(cc.p(0, 0));
        	cell:addChild(defaultCell);

        	local defaultCellSize = defaultCell:getContentSize();

        	--头像
            local head = PopupInfoHead:createClipHead(curUserTab, 56)
            head:setPosition(cc.p(100,defaultCellSize.height/2))
            --根据会员等级确定头像框
            head:enableHeadFrame(false)
            head:setName("head")
            defaultCell:addChild(head)

        	--昵称
        	local nickName = cc.LabelTTF:create(curUserTab.szNickName,"fonts/round_body.ttf",20,cc.size(130,40),cc.TEXT_ALIGNMENT_LEFT);
         	nickName:setName("nickName");
         	nickName:setFontFillColor(cc.c3b(41,82,146));
         	nickName:setAnchorPoint(cc.p(0,0.5));
         	nickName:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
			nickName:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
         	nickName:setPosition(cc.p(135,defaultCellSize.height/2));
         	defaultCell:addChild(nickName);
	
         	--指示
         	local  spUser = cc.Sprite:create("Friend/dikuang17.png");
         	spUser:setName("spUser");
         	spUser:setAnchorPoint(cc.p(1,0.5));
         	spUser:setPosition(cc.p(defaultCellSize.width,defaultCellSize.height/2));
         	spUser:setVisible(self.m_dwCurrentFriendID == curUserTab.dwUserID or false);
         	defaultCell:addChild(spUser);
	
         	local start = cc.Sprite:create("Friend/tutiao3.png");
         	start:setPosition(cc.p(20,defaultCellSize.height/2));
         	defaultCell:addChild(start);
	
         	local lv = cc.Sprite:create("Friend/biaoti3.png");
         	lv:setPosition(cc.p(50,defaultCellSize.height/2));
         	defaultCell:addChild(lv);

  			local userStatus = nil         
         	if curUserTab.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then
         		userStatus = cc.Sprite:create("Friend/biaoti5.png")
         	else
         		userStatus = cc.Sprite:create("Friend/biaoti4.png")
         	end
            if nil ~= userStatus then
                userStatus:setName("userStatus");
                userStatus:setAnchorPoint(cc.p(1,0.5))
                userStatus:setPosition(cc.p(defaultCellSize.width-10,defaultCellSize.height/2));
                defaultCell:addChild(userStatus);
            end         	
    	else
        	defaultCell  = cell:getChildByName("defaultCell");
    		local nickName  = defaultCell:getChildByName("nickName");
    		local head  = defaultCell:getChildByName("head");
    		local userStatus = defaultCell:getChildByName("userStatus");
    		local spUser =  defaultCell:getChildByName("spUser");
    		spUser:setVisible(self.m_dwCurrentFriendID == curUserTab.dwUserID or false);
    		nickName:setString(curUserTab.szNickName)
    		head:updateHead(curUserTab);

         	if curUserTab.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then 
         		local sp = cc.Sprite:create("Friend/biaoti5.png")
        		userStatus:setSpriteFrame(sp:getSpriteFrame());
         	else
         		local sp = cc.Sprite:create("Friend/biaoti4.png")
        		userStatus:setSpriteFrame(sp:getSpriteFrame());
         	end    		
    	end
        cell:setTag(curUserTab.dwUserID)
        --获取未读通知列表
        local unread = NotifyMgr:getInstance():getUnreadNotifyList()
        --隐藏红点
        NotifyMgr:getInstance():hideNotify(defaultCell, false)
        --通知红点
        for k,v in pairs(unread) do
            if type(v.param) == "table" then
                local sendUser = v.param.dwSenderID
                if nil ~= sendUser and sendUser == curUserTab.dwUserID then                
                    NotifyMgr:getInstance():showNotify(defaultCell, v)
                    break
                end
            end
        end
	elseif table == self.m_ChatTableView then
		local curUserTab = self.chatTab[idx+1]
        if not cell then        
            cell = cc.TableViewCell:new()
        else
            cell:removeAllChildren()
        end
        local defaultCell = nil
        if curUserTab.dwSenderID==GlobalUserItem.dwUserID then
            defaultCell = cc.Scale9Sprite:create("Friend/dikuang16.png",cc.rect(0,0,200,56),cc.rect(20,20,100,5));
        else
            defaultCell = cc.Scale9Sprite:create("Friend/dikuang15.png",cc.rect(0,0,200,56),cc.rect(40,20,100,5));
        end

        defaultCell:setAnchorPoint(cc.p(0,0));
        defaultCell:setPosition(cc.p(0, 0));
        defaultCell:setName("defaultCell");
        cell:addChild(defaultCell);

        --头像
        local head = PopupInfoHead:createClipHead(curUserTab, 56)
        --根据会员等级确定头像框
        head:enableHeadFrame(false)
        head:setName("head")
        cell:addChild(head)

        -- 图片内容
        if true == curUserTab.bImage then
            if cc.FileUtils:getInstance():isFileExist(curUserTab.szMessageContent) then
                local image = ccui.ImageView:create(curUserTab.szMessageContent)
                image:setScale9Enabled(true)
                image:setName("share_image")
                image:setContentSize(cc.size(200, 100))
                defaultCell:addChild(image)
                image:setTouchEnabled(true)
                --image:setSwallowTouches(false)
                image:addTouchEventListener(function(ref, tType)
                    if tType == ccui.TouchEventType.ended then
                        local runScene = cc.Director:getInstance():getRunningScene()
                        if nil ~= runScene then
                            local mask = ccui.Layout:create() --cc.LayerColor:create(cc.c4b(0, 0, 0, 200))
                            mask:setContentSize(yl.WIDTH,yl.HEIGHT)
                            mask:setBackGroundColor(cc.c3b(0, 0, 0))
                            mask:setBackGroundColorOpacity(200)
                            mask:setBackGroundColorType(LAYOUT_COLOR_SOLID)
                            local scaleImage = ccui.ImageView:create(curUserTab.szMessageContent)
                            scaleImage:setScale(0.000001)
                            scaleImage:setScale9Enabled(true)
                            --scaleImage:setTouchEnabled(true)
                            --scaleImage:setContentSize(1335,750)                         
                            mask:addTouchEventListener(function(ref, tType)
                                    if tType == ccui.TouchEventType.ended then
                                        scaleImage:stopAllActions()
                                        scaleImage:runAction(cc.Sequence:create(cc.ScaleTo:create(0.1, 0.01), cc.CallFunc:create(function()
                                            mask:removeFromParent()
                                        end)))
                                    end
                                end
                            )
                            scaleImage:setPosition(cc.p(1335 * 0.5, 750 * 0.5))
                            mask:addChild(scaleImage)
                            runScene:addChild(mask)
                            scaleImage:stopAllActions()
                            local conSize = scaleImage:getContentSize()
                            scaleImage:runAction(cc.Sequence:create(cc.ScaleTo:create(0.1, 700 / conSize.height), cc.CallFunc:create(function()
                                mask:setTouchEnabled(true)
                            end)))
                        end
                    end
                end)
                if curUserTab.dwSenderID == GlobalUserItem.dwUserID then
                    image:setPosition(cc.p(107, 55))
                else
                    image:setPosition(cc.p(113, 55))
                end
            else
                local mask = cc.LayerColor:create(cc.c4b(0, 0, 0, 125), 200, 100)
                mask:ignoreAnchorPointForPosition(false)
                mask:setAnchorPoint(cc.p(0.5, 0.5))
                
                if curUserTab.dwSenderID == GlobalUserItem.dwUserID then
                    mask:setPosition(cc.p(107, 55))
                else
                    mask:setPosition(cc.p(113, 55))
                end
                defaultCell:addChild(mask)
                -- 旋转
                local cycle = cc.Sprite:create("GameList/spinner_circle.png")
                if nil ~= cycle then
                    cycle:setPosition(cc.p(113, 55))
                    defaultCell:addChild(cycle)
                    cycle:stopAllActions()
                    cycle:runAction(cc.Sequence:create(cc.Repeat:create(cc.RotateBy:create(1.0, 360), 10), cc.RemoveSelf:create(true)))
                end       
            end
            defaultCell:setContentSize(220, 110)
        else
            local labelSize = self:calculationLableSize(curUserTab.szMessageContent);
            local content = cc.LabelTTF:create("1","fonts/round_body.ttf",20);
            content:setDimensions(cc.size(FriendChatList.MAX_LINE_WIDTH,0));
            content:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
            content:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
            content:setName("content");             
            defaultCell:addChild(content);            
            content:setString(curUserTab.szMessageContent);
            defaultCell:setContentSize(cc.size(labelSize.width+20, labelSize.height+20));
            content:setPosition(cc.p(defaultCell:getContentSize().width/2,defaultCell:getContentSize().height/2));
        end         
        local tmpHeight = defaultCell:getContentSize().height > 56 and defaultCell:getContentSize().height or 56
        if curUserTab.dwSenderID==GlobalUserItem.dwUserID then
            head:setPosition(cc.p(860,0));
            defaultCell:setAnchorPoint(cc.p(1,0));
            defaultCell:setPosition(cc.p(825,0));               
        else
            head:setPosition(cc.p(30,0))
            defaultCell:setAnchorPoint(cc.p(0,0));
            defaultCell:setPosition(cc.p(70,0));
        end
        head:setPositionY(tmpHeight * 0.5)
	end
    return cell
end

function FriendChatList:numberOfCellsInTableView(table)
	if table == self.m_FriendTableView  then
 		return #FriendMgr:getInstance():getFriendList()
	elseif  table == self.m_ChatTableView  then
		return #self.chatTab;
	end
end

function FriendChatList:removeTableView()
	self.m_FriendTableView:removeFromParent();
    self.m_ChatTableView:removeFromParent();
end

function FriendChatList:refreshChatList()
    self.m_nFriendCount = #FriendMgr:getInstance():getFriendList()
    local friendlist = FriendMgr:getInstance():getFriendList()
    if nil == FriendMgr:getInstance():getFriendByID( self.m_dwCurrentFriendID ) then
        if self.m_nFriendCount > 0 then
            self.m_dwCurrentFriendID = friendlist[1].dwUserID
        else
            self.m_dwCurrentFriendID = nil
        end
    end

    if nil ~= self.m_FriendTableView then
        self.m_FriendTableView:reloadData()
    end
    
    if nil ~= self.m_ChatTableView then
        self.m_ChatTableView:reloadData()
        self:moveToLastRow()
        self:enableChatListTouch()
    end
end

function FriendChatList:calculationLableSize(content)
    local label = cc.LabelTTF:create("1","Arial",20);
    label:setDimensions(cc.size(FriendChatList.MAX_LINE_WIDTH,0));
    label:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
    label:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
    label:setString(content);

    local contentSize = label:getContentSize();
    return contentSize;
end

--新消息通知
function FriendChatList:messageNotify(msgTab)
    local curFriend = FriendMgr:getInstance():getFriendByID( self.m_dwCurrentFriendID )
    if nil == curFriend then
        return 
    end
	self.chatTab = FriendMgr:getInstance():getUserRecordMsg(curFriend.dwUserID);

	self.chatTab = self:sortTable(self.chatTab)

	self.m_ChatTableView:reloadData()
	self:moveToLastRow()
    self:enableChatListTouch()
end

--聊天table滚动到最后一行
function FriendChatList:moveToLastRow()
	self.m_ChatTableView:setContentOffset(cc.p(0,0),false);
end

--倒叙输出Table
function FriendChatList:sortTable(table)
	local sortTab = {}
	if #table >0 then
		for i=1, #table do		
			sortTab[i] = table[#table-i+1]
		end
		return sortTab
	end
	return {}
end

--获取通知红点显示node
function FriendChatList:getNotifyNode(dwUserID)
    local cell = self:getFriendCell(dwUserID)
    if nil ~= cell then
        return cell:getChildByName("defaultCell")
    end
    return nil
end

--好友cell
function FriendChatList:getFriendCell(dwUserID)
    local container = self.m_FriendTableView:getContainer()
    if nil ~= container then
        return container:getChildByTag(dwUserID)
    end
end

--刷新好友cell
function FriendChatList:refreshFriendState(userInfo)
    local cell = self:getFriendCell(userInfo.dwUserID)
    if nil ~= cell then
        local defaultCell  = cell:getChildByName("defaultCell")
        if nil == defaultCell then
            return
        end        
        local userStatus = defaultCell:getChildByName("userStatus")
        if nil == userStatus then
            return
        end

        if userInfo.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then 
            local sp = cc.Sprite:create("Friend/biaoti5.png")
            userStatus:setSpriteFrame(sp:getSpriteFrame())
        else
            local sp = cc.Sprite:create("Friend/biaoti4.png")
            userStatus:setSpriteFrame(sp:getSpriteFrame())
        end  
    end
end

--
function FriendChatList:enableChatListTouch()
    local container = self.m_ChatTableView:getContainer()
    if nil ~= container then
        self.m_ChatTableView:setTouchEnabled(container:getContentSize().height >= self.m_ChatTableView:getViewSize().height)
    end
end

return FriendChatList