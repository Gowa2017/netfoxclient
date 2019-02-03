--[[
  好友界面
  2016_06_16 Ravioyla
]]
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local FriendLayer = class("FriendLayer", function(scene)
    local FriendLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return FriendLayer
end)

local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local FriendChatList = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.FriendChatList")
local FriendListCell = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.FriendListCell")
local QueryDialog = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local NearFriendListLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.NearFriendListLayer")

local BT_FRIENDLIST         = 101
local BT_ADDFRIEND          = 102
local BT_MESSAGENOTIRY      = 103
local NEAR_FRIEND_LIST = "__near_friend_list_layer__"

-- 进入场景而且过渡动画结束时候触发。
function FriendLayer:onEnterTransitionFinish()
    --判断好友系统网络状态
    if false == FriendMgr:getInstance():isConnected() then
        FriendMgr:getInstance():reSetAndLogin()
    end
    self:initFriendListener()

    --激活通知
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_CHAT_NOTIFY, "friend_chat")
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY, "friend_apply")
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_RESPOND_NOTIFY, "friend_response")
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_GAME_NOTIFY, "friend_invite")
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_SHARE_NOTIFY, "friend_share")
    NotifyMgr:getInstance():resumeNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY, "pri_friend_invite")
    --处理通知
    NotifyMgr:getInstance():excuteSleepNotfiy()

    --判断是否有未读消息通知(针对消息通知界面)
    local tmp = FriendMgr:getInstance():getUnReadNotify()
    if #tmp > 0 then
        NotifyMgr:getInstance():excute(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY, nil)
    end
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function FriendLayer:onExitTransitionStart()
    --暂停通知
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_CHAT_NOTIFY, "friend_chat")
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY, "friend_apply")
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_RESPOND_NOTIFY, "friend_response")
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_GAME_NOTIFY, "friend_invite")
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_SHARE_NOTIFY, "friend_share")
    NotifyMgr:getInstance():pauseNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFYpri_friend_invite, "pri_friend_invite")
    FriendMgr:getInstance():setViewLayer(nil)
    --FriendMgr:getInstance():setCallBackDelegate(nil);
    self:removeFriendListener()    
    return self
end

function FriendLayer:onNotify(msg)
    local bHandled = false
    if msg.main == chat_cmd.MDM_GC_USER then
        if msg.sub == chat_cmd.SUB_GC_USER_CHAT_NOTIFY 
            or msg.sub == chat_cmd.SUB_GC_INVITE_GAME_NOTIFY
            or msg.sub == chat_cmd.SUB_GC_USER_SHARE_NOTIFY
            or msg.sub == chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY then
            --dump(msg, "|", 6)
            --确保不在好友列表界面
            if self.m_nSelect ~= BT_FRIENDLIST then
                NotifyMgr:getInstance():showNotify(self.m_btnFriendList, msg, cc.p(210, 90))
            end 
            
            --单独解析 dwSenderID 为发送者id
            if type(msg.param) == "table" then
                local sendUser = msg.param.dwSenderID
                if nil == sendUser then
                    return false
                end
                if false == self.m_bIsChatLayer then
                    --不在聊天界面
                    bHandled = false
                else
                    --是否当前聊天对象
                    bHandled = (sendUser == self.m_dwCurrentUserId)
                end
                if false == bHandled then
                    local friendcell = self:getFriendListCell(sendUser)
                    if nil ~= friendcell then
                        local chatbtn = friendcell:getChatBtn()
                        NotifyMgr:getInstance():showNotify(chatbtn, msg)
                    end
                end

                --聊天界面
                if true == self.m_bIsChatLayer then
                    --非当前聊天对象
                    local node = self:getChatFriendCellNotifyNode(sendUser)
                    if nil ~= node then
                        NotifyMgr:getInstance():showNotify(node, msg)
                    end
                end                
            end                     --用户聊天
        elseif msg.sub == chat_cmd.SUB_GC_APPLYFOR_NOTIFY           --申请好友通知
        or msg.sub == chat_cmd.SUB_GC_RESPOND_NOTIFY                --回应通知
        --or msg.sub == chat_cmd.SUB_GC_INVITE_GAME_NOTIFY            --邀请通知
        then            
            --非通知界面
            if self.m_nSelect ~= BT_MESSAGENOTIRY then
                NotifyMgr:getInstance():showNotify(self.m_btnMsgNotice, msg,cc.p(210, 90))
            end 
            bHandled = true
        end        
    end
    return bHandled
end

function FriendLayer:ctor(scene)
    ExternalFun.registerNodeEvent(self)
    FriendMgr:getInstance():setViewLayer(self)

    --是否在聊天界面
    self.m_bIsChatLayer = false
    --当前聊天对象
    self.m_dwCurrentUserId = -1
    --好友列表按钮
    self.m_btnFriendList = nil
    --消息通知按钮
    self.m_btnMsgNotice = nil

    local this = self
    self._scene = scene
    self.m_TableData = {};
    self.m_nFriendCount = 0
    self.m_chatListManager = nil;
    self.m_listener = nil
    self.m_downListener = nil
    self.m_nSelect = BT_FRIENDLIST
  
    self:registerScriptHandler(function(eventType)
        if eventType == "enterTransitionFinish" then  -- 进入场景而且过渡动画结束时候触发。
            self:onEnterTransitionFinish()
        elseif eventType == "exitTransitionStart" then  -- 退出场景而且开始过渡动画时候触发。
            self:onExitTransitionStart()
        end
    end)

    self.Layer = cc.CSLoader:createNode("Friend/FriendLayer.csb");

    self:addChild(self.Layer);
    self:initWithButton();
    self:initFriendList();
end

--初始化按钮
function FriendLayer:initWithButton()
    --按钮回调
    self._btcallback = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local FriendBg = self.Layer:getChildByName("FriendBg");
    self.m_friendBg = FriendBg
    local FriendAddLayer = self.Layer:getChildByName("FriendAddLayer");
    self.m_friendAddLayer = FriendAddLayer
    local FriendChatLayer = self.Layer:getChildByName("FriendChatLayer");
    self.m_friendChatLayer = FriendChatLayer
    local FriendNotifyLayer = appdf.getNodeByName(self.Layer,"FriendNotifyLayer");
    self.m_friendNotifyLayer = FriendNotifyLayer
    local ListUserSearch = appdf.getNodeByName(FriendAddLayer,"ListUserSearch");
    ListUserSearch:setScrollBarEnabled(false)

    FriendAddLayer:setTouchEnabled(true);
    FriendChatLayer:setTouchEnabled(true);
    FriendNotifyLayer:setTouchEnabled(true);
    FriendBg:setTouchEnabled(true);

    -- 返回按钮
    local BcakBtn = appdf.getNodeByName(FriendBg,"BackBtn");
    BcakBtn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then            
            if FriendChatLayer:isVisible() then
                FriendChatLayer:setVisible(false);

                --重置当前保存信息
                self:setIsChatLayer(false)
                self:setCurrentChatUser(-1)

                if self.m_chatListManager then
                    self.m_chatListManager:removeTableView();
                    self.m_chatListManager = nil;
                end
            else                    
                self._scene:onKeyBack();
            end
        end
    end);

    -- 好友列表
    local BtnList = appdf.getNodeByName(FriendBg,"BtnList");
    BtnList:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self.m_nSelect = BT_FRIENDLIST
            NotifyMgr:getInstance():hideNotify(self.m_btnFriendList)

            FriendAddLayer:setVisible(false);  
            ref:setEnabled(false);
            FriendNotifyLayer:setVisible(false);
            local  BtnAddFriend = appdf.getNodeByName(FriendBg,"BtnAddFriend");
            BtnAddFriend:setEnabled(true);    
            local  BtnMsgNotice = appdf.getNodeByName(FriendBg,"BtnMsgNotice");
            BtnMsgNotice:setEnabled(true);  

            if nil ~= self.m_friendUserList then
                self.m_friendUserList:setTouchEnabled(true)
                self.m_friendUserList:setVisible(true)
            end
            self:refreshFriendList()
        end
    end);
    self.m_btnFriendList = BtnList

    -- 添加好友
    local BtnAddFriend = appdf.getNodeByName(FriendBg,"BtnAddFriend");
    self.m_btnAddFriend = BtnAddFriend
    BtnAddFriend:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            ListUserSearch:removeAllItems()
            self:onClickAddFriend()
        end
    end);

    -- 消息通知
    local BtnMsgNotice = appdf.getNodeByName(FriendBg,"BtnMsgNotice");
    BtnMsgNotice:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self.m_nSelect = BT_MESSAGENOTIRY
            if nil ~= self.m_friendUserList then
                self.m_friendUserList:setTouchEnabled(false)
                self.m_friendUserList:setVisible(false)
            end

            NotifyMgr:getInstance():hideNotify(self.m_btnMsgNotice, true)

            FriendAddLayer:setVisible(false); 
            ref:setEnabled(false);
            FriendNotifyLayer:setVisible(true);
            local BtnList = appdf.getNodeByName(FriendBg,"BtnList");
            BtnList:setEnabled(true);     
            local BtnAddFriend = appdf.getNodeByName(FriendBg,"BtnAddFriend");
            BtnAddFriend:setEnabled(true);
            self:updateNotifyList();
        end
    end);
    self.m_btnMsgNotice = BtnMsgNotice

    -- 查找按钮
    local searchBtn = appdf.getNodeByName(FriendAddLayer,"BtnSearch")
    searchBtn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then 
            if nil == self.m_editId then
                showToast(self, "输入不能为空", 2)
            end

            local content = tonumber(self.m_editId:getText())

            if nil == content then
                showToast(self, "请输入合法的ID", 2)
                return
            end

            if string.len(content)<1 then
                showToast(self, "输入不能为空", 2)
                return
            else
                FriendMgr:getInstance():sendSearchFriend(content)                
            end      
        end
    end);
    BtnList:setEnabled(false)
    FriendAddLayer:setVisible(false)

    -- 附近的人
    local btn = FriendAddLayer:getChildByName("BtnNearBy")
    btn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then 
            FriendMgr:getInstance():queryNearUser()
            local near = NearFriendListLayer:create()
            self:addChild(near)
            near:setName(NEAR_FRIEND_LIST)
        end
    end)

    -- 通讯录
    btn = FriendAddLayer:getChildByName("BtnTelList")
    btn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then 
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享完成", 2)
                end
            end
            local url = GlobalUserItem.szSpreaderURL or yl.HTTP_URL
            local msg = "亲爱的好友，我最近玩了一款超好玩的游戏，玩法超级多，内容超级精彩，快来加入我，和我一起精彩游戏吧！下载地址：" .. url
            MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.SMS, sharecall, "好友分享", msg)
        end
    end)

    -- 微信
    btn = FriendAddLayer:getChildByName("BtnWechat")
    btn:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享完成", 2)
                end
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.WECHAT, sharecall, "有人@你一起玩游戏！", "你的好友正在玩游戏！玩法超多超精彩！快来打败他！", url, "")
        end
    end)
end

--初始化好友列表
function FriendLayer:initFriendList()
    local userListBg = appdf.getNodeByName(self.Layer,"UserListBg");
    local userListBgSize = userListBg:getContentSize();
    userListBg:removeAllChildren();
    self.m_TableData = FriendMgr:getInstance():getFriendList();  
    self.m_nFriendCount = #self.m_TableData

    local tableView = cc.TableView:create(cc.size(userListBgSize.width-40, userListBgSize.height-50));
    tableView:setName("FriendUserList")
    tableView:setColor(cc.c3b(158, 200, 200))
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(20,30));
    self.m_friendUserList = tableView

    tableView:setDelegate()
    userListBg:addChild(tableView)
    --tableView:registerScriptHandler(FriendLayer.scrollViewDidScroll,cc.SCROLLVIEW_SCRIPT_SCROLL)
    --tableView:registerScriptHandler(FriendLayer.scrollViewDidZoom,cc.SCROLLVIEW_SCRIPT_ZOOM)
    tableView:registerScriptHandler(handler(self,self.tableCellTouched),cc.TABLECELL_TOUCHED)
    tableView:registerScriptHandler(handler(self,self.cellSizeForTable),cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(handler(self,self.tableCellAtIndex),cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(handler(self,self.numberOfCellsInTableView),cc.NUMBER_OF_CELLS_IN_TABLEVIEW);
    
    self:refreshFriendList()
end

function FriendLayer:initFriendListener( )
    self.m_listener = cc.EventListenerCustom:create(yl.RY_FRIEND_NOTIFY,handler(self, self.onFriendInfo))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    local function eventCustomListener(event)
        if nil ~= event.filename and 0 == event.code then
            if type(event.filename) == "string" and cc.FileUtils:getInstance():isFileExist(event.filenam) then
                print("刷新图片")
                if nil ~= self.m_chatListManager and nil ~= self.m_chatListManager.messageNotify then
                    self.m_chatListManager:messageNotify()
                end
            end
        end
    end
    self.m_downListener = cc.EventListenerCustom:create(yl.RY_IMAGE_DOWNLOAD_NOTIFY, eventCustomListener)
    self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_downListener, 1)
end

function FriendLayer:onClickAddFriend()
    self.m_nSelect = BT_ADDFRIEND
    if nil ~= self.m_friendUserList then
        self.m_friendUserList:setTouchEnabled(false)
        self.m_friendUserList:setVisible(false)
    end

    self.m_friendAddLayer:setVisible(true); 
    self.m_btnAddFriend:setEnabled(false);
    self.m_friendNotifyLayer:setVisible(false);

    local BtnList = appdf.getNodeByName(self.m_friendBg,"BtnList");
    BtnList:setEnabled(true);    
    local  BtnMsgNotice = appdf.getNodeByName(self.m_friendBg,"BtnMsgNotice");
    BtnMsgNotice:setEnabled(true);   

    local InputTextBg = appdf.getNodeByName(self.m_friendAddLayer,"InputTextBg");
    if not appdf.getNodeByName(self.m_friendAddLayer,"EditBoxSearchIput") then
        local EditID = cc.EditBox:create(InputTextBg:getContentSize(),"Friend/dikuang20.png");
        EditID:setFontSize(30);
        EditID:setPlaceholderFontSize(30)
        EditID:setFontColor(cc.c3b(206, 211, 255));
        EditID:setPlaceHolder("请填写对方的游戏ID号:");
        EditID:setFontName("fonts/round_body.ttf")
        EditID:setPlaceholderFontName("fonts/round_body.ttf")
        EditID:setPlaceholderFontColor(cc.c3b(206, 211, 255));
        EditID:setMaxLength(32);
        EditID:setReturnType(cc.KEYBOARD_RETURNTYPE_DONE);
        EditID:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        EditID:setPosition(InputTextBg:getPosition());
        EditID:setName("EditBoxSearchIput");
        self.m_friendAddLayer:addChild(EditID);
        self.m_editId = EditID
     end
    InputTextBg:setVisible(false);
end

function FriendLayer:onFriendInfo( event )
    local msgWhat = event.obj

    if nil ~= msgWhat and yl.RY_MSG_FRIENDDEL == msgWhat then
        --删除好友
        self:refreshFriendList()
    end
end

function FriendLayer:removeFriendListener( )
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end

    if nil ~= self.m_downListener then
        self:getEventDispatcher():removeEventListener(self.m_downListener)
        self.m_downListener = nil
    end
end

--获取好友界面聊天按钮
function FriendLayer:getFriendListCell(userid)
    if nil == userid or -1 == userid then
        return nil
    end

    local container = self.m_friendUserList:getContainer()
    if nil ~= container then
        local cell = container:getChildByTag(userid)
        if nil ~= cell then
            return cell:getChildByName("friendCell")
        end
    end
    return nil
end

--获取好友界面好友cell
function FriendLayer:getChatFriendCell(dwUserID)
    if nil == dwUserID or -1 == dwUserID then
        return nil
    end
    if nil ~= self.m_chatListManager and nil ~= self.m_chatListManager.getFriendCell then
        return self.m_chatListManager:getFriendCell(dwUserID)
    end
    return nil
end

--获取好友界面好友红点显示node
function FriendLayer:getChatFriendCellNotifyNode(dwUserID, ingoreSelf)
    --当前聊天对象
    if dwUserID == self.m_dwCurrentUserId then
        return nil
    end
    if nil ~= self.m_chatListManager and nil ~= self.m_chatListManager.getNotifyNode then
        return self.m_chatListManager:getNotifyNode(dwUserID)
    end
    return nil
end

function FriendLayer:refreshFriendList()    
    self.m_TableData = FriendMgr:getInstance():getFriendList();
    self.m_nFriendCount = #self.m_TableData
    local userListBg = appdf.getNodeByName(self.Layer,"UserListBg");
    local tableView = appdf.getNodeByName(userListBg,"FriendUserList");
    tableView:reloadData();

    userListBg:removeChildByName("tips_name")
    if 0 == self.m_nFriendCount then
        local conSize = userListBg:getContentSize()
        local lay = ccui.Layout:create()
        lay:setContentSize(conSize)
        userListBg:addChild(lay)
        lay:setName("tips_name")
        local sp = cc.Sprite:create("Friend/sp_nofriend.png")
        sp:setPosition(conSize.width * 0.5, conSize.height * 0.5 + 80)
        lay:addChild(sp)
        local tipLab = cc.Label:createWithTTF("您还没有好友!","fonts/round_body.ttf",30)
        tipLab:setPosition(cc.p(conSize.width/2,conSize.height * 0.5 - 30))
        lay:addChild(tipLab)
        ccui.Button:create("Friend/anniu14.png","Friend/anniu15.png")
        :move(conSize.width * 0.5,conSize.height * 0.5 - 130)
        :addTo(lay)
        :addTouchEventListener(function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    self:onClickAddFriend()
                end
            end)
    end

    if self.m_chatListManager then
        self.m_chatListManager:refreshChatList()
    end
end

--刷新指定好友游戏状态
function FriendLayer:refreshFriendState(userInfo, isGameState)
    isGameState = isGameState or false
    --刷新好友列表
    local friendCell = self:getFriendListCell(userInfo.dwUserID)
    if nil ~= friendCell and nil ~= friendCell.refreshFriendState then
        friendCell:refreshFriendState(userInfo)
    end

    if false == isGameState then
        --刷新聊天列表
        if nil ~= self.m_chatListManager and nil ~= self.m_chatListManager.refreshFriendState then
            return self.m_chatListManager:refreshFriendState(userInfo)
        end
    end    
end

function FriendLayer:messageNotify(notify)
    if nil ~= self.m_chatListManager and nil ~= self.m_chatListManager.messageNotify then
        self.m_chatListManager:messageNotify(notify)
    end
end

function FriendLayer:updateNotifyList()
    local notifyTab = FriendMgr:getInstance():getFriendNotify()
    dump(notifyTab,"==========当前数据==========")
    local FriendNotifyLayer = appdf.getNodeByName(self.Layer,"FriendNotifyLayer")
    local NotifyList = appdf.getNodeByName(FriendNotifyLayer,"NotifyList")
    local NotifyListSize  = NotifyList:getContentSize()
    NotifyList:removeAllItems()

    if #notifyTab ==0 then
        NotifyList:setVisible(false)
        local tipLab = cc.Label:createWithTTF("暂无消息通知","fonts/round_body.ttf",30)
        tipLab:setPosition(cc.p(NotifyListSize.width * 0.5,NotifyListSize.height * 0.5))
        FriendNotifyLayer:addChild(tipLab)
        local function removeTipLab()
            tipLab:removeFromParent()
        end 
        tipLab:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(removeTipLab)))
    else
        NotifyList:setVisible(true)
        for i=#notifyTab,1,-1 do  
            local curTab = notifyTab[i]

            local notifyItemBase = ccui.ImageView:create("Friend/dikuang5.png")--ccui.Layout:create()
            notifyItemBase:setScale9Enabled(true)

            local deleBtn = ccui.Button:create("Friend/btn_del_msg.png","Friend/btn_del_msg.png","Friend/btn_del_msg.png");
            deleBtn:addTouchEventListener(function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    print("==============删除消息============== " .. curTab.notifyId)
                    FriendMgr:getInstance():deleteFriendNotify( curTab.notifyId )
                    self:updateNotifyList()
                end
            end)
            deleBtn:setAnchorPoint(cc.p(1,0.5))
            deleBtn:setPosition(cc.p(notifyItemBase:getContentSize().width,notifyItemBase:getContentSize().height * 0.5))
            deleBtn:setEnabled(false)
            notifyItemBase:addChild(deleBtn)

            local notifyItem = ccui.ImageView:create("Friend/dikuang5.png")--ccui.Layout:create()
            notifyItem:setScale9Enabled(true)
            notifyItem:setAnchorPoint(cc.p(0, 0))
            notifyItem:setPosition(0, 0)
            notifyItemBase:addChild(notifyItem)
            notifyItem:setContentSize(NotifyListSize.width,120)            

            --  1:申请好友通知  2:回应通知  3：邀请通知 
            if curTab.notifyType == 1 then            
                local tip = cc.Label:createWithTTF(curTab.notify.szNickName.."请求成为您的好友","fonts/round_body.ttf",30,cc.size(600,0),cc.TEXT_ALIGNMENT_LEFT)
                tip:setTextColor(cc.c4b(41,82,146,255))
                tip:setAnchorPoint(cc.p(0,0.5))
                tip:setPosition(cc.p(20,notifyItem:getContentSize().height/2))
                notifyItem:addChild(tip)

                if not curTab.bRead then
                    local sureBtn = ccui.Button:create("Friend/anniu21.png","Friend/anniu22.png");
                    sureBtn:setAnchorPoint(cc.p(0,0.5));
                    sureBtn:setPosition(cc.p(650,notifyItem:getContentSize().height/2));
                    notifyItem:addChild(sureBtn);
                    sureBtn:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                            local sendTab = {}
                            sendTab.dwUserID = GlobalUserItem.dwUserID
                            sendTab.dwRequestID = curTab.notify.dwRequestID
                            sendTab.bAccepted = true

                            FriendMgr:getInstance():sendRespondFriend(sendTab,curTab.notifyId)
                        end
                    end)
                    local cancleBtn = ccui.Button:create("Friend/anniu23.png");
                    cancleBtn:setAnchorPoint(cc.p(0,0.5));
                    cancleBtn:setPosition(cc.p(800,notifyItem:getContentSize().height/2));
                    notifyItem:addChild(cancleBtn);
                    cancleBtn:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                            local sendTab = {}
                            sendTab.dwUserID = GlobalUserItem.dwUserID
                            sendTab.dwRequestID = curTab.notify.dwRequestID
                            sendTab.bAccepted = false

                            FriendMgr:getInstance():sendRespondFriend(sendTab,curTab.notifyId)     
                        end
                    end)
                end                
            elseif curTab.notifyType  == 2 then
                local tip = cc.Label:createWithTTF(curTab.notify,"fonts/round_body.ttf",30,cc.size(600,0),cc.TEXT_ALIGNMENT_LEFT)
                tip:setTextColor(cc.c4b(41,82,146,255))
                tip:setAnchorPoint(cc.p(0,0.5))
                tip:setPosition(cc.p(20,notifyItem:getContentSize().height/2))
                notifyItem:addChild(tip)
                if not curTab.bRead then
                    notifyItem:setTouchEnabled(true)
                    notifyItem:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                            print("read" .. i)
                            FriendMgr:getInstance():markFriendNotifyRead(curTab.notifyId)
                            ref:removeChildByName("notify_dot_cell")
                            self:updateNotifyList()
                        end
                    end)
                end                
            elseif curTab.notifyType  == 3 then
                local str = tostring(curTab.notify.dwInviteUserID).."在"..tostring(curTab.notify.wServerID).."房间"..tostring(curTab.notify.wTableID).."号桌"..curTab.notify.szInviteMsg
                local tip = cc.Label:createWithTTF(str,"fonts/round_body.ttf",30,cc.size(600,0),cc.TEXT_ALIGNMENT_LEFT)
                tip:setTextColor(cc.c4b(41,82,146,255))
                tip:setAnchorPoint(cc.p(0,0.5))
                tip:setPosition(cc.p(20,notifyItem:getContentSize().height/2))                
                notifyItem:addChild(tip)

                if not curTab.bRead then
                    local sureBtn = ccui.Button:create("Friend/anniu21.png","Friend/anniu22.png")
                    sureBtn:setAnchorPoint(cc.p(0,0.5))
                    sureBtn:setTag(i+400)
                    sureBtn:setPosition(cc.p(650,notifyItem:getContentSize().height/2))
                    notifyItem:addChild(sureBtn)
                    sureBtn:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                            local notifyList = FriendMgr:getInstance():getFriendNotify();
                            local index = sureBtn:getTag() - 400;
                            print("同意进入某房间==========")
                        end
                    end)

                    local cancleBtn = ccui.Button:create("Friend/anniu23.png");
                    cancleBtn:setAnchorPoint(cc.p(0,0.5));
                    cancleBtn:setTag(i+800);
                    cancleBtn:setPosition(cc.p(800,notifyItem:getContentSize().height/2));
                    notifyItem:addChild(cancleBtn);
                    cancleBtn:addTouchEventListener(function(ref, tType)
                        if tType == ccui.TouchEventType.ended then
                           print("不同意进入某房间==========")
                        end
                    end)
                end                
            end

            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_dot.png")
            if nil ~= frame and not curTab.bRead then
                dot = cc.Sprite:createWithSpriteFrame(frame)
                dot:setName("notify_dot_cell")
                notifyItem:addChild(dot)

                local nodesize = notifyItem:getContentSize()
                local dotsize = dot:getContentSize()
                local pos = cc.p(nodesize.width - dotsize.width * 0.5, nodesize.height - dotsize.height * 0.5 )
                dot:setPosition(pos)            
            end

            local function onTouchBegan(touch,event)        
                local locationInNode = notifyItem:convertToNodeSpace(touch:getLocation())
                local size = notifyItem:getContentSize()
                local rect = cc.rect(0,0,size.width,size.height)
                if cc.rectContainsPoint(rect,locationInNode) then
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
                        notifyItem.bMoveRight = true
                        local oldp = notifyItem:getPositionX()
                        local newpos = oldp + deltax
                        if newpos > 0 then
                            newpos = 0
                        end
                        notifyItem:setPosition(cc.p(newpos,0))                            
                    else --向左滑动 
                        notifyItem.bMoveRight = false               
                        local oldp = notifyItem:getPositionX()
                        local newpos = oldp + deltax
                        if newpos < -160 then
                            newpos = -160
                        end
                        notifyItem:setPosition(cc.p(newpos,0))
                        deleBtn:setVisible(true)
                        deleBtn:setEnabled(true)
                    end
                end
            end

            local function onTouchEnded(touch, event)
                --水平滑动
                if notifyItem.bMoveRight == true then --向右划动
                    if notifyItem:getPositionX() ~= 0 then
                        notifyItem:setPosition(cc.p(0,0))
                    end
                    deleBtn:setEnabled(false)  
                    deleBtn:setVisible(false)
                elseif notifyItem.bMoveRight == false then
                    if notifyItem:getPositionX() ~= -160 then
                        notifyItem:setPosition(cc.p(-160,0))         
                    end
                    deleBtn:setEnabled(true) 
                    deleBtn:setVisible(true)
                end
                notifyItem.bMoveRight = nil
            end

            local m_Listener = cc.EventListenerTouchOneByOne:create()
            --self.m_Listener:setSwallowTouches(true)
            m_Listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN)
            m_Listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED)
            m_Listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED)

            local dispacther = cc.Director:getInstance():getEventDispatcher()
            dispacther:addEventListenerWithSceneGraphPriority(m_Listener,  notifyItem)

            local function onNodeEvent( event )
                if event == "exit" then
                    print("node itme remove")
                    local eventDispatcher = notifyItem:getEventDispatcher()
                    eventDispatcher:removeEventListener(m_Listener)
                end
            end
            notifyItem:registerScriptHandler(onNodeEvent)

            NotifyList:pushBackCustomItem(notifyItemBase)
        end
    end
end

function FriendLayer:onDeleteFriend()
    local eventListener = cc.EventCustom:new(yl.RY_FRIEND_NOTIFY)
    eventListener.obj = yl.RY_MSG_FRIENDDEL
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function FriendLayer:searchResult(userTab)
    local FriendAddLayer = appdf.getNodeByName(self.Layer,"FriendAddLayer");
    local ListUserSearch = appdf.getNodeByName(FriendAddLayer,"ListUserSearch");
    local ListUserSearchSize  = ListUserSearch:getContentSize();
    if #userTab ==0 then
        local tipLab = cc.Label:createWithTTF("找不到该用户!!!","fonts/round_body.ttf",30);
        tipLab:setPosition(cc.p(ListUserSearchSize.width/2,ListUserSearchSize.height/2));
        ListUserSearch:addChild(tipLab);
        tipLab:setTextColor(cc.c4b(41, 82, 146, 255));
        local function removeTipLab()
            tipLab:removeFromParent();
        end 
        tipLab:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(removeTipLab)));
    else
        if nil ~= self.m_editId then
            self.m_editId:setText("")
        end
        ListUserSearch:removeAllItems();

        for i=1,#userTab do   
            local userInfoTab = userTab[i];
            local userItem = ccui.ImageView:create("Friend/dikuang5.png")--ccui.Layout:create();
            userItem:setScale9Enabled(true)
            userItem:setContentSize(ListUserSearchSize.width,120);
            local head = PopupInfoHead:createClipHead(userInfoTab, 80)
            head:setPosition(cc.p(100,60))
            userItem:addChild(head)
            head:setIsGamePop(false)
            --根据会员等级确定头像框
            head:enableHeadFrame(true)
            --head:enableInfoPop(true, cc.p(420, 120), cc.p(0.5, 0.5))

            local yPos = 60
            local nickName = ClipText:createClipText(cc.size(150,30), userInfoTab.szNickName, "fonts/round_body.ttf", 30)
            nickName:setName("nickName");
            nickName:setTextColor(cc.c4b(41,82,146,255));
            nickName:setAnchorPoint(cc.p(0,0.5))
            nickName:setPosition(cc.p(200,yPos + 20))
            userItem:addChild(nickName)

            local userid = ClipText:createClipText(cc.size(150,30), "ID:" .. userInfoTab.dwGameID, "fonts/round_body.ttf", 30)
            userid:setName("userid");
            userid:setTextColor(cc.c4b(41,82,146,255));
            userid:setAnchorPoint(cc.p(0,0.5))
            userid:setPosition(cc.p(200,yPos - 20))
            userItem:addChild(userid)

            local start = cc.Sprite:create("Friend/tutiao2.png");
            start:setAnchorPoint(cc.p(0,0.5))
            start:setPosition(cc.p(360,yPos))
            userItem:addChild(start)

            local lv = cc.Sprite:create("Friend/biaoti2.png");
            lv:setAnchorPoint(cc.p(0,0.5))
            lv:setPosition(cc.p(405,yPos))
            userItem:addChild(lv)

            local lValue = cc.LabelAtlas:_create(tostring(userInfoTab.cbMemberOrder),"Friend/shuzi1.png",16,20,string.byte("0"));
            lValue:setAnchorPoint(cc.p(0,0.5))
            lValue:setPosition(cc.p(440,yPos))
            lValue:setName("lValue")
            userItem:addChild(lValue)        
  
            local friendTab = FriendMgr:getInstance():getFriendList();
            --查询好友列表是否存在该用户
            local userIsExit = false;
            for i=1,#friendTab do
                local curUser = friendTab[i];
                if curUser.dwUserID == userInfoTab.dwUserID then
                    --好友列表存在该好友
                    userIsExit = true
                    break
                end               
            end
            --判断是否是自己
            local isMe = userInfoTab.dwUserID == GlobalUserItem.dwUserID

            local actionBtn = nil
            if true == isMe then

            elseif userIsExit then
                actionBtn = ccui.Button:create("Friend/anniu20.png");
            else
                actionBtn = ccui.Button:create("Friend/anniu14.png","Friend/anniu15.png","Friend/anniu15_1.png");
                actionBtn:setTag(i+1000);
                actionBtn:addTouchEventListener(function(ref, tType)
                    if tType == ccui.TouchEventType.ended then
                        local addFriendTab = {}    
                        addFriendTab.dwUserID = GlobalUserItem.dwUserID;
                        addFriendTab.dwFriendID = userTab[ref:getTag()-1000].dwUserID;
                        addFriendTab.cbGroupID = 0;
                        local function sendResult(isAction)
                            --ref:setVisible(not isAction)
                            ref:setEnabled(false)
                        end
                        --添加好友
                        FriendMgr:getInstance():sendAddFriend(addFriendTab,sendResult);
                    end
                end);
            end
            if nil ~= actionBtn then
                actionBtn:setAnchorPoint(cc.p(1,0.5));
                actionBtn:setPosition(cc.p(900,60));
                userItem:addChild(actionBtn);
            end
            
            ListUserSearch:pushBackCustomItem(userItem);  
        end     
    end
end

function FriendLayer:tableCellTouched(table,cell)
    print("cell touched at index: " .. cell:getIdx())
end

function FriendLayer:cellSizeForTable(table,idx) 
    return 962,125
end

function FriendLayer:tableCellAtIndex(table, idx)
    local cell = table:dequeueCell()
    local userInfoTab = self.m_TableData[self.m_nFriendCount - idx]

    local friendCell = nil
    --获取未读通知列表
    local unread = NotifyMgr:getInstance():getUnreadNotifyList()
    if nil == cell then
        cell = cc.TableViewCell:create();
        friendCell = FriendListCell:create(userInfoTab.dwUserID,self);
        friendCell:setName("friendCell");
        cell:addChild(friendCell);
    else
        friendCell = cell:getChildByName("friendCell")
        local head = appdf.getNodeByName(cell,"head")
        local nickName = appdf.getNodeByName(cell,"nickName")        
        local lValue = appdf.getNodeByName(cell,"lValue")
        
        lValue:setString(tostring(userInfoTab.wGrowLevel));     
        nickName:setString(userInfoTab.szNickName);
        head:updateHead(userInfoTab);
    end

    if nil ~= friendCell then
        friendCell:setCellStatusNormal()
        local chatbtn = friendCell:getChatBtn()
        if nil ~= chatbtn then
            --隐藏红点
            NotifyMgr:getInstance():hideNotify(chatbtn, false)

            --通知红点
            for k,v in pairs(unread) do
                if type(v.param) == "table" then
                    local sendUser = v.param.dwSenderID
                    if nil ~= sendUser and sendUser == userInfoTab.dwUserID then                
                        NotifyMgr:getInstance():showNotify(chatbtn, v)
                        break
                    end
                end
            end
            chatbtn:setTag(userInfoTab.dwUserID)
        end
        
        local userStatusLab = appdf.getNodeByName(cell,"userStatusLab")

        --好友状态
        friendCell:refreshFriendState(userInfoTab)
    end

    cell:setTag(userInfoTab.dwUserID)
    return cell
end

function FriendLayer:numberOfCellsInTableView(table)
   return self.m_nFriendCount
end

function FriendLayer:setIsChatLayer(var)
    self.m_bIsChatLayer = var
end

function FriendLayer:setCurrentChatUser(dwUserID)
    print("current user ==> " .. dwUserID)
    --关闭通知
    local friendcell = self:getFriendListCell(dwUserID)
    if nil ~= friendcell then
        local chatbtn = friendcell:getChatBtn()
        NotifyMgr:getInstance():hideNotify(chatbtn, true)
    end
    self.m_dwCurrentUserId = dwUserID
end

function FriendLayer:refreshNearFriendList( tablist )
    local near = self:getChildByName(NEAR_FRIEND_LIST)
    if nil ~= near and nil ~= near.onRefreshList then
        near:onRefreshList(tablist)
    end
end

function FriendLayer:onKeyBack()
    return not self.m_bIsChatLayer
end

return FriendLayer