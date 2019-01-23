--
-- Author: zhong
-- Date: 2016-12-29 19:30:40
--
-- 好友分享列表
local FriendShareListLayer = class("FriendShareListLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local TAG_MASK = 101
local BTN_CANCELSHARE = 102
function FriendShareListLayer:ctor( callback )
    self.m_callback = callback
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Friend/FriendShareListLayer.csb", self)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setTouchEnabled(true)
    image_bg:setSwallowTouches(true)
    image_bg:setScale(0.00001)
    self.m_imageBg = image_bg

    local content = image_bg:getChildByName("content")
    local contentsize = content:getContentSize()
    self.m_fSix = contentsize.width / 6
    -- 列表
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)    
    self._listView:setPosition(content:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(image_bg)
    self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:reloadData()
    content:removeFromParent()    

    -- 取消分享
    local btn = image_bg:getChildByName("btn_cancelshare")
    btn:setTag(BTN_CANCELSHARE)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    image_bg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

function FriendShareListLayer:tableCellTouched(view, cell)
    
end

function FriendShareListLayer:cellSizeForTable(view, idx)
    return self.m_fSix, 250
end

function FriendShareListLayer:numberOfCellsInTableView(view)
    return #(FriendMgr:getInstance():getFriendList())
end

function FriendShareListLayer:tableCellAtIndex(view, idx)  
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    -- 好友数据
    local frienddata = FriendMgr:getInstance():getFriendList()[idx + 1]
    -- 头像
    local head = PopupInfoHead:createClipHead(frienddata, 100, "Friend/friendhd_clip.png")
    head:setPosition(cc.p(74,165))
    head:setIsGamePop(false)
    local str = ""
    if frienddata.cbMainStatus == 0 then-- chat_cmd.FRIEND_US_OFFLINE
        str = "离线"
        -- 灰度图
        if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
            convertToGraySprite(head.m_head.m_spRender)
        end
    elseif frienddata.cbMainStatus == 1 then
        str = "在线"
    end
    -- 在线
    local statusLab = cc.Label:createWithTTF(str, "fonts/round_body.ttf", 20)
            :move(74,40)    

    -- 底图
    local bg = ccui.ImageView:create("Friend/sp_friend_cellbg.png")
    bg:setTouchEnabled(true)
    bg:setSwallowTouches(false)
    bg:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local frienddata = FriendMgr:getInstance():getFriendList()[idx + 1]
            if type(self.m_callback) == "function" and nil ~= frienddata then
                if frienddata.cbMainStatus ~= 1 then
                    showToast(self, "好友已离线, 暂时无法发送数据", 2)
                    -- 灰度图
                    if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
                        convertToGraySprite(head.m_head.m_spRender)
                    end
                    statusLab:setString("离线")
                    return
                end
                if "离线" == statusLab:getString() then
                    statusLab:setString("在线")
                    if nil ~= head and nil ~= head.m_head and nil ~= head.m_head.m_spRender then
                        convertToNormalSprite(head.m_head.m_spRender)
                    end
                end
                self.m_callback(frienddata)
            end
        end
    end)
    bg:setPosition(self.m_fSix * 0.5, view:getViewSize().height * 0.5)
    cell:addChild(bg)
    bg:addChild(head)
    bg:addChild(statusLab)

    -- 昵称
    local nick = ClipText:createClipText(cc.size(120, 30), frienddata.szNickName)
    nick:setAnchorPoint(cc.p(0.5, 0.5))
    nick:setPosition(cc.p(74, 80))
    bg:addChild(nick)

    return cell
end

function FriendShareListLayer:onButtonClickedEvent(tag, ref)
    if TAG_MASK == tag then
        self:hide()
    elseif BTN_CANCELSHARE == tag then
        self:hide()
    end
end

function FriendShareListLayer:hide()
    local scale1 = cc.ScaleTo:create(0.2, 0.0001)
    local call1 = cc.CallFunc:create(function()
        self:removeFromParent()
    end)
    self.m_imageBg:runAction(cc.Sequence:create(scale1, call1))
end

return FriendShareListLayer