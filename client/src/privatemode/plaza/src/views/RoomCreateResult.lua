--
-- Author: zhong
-- Date: 2016-12-29 17:59:54
--
-- 房间创建结果
local RoomCreateResult = class("RoomCreateResult", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local TAG_MASK = 101
local BT_JOIN = 102
local BT_INVITE = 103
function RoomCreateResult:ctor( scene )
    self.scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/CreateRoomResult.csb", self)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setTouchEnabled(true)
    image_bg:setSwallowTouches(true)
    image_bg:setScale(0.00001)
    self.m_imageBg = image_bg

    -- 创建结果
    local tips = image_bg:getChildByName("txt_tips")
    tips:setString("房间 " .. PriRoom:getInstance().m_tabPriData.szServerID .. " 创建成功, 是否进入游戏?")

    -- 加入游戏
    local btn = image_bg:getChildByName("btn_join")
    btn:setTag(BT_JOIN)
    btn:addTouchEventListener( touchFunC )

    -- 邀请好友
    btn = image_bg:getChildByName("btn_invite")
    btn:setTag(BT_INVITE)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    image_bg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

function RoomCreateResult:onButtonClickedEvent( tag, sender )
    if TAG_MASK == tag then
        local scale1 = cc.ScaleTo:create(0.2, 0.0001)
        local call1 = cc.CallFunc:create(function()
            -- 切换场景
            self.scene:onChangeShowMode(PriRoom.LAYTAG.LAYER_MYROOMRECORD)
            -- 移除创建房间记录
            local idx = nil
            for k,v in pairs(self.scene._sceneRecord) do
                if v == PriRoom.LAYTAG.LAYER_CREATEPRIROOME then
                    idx = k
                    break
                end
            end
            if nil ~= idx then
                table.remove(self.scene._sceneRecord, idx)
            end

            self:removeFromParent()
        end)
        self.m_imageBg:runAction(cc.Sequence:create(scale1, call1))
    elseif BT_JOIN == tag then
        local scale1 = cc.ScaleTo:create(0.2, 0.0001)
        local call1 = cc.CallFunc:create(function()
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():getNetFrame():onSearchRoom(PriRoom:getInstance().m_tabPriData.szServerID)
            self:removeFromParent()
        end)
        self.m_imageBg:runAction(cc.Sequence:create(scale1, call1))        
    elseif BT_INVITE == tag then
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
            bMyFriend = bMyFriend or false
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享成功", 2)
                end
            end
            local m_tabDetail = {} 
            m_tabDetail.szRoomID = PriRoom:getInstance().m_tabPriData.szServerID
            m_tabDetail.dwPlayTurnCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            local msgTab = PriRoom:getInstance():getInviteShareMsg(m_tabDetail)
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            if bMyFriend then
                PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                    local serverid = tonumber(PriRoom:getInstance().m_tabPriData.szServerID) or 0
                    PriRoom:getInstance():priInviteFriend(frienddata, GlobalUserItem.nCurGameKind, serverid, yl.INVALID_TABLE, msgTab.friendContent)
                end)
            elseif nil ~= target then
                MultiPlatform:getInstance():shareToTarget(target, sharecall, msgTab.title, msgTab.content, url, "")
            end
        end)
    end
end

return RoomCreateResult