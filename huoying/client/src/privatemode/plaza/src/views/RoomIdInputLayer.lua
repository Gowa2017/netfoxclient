--
-- Author: zhong
-- Date: 2016-12-17 09:39:30
--
-- 私人房 ID 输入界面

local RoomIdInputLayer = class("RoomIdInputLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BTN_CLEAR = 1
local BTN_DEL = 2
function RoomIdInputLayer:ctor()
    -- 注册触摸事件
    ExternalFun.registerTouchEvent(self, true)

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/RoomIdInputLayer.csb", self)

    --
    self.m_spBg = csbNode:getChildByName("pri_sp_ideditbg")
    self.m_spBg:setScale(0.000001)
    local bg = self.m_spBg

    -- 房间ID
    self.m_atlasRoomId = bg:getChildByName("atlas_roomid")
    self.m_atlasRoomId:setString("")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onNumButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 数字按钮
    for i = 1, 10 do
        local tag = i - 1
        local btn = bg:getChildByName("btn_num" .. tag)
        btn:setTag(tag)
        btn:addTouchEventListener(btncallback)
    end

    local function callback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 清除按钮
    local btn = bg:getChildByName("btn_clear")
    btn:setTag(BTN_CLEAR)
    btn:addTouchEventListener(callback)

    -- 删除按钮
    btn = bg:getChildByName("btn_del")
    btn:setTag(BTN_DEL)
    btn:addTouchEventListener(callback)

    -- 加载动画
    local call = cc.CallFunc:create(function()
        self:setVisible(true)
    end)
    local scale = cc.ScaleTo:create(0.2, 1.3)
    self.m_actShowAct = cc.Sequence:create(call, scale)
    ExternalFun.SAFE_RETAIN(self.m_actShowAct)

    local scale1 = cc.ScaleTo:create(0.2, 0.0001)
    local call1 = cc.CallFunc:create(function( )
        self:setVisible(false)
    end)
    self.m_actHideAct = cc.Sequence:create(scale1, call1)
    ExternalFun.SAFE_RETAIN(self.m_actHideAct)

    self:setVisible(false)
end

function RoomIdInputLayer:showLayer( var )
    local ani = nil
    if var then
        ani = self.m_actShowAct

        local copyId = string.gsub(GlobalUserItem.szCopyRoomId,"([^0-9])","")
        if "" ~= copyId and 6 == string.len(copyId) then
            self.m_atlasRoomId:setString(copyId)
            PriRoom:getInstance():showPopWait()
            self:runAction(cc.Sequence:create(cc.DelayTime:create(1.5), cc.CallFunc:create(function()
                self:showLayer(false)
                PriRoom:getInstance():getNetFrame():onSearchRoom(copyId)
                self.m_atlasRoomId:setString("")
                GlobalUserItem.szCopyRoomId = ""
            end)))
        else
            self.m_atlasRoomId:setString("")
        end
    else 
        ani = self.m_actHideAct
    end

    if nil ~= ani then
        self.m_spBg:stopAllActions()
        self.m_spBg:runAction(ani)
    end
end

function RoomIdInputLayer:onExit()
    ExternalFun.SAFE_RELEASE(self.m_actShowAct)
    self.m_actShowAct = nil
    ExternalFun.SAFE_RELEASE(self.m_actHideAct)
    self.m_actHideAct = nil
end

function RoomIdInputLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function RoomIdInputLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:showLayer(false)
        local roomid = self.m_atlasRoomId:getString()
        if string.len(roomid) == 6 then
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():getNetFrame():onSearchRoom(roomid)
            self.m_atlasRoomId:setString("")
        end
    end
end

function RoomIdInputLayer:onNumButtonClickedEvent( tag, sender )
    local roomid = self.m_atlasRoomId:getString()
    if string.len(roomid) < 6 then
        roomid = roomid .. tag
        self.m_atlasRoomId:setString(roomid)
    end

    if string.len(roomid) == 6 then        
        PriRoom:getInstance():showPopWait()
        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function()
            self:showLayer(false)
            PriRoom:getInstance():getNetFrame():onSearchRoom(roomid)
            self.m_atlasRoomId:setString("")
            GlobalUserItem.szCopyRoomId = ""
        end)))
    end
end

function RoomIdInputLayer:onButtonClickedEvent( tag, sender )
    if BTN_CLEAR == tag then
        self.m_atlasRoomId:setString("")
    elseif BTN_DEL == tag then
        local roomid = self.m_atlasRoomId:getString()
        local len = string.len(roomid)
        if len > 0 then
            roomid = string.sub(roomid, 1, len - 1)
        end
        self.m_atlasRoomId:setString(roomid)        
    end
end

return RoomIdInputLayer