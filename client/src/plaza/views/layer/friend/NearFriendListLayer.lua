--
-- Author: zhong
-- Date: 2017-01-17 20:18:48
--
-- 附近好友列表
local NearFriendListLayer = class("NearFriendListLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local TAG_MASK = 101
local BTN_CLOSE = 102
function NearFriendListLayer:ctor( )
    -- 列表
    self.m_tabList = {}
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Friend/NearFriendListLayer.csb", self)

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

    -- 关闭按钮
    local btn = image_bg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    image_bg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

function NearFriendListLayer:tableCellTouched(view, cell)
    
end

function NearFriendListLayer:cellSizeForTable(view, idx)
    return self.m_fSix, 250
end

function NearFriendListLayer:numberOfCellsInTableView(view)
    return #self.m_tabList
end

function NearFriendListLayer:tableCellAtIndex(view, idx)  
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    -- 好友数据
    local frienddata = self.m_tabList[idx + 1]
    if nil == frienddata then
        return cell
    end  

    -- 底图
    local bg = ccui.ImageView:create("Friend/sp_friend_cellbg.png")
    bg:setTouchEnabled(true)
    bg:setSwallowTouches(false)
    bg:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local friendinfo = FriendMgr:getInstance():getFriendInfoByID(frienddata.dwUserID)
            if nil == friendinfo then
                local query = QueryDialog:create("是否发送添加好友申请?", function(ok)
                        if ok then
                            local addFriendTab = {}    
                            addFriendTab.dwUserID = GlobalUserItem.dwUserID
                            addFriendTab.dwFriendID = frienddata.dwUserID
                            addFriendTab.cbGroupID = 0
                            local function sendResult(isAction)
                                if isAction then
                                    showToast(self, "已发送好友申请", 2)
                                end     
                            end
                            --添加好友
                            FriendMgr:getInstance():sendAddFriend(addFriendTab,sendResult)
                        end
                    end)
                    :setCanTouchOutside(false)
                    :addTo(self)
            else
                showToast(self, "已经是好友", 2)
            end            
        end
    end)
    bg:setPosition(self.m_fSix * 0.5, view:getViewSize().height * 0.5)
    cell:addChild(bg)
    
    -- 头像
    local head = PopupInfoHead:createClipHead(frienddata, 100, "Friend/friendhd_clip.png")
    head:setPosition(cc.p(74,165))
    head:setIsGamePop(false)
    bg:addChild(head)

    -- 昵称
    local szNick = frienddata.szNickName or ""
    local nick = ClipText:createClipText(cc.size(120, 30), szNick)
    nick:setAnchorPoint(cc.p(0.5, 0.5))
    nick:setPosition(cc.p(74, 80))
    bg:addChild(nick)

    -- 距离
    local dis = frienddata.dwDistance or 0
    local dLongitude = frienddata.dLongitude
    local dLatitude = frienddata.dLatitude
    --[[local tab = {}
    tab.myLatitude = GlobalUserItem.tabCoordinate.la
    tab.myLongitude =  GlobalUserItem.tabCoordinate.lo
    tab.otherLatitude = dLatitude
    tab.otherLongitude =  dLongitude
    dump(MultiPlatform:getInstance():metersBetweenLocation(tab), "MultiPlatform:getInstance():metersBetweenLocation(tab)", 6)
    print("NearFriendListLayer ==> ", frienddata.dwDistance)]]
    if dLongitude ~= 0 and dLatitude ~= 0 then
        local szDis = ""
        local szUnit = ""
        if frienddata.dwDistance > 1000 then
            szDis = string.format("%.2f", frienddata.dwDistance / 1000)
            szUnit = "千米"
        else
            szDis = "" .. frienddata.dwDistance
            szUnit = "米"
        end

        -- 距离
        local clipdis = ClipText:createClipText(cc.size(120, 30), szDis)
        clipdis:setAnchorPoint(cc.p(1, 0.5))
        clipdis:setPosition(cc.p(96, 40))
        bg:addChild(clipdis)

        -- 单位
        local txt = cc.Label:createWithTTF(szUnit, "fonts/round_body.ttf", 20)
        bg:addChild(txt)
        txt:setAnchorPoint(cc.p(0, 0.5))
        txt:setPosition(96,40)
    end

    return cell
end

function NearFriendListLayer:onRefreshList( tablist )
    if type(tablist) == "table" then
        self.m_tabList = tablist
        self._listView:reloadData()
    end
end

function NearFriendListLayer:onButtonClickedEvent(tag, ref)
    if TAG_MASK == tag then
        self:hide()
    elseif BTN_CLOSE == tag then
        self:hide()
    end
end

function NearFriendListLayer:hide()
    local scale1 = cc.ScaleTo:create(0.2, 0.0001)
    local call1 = cc.CallFunc:create(function()
        self:removeFromParent()
    end)
    self.m_imageBg:runAction(cc.Sequence:create(scale1, call1))
end

return NearFriendListLayer