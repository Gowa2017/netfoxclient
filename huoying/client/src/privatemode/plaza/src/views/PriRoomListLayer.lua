--
-- Author: zhong
-- Date: 2016-12-21 15:21:11
--
-- 私人房模式 房间列表
local PriRoomListLayer = class("PriRoomListLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BTN_NORMAL_ROOMLIST   = 101               -- 普通房间列表
local BTN_JOIN_PRIROOM      = 102               -- 加入房间
local BTN_CREATE_PRIROOM    = 103               -- 创建房间
function PriRoomListLayer:ctor( scene )
    ExternalFun.registerNodeEvent(self)
    GlobalUserItem.nCurRoomIndex = -1

    self._scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PriRoomListLayer.csb", self)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    -- 普通房间列表
    local btn = csbNode:getChildByName("btn_roomlist")
    btn:setTag(BTN_NORMAL_ROOMLIST)
    btn:addTouchEventListener(touchFunC)

    -- 加入房间
    btn = csbNode:getChildByName("btn_joinroom")
    btn:setTag(BTN_JOIN_PRIROOM)
    btn:addTouchEventListener(touchFunC)

    -- 创建房间
    btn = csbNode:getChildByName("btn_createroom")
    btn:setTag(BTN_CREATE_PRIROOM)
    btn:addTouchEventListener(touchFunC)

    self._scene:showPopWait()
    -- 请求私人房配置
    PriRoom:getInstance():getNetFrame():onGetRoomParameter()
end

function PriRoomListLayer:onButtonClickedEvent( tag, sender )
    if BTN_NORMAL_ROOMLIST == tag then
        -- 重置搜索路径
        PriRoom:getInstance():exitRoom()
        self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
    elseif BTN_JOIN_PRIROOM == tag then
        PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_ROOMID)
    elseif BTN_CREATE_PRIROOM == tag then
        self._scene:onChangeShowMode(PriRoom.LAYTAG.LAYER_CREATEPRIROOME)        
    end
end

function PriRoomListLayer:onEnterTransitionFinish()
end

return PriRoomListLayer