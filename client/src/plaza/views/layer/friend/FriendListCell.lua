
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local ClipText = require(appdf.EXTERNAL_SRC .. "ClipText")

local   FriendListCell = class("FriendListCell", function()
	local cellSprite =  cc.Sprite:create("Friend/dikuang5.png");
    return cellSprite
end);

function FriendListCell:ctor(dwID,superView)
    self:setAnchorPoint(cc.p(0,0))
    self:setPosition(cc.p(0, 0))
    self.m_StartPos = cc.p(0,0);--触摸起始点
    self.m_MoveDistance = 30;
    self.m_Super =  superView;
    self.m_SuperView = superView.Layer;
    --聊天按钮
    self.m_btnChat = nil
    --用户状态
    self.m_clipUserStatus = nil
    local userInfo = FriendMgr:getInstance():getFriendByID(dwID)
    if nil == userInfo then
        return
    end

    local deleBtn = ccui.Button:create("Friend/anniu11.png","Friend/anniu11.png", "Friend/anniu11.png");
    deleBtn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then   
            local friendid = userInfo.dwUserID
            print("==============删除好友============== ", userInfo.szNickName, friendid)
            FriendMgr:getInstance():sendDeleteFriend(friendid, 0)
        end
    end);
    deleBtn:setAnchorPoint(cc.p(1,0.5));
    deleBtn:setPosition(cc.p(self:getContentSize().width,self:getContentSize().height/2))
    self.m_btnDele = deleBtn
    self.m_btnDele:setEnabled(false)
    self:addChild(deleBtn);

    self.m_ContentSprite =  cc.Sprite:create("Friend/dikuang5.png");
    self.m_ContentSprite:setAnchorPoint(cc.p(0,0))
    self.m_ContentSprite:setPosition(cc.p(0, 0))
    self:addChild(self.m_ContentSprite);

    self:createCell(userInfo,index,self.m_ContentSprite,self.m_SuperView);
    local function onTouchBegan(touch,event)    	
    	local locationInNode = self:convertToNodeSpace(touch:getLocation());
    	local size = self:getContentSize();
    	local rect = cc.rect(0,0,size.width,size.height);
    	if cc.rectContainsPoint(rect,locationInNode) then    		
    		self.m_StartPos = locationInNode
    		return true;
    	end
    	return false;
    end

    local function onTouchMoved(touch,event)    	
        local delta = touch:getDelta()  
        local deltax = delta.x
        local deltay = delta.y

    	--水平滑动
    	if deltax ~= 0 and math.abs(deltay) < 10  then
    		if deltax > 0 then --向右划动
                self.bMoveRight = true
                local oldp = self.m_ContentSprite:getPositionX()
                local newpos = oldp + deltax
                if newpos > 0 then
                    newpos = 0
                end
                self.m_ContentSprite:setPosition(cc.p(newpos,0))                 			
    		else --向左滑动 
                self.bMoveRight = false               
                local oldp = self.m_ContentSprite:getPositionX()
                local newpos = oldp + deltax
                if newpos < -160 then
                    newpos = -160
                end
                self.m_ContentSprite:setPosition(cc.p(newpos,0))
                self.m_btnDele:setVisible(true)
                self.m_btnDele:setEnabled(true)
    		end
    	end
    end

    local function onTouchEnded(touch, event)
        --水平滑动
        if self.bMoveRight == true then --向右划动
            if self.m_ContentSprite:getPositionX() ~= 0 then
                self.m_ContentSprite:setPosition(cc.p(0,0))
            end
            self.m_btnDele:setEnabled(false)  
            self.m_btnDele:setVisible(false)
            self.m_btnChat:setVisible(self.m_clipUserStatus:getString() == "在线")
            self.m_btnChat:setEnabled(true)
        elseif self.bMoveRight == false then
            if self.m_ContentSprite:getPositionX() ~= -160 then
                self.m_ContentSprite:setPosition(cc.p(-160,0))         
            end
            self.m_btnChat:setEnabled(false) 
            self.m_btnChat:setVisible(false)
        end
        self.bMoveRight = nil
    end

    self.m_Listener =  cc.EventListenerTouchOneByOne:create()
    --self.m_Listener:setSwallowTouches(true)
    self.m_Listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
    self.m_Listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED)
    self.m_Listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED)

    local dispacther = cc.Director:getInstance():getEventDispatcher()
    dispacther:addEventListenerWithSceneGraphPriority(self.m_Listener,  self.m_ContentSprite)
end

function FriendListCell:createCell(userInfo,index,contentView,superView)
    local defaultCellSize = self:getContentSize();

    local head = PopupInfoHead:createClipHead(userInfo, 80)
    head:setPosition(cc.p(90,58))
    head:setIsGamePop(false)
    --根据会员等级确定头像框
    head:enableHeadFrame(true)
    head:enableInfoPop(true, cc.p(420, 120), cc.p(0.5, 0.5))
    head:setName("head")
    contentView:addChild(head)

    local yPos = defaultCellSize.height * 0.7
    local nickName = ClipText:createClipText(cc.size(150,30), userInfo.szNickName, "fonts/round_body.ttf", 30)
    nickName:setName("nickName");
    nickName:setTextColor(cc.c4b(41,82,146,255));
    nickName:setAnchorPoint(cc.p(0,0.5))
    nickName:setPosition(cc.p(180,yPos))
    contentView:addChild(nickName)

    local start = cc.Sprite:create("Friend/tutiao2.png");
    start:setAnchorPoint(cc.p(0,0.5))
    start:setPosition(cc.p(340,yPos))
    contentView:addChild(start)

    local lv = cc.Sprite:create("Friend/biaoti2.png");
    lv:setAnchorPoint(cc.p(0,0.5))
    lv:setPosition(cc.p(385,yPos))
    contentView:addChild(lv)

    local lValue = cc.LabelAtlas:_create(tostring(userInfo.wGrowLevel),"Friend/shuzi1.png",16,20,string.byte("0"));
    lValue:setAnchorPoint(cc.p(0,0.5))
    lValue:setPosition(cc.p(420,yPos))
    lValue:setName("lValue")
    contentView:addChild(lValue)
     
    local chatBtn = ccui.Button:create("Friend/anniu9.png", "Friend/anniu9.png", "Friend/anniu9.png");
    chatBtn:setName("chatBtn");
    chatBtn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            --关闭通知
            NotifyMgr:getInstance():hideNotify(chatBtn, true)

            --标记在聊天界面
            if nil ~= self.m_Super.setIsChatLayer then
                self.m_Super:setIsChatLayer(true)
            end
            local chatuser = FriendMgr:getInstance():getFriendByID(chatBtn:getTag())
            if nil == chatuser then
                return
            end

            --聊天
            local FriendChatLayer =  appdf.getNodeByName(superView,"FriendChatLayer");
            FriendChatLayer:setVisible(true)
            --加載好友列表与聊天列表
            self.m_Super.m_chatListManager = FriendChatList:create(FriendChatLayer,chatuser.dwUserID, self.m_Super);
        end
    end);
    chatBtn:setAnchorPoint(cc.p(1,0.5));
    chatBtn:setPosition(cc.p(950,60));
    contentView:addChild(chatBtn);
    self.m_btnChat = chatBtn

    local userStatusLab = ClipText:createClipText(cc.size(350,25), "", "fonts/round_body.ttf", 25)
    userStatusLab:setName("userStatusLab");
    userStatusLab:setTextColor(cc.c4b(88,94,101,255));
    userStatusLab:setAnchorPoint(cc.p(0,0.5));
    userStatusLab:setPosition(cc.p(180,defaultCellSize.height * 0.3));
    contentView:addChild(userStatusLab)
    self.m_clipUserStatus = userStatusLab

    if userInfo.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then 
        --离线
        chatBtn:setVisible(false)
        chatBtn:setEnabled(false)
    else
        --在线
        chatBtn:setVisible(true)
        chatBtn:setEnabled(true)
    end
end

function FriendListCell:setCellStatusNormal()
	self.m_ContentSprite:setPosition(cc.p(0,0));

	self.m_btnChat:setVisible(true);
end

function FriendListCell:getChatBtn()
    return self.m_btnChat
end

function FriendListCell:refreshFriendState(userInfoTab)
    --好友状态
    if userInfoTab.cbMainStatus == chat_cmd.FRIEND_US_OFFLINE then
        --离线 
        self.m_btnChat:setVisible(false)
        self.m_btnChat:setEnabled(false)
        self.m_clipUserStatus:setString("离线")
    else
        --上线
        self.m_btnChat:setVisible(true)
        self.m_btnChat:setEnabled(true)

        --dump(userInfoTab, "userInfoTab")
        --游戏状态(游戏中)
        if userInfoTab.cbGameStatus >= yl.US_FREE then
            if nil ~= userInfoTab.szServerName then
                local str = ""
                if userInfoTab.cbGameStatus >= yl.US_SIT then
                    str = "在" .. userInfoTab.szServerName .. tostring(userInfoTab.wTableID) .."号桌"                
                else
                    str = "在" .. userInfoTab.szServerName
                end
                self.m_clipUserStatus:setString(str)
            end            
        else
            self.m_clipUserStatus:setString("在线")
        end   
    end
end

return FriendListCell