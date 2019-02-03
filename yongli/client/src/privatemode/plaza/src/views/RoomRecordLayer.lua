--
-- Author: zhong
-- Date: 2016-12-17 10:32:26
--
-- 房间记录界面

local RoomRecordLayer = class("RoomRecordLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local RoomDetailLayer = appdf.req(PriRoom.MODULE.PLAZAMODULE .. "views.RoomDetailLayer")
local cmd_private = appdf.req(PriRoom.MODULE.PRIHEADER .. "CMD_Private")

local ROOMDETAIL_NAME = "__pri_room_detail_layer_name__"
function RoomRecordLayer:ctor( scene )
    ExternalFun.registerNodeEvent(self)

    self.scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/RecordLayer.csb", self)

    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender,eventType)
    end
    -- 切换按钮
    self.m_checkSwitch = csbNode:getChildByName("check_switchrec")
    self.m_checkSwitch:setSelected(true)
    self.m_checkSwitch:addEventListener(cbtlistener)

    -- 创建记录
    self.m_layCreateRec = csbNode:getChildByName("lay_create")
    self.m_layCreateRec:setVisible(true)
    self.m_creCreateTime = self.m_layCreateRec:getChildByName("pri_sp_createtime")
    self.m_creRoomID = self.m_layCreateRec:getChildByName("pri_sp_roomid")
    self.m_creRoomLimit = self.m_layCreateRec:getChildByName("pri_sp_roomlimit")
    self.m_creCost = self.m_layCreateRec:getChildByName("pri_sp_createcost")
    self.m_creDisTime = self.m_layCreateRec:getChildByName("pri_sp_dissolvetime")
    self.m_creAward = self.m_layCreateRec:getChildByName("pri_sp_award")
    self.m_creStatus = self.m_layCreateRec:getChildByName("pri_sp_roomstatus")

    -- 参与记录
    self.m_layJoinRec = csbNode:getChildByName("lay_join")
    self.m_layJoinRec:setVisible(false)
    self.m_joinCreateTime = self.m_layJoinRec:getChildByName("pri_sp_createtime")
    self.m_joinRoomID = self.m_layJoinRec:getChildByName("pri_sp_roomid")
    self.m_joinCreateUser = self.m_layJoinRec:getChildByName("pri_sp_createuser")
    self.m_joinUinfo = self.m_layJoinRec:getChildByName("pri_sp_uinfo")
    self.m_joinDisTime = self.m_layJoinRec:getChildByName("pri_sp_dissolvetime")

    local content = csbNode:getChildByName("lay_content")
    -- 列表
    local m_tableView = cc.TableView:create(content:getContentSize())
    m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    m_tableView:setPosition(content:getPosition())
    m_tableView:setDelegate()
    m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    csbNode:addChild(m_tableView)
    self.m_tableView = m_tableView
    content:removeFromParent()

    -- 房间信息
    self.m_layRoomDetail = nil
end

function RoomRecordLayer:onSelectedEvent( tag,sender,eventType )
    local sel = sender:isSelected()
    self.m_layCreateRec:setVisible(sel)
    self.m_layJoinRec:setVisible(not sel)

    if not sel and 0 == #(PriRoom:getInstance().m_tabJoinRecord) then
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onQueryJoinList()
        return
    end
    self.m_tableView:reloadData()
end

function RoomRecordLayer:onEnterTransitionFinish()
    PriRoom:getInstance():showPopWait()
    -- 请求记录列表
    PriRoom:getInstance():getNetFrame():onQueryRoomList()
end

function RoomRecordLayer:onExit()
    -- 清除缓存
    PriRoom:getInstance().m_tabJoinRecord = {}
    PriRoom:getInstance().m_tabCreateRecord = {}
end

function RoomRecordLayer:onReloadRecordList()
    self.m_tableView:reloadData()
    local rd = self:getChildByName(ROOMDETAIL_NAME)
    if nil ~= rd then
        rd:hide()
    end
end

function RoomRecordLayer.cellSizeForTable( view, idx )
    return 1130,50
end

function RoomRecordLayer:numberOfCellsInTableView( view )
    if self.m_checkSwitch:isSelected() then
        return #(PriRoom:getInstance().m_tabCreateRecord)
    else
        return #(PriRoom:getInstance().m_tabJoinRecord)
    end
end

function RoomRecordLayer:tableCellAtIndex( view, idx )
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if self.m_checkSwitch:isSelected() then
        local tabData = PriRoom:getInstance().m_tabCreateRecord[idx + 1]
        local item = self:createRecordItem(tabData)
        item:setPosition(view:getViewSize().width * 0.5, 25)
        cell:addChild(item)
    else
        local tabData = PriRoom:getInstance().m_tabJoinRecord[idx + 1]
        local item = self:joinRecordItem(tabData)
        item:setPosition(view:getViewSize().width * 0.5, 25)
        cell:addChild(item)
    end

    return cell
end

-- 创建记录
function RoomRecordLayer:createRecordItem( tabData )
    --tabData = tagPersonalRoomInfo
    local item = ccui.Widget:create()
    item:setContentSize(cc.size(1130, 50))
    
    -- 线
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("pri_sp_listline.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        item:addChild(sp)
        sp:setPosition(565, 0)
    end
    
    -- 创建时间
    local tabTime = tabData.sysCreateTime
    local strTime = string.format("%d-%02d-%02d %02d:%02d:%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay, tabTime.wHour, tabTime.wMinute, tabTime.wSecond)
    local createtime = cc.Label:createWithTTF(strTime,"base/fonts/round_body.ttf",20)
    createtime:setTextColor(cc.c3b(244,237,182))
    item:addChild(createtime)
    createtime:setPosition(self.m_creCreateTime:getPositionX(), 25)

    -- 房间ID
    local roomid = cc.Label:createWithTTF(tabData.szRoomID,"base/fonts/round_body.ttf",20)
    roomid:setTextColor(cc.c3b(244,237,182))
    item:addChild(roomid)
    roomid:setPosition(self.m_creRoomID:getPositionX(), 25)

    -- 房间限制
    local roomlimit = cc.Label:createWithTTF(tabData.dwPlayTurnCount .. "","base/fonts/round_body.ttf",20)
    roomlimit:setTextColor(cc.c3b(244,237,182))
    item:addChild(roomlimit)
    roomlimit:setPosition(self.m_creRoomLimit:getPositionX(), 25)

    local feeType = "房卡"
    if tabData.cbCardOrBean == 0 then
        feeType = "游戏豆"
    end
    -- 创建消耗
    local cost = cc.Label:createWithTTF(tabData.lFeeCardOrBeanCount .. feeType,"base/fonts/round_body.ttf",20)
    cost:setTextColor(cc.c3b(244,237,182))
    item:addChild(cost)
    cost:setPosition(self.m_creCost:getPositionX(), 25)

    -- 奖励
    local award = cc.Label:createWithTTF(tabData.lScore .. "游戏币","base/fonts/round_body.ttf",20)
    award:setTextColor(cc.c3b(244,237,182))
    item:addChild(award)
    award:setPosition(self.m_creAward:getPositionX(), 25)

    -- 房间状态
    local bOnGame = false
    local status = cc.Label:createWithTTF("","base/fonts/round_body.ttf",20)
    if tabData.cbIsDisssumRoom == 1 then -- 解散
        status:setTextColor(cc.c3b(23,170,255))
        status:setString("已解散")
        tabTime = tabData.sysDissumeTime
        strTime = string.format("%d-%02d-%02d %02d:%02d:%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay, tabTime.wHour, tabTime.wMinute, tabTime.wSecond)
    else -- 游戏中
        status:setTextColor(cc.c3b(255,21,21))
        status:setString("游戏中")
        bOnGame = true
        strTime = ""
    end    
    item:addChild(status)
    status:setPosition(self.m_creStatus:getPositionX(), 25)

    -- 解散时间    
    local distime = cc.Label:createWithTTF(strTime,"base/fonts/round_body.ttf",20)
    distime:setTextColor(cc.c3b(244,237,182))
    item:addChild(distime)
    distime:setPosition(self.m_creDisTime:getPositionX(), 25)

    item:setTouchEnabled(true)
    item:setSwallowTouches(false)
    local itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local tabDetail = tabData
            tabDetail.onGame = bOnGame
            local rd = RoomDetailLayer:create(tabDetail)
            rd:setName(ROOMDETAIL_NAME)
            self:addChild(rd)
        end
    end
    item:addTouchEventListener( itemFunC )
    return item
end

-- 参与记录
function RoomRecordLayer:joinRecordItem( tabData )
    --tabData = tagQueryPersonalRoomUserScore
    local item = ccui.Widget:create()
    item:setContentSize(cc.size(1130, 50))

    -- 线
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("pri_sp_listline.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        item:addChild(sp)
        sp:setPosition(565, 0)
    end
    
    -- 创建时间
    local tabTime = tabData.sysCreateTime
    local strTime = string.format("%d-%02d-%02d %02d:%02d:%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay, tabTime.wHour, tabTime.wMinute, tabTime.wSecond)
    local createtime = cc.Label:createWithTTF(strTime,"base/fonts/round_body.ttf",20)
    createtime:setTextColor(cc.c3b(244,237,182))
    item:addChild(createtime)
    createtime:setPosition(self.m_joinCreateTime:getPositionX(), 25)

    -- 房间ID
    local roomid = cc.Label:createWithTTF(tabData.szRoomID,"base/fonts/round_body.ttf",20)
    roomid:setTextColor(cc.c3b(244,237,182))
    item:addChild(roomid)
    roomid:setPosition(self.m_joinRoomID:getPositionX(), 25)

    -- 创建玩家
    local createusr = ClipText:createClipText(cc.size(120, 30), tabData.szUserNicname, "base/fonts/round_body.ttf", 20)
    createusr:setAnchorPoint(cc.p(0.5, 0.5))
    createusr:setTextColor(cc.c4b(244,237,182,255))
    item:addChild(createusr)
    createusr:setPosition(self.m_joinCreateUser:getPositionX(), 25)

    local scorestr = "+" .. tabData.lScore
    if tabData.lScore < 0 then
        scorestr = "" .. tabData.lScore
    end
    if tabData.bFlagOnGame then
        scorestr = ""
    end
    -- 个人战绩
    local uinfo = ClipText:createClipText(cc.size(150, 30), scorestr, "base/fonts/round_body.ttf", 20)
    uinfo:setAnchorPoint(cc.p(0.5, 0.5))
    uinfo:setTextColor(cc.c4b(244,237,182,255))
    item:addChild(uinfo)
    uinfo:setPosition(self.m_joinUinfo:getPositionX(), 25)

    -- 解散时间
    tabTime = tabData.sysDissumeTime
    strTime = string.format("%d-%02d-%02d %02d:%02d:%02d", tabTime.wYear, tabTime.wMonth, tabTime.wDay, tabTime.wHour, tabTime.wMinute, tabTime.wSecond)
    local distime = cc.Label:createWithTTF(strTime,"base/fonts/round_body.ttf",20)
    distime:setTextColor(cc.c3b(244,237,182))
    if tabData.bFlagOnGame then
        distime:setString("游戏中")
        distime:setTextColor(cc.c3b(255,21,21))
    end
    item:addChild(distime)
    distime:setPosition(self.m_joinDisTime:getPositionX(), 25)

    item:setTouchEnabled(true)
    item:setSwallowTouches(false)
    local itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local tabDetail = tabData
            tabDetail.onGame = tabData.bFlagOnGame or false 
            local rd = RoomDetailLayer:create(tabDetail)
            rd:setName(ROOMDETAIL_NAME)
            self:addChild(rd)            
        end
    end
    item:addTouchEventListener( itemFunC )
    return item
end

return RoomRecordLayer