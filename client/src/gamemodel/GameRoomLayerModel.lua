--
-- Author: zhong
-- Date: 2016-10-12 09:19:00
--
local GameRoomLayerModel = class("GameRoomLayerModel")

--获取桌子参数(背景、椅子布局)
function GameRoomLayerModel:getTableParam()
    --第一个返回值为桌子背景(Sprite or ImageView)
    --第二个返回值为椅子布局位置，如 {cc.p(168,219),cc.p(168,-29)}, --2
    return nil, nil
end

--获取一页桌子显示数量(默认为3)
function GameRoomLayerModel:getShowCount()
    return 3
end

return GameRoomLayerModel